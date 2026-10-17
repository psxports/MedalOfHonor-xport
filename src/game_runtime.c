#include "game_draft_signatures.h"
#include "game_shell_signatures.h"
#include "game_level_signatures.h"
#include "xport_trace.h"
#include <windows.h>
#include "psx.h"
#include "psx_spu.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

const uint32 xport_cd_ready_callback_address = 0x800D0A78u;
const uint32 xport_cd_sync_callback_address = 0x800D0A74u;
const uint32 xport_spu_register_pointer_address = 0x800302BCu;
const uint32 xport_cd_status_address = 0x800345ECu;
/* TODO CdControlB is unavailable for this LIBDS image and must not be dispatched */
const uint32 xport_cd_setloc_table_address = 0u;

static PsxIrqState moh_irq;
static uint32 moh_dma_priority;
static uint32 moh_irq_active;
static uint32 moh_irq_environment;
static uint32 (*moh_irq_handler)(void);
static uint32 moh_dma_pending;
static uint32 moh_files_ready;
static sint32 moh_file_read_status;
static uint32 moh_file_read_deadline;
static uint32 moh_overlay_kind;
static uint32 moh_initialized_screens[64];

uint32 moh_native_music_enabled(void)
{
    const char *setting = getenv("MOH_NATIVE_MUSIC");
    /* TODO Connect native ADPCM streaming before enabling music by default */
    return setting && strcmp(setting, "1") == 0;
}

uint32 moh_native_screen_initialized(uint32 screen)
{
    uint32 index;
    for (index = 0u; index < 64u; ++index)
        if (moh_initialized_screens[index] == screen) return 1u;
    return 0u;
}

void moh_native_note_screen_initialized(uint32 screen)
{
    uint32 index;
    if (moh_native_screen_initialized(screen)) return;
    for (index = 0u; index < 64u; ++index)
        if (!moh_initialized_screens[index]) { moh_initialized_screens[index] = screen; return; }
    abort();
}

void moh_native_forget_screen(uint32 screen)
{
    uint32 index;
    for (index = 0u; index < 64u; ++index)
        if (moh_initialized_screens[index] == screen) moh_initialized_screens[index] = 0u;
}

static sint32 moh_card_callback(void *context, uint32 address)
{
    if (address < 0x80078CD0u || address > 0x80078D5Cu || (address - 0x80078CD0u) % 20u)
        return 0;
    w_u32(0x8008F938u + ((address - 0x80078CD0u) / 20u) * 4u, 1u);
    return 1;
}

__declspec(noreturn) static void moh_runtime_error(const char *operation, uint32 address)
{
    char message[256];
    sprintf(message, "%s at 0x%08X", operation, address);
    OutputDebugStringA(message);
    MessageBoxA(NULL, message, "Medal Of Honor: incomplete native runtime", MB_OK | MB_ICONERROR);
    abort();
}

void moh_draft_fail_fast(const char *operation, uint32 address)
{
    moh_runtime_error(operation, address);
}

void xport_bind_native_spu_transfer(void)
{
    SpuNativeTransferGuestBinding binding;
    binding.requested_mode = 0x8003035Cu;
    binding.normalized_mode = 0x800302D8u;
    binding.address_units = 0x800302D4u;
    binding.completion = 0x800302F0u;
    binding.completion_callback = 0x800302F4u;
    binding.source_address = 0x80030310u;
    binding.dma_blocks = 0x80030314u;
    binding.address_shift = r_u32(0x800302E4u);
    if (!spu_bind_native_transfer_guest(&binding)) moh_runtime_error("Invalid native SPU transfer binding", 0u);
}

static uint32 *moh_mmio_word(uint32 address)
{
    if (address == 0x1F801070u) return &moh_irq.status;
    if (address == 0x1F801074u) return &moh_irq.mask;
    if (address == 0x1F8010F0u) return &moh_dma_priority;
    moh_runtime_error("Unsupported MMIO register", address);
}

static sint32 moh_mmio_read(void *user, uint32 address, uint32 width, uint32 *value)
{
    uint32 shift = (address & 3u) * 8u;
    uint32 mask;
    if ((width != 1u && width != 2u && width != 4u) || (address & 3u) + width > 4u)
        moh_runtime_error("Invalid MMIO read width", address);
    mask = width == 4u ? ~0u : (1u << (width * 8u)) - 1u;
    *value = (*moh_mmio_word(address & ~3u) >> shift) & mask;
    return 1;
}

static sint32 moh_mmio_write(void *user, uint32 address, uint32 width, uint32 value, uint32 byte_mask)
{
    uint32 base = address & ~3u;
    uint32 *word = moh_mmio_word(base);
    uint32 merged = *word, index, shift;
    if ((width != 1u && width != 2u && width != 4u) || (address & 3u) + width > 4u)
        moh_runtime_error("Invalid MMIO write width", address);
    for (index = 0u; index < width; ++index)
    {
        if (!(byte_mask & (1u << index))) continue;
        shift = ((address & 3u) + index) * 8u;
        merged = (merged & ~(255u << shift)) | (((value >> (index * 8u)) & 255u) << shift);
    }
    if (base == 0x1F801070u || base == 0x1F801074u)
        return psx_irq_write(&moh_irq, base - 0x1F801070u, merged);
    *word = merged;
    return 1;
}

static void moh_irq_frame(void)
{
    psx_irq_line(&moh_irq, 0u, 1u);
    psx_irq_line(&moh_irq, 0u, 0u);
    if (moh_irq_handler && psx_irq_pending(&moh_irq))
    {
        if (moh_irq_active) moh_runtime_error("Nested native IRQ", moh_irq_environment);
        moh_irq_active = 1u;
        moh_irq_handler();
        moh_irq_active = 0u;
    }
}

void moh_native_irq_prepare(uint32 environment, uint32 (*handler)(void))
{
    if (moh_irq_active || !handler || !xport_guest_ptr(environment, 48u))
        moh_runtime_error("Invalid native IRQ continuation", environment);
    moh_irq_environment = environment;
    moh_irq_handler = handler;
}

static void moh_native_irq_install(uint32 environment)
{
    if (!moh_irq_handler || environment != moh_irq_environment)
        moh_runtime_error("Unprepared native IRQ continuation", environment);
    VSyncCallback(moh_irq_frame);
}

void moh_load_executable(void)
{
    char path[MAX_PATH];
    char *separator;
    FILE *file;
    uint8 header[2048];
    uint32 address, bytes;
    void *destination;
    DWORD length = GetModuleFileNameA(NULL, path, sizeof(path));
    if (!length || length >= sizeof(path)) moh_runtime_error("Executable path unavailable", 0u);
    separator = strrchr(path, '\\');
    if (!separator || (size_t)(separator - path) + 25u >= sizeof(path))
        moh_runtime_error("Executable path too long", 0u);
    strcpy(separator + 1, "..\\orig\\SLUS_009.74");
    file = fopen(path, "rb");
    if (!file) moh_runtime_error("Cannot open SLUS_009.74", 0u);
    if (fread(header, 1u, sizeof(header), file) != sizeof(header) || memcmp(header, "PS-X EXE", 8u))
    {
        fclose(file);
        moh_runtime_error("Invalid PS-X EXE header", 0u);
    }
    address = xport_load_le32(header + 0x18u);
    bytes = xport_load_le32(header + 0x1Cu);
    if (xport_load_le32(header + 0x10u) != 0x8001DFD4u)
    {
        fclose(file);
        moh_runtime_error("Unsupported PS-X EXE entry", xport_load_le32(header + 0x10u));
    }
    destination = xport_guest_ptr(address, bytes);
    if (!destination || !bytes || fread(destination, 1u, bytes, file) != bytes)
    {
        fclose(file);
        moh_runtime_error("Invalid PS-X EXE payload", address);
    }
    fclose(file);
    psx_bios_bind_guest_callback_service(moh_card_callback, NULL);
    xport_memory_bind_mmio(0x1F801000u, 0x1000u, moh_mmio_read, moh_mmio_write, NULL);
}

void *moh_draft_guest_pointer(uint32 address, uint32 bytes)
{
    void *pointer = xport_guest_ptr(address, bytes);
    if (!pointer) moh_runtime_error("Unmapped guest pointer", address);
    return pointer;
}

uint32 moh_draft_local_address(const void *buffer, uint32 bytes)
{
    XportMemoryRegion region;
    uint32 offset;
    if (xport_memory_pointer_identity(buffer, bytes, &region, &offset) && region == XPORT_MEMORY_DRAM)
        return 0x80000000u + offset;
    /* TODO Preserve local buffer lifetime and copy-back through a guest-address adapter */
    moh_runtime_error("Missing local buffer adapter", 0u);
    return 0u;
}

uint32 moh_native_irq_return(void)
{
    if (!moh_irq_active) moh_runtime_error("ReturnFromException outside native IRQ", 0x800269A8u);
    return 0u;
}

static uint32 moh_dispatch(uint32 address, va_list arguments);

void moh_native_dma_complete(uint32 channel)
{
    if (channel >= 7u) moh_runtime_error("Invalid native DMA channel", channel);
    moh_dma_pending |= 1u << channel;
    moh_draft_indirect_call(0x80026754u);
}

uint32 moh_native_reset_graph(uint32 mode)
{
    uint32 subtype = mode & 7u;
    sint32 result = ResetGraph(subtype == 3u ? 0 : (sint32)mode);
    if (subtype == 0u || subtype == 3u || subtype == 5u)
    {
        sub_800191F8(0x8002DF18u, 0u, 128u);
        /* The native GPU uses the standard 1024 by 512 VRAM layout */
        w_u8(0x8002DF19u, 1u);
        w_u16(0x8002DF1Cu, 1024u);
        w_u16(0x8002DF1Eu, 512u);
        sub_800191F8(0x8002DF28u, 0xFFFFFFFFu, 92u);
        sub_800191F8(0x8002DF84u, 0xFFFFFFFFu, 20u);
    }
    return (uint32)result;
}

static FILE *moh_open_asset(uint32 filename)
{
    char name[128], path[MAX_PATH];
    char *base;
    uint32 index, input = filename;
    DWORD length;
    while (r_u8(input) == '\\' || r_u8(input) == '/') ++input;
    for (index = 0u; index < sizeof(name) - 1u; ++index)
    {
        uint8 byte = r_u8(input++);
        if (!byte || byte == ';') break;
        name[index] = byte == '/' ? '\\' : (char)byte;
    }
    name[index] = 0;
    if (!index || strstr(name, "..")) moh_runtime_error("Invalid asset name", filename);
    length = GetModuleFileNameA(NULL, path, sizeof(path));
    if (!length || length >= sizeof(path)) moh_runtime_error("Asset path unavailable", filename);
    base = strrchr(path, '\\');
    if (!base || (size_t)(base - path) + strlen(name) + 16u >= sizeof(path))
        moh_runtime_error("Asset path too long", filename);
    sprintf(base + 1, "DATA\\%s", name);
    {
        FILE *file = fopen(path, "rb");
        if (file) return file;
    }
    if (strchr(name, '\\')) return NULL;
    sprintf(base + 1, "..\\orig\\%s", name);
    return fopen(path, "rb");
}

uint32 moh_native_find_asset(uint32 result, uint32 filename)
{
    FILE *file;
    long bytes;
    uint32 lba;
    if (!moh_files_ready) moh_runtime_error("Native files are not initialized", filename);
    file = moh_open_asset(filename);
    if (!file) moh_runtime_error("Native indexed asset unavailable", filename);
    if (fseek(file, 0, SEEK_END) || (bytes = ftell(file)) < 0) {
        fclose(file);
        moh_runtime_error("Cannot determine indexed asset size", filename);
    }
    fclose(file);
    lba = moh_native_asset_lba((const char *)moh_draft_guest_pointer(filename, 1u));
    if (!lba) moh_runtime_error("Asset missing from original disc index", filename);
    w_u32(result, lba);
    w_u32(result + 4u, (uint32)bytes);
    return result;
}

static size_t moh_native_read(FILE *file, void *buffer, uint32 bytes)
{
    size_t received;
    moh_file_read_status = (sint32)((bytes + 2047u) >> 11);
    moh_file_read_deadline = (uint32)VSync(-1) + 1200u;
    received = fread(buffer, 1u, bytes, file);
    moh_file_read_status = received == bytes ? 0 : -1;
    return received;
}

static sint32 moh_native_cd_read_status(uint32 result)
{
    sint32 status = moh_file_read_status;
    uint32 pending = 0u;
    if (status > 0 && (sint32)moh_file_read_deadline < (sint32)VSync(-1)) {
        moh_file_read_status = -1;
        status = -1;
    }
    if (r_u32(0x800CBB34u) == 1u) pending = 0x800CBB34u;
    else if (r_u32(0x800CBB24u) == 1u) pending = 0x800CBB24u;
    if (pending) {
        uint32 index;
        w_u32(pending, 0u);
        if (result) {
            for (index = 0u; index < 8u; ++index)
                w_u8(result + index, r_u8(pending + 5u + index));
        }
    }
    return status;
}

static uint32 moh_read_asset(uint32 filename, uint32 destination, uint32 *size)
{
    FILE *file;
    long bytes;
    void *buffer;
    if (!moh_files_ready) moh_runtime_error("Native files are not initialized", filename);
    moh_file_read_status = -1;
    file = moh_open_asset(filename);
    if (!file) { *size = 0u; return 0u; }
    if (fseek(file, 0, SEEK_END) || (bytes = ftell(file)) < 0 || fseek(file, 0, SEEK_SET))
    {
        fclose(file);
        moh_runtime_error("Cannot determine asset size", filename);
    }
    *size = (uint32)bytes;
    if (!destination) destination = sub_80036BF0(*size + 1u);
    if (!destination) { fclose(file); return 0u; }
    buffer = moh_draft_guest_pointer(destination, *size + 1u);
    if (moh_native_read(file, buffer, *size) != *size)
    {
        fclose(file);
        moh_runtime_error("Short native asset read", filename);
    }
    fclose(file);
    w_u8(destination + *size, 0u);
    w_u32(0x80038590u, destination);
    w_u32(0x80038594u, *size >> 11);
    w_u32(0x80038598u, 0u);
    return destination;
}

uint32 moh_native_load_file(uint32 filename, uint32 size_address, uint32 destination)
{
    uint32 size = 0u;
    uint32 result = moh_read_asset(filename, destination, &size);
    if (size_address) w_u32(size_address, size);
    return result;
}

uint32 moh_native_load_resource(uint32 filename, uint32 destination, uint32 capacity)
{
    FILE *file = moh_open_asset(filename);
    uint32 size, rounded;
    long length;
    void *buffer;
    if (!moh_files_ready) moh_runtime_error("Native files are not initialized", filename);
    moh_file_read_status = -1;
    if (!file) moh_runtime_error("Cannot open resource file", filename);
    if (fseek(file, 0, SEEK_END) || (length = ftell(file)) < 0 || fseek(file, 0, SEEK_SET))
    {
        fclose(file);
        moh_runtime_error("Cannot determine resource size", filename);
    }
    size = (uint32)length;
    if (size > 0x7FFFF000u) { fclose(file); moh_runtime_error("Resource file too large", filename); }
    rounded = ((size + 2047u) & ~2047u) + 1u;
    if (destination && (rounded > capacity || rounded < 2u))
    {
        fclose(file);
        return 0u;
    }
    if (!destination) destination = sub_80036BF0(rounded);
    if (!destination) { fclose(file); return 0u; }
    buffer = moh_draft_guest_pointer(destination, rounded);
    if (moh_native_read(file, buffer, size) != size)
    {
        fclose(file);
        moh_runtime_error("Short resource file read", filename);
    }
    fclose(file);
    w_u8(destination, 0u);
    return destination;
}

sint32 moh_native_load_overlay(uint32 filename, uint32 destination)
{
    uint32 size = 0u;
    uint32 index;
    char name[128];
    if (!moh_read_asset(filename, destination, &size)) return -1;
    for (index = 0u; index < sizeof(name) - 1u; ++index) {
        name[index] = (char)r_u8(filename + index);
        if (!name[index]) break;
    }
    name[index] = 0;
    if (strstr(name, "LEVEL2P")) moh_overlay_kind = 2u;
    else if (strstr(name, "LEVEL")) moh_overlay_kind = 1u;
    else if (strstr(name, "SHELL")) moh_overlay_kind = 0u;
    else moh_runtime_error("Unknown native overlay", filename);
    return 0;
}

