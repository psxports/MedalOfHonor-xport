#include "game_draft_signatures.h"
#include "game_shell_signatures.h"
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
    VSyncCallback((void *)moh_irq_frame);
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

static uint32 moh_read_asset(uint32 filename, uint32 destination, uint32 *size)
{
    FILE *file;
    long bytes;
    void *buffer;
    if (!moh_files_ready) moh_runtime_error("Native files are not initialized", filename);
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
    if (fread(buffer, 1u, *size, file) != *size)
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
    if (fread(buffer, 1u, size, file) != size)
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
    return moh_read_asset(filename, destination, &size) ? 0 : -1;
}

uint32 moh_native_cd_reset(void)
{
    w_u32(0x80038598u, 0u);
    return moh_files_ready;
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
    uint32 a[16], i;
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
        if (a[0]) moh_runtime_error("Unsupported SPU initialization mode", a[0]);
        SpuInit();
        spu_bind_reverb_presets((const uint32 *)moh_draft_guest_pointer(0x800358ACu, 40u),
            (const uint8 *)moh_draft_guest_pointer(0x8003594Cu, 680u), 3u);
        w_u32(0x8003035Cu, 0u); w_u32(0x800302D8u, 0u);
        w_u32(0x800302DCu, 0u); w_u16(0x800302D4u, 0u);
        w_u32(0x800302E0u, 2u); w_u32(0x800302E4u, 3u);
        w_u32(0x800302E8u, 8u); w_u32(0x800302ECu, 7u);
        w_u32(0x800302F0u, 1u); w_u32(0x800302F4u, 0u);
        xport_bind_native_spu_transfer();
        return 0u;
    case 0x800156DCu:
        return SpuSetTransferStartAddr(va_arg(arguments, uint32));
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
        /* The original constructor count is zero */
        if (!r_u32(0x8002F278u)) w_u32(0x8002F278u, 1u);
        return 0u;
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
    case 0x800154ECu:
    {
        uint32 destination = va_arg(arguments, uint32);
        uint32 source = va_arg(arguments, uint32);
        uint32 bytes = va_arg(arguments, uint32);
        memcpy(moh_draft_guest_pointer(destination, bytes), moh_draft_guest_pointer(source, bytes), bytes);
        return destination;
    }
    case 0x8001575Cu:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        a[2] = va_arg(arguments, uint32);
        return (uint32)sub_8001575C(a[0], a[1], a[2]);
    }
    case 0x8001582Cu:
    {
        return (uint32)sub_8001582C();
    }
    case 0x80016B60u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_80016B60(a[0]);
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
        return (uint32)sub_80016E38(a[0]);
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
    case 0x80017234u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        return (uint32)sub_80017234(a[0], a[1]);
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
        return (uint32)sub_80017348(a[0]);
    }
    case 0x80017408u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_80017408(a[0]);
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
        return (uint32)sub_8001AD98();
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
        return (uint32)sub_8001B3E0(a[0]);
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
        return (uint32)sub_80022D34();
    }
    case 0x80022D74u:
    {
        a[0] = va_arg(arguments, uint32);
        return (uint32)sub_80022D74(a[0]);
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
        return (uint32)sub_80022E64(a[0]);
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
        return (uint32)sub_800266BC(a[0]);
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
        return (uint32)sub_800363FC(a[0], a[1], a[2]);
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
        sub_80036AF8();
        return 0u;
    }
    case 0x80036B20u:
    {
        a[0] = va_arg(arguments, uint32);
        a[1] = va_arg(arguments, uint32);
        sub_80036B20(a[0], a[1]);
        return 0u;
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
        return (uint32)sub_80037318(a[0], a[1]);
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
    default:
        moh_runtime_error("Unimplemented native call", address);
        return 0u;
    }
}