uint32 moh_native_cd_reset(void)
{
    w_u32(0x80038598u, 0u);
    return moh_files_ready;
}

static uint32 moh_native_load_bsd(uint32 filename, uint32 count_out)
{
    FILE *file;
    uint32 header, asset, selected, bytes;
    size_t received;
    w_u32(count_out, 0u);
    if (!moh_files_ready) moh_runtime_error("Native files are not initialized", filename);
    moh_file_read_status = -1;
    file = moh_open_asset(filename);
    if (!file) moh_runtime_error("BSD asset unavailable", filename);
    header = sub_80036BF0(2048u);
    if (!header) { fclose(file); return 0u; }
    if (moh_native_read(file, moh_draft_guest_pointer(header, 2048u), 2048u) != 2048u) {
        fclose(file); sub_80036E44(header);
        moh_runtime_error("Short BSD header read", filename);
    }
    if (r_s32(header) >= 512) {
        fclose(file); sub_80036E44(header);
        moh_runtime_error("Invalid BSD variant count", filename);
    }
    asset = level_sub_8004A300(header, count_out);
    if (!asset) { moh_file_read_status = -1; fclose(file); sub_80036E44(header); return 0u; }
    selected = r_u32(count_out);
    /* Read the real payload sectors without reproducing the CD controller */
    bytes = r_u32(header + selected * 4u) & ~2047u;
    received = moh_native_read(file, moh_draft_guest_pointer(asset, bytes), bytes);
    /* Extracted assets omit the unused tail of their final CD sector */
    if (received < bytes && feof(file) && !ferror(file) && bytes - received < 2048u) {
        memset((uint8 *)moh_draft_guest_pointer(asset, bytes) + received, 0, bytes - received);
        moh_file_read_status = 0;
    } else if (received != bytes) {
        fclose(file); sub_80036E44(header); sub_80036E44(asset);
        moh_runtime_error("Short BSD payload read", filename);
    }
    fclose(file);
    sub_80036E44(header);
    return asset;
}

uint32 moh_draft_missing_call(uint32 address, ...)
{
    uint32 result;
    va_list arguments;
    /* Missing adapters must not re-enter their translated wrapper */
    if (address == 0x800266BCu)
        moh_runtime_error("Missing native continuation adapter", address);
    if (address == 0x8001AD38u || address == 0x8001AF88u)
        moh_runtime_error("Missing native BIOS adapter", address);
    va_start(arguments, address);
    result = moh_dispatch(address, arguments);
    va_end(arguments);
    return result;
}

uint32 moh_draft_indirect_call(uint32 target, ...)
{
    uint32 result;
    va_list arguments;
    va_start(arguments, target);
    result = moh_dispatch(target, arguments);
    va_end(arguments);
    return result;
}

static uint32 moh_dispatch(uint32 address, va_list arguments)
{
    uint32 a[18], i;
    if (moh_overlay_kind && address >= 0x80039D90u && address < 0x80200000u) {
        if (moh_overlay_kind != 1u) moh_runtime_error("Untranslated LEVEL2P call", address);
        switch (address) {
        case 0x80064374u: return level_sub_80064374();
        case 0x8004A634u: level_sub_8004A634(); return 0u;
        case 0x80061598u: level_sub_80061598(); return 0u;
        case 0x8004943Cu: return level_sub_8004943C();
        case 0x800640B4u: level_sub_800640B4(); return 0u;
        case 0x8008B3D4u: return level_sub_8008B3D4();
        case 0x8006B894u: level_sub_8006B894(); return 0u;
        case 0x80090D38u: level_sub_80090D38(); return 0u;
        case 0x8006240Cu: level_sub_8006240C(); return 0u;
        case 0x80061E14u: level_sub_80061E14(); return 0u;
        case 0x800616D4u: level_sub_800616D4(); return 0u;
        case 0x800625BCu: return level_sub_800625BC();
        case 0x8003EC04u: level_sub_8003EC04(); return 0u;
        case 0x8005898Cu:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            return level_sub_8005898C(a[0], a[1]);
        case 0x800563F0u:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_800563F0(a[0], a[1]);
        case 0x80045A78u:
            for (i = 0u; i < 14u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80045A78(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11], a[12], a[13]);
        case 0x8006D670u:
            return level_sub_8006D670();
        case 0x8008EEF0u:
            return level_sub_8008EEF0();
        case 0x8008ED30u:
            return level_sub_8008ED30();
        case 0x8005A574u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8005A574(a[0], a[1], a[2]);
        case 0x8005BC74u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8005BC74(a[0], a[1], a[2]);
        case 0x8005BD70u:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8005BD70(a[0], a[1]);
        case 0x8005BF94u:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8005BF94(a[0], a[1]);
        case 0x80055628u:
            a[0] = va_arg(arguments, uint32);
            level_sub_80055628(a[0]); return 0u;
        case 0x80055528u:
            a[0] = va_arg(arguments, uint32);
            level_sub_80055528(a[0]); return 0u;
        case 0x8005163Cu:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8005163C(a[0], a[1], a[2], a[3]); return 0u;
        case 0x80055198u:
            a[0] = va_arg(arguments, uint32);
            level_sub_80055198(a[0]); return 0u;
        case 0x80052AF4u:
            a[0] = va_arg(arguments, uint32);
            level_sub_80052AF4(a[0]); return 0u;
        case 0x8005BF2Cu:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8005BF2C(a[0], a[1], a[2]);
        case 0x8005AC00u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8005AC00(a[0], a[1], a[2]);
        case 0x800599E4u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_800599E4(a[0], a[1], a[2]);
        case 0x80072704u:
            a[0] = va_arg(arguments, uint32);
            return level_sub_80072704(a[0]);
        case 0x800730D4u:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_800730D4(a[0], a[1]);
        case 0x800563E8u:
            level_sub_800563E8(); return 0u;
        case 0x80040DF4u:
            a[0] = va_arg(arguments, uint32);
            level_sub_80040DF4(a[0]); return 0u;
        case 0x80040A0Cu:
            a[0] = va_arg(arguments, uint32);
            level_sub_80040A0C(a[0]); return 0u;
        case 0x80040180u:
            for (i = 0u; i < 5u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_80040180(a[0], a[1], a[2], a[3], a[4]); return 0u;
        case 0x8004CBC8u:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8004CBC8(a[0], a[1], a[2], a[3]);
        case 0x8005608Cu:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8005608C(a[0], a[1]); return 0u;
        case 0x80082CA8u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80082CA8(a[0], a[1], a[2]);
        case 0x80050E08u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_80050E08(a[0], a[1], a[2]); return 0u;
        case 0x8005081Cu:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8005081C(a[0], a[1], a[2], a[3]); return 0u;
        case 0x8006D4DCu:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8006D4DC(a[0], a[1]); return 0u;
        case 0x8006D460u:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8006D460(a[0], a[1]); return 0u;
        case 0x8006D398u:
            a[0] = va_arg(arguments, uint32);
            level_sub_8006D398(a[0]); return 0u;
        case 0x80081CDCu:
            for (i = 0u; i < 5u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80081CDC(a[0], a[1], a[2], a[3], a[4]);
        case 0x80056250u:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80056250(a[0], a[1]);
        case 0x8003FFA4u:
            a[0] = va_arg(arguments, uint32);
            return level_sub_8003FFA4(a[0]);
        case 0x80058420u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80058420(a[0], a[1], a[2]);
        case 0x80045084u:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_80045084(a[0], a[1]); return 0u;
        case 0x8004CDB0u:
            a[0] = va_arg(arguments, uint32);
            level_sub_8004CDB0(a[0]); return 0u;
        case 0x8006B854u:
            a[0] = va_arg(arguments, uint32);
            level_sub_8006B854(a[0]); return 0u;
        case 0x8006DCA0u:
            a[0] = va_arg(arguments, uint32);
            level_sub_8006DCA0(a[0]); return 0u;
        case 0x80087FA0u:
            a[0] = va_arg(arguments, uint32);
            level_sub_80087FA0(a[0]); return 0u;
        case 0x80043284u:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80043284(a[0], a[1]);
        case 0x8004E614u:
            a[0] = va_arg(arguments, uint32);
            return level_sub_8004E614(a[0]);
        case 0x8004DE80u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8004DE80(a[0], a[1], a[2]); return 0u;
        case 0x80073BA8u:
            for (i = 0u; i < 15u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80073BA8(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11], a[12], a[13], a[14]);
        case 0x80043140u:
            a[0] = va_arg(arguments, uint32);
            level_sub_80043140(a[0]); return 0u;
        case 0x80044BE4u:
            a[0] = va_arg(arguments, uint32);
            level_sub_80044BE4(a[0]); return 0u;
        case 0x8006F7BCu:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8006F7BC(a[0], a[1]); return 0u;
        case 0x800732F8u:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_800732F8(a[0], a[1]); return 0u;
        case 0x8008D9D8u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8008D9D8(a[0], a[1], a[2]); return 0u;
        case 0x800701DCu:
            a[0] = va_arg(arguments, uint32);
            level_sub_800701DC(a[0]); return 0u;
        case 0x80073B08u:
            a[0] = va_arg(arguments, uint32);
            level_sub_80073B08(a[0]); return 0u;
        case 0x80073960u:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80073960(a[0], a[1]);
        case 0x80078BCCu:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80078BCC(a[0], a[1], a[2], a[3]);
        case 0x800798ACu:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_800798AC(a[0], a[1]); return 0u;
        case 0x800705ECu:
            for (i = 0u; i < 5u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_800705EC(a[0], a[1], a[2], a[3], a[4]);
        case 0x800702E0u:
            for (i = 0u; i < 13u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_800702E0(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11], a[12]);
        case 0x8004E354u:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8004E354(a[0], a[1]);
        case 0x8005C210u:
        case 0x80059E40u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            if (address == 0x80059E40u) return level_sub_80059E40(a[0], a[1], a[2]);
            return level_sub_8005C210(a[0], a[1], a[2]);
        case 0x8005CB00u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8005CB00(a[0], a[1], a[2]); return 0u;
        case 0x8005B23Cu:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8005B23C(a[0], a[1], a[2]);
        case 0x80040F48u:
            for (i = 0u; i < 5u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80040F48(a[0], a[1], a[2], a[3], a[4]);
        case 0x8004E01Cu:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8004E01C(a[0], a[1], a[2]); return 0u;
        case 0x8004E1B8u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8004E1B8(a[0], a[1], a[2]); return 0u;
        case 0x80044470u:
            a[0] = va_arg(arguments, uint32);
            level_sub_80044470(a[0]); return 0u;
        case 0x80044B0Cu:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_80044B0C(a[0], a[1]); return 0u;
        case 0x8004492Cu:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8004492C(a[0], a[1]); return 0u;
        case 0x8006E058u:
            for (i = 0u; i < 11u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8006E058(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10]);
        case 0x8004CD90u:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8004CD90(a[0], a[1], a[2], a[3]);
        case 0x80088040u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80088040(a[0], a[1], a[2]);
        case 0x8004D8DCu:
            for (i = 0u; i < 5u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8004D8DC(a[0], a[1], a[2], a[3], a[4]); return 0u;
        case 0x8004572Cu:
            for (i = 0u; i < 7u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8004572C(a[0], a[1], a[2], a[3], a[4], a[5], a[6]); return 0u;
        case 0x80044D18u:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_80044D18(a[0], a[1], a[2], a[3]); return 0u;
        case 0x8006D6A0u:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8006D6A0(a[0], a[1]);
        case 0x800451B0u:
            for (i = 0u; i < 9u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_800451B0(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8]); return 0u;
        case 0x8004532Cu:
            for (i = 0u; i < 18u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8004532C(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11], a[12], a[13], a[14], a[15], a[16], a[17]);
        case 0x8005AB04u:
            a[0] = va_arg(arguments, uint32);
            level_sub_8005AB04(a[0]); return 0u;
        case 0x8005ABA0u:
            a[0] = va_arg(arguments, uint32);
            level_sub_8005ABA0(a[0]); return 0u;
        case 0x8005ABECu:
            a[0] = va_arg(arguments, uint32);
            level_sub_8005ABEC(a[0]); return 0u;
        case 0x80057C24u:
            a[0] = va_arg(arguments, uint32);
            return (uint32)level_sub_80057C24(a[0]);
        case 0x80057C8Cu:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            return (uint32)level_sub_80057C8C(a[0], a[1]);
        case 0x8005A0C8u:
            for (i = 0u; i < 6u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8005A0C8(a[0], a[1], a[2], a[3], a[4], a[5]);
        case 0x80056E40u:
            a[0] = va_arg(arguments, uint32);
            return (uint32)level_sub_80056E40(a[0]);
        case 0x80056EB4u:
            a[0] = va_arg(arguments, uint32);
            return (uint32)level_sub_80056EB4(a[0]);
        case 0x80056EA0u:
            a[0] = va_arg(arguments, uint32);
            return level_sub_80056EA0(a[0]);
        case 0x8006D600u:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8006D600(a[0], a[1]);
        case 0x8006A570u:
            a[0] = va_arg(arguments, uint32);
            level_sub_8006A570(a[0]); return 0u;
        case 0x8006A3BCu:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8006A3BC(a[0], a[1]);
        case 0x80078350u:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80078350(a[0], a[1], a[2], a[3]);
        case 0x80050720u:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80050720(a[0], a[1]);
        case 0x8008406Cu:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8008406C(a[0], a[1]); return 0u;
        case 0x80083F64u:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_80083F64(a[0], a[1]); return 0u;
        case 0x80083FE8u:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_80083FE8(a[0], a[1]); return 0u;
        case 0x80074DC4u:
            a[0] = va_arg(arguments, uint32);
            level_sub_80074DC4(a[0]); return 0u;
        case 0x80059500u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80059500(a[0], a[1], a[2]);
        case 0x8008E354u:
            for (i = 0u; i < 5u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8008E354(a[0], a[1], a[2], a[3], a[4]);
        case 0x8008DABCu:
            for (i = 0u; i < 6u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8008DABC(a[0], a[1], a[2], a[3], a[4], a[5]);
        case 0x80044E0Cu:
            for (i = 0u; i < 9u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_80044E0C(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8]); return 0u;
        case 0x8003DC10u: return level_sub_8003DC10(va_arg(arguments, uint32));
        case 0x8003EC78u:
        case 0x8003ECE4u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            if (address == 0x8003EC78u) return level_sub_8003EC78(a[0], a[1], a[2]);
            return level_sub_8003ECE4(a[0], a[1], a[2]);
        case 0x8003EACCu:
        case 0x8003EF5Cu:
        case 0x8003E3F0u:
            for (i = 0u; i < 7u; ++i) a[i] = va_arg(arguments, uint32);
            if (address == 0x8003E3F0u) return level_sub_8003E3F0(a[0], a[1], a[2], a[3], a[4], a[5], a[6]);
            if (address == 0x8003EACCu) return level_sub_8003EACC(a[0], a[1], a[2], a[3], a[4], a[5], a[6]);
            return level_sub_8003EF5C(a[0], a[1], a[2], a[3], a[4], a[5], a[6]);
        case 0x800662ECu: return level_sub_800662EC();
        case 0x800663B8u: return level_sub_800663B8();
        case 0x80075034u: level_sub_80075034(); return 0u;
        case 0x80062358u: level_sub_80062358(); return 0u;
        case 0x80087308u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            return level_sub_80087308(a[0], a[1]);
        case 0x80086B20u: level_sub_80086B20(va_arg(arguments, uint32)); return 0u;
        case 0x80087078u: level_sub_80087078(); return 0u;
        case 0x8006A8FCu: return level_sub_8006A8FC(va_arg(arguments, uint32));
        case 0x80087080u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            a[2] = va_arg(arguments, uint32);
            level_sub_80087080(a[0], a[1], a[2]); return 0u;
        case 0x80053E68u: return level_sub_80053E68();
        case 0x8006B5B8u:
            for (i = 0u; i < 8u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8006B5B8(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
        case 0x80049540u: return level_sub_80049540(va_arg(arguments, uint32));
        case 0x80049444u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            return level_sub_80049444(a[0], a[1]);
        case 0x8006D930u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_8006D930(a[0], a[1]); return 0u;
        case 0x8006B350u: level_sub_8006B350(va_arg(arguments, uint32)); return 0u;
        case 0x8006D84Cu:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            return level_sub_8006D84C(a[0], a[1]);
        case 0x8005D104u: level_sub_8005D104(va_arg(arguments, uint32)); return 0u;
        case 0x8007529Cu: return level_sub_8007529C(va_arg(arguments, uint32));
        case 0x80075120u: level_sub_80075120(va_arg(arguments, uint32)); return 0u;
        case 0x800751B4u: level_sub_800751B4(va_arg(arguments, uint32)); return 0u;
        case 0x800751D0u: level_sub_800751D0(va_arg(arguments, uint32)); return 0u;
        case 0x800751ECu: level_sub_800751EC(va_arg(arguments, uint32)); return 0u;
        case 0x80086F98u: level_sub_80086F98(va_arg(arguments, uint32)); return 0u;
        case 0x8007540Cu: return level_sub_8007540C(va_arg(arguments, uint32));
        case 0x80066108u: return level_sub_80066108();
        case 0x80066790u: return level_sub_80066790();
        case 0x8006685Cu: level_sub_8006685C(); return 0u;
        case 0x80087294u: level_sub_80087294(); return 0u;
        case 0x8004787Cu: return level_sub_8004787C();
        case 0x8004770Cu: level_sub_8004770C(va_arg(arguments, uint32)); return 0u;
        case 0x8006A76Cu: level_sub_8006A76C(va_arg(arguments, uint32)); return 0u;
        case 0x8008CFE8u: return level_sub_8008CFE8(va_arg(arguments, uint32));
        case 0x8008CDF4u: return level_sub_8008CDF4(va_arg(arguments, uint32));
        case 0x8003F454u:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8003F454(a[0], (sint32)a[1], a[2], a[3]);
        case 0x8003F8F0u: level_sub_8003F8F0(va_arg(arguments, uint32)); return 0u;
        case 0x8004C64Cu:
            for (i = 0u; i < 5u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8004C64C(a[0], a[1], (sint32)a[2], (sint32)a[3], (sint32)a[4]); return 0u;
        case 0x8005D18Cu:
            for (i = 0u; i < 6u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8005D18C(a[0], a[1], a[2], a[3], a[4], a[5]);
        case 0x800590B4u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_800590B4(a[0], a[1], a[2]);
        case 0x80059C88u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80059C88(a[0], a[1], a[2]);
        case 0x80059218u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80059218(a[0], a[1], a[2]);
        case 0x80046C38u:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_80046C38(a[0], a[1], a[2], a[3]); return 0u;
        case 0x8004760Cu:
            level_sub_8004760C(va_arg(arguments, uint32)); return 0u;
        case 0x8006D574u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            return level_sub_8006D574(a[0], a[1]);
        case 0x80061168u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            return level_sub_80061168(a[0], a[1]);
        case 0x8007BD68u: return level_sub_8007BD68();
        case 0x8007BD1Cu:
            level_sub_8007BD1C(va_arg(arguments, uint32)); return 0u;
        case 0x80058C38u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80058C38(a[0], a[1], a[2]);
        case 0x80058C58u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80058C58(a[0], a[1], a[2]);
        case 0x80058C78u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            return level_sub_80058C78(a[0], a[1]);
        case 0x80058F84u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80058F84(a[0], a[1], a[2]);
        case 0x800594ACu:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_800594AC(a[0], a[1], a[2]);
        case 0x8008DAA8u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_8008DAA8(a[0], a[1]); return 0u;
        case 0x800589E8u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_800589E8(a[0], a[1], a[2]);
        case 0x80057D9Cu:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80057D9C(a[0], a[1], a[2]);
        case 0x8003F1F8u:
            level_sub_8003F1F8(va_arg(arguments, uint32)); return 0u;
        case 0x800665BCu:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_800665BC(a[0], a[1]); return 0u;
        case 0x800664A0u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_800664A0(a[0], a[1], a[2]); return 0u;
        case 0x8006602Cu:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            return level_sub_8006602C(a[0], a[1]);
        case 0x8006D8ACu: return level_sub_8006D8AC(va_arg(arguments, uint32));
        case 0x8006D898u: return level_sub_8006D898(va_arg(arguments, uint32));
        case 0x800694C8u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            return level_sub_800694C8(a[0], a[1]);
        case 0x800672E0u:
            level_sub_800672E0(va_arg(arguments, uint32)); return 0u;
        case 0x80064858u:
            level_sub_80064858(va_arg(arguments, uint32)); return 0u;
        case 0x80066000u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80066000(a[0], a[1], a[2]);
        case 0x80069514u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_80069514(a[0], a[1], a[2]); return 0u;
        case 0x8006692Cu:
            for (i = 0u; i < 9u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8006692C(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8]);
        case 0x80066C14u:
            for (i = 0u; i < 7u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80066C14(a[0], a[1], a[2], a[3], a[4], a[5], a[6]);
        case 0x80066894u: return level_sub_80066894();
        case 0x80074AD8u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_80074AD8(a[0], a[1]); return 0u;
        case 0x8007BC0Cu:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_8007BC0C(a[0], a[1]); return 0u;
        case 0x8007BADCu:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8007BADC(a[0], a[1], a[2]); return 0u;
        case 0x8007A050u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8007A050(a[0], a[1], a[2]); return 0u;
        case 0x80076240u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80076240(a[0], a[1], a[2]);
        case 0x800756D8u:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_800756D8(a[0], a[1], a[2], a[3]); return 0u;
        case 0x80075C6Cu:
            for (i = 0u; i < 7u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_80075C6C(a[0], a[1], a[2], a[3], a[4], a[5], a[6]); return 0u;
        case 0x80075810u:
            for (i = 0u; i < 9u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_80075810(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8]); return 0u;
        case 0x800759A4u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_800759A4(a[0], a[1], a[2]); return 0u;
        case 0x80076094u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_80076094(a[0], a[1]); return 0u;
        case 0x80075D1Cu:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_80075D1C(a[0], a[1], a[2], a[3]); return 0u;
        case 0x800758B4u:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_800758B4(a[0], a[1], a[2], a[3]); return 0u;
        case 0x80076118u:
            level_sub_80076118(va_arg(arguments, uint32)); return 0u;
        case 0x800760C8u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_800760C8(a[0], a[1]); return 0u;
        case 0x80076160u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_80076160(a[0], a[1]); return 0u;
        case 0x800469E0u: level_sub_800469E0(); return 0u;
        case 0x8007555Cu: return level_sub_8007555C(va_arg(arguments, uint32));
        case 0x8007BA20u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            return level_sub_8007BA20(a[0], a[1]);
        case 0x80046874u: level_sub_80046874(va_arg(arguments, uint32)); return 0u;
        case 0x80046574u: level_sub_80046574(va_arg(arguments, uint32)); return 0u;
        case 0x800467ECu: level_sub_800467EC(va_arg(arguments, uint32)); return 0u;
        case 0x80046700u:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_80046700(a[0], (sint32)a[1], a[2], a[3]); return 0u;
        case 0x8008AB48u: level_sub_8008AB48(); return 0u;
        case 0x80049A9Cu: level_sub_80049A9C(); return 0u;
        case 0x80066290u: level_sub_80066290(); return 0u;
        case 0x80041AA0u: return (uint32)level_sub_80041AA0();
        case 0x80069D04u: level_sub_80069D04(); return 0u;
        case 0x80069DECu: level_sub_80069DEC(); return 0u;
        case 0x80069A74u: level_sub_80069A74(); return 0u;
        case 0x80052D70u: level_sub_80052D70(va_arg(arguments, uint32)); return 0u;
        case 0x8007505Cu: level_sub_8007505C(va_arg(arguments, uint32)); return 0u;
        case 0x80074C54u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            return level_sub_80074C54(a[0], a[1]);
        case 0x80074CE8u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            return level_sub_80074CE8(a[0], a[1]);
        case 0x80083960u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80083960(a[0], a[1], a[2]);
        case 0x800833A0u:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_800833A0(a[0], a[1], a[2], a[3]);
        case 0x8008335Cu:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8008335C(a[0], a[1], a[2]); return 0u;
        case 0x800830B0u:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_800830B0(a[0], a[1], a[2], a[3]); return 0u;
        case 0x80060474u:
        case 0x800603DCu:
        case 0x8005FC00u:
        case 0x8005FDCCu:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            if (address == 0x80060474u) return (uint32)level_sub_80060474(a[0], a[1]);
            if (address == 0x800603DCu) return (uint32)level_sub_800603DC(a[0], a[1]);
            if (address == 0x8005FC00u) return level_sub_8005FC00(a[0], a[1]);
            return (uint32)level_sub_8005FDCC(a[0], a[1]);
        case 0x8005FB20u: level_sub_8005FB20(); return 0u;
        case 0x80088170u: level_sub_80088170(); return 0u;
        case 0x8004F700u: return level_sub_8004F700();
        case 0x8008A580u: level_sub_8008A580(); return 0u;
        case 0x8008EAC8u: level_sub_8008EAC8(); return 0u;
        case 0x8004DE3Cu: level_sub_8004DE3C(); return 0u;
        case 0x8006BA20u: level_sub_8006BA20(va_arg(arguments, uint32)); return 0u;
        case 0x800473DCu: level_sub_800473DC(va_arg(arguments, uint32)); return 0u;
        case 0x800479D8u: { uint32 object = va_arg(arguments, uint32), target = va_arg(arguments, uint32); level_sub_800479D8(object, target); return 0u; }
        case 0x800482A4u: { uint32 object = va_arg(arguments, uint32), target = va_arg(arguments, uint32); level_sub_800482A4(object, target); return 0u; }
        case 0x8004D43Cu: level_sub_8004D43C(va_arg(arguments, uint32)); return 0u;
        case 0x8004913Cu: level_sub_8004913C(va_arg(arguments, uint32)); return 0u;
        case 0x8004F488u: level_sub_8004F488(va_arg(arguments, uint32)); return 0u;
        case 0x80053C00u: level_sub_80053C00(va_arg(arguments, uint32)); return 0u;
        case 0x8007A6B0u: level_sub_8007A6B0(va_arg(arguments, uint32)); return 0u;
        case 0x8007F300u: { uint32 object = va_arg(arguments, uint32), controller = va_arg(arguments, uint32); level_sub_8007F300(object, controller); return 0u; }
        case 0x8007E034u: { uint32 object = va_arg(arguments, uint32), state = va_arg(arguments, uint32); level_sub_8007E034(object, state); return 0u; }
        case 0x8006C83Cu: return level_sub_8006C83C(va_arg(arguments, uint32));
        case 0x80078F60u:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            return (uint32)level_sub_80078F60(a[0], a[1], a[2], a[3]);
        case 0x80078A38u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80078A38(a[0], a[1], a[2]);
        case 0x80078AA4u:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80078AA4(a[0], a[1], a[2], a[3]);
        case 0x80084400u:
            a[0] = va_arg(arguments, uint32);
            level_sub_80084400(a[0]); return 0u;
        case 0x800845A8u:
            a[0] = va_arg(arguments, uint32);
            level_sub_800845A8(a[0]); return 0u;
        case 0x8007B350u:
            a[0] = va_arg(arguments, uint32);
            level_sub_8007B350(a[0]); return 0u;
        case 0x8007D5C8u:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8007D5C8(a[0], a[1], a[2], a[3]); return 0u;
        case 0x8006C08Cu:
            a[0] = va_arg(arguments, uint32);
            level_sub_8006C08C(a[0]); return 0u;
        case 0x8005F5A0u:
            a[0] = va_arg(arguments, uint32);
            level_sub_8005F5A0(a[0]); return 0u;
        case 0x8005F69Cu:
            level_sub_8005F69C(); return 0u;
        case 0x80050548u:
            a[0] = va_arg(arguments, uint32);
            level_sub_80050548(a[0]); return 0u;
        case 0x800503D4u:
            level_sub_800503D4(); return 0u;
        case 0x80053B8Cu:
            a[0] = va_arg(arguments, uint32);
            level_sub_80053B8C(a[0]); return 0u;
        case 0x80053BD4u:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80053BD4(a[0], a[1]);
        case 0x80053BF4u:
            a[0] = va_arg(arguments, uint32);
            level_sub_80053BF4(a[0]); return 0u;
        case 0x80088280u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_80088280(a[0], a[1], a[2]); return 0u;
        case 0x80088368u:
            a[0] = va_arg(arguments, uint32);
            return level_sub_80088368(a[0]);
        case 0x80089A0Cu:
            a[0] = va_arg(arguments, uint32);
            level_sub_80089A0C(a[0]); return 0u;
        case 0x800787C0u:
            for (i = 0u; i < 10u; ++i) a[i] = va_arg(arguments, uint32);
            return (uint32)level_sub_800787C0(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9]);
        case 0x8007848Cu:
            for (i = 0u; i < 14u; ++i) a[i] = va_arg(arguments, uint32);
            return (uint32)level_sub_8007848C(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11], a[12], a[13]);
        case 0x80083D30u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80083D30(a[0], a[1], a[2]);
        case 0x800840E0u:
            a[0] = va_arg(arguments, uint32); level_sub_800840E0(a[0]); return 0u;
        case 0x80083EF0u:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_80083EF0(a[0], a[1]); return 0u;
        case 0x80083DE8u:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_80083DE8(a[0], a[1]); return 0u;
        case 0x80074E10u:
            a[0] = va_arg(arguments, uint32); level_sub_80074E10(a[0]); return 0u;
        case 0x800662CCu:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_800662CC(a[0], a[1], a[2], a[3]);
        case 0x80066EA0u:
            for (i = 0u; i < 5u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80066EA0(a[0], a[1], a[2], a[3], a[4]);
        case 0x8005359Cu:
            for (i = 0u; i < 5u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8005359C(a[0], a[1], a[2], a[3], a[4]);
        case 0x800670E8u:
            for (i = 0u; i < 5u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_800670E8(a[0], a[1], a[2], a[3], a[4]);
        case 0x80064870u:
            return level_sub_80064870(va_arg(arguments, uint32));
        case 0x8004CB18u:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8004CB18(a[0], a[1]);
        case 0x800830BCu:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_800830BC(a[0], a[1]);
        case 0x8005F60Cu:
            level_sub_8005F60C(va_arg(arguments, uint32)); return 0u;
        case 0x8004301Cu:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8004301C(a[0], a[1], a[2]); return 0u;
        case 0x80042524u: level_sub_80042524(va_arg(arguments, uint32)); return 0u;
        case 0x8006B89Cu: level_sub_8006B89C(va_arg(arguments, uint32)); return 0u;
        case 0x8006DA50u: level_sub_8006DA50(va_arg(arguments, uint32)); return 0u;
        case 0x8006A318u: level_sub_8006A318(va_arg(arguments, uint32)); return 0u;
        case 0x8006A37Cu: level_sub_8006A37C(va_arg(arguments, uint32)); return 0u;
        case 0x80066918u: level_sub_80066918(va_arg(arguments, uint32)); return 0u;
        case 0x800665FCu:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_800665FC(a[0], a[1]); return 0u;
        case 0x8007C030u:
            for (i = 0u; i < 2u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8007C030(a[0], a[1]);
        case 0x8004B6DCu:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8004B6DC(a[0], a[1], a[2], a[3]);
        case 0x8004C990u:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8004C990(a[0], a[1], a[2], a[3]);
        case 0x80088A5Cu: level_sub_80088A5C(va_arg(arguments, uint32)); return 0u;
        case 0x8006C714u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_8006C714(a[0], a[1]); return 0u;
        case 0x80046DD4u: level_sub_80046DD4(va_arg(arguments, uint32)); return 0u;
        case 0x8004F670u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_8004F670(a[0], a[1]); return 0u;
        case 0x80053C20u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_80053C20(a[0], a[1]); return 0u;
        case 0x80060AFCu: level_sub_80060AFC(va_arg(arguments, uint32)); return 0u;
        case 0x80060924u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            return level_sub_80060924(a[0], a[1]);
        case 0x8008CBE4u: return level_sub_8008CBE4(va_arg(arguments, uint32));
        case 0x8008D850u: level_sub_8008D850(va_arg(arguments, uint32)); return 0u;
        case 0x8008BF00u:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8008BF00(a[0], a[1], a[2], a[3]);
        case 0x8008B3E8u: level_sub_8008B3E8(va_arg(arguments, uint32)); return 0u;
        case 0x8008C0C8u: level_sub_8008C0C8(); return 0u;
        case 0x8007B8F4u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_8007B8F4(a[0], a[1]); return 0u;
        case 0x8006DD54u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_8006DD54(a[0], a[1]); return 0u;
        case 0x8006A5B4u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_8006A5B4(a[0], a[1]); return 0u;
        case 0x8006D2E4u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8006D2E4(a[0], a[1], a[2]); return 0u;
        case 0x8008D660u: level_sub_8008D660(va_arg(arguments, uint32)); return 0u;
        case 0x800824D0u:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_800824D0(a[0], a[1], a[2], a[3]);
        case 0x80082948u:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80082948(a[0], a[1], a[2], a[3]);
        case 0x80080DD4u:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80080DD4(a[0], a[1], a[2], a[3]);
        case 0x8007CAF8u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8007CAF8(a[0], a[1], a[2]); return 0u;
        case 0x800504F8u: level_sub_800504F8(); return 0u;
        case 0x800504A0u: level_sub_800504A0(); return 0u;
        case 0x80090B80u: level_sub_80090B80(); return 0u;
        case 0x80090A30u: level_sub_80090A30(); return 0u;
        case 0x80090AECu: level_sub_80090AEC(); return 0u;
        case 0x80066318u: return level_sub_80066318();
        case 0x8006A204u: return level_sub_8006A204(va_arg(arguments, uint32));
        case 0x8004137Cu: level_sub_8004137C(); return 0u;
        case 0x8005D138u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            return level_sub_8005D138(a[0], a[1]);
        case 0x80059F38u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80059F38(a[0], a[1], a[2]);
        case 0x80059C2Cu:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            return level_sub_80059C2C(a[0], a[1]);
        case 0x8007B4B8u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            return level_sub_8007B4B8(a[0], a[1]);
        case 0x80054100u: level_sub_80054100(); return 0u;
        case 0x800540C8u: return level_sub_800540C8(va_arg(arguments, uint32));
        case 0x80054238u: return level_sub_80054238(va_arg(arguments, uint32));
        case 0x80054298u: level_sub_80054298(); return 0u;
        case 0x8005439Cu: level_sub_8005439C(); return 0u;
        case 0x8006DA88u: level_sub_8006DA88(); return 0u;
        case 0x80074F9Cu: return level_sub_80074F9C();
        case 0x80080480u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80080480(a[0], a[1], a[2]);
        case 0x8008080Cu:
            for (i = 0u; i < 6u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8008080C(a[0], a[1], a[2], a[3], a[4], a[5]);
        case 0x8008E5B0u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            return level_sub_8008E5B0(a[0], a[1]);
        case 0x8008E9DCu: return level_sub_8008E9DC(va_arg(arguments, uint32));
        case 0x800460F4u: return level_sub_800460F4(va_arg(arguments, uint32));
        case 0x80046A88u: level_sub_80046A88(va_arg(arguments, uint32)); return 0u;
        case 0x8005959Cu:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8005959C(a[0], a[1], a[2]);
        case 0x80058734u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80058734(a[0], a[1], a[2]);
        case 0x80046B2Cu:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            return level_sub_80046B2C(a[0], a[1]);
        case 0x80061350u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            return level_sub_80061350(a[0], a[1]);
        case 0x8006D6F0u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            return level_sub_8006D6F0(a[0], a[1]);
        case 0x80069714u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_80069714(a[0], a[1]); return 0u;
        case 0x8008019Cu:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_8008019C(a[0], a[1]); return 0u;
        case 0x8004E8FCu:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_8004E8FC(a[0], a[1]); return 0u;
        case 0x8008BFFCu:
            for (i = 0u; i < 7u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8008BFFC(a[0], a[1], a[2], a[3], a[4], a[5], a[6]); return 0u;
        case 0x8008CB58u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            return level_sub_8008CB58(a[0], a[1]);
        case 0x8008CB10u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8008CB10(a[0], a[1], a[2]);
        case 0x8008C9F0u:
            for (i = 0u; i < 7u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8008C9F0(a[0], a[1], a[2], a[3], a[4], a[5], a[6]);
        case 0x8008C964u:
            for (i = 0u; i < 5u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8008C964(a[0], a[1], a[2], a[3], a[4]);
        case 0x80086634u: level_sub_80086634(va_arg(arguments, uint32)); return 0u;
        case 0x80085648u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_80085648(a[0], a[1]); return 0u;
        case 0x8008492Cu:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8008492C(a[0], a[1], a[2], a[3]);
        case 0x800847CCu:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_800847CC(a[0], a[1], a[2]);
        case 0x8008DA98u: level_sub_8008DA98(); return 0u;
        case 0x80084718u:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_80084718(a[0], a[1], a[2], a[3]); return 0u;
        case 0x80064550u: level_sub_80064550(); return 0u;
        case 0x8005F86Cu:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return (uint32)level_sub_8005F86C(a[0], a[1], a[2]);
        case 0x8005F3E8u:
            for (i = 0u; i < 16u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8005F3E8(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11], a[12], a[13], a[14], a[15]);
        case 0x800776C0u:
            for (i = 0u; i < 8u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_800776C0(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
        case 0x8007711Cu:
            for (i = 0u; i < 7u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8007711C(a[0], a[1], a[2], a[3], a[4], a[5], a[6]);
        case 0x800770A8u: level_sub_800770A8(); return 0u;
        case 0x80078E2Cu:
            for (i = 0u; i < 10u; ++i) a[i] = va_arg(arguments, uint32);
            return (uint32)level_sub_80078E2C(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9]);
        case 0x80078D1Cu:
            for (i = 0u; i < 8u; ++i) a[i] = va_arg(arguments, uint32);
            return (uint32)level_sub_80078D1C(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
        case 0x80078C4Cu:
            for (i = 0u; i < 5u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_80078C4C(a[0], a[1], a[2], a[3], a[4]); return 0u;
        case 0x8006C808u: level_sub_8006C808(va_arg(arguments, uint32)); return 0u;
        case 0x8006A870u: { uint32 object = va_arg(arguments, uint32), matrix = va_arg(arguments, uint32); level_sub_8006A870(object, matrix); return 0u; }
        case 0x8008D918u:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8008D918(a[0], a[1], a[2], a[3]); return 0u;
        case 0x800760ACu: return level_sub_800760AC(va_arg(arguments, uint32));
        case 0x8004D590u:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8004D590(a[0], a[1], a[2], a[3]);
        case 0x800759ECu: { uint32 controller = va_arg(arguments, uint32), axis = va_arg(arguments, uint32); return level_sub_800759EC(controller, axis); }
        case 0x80075A30u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80075A30(a[0], a[1], a[2]);
        case 0x80075FB4u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80075FB4(a[0], a[1], a[2]);
        case 0x80076008u:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_80076008(a[0], a[1], a[2], a[3]);
        case 0x80075118u: level_sub_80075118(va_arg(arguments, uint32)); return 0u;
        case 0x8007570Cu: { uint32 controller = va_arg(arguments, uint32), action = va_arg(arguments, uint32); return level_sub_8007570C(controller, action); }
        case 0x80075778u: { uint32 controller = va_arg(arguments, uint32), action = va_arg(arguments, uint32); return level_sub_80075778(controller, action); }
        case 0x80053AC0u: level_sub_80053AC0(); return 0u;
        case 0x80082E88u: level_sub_80082E88(va_arg(arguments, uint32)); return 0u;
        case 0x8006B8F0u: level_sub_8006B8F0(va_arg(arguments, uint32)); return 0u;
        case 0x8008D0CCu: level_sub_8008D0CC(va_arg(arguments, uint32)); return 0u;
        case 0x8008D604u: level_sub_8008D604(va_arg(arguments, uint32)); return 0u;
        case 0x8008CCC8u: { uint32 input = va_arg(arguments, uint32), output = va_arg(arguments, uint32); level_sub_8008CCC8(input, output); return 0u; }
        case 0x8008CE28u:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8008CE28(a[0], a[1], a[2], a[3]); return 0u;
        case 0x80048EECu: { uint32 view = va_arg(arguments, uint32), object = va_arg(arguments, uint32); level_sub_80048EEC(view, object); return 0u; }
        case 0x8004CEE4u:
            for (i = 0u; i < 6u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8004CEE4(a[0], a[1], a[2], a[3], a[4], a[5]); return 0u;
        case 0x80046C64u: level_sub_80046C64(va_arg(arguments, uint32)); return 0u;
        case 0x80052C38u: level_sub_80052C38(); return 0u;
        case 0x8004A828u: level_sub_8004A828(va_arg(arguments, uint32)); return 0u;
        case 0x8004D974u: level_sub_8004D974(); return 0u;
        case 0x8004D670u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32); a[2] = va_arg(arguments, uint32);
            return level_sub_8004D670(a[0], a[1], a[2]);
        case 0x8004D694u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32); a[2] = va_arg(arguments, uint32);
            a[3] = va_arg(arguments, uint32); a[4] = va_arg(arguments, uint32);
            return level_sub_8004D694(a[0], a[1], a[2], a[3], a[4]);
        case 0x8004D7DCu:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_8004D7DC(a[0], a[1]); return 0u;
        case 0x8008C850u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_8008C850(a[0], a[1]); return 0u;
        case 0x8008C514u:
        case 0x8003F1B8u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            if (address == 0x8008C514u) return level_sub_8008C514(a[0], a[1]);
            return level_sub_8003F1B8(a[0], a[1]);
        case 0x8003EB44u: return level_sub_8003EB44(va_arg(arguments, uint32));
        case 0x800545D8u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_800545D8(a[0], a[1], a[2]); return 0u;
        case 0x80049AC4u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            /* Native file transport retains size output, destination and terminator */
            return moh_native_load_file(a[0], a[1], a[2]);
        case 0x8006028Cu:
        case 0x8005FB94u:
        case 0x80060138u:
        case 0x80060034u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            if (address == 0x8006028Cu) return (uint32)level_sub_8006028C(a[0], a[1], a[2]);
            if (address == 0x8005FB94u) return level_sub_8005FB94((sint32)a[0], (sint32)a[1], a[2]);
            if (address == 0x80060138u) return (uint32)level_sub_80060138(a[0], a[1], a[2]);
            return (uint32)level_sub_80060034(a[0], a[1], a[2]);
        case 0x8005FEA0u:
            for (i = 0u; i < 5u; ++i) a[i] = va_arg(arguments, uint32);
            return (uint32)level_sub_8005FEA0(a[0], a[1], a[2], (sint32)a[3], a[4]);
        case 0x80074EE8u: level_sub_80074EE8(va_arg(arguments, uint32)); return 0u;
        case 0x8005372Cu:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_8005372C(a[0], a[1]); return 0u;
        case 0x80052DC4u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_80052DC4(a[0], a[1]); return 0u;
        case 0x80042AA4u:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_80042AA4(a[0], a[1], a[2], (sint32)a[3]); return 0u;
        case 0x800427F4u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_800427F4(a[0], a[1], a[2]); return 0u;
        case 0x80042998u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            return (uint32)level_sub_80042998((sint32)a[0], (sint32)a[1]);
        case 0x80041CE8u:
            for (i = 0u; i < 11u; ++i) a[i] = va_arg(arguments, uint32);
            return (uint32)level_sub_80041CE8(a[0], (sint32)a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10]);
        case 0x80041B04u:
            for (i = 0u; i < 7u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_80041B04((sint32)a[0], (sint32)a[1], a[2], (sint32)a[3], (sint32)a[4], a[5], a[6]); return 0u;
        case 0x8006B234u:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8006B234(a[0], a[1], a[2]); return 0u;
        case 0x8006B1C4u:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8006B1C4(a[0], a[1], a[2], a[3]);
        case 0x8004249Cu:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_8004249C(a[0], a[1]); return 0u;
        case 0x8006DCACu:
            for (i = 0u; i < 5u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8006DCAC(a[0], a[1], a[2], a[3], a[4]); return 0u;
        case 0x8006D7D0u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            return level_sub_8006D7D0(a[0], a[1]);
        case 0x8006E004u:
            for (i = 0u; i < 10u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8006E004(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9]); return 0u;
        case 0x8008A4F8u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_8008A4F8(a[0], a[1]); return 0u;
        case 0x8006192Cu: level_sub_8006192C(); return 0u;
        case 0x80061A2Cu:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_80061A2C(a[0], a[1]); return 0u;
        case 0x8003FBFCu: return level_sub_8003FBFC(va_arg(arguments, uint32));
        case 0x8007DD4Cu: level_sub_8007DD4C(va_arg(arguments, uint32)); return 0u;
        case 0x80080240u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_80080240(a[0], a[1]); return 0u;
        case 0x80054554u: level_sub_80054554(va_arg(arguments, uint32)); return 0u;
        case 0x80088744u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_80088744(a[0], a[1]); return 0u;
        case 0x8008A634u: level_sub_8008A634(); return 0u;
        case 0x80061D08u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_80061D08(a[0], a[1]); return 0u;
        case 0x80074EA8u: level_sub_80074EA8(); return 0u;
        case 0x800540ACu: level_sub_800540AC(); return 0u;
        case 0x80054580u: level_sub_80054580(); return 0u;
        case 0x80075684u: level_sub_80075684(); return 0u;
        case 0x80075208u: level_sub_80075208(); return 0u;
        case 0x80061158u: level_sub_80061158(); return 0u;
        case 0x80052C18u: level_sub_80052C18(); return 0u;
        case 0x800888C4u: level_sub_800888C4(); return 0u;
        case 0x80088980u: level_sub_80088980(); return 0u;
        case 0x80061DE0u: level_sub_80061DE0(); return 0u;
        case 0x80053DB0u: level_sub_80053DB0(); return 0u;
        case 0x8004F748u:
            for (i = 0u; i < 11u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8004F748(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10]);
        case 0x8005F28Cu:
            for (i = 0u; i < 11u; ++i) a[i] = va_arg(arguments, uint32);
            return (uint32)level_sub_8005F28C(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10]);
        case 0x80079718u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_80079718(a[0], (sint32)a[1]); return 0u;
        case 0x8005D148u:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8005D148(a[0], a[1], a[2], a[3]);
        case 0x8005C97Cu:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_8005C97C(a[0], a[1], a[2]); return 0u;
        case 0x8005CBA8u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            return level_sub_8005CBA8(a[0], a[1]);
        case 0x800754CCu: return level_sub_800754CC((sint32)va_arg(arguments, uint32));
        case 0x80074BB8u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            return level_sub_80074BB8(a[0], a[1]);
        case 0x8006A910u: return level_sub_8006A910(va_arg(arguments, uint32));
        case 0x8006A96Cu:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            return level_sub_8006A96C(a[0], a[1], a[2], a[3]);
        case 0x800425C4u: level_sub_800425C4(va_arg(arguments, uint32)); return 0u;
        case 0x8007BE04u: level_sub_8007BE04(va_arg(arguments, uint32)); return 0u;
        case 0x80048DC8u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            a[2] = va_arg(arguments, uint32);
            level_sub_80048DC8(a[0], a[1], a[2]); return 0u;
        case 0x8006B834u: level_sub_8006B834(va_arg(arguments, uint32)); return 0u;
        case 0x80061130u: level_sub_80061130(); return 0u;
        case 0x800412E0u: level_sub_800412E0(); return 0u;
        case 0x8003F788u: return (uint32)level_sub_8003F788(va_arg(arguments, uint32));
        case 0x80086C9Cu: level_sub_80086C9C(va_arg(arguments, uint32)); return 0u;
        case 0x80086D18u: level_sub_80086D18(va_arg(arguments, uint32)); return 0u;
        case 0x80086D8Cu: level_sub_80086D8C(va_arg(arguments, uint32)); return 0u;
        case 0x80086E34u: level_sub_80086E34(va_arg(arguments, uint32)); return 0u;
        case 0x80086EB0u: level_sub_80086EB0(va_arg(arguments, uint32)); return 0u;
        case 0x80086F24u: level_sub_80086F24(va_arg(arguments, uint32)); return 0u;
        case 0x800418E4u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            return (uint32)level_sub_800418E4(a[0], a[1]);
        case 0x8004270Cu:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            a[2] = va_arg(arguments, uint32);
            return level_sub_8004270C(a[0], a[1], a[2]);
        case 0x800869E0u: return level_sub_800869E0(va_arg(arguments, uint32));
        case 0x80086A94u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            level_sub_80086A94(a[0], a[1]); return 0u;
        case 0x8004A374u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            return moh_native_load_bsd(a[0], a[1]);
        case 0x8004A300u:
            a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
            return level_sub_8004A300(a[0], a[1]);
        case 0x8008A554u: level_sub_8008A554(); return 0u;
        case 0x8008EA70u: level_sub_8008EA70(); return 0u;
        case 0x8004DDACu: level_sub_8004DDAC(); return 0u;
        case 0x80061590u: level_sub_80061590(); return 0u;
        case 0x80061568u: level_sub_80061568(); return 0u;
        case 0x80049410u: level_sub_80049410(); return 0u;
        case 0x80041260u: level_sub_80041260(); return 0u;
        case 0x8005F260u: level_sub_8005F260(); return 0u;
        case 0x800606E0u: level_sub_800606E0(va_arg(arguments, uint32)); return 0u;
        case 0x8007B780u: level_sub_8007B780(); return 0u;
        case 0x8008EDDCu: level_sub_8008EDDC(va_arg(arguments, uint32)); return 0u;
        case 0x8008EC14u: level_sub_8008EC14(va_arg(arguments, uint32)); return 0u;
        case 0x80042790u: level_sub_80042790(); return 0u;
        case 0x8008A6A8u: level_sub_8008A6A8(); return 0u;
        case 0x80060548u: level_sub_80060548(); return 0u;
        case 0x800604D0u: level_sub_800604D0(va_arg(arguments, uint32)); return 0u;
        case 0x800501B8u: level_sub_800501B8(va_arg(arguments, uint32)); return 0u;
        case 0x800501CCu:
            for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_800501CC(a[0], (sint32)a[1], (sint32)a[2]); return 0u;
        case 0x800502B4u:
            for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
            level_sub_800502B4(a[0], a[1], a[2], a[3]); return 0u;
        default: moh_runtime_error("Untranslated LEVEL call", address);
        }
    }
    switch (address)
    {
    case 0x8005FC28u:
        return shell_sub_8005FC28();
    case 0x8001FC78u:
    {
        SpuReverbAttr attr = {0};
        uint32 mode = va_arg(arguments, uint32), magnitude = mode;
        sint32 result;
        if (mode & 0x8000u) magnitude = 0u - mode;
        if ((uint16)magnitude >= 10u) return (uint32)-1;
        attr.mask = 1u;
        attr.mode = (sint16)((uint16)magnitude | ((mode & 0x8000u) ? 0x100u : 0u));
        w_u32(0x800D1590u, 1u); w_u32(0x800D1594u, (uint32)attr.mode);
        if (!(uint16)magnitude) SpuSetReverb(0);
        result = SpuSetReverbModeParam(&attr);
        if (result < 0) return (uint32)result;
        w_u32(0x80030370u, (uint16)magnitude);
        w_u32(0x80030368u, r_u32(0x800358ACu + (uint16)magnitude * 4u));
        w_u32(0x80030378u, magnitude == 7u || magnitude == 8u ? 127u : 0u);
        w_u32(0x8003037Cu, magnitude == 7u ? 127u : 0u);
        return (uint16)magnitude;
    }
    case 0x8001E42Cu:
        for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
        if ((uint16)a[0] >= 24u) return (uint32)-1;
        a[1] = (uint16)((sint32)(sint16)a[1] * 129);
        a[2] = (uint16)((sint32)(sint16)a[2] * 129);
        w_u16(0x800D0D88u + (uint16)a[0] * 16u, (uint16)a[1]);
        w_u16(0x800D0D8Au + (uint16)a[0] * 16u, (uint16)a[2]);
        w_u8(0x800D0F0Cu + (uint16)a[0], r_u8(0x800D0F0Cu + (uint16)a[0]) | 3u);
        SpuSetVoiceVolume((sint16)a[0], (sint16)a[1], (sint16)a[2]); return 0u;
    case 0x8001E52Cu:
        a[0] = (uint8)va_arg(arguments, uint32);
        if (a[0] > 1u) return 1u;
        spu_native_set_mute((sint32)a[0]);
        return a[0];
    case 0x80043DF0u:
        shell_sub_80043DF0(va_arg(arguments, uint32)); return 0u;
    case 0x8004D838u:
        return shell_sub_8004D838(va_arg(arguments, uint32));
    case 0x80043518u:
        return shell_sub_80043518();
    case 0x8001B3D8u:
        a[0] = va_arg(arguments, uint32);
        return (uint32)PadGetState((sint32)(a[0] == 1u ? 16u : a[0]));
    case 0x8001B728u:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        PadSetMainMode((sint32)a[0], (sint32)a[1], (sint32)a[2]);
        return 1u;
    case 0x8001B2D8u:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        return (uint32)PadInfoMode((sint32)a[0], (sint32)a[1], (sint32)a[2]);
    case 0x800419E8u:
        shell_sub_800419E8(va_arg(arguments, uint32)); return 0u;
    case 0x80056CB4u:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        shell_sub_80056CB4(a[0], a[1]); return 0u;
    case 0x8004D9D8u:
        return shell_sub_8004D9D8();
    case 0x8005D590u:
        shell_sub_8005D590(); return 0u;
    case 0x8005F3A4u:
        shell_sub_8005F3A4(va_arg(arguments, uint32)); return 0u;
    case 0x8003D8ECu:
        return (uint32)shell_sub_8003D8EC(va_arg(arguments, uint32));
    case 0x8003D890u:
        return (uint32)shell_sub_8003D890(va_arg(arguments, uint32));
    case 0x80060C7Cu:
        shell_sub_80060C7C(); return 0u;
    case 0x80061F00u:
        shell_sub_80061F00(); return 0u;
    case 0x8003D03Cu:
        return shell_sub_8003D03C();
    case 0x8004A8BCu:
        shell_sub_8004A8BC(); return 0u;
    case 0x8006CDF8u:
        return shell_sub_8006CDF8();
    case 0x8006C600u:
        return shell_sub_8006C600();
    case 0x8006A888u:
        return shell_sub_8006A888();
    case 0x80072D38u:
        return shell_sub_80072D38();
    case 0x8006D760u:
        return shell_sub_8006D760();
    case 0x80072038u:
        return shell_sub_80072038();
    case 0x80070960u:
        return shell_sub_80070960();
    case 0x80069C68u:
        return shell_sub_80069C68();
    case 0x80063E30u:
        return shell_sub_80063E30();
    case 0x80074D20u:
        return shell_sub_80074D20();
    case 0x80067820u:
        return shell_sub_80067820();
    case 0x800687A0u:
        return shell_sub_800687A0();
    case 0x80066830u:
        return shell_sub_80066830();
    case 0x8003D9D8u:
        return shell_sub_8003D9D8();
    case 0x8004FB30u:
        return shell_sub_8004FB30();
    case 0x8003FD60u:
        return shell_sub_8003FD60();
    case 0x80042C58u:
        return shell_sub_80042C58();
    case 0x8003ECE0u:
        return shell_sub_8003ECE0();
    case 0x80046690u:
        return shell_sub_80046690();
    case 0x8005A720u:
        return shell_sub_8005A720();
    case 0x80056230u:
        return shell_sub_80056230();
    case 0x8005C120u:
        return shell_sub_8005C120();
    case 0x80057B50u:
        return shell_sub_80057B50();
    case 0x8006FA48u:
        return shell_sub_8006FA48();
    case 0x8004E3E0u:
        return shell_sub_8004E3E0();
    case 0x80063510u:
        for (i = 0u; i < 7u; ++i) a[i] = va_arg(arguments, uint32);
        shell_sub_80063510(a[0], (sint32)a[1], a[2], a[3], a[4], a[5], a[6]); return 0u;
    case 0x8003BE58u:
        return shell_sub_8003BE58();
    case 0x8003BEE8u:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        return shell_sub_8003BEE8(a[0], (sint32)a[1]);
    case 0x8003C4ACu:
        return shell_sub_8003C4AC(va_arg(arguments, uint32));
    case 0x80063A3Cu:
        shell_sub_80063A3C(va_arg(arguments, uint32));
        return 0u;
    case 0x80063AA8u:
        shell_sub_80063AA8(va_arg(arguments, uint32));
        return 0u;
    case 0x8004DE8Cu:
        return shell_sub_8004DE8C(va_arg(arguments, uint32));
    case 0x8004D794u:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32); a[2] = va_arg(arguments, uint32);
        shell_sub_8004D794(a[0], a[1], a[2]); return 0u;
    case 0x80053418u:
        return shell_sub_80053418(va_arg(arguments, uint32));
    case 0x8004DE14u:
        shell_sub_8004DE14(); return 0u;
    case 0x80058F8Cu:
        shell_sub_80058F8C(va_arg(arguments, uint32)); return 0u;
    case 0x8004E16Cu:
        shell_sub_8004E16C(va_arg(arguments, uint32)); return 0u;
    case 0x80060D1Cu:
        shell_sub_80060D1C(); return 0u;
    case 0x80063070u:
        return shell_sub_80063070(va_arg(arguments, uint32));
    case 0x800630CCu:
        shell_sub_800630CC(); return 0u;
    case 0x80063124u:
        shell_sub_80063124(); return 0u;
    case 0x8005F5E4u:
        return shell_sub_8005F5E4(va_arg(arguments, uint32));
    case 0x800492D0u:
        return shell_sub_800492D0();
    case 0x8001AB5Cu:
        return sub_8001AB5C(va_arg(arguments, uint32));
    case 0x8001AB2Cu:
        psx_root_counter_bind(0x8002E128u, 0x8002E130u);
        return (uint32)StartRCnt(va_arg(arguments, uint32));
    case 0x80019640u:
        return moh_native_cd_read_callback(va_arg(arguments, uint32));
    case 0x8001D140u:
        return sub_8001D140(va_arg(arguments, uint32));
    case 0x8001C9A8u:
        return (uint32)rsin((sint32)va_arg(arguments, uint32));
    case 0x8001BC68u: return sub_8001BC68(va_arg(arguments, uint32));
    case 0x80016A40u:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        return VectorNormalSS((SVECTOR *)moh_draft_guest_pointer(a[0], 6u),
            (SVECTOR *)moh_draft_guest_pointer(a[1], 6u));
    case 0x8001676Cu:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        RotMatrix((SVECTOR *)moh_draft_guest_pointer(a[0], 6u),
            (MATRIX *)moh_draft_guest_pointer(a[1], 18u)); return a[1];
    case 0x8001DAE0u:
        for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
        for (i = 0u; i < 5u; ++i) xport_gte_write_control(i, r_u32(a[0] + i * 4u));
        xport_gte_write_data(0u, r_u32(a[1])); xport_gte_write_data(1u, r_u32(a[1] + 4u));
        xport_gte_execute(0x486012u);
        a[3] = xport_gte_read_data(9u); a[4] = xport_gte_read_data(10u); a[5] = xport_gte_read_data(11u);
        w_u16(a[2], a[3]); w_u16(a[2] + 2u, a[4]); w_u16(a[2] + 4u, a[5]); return a[2];
    case 0x8001BC28u:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        TransposeMatrix((MATRIX *)moh_draft_guest_pointer(a[0], 18u),
            (MATRIX *)moh_draft_guest_pointer(a[1], 18u)); return a[1];
    case 0x80013AE4u:
        for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
        moh_native_model_transform(a[0], a[1], a[2], a[3]); return 0u;
    case 0x80013E58u:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        return sub_80013E58(a[0], a[1]);
    case 0x80013F98u:
        for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
        return sub_80013F98(a[0], a[1], a[2]);
    case 0x8001536Cu:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        return (uint32)ratan2((sint32)a[0], (sint32)a[1]);
    case 0x80016188u:
        return SquareRoot0((sint32)va_arg(arguments, uint32));
    case 0x80016168u:
        a[0] = va_arg(arguments, uint32);
        w_u8(a[0] + 3u, 4u); w_u8(a[0] + 7u, 0x50u);
        return 0u;
    case 0x8001C908u:
        return (uint32)rcos((sint32)va_arg(arguments, uint32));
    case 0x8001CB6Cu:
        /* Native overlay teardown clears callbacks while retaining the frame clock */
        return ResetCallbackPSX();
    case 0x80016D40u:
        return sub_80016D40(va_arg(arguments, uint32));
    case 0x800158ACu:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        if (!a[0] || (sint32)a[2] <= 0) return 0u;
        memset(moh_draft_guest_pointer(a[0], a[2]), (uint8)a[1], a[2]);
        return a[0];
    case 0x8003BE8Cu:
        return shell_sub_8003BE8C(va_arg(arguments, uint32));
    case 0x8003C028u:
        return shell_sub_8003C028(va_arg(arguments, uint32));
    case 0x8007699Cu:
        shell_sub_8007699C(); return 0u;
    case 0x80078F4Cu:
        shell_sub_80078F4C(); return 0u;
    case 0x80079EF8u:
        return shell_sub_80079EF8();
    case 0x80078048u:
        shell_sub_80078048(); return 0u;
    case 0x80058FB0u:
        shell_sub_80058FB0(va_arg(arguments, uint32)); return 0u;
    case 0x8003F038u:
        shell_sub_8003F038(va_arg(arguments, uint32)); return 0u;
    case 0x800496A0u:
        return shell_sub_800496A0(va_arg(arguments, uint32));
    case 0x800498E8u:
        return shell_sub_800498E8(va_arg(arguments, uint32));
    case 0x80049A6Cu:
        return shell_sub_80049A6C(va_arg(arguments, uint32));
    case 0x80049B54u:
        return shell_sub_80049B54(va_arg(arguments, uint32));
    case 0x80049DA8u:
        return shell_sub_80049DA8(va_arg(arguments, uint32));
    case 0x80049C60u:
        return shell_sub_80049C60(va_arg(arguments, uint32));
    case 0x800204C8u:
        SsEnd(); return 0u;
    case 0x8001D160u:
        moh_native_spu_quit(); return 0u;
    case 0x800269C8u:
        moh_native_spu_quit(); return 0u;
    case 0x8001CFB4u:
        return sub_8001CFB4();
    case 0x80016D30u:
        return sub_80016D30();
    case 0x80022E54u:
        return sub_80022E54();
    case 0x8002666Cu:
        return sub_8002666C(va_arg(arguments, uint32));
    case 0x8001DFB0u:
        sub_8001DFB0(va_arg(arguments, uint32)); return 0u;
    case 0x8001C714u:
        sub_8001C714(va_arg(arguments, uint32)); return 0u;
    case 0x8002669Cu:
        sub_8002669C(va_arg(arguments, uint32)); return 0u;
    case 0x800266ACu:
        sub_800266AC(va_arg(arguments, uint32)); return 0u;
    case 0x80016A70u:
        for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
        return sub_80016A70(a[0], a[1], a[2], a[3]);
    case 0x8001716Cu:
        for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
        return sub_8001716C(a[0], a[1], a[2]);
    case 0x80049324u:
        return shell_sub_80049324(va_arg(arguments, uint32));
    case 0x800453F8u:
        shell_sub_800453F8(); return 0u;
    case 0x80045430u:
        shell_sub_80045430(); return 0u;
    case 0x8005FB50u:
        shell_sub_8005FB50(va_arg(arguments, uint32)); return 0u;
    case 0x80045234u:
        return shell_sub_80045234();
    case 0x8004CDECu:
        shell_sub_8004CDEC(va_arg(arguments, uint32)); return 0u;
    case 0x80053358u:
        return shell_sub_80053358(va_arg(arguments, uint32));
    case 0x8003EF24u:
        shell_sub_8003EF24(va_arg(arguments, uint32)); return 0u;
    case 0x8003F068u:
        shell_sub_8003F068(va_arg(arguments, uint32)); return 0u;
    case 0x8003F014u:
        shell_sub_8003F014(va_arg(arguments, uint32)); return 0u;
    case 0x8003FA7Cu:
        shell_sub_8003FA7C(va_arg(arguments, uint32)); return 0u;
    case 0x8003FAA0u:
        return shell_sub_8003FAA0(va_arg(arguments, uint32));
    case 0x8003F6ECu:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        shell_sub_8003F6EC(a[0], a[1]); return 0u;
    case 0x80062108u:
        shell_sub_80062108(va_arg(arguments, uint32)); return 0u;
    case 0x8003CA30u:
        return shell_sub_8003CA30(va_arg(arguments, uint32));
    case 0x8005A430u:
        return shell_sub_8005A430(va_arg(arguments, uint32));
    case 0x8005F258u:
        return shell_sub_8005F258(va_arg(arguments, uint32));
    case 0x80059030u:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        shell_sub_80059030(a[0], a[1]); return 0u;
    case 0x80053C8Cu:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        return shell_sub_80053C8C(a[0], a[1]);
    case 0x800172D8u:
        return moh_native_draw_otag(va_arg(arguments, uint32));
    case 0x8003CCECu:
        for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
        shell_sub_8003CCEC(a[0], a[1], a[2]); return 0u;
    case 0x8004BF74u:
        return shell_sub_8004BF74(va_arg(arguments, uint32));
    case 0x8004CAF4u:
        shell_sub_8004CAF4(va_arg(arguments, uint32)); return 0u;
    case 0x8004C51Cu:
    case 0x8004C710u:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        if (address == 0x8004C51Cu) return shell_sub_8004C51C(a[0], a[1]);
        shell_sub_8004C710(a[0], a[1]); return 0u;
    case 0x8004D508u:
        shell_sub_8004D508(va_arg(arguments, uint32)); return 0u;
    case 0x8004CFB4u:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        shell_sub_8004CFB4(a[0], a[1]); return 0u;
    case 0x8004D694u:
        shell_sub_8004D694(va_arg(arguments, uint32)); return 0u;
    case 0x8004BDB4u:
        return shell_sub_8004BDB4(va_arg(arguments, uint32));
    case 0x8005A1ECu:
        shell_sub_8005A1EC(va_arg(arguments, uint32)); return 0u;
    case 0x8004E1B8u:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        shell_sub_8004E1B8(a[0], a[1]); return 0u;
    case 0x8004DD9Cu:
        shell_sub_8004DD9C(); return 0u;
    case 0x8001D590u:
    {
        float number = (float)(sint32)va_arg(arguments, uint32);
        uint32 bits;
        memcpy(&bits, &number, sizeof(bits));
        return bits;
    }
    case 0x8001D3F0u:
    case 0x8001D1F0u:
    {
        float left, right, result;
        uint32 bits;
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        if (address == 0x8001D3F0u) {
            if (!(a[1] & 0x7FFFFFFFu)) return a[0];
            if (!(a[0] & 0x7FFFFFFFu)) return a[1] ^ 0x80000000u;
        } else {
            if (!(a[1] & 0x7FFFFFFFu)) return ((a[0] ^ a[1]) & 0x80000000u) ? 0x4F800000u : 0x4F000000u;
            if (!(a[0] & 0x7FFFFFFFu)) return (a[0] ^ a[1]) & 0x80000000u;
        }
        memcpy(&left, &a[0], sizeof(left)); memcpy(&right, &a[1], sizeof(right));
        /* Native float arithmetic replaces the SDK software implementation */
        /* TODO Original rounding at ties may differ from the host SDK adapter */
        result = address == 0x8001D3F0u ? left - right : left / right;
        memcpy(&bits, &result, sizeof(bits));
        return bits;
    }
    case 0x8001E11Cu:
    {
        uint32 bits = va_arg(arguments, uint32), exponent = (bits >> 23u) & 255u, magnitude;
        if (!(bits & 0x7FFFFFFFu)) return 0u;
        if (exponent > 157u) return bits & 0x80000000u ? 0x80000000u : 0x7FFFFFFFu;
        if (exponent < 126u) return 0u;
        magnitude = ((bits & 0x7FFFFFu) | 0x800000u) << 7u;
        magnitude >>= 157u - exponent;
        return bits & 0x80000000u ? 0u - magnitude : magnitude;
    }
    case 0x8003C110u:
        return shell_sub_8003C110(va_arg(arguments, uint32));
    case 0x8003C30Cu:
    case 0x8003C448u:
        for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
        if (address == 0x8003C30Cu) shell_sub_8003C30C(a[0], a[1], (sint32)a[2], a[3]);
        else shell_sub_8003C448(a[0], a[1], (sint32)a[2], a[3]);
        return 0u;
    case 0x80053150u:
        return shell_sub_80053150(va_arg(arguments, uint32));
    case 0x80052FE8u:
        return shell_sub_80052FE8(va_arg(arguments, uint32));
    case 0x80051EF4u:
        for (i = 0u; i < 6u; ++i) a[i] = va_arg(arguments, uint32);
        return shell_sub_80051EF4(a[0], a[1], a[2], a[3], a[4], a[5]);
    case 0x80051EC0u:
        for (i = 0u; i < 5u; ++i) a[i] = va_arg(arguments, uint32);
        return moh_draft_missing_call(0x80051EF4u, a[0], a[1], (uint32)(sint32)(sint16)a[2], a[3], a[4], 1u);
    case 0x8004B2DCu:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32); a[2] = va_arg(arguments, uint32);
        return shell_sub_8004B2DC(a[0], a[1], a[2]);
    case 0x8004AAA4u:
    case 0x8004AC98u:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32); a[2] = va_arg(arguments, uint32);
        if (address == 0x8004AC98u) return shell_sub_8004AC98(a[0], a[1], a[2]);
        shell_sub_8004AAA4(a[0], a[1], a[2]); return 0u;
    case 0x8004B10Cu:
    case 0x8004B974u:
    case 0x8004C21Cu:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32); a[2] = va_arg(arguments, uint32);
        if (address == 0x8004B10Cu) return shell_sub_8004B10C(a[0], a[1], a[2]);
        if (address == 0x8004B974u) return shell_sub_8004B974(a[0], a[1], a[2]);
        return shell_sub_8004C21C(a[0], a[1], a[2]);
    case 0x80058CC8u:
        shell_sub_80058CC8(va_arg(arguments, uint32)); return 0u;
    case 0x80058FE0u:
        shell_sub_80058FE0(va_arg(arguments, uint32)); return 0u;
    case 0x8004E0BCu:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        shell_sub_8004E0BC(a[0], a[1]); return 0u;
    case 0x80058928u:
        return shell_sub_80058928();
    case 0x8006349Cu:
        return shell_sub_8006349C();
    case 0x80049E90u:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        return shell_sub_80049E90(a[0], a[1]);
    case 0x80041790u:
        return shell_sub_80041790();
    case 0x8004DC40u:
        shell_sub_8004DC40(); return 0u;
    case 0x80020218u:
        return (uint32)spu_native_vab_address((sint16)va_arg(arguments, uint32));
    case 0x80062F50u:
    case 0x80062F70u:
        return shell_sub_80062F70(va_arg(arguments, uint32));
    case 0x80045328u:
        shell_sub_80045328(va_arg(arguments, uint32)); return 0u;
    case 0x8001552Cu:
        a[0] = r_u32(0x800CA2A4u) * 0x41C64E6Du + 0x3039u;
        w_u32(0x800CA2A4u, a[0]); return (a[0] >> 16u) & 0x7FFFu;
    case 0x8001555Cu:
        a[0] = va_arg(arguments, uint32);
        w_u32(0x800CA2A4u, a[0]); srandPSX(a[0]); return 0u;
    case 0x80020968u:
    {
        SpuCommonAttr attr = {0};
        SpuExtAttr *serial;
        for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
        if ((uint8)a[0] > 1u || (uint8)a[1] > 1u)
            moh_runtime_error("Invalid serial audio switch", address);
        serial = (uint8)a[0] ? &attr.ext : &attr.cd;
        attr.mask = ((uint8)a[1] ? 0x100u : 0x200u) << ((uint8)a[0] ? 4u : 0u);
        if ((uint8)a[1]) serial->mix = (sint8)a[2];
        else serial->reverb = (sint8)a[2];
        SpuSetCommonAttr(&attr);
        return 0u;
    }
    case 0x8001E2ECu:
    {
        SpuCommonAttr attr = {0};
        SpuExtAttr *serial;
        sint32 left, right;
        for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
        if ((uint8)a[0] > 1u) moh_runtime_error("Invalid serial audio volume", a[0]);
        serial = (uint8)a[0] ? &attr.ext : &attr.cd;
        attr.mask = (uint8)a[0] ? 0xC00u : 0xC0u;
        left = (sint16)a[1]; right = (sint16)a[2];
        if (left >= 128) left = 127;
        if (right >= 128) right = 127;
        serial->volume.left = (sint16)(left * 258);
        serial->volume.right = (sint16)(right * 258);
        SpuSetCommonAttr(&attr);
        return 0u;
    }
    case 0x80063834u:
        shell_sub_80063834(va_arg(arguments, uint32)); return 0u;
    case 0x800637FCu:
        shell_sub_800637FC(); return 0u;
    case 0x8001E4BCu:
        w_u16(0x800D158Cu, 1u); return 0u;
    case 0x8001E4CCu:
        w_u16(0x800D158Cu, 0u); return 0u;
    case 0x8001F70Cu:
    {
        SpuReverbAttr attr = {0};
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        attr.mask = 6u;
        attr.depth.left = (sint16)(((sint32)(sint16)a[0] * 32767) / 127);
        attr.depth.right = (sint16)(((sint32)(sint16)a[1] * 32767) / 127);
        w_u32(0x800D1590u, 6u);
        w_u16(0x800D1598u, (uint16)attr.depth.left); w_u16(0x800D159Au, (uint16)attr.depth.right);
        SpuSetReverbDepth(&attr);
        return 0u;
    }
    case 0x8001FDB8u:
        a[0] = va_arg(arguments, uint32);
        if ((a[0] & ~0x1000u) != 1u) moh_runtime_error("Unsupported native sequencer tick mode", a[0]);
        w_u32(0x8002FF60u, (a[0] & 0x1000u) != 0u);
        w_u32(0x8002FF5Cu, r_u8(0x8002DF18u) ? 60u : 5u);
        w_u32(0x800D094Cu, 60u);
        SsSetTickMode(SS_TICK60);
        return r_u8(0x8002DF18u) ? 60u : 5u;
    case 0x8001E4DCu:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        SsSetMVol((sint16)a[0], (sint16)a[1]); return 0u;
    case 0x80020138u:
        SsStart(); return 0u;
    case 0x800201F8u:
        return (uint32)SpuSetReverb(0);
    case 0x8001FC58u:
        return (uint32)SpuSetReverb(1);
    case 0x80021B80u:
        a[0] = va_arg(arguments, uint32);
        if (a[0] > 1u) moh_runtime_error("Unsupported SPU initialization mode", a[0]);
        if (a[0]) SpuInitHot(); else SpuInit();
        spu_bind_reverb_presets((const uint32 *)moh_draft_guest_pointer(0x800358ACu, 40u),
            (const uint8 *)moh_draft_guest_pointer(0x8003594Cu, 680u), 3u);
        w_u32(0x8003035Cu, 0u); w_u32(0x800302D8u, 0u);
        w_u32(0x800302DCu, 0u); w_u16(0x800302D4u, 0u);
        w_u32(0x800302E0u, 2u); w_u32(0x800302E4u, 3u);
        w_u32(0x800302E8u, 8u); w_u32(0x800302ECu, 7u);
        w_u32(0x800302F0u, 1u); w_u32(0x800302F4u, 0u);
        xport_bind_native_spu_transfer();
        return 0u;
    case 0x8001AF68u:
        return sub_8001AF68();
    case 0x80015B68u:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        level_native_master_volume(a[0], a[1]); return 0u;
    case 0x8001586Cu:
        level_native_cd_mix(va_arg(arguments, uint32)); return 0u;
    case 0x80015BE8u:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        level_native_cd_volume(a[0], a[1]); return 0u;
    case 0x800195D0u:
        return (uint32)moh_native_cd_read_status(va_arg(arguments, uint32));
    case 0x8001DD80u:
        a[0] = va_arg(arguments, uint32);
        if (a[0] == 0u || a[0] == 3u) {
            if (!spu_mmio_read(NULL, 0x1F801DAAu, 2u, &a[1]) ||
                !spu_mmio_write(NULL, 0x1F801DAAu, 2u, a[1] & ~0x40u, 3u))
                moh_runtime_error("Native SPU IRQ disable unavailable", address);
        }
        if (a[0] == 1u || a[0] == 3u) {
            if (!spu_mmio_read(NULL, 0x1F801DAAu, 2u, &a[1]) ||
                !spu_mmio_write(NULL, 0x1F801DAAu, 2u, a[1] | 0x40u, 3u))
                moh_runtime_error("Native SPU IRQ enable unavailable", address);
        }
        return a[0];
    case 0x8001C698u:
        a[0] = va_arg(arguments, uint32); a[1] = r_u32(0x800302F8u);
        if (a[0] != a[1]) {
            w_u32(0x800302F8u, a[0]);
            sub_8001CAA8(9u, a[0]);
        }
        return a[1];
    case 0x8001DF40u:
        a[0] = va_arg(arguments, uint32);
        if (r_u32(0x800302F4u) != a[0]) w_u32(0x800302F4u, a[0]);
        return 0u;
    case 0x80015C08u:
        for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
        SpuSetVoiceVolume((sint32)a[0], (sint16)(a[1] & 0x7FFFu), (sint16)(a[2] & 0x7FFFu));
        return 0u;
    case 0x800157BCu:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        SpuSetVoicePitch((sint32)a[0], (uint16)a[1]); return 0u;
    case 0x80015D38u:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        a[1] = sub_8002178C(0xFFFFFFFFu, a[1]);
        SpuSetVoiceStartAddr((sint32)a[0], a[1] << 3); return 0u;
    case 0x8001559Cu:
    {
        SpuVoiceAttr attr = {0};
        uint32 sustain = 0x4000u;
        for (i = 0u; i < 9u; ++i) a[i] = va_arg(arguments, uint32);
        if (a[0] >= 24u) moh_runtime_error("Invalid native ADSR voice", a[0]);
        attr.voice = 1u << a[0];
        attr.mask = SPU_VOICE_ADSR_ADSR1 | SPU_VOICE_ADSR_ADSR2;
        attr.adsr1 = (uint16)(((a[1] & 127u) << 8) | ((a[2] & 15u) << 4) | (a[5] & 15u) | ((a[6] == 5u) << 15));
        if (a[7] == 1u) sustain = 0u;
        else if (a[7] == 5u) sustain = 0x8000u;
        else if (a[7] == 7u) sustain = 0xC000u;
        if (a[8] == 7u) sustain |= 32u;
        attr.adsr2 = (uint16)(((a[3] & 127u) << 6) | (a[4] & 31u) | sustain);
        SpuSetVoiceAttr(&attr); return 0u;
    }
    case 0x8001573Cu:
        a[0] = va_arg(arguments, uint32);
        return (sint32)a[0] < 0 ? 0u - a[0] : a[0];
    case 0x8001DB60u:
        return sub_8001DB60(va_arg(arguments, uint32));
    case 0x8001AF98u: {
        uint32 rotation = va_arg(arguments, uint32), matrix = va_arg(arguments, uint32);
        return sub_8001AF98(rotation, matrix);
    }
    case 0x800165D8u: {
        uint32 matrix = va_arg(arguments, uint32), scale = va_arg(arguments, uint32);
        return sub_800165D8(matrix, scale);
    }
    case 0x800158DCu:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32) & 0xFFFFFFu;
        if (a[0] <= 1u) {
            /* Native SPU owns key events; retain the SDK key bitmap */
            SpuSetKey((sint32)a[0], a[1]);
            a[2] = r_u32(0x80030358u);
            w_u32(0x80030358u, a[0] ? a[2] | a[1] : a[2] & ~a[1]);
        }
        return 0u;
    case 0x800156DCu:
        return SpuSetTransferStartAddr(va_arg(arguments, uint32));
    case 0x8001556Cu:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        return sub_8001556C(a[0], a[1]);
    case 0x80016718u: return sub_80016718(va_arg(arguments, uint32));
    case 0x80016248u:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        return sub_80016248((sint32)a[0], a[1]);
    case 0x80016A10u:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32); return sub_80016A10(a[0], a[1]);
    case 0x80016578u:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32); a[2] = va_arg(arguments, uint32); return sub_80016578(a[0], a[1], a[2]);
    case 0x800163E8u:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32); a[2] = va_arg(arguments, uint32); return sub_800163E8(a[0], a[1], a[2]);
    case 0x80016218u:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        return sub_80016218(a[0], a[1]);
    case 0x80016B30u:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32); a[2] = va_arg(arguments, uint32);
        return sub_80016B30(a[0], a[1], a[2]);
    case 0x80015C88u:
        return (uint32)SpuIsTransferCompleted((sint32)va_arg(arguments, uint32));
    case 0x80020268u:
    {
        static const uint8 silence[1024] = {0};
        uint32 remaining = va_arg(arguments, uint32), written = 0u;
        uint32 destination = (uint32)r_u16(0x800302D4u) << (r_u32(0x800302E4u) & 31u);
        do
        {
            uint32 bytes = remaining > 1024u ? 1024u : (remaining + 63u) & ~63u;
            uint32 first = bytes;
            if (first > 0x80000u - (destination & 0x7FFFFu)) first = 0x80000u - (destination & 0x7FFFFu);
            if (!spu_upload(destination & 0x7FFFFu, silence, first) ||
                (first < bytes && !spu_upload(0u, silence + first, bytes - first)))
                moh_runtime_error("Native SPU memory clear failed", destination);
            written += bytes;
            if (remaining <= 1024u) break;
            remaining -= 1024u; destination += 1024u;
        } while (1);
        w_u32(0x800302F0u, 1u);
        return written;
    }
    case 0x80056C58u:
        shell_sub_80056C58(); return 0u;
    case 0x800631D0u:
        shell_sub_800631D0(); return 0u;
    case 0x8003D3A0u:
        for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
        return shell_sub_8003D3A0(a[0], a[1], a[2]);
    case 0x8003D3E8u:
        for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
        return shell_sub_8003D3E8(a[0], a[1], a[2]);
    case 0x8003D500u:
        return shell_sub_8003D500(va_arg(arguments, uint32));
    case 0x8003D594u:
        for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
        return (uint32)shell_sub_8003D594(a[0], a[1], a[2]);
    case 0x800767E0u:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        shell_sub_800767E0(a[0], a[1]); return 0u;
    case 0x8005F580u:
        for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
        return shell_sub_8005F580(a[0], a[1], a[2]);
    case 0x8005F614u:
        for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
        return shell_resource_find(a[0], a[1], (uint32 *)moh_draft_guest_pointer(a[2], 4u));
    case 0x8001E2ACu:
        a[0] = va_arg(arguments, uint32) & 255u;
        return (uint32)(sint32)(sint8)((r_u8(0x8002F29Du + a[0]) & 1u) ? a[0] + 32u : a[0]);
    case 0x8006118Cu:
        shell_sub_8006118C(va_arg(arguments, uint32)); return 0u;
    case 0x800619C0u:
        shell_sub_800619C0(va_arg(arguments, uint32)); return 0u;
    case 0x80077F10u:
        shell_sub_80077F10(); return 0u;
    case 0x80078C4Cu:
        shell_sub_80078C4C(); return 0u;
    case 0x80078CB8u:
        return shell_sub_80078CB8();
    case 0x80079E20u:
        shell_native_card_init(va_arg(arguments, uint32)); return 0u;
    case 0x80079EB0u:
        return shell_native_card_start();
    case 0x80045390u:
        shell_sub_80045390(); return 0u;
    case 0x80078018u:
        shell_sub_80078018(); return 0u;
    case 0x8007692Cu:
        shell_sub_8007692C(); return 0u;
    case 0x8005F89Cu:
        for (i = 0u; i < 5u; ++i) a[i] = va_arg(arguments, uint32);
        return shell_sub_8005F89C(a[0], a[1], a[2], a[3], a[4]);
    case 0x8005F7A0u:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return shell_sub_8005F7A0(a[0], a[1]);
    case 0x800451F0u:
        return shell_sub_800451F0(va_arg(arguments, uint32));
    case 0x80045260u:
        return shell_sub_80045260(va_arg(arguments, uint32));
    case 0x8003CFD8u:
        return shell_sub_8003CFD8(va_arg(arguments, uint32));
    case 0x80019E20u:
    {
        FILE *file = moh_open_asset(r_u32(0x800385ACu));
        if (!file) moh_runtime_error("SHELL.BIN asset unavailable", address);
        fclose(file);
        moh_files_ready = 1u;
        return 1u;
    }
    case 0x80019F64u:
        return moh_native_cd_reset();
    case 0x8001B228u:
    {
        uint32 first = va_arg(arguments, uint32);
        uint32 second = va_arg(arguments, uint32);
        uint32 port, context, packet, byte;
        w_u32(0x8003476Cu, 0u);
        w_u32(0x80034780u, 0u);
        memset(moh_draft_guest_pointer(0x800D0AA4u, 480u), 0, 480u);
        w_u32(0x80034768u, 0x800D0AA4u);
        for (port = 0u; port < 2u; ++port)
        {
            context = 0x800D0AA4u + port * 240u;
            packet = port ? second : first;
            w_u32(context + 12u, 0u);
            w_u32(context + 16u, context);
            w_u32(context + 48u, packet);
            for (byte = 93u; byte < 99u; ++byte) w_u8(context + byte, 255u);
        }
        if (!InitPAD(first, 36u, second, 36u))
            moh_runtime_error("Native PAD initialization failed", first);
        w_u32(0x8003476Cu, 1u);
        return 1u;
    }
    case 0x8001B4A8u:
        if (!StartPAD()) moh_runtime_error("Native PAD start failed", address);
        w_u32(0x800D0AA0u, 0u);
        w_u32(0x800D0A9Cu, 0u);
        w_u32(0x8003476Cu, 1u);
        return 1u;
    case 0x8001B574u:
        PadStopCom();
        return 0u;
    case 0x80026708u:
    {
        uint32 index;
        /* Native transfer completion delivers callbacks directly */
        moh_dma_pending = 0u;
        for (index = 0u; index < 8u; ++index) w_u32(0x80035000u + 4u * index, 0u);
        return 0x800268D4u;
    }
    case 0x800268D4u:
    {
        uint32 channel = va_arg(arguments, uint32);
        uint32 callback = va_arg(arguments, uint32);
        uint32 previous;
        if (channel >= 7u) moh_runtime_error("Invalid DMA callback slot", channel);
        previous = r_u32(0x80035000u + channel * 4u);
        w_u32(0x80035000u + channel * 4u, callback);
        return previous;
    }
    case 0x80026754u:
    {
        uint32 channel, pending, callback;
        while ((pending = moh_dma_pending) != 0u)
        {
            for (channel = 0u; channel < 7u; ++channel)
            {
                if (!(pending & (1u << channel))) continue;
                moh_dma_pending &= ~(1u << channel);
                callback = r_u32(0x80035000u + channel * 4u);
                if (callback) moh_draft_indirect_call(callback);
            }
        }
        return 0u;
    }
    case 0x8001D1D8u:
        /* Native CD services have no BIOS CD interrupt handler to remove */
        return 0u;
    case 0x80022390u:
    {
        uint32 index;
        /* Native VSync supplies frame timing instead of a hardware timer */
        w_u32(0x800303ECu, 0u);
        for (index = 0u; index < 8u; ++index) w_u32(0x800303CCu + 4u * index, 0u);
        sub_8001CAA8(0u, 0x800223E8u);
        return 0x80022454u;
    }
    case 0x800223E8u:
    {
        uint32 index, callback;
        w_u32(0x800303ECu, r_u32(0x800303ECu) + 1u);
        for (index = 0u; index < 8u; ++index)
        {
            callback = r_u32(0x800303CCu + 4u * index);
            if (callback) moh_draft_indirect_call(callback);
        }
        return 0u;
    }
    case 0x80022454u:
    {
        uint32 index = va_arg(arguments, uint32);
        uint32 callback = va_arg(arguments, uint32);
        uint32 previous;
        if (index >= 8u) moh_runtime_error("Invalid VBlank callback slot", index);
        previous = r_u32(0x800303CCu + 4u * index);
        w_u32(0x800303CCu + 4u * index, callback);
        return previous;
    }
    case 0x80022510u:
        return ChangeClearPAD(va_arg(arguments, uint32));
    case 0x80022520u:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return ChangeClearRCnt(a[0], a[1]);
    case 0x80015DB8u:
        return (uint32)VSync((sint32)va_arg(arguments, uint32));
    case 0x800266F8u:
        moh_native_irq_install(va_arg(arguments, uint32));
        return 0u;
    case 0x8001E07Cu:
        return moh_startup_initializers();
    case 0x80015B28u:
    {
        uint32 format_address = va_arg(arguments, uint32);
        const char *format = (const char *)moh_draft_guest_pointer(format_address, 1u);
        char message[2048];
        sint32 count;
        /* TODO Convert guest string arguments before passing them to the host CRT */
        if (strstr(format, "%s") || strstr(format, "%p") || strstr(format, "%n"))
            moh_runtime_error("Missing guest printf argument adapter", address);
        count = vsnprintf(message, sizeof(message), format, arguments);
        OutputDebugStringA(message);
        return (uint32)count;
    }
    case 0x8001ABC8u:
    {
        uint32 destination = va_arg(arguments, uint32), source = va_arg(arguments, uint32);
        uint32 output = destination, input = source;
        uint8 byte;
        if (!destination || !source) return 0u;
        while (r_u8(output)) ++output;
        while (r_u8(input)) ++input;
        if (output == input) return 0u;
        do {
            byte = r_u8(source++);
            w_u8(output++, byte);
        } while (byte);
        return destination;
    }
    case 0x8001AD48u:
    {
        uint32 destination = va_arg(arguments, uint32);
        uint32 source = va_arg(arguments, uint32);
        uint32 output = destination;
        uint8 byte;
        if (!destination || !source) return 0u;
        do {
            byte = r_u8(source++);
            w_u8(output++, byte);
        } while (byte);
        return destination;
    }
    case 0x800154ECu:
    {
        uint32 destination = va_arg(arguments, uint32);
        uint32 source = va_arg(arguments, uint32);
        uint32 bytes = va_arg(arguments, uint32);
        uint32 output = destination;
        if (!destination) return 0u;
        while ((sint32)bytes > 0) {
            uint32 byte = r_u8(source);
            ++source;
            --bytes;
            w_u8(output++, byte);
        }
        return destination;
    }
    case 0x8001575Cu:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        if (a[1] > 0x7EFF0u) a[1] = 0x7EFF0u;
        return SpuWrite(moh_draft_guest_pointer(a[0], a[1]), a[1]);
    }
    case 0x80015A9Cu:
        a[0] = va_arg(arguments, uint32);
        for (i = 0u; i < 24u; ++i) w_u8(a[0] + i, (uint8)SpuGetKeyStatus(1u << i));
        return 0u;
    case 0x8001582Cu:
    {
        return (uint32)moh_native_sound_init();
    }
    case 0x80016B60u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)moh_native_reset_graph(a[0]);
    }
    case 0x80016CD4u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_80016CD4(a[0]);
    }
    case 0x80016DA0u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_80016DA0(a[0]);
    }
    case 0x80016E38u:
    {
        a[0] = va_arg(arguments, uint32);
        return moh_native_draw_sync(a[0]);
    }
    case 0x80016EA0u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_80016EA0(a[0], a[1]);
    }
    case 0x80016FBCu:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        a[3] = va_arg(arguments, uint32);
        return (uint32)sub_80016FBC(a[0], a[1], a[2], a[3]);
    }
    case 0x8001704Cu:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_8001704C(a[0], a[1]);
    }
    case 0x800170ACu:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        return (uint32)sub_800170AC(a[0], a[1], a[2]);
    }
    case 0x8001722Cu:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        ClearOTagR((uint32 *)moh_draft_guest_pointer(a[0], a[1] * 4u), (sint32)a[1]);
        w_u32(0x8002DFD8u, 0x04000000u | (0x8002DFC4u & 0xFFFFFFu));
        w_u32(a[0], 0x8002DFD8u & 0xFFFFFFu);
        return a[0];
    case 0x8001BA68u:
        SetGeomScreen((sint32)va_arg(arguments, uint32)); return 0u;
    case 0x80010B60u:
        for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
        return sub_80010B60(a[0], a[1], a[2], a[3]);
    case 0x80010000u:
        for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
        return sub_80010000(a[0], a[1], a[2], a[3]);
    case 0x80010A7Cu:
        for (i = 0; i < 5; ++i) a[i] = va_arg(arguments, uint32);
        return sub_80010A7C(a[0], a[1], a[2], a[3], a[4]);
    case 0x80011214u:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        return sub_80011214(a[0], a[1]);
    case 0x80011AB0u: return sub_80011AB0(va_arg(arguments, uint32));
    case 0x800114B0u:
        for (i = 0u; i < 3u; ++i) a[i] = va_arg(arguments, uint32);
        sub_800114B0(a[0], a[1], a[2]); return 0u;
    case 0x8001BA18u:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        return GetClut((sint32)a[0], (sint32)a[1]);
    case 0x8001B9F8u:
        a[0] = va_arg(arguments, uint32);
        w_u8(a[0] + 3u, 9u); w_u8(a[0] + 7u, 0x2Cu); return 0u;
    case 0x8001BA38u:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        a[2] = r_u8(a[0] + 7u);
        w_u8(a[0] + 7u, a[1] ? a[2] | 2u : a[2] & 0xFDu); return 0u;
    case 0x8001C768u:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        RotMatrixZ((sint32)a[0], (MATRIX *)moh_draft_guest_pointer(a[1], 32u));
        return a[1];
    case 0x8001BD08u:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        MulMatrix0((MATRIX *)moh_draft_guest_pointer(a[0], 20u),
            (MATRIX *)moh_draft_guest_pointer(a[1], 20u),
            (MATRIX *)moh_draft_guest_pointer(a[1], 20u));
        return a[1];
    case 0x8001B968u:
        for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
        return GetTPage((sint32)a[0], (sint32)a[1], (sint32)a[2], (sint32)a[3]);
    case 0x8001C748u:
        a[0] = va_arg(arguments, uint32);
        SetSprt(moh_draft_guest_pointer(a[0], 20u)); return 0u;
    case 0x8001B8C8u:
        for (i = 0u; i < 5u; ++i) a[i] = va_arg(arguments, uint32);
        SetDrawMode(moh_draft_guest_pointer(a[0], 12u), a[1], a[2], a[3],
            a[4] ? (PSX_RECT *)moh_draft_guest_pointer(a[4], 8u) : NULL);
        return 0u;
    case 0x80017234u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        return (uint32)sub_80017234(a[0], a[1], a[2]);
    }
    case 0x800172E0u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_800172E0(a[0], a[1]);
    }
    case 0x80017348u:
    {
        a[0] = va_arg(arguments, uint32);
        return moh_native_put_draw_env(a[0]);
    }
    case 0x80017408u:
    {
        a[0] = va_arg(arguments, uint32);
        return moh_native_put_disp_env(a[0]);
    }
    case 0x80017AF8u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_80017AF8(a[0], a[1]);
    }
    case 0x80017D68u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        return (uint32)sub_80017D68(a[0], a[1], a[2]);
    }
    case 0x80017D88u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_80017D88(a[0], a[1]);
    }
    case 0x80017E20u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_80017E20(a[0], a[1]);
    }
    case 0x80017EB8u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_80017EB8(a[0], a[1]);
    }
    case 0x80017ED4u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_80017ED4(a[0]);
    }
    case 0x80017F6Cu:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_80017F6C(a[0], a[1]);
    }
    case 0x8001804Cu:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_8001804C(a[0], a[1]);
    }
    case 0x8001827Cu:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_8001827C(a[0], a[1]);
    }
    case 0x80018738u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_80018738(a[0]);
    }
    case 0x80018794u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_80018794(a[0]);
    }
    case 0x800187DCu:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_800187DC(a[0]);
    }
    case 0x80018830u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        a[3] = va_arg(arguments, uint32);
        return (uint32)sub_80018830(a[0], a[1], a[2], a[3]);
    }
    case 0x80018AE0u:
    {
        return (uint32)sub_80018AE0();
    }
    case 0x80018D40u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_80018D40(a[0]);
    }
    case 0x80018E7Cu:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_80018E7C(a[0]);
    }
    case 0x80018FB8u:
    {
        return (uint32)sub_80018FB8();
    }
    case 0x80018FECu:
    {
        return (uint32)sub_80018FEC();
    }
    case 0x80019130u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_80019130(a[0]);
    }
    case 0x800191F8u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        return (uint32)sub_800191F8(a[0], a[1], a[2]);
    }
    case 0x80019740u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_80019740(a[0]);
    }
    case 0x8001AC88u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_8001AC88(a[0]);
    }
    case 0x8001AD38u:
    {
        return (uint32)sub_8001AD38();
    }
    case 0x8001AD98u:
    {
        return (uint32)moh_native_cd_reset();
    }
    case 0x8001ADD8u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_8001ADD8(a[0], a[1]);
    }
    case 0x8001AEE8u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_8001AEE8(a[0]);
    }
    case 0x8001AF88u:
    {
        return (uint32)sub_8001AF88();
    }
    case 0x8001B3E0u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_8001B3E0(a[0], a[1]);
    }
    case 0x8001B730u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        a[3] = va_arg(arguments, uint32);
        return (uint32)sub_8001B730(a[0], a[1], a[2], a[3]);
    }
    case 0x8001BE24u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        return (uint32)sub_8001BE24(a[0], a[1], a[2]);
    }
    case 0x8001CA78u:
    {
        return (uint32)sub_8001CA78();
    }
    case 0x8001CAA8u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_8001CAA8(a[0], a[1]);
    }
    case 0x8001CAD8u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_8001CAD8(a[0], a[1]);
    }
    case 0x8001CB08u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_8001CB08(a[0]);
    }
    case 0x8001CB3Cu:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_8001CB3C(a[0], a[1]);
    }
    case 0x8001CB9Cu:
    {
        return (uint32)sub_8001CB9C();
    }
    case 0x8001CBACu:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_8001CBAC(a[0]);
    }
    case 0x8001CBC4u:
    {
        return (uint32)sub_8001CBC4();
    }
    case 0x8001CC9Cu:
    {
        return sub_8001CC9C();
    }
    case 0x8001CE6Cu:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_8001CE6C(a[0], a[1]);
    }
    case 0x8001D0CCu:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_8001D0CC(a[0], a[1]);
    }
    case 0x8001D0F0u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_8001D0F0(a[0]);
    }
    case 0x8001D180u:
    {
        return (uint32)sub_8001D180();
    }
    case 0x8001D1B0u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_8001D1B0(a[0]);
    }
    case 0x8001DFD4u:
    {
        sub_8001DFD4();
        return 0u;
    }
    case 0x8001E0ECu:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_8001E0EC(a[0]);
    }
    case 0x8001F17Cu:
    {
        sub_8001F17C();
        return 0u;
    }
    case 0x8001F55Cu:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        return (uint32)sub_8001F55C(a[0], a[1], a[2]);
    }
    case 0x8001F67Cu:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        return (uint32)sub_8001F67C(a[0], a[1], a[2]);
    }
    case 0x8001F6ECu:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_8001F6EC(a[0]);
    }
    case 0x8001F740u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        a[3] = va_arg(arguments, uint32);
        return (uint32)sub_8001F740(a[0], a[1], a[2], a[3]);
    }
    case 0x8001FF08u:
    {
        a[0] = va_arg(arguments, uint32);
        sub_8001FF08(a[0]);
        return 0u;
    }
    case 0x800203F8u:
    {
        return (uint32)sub_800203F8();
    }
    case 0x80020F48u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_80020F48(a[0]);
    }
    case 0x800211C8u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_800211C8(a[0], a[1]);
    }
    case 0x80021388u:
    {
        return (uint32)sub_80021388();
    }
    case 0x80021444u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        return (uint32)sub_80021444(a[0], a[1], a[2]);
    }
    case 0x800216C4u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        return (uint32)sub_800216C4(a[0], a[1], a[2]);
    }
    case 0x80021748u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        return (uint32)sub_80021748(a[0], a[1], a[2]);
    }
    case 0x8002178Cu:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_8002178C(a[0], a[1]);
    }
    case 0x80021888u:
    {
        return (uint32)sub_80021888();
    }
    case 0x800218D8u:
    {
        return (uint32)sub_800218D8();
    }
    case 0x80022560u:
    {
        return (uint32)sub_80022560();
    }
    case 0x80022B9Cu:
    {
        return (uint32)sub_80022B9C();
    }
    case 0x80022BBCu:
    {
        return (uint32)sub_80022BBC();
    }
    case 0x80022C54u:
    {
        return (uint32)sub_80022C54();
    }
    case 0x80022D34u:
    {
        return (uint32)moh_native_cd_stop();
    }
    case 0x80022D74u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_80022D74(a[0], a[1]);
    }
    case 0x80022DF0u:
    {
        a[0] = va_arg(arguments, uint32);
        sub_80022DF0(a[0]);
        return 0u;
    }
    case 0x80022DFCu:
    {
        a[0] = va_arg(arguments, uint32);
        sub_80022DFC(a[0]);
        return 0u;
    }
    case 0x80022E08u:
    {
        a[0] = va_arg(arguments, uint32);
        sub_80022E08(a[0]);
        return 0u;
    }
    case 0x80022E14u:
    {
        a[0] = va_arg(arguments, uint32);
        sub_80022E14(a[0]);
        return 0u;
    }
    case 0x80022E20u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_80022E20(a[0]);
    }
    case 0x80022E38u:
    {
        return (uint32)sub_80022E38();
    }
    case 0x80022E48u:
    {
        return (uint32)sub_80022E48();
    }
    case 0x80022E64u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_80022E64(a[0], a[1]);
    }
    case 0x80022EA4u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_80022EA4(a[0], a[1]);
    }
    case 0x8002307Cu:
    {
        return (uint32)sub_8002307C();
    }
    case 0x800233D4u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_800233D4(a[0], a[1]);
    }
    case 0x800234E4u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_800234E4(a[0], a[1]);
    }
    case 0x800236C8u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_800236C8(a[0], a[1]);
    }
    case 0x80023A3Cu:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_80023A3C(a[0], a[1]);
    }
    case 0x80023AD0u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_80023AD0(a[0], a[1]);
    }
    case 0x80023B80u:
    {
        return (uint32)sub_80023B80();
    }
    case 0x80023BF8u:
    {
        return (uint32)sub_80023BF8();
    }
    case 0x80023C08u:
    {
        return (uint32)sub_80023C08();
    }
    case 0x80023C18u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        sub_80023C18(a[0], a[1]);
        return 0u;
    }
    case 0x80023C60u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        sub_80023C60(a[0], a[1]);
        return 0u;
    }
    case 0x80023CACu:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_80023CAC(a[0]);
    }
    case 0x80023CDCu:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_80023CDC(a[0]);
    }
    case 0x80023D0Cu:
    {
        return (uint32)sub_80023D0C();
    }
    case 0x80023D74u:
    {
        return (uint32)sub_80023D74();
    }
    case 0x80023F04u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_80023F04(a[0]);
    }
    case 0x80024238u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_80024238(a[0]);
    }
    case 0x80024328u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_80024328(a[0], a[1]);
    }
    case 0x8002454Cu:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_8002454C(a[0], a[1]);
    }
    case 0x800247C8u:
    {
        return (uint32)sub_800247C8();
    }
    case 0x80025130u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_80025130(a[0], a[1]);
    }
    case 0x800251D8u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_800251D8(a[0]);
    }
    case 0x800251F8u:
    {
        return (uint32)sub_800251F8();
    }
    case 0x80025298u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_80025298(a[0]);
    }
    case 0x800252E0u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_800252E0(a[0]);
    }
    case 0x800253B8u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_800253B8(a[0]);
    }
    case 0x80025464u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_80025464(a[0]);
    }
    case 0x80025528u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_80025528(a[0]);
    }
    case 0x800266BCu:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_800266BC(a[0], sub_8001CC9C);
    }
    case 0x80026AB8u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_80026AB8(a[0]);
    }
    case 0x80026AD8u:
    {
        return (uint32)sub_80026AD8();
    }
    case 0x8002703Cu:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_8002703C(a[0], a[1]);
    }
    case 0x800272BCu:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_800272BC(a[0], a[1]);
    }
    case 0x80027584u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        a[3] = va_arg(arguments, uint32);
        return (uint32)sub_80027584(a[0], a[1], a[2], a[3]);
    }
    case 0x80027990u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_80027990(a[0]);
    }
    case 0x80027A18u:
    {
        return (uint32)sub_80027A18();
    }
    case 0x80027AECu:
    {
        return (uint32)sub_80027AEC();
    }
    case 0x80027BDCu:
    {
        return (uint32)sub_80027BDC();
    }
    case 0x80027DC8u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        a[3] = va_arg(arguments, uint32);
        return (uint32)sub_80027DC8(a[0], a[1], a[2], a[3]);
    }
    case 0x80028A74u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_80028A74(a[0], a[1]);
    }
    case 0x80028AD4u:
    {
        return (uint32)sub_80028AD4();
    }
    case 0x80028BB0u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        sub_80028BB0(a[0], a[1], a[2]);
        return 0u;
    }
    case 0x80029420u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_80029420(a[0]);
    }
    case 0x80029448u:
    {
        return (uint32)sub_80029448();
    }
    case 0x80029530u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        a[3] = va_arg(arguments, uint32);
        return (uint32)sub_80029530(a[0], a[1], a[2], a[3]);
    }
    case 0x8002A8F0u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_8002A8F0(a[0]);
    }
    case 0x8002ACD0u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_8002ACD0(a[0]);
    }
    case 0x8002AD00u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_8002AD00(a[0]);
    }
    case 0x8002ADFCu:
    {
        sub_8002ADFC();
        return 0u;
    }
    case 0x8002AE48u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_8002AE48(a[0]);
    }
    case 0x800362E8u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        return (uint32)sub_800362E8(a[0], a[1], a[2]);
    }
    case 0x800363FCu:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        return (uint32)moh_native_load_file(a[0], a[1], a[2]);
    }
    case 0x800369ECu:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_800369EC(a[0]);
    }
    case 0x80036A70u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_80036A70(a[0]);
    }
    case 0x80036AF8u:
    {
        return sub_80036AF8();
    }
    case 0x80036B20u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return sub_80036B20(a[0], a[1]);
    }
    case 0x80036BF0u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_80036BF0(a[0]);
    }
    case 0x80036C14u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        return (uint32)sub_80036C14(a[0], a[1], a[2]);
    }
    case 0x80036DA0u:
        a[0] = va_arg(arguments, uint32); a[1] = va_arg(arguments, uint32);
        return sub_80036DA0(a[0], a[1]);
    case 0x80036DC4u:
        for (i = 0u; i < 4u; ++i) a[i] = va_arg(arguments, uint32);
        return sub_80036DC4(a[0], a[1], a[2], a[3]);
    case 0x80036E44u:
    {
        a[0] = va_arg(arguments, uint32);
        sub_80036E44(a[0]);
        return 0u;
    }
    case 0x80037060u:
    {
        sub_80037060();
        return 0u;
    }
    case 0x80037318u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)moh_native_load_overlay(a[0], a[1]);
    }
    case 0x80037370u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        return (uint32)sub_80037370(a[0], a[1], a[2]);
    }
    case 0x8003740Cu:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_8003740C(a[0], a[1]);
    }
    case 0x800374ECu:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_800374EC(a[0], a[1]);
    }
    case 0x80037A0Cu:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        return (uint32)sub_80037A0C(a[0], a[1], a[2]);
    }
    case 0x80037BB4u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_80037BB4(a[0], a[1]);
    }
    case 0x80037C70u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        return (uint32)sub_80037C70(a[0], a[1], a[2]);
    }
    case 0x8001874C:
        sub_8001874C(); return 0;
    case 0x8002B758:
        sub_8002B758(); return 0;
    case 0x80024880:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        a[3] = va_arg(arguments, uint32);
        sub_80024880(a[0], a[1], a[2], a[3]); return 0;
    case 0x80025150:
        a[0] = va_arg(arguments, uint32);
        return sub_80025150(a[0]);
    case 0x800251C4:
        a[0] = va_arg(arguments, uint32);
        return sub_800251C4(a[0]);
    case 0x80028B90:
        a[0] = va_arg(arguments, uint32);
        return sub_80028B90(a[0]);
    case 0x80017F54:
        return sub_80017F54();
    case 0x80019654:
        return sub_80019654();
    case 0x80020CE8:
        return sub_80020CE8();
    case 0x80022530:
        return sub_80022530();
    case 0x80022B7C:
        return sub_80022B7C();
    case 0x80025164:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return sub_80025164(a[0], a[1]);
    case 0x80025184:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return sub_80025184(a[0], a[1]);
    case 0x800251A4:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return sub_800251A4(a[0], a[1]);
    case 0x8001880C:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        return sub_8001880C(a[0], a[1], a[2]);
    case 0x80028B60:
        a[0] = va_arg(arguments, uint32);
        return sub_80028B60(a[0]);
    case 0x800191D0:
        return sub_800191D0();
    case 0x800218B0:
        return sub_800218B0();
    case 0x80024858:
        return sub_80024858();
    case 0x8002BA68:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return sub_8002BA68(a[0], a[1]);
    case 0x8001AB90:
        a[0] = va_arg(arguments, uint32);
        return sub_8001AB90(a[0]);
    case 0x80024B84:
        a[0] = va_arg(arguments, uint32);
        return sub_80024B84(a[0]);
    case 0x80018754:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return sub_80018754(a[0], a[1]);
    case 0x800201A4:
        return sub_800201A4();
    case 0x80020158:
        return sub_80020158();
    case 0x800135F8:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return sub_800135F8(a[0], a[1]);
    case 0x8001D054:
        return sub_8001D054();
    case 0x80024890:
        a[0] = va_arg(arguments, uint32);
        return sub_80024890(a[0]);
    case 0x8002334C:
        return sub_8002334C();
    case 0x8001AA90:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        return sub_8001AA90(a[0], a[1], a[2]);
    case 0x80024C90:
        a[0] = va_arg(arguments, uint32);
        return sub_80024C90(a[0]);
    case 0x80024BBC:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return sub_80024BBC(a[0], a[1]);
    case 0x8002BFC8:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return sub_8002BFC8(a[0], a[1]);
    case 0x800145DC:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        return sub_800145DC(a[0], a[1], a[2]);
    case 0x8001449C:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return sub_8001449C(a[0], a[1]);
    case 0x80023950:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return sub_80023950(a[0], a[1]);
    case 0x8001B2E0:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        a[3] = va_arg(arguments, uint32);
        return sub_8001B2E0(a[0], a[1], a[2], a[3]);
    case 0x8001B5C0:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        return sub_8001B5C0(a[0], a[1], a[2]);
    case 0x8001B890:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        a[3] = va_arg(arguments, uint32);
        return sub_8001B890(a[0], a[1], a[2], a[3]);
    case 0x8001B5B8:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return sub_8001B5B8(a[0], a[1]);
    case 0x8001B888:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        return sub_8001B888(a[0], a[1], a[2]);
    case 0x80017900:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return sub_80017900(a[0], a[1]);
    case 0x800184B8:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return sub_800184B8(a[0], a[1]);
    case 0x80024914:
        a[0] = va_arg(arguments, uint32);
        return sub_80024914(a[0]);
    case 0x80024D38:
        a[0] = va_arg(arguments, uint32);
        return sub_80024D38(a[0]);
    case 0x8001F7AC:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        return sub_8001F7AC(a[0], a[1], a[2]);
    case 0x8001F79C:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return sub_8001F79C(a[0], a[1]);
    case 0x80014128:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        a[3] = va_arg(arguments, uint32);
        return sub_80014128(a[0], a[1], a[2], a[3]);
    case 0x800125C4:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        a[3] = va_arg(arguments, uint32);
        a[4] = va_arg(arguments, uint32);
        a[5] = va_arg(arguments, uint32);
        return sub_800125C4(a[0], a[1], a[2], a[3], a[4], a[5]);
    case 0x80012D5C:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        return sub_80012D5C(a[0], a[1], a[2]);
    case 0x80011B48:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        a[3] = va_arg(arguments, uint32);
        return sub_80011B48(a[0], a[1], a[2], a[3]);
    case 0x80012FF8:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        a[3] = va_arg(arguments, uint32);
        sub_80012FF8(a[0], a[1], a[2], a[3]); return 0;
    case 0x80013698:
        a[0] = va_arg(arguments, uint32);
        return sub_80013698(a[0]);
    case 0x8002B068:
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return sub_8002B068(a[0], a[1]);
    default:
        moh_runtime_error("Unimplemented native call", address);
        return 0u;
    }
}
