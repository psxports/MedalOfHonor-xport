#include "game_shell_signatures.h"
#include "xport_trace.h"
#include "psx.h"
#include <string.h>
#include <stdlib.h>

extern uint32 xport_bios_init_card(uint32 mode);
extern uint32 xport_bios_start_card(void);
static uint32 shell_names_equal(uint32 left, uint32 right);

void shell_native_card_init(uint32 mode)
{
    uint32 previous;
    FUNCTION_MARKER(0x80079E20u, "SHELL.BIN");
    ChangeClearPAD(0u);
    VSync(0);
    previous = sub_8001AF88();
    if (!r_u32(0xA0000124u) && !SetConf(32, 1, 0x801FFF00u)) abort();
    if (!ReadInitPadFlag()) mode = 0u;
    xport_bios_init_card(mode);
    /* Native card services replace executable BIOS patch installation */
    if (previous == 1u) sub_8001AD38();
}

uint32 shell_native_card_start(void)
{
    uint32 previous = sub_8001AF88();
    FUNCTION_MARKER(0x80079EB0u, "SHELL.BIN");
    xport_bios_start_card();
    ChangeClearPAD(0u);
    if (previous == 1u) sub_8001AD38();
    return 0u;
}

uint32 shell_sub_80045260(uint32 bytes)
{
    uint32 pointer;
    FUNCTION_MARKER(0x80045260u, "SHELL.BIN");
    if (!r_u8(0x8007BFCFu)) pointer = sub_80036BF0(bytes);
    else
    {
        uint32 adjusted = (sint32)bytes < 0 ? bytes + 3u : bytes;
        uint32 remainder = bytes - ((uint32)((sint32)adjusted >> 2) << 2);
        uint32 used = r_u32(0x8007A288u);
        pointer = r_u32(0x8007A290u) + used;
        used += remainder ? bytes + 4u - remainder : bytes;
        w_u32(0x8007A288u, used);
        if ((sint32)used >= r_s32(0x8007A28Cu)) return 0u;
    }
    if (pointer) memset(moh_draft_guest_pointer(pointer, bytes), 0, bytes);
    return pointer;
}

uint32 shell_sub_800451F0(uint32 bytes)
{
    uint32 pointer;
    FUNCTION_MARKER(0x800451F0u, "SHELL.BIN");
    w_u32(0x8007A288u, 0u);
    w_u32(0x8007A28Cu, bytes);
    pointer = shell_sub_80045260(bytes);
    w_u32(0x8007A290u, pointer);
    return pointer ? 0u : ~0u;
}

uint32 shell_sub_8003CFD8(uint32 bytes)
{
    uint32 pointer;
    FUNCTION_MARKER(0x8003CFD8u, "SHELL.BIN");
    if (r_u32(0x8007A228u)) return (uint32)-8;
    pointer = shell_sub_80045260(bytes);
    if (!pointer) return (uint32)-7;
    w_u32(0x8007A228u, pointer);
    w_u32(0x8007A22Cu, bytes);
    return 0u;
}

uint32 shell_sub_8005F7A0(uint32 packet_count, uint32 secondary)
{
    uint32 pointer;
    FUNCTION_MARKER(0x8005F7A0u, "SHELL.BIN");
    moh_native_reset_graph(3u);
    SetGraphDebug(0);
    memset(moh_draft_guest_pointer(0x8007BE5Cu, 0x14Cu), 0, 0x14Cu);
    pointer = sub_80036BF0(0x1800u);
    w_u32(0x8007BED0u, pointer);
    if (!pointer) return (uint32)-5;
    pointer = sub_80036BF0(0x1800u);
    w_u32(0x8007BF48u, pointer);
    if (!pointer) return (uint32)-5;
    if ((uint8)secondary)
    {
        pointer = sub_80036BF0(0x1800u);
        w_u32(0x8007BECCu, pointer);
        if (!pointer) return (uint32)-5;
        pointer = sub_80036BF0(0x1800u);
        w_u32(0x8007BF44u, pointer);
        if (!pointer) return (uint32)-5;
    }
    if ((sint32)packet_count > 0)
    {
        pointer = sub_80036BF0(packet_count * 12u);
        if (!pointer) return (uint32)-4;
        w_u32(0x8007BF9Cu, pointer);
        w_u32(0x8007BFA0u, packet_count);
        w_u32(0x8007BFA4u, 0u);
        w_u8(0x8007BF98u, 1u);
    }
    SetDispMask(1);
    return 0u;
}

uint32 shell_sub_8005F89C(uint32 mode, uint32 background, uint32 red, uint32 green, uint32 blue)
{
    uint32 width = 512u, height = 240u, index, address;
    uint32 factor_bits = 0x3FDDB3D7u;
    float factor;
    volatile float projection;
    sint32 distance;
    FUNCTION_MARKER(0x8005F89Cu, "SHELL.BIN");
    if (mode >= 4u) return (uint32)-2;
    if (mode == 0u) width = 256u;
    else if (mode == 1u) width = 320u;
    else
    {
        w_u8(0x8007BF7Cu, 1u);
        w_u32(0x8007BF80u, mode == 2u ? 5u : 2u);
        w_u32(0x8007BF84u, mode == 2u ? 8u : 3u);
        if (mode == 3u) height = 256u;
    }
    w_u32(0x8007BF50u, mode);
    SetDefDrawEnv((DRAWENV *)moh_draft_guest_pointer(0x8007BE5Cu, sizeof(DRAWENV)), 0, 0, width, height);
    SetDefDrawEnv((DRAWENV *)moh_draft_guest_pointer(0x8007BED4u, sizeof(DRAWENV)), 0, height, width, height);
    SetDefDispEnv((DISPENV *)moh_draft_guest_pointer(0x8007BEB8u, sizeof(DISPENV)), 0, height, width, height);
    SetDefDispEnv((DISPENV *)moh_draft_guest_pointer(0x8007BF30u, sizeof(DISPENV)), 0, 0, width, height);
    w_u32(0x8007BF68u, width - 1u); w_u32(0x8007BF6Cu, height - 1u);
    w_u32(0x8007BF70u, (width - 1u) >> 1); w_u32(0x8007BF74u, (height - 1u) >> 1);
    w_u32(0x8007BF60u, 0u); w_u32(0x8007BF64u, 0u);
    for (index = 0u; index < 2u; ++index)
    {
        address = 0x8007BE5Cu + index * 0x78u;
        w_u8(address + 24u, (uint8)background != 0u);
        if ((uint8)background)
        {
            w_u8(address + 25u, red); w_u8(address + 26u, green); w_u8(address + 27u, blue);
        }
    }
    if (!(uint8)background)
    {
        w_u8(0x8007BF55u, 255u); w_u8(0x8007BF56u, 255u); w_u8(0x8007BF57u, 255u);
    }
    w_u8(0x8007BF54u, background); w_u8(0x8007BF90u, 1u);
    w_u32(0x8007BF4Cu, 0x8007BE5Cu);
    w_u32(0x8007BF58u, width); w_u32(0x8007BF5Cu, height);
    /* Native float replaces the SDK software arithmetic for the projection distance */
    memcpy(&factor, &factor_bits, sizeof(factor));
    projection = (float)width * factor;
    projection = projection * 0.5f;
    distance = (sint32)projection;
    w_u32(0x8007BF78u, (uint32)distance);
    InitGeom(); SetGeomOffset(width >> 1, height >> 1); SetGeomScreen(distance);
    ClearOTagR((uint32 *)moh_draft_guest_pointer(r_u32(0x8007BED0u), 0x1800u), 0x600);
    if (r_u32(0x8007BECCu))
        ClearOTagR((uint32 *)moh_draft_guest_pointer(r_u32(0x8007BECCu), 0x1800u), 0x600);
    return 0u;
}

void shell_sub_80078018(void)
{
    FUNCTION_MARKER(0x80078018u, "SHELL.BIN");
    shell_native_card_init(0u);
    shell_native_card_start();
    xport_bios_start_card();
}

void shell_sub_8007692C(void)
{
    FUNCTION_MARKER(0x8007692Cu, "SHELL.BIN");
    w_u32(0x8008F874u, 0u); w_u32(0x8008F8ACu, 0u);
    w_u32(0x8007EB70u, ~0u);
    w_u32(0x8008F868u, 0u); w_u32(0x8008F86Cu, 0u); w_u32(0x8008F870u, 0u);
    w_u32(0x8008F8BCu, 0u); w_u32(0x8008F87Cu, ~0u);
    w_u32(0x8008F8B4u, 1u); w_u32(0x8008F8B0u, 1u); w_u32(0x8008F8B8u, 0u);
    /* TODO Native card event callbacks */
    shell_sub_80078D70();
    moh_draft_missing_call(0x8001CB3Cu, 7u, 0x80077F10u);
}

void shell_sub_80045390(void)
{
    FUNCTION_MARKER(0x80045390u, "SHELL.BIN");
    shell_sub_80078018();
    shell_sub_8007692C();
    memset(moh_draft_guest_pointer(0x8007B668u, 7u), 0, 7u);
    memset(moh_draft_guest_pointer(0x8007A3A0u, 0x12B8u), 0, 0x12B8u);
    memset(moh_draft_guest_pointer(0x8007A30Cu, 0x94u), 0, 0x94u);
    w_u8(0x8007B658u, 0u);
}

void shell_sub_80078D70(void)
{
    static const uint32 specifications[4] = {4u, 0x8000u, 0x100u, 0x2000u};
    uint32 previous = sub_8001AF88(), index;
    FUNCTION_MARKER(0x80078D70u, "SHELL.BIN");
    for (index = 0u; index < 8u; ++index)
    {
        sint32 handle = OpenEventPSX(index < 4u ? 0xF4000001u : 0xF0000011u,
            specifications[index & 3u], 0x1000u, 0x80078CD0u + 20u * index);
        if (handle == -1) abort();
        w_u32(0x8008F918u + index * 4u, (uint32)handle);
    }
    for (index = 0u; index < 8u; ++index) EnableEventPSX(r_u32(0x8008F918u + index * 4u));
    for (index = 0u; index < 8u; ++index) TestEvent(r_u32(0x8008F918u + index * 4u));
    for (index = 0u; index < 8u; ++index) w_u32(0x8008F938u + index * 4u, 0u);
    if (previous == 1u) sub_8001AD38();
}

uint32 shell_sub_80078CB8(void)
{
    return r_u32(0x8007EB70u) >> 31;
}

void shell_sub_80078C4C(void)
{
    uint32 index = r_u32(0x8007EB70u);
    FUNCTION_MARKER(0x80078C4Cu, "SHELL.BIN");
    if ((sint32)index >= 0 && moh_draft_indirect_call(r_u32(0x8008F908u + index * 4u),
        0x8008F8C8u + index * 16u))
        w_u32(0x8007EB70u, r_u32(0x8007EB70u) - 1u);
}

void shell_sub_80077F10(void)
{
    FUNCTION_MARKER(0x80077F10u, "SHELL.BIN");
    if (!shell_sub_80078CB8())
    {
        shell_sub_80078C4C();
        if (shell_sub_80078CB8())
        {
            uint32 first = r_u32(0x8008F868u), second = r_u32(0x8008F86Cu);
            w_u32(0x8008F870u, 1u);
            w_u32(0x8008F858u, first); w_u32(0x8008F85Cu, second);
            w_u32(0x8008F868u, 0u); w_u32(0x8008F86Cu, 0u);
            if (r_u32(0x8008F8ACu)) moh_draft_indirect_call(r_u32(0x8008F8ACu), first, second);
        }
    }
    w_u32(0x8008F8B8u, r_u32(0x8008F8B8u) + 1u);
    w_u32(0x8008F8BCu, r_u32(0x8008F8BCu) + 1u);
}

static uint32 shell_loading_buffer(void)
{
    uint32 frame;
    w_u8(0x8007BF90u, !r_u8(0x8007BF90u));
    frame = r_u8(0x8007BF90u) ? 0x8007BED4u : 0x8007BE5Cu;
    w_u32(0x8007BF4Cu, frame);
    ClearOTagR((uint32 *)moh_draft_guest_pointer(r_u32(frame + 0x74u), 0x1800u), 0x600);
    if (r_u8(0x8007BF91u) && r_u32(frame + 0x70u))
        ClearOTagR((uint32 *)moh_draft_guest_pointer(r_u32(frame + 0x70u), 0x1800u), 0x600);
    return frame;
}

static void shell_link_packet(uint32 head, uint32 packet)
{
    w_u32(packet, (r_u32(packet) & 0xFF000000u) | (r_u32(head) & 0xFFFFFFu));
    w_u32(head, (r_u32(head) & 0xFF000000u) | (packet & 0xFFFFFFu));
}

void shell_sub_800619C0(uint32 state)
{
    uint32 frame = shell_loading_buffer(), buffer = r_u8(0x8007BF90u);
    uint32 ot = r_u32(frame + 0x74u), packet, command, index;
    FUNCTION_MARKER(0x800619C0u, "SHELL.BIN");
    if (r_u32(0x80039C5Cu) || !r_u8(0x8007C010u) || r_u32(0x8007BFB0u))
    {
        shell_link_packet(ot + 12u, state + buffer * 40u + 0x1Cu);
        shell_link_packet(ot + 12u, state + buffer * 40u + 0x6Cu);
    }
    packet = state + buffer * 8u + 0xBCu;
    w_u8(packet + 3u, 1u);
    command = 0xE1000000u | (r_u32(state + 0xFCu) & 0x9FFu);
    if (r_u8(frame + 22u)) command |= 0x200u;
    if (r_u8(frame + 23u)) command |= 0x400u;
    w_u32(packet + 4u, command);
    shell_link_packet(ot + 4u, packet);
    packet = state + buffer * 24u + 0xCCu;
    w_u8(packet + 3u, 5u); w_u8(packet + 7u, 0x2Au);
    w_u8(packet + 4u, r_u8(state + 20u));
    w_u8(packet + 5u, r_u8(state + 21u)); w_u8(packet + 6u, r_u8(state + 22u));
    w_u16(packet + 8u, 0u); w_u16(packet + 10u, 0u);
    w_u16(packet + 12u, 512u); w_u16(packet + 14u, 0u);
    w_u16(packet + 16u, 0u); w_u16(packet + 18u, 240u);
    w_u16(packet + 20u, 512u); w_u16(packet + 22u, 240u);
    shell_link_packet(ot, packet);
    w_u32(0x8007BF88u, (uint32)DrawSync(0));
    if (r_u8(0x8007BF98u))
    {
        packet = r_u32(0x8007BF9Cu);
        for (index = 0u; (sint32)index < r_s32(0x8007BFA4u); ++index, packet += 12u)
            sub_8001704C(packet, r_u32(packet + 8u));
        w_u32(0x8007BFA4u, 0u);
    }
    w_u32(0x8007BF8Cu, (uint32)VSync(2));
    PutDispEnv((DISPENV *)moh_draft_guest_pointer(frame + 0x5Cu, sizeof(DISPENV)));
    PutDrawEnv((DRAWENV *)moh_draft_guest_pointer(frame, sizeof(DRAWENV)));
    DrawOTag((uint32 *)moh_draft_guest_pointer(r_u32(frame + 0x74u) + 0x17FCu, 4u));
}

uint32 shell_sub_8005F580(uint32 filename, uint32 unused, uint32 work_buffer)
{
    uint32 destination = (uint8)work_buffer ? r_u32(0x8007A228u) : 0u;
    uint32 resource;
    FUNCTION_MARKER(0x8005F580u, "SHELL.BIN");
    resource = moh_native_load_resource(filename, destination, r_u32(0x8007A22Cu));
    if (!resource) w_u32(0x8007BE50u, (uint32)-3);
    else w_u32(resource + 0x44u, resource + 0x48u);
    return resource;
}

uint32 shell_resource_find(uint32 resource, uint32 name, uint32 *index)
{
    uint32 entry, item;
    FUNCTION_MARKER(0x8005F614u, "SHELL.BIN");
    if (!resource || !name) { w_u32(0x8007BE50u, (uint32)-2); return 0u; }
    entry = r_u32(resource + 0x44u);
    for (item = 0u; (sint32)item < r_s32(resource + 0x40u); ++item, entry += 80u)
    {
        uint32 offset = 0u;
        sint32 comparison = entry ? 0 : -1;
        if (entry)
        {
            while (r_u8(entry + offset))
            {
                uint32 left = r_u8(entry + offset), right = r_u8(name + offset);
                if (r_u8(0x8002F29Du + left) & 1u) left = (left + 32u) & 255u;
                if (r_u8(0x8002F29Du + right) & 1u) right = (right + 32u) & 255u;
                if (left != right) { comparison = left > right ? 1 : -1; break; }
                ++offset;
            }
            if (r_u8(name + offset)) comparison = -1;
        }
        if (!comparison) { *index = item; return resource + r_u32(entry + 72u); }
    }
    w_u32(0x8007BE50u, 1u);
    return 0u;
}

static void shell_loading_images(uint32 resource_name, uint32 left_name, uint32 right_name)
{
    uint32 item[2] = {0u, 0u}, data, resource;
    resource = moh_draft_missing_call(0x8005F580u, resource_name, 0u, 1u);
    data = shell_resource_find(resource, left_name, item);
    w_u32(0x8007C018u, moh_draft_missing_call(0x8003D3A0u, data,
        r_u32(r_u32(resource + 0x44u) + item[0] * 80u + 0x44u), 8u));
    data = shell_resource_find(resource, right_name, item);
    w_u32(0x8007C01Cu, moh_draft_missing_call(0x8003D3A0u, data,
        r_u32(r_u32(resource + 0x44u) + item[0] * 80u + 0x44u), 8u));
}

void shell_sub_8006118C(uint32 mode)
{
    uint32 index, packet, pointer, frame, color;
    sint32 brightness;
    PSX_RECT rectangle;
    FUNCTION_MARKER(0x8006118Cu, "SHELL.BIN");
    if (!r_u8(0x8007BFF9u))
    {
        memset(moh_draft_guest_pointer(0x8007C010u, 256u), 0, 256u);
        w_u8(0x8007C010u, 1u);
        if (!r_u32(0x80039C5Cu)) w_u32(0x8007C020u, 30u);
        for (index = 0u; index < 4u; ++index)
        {
            uint32 left = index < 2u ? 0u : 256u, right = index < 2u ? 256u : 511u;
            packet = 0x8007C02Cu + index * 40u;
            w_u8(packet + 3u, 9u); w_u8(packet + 7u, 0x2Cu);
            w_u8(packet + 12u, 0u); w_u8(packet + 13u, 0u);
            w_u8(packet + 20u, 255u); w_u8(packet + 21u, 0u);
            w_u8(packet + 28u, 0u); w_u8(packet + 29u, 239u);
            w_u8(packet + 36u, 255u); w_u8(packet + 37u, 239u);
            w_u8(packet + 4u, 128u); w_u8(packet + 5u, 128u); w_u8(packet + 6u, 128u);
            w_u16(packet + 8u, left); w_u16(packet + 10u, 0u);
            w_u16(packet + 16u, right); w_u16(packet + 18u, 0u);
            w_u16(packet + 24u, left); w_u16(packet + 26u, 239u);
            w_u16(packet + 32u, right); w_u16(packet + 34u, 239u);
        }
        w_u8(0x8007BFF9u, 1u);
    }
    w_u32(0x8007C014u, mode);
    switch (mode)
    {
    case 0u:
        frame = shell_loading_buffer();
        if (r_u32(0x80039C5Cu)) shell_loading_images(0x8003B044u, 0x8003B060u, 0x8003B07Cu);
        else if (!r_u8(0x8007C010u))
        {
            if (!r_u32(0x80038664u) || r_u32(0x80038660u) == 11u)
                shell_loading_images(0x8003B098u, 0x8003B0B4u, 0x8003B0CCu);
            else shell_loading_images(0x8003B0E4u, 0x8003B100u, 0x8003B118u);
        }
        else if (r_u32(0x8007BFB0u)) shell_loading_images(0x8003B130u, 0x8003B14Cu, 0x8003B164u);
        for (index = 0u; index < 2u; ++index)
        {
            pointer = r_u32(0x8007C018u + index * 4u);
            if (!pointer) continue;
            pointer = r_u32(pointer);
            packet = 0x8007C02Cu + index * 80u;
            w_u16(packet + 14u, r_u16(pointer + 48u));
            w_u16(packet + 22u, r_u16(pointer + 50u));
            w_u16(packet + 54u, r_u16(pointer + 48u));
            w_u16(packet + 62u, r_u16(pointer + 50u));
        }
        w_u32(0x8007C10Cu, GetTPage(0, 2, 0, 0));
        for (brightness = 128; brightness >= -16; brightness -= 21)
        {
            color = brightness > 0 ? (uint32)brightness : 0u;
            w_u8(0x8007C024u, color); w_u8(0x8007C025u, color); w_u8(0x8007C026u, color);
            shell_sub_800619C0(0x8007C010u);
        }
        shell_sub_800619C0(0x8007C010u);
        w_u32(0x8007C014u, 1u);
        break;
    case 1u:
        w_u32(0x8007C10Cu, GetTPage(0, 2, 0, 0));
        if (!r_u32(0x80039C5Cu))
        {
            if (r_s32(0x8007C020u) >= 242) w_u32(0x8007C020u, 241u);
            rectangle.x = 654; rectangle.y = 207; rectangle.w = r_u16(0x8007C020u); rectangle.h = 6;
            ClearImage(&rectangle, r_u8(0x8007BFD3u), r_u8(0x8007BFD4u), r_u8(0x8007BFD5u));
            DrawSync(0);
        }
        for (brightness = 0; brightness < 129; brightness += 21)
        {
            color = brightness < 128 ? (uint32)brightness : 128u;
            w_u8(0x8007C024u, color); w_u8(0x8007C025u, color); w_u8(0x8007C026u, color);
            shell_sub_800619C0(0x8007C010u);
        }
        w_u32(0x8007C014u, 2u);
        moh_draft_missing_call(0x8003D890u, r_u32(0x8007C018u));
        moh_draft_missing_call(0x8003D890u, r_u32(0x8007C01Cu));
        w_u32(0x8007C01Cu, 0u); w_u32(0x8007C018u, 0u);
        rectangle.x = 0; rectangle.y = 0; rectangle.w = 512; rectangle.h = 512;
        ClearImage(&rectangle, 0u, 0u, 0u); DrawSync(0);
        w_u32(0x8007C014u, 2u); w_u8(0x8007BE74u, 1u); w_u8(0x8007BEECu, 1u);
        break;
    case 3u:
    case 4u:
        if (r_u32(0x80039C5Cu)) break;
        if (r_s32(0x8007C020u) >= 242) w_u32(0x8007C020u, 241u);
        rectangle.x = 142; rectangle.y = 207; rectangle.w = r_u16(0x8007C020u); rectangle.h = 6;
        color = mode == 3u ? 0x8007BFD6u : 0x8007BFD3u;
        ClearImage(&rectangle, r_u8(color), r_u8(color + 1u), r_u8(color + 2u));
        rectangle.y = 447;
        ClearImage(&rectangle, r_u8(color), r_u8(color + 1u), r_u8(color + 2u));
        w_u32(0x8007C020u, r_u32(0x8007C020u) + (mode == 3u ? 24u : 30u));
        DrawSync(0);
        break;
    }
    DrawSync(0);
}

void shell_sub_800767E0(uint32 data, uint32 texture)
{
    uint32 pixel_header = data + 8u, index;
    FUNCTION_MARKER(0x800767E0u, "SHELL.BIN");
    w_u32(texture, r_u32(data));
    if (r_u32(data) & 8u)
    {
        for (index = 0u; index < 8u; index += 2u) w_u16(texture + 16u + index, r_u16(data + 8u + index));
        w_u32(texture + 24u, data + 16u);
        pixel_header = data + 8u + (r_u32(data + 4u) & ~3u);
    }
    for (index = 0u; index < 8u; index += 2u) w_u16(texture + 4u + index, r_u16(pixel_header + index));
    w_u32(texture + 12u, pixel_header + 8u);
}

uint32 shell_sub_8003D500(uint32 list)
{
    uint32 texture, tail;
    FUNCTION_MARKER(0x8003D500u, "SHELL.BIN");
    if (!list) { w_u32(0x8007A23Cu, (uint32)-2); return 0u; }
    texture = shell_sub_80045260(72u);
    if (!texture) { w_u32(0x8007A23Cu, (uint32)-4); return 0u; }
    tail = r_u32(list + 4u);
    if (!tail) w_u32(list, texture);
    else { w_u32(tail + 64u, texture); w_u32(texture + 60u, tail); }
    w_u32(list + 4u, texture); w_u32(list + 8u, r_u32(list + 8u) + 1u);
    w_u32(texture + 68u, list);
    return texture;
}

sint32 shell_sub_8003D594(uint32 texture, uint32 flags, uint32 data)
{
    uint32 depth, blend, scale = 1u, index, count, colors, width, height;
    sint32 x, y, u, v, right, bottom;
    PSX_RECT rectangle;
    FUNCTION_MARKER(0x8003D594u, "SHELL.BIN");
    shell_sub_800767E0(data + 4u, texture);
    depth = r_u32(texture) & 7u;
    w_u32(texture + 40u, depth);
    blend = flags & 4u ? flags & 3u : 0u;
    w_u32(texture + 44u, flags & 4u ? blend : 8u);
    if (depth >= 5u) return -5;
    width = r_u16(texture + 8u); height = r_u16(texture + 10u);
    w_u8(texture + 28u, depth < 2u);
    if (depth < 2u)
    {
        scale = depth == 1u ? 2u : 4u;
        count = depth == 1u ? 256u : 16u;
        colors = r_u32(texture + 24u);
        if (flags & 4u)
            for (index = 0u; index < count; ++index)
                if (r_u16(colors + index * 2u)) w_u16(colors + index * 2u, r_u16(colors + index * 2u) | 0x8000u);
        rectangle.x = r_s16(texture + 16u); rectangle.y = r_s16(texture + 18u);
        rectangle.w = (sint16)count; rectangle.h = 1;
        LoadImagePSX(&rectangle, (uint32 *)moh_draft_guest_pointer(colors, count * 2u));
        w_u16(texture + 48u, ((uint32)rectangle.y << 6) | (((uint32)rectangle.x >> 4) & 63u));
        w_u32(texture + 32u, width * scale);
    }
    else
    {
        w_u32(texture + 32u, width);
        colors = r_u32(texture + 12u);
        if (flags & 4u)
            for (index = 0u; index < width * height; ++index)
                if (r_u16(colors + index * 2u)) w_u16(colors + index * 2u, r_u16(colors + index * 2u) | 0x8000u);
    }
    x = r_s16(texture + 4u); y = r_s16(texture + 6u);
    rectangle.x = (sint16)x; rectangle.y = (sint16)y;
    rectangle.w = (sint16)width; rectangle.h = (sint16)height;
    LoadImagePSX(&rectangle, (uint32 *)moh_draft_guest_pointer(r_u32(texture + 12u), width * height * 2u));
    DrawSync(0);
    w_u16(texture + 50u, GetTPage(depth, blend, x, y));
    w_u32(texture + 36u, height);
    v = y - (y / 256) * 256;
    bottom = (v & 255) + (sint32)height - 1;
    if (bottom > 255) bottom = 255;
    u = (sint32)scale * (sint16)(x - (x / 64) * 64);
    right = (u & 255) + r_s32(texture + 32u) - 1;
    if (right > 255) right = 255;
    w_u8(texture + 52u, u); w_u8(texture + 53u, v);
    w_u8(texture + 54u, right); w_u8(texture + 55u, v);
    w_u8(texture + 56u, u); w_u8(texture + 57u, bottom);
    w_u8(texture + 58u, right); w_u8(texture + 59u, bottom);
    return 0;
}

uint32 shell_sub_8003D3E8(uint32 data, uint32 flags, uint32 bytes)
{
    uint32 list = shell_sub_80045260(12u), texture, next, consumed;
    sint32 error;
    FUNCTION_MARKER(0x8003D3E8u, "SHELL.BIN");
    if (!list) { w_u32(0x8007A23Cu, (uint32)-4); return 0u; }
    for (;;)
    {
        texture = shell_sub_8003D500(list);
        if (!texture) { w_u32(0x8007A23Cu, (uint32)-4); break; }
        error = shell_sub_8003D594(texture, (uint32)(sint32)(sint16)flags, data);
        if (error < 0) { w_u32(0x8007A23Cu, (uint32)error); break; }
        next = r_u32(texture + 12u) + r_u16(texture + 8u) * r_u16(texture + 10u) * 2u;
        consumed = next - data;
        if ((sint32)consumed >= (sint32)bytes) return list;
        bytes -= consumed; data = next;
    }
    /* TODO Translate texture-list cleanup for allocation or format errors */
    while (r_u32(list)) moh_draft_missing_call(0x8003D8ECu, r_u32(list));
    moh_draft_missing_call(0x80045328u, list);
    return 0u;
}

uint32 shell_sub_8003D3A0(uint32 data, uint32 bytes, uint32 flags)
{
    FUNCTION_MARKER(0x8003D3A0u, "SHELL.BIN");
    if (!data) { w_u32(0x8007A23Cu, (uint32)-2); return 0u; }
    return shell_sub_8003D3E8(data, (uint32)(sint32)(sint16)flags, bytes);
}

void shell_sub_80056C58(void)
{
    CdlATV volume;
    uint8 response[8] = {0};
    FUNCTION_MARKER(0x80056C58u, "SHELL.BIN");
    w_u32(0x8007BB94u, 0u); w_u32(0x8007BB98u, 0u); w_u32(0x8007BB8Cu, 0u);
    memset(&volume, 127, sizeof(volume));
    CdControl(12u, NULL, response);
    CdMix(&volume);
}

void shell_sub_80063834(uint32 volume)
{
    CdlATV attr;
    FUNCTION_MARKER(0x80063834u, "SHELL.BIN");
    switch (r_u8(0x8007C368u))
    {
    case 0u: volume = 0u; break;
    case 1u: volume -= 16u; break;
    case 3u: volume += 16u; break;
    default: break;
    }
    memset(&attr, (uint8)volume, sizeof(attr));
    CdMix(&attr);
}

void shell_sub_800637FC(void)
{
    uint32 index;
    FUNCTION_MARKER(0x800637FCu, "SHELL.BIN");
    for (index = 0u; index < 16u; ++index)
    {
        w_u32(0x8007C304u + index * 4u, 0u);
        w_u16(0x8007C344u + index * 2u, 0xFFFFu);
    }
}

uint16 shell_sub_8006377C(uint32 sample, uint32 programs, uint32 header)
{
    uint32 index;
    FUNCTION_MARKER(0x8006377Cu, "SHELL.BIN");
    for (index = 0u; index < programs * 16u; ++index)
    {
        uint32 tone = header + 0x824u + index * 32u;
        if (sample == (uint32)r_u16(tone + 18u) - 1u)
        {
            uint16 pitch = r_u16(0x8007C110u + ((uint32)r_u8(tone) + 4u) * 2u);
            return pitch < 0x1000u ? pitch : 0x1000u;
        }
    }
    return 0x800u;
}

sint32 shell_sub_80063314(uint32 resource, uint32 slot)
{
    uint32 header = r_u32(0x8007C2C4u + slot * 4u);
    uint32 bytes = (uint32)r_u16(resource + 18u) * 512u + 0xA20u;
    sint16 bank;
    uint32 index;
    FUNCTION_MARKER(0x80063314u, "SHELL.BIN");
    if (r_u32(0x8007C304u + slot * 4u)) { shell_sub_80045328(resource); return -1; }
    memcpy(moh_draft_guest_pointer(header, bytes), moh_draft_guest_pointer(resource, bytes), bytes);
    if (r_u32(header) != 0x56414270u) { shell_sub_80045328(resource); return -1; }
    bank = SsVabOpenHead(moh_draft_guest_pointer(header, bytes), -1);
    w_u16(0x8007C344u + slot * 2u, (uint16)bank);
    if (bank < 0) return bank;
    if (SsVabTransBody(moh_draft_guest_pointer(resource + bytes, 1u), bank) != bank) return -1;
    SsVabTransCompleted(1);
    w_u32(0x8007C284u + slot * 4u, header + bytes - 512u);
    w_u32(0x8007C244u + slot * 4u, moh_draft_missing_call(0x80020218u, (uint32)(sint32)bank));
    for (index = 0u; index < 256u; ++index)
        w_u16(0x8007C390u + slot * 2u + index * 32u,
            shell_sub_8006377C(index, r_u16(header + 18u), header));
    w_u32(0x8007C304u + slot * 4u, 1u);
    return bank;
}

sint32 shell_sub_800638D4(uint32 resource, uint32 slot)
{
    uint32 header, bytes;
    FUNCTION_MARKER(0x800638D4u, "SHELL.BIN");
    if (!resource) return -2;
    if (slot >= 16u) return -2;
    bytes = (uint32)r_u16(resource + 18u) * 512u + 0xA20u;
    // Allocate the complete header copied by the loader
    header = sub_80036BF0(bytes);
    if (!header) return -4;
    w_u32(0x8007C2C4u + slot * 4u, header);
    return (sint16)shell_sub_80063314(resource, slot);
}

void shell_sub_80045328(uint32 pointer)
{
    uint32 base = r_u32(0x8007A290u);
    FUNCTION_MARKER(0x80045328u, "SHELL.BIN");
    if (!pointer) w_u32(0x8007A294u, (uint32)-2);
    else if (!base || pointer < base || pointer >= base + r_u32(0x8007A288u)) sub_80036E44(pointer);
}

uint32 shell_sub_80062F70(uint32 resource)
{
    uint32 descriptor, slot;
    sint16 count = r_s16(resource + 0x16u);
    FUNCTION_MARKER(0x80062F70u, "SHELL.BIN");
    descriptor = shell_sub_80045260((uint32)((sint32)count * 4 + 20));
    if (!descriptor) { w_u32(0x8007C218u, (uint32)-4); return 0u; }
    w_u16(descriptor + 2u, (uint16)count);
    w_u32(descriptor + 12u, descriptor + 20u);
    for (slot = 0u; slot < 16u && !r_u8(0x8007C21Cu + slot); ++slot) {}
    if (slot < 16u)
    {
        w_u16(descriptor, (uint16)slot);
        w_u8(descriptor + 16u, 110u); w_u8(descriptor + 17u, 110u);
        w_u32(descriptor + 4u, 0x10000u); w_u32(descriptor + 8u, 0u);
        if (shell_sub_800638D4(resource, slot) >= 0) return descriptor;
        w_u32(0x8007C218u, (uint32)-6);
    }
    else w_u32(0x8007C218u, (uint32)-5);
    shell_sub_80045328(descriptor);
    return 0u;
}

uint32 shell_sub_8003BE58(void)
{
    uint32 animation;
    FUNCTION_MARKER(0x8003BE58u, "SHELL.BIN");
    animation = shell_sub_80045260(0x70u);
    if (!animation) w_u32(0x8007BA80u, (uint32)-3);
    return animation;
}

uint32 shell_sub_8003BEE8(uint32 animation, sint32 mode)
{
    uint32 frame, tail, index;
    FUNCTION_MARKER(0x8003BEE8u, "SHELL.BIN");
    if (!animation) { w_u32(0x8007A220u, (uint32)-2); return 0u; }
    if (mode < 0) mode = 0;
    if (mode > 1) mode = 1;
    if (mode == 1) w_u8(animation + 96u, 1u);
    frame = shell_sub_80045260(0x350u);
    if (!frame) { w_u32(0x8007A220u, (uint32)-3); return 0u; }
    for (index = 0u; index < 3u; ++index)
    {
        w_u32(frame + 24u + index * 4u, 0x800000u);
        w_u32(frame + 292u + index * 4u, 0x800000u);
    }
    w_u32(frame + 280u, r_u32(0x8007BF70u) << 16u);
    w_u32(frame + 284u, r_u32(0x8007BF74u) << 16u);
    w_u32(frame + 548u, r_u32(0x8007BF70u) << 16u);
    w_u32(frame + 552u, r_u32(0x8007BF74u) << 16u);
    w_u32(frame + 288u, 0x10000000u); w_u32(frame + 556u, 0x10000000u);
    w_u32(frame + 16u, (uint32)mode);
    tail = r_u32(animation + 68u);
    if (!tail) { w_u32(animation + 72u, frame); w_u32(animation + 64u, frame); }
    else { w_u32(tail + 4u, frame); w_u32(frame + 8u, tail); }
    w_u32(animation + 68u, frame);
    index = r_u32(animation + 76u); w_u32(animation + 76u, index + 1u);
    w_u32(frame, index); w_u32(frame + 12u, animation);
    return frame;
}

uint32 shell_sub_80048E98(void)
{
    uint32 screen = shell_sub_80045260(0x84u), tail;
    FUNCTION_MARKER(0x80048E98u, "SHELL.BIN");
    if (!screen) { w_u32(0x8007BA80u, (uint32)-3); return 0u; }
    tail = r_u32(0x8007BA70u);
    if (tail) { w_u32(tail + 0x80u, screen); w_u32(screen + 0x7Cu, tail); }
    else w_u32(0x8007BA6Cu, screen);
    w_u32(0x8007BA70u, screen); w_u32(0x8007BA74u, r_u32(0x8007BA74u) + 1u);
    return screen;
}

static uint32 shell_append_element(uint32 owner, uint32 bytes, uint32 head_offset,
    uint32 tail_offset, uint32 count_offset, uint32 previous_offset, uint32 next_offset,
    uint32 owner_offset, uint32 index_offset)
{
    uint32 element, tail, count;
    if (!owner) { w_u32(0x8007BA80u, (uint32)-2); return 0u; }
    element = shell_sub_80045260(bytes);
    if (!element) { w_u32(0x8007BA80u, (uint32)-3); return 0u; }
    tail = r_u32(owner + tail_offset);
    if (tail) { w_u32(tail + next_offset, element); w_u32(element + previous_offset, tail); }
    else { w_u32(owner + head_offset, element); w_u32(owner + head_offset + 4u, element); }
    w_u32(owner + tail_offset, element);
    count = r_u32(owner + count_offset);
    w_u32(element + owner_offset, owner); w_u32(element + index_offset, count);
    w_u32(owner + count_offset, count + 1u);
    return element;
}

uint32 shell_sub_80048F14(uint32 owner)
{
    FUNCTION_MARKER(0x80048F14u, "SHELL.BIN");
    return shell_append_element(owner, 0xBCu, 0x1Cu, 0x24u, 0x28u, 0xB0u, 0xB4u, 0xB8u, 0x38u);
}

uint32 shell_sub_80048FB0(uint32 owner)
{
    FUNCTION_MARKER(0x80048FB0u, "SHELL.BIN");
    return shell_append_element(owner, 0x94u, 0x30u, 0x38u, 0x3Cu, 0x84u, 0x88u, 0x8Cu, 0x90u);
}

uint32 shell_sub_8004904C(uint32 owner)
{
    FUNCTION_MARKER(0x8004904Cu, "SHELL.BIN");
    return shell_append_element(owner, 0x8Cu, 0x60u, 0x68u, 0x6Cu, 0x7Cu, 0x80u, 0x84u, 0x88u);
}

uint32 shell_sub_800490F0(uint32 owner)
{
    FUNCTION_MARKER(0x800490F0u, "SHELL.BIN");
    return shell_append_element(owner, 0xDCu, 0x74u, 0x7Cu, 0x80u, 0xCCu, 0xD0u, 0xD4u, 0xD8u);
}

uint32 shell_sub_80049190(uint32 owner)
{
    FUNCTION_MARKER(0x80049190u, "SHELL.BIN");
    return shell_append_element(owner, 0x140u, 0x8Cu, 0x94u, 0x98u, 0x134u, 0x138u, 0x13Cu, 0x130u);
}

uint32 shell_sub_80049230(uint32 owner)
{
    FUNCTION_MARKER(0x80049230u, "SHELL.BIN");
    return shell_append_element(owner, 0x60u, 0xA0u, 0xA8u, 0xACu, 0x50u, 0x54u, 0x58u, 0x5Cu);
}

static void shell_copy_string(uint32 destination, uint32 source)
{
    uint8 value;
    if (!destination || !source) return;
    do { value = r_u8(source++); w_u8(destination++, value); } while (value);
}

static void shell_copy_bytes(uint32 destination, uint32 source, uint32 bytes)
{
    memcpy(moh_draft_guest_pointer(destination, bytes), moh_draft_guest_pointer(source, bytes), bytes);
}

uint32 shell_sub_80049E90(uint32 script, uint32 bytes)
{
    uint32 end = script + bytes, cursor = script, screen = 0u, group = 0u, animated = 0u;
    FUNCTION_MARKER(0x80049E90u, "SHELL.BIN");
    do
    {
        uint32 type = r_u32(cursor), object, index, length;
        switch (type)
        {
        case 1u:
            screen = shell_sub_80048E98();
            if (!screen) return (uint32)-3;
            shell_copy_string(screen, cursor + 4u); cursor += 32u; group = 0u;
            break;
        case 2u:
            group = shell_sub_80048F14(screen);
            if (!group) return (uint32)-3;
            shell_copy_string(group, cursor + 4u); shell_copy_string(group + 28u, cursor + 32u);
            { sint32 value = r_s32(cursor + 60u); w_u32(group + 84u, value < 0 ? 0u : value > 15 ? 15u : (uint32)value); }
            cursor += 64u;
            break;
        case 10u:
            animated = shell_sub_80048FB0(screen);
            if (!animated) return (uint32)-3;
            shell_copy_string(animated, cursor + 4u); shell_copy_string(animated + 28u, cursor + 32u);
            shell_copy_string(animated + 56u, cursor + 60u);
            w_u32(animated + 84u, r_u32(cursor + 88u));
            w_u8(animated + 109u, r_u8(cursor + 92u)); w_u8(animated + 110u, r_u8(cursor + 93u));
            shell_copy_bytes(animated + 112u, cursor + 96u, 20u);
            object = moh_draft_missing_call(0x8003BE58u);
            if (!object) return (uint32)-3;
            w_u32(animated + 88u, object); cursor += 116u;
            break;
        case 11u:
            if (!animated) return (uint32)-6;
            object = moh_draft_missing_call(0x8003BEE8u, r_u32(animated + 88u), r_u32(cursor + 12u));
            if (!object) return (uint32)-3;
            shell_copy_bytes(object + 24u, cursor + 16u, 268u);
            shell_copy_bytes(object + 292u, cursor + 16u, 268u);
            w_u32(object + 840u, r_u32(cursor + 4u)); w_u32(object + 20u, r_u32(cursor + 8u));
            w_u32(object + 828u, r_u32(cursor + 284u)); w_u32(object + 832u, r_u32(cursor + 288u));
            cursor += 292u;
            break;
        case 5u:
        case 12u:
            group = shell_sub_80045260(188u);
            if (!group) return (uint32)-3;
            w_u32(screen + (type == 5u ? 64u : 68u), group); cursor += 4u;
            break;
        case 3u:
        case 6u:
            if (r_u8(cursor + 32u))
            {
                if (r_u32(group + 112u)) return (uint32)-8;
                object = shell_sub_80045260(220u);
                if (!object) return (uint32)-3;
                w_u32(group + 112u, object);
            }
            else object = shell_sub_800490F0(group);
            if (!object) return (uint32)-3;
            shell_copy_string(object, cursor + 4u);
            for (index = 0u; index < 4u; ++index) w_u16(object + 46u + index * 2u, r_u16(cursor + 36u + index * 4u));
            if (type == 3u)
            {
                w_u8(object + 54u, r_u8(cursor + 52u)); w_u8(object + 55u, r_u8(cursor + 53u));
                w_u16(object + 40u, 0xFFFFu); w_u16(object + 42u, 0xFFFFu); w_u8(object + 44u, 0u);
                w_u8(object + 56u, r_u8(cursor + 54u)); w_u32(object + 200u, r_u32(cursor + 56u));
                w_u32(object + 144u, r_u32(cursor + 60u)); w_u32(object + 148u, r_u32(cursor + 64u));
                w_u32(object + 188u, r_u32(cursor + 68u)); w_u32(object + 152u, r_u32(cursor + 60u));
                cursor += 72u;
            }
            else
            {
                w_u8(object + 55u, r_u8(cursor + 68u));
                for (index = 0u; index < 4u; ++index) w_u16(object + 36u + index * 2u, r_u16(cursor + 52u + index * 4u));
                w_u8(object + 44u, 1u); w_u8(object + 56u, r_u8(cursor + 69u));
                w_u32(object + 200u, r_u32(cursor + 72u));
                for (index = 0u; index < 3u; ++index) w_u32(object + 144u + index * 4u, r_u32(cursor + 76u));
                w_u32(object + 188u, (uint32)-1); cursor += 80u;
            }
            break;
        case 4u:
            if (r_u8(cursor + 32u))
            {
                if (r_u32(group + 92u)) return (uint32)-8;
                object = shell_sub_80045260(140u);
                if (!object) return (uint32)-3;
                w_u32(group + 92u, object);
            }
            else object = shell_sub_8004904C(group);
            if (!object) return (uint32)-3;
            shell_copy_string(object, cursor + 4u);
            shell_copy_bytes(object + 40u, cursor + 36u, 16u);
            shell_copy_bytes(object + 32u, cursor + 52u, 8u);
            w_u32(object + 56u, r_u32(cursor + 60u));
            w_u8(object + 60u, r_u8(cursor + 64u)); w_u8(object + 61u, r_u8(cursor + 65u));
            w_u32(object + 120u, r_u32(cursor + 68u));
            for (index = 0u; index < 3u; ++index) w_u32(object + 64u + index * 4u, r_u32(cursor + 72u));
            w_u32(object + 108u, (uint32)-1); cursor += 76u;
            break;
        case 8u:
            if (r_u8(cursor + 4u))
            {
                if (r_u32(group + 136u)) return (uint32)-8;
                object = shell_sub_80045260(320u);
                if (!object) return (uint32)-3;
                w_u32(group + 136u, object);
            }
            else object = shell_sub_80049190(group);
            if (!object) return (uint32)-3;
            shell_copy_string(object, cursor + 7u); shell_copy_string(object + 28u, cursor + 35u);
            shell_copy_string(object + 272u, cursor + 252u);
            shell_copy_bytes(object + 72u, cursor + 4u, 3u);
            shell_copy_bytes(object + 76u, cursor + 64u, 36u);
            shell_copy_bytes(object + 112u, cursor + 100u, 44u);
            shell_copy_bytes(object + 156u, cursor + 144u, 44u);
            shell_copy_bytes(object + 200u, cursor + 188u, 8u);
            w_u8(object + 208u, r_u8(cursor + 196u));
            shell_copy_bytes(object + 212u, cursor + 200u, 16u);
            shell_copy_bytes(object + 228u, cursor + 216u, 12u);
            shell_copy_bytes(object + 244u, cursor + 228u, 6u);
            shell_copy_bytes(object + 252u, cursor + 236u, 16u);
            cursor += 280u;
            break;
        case 7u:
            object = shell_sub_80049230(group);
            if (!object) return (uint32)-3;
            shell_copy_string(object, cursor + 4u);
            w_u32(object + 28u, r_u32(cursor + 32u)); w_u8(object + 32u, r_u8(cursor + 36u));
            cursor += 40u;
            break;
        case 9u:
            if (r_u32(screen + 72u)) return (uint32)-6;
            length = r_u32(cursor + 4u) * 4u;
            object = shell_sub_80045260(length + 8u);
            if (!object) return (uint32)-3;
            w_u32(object, r_u32(cursor + 4u)); w_u32(object + 4u, object + 8u);
            w_u32(cursor + 8u, cursor + 12u); w_u32(screen + 72u, object);
            shell_copy_bytes(object + 8u, cursor + 12u, length); cursor += length + 12u;
            break;
        default: return (uint32)-6;
        }
    } while (cursor < end);
    return 0u;
}

uint32 shell_sub_80041790(void)
{
    uint32 index[2] = {0}, resource = r_u32(0x8007BFB8u), screen;
    uint32 script, offset;
    FUNCTION_MARKER(0x80041790u, "SHELL.BIN");
    script = shell_resource_find(resource, 0x80039FC0u, index);
    moh_draft_missing_call(0x80049E90u, script,
        r_u32(r_u32(resource + 0x44u) + index[0] * 80u + 0x44u));
    for (screen = r_u32(0x8007BA6Cu); screen; screen = r_u32(screen + 0x80u))
    {
        uint32 data;
        for (offset = 0u; offset < 7u; ++offset)
        {
            uint32 left = moh_draft_missing_call(0x8001E2ACu, (uint32)r_u8(screen + offset)) & 255u;
            uint32 right = moh_draft_missing_call(0x8001E2ACu, (uint32)r_u8(0x80039FE0u + offset)) & 255u;
            if (left != right) break;
            if (!left) break;
        }
        if (offset != 7u) continue;
        w_u32(screen + 0x60u, 0x80041DF0u); w_u32(screen + 0x64u, 0x800429A0u);
        w_u32(screen + 0x70u, 0x80041E68u); w_u32(screen + 0x5Cu, 0x800419E8u);
        w_u32(screen + 0x6Cu, 0u); w_u32(screen + 0x58u, 0x80041E44u);
        for (offset = 0u; offset < 6u; ++offset)
            if (r_u32(screen + 0x5Cu + offset * 4u)) w_u8(screen + 0x74u + offset, 1u);
        data = shell_sub_80045260(4u);
        if (!data) return (uint32)-1;
        w_u32(data, r_u32(0x8007BFB4u)); w_u32(screen + 0x50u, data);
    }
    return 0u;
}

void shell_sub_8004DC40(void)
{
    uint32 index, mode = r_u32(0x80038650u), bank = r_u32(0x8007BFB4u);
    uint8 volume = (uint8)(r_u32(0x80038654u) * 21u);
    FUNCTION_MARKER(0x8004DC40u, "SHELL.BIN");
    w_u8(0x8007C371u, mode == 2u); w_u8(0x8007C370u, mode == 0u || mode == 2u);
    w_u8(bank + 17u, volume); w_u8(bank + 16u, volume); w_u16(0x8007C36Eu, volume);
    moh_draft_missing_call(0x8001E4DCu, (uint32)r_u8(0x8007C372u), (uint32)r_u8(0x8007C372u));
    for (index = 0u; index < 24u; ++index)
    {
        uint32 voice_volume = r_u8(0x8007C378u + index) ? 0u : volume;
        if (!voice_volume && r_u8(0x8007C378u + index)) w_u8(0x8007C378u + index, 1u);
        moh_draft_missing_call(0x8001E42Cu, index, voice_volume, voice_volume);
    }
    moh_draft_missing_call(r_u8(0x8007C370u) ? 0x8001E4CCu : 0x8001E4BCu);
    moh_draft_missing_call(0x8001E52Cu, (uint32)(r_u8(0x8007C371u) != 0u));
}

void shell_sub_80040D10(void)
{
    static const uint8 defaults[10] = {128, 128, 0, 0, 255, 0, 255, 255, 0, 255};
    uint32 target;
    FUNCTION_MARKER(0x80040D10u, "SHELL.BIN");
    if (!r_u32(0x8007A258u)) target = r_u32(0x8007A260u) ? 0x800386EAu : 0x800386E0u;
    else target = r_u32(0x8007A260u) ? 0x800386FFu : 0x800386F5u;
    w_u32(0x8007A264u, target);
    memcpy(moh_draft_guest_pointer(target, sizeof(defaults)), defaults, sizeof(defaults));
}

void shell_sub_8005D590(void)

{
  uint8 bVar1;
  uint8 cVar2;
  uint8 cVar3;
  uint8 cVar4;
  uint8 cVar5;
  uint8 cVar6;
  uint8 cVar7;
  uint8 cVar8;
  uint8 cVar9;
  uint8 cVar10;
  uint8 cVar11;
  uint8 cVar12;
  uint8 cVar13;
  uint8 cVar14;
  uint8 cVar15;
  uint8 cVar16;
  uint8 cVar17;
  uint8 cVar18;
  uint8 cVar19;
  uint8 cVar20;
  uint8 cVar21;
  uint8 cVar22;
  uint8 cVar23;
  uint8 cVar24;
  uint8 cVar25;
  uint8 cVar26;
  uint8 cVar27;
  uint8 cVar28;
  uint8 cVar29;
  uint8 cVar30;
  uint32 uVar31;
  uint32 uVar32;
  
  if ((*(uint8 *)moh_draft_guest_pointer(0x8007BDF8u, 1u)) == '\0') {
    (*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) = ~(uint32)(((uint32)(*(uint8 *)moh_draft_guest_pointer(0x8007BDFAu, 1u)) << 8u) | (uint32)(*(uint8 *)moh_draft_guest_pointer(0x8007BDFBu, 1u)));
    shell_sub_8005F3A4(0);
    (*(uint8 *)moh_draft_guest_pointer(0x8007BCC8u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BDFEu, 1u));
    (*(uint8 *)moh_draft_guest_pointer(0x8007BCC9u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BDFFu, 1u));
    (*(uint8 *)moh_draft_guest_pointer(0x8007BCCAu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BDFCu, 1u));
    (*(uint8 *)moh_draft_guest_pointer(0x8007BCCBu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BDFDu, 1u));
  }
  else {
    (*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) = 0;
    (*(uint32 *)moh_draft_guest_pointer(0x8007BE48u, 4u)) = 3;
  }
  if ((*(uint8 *)moh_draft_guest_pointer(0x8007BE20u, 1u)) == '\0') {
    (*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) = ~(uint32)(((uint32)(*(uint8 *)moh_draft_guest_pointer(0x8007BE22u, 1u)) << 8u) | (uint32)(*(uint8 *)moh_draft_guest_pointer(0x8007BE23u, 1u)));
    shell_sub_8005F3A4(1);
    (*(uint8 *)moh_draft_guest_pointer(0x8007BCCCu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BE26u, 1u));
    (*(uint8 *)moh_draft_guest_pointer(0x8007BCCDu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BE27u, 1u));
    (*(uint8 *)moh_draft_guest_pointer(0x8007BCCEu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BE24u, 1u));
    (*(uint8 *)moh_draft_guest_pointer(0x8007BCCFu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BE25u, 1u));
  }
  else {
    (*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) = 0;
    (*(uint32 *)moh_draft_guest_pointer(0x8007BE4Cu, 4u)) = 3;
  }
  cVar28 = (*(uint8 *)moh_draft_guest_pointer(0x8007BD98u, 1u));
  cVar27 = (*(uint8 *)moh_draft_guest_pointer(0x8007BD94u, 1u));
  cVar26 = (*(uint8 *)moh_draft_guest_pointer(0x8007BD90u, 1u));
  cVar25 = (*(uint8 *)moh_draft_guest_pointer(0x8007BD8Cu, 1u));
  cVar24 = (*(uint8 *)moh_draft_guest_pointer(0x8007BD88u, 1u));
  cVar23 = (*(uint8 *)moh_draft_guest_pointer(0x8007BD84u, 1u));
  cVar22 = (*(uint8 *)moh_draft_guest_pointer(0x8007BD80u, 1u));
  cVar21 = (*(uint8 *)moh_draft_guest_pointer(0x8007BD7Cu, 1u));
  cVar20 = (*(uint8 *)moh_draft_guest_pointer(0x8007BD78u, 1u));
  cVar19 = (*(uint8 *)moh_draft_guest_pointer(0x8007BD74u, 1u));
  cVar18 = (*(uint8 *)moh_draft_guest_pointer(0x8007BD70u, 1u));
  cVar17 = (*(uint8 *)moh_draft_guest_pointer(0x8007BD6Cu, 1u));
  cVar16 = (*(uint8 *)moh_draft_guest_pointer(0x8007BD68u, 1u));
  cVar15 = (*(uint8 *)moh_draft_guest_pointer(0x8007BD64u, 1u));
  cVar14 = (*(uint8 *)moh_draft_guest_pointer(0x8007BD10u, 1u));
  cVar13 = (*(uint8 *)moh_draft_guest_pointer(0x8007BD0Cu, 1u));
  cVar12 = (*(uint8 *)moh_draft_guest_pointer(0x8007BD08u, 1u));
  cVar11 = (*(uint8 *)moh_draft_guest_pointer(0x8007BD04u, 1u));
  cVar10 = (*(uint8 *)moh_draft_guest_pointer(0x8007BD00u, 1u));
  cVar9 = (*(uint8 *)moh_draft_guest_pointer(0x8007BCFCu, 1u));
  cVar8 = (*(uint8 *)moh_draft_guest_pointer(0x8007BCF8u, 1u));
  cVar7 = (*(uint8 *)moh_draft_guest_pointer(0x8007BCF4u, 1u));
  cVar6 = (*(uint8 *)moh_draft_guest_pointer(0x8007BCF0u, 1u));
  cVar5 = (*(uint8 *)moh_draft_guest_pointer(0x8007BCECu, 1u));
  cVar4 = (*(uint8 *)moh_draft_guest_pointer(0x8007BCE8u, 1u));
  cVar3 = (*(uint8 *)moh_draft_guest_pointer(0x8007BCE4u, 1u));
  cVar2 = (*(uint8 *)moh_draft_guest_pointer(0x8007BCE0u, 1u));
  cVar30 = (*(uint8 *)moh_draft_guest_pointer(0x8007BCDCu, 1u));
  uVar32 = (uint32)(*(uint8 *)moh_draft_guest_pointer(0x8007BD24u, 1u));
  uVar31 = (*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) >> 0xf & 1;
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD25u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD24u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD24u, 1u)) = (uint8)uVar31;
  if (uVar31 != uVar32) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD26u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD26u, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD26u, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD27u, 1u)) = 0;
    }
  }
  uVar32 = (uint32)(*(uint8 *)moh_draft_guest_pointer(0x8007BD28u, 1u));
  uVar31 = (*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) >> 0xd & 1;
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD29u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD28u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD28u, 1u)) = (uint8)uVar31;
  if (uVar31 != uVar32) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD2Au, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD2Au, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD2Au, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD2Bu, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD2Du, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD2Cu, 1u));
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) == 0) || (1 < (*(uint32 *)moh_draft_guest_pointer(0x8007BE48u, 4u)) - 1U)) || (cVar29 = '\x01', 8 < (*(uint8 *)moh_draft_guest_pointer(0x8007BCC8u, 1u)))) {
    cVar29 = '\0';
  }
  bVar1 = cVar29 != (*(uint8 *)moh_draft_guest_pointer(0x8007BD2Cu, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD2Cu, 1u)) = cVar29;
  if (bVar1) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD2Eu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD2Eu, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD2Eu, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD2Fu, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD31u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD30u, 1u));
  if (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) == 0) || (1 < (*(uint32 *)moh_draft_guest_pointer(0x8007BE48u, 4u)) - 1U)) {
    cVar29 = '\0';
  }
  else {
    cVar29 = '\0';
    if (0xf6 < (*(uint8 *)moh_draft_guest_pointer(0x8007BCC8u, 1u))) {
      cVar29 = '\x01';
    }
  }
  bVar1 = cVar29 != (*(uint8 *)moh_draft_guest_pointer(0x8007BD30u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD30u, 1u)) = cVar29;
  if (bVar1) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD32u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD32u, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD32u, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD33u, 1u)) = 0;
    }
  }
  uVar32 = (uint32)(*(uint8 *)moh_draft_guest_pointer(0x8007BD34u, 1u));
  uVar31 = (*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) >> 6 & 1;
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD35u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD34u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD34u, 1u)) = (uint8)uVar31;
  if (uVar31 != uVar32) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD36u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD36u, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD36u, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD37u, 1u)) = 0;
    }
  }
  uVar32 = (uint32)(*(uint8 *)moh_draft_guest_pointer(0x8007BD38u, 1u));
  uVar31 = (*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) >> 4 & 1;
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD39u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD38u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD38u, 1u)) = (uint8)uVar31;
  if (uVar31 != uVar32) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD3Au, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD3Au, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD3Au, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD3Bu, 1u)) = 0;
    }
  }
  uVar32 = (uint32)(*(uint8 *)moh_draft_guest_pointer(0x8007BD3Cu, 1u));
  uVar31 = (*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) >> 0xb & 1;
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD3Du, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD3Cu, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD3Cu, 1u)) = (uint8)uVar31;
  if (uVar31 != uVar32) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD3Eu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD3Eu, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD3Eu, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD3Fu, 1u)) = 0;
    }
  }
  uVar32 = (uint32)(*(uint8 *)moh_draft_guest_pointer(0x8007BD40u, 1u));
  uVar31 = (*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) >> 8 & 1;
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD41u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD40u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD40u, 1u)) = (uint8)uVar31;
  if (uVar31 != uVar32) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD42u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD42u, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD42u, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD43u, 1u)) = 0;
    }
  }
  uVar32 = (uint32)(*(uint8 *)moh_draft_guest_pointer(0x8007BDCCu, 1u));
  uVar31 = (*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) >> 0xf & 1;
  (*(uint8 *)moh_draft_guest_pointer(0x8007BDCDu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BDCCu, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BDCCu, 1u)) = (uint8)uVar31;
  if (uVar31 != uVar32) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BDCEu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BDCEu, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BDCEu, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BDCFu, 1u)) = 0;
    }
  }
  uVar32 = (uint32)(*(uint8 *)moh_draft_guest_pointer(0x8007BDD0u, 1u));
  uVar31 = (*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) >> 0xd & 1;
  (*(uint8 *)moh_draft_guest_pointer(0x8007BDD1u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BDD0u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BDD0u, 1u)) = (uint8)uVar31;
  if (uVar31 != uVar32) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BDD2u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BDD2u, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BDD2u, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BDD3u, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BDD5u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BDD4u, 1u));
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) == 0) || (1 < (*(uint32 *)moh_draft_guest_pointer(0x8007BE4Cu, 4u)) - 1U)) || (cVar29 = '\x01', 8 < (*(uint8 *)moh_draft_guest_pointer(0x8007BCCCu, 1u)))) {
    cVar29 = '\0';
  }
  bVar1 = cVar29 != (*(uint8 *)moh_draft_guest_pointer(0x8007BDD4u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BDD4u, 1u)) = cVar29;
  if (bVar1) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BDD6u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BDD6u, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BDD6u, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BDD7u, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BDD9u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BDD8u, 1u));
  if (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) == 0) || (1 < (*(uint32 *)moh_draft_guest_pointer(0x8007BE4Cu, 4u)) - 1U)) {
    cVar29 = '\0';
  }
  else {
    cVar29 = '\0';
    if (0xf6 < (*(uint8 *)moh_draft_guest_pointer(0x8007BCCCu, 1u))) {
      cVar29 = '\x01';
    }
  }
  bVar1 = cVar29 != (*(uint8 *)moh_draft_guest_pointer(0x8007BDD8u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BDD8u, 1u)) = cVar29;
  if (bVar1) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BDDAu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BDDAu, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BDDAu, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BDDBu, 1u)) = 0;
    }
  }
  uVar32 = (uint32)(*(uint8 *)moh_draft_guest_pointer(0x8007BDDCu, 1u));
  uVar31 = (*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) >> 6 & 1;
  (*(uint8 *)moh_draft_guest_pointer(0x8007BDDDu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BDDCu, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BDDCu, 1u)) = (uint8)uVar31;
  if (uVar31 != uVar32) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BDDEu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BDDEu, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BDDEu, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BDDFu, 1u)) = 0;
    }
  }
  uVar32 = (uint32)(*(uint8 *)moh_draft_guest_pointer(0x8007BDE0u, 1u));
  uVar31 = (*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) >> 4 & 1;
  (*(uint8 *)moh_draft_guest_pointer(0x8007BDE1u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BDE0u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BDE0u, 1u)) = (uint8)uVar31;
  if (uVar31 != uVar32) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BDE2u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BDE2u, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BDE2u, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BDE3u, 1u)) = 0;
    }
  }
  uVar32 = (uint32)(*(uint8 *)moh_draft_guest_pointer(0x8007BDE4u, 1u));
  uVar31 = (*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) >> 0xb & 1;
  (*(uint8 *)moh_draft_guest_pointer(0x8007BDE5u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BDE4u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BDE4u, 1u)) = (uint8)uVar31;
  if (uVar31 != uVar32) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BDE6u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BDE6u, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BDE6u, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BDE7u, 1u)) = 0;
    }
  }
  uVar32 = (uint32)(*(uint8 *)moh_draft_guest_pointer(0x8007BDE8u, 1u));
  uVar31 = (*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) >> 8 & 1;
  (*(uint8 *)moh_draft_guest_pointer(0x8007BDE9u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BDE8u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BDE8u, 1u)) = (uint8)uVar31;
  if (uVar31 != uVar32) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BDEAu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BDEAu, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BDEAu, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BDEBu, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BCDDu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BCDCu, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BCDCu, 1u)) = '\0';
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) & 0x1000) != 0) || (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) & 0x1000) != 0)) {
    (*(uint8 *)moh_draft_guest_pointer(0x8007BCDCu, 1u)) = '\x01';
  }
  if (cVar30 != (*(uint8 *)moh_draft_guest_pointer(0x8007BCDCu, 1u))) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BCDEu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BCDEu, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BCDEu, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BCDFu, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD65u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD64u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD64u, 1u)) = '\0';
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) & 0x1000) != 0) || (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) & 0x1000) != 0)) {
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD64u, 1u)) = '\x01';
  }
  if (cVar15 != (*(uint8 *)moh_draft_guest_pointer(0x8007BD64u, 1u))) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD66u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD66u, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD66u, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD67u, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BCE1u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BCE0u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BCE0u, 1u)) = '\0';
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) & 0x4000) != 0) || (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) & 0x4000) != 0)) {
    (*(uint8 *)moh_draft_guest_pointer(0x8007BCE0u, 1u)) = '\x01';
  }
  if (cVar2 != (*(uint8 *)moh_draft_guest_pointer(0x8007BCE0u, 1u))) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BCE2u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BCE2u, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BCE2u, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BCE3u, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD69u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD68u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD68u, 1u)) = '\0';
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) & 0x4000) != 0) || (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) & 0x4000) != 0)) {
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD68u, 1u)) = '\x01';
  }
  if (cVar16 != (*(uint8 *)moh_draft_guest_pointer(0x8007BD68u, 1u))) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD6Au, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD6Au, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD6Au, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD6Bu, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BCE5u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BCE4u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BCE4u, 1u)) = '\0';
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) & 0x8000) != 0) || (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) & 0x8000) != 0)) {
    (*(uint8 *)moh_draft_guest_pointer(0x8007BCE4u, 1u)) = '\x01';
  }
  if (cVar3 != (*(uint8 *)moh_draft_guest_pointer(0x8007BCE4u, 1u))) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BCE6u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BCE6u, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BCE6u, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BCE7u, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD6Du, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD6Cu, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD6Cu, 1u)) = '\0';
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) & 0x8000) != 0) || (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) & 0x8000) != 0)) {
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD6Cu, 1u)) = '\x01';
  }
  if (cVar17 != (*(uint8 *)moh_draft_guest_pointer(0x8007BD6Cu, 1u))) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD6Eu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD6Eu, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD6Eu, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD6Fu, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BCE9u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BCE8u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BCE8u, 1u)) = '\0';
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) & 0x2000) != 0) || (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) & 0x2000) != 0)) {
    (*(uint8 *)moh_draft_guest_pointer(0x8007BCE8u, 1u)) = '\x01';
  }
  if (cVar4 != (*(uint8 *)moh_draft_guest_pointer(0x8007BCE8u, 1u))) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BCEAu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BCEAu, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BCEAu, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BCEBu, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD71u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD70u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD70u, 1u)) = '\0';
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) & 0x2000) != 0) || (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) & 0x2000) != 0)) {
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD70u, 1u)) = '\x01';
  }
  if (cVar18 != (*(uint8 *)moh_draft_guest_pointer(0x8007BD70u, 1u))) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD72u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD72u, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD72u, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD73u, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BCEDu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BCECu, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BCECu, 1u)) = '\0';
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) & 4) != 0) || (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) & 4) != 0)) {
    (*(uint8 *)moh_draft_guest_pointer(0x8007BCECu, 1u)) = '\x01';
  }
  if (cVar5 != (*(uint8 *)moh_draft_guest_pointer(0x8007BCECu, 1u))) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BCEEu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BCEEu, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BCEEu, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BCEFu, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD75u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD74u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD74u, 1u)) = '\0';
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) & 4) != 0) || (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) & 4) != 0)) {
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD74u, 1u)) = '\x01';
  }
  if (cVar19 != (*(uint8 *)moh_draft_guest_pointer(0x8007BD74u, 1u))) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD76u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD76u, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD76u, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD77u, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BCF1u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BCF0u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BCF0u, 1u)) = '\0';
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) & 1) != 0) || (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) & 1) != 0)) {
    (*(uint8 *)moh_draft_guest_pointer(0x8007BCF0u, 1u)) = '\x01';
  }
  if (cVar6 != (*(uint8 *)moh_draft_guest_pointer(0x8007BCF0u, 1u))) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BCF2u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BCF2u, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BCF2u, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BCF3u, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD79u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD78u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD78u, 1u)) = '\0';
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) & 1) != 0) || (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) & 1) != 0)) {
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD78u, 1u)) = '\x01';
  }
  if (cVar20 != (*(uint8 *)moh_draft_guest_pointer(0x8007BD78u, 1u))) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD7Au, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD7Au, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD7Au, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD7Bu, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BCF5u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BCF4u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BCF4u, 1u)) = '\0';
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) & 0x10) != 0) || (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) & 0x10) != 0)) {
    (*(uint8 *)moh_draft_guest_pointer(0x8007BCF4u, 1u)) = '\x01';
  }
  if (cVar7 != (*(uint8 *)moh_draft_guest_pointer(0x8007BCF4u, 1u))) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BCF6u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BCF6u, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BCF6u, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BCF7u, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD7Du, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD7Cu, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD7Cu, 1u)) = '\0';
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) & 0x10) != 0) || (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) & 0x10) != 0)) {
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD7Cu, 1u)) = '\x01';
  }
  if (cVar21 != (*(uint8 *)moh_draft_guest_pointer(0x8007BD7Cu, 1u))) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD7Eu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD7Eu, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD7Eu, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD7Fu, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BCF9u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BCF8u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BCF8u, 1u)) = '\0';
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) & 0x40) != 0) || (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) & 0x40) != 0)) {
    (*(uint8 *)moh_draft_guest_pointer(0x8007BCF8u, 1u)) = '\x01';
  }
  if (cVar8 != (*(uint8 *)moh_draft_guest_pointer(0x8007BCF8u, 1u))) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BCFAu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BCFAu, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BCFAu, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BCFBu, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD81u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD80u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD80u, 1u)) = '\0';
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) & 0x40) != 0) || (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) & 0x40) != 0)) {
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD80u, 1u)) = '\x01';
  }
  if (cVar22 != (*(uint8 *)moh_draft_guest_pointer(0x8007BD80u, 1u))) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD82u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD82u, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD82u, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD83u, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BCFDu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BCFCu, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BCFCu, 1u)) = '\0';
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) & 0x80) != 0) || (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) & 0x80) != 0)) {
    (*(uint8 *)moh_draft_guest_pointer(0x8007BCFCu, 1u)) = '\x01';
  }
  if (cVar9 != (*(uint8 *)moh_draft_guest_pointer(0x8007BCFCu, 1u))) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BCFEu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BCFEu, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BCFEu, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BCFFu, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD85u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD84u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD84u, 1u)) = '\0';
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) & 0x80) != 0) || (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) & 0x80) != 0)) {
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD84u, 1u)) = '\x01';
  }
  if (cVar23 != (*(uint8 *)moh_draft_guest_pointer(0x8007BD84u, 1u))) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD86u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD86u, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD86u, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD87u, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD01u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD00u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD00u, 1u)) = '\0';
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) & 0x20) != 0) || (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) & 0x20) != 0)) {
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD00u, 1u)) = '\x01';
  }
  if (cVar10 != (*(uint8 *)moh_draft_guest_pointer(0x8007BD00u, 1u))) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD02u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD02u, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD02u, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD03u, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD89u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD88u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD88u, 1u)) = '\0';
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) & 0x20) != 0) || (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) & 0x20) != 0)) {
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD88u, 1u)) = '\x01';
  }
  if (cVar24 != (*(uint8 *)moh_draft_guest_pointer(0x8007BD88u, 1u))) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD8Au, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD8Au, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD8Au, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD8Bu, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD05u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD04u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD04u, 1u)) = '\0';
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) & 8) != 0) || (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) & 8) != 0)) {
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD04u, 1u)) = '\x01';
  }
  if (cVar11 != (*(uint8 *)moh_draft_guest_pointer(0x8007BD04u, 1u))) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD06u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD06u, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD06u, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD07u, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD8Du, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD8Cu, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD8Cu, 1u)) = '\0';
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) & 8) != 0) || (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) & 8) != 0)) {
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD8Cu, 1u)) = '\x01';
  }
  if (cVar25 != (*(uint8 *)moh_draft_guest_pointer(0x8007BD8Cu, 1u))) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD8Eu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD8Eu, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD8Eu, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD8Fu, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD09u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD08u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD08u, 1u)) = '\0';
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) & 2) != 0) || (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) & 2) != 0)) {
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD08u, 1u)) = '\x01';
  }
  if (cVar12 != (*(uint8 *)moh_draft_guest_pointer(0x8007BD08u, 1u))) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD0Au, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD0Au, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD0Au, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD0Bu, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD91u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD90u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD90u, 1u)) = '\0';
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) & 2) != 0) || (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) & 2) != 0)) {
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD90u, 1u)) = '\x01';
  }
  if (cVar26 != (*(uint8 *)moh_draft_guest_pointer(0x8007BD90u, 1u))) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD92u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD92u, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD92u, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD93u, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD0Du, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD0Cu, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD0Cu, 1u)) = '\0';
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) & 0x800) != 0) || (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) & 0x800) != 0)) {
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD0Cu, 1u)) = '\x01';
  }
  if (cVar13 != (*(uint8 *)moh_draft_guest_pointer(0x8007BD0Cu, 1u))) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD0Eu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD0Eu, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD0Eu, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD0Fu, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD95u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD94u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD94u, 1u)) = '\0';
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) & 0x800) != 0) || (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) & 0x800) != 0)) {
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD94u, 1u)) = '\x01';
  }
  if (cVar27 != (*(uint8 *)moh_draft_guest_pointer(0x8007BD94u, 1u))) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD96u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD96u, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD96u, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD97u, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD11u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD10u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD10u, 1u)) = '\0';
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) & 0x100) != 0) || (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) & 0x100) != 0)) {
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD10u, 1u)) = '\x01';
  }
  if (cVar14 != (*(uint8 *)moh_draft_guest_pointer(0x8007BD10u, 1u))) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD12u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD12u, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD12u, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD13u, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD99u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD98u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD98u, 1u)) = '\0';
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) & 0x100) != 0) || (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) & 0x100) != 0)) {
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD98u, 1u)) = '\x01';
  }
  if (cVar28 != (*(uint8 *)moh_draft_guest_pointer(0x8007BD98u, 1u))) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD9Au, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD9Au, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD9Au, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD9Bu, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD15u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD14u, 1u));
  if (((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) == 0) || (1 < (*(uint32 *)moh_draft_guest_pointer(0x8007BE48u, 4u)) - 1U)) || (cVar30 = '\x01', 8 < (*(uint8 *)moh_draft_guest_pointer(0x8007BCC8u, 1u)))) &&
     ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) == 0 || (1 < (*(uint32 *)moh_draft_guest_pointer(0x8007BE4Cu, 4u)) - 1U)) || (cVar30 = '\x01', 8 < (*(uint8 *)moh_draft_guest_pointer(0x8007BCCCu, 1u)))))) {
    cVar30 = '\0';
  }
  bVar1 = cVar30 != (*(uint8 *)moh_draft_guest_pointer(0x8007BD14u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD14u, 1u)) = cVar30;
  if (bVar1) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD16u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD16u, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD16u, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD17u, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD9Du, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD9Cu, 1u));
  if (((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) == 0) || (1 < (*(uint32 *)moh_draft_guest_pointer(0x8007BE48u, 4u)) - 1U)) || (cVar30 = '\x01', 8 < (*(uint8 *)moh_draft_guest_pointer(0x8007BCC8u, 1u)))) &&
     ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) == 0 || (1 < (*(uint32 *)moh_draft_guest_pointer(0x8007BE4Cu, 4u)) - 1U)) || (cVar30 = '\x01', 8 < (*(uint8 *)moh_draft_guest_pointer(0x8007BCCCu, 1u)))))) {
    cVar30 = '\0';
  }
  bVar1 = cVar30 != (*(uint8 *)moh_draft_guest_pointer(0x8007BD9Cu, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD9Cu, 1u)) = cVar30;
  if (bVar1) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD9Eu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD9Eu, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD9Eu, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD9Fu, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD19u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD18u, 1u));
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) == 0) || (1 < (*(uint32 *)moh_draft_guest_pointer(0x8007BE48u, 4u)) - 1U)) || (cVar30 = '\x01', (*(uint8 *)moh_draft_guest_pointer(0x8007BCC8u, 1u)) < 0xf7)) {
    if (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) == 0) || (1 < (*(uint32 *)moh_draft_guest_pointer(0x8007BE4Cu, 4u)) - 1U)) {
      cVar30 = '\0';
    }
    else {
      cVar30 = '\0';
      if (0xf6 < (*(uint8 *)moh_draft_guest_pointer(0x8007BCCCu, 1u))) {
        cVar30 = '\x01';
      }
    }
  }
  bVar1 = cVar30 != (*(uint8 *)moh_draft_guest_pointer(0x8007BD18u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD18u, 1u)) = cVar30;
  if (bVar1) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD1Au, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD1Au, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD1Au, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD1Bu, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BDA1u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BDA0u, 1u));
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) == 0) || (1 < (*(uint32 *)moh_draft_guest_pointer(0x8007BE48u, 4u)) - 1U)) || (cVar30 = '\x01', (*(uint8 *)moh_draft_guest_pointer(0x8007BCC8u, 1u)) < 0xf7)) {
    if (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) == 0) || (1 < (*(uint32 *)moh_draft_guest_pointer(0x8007BE4Cu, 4u)) - 1U)) {
      cVar30 = '\0';
    }
    else {
      cVar30 = '\0';
      if (0xf6 < (*(uint8 *)moh_draft_guest_pointer(0x8007BCCCu, 1u))) {
        cVar30 = '\x01';
      }
    }
  }
  bVar1 = cVar30 != (*(uint8 *)moh_draft_guest_pointer(0x8007BDA0u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BDA0u, 1u)) = cVar30;
  if (bVar1) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BDA2u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BDA2u, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BDA2u, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BDA3u, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD1Du, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD1Cu, 1u));
  if (((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) == 0) || (1 < (*(uint32 *)moh_draft_guest_pointer(0x8007BE48u, 4u)) - 1U)) || (cVar30 = '\x01', 8 < (*(uint8 *)moh_draft_guest_pointer(0x8007BCC9u, 1u)))) &&
     ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) == 0 || (1 < (*(uint32 *)moh_draft_guest_pointer(0x8007BE4Cu, 4u)) - 1U)) || (cVar30 = '\x01', 8 < (*(uint8 *)moh_draft_guest_pointer(0x8007BCCDu, 1u)))))) {
    cVar30 = '\0';
  }
  bVar1 = cVar30 != (*(uint8 *)moh_draft_guest_pointer(0x8007BD1Cu, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD1Cu, 1u)) = cVar30;
  if (bVar1) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD1Eu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD1Eu, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD1Eu, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD1Fu, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BDA5u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BDA4u, 1u));
  if (((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) == 0) || (1 < (*(uint32 *)moh_draft_guest_pointer(0x8007BE48u, 4u)) - 1U)) || (cVar30 = '\x01', 8 < (*(uint8 *)moh_draft_guest_pointer(0x8007BCC9u, 1u)))) &&
     ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) == 0 || (1 < (*(uint32 *)moh_draft_guest_pointer(0x8007BE4Cu, 4u)) - 1U)) || (cVar30 = '\x01', 8 < (*(uint8 *)moh_draft_guest_pointer(0x8007BCCDu, 1u)))))) {
    cVar30 = '\0';
  }
  bVar1 = cVar30 != (*(uint8 *)moh_draft_guest_pointer(0x8007BDA4u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BDA4u, 1u)) = cVar30;
  if (bVar1) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BDA6u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BDA6u, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BDA6u, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BDA7u, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD21u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD20u, 1u));
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) == 0) || (1 < (*(uint32 *)moh_draft_guest_pointer(0x8007BE48u, 4u)) - 1U)) || (cVar30 = '\x01', (*(uint8 *)moh_draft_guest_pointer(0x8007BCC9u, 1u)) < 0xf7)) {
    if (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) == 0) || (1 < (*(uint32 *)moh_draft_guest_pointer(0x8007BE4Cu, 4u)) - 1U)) {
      cVar30 = '\0';
    }
    else {
      cVar30 = '\0';
      if (0xf6 < (*(uint8 *)moh_draft_guest_pointer(0x8007BCCDu, 1u))) {
        cVar30 = '\x01';
      }
    }
  }
  bVar1 = cVar30 != (*(uint8 *)moh_draft_guest_pointer(0x8007BD20u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BD20u, 1u)) = cVar30;
  if (bVar1) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BD22u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BD22u, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BD22u, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BD23u, 1u)) = 0;
    }
  }
  (*(uint8 *)moh_draft_guest_pointer(0x8007BDA9u, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BDA8u, 1u));
  if ((((*(uint32 *)moh_draft_guest_pointer(0x8007BDF0u, 4u)) == 0) || (1 < (*(uint32 *)moh_draft_guest_pointer(0x8007BE48u, 4u)) - 1U)) || (cVar30 = '\x01', (*(uint8 *)moh_draft_guest_pointer(0x8007BCC9u, 1u)) < 0xf7)) {
    if (((*(uint32 *)moh_draft_guest_pointer(0x8007BDF4u, 4u)) == 0) || (1 < (*(uint32 *)moh_draft_guest_pointer(0x8007BE4Cu, 4u)) - 1U)) {
      cVar30 = '\0';
    }
    else {
      cVar30 = '\0';
      if (0xf6 < (*(uint8 *)moh_draft_guest_pointer(0x8007BCCDu, 1u))) {
        cVar30 = '\x01';
      }
    }
  }
  bVar1 = cVar30 != (*(uint8 *)moh_draft_guest_pointer(0x8007BDA8u, 1u));
  (*(uint8 *)moh_draft_guest_pointer(0x8007BDA8u, 1u)) = cVar30;
  if (bVar1) {
    (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) = (*(uint32 *)moh_draft_guest_pointer(0x80039C68u, 4u)) + 1;
    (*(uint8 *)moh_draft_guest_pointer(0x8007BDAAu, 1u)) = (*(uint8 *)moh_draft_guest_pointer(0x8007BDAAu, 1u)) == '\0';
    if (!(uint8)(*(uint8 *)moh_draft_guest_pointer(0x8007BDAAu, 1u))) {
      (*(uint8 *)moh_draft_guest_pointer(0x8007BDABu, 1u)) = 0;
    }
  }
  return;
}

void shell_sub_800419E8(uint32 screen)
{
    uint32 framed, index, node, resource, name_index;
    FUNCTION_MARKER(0x800419E8u, "SHELL.BIN");
    w_u8(screen + 118u, 1u); w_u8(screen + 121u, 1u); w_u8(screen + 120u, 1u);
    w_u8(screen + 116u, 0u); w_u8(screen + 117u, 0u);
    w_u8(0x8007A274u, 0u);
    framed = shell_names_equal(screen, 0x80039FE8u) || shell_names_equal(screen, 0x80039FFCu);
    if (!framed)
    {
        framed = 1u;
        for (index = 0u; index < 9u; ++index)
            if ((uint8)moh_draft_missing_call(0x8001E2ACu, (uint32)r_u8(screen + index)) !=
                (uint8)moh_draft_missing_call(0x8001E2ACu, (uint32)r_u8(0x8003A010u + index))) framed = 0u;
        if (r_u8(0x8003A019u)) framed = 0u;
    }
    if (framed)
    {
        w_u32(0x8007BB8Cu, 0x80042A88u);
        resource = shell_sub_8005F580(0x8003A11Cu, 0u, 1u);
        for (index = 0u; index < 4u; ++index)
        {
            /* Use a temporary game object without reproducing the guest stack */
            node = shell_sub_80045260(220u);
            for (name_index = 0u; name_index < 64u; ++name_index)
            {
                uint8 character = r_u8(0x8003A01Cu + index * 64u + name_index);
                w_u8(node + name_index, character);
                if (!character) break;
            }
            w_u8(node + 44u, 0u); w_u8(node + 56u, 0u); w_u8(node + 140u, 0u);
            moh_draft_missing_call(0x8004B10Cu, node, resource, 0u);
            shell_sub_8003D890(r_u32(node + 28u));
            w_u32(node + 28u, 0u);
            shell_sub_80045328(node);
        }
    }
    else w_u32(0x8007BB8Cu, 0u);
    shell_sub_80056CB4(r_u32(0x8007A228u), r_u32(0x8007A228u) + 0x40000u);
}

void shell_sub_80056CB4(uint32 first, uint32 second)
{
    FUNCTION_MARKER(0x80056CB4u, "SHELL.BIN");
    w_u32(0x8007BB94u, first);
    w_u32(0x8007BB98u, second);
}

uint32 shell_sub_8004D9D8(void)
{
    static const uint32 callback_offsets[3] = {92u, 104u, 100u};
    static const uint32 flag_offsets[3] = {116u, 119u, 118u};
    uint32 screen = r_u32(0x8007BA68u), result = 0u, callback, index, buffer, packet;
    PSX_RECT rectangle = {0, 0, 512, 480};
    FUNCTION_MARKER(0x8004D9D8u, "SHELL.BIN");
    if (moh_native_movie_screen(screen)) return 0u;
    for (index = 0u; index < 3u; ++index)
    {
        callback = r_u32(screen + callback_offsets[index]);
        if (r_u8(screen + flag_offsets[index]) && callback) {
            moh_draft_indirect_call(callback, screen);
            if (!index) moh_native_note_screen_initialized(screen);
        }
    }
    callback = r_u32(screen + 108u);
    if (r_u8(screen + 120u) && callback) result = moh_draft_indirect_call(callback, screen);
    callback = r_u32(screen + 112u);
    if (r_u8(screen + 121u) && callback) moh_draft_indirect_call(callback, screen, result);
    callback = r_u32(screen + 96u);
    if (r_u8(screen + 117u) && callback)
    {
        moh_draft_missing_call(0x80016E38u, 0u);
        moh_draft_indirect_call(callback, screen);
        moh_draft_missing_call(0x80016E38u, 0u);
        ClearImage(&rectangle, 0u, 0u, 0u);
        moh_draft_missing_call(0x80016E38u, 0u);
        buffer = r_u32(0x8007BF4Cu);
        w_u32(0x8007BF88u, moh_draft_missing_call(0x80016E38u, 0u));
        if (r_u8(0x8007BF98u))
        {
            packet = r_u32(0x8007BF9Cu);
            for (index = 0u; (sint32)index < r_s32(0x8007BFA4u); ++index, packet += 12u)
                moh_draft_missing_call(0x8001704Cu, packet, r_u32(packet + 8u));
            w_u32(0x8007BFA4u, 0u);
        }
        w_u32(0x8007BF8Cu, moh_draft_missing_call(0x80015DB8u, 2u));
        moh_draft_missing_call(0x80017408u, buffer + 92u);
        moh_draft_missing_call(0x80017348u, buffer);
        moh_draft_missing_call(0x800172D8u, r_u32(buffer + 116u) + 0x17FCu);
        moh_draft_missing_call(0x8001722Cu, r_u32(0x8007BED0u), 0x600u);
        moh_draft_missing_call(0x8001722Cu, r_u32(0x8007BF48u), 0x600u);
        if (r_u8(0x8007BF91u))
        {
            moh_draft_missing_call(0x8001722Cu, r_u32(0x8007BECCu), 0x600u);
            moh_draft_missing_call(0x8001722Cu, r_u32(0x8007BF44u), 0x600u);
        }
    }
    return result == 1u;
}

void shell_sub_8005F3A4(uint32 player)
{
    uint32 mode_address = 0x8007BE48u + player * 4u;
    uint32 reconnect, packet, type, state;
    FUNCTION_MARKER(0x8005F3A4u, "SHELL.BIN");
    reconnect = (r_u8(0x8007BCD4u + player) && r_u32(mode_address) != 3u) ||
        r_u8(0x8007BCD0u + player);
    w_u8(0x8007BCD0u + player, 0u);
    w_u8(0x8007BCD4u + player, r_u32(mode_address) == 3u);
    w_u32(mode_address, 0u);
    packet = player ? 0x8007BE20u : 0x8007BDF8u;
    type = r_u8(packet + 1u) >> 4u;
    if (reconnect || r_u8(0x8007BCD8u + player))
    {
        state = moh_draft_missing_call(0x8001B3D8u, player);
        if (state == 2u || state == 6u)
        {
            w_u8(0x8007BCD8u + player, 0u);
            moh_draft_missing_call(0x8001B728u, player << 4u, 1u, 0u);
        }
        else w_u8(0x8007BCD8u + player, 1u);
    }
    if (type == 7u)
    {
        state = moh_draft_missing_call(0x8001B2D8u, player ? 16u : 0u, 2u, 0u);
        w_u32(mode_address, state ? 2u : 1u);
    }
}

void shell_sub_80060C7C(void)
{
    static const uint32 values[24] = {0u,0u,3u,1u,0u,4u,2u,0u,4u,3u,0u,3u,
        4u,0u,4u,6u,0u,3u,7u,0u,3u,8u,0u,3u};
    uint32 index;
    FUNCTION_MARKER(0x80060C7Cu, "SHELL.BIN");
    w_u32(0x80038674u, 0u);
    for (index = 0u; index < 24u; ++index) w_u32(0x80038D90u + index * 4u, values[index]);
}

void shell_sub_80061F00(void)
{
    static const uint32 zeroes[] = {0x80038660u,0x80038664u,0x8003869Cu,0x800386A0u,
        0x800386A4u,0x8003866Cu,0x80038670u,0x80038678u,0x80038640u,0x80039C8Cu,
        0x80039C7Cu,0x80039C74u,0x80039C90u,0x80039C88u,0x80039C84u};
    uint32 index, offset, row, sample, block;
    FUNCTION_MARKER(0x80061F00u, "SHELL.BIN");
    shell_sub_80060C7C();
    memcpy(moh_draft_guest_pointer(0x800385D0u, 7u), moh_draft_guest_pointer(0x8003AAE4u, 7u), 7u);
    memcpy(moh_draft_guest_pointer(0x800385CCu, 4u), moh_draft_guest_pointer(0x8003AAF0u, 4u), 4u);
    memcpy(moh_draft_guest_pointer(0x80038D48u, 4u), moh_draft_guest_pointer(0x8003AAF0u, 4u), 4u);
    w_u32(0x80038668u,5u); w_u32(0x80039C58u,4u);
    w_u32(0x80038644u,127u); w_u32(0x80038648u,31u);
    for (index = 0u; index < 12u; ++index) w_u32(0x800385DCu + index * 4u, 0u);
    for (offset = 0u; offset < 0x5F0u; offset += 0x98u)
    {
        w_u32(0x800385CCu + offset + 0x21Cu,0u);
        w_u32(0x800385CCu + offset + 0x220u,0u);
        for (row = 0u; row < 4u; ++row)
        {
            block = offset + row * 0x24u;
            w_u32(0x80038778u + block,0u);
            w_u16(0x80038772u + block,0xFFFFu);
            for (sample = 0u; sample < 13u; ++sample) w_u16(0x80038758u + block + sample * 2u,0xFFFFu);
            w_u32(0x80038774u + block,row ? 0u : 0x800000u);
        }
    }
    w_u32(0x80038778u,1u);
    for (index = 0u; index < 13u; ++index) w_u32(0x8003863Cu - index * 4u,0u);
    for (index = 0u; index < sizeof(zeroes)/sizeof(zeroes[0]); ++index) w_u32(zeroes[index],0u);
    w_u32(0x80039C78u,1u);
}

void shell_sub_80060B0C(void)
{
    static const uint32 enabled[] = {0x800386B0u, 0x800386ACu, 0x800386C4u,
        0x800386C0u, 0x800386D4u, 0x800386D8u};
    uint32 index;
    FUNCTION_MARKER(0x80060B0Cu, "SHELL.BIN");
    memset(moh_draft_guest_pointer(0x800385CCu, 0x780u), 0, 0x780u);
    w_u32(0x80038668u, 5u); w_u32(0x8003864Cu, 4u); w_u32(0x80038654u, 4u);
    w_u8(0x800386DFu, 1u); w_u8(0x800386F4u, 1u);
    for (index = 0u; index < sizeof(enabled) / sizeof(enabled[0]); ++index) w_u32(enabled[index], 1u);
    for (index = 0u; index < 4u; ++index)
    {
        w_u32(0x8007A258u, index >> 1u); w_u32(0x8007A260u, index & 1u);
        shell_sub_80040D10();
    }
    w_u32(0x8007A258u, 0u); w_u32(0x8007A260u, 0u);
    memcpy(moh_draft_guest_pointer(0x800385D0u, 7u), moh_draft_guest_pointer(0x8003AAE4u, 7u), 7u);
    memcpy(moh_draft_guest_pointer(0x800385CCu, 4u), moh_draft_guest_pointer(0x8003AAF0u, 4u), 4u);
    memcpy(moh_draft_guest_pointer(0x80038D48u, 4u), moh_draft_guest_pointer(0x8003AAF0u, 4u), 4u);
    w_u8(0x8007BFD1u, 0u); w_u8(0x8007BFD0u, 0u); w_u8(0x8007BFD2u, 0u);
    w_u8(0x8007C006u, 0u); w_u8(0x8007C007u, 0u);
    w_u32(0x80039C58u, 4u); w_u32(0x80039D1Cu, 0u); w_u32(0x80039C90u, 0u);
}

void shell_sub_80063940(const void *media, const void *rectangle)
{
    FUNCTION_MARKER(0x80063940u, "SHELL.BIN");
    memset(moh_draft_guest_pointer(0x8007E6F0u, 0xA4u), 0, 0xA4u);
    if (media) memcpy(moh_draft_guest_pointer(0x8007E720u, 16u), media, 16u);
    if (rectangle) memcpy(moh_draft_guest_pointer(0x8007E730u, 8u), rectangle, 8u);
}

void shell_sub_800631D0(void)
{
    uint32 index;
    FUNCTION_MARKER(0x800631D0u, "SHELL.BIN");
    memset(moh_draft_guest_pointer(0x8007C244u, 0x124u), 0, 0x124u);
    VSync(r_u32(0x80039C5Cu) ? 16 : 1);
    sub_800203F8();
    moh_draft_missing_call(0x800156DCu, 0x66FFFu);
    moh_draft_missing_call(0x80020268u, 0x19000u);
    moh_native_sound_init();
    moh_draft_missing_call(0x8001FDB8u, 1u);
    moh_draft_missing_call(0x8001E4DCu, 127u, 127u);
    moh_draft_missing_call(0x80020138u);
    moh_draft_missing_call(0x800201F8u);
    moh_draft_missing_call(0x8001FC78u, 0u);
    moh_draft_missing_call(0x8001F70Cu, 0u, 0u);
    moh_draft_missing_call(0x80020968u, 0u, 0u, 1u);
    moh_draft_missing_call(0x8001E2ECu, 0u, 127u, 127u);
    moh_draft_missing_call(0x8001FC78u, 3u);
    moh_draft_missing_call(0x8001FC58u);
    moh_draft_missing_call(0x8001F70Cu, 40u, 40u);
    moh_draft_missing_call(0x80063834u, 64u);
    moh_draft_missing_call(0x800637FCu);
    moh_draft_missing_call(r_u8(0x8007C36Au) ? 0x8001E4CCu : 0x8001E4BCu);
    for (index = 0u; index < 16u; ++index) w_u32(0x8007C340u - index * 4u, 0u);
}

static uint32 shell_find_screen(uint32 name)
{
    uint32 node = r_u32(0x8007BA6Cu);
    while (node)
    {
        uint32 offset = 0u;
        sint32 comparison = 0;
        while (r_u8(node + offset))
        {
            uint32 left = moh_draft_missing_call(0x8001E2ACu, (uint32)r_u8(node + offset)) & 255u;
            uint32 right = moh_draft_missing_call(0x8001E2ACu, (uint32)r_u8(name + offset)) & 255u;
            if (left != right) { comparison = left > right ? 1 : -1; break; }
            ++offset;
        }
        if (r_u8(name + offset)) comparison = -1;
        if (!comparison) return node;
        node = r_u32(node + 0x80u);
    }
    return 0u;
}

static uint32 shell_find_named_item(uint32 node, uint32 name)
{
    for (; node; node = r_u32(node + 180u))
    {
        uint32 offset = 0u;
        while (r_u8(node + offset))
        {
            uint32 left = moh_draft_missing_call(0x8001E2ACu, (uint32)r_u8(node + offset)) & 255u;
            uint32 right = moh_draft_missing_call(0x8001E2ACu, (uint32)r_u8(name + offset)) & 255u;
            if (left != right) break;
            ++offset;
        }
        if (!r_u8(node + offset) && !r_u8(name + offset)) return node;
    }
    return 0u;
}

static void shell_select_bound_item(uint32 owner, uint32 binding_offset, uint32 count_offset,
    uint32 head_offset, uint32 next_offset, uint32 index_offset, uint32 selected_offset)
{
    uint32 binding = r_u32(owner + binding_offset), node;
    sint32 selected, last;
    if (!binding) return;
    selected = r_s32(binding);
    if (selected < 0) selected = 0;
    last = (sint32)(r_u32(owner + count_offset) - 1u);
    if (last < selected) selected = last;
    for (node = r_u32(owner + head_offset); node; node = r_u32(node + next_offset))
        if (r_s32(node + index_offset) == selected) { w_u32(owner + selected_offset, node); break; }
}

uint32 shell_sub_8004D838(uint32 screen)
{
    uint32 item;
    FUNCTION_MARKER(0x8004D838u, "SHELL.BIN");
    if (!screen) return (uint32)-2;
    shell_select_bound_item(screen, 44u, 40u, 32u, 180u, 56u, 28u);
    for (item = r_u32(screen + 32u); item; item = r_u32(item + 180u))
    {
        shell_select_bound_item(item, 132u, 128u, 120u, 208u, 216u, 116u);
        shell_select_bound_item(item, 156u, 152u, 144u, 312u, 304u, 140u);
    }
    return 0u;
}

void shell_sub_80043DF0(uint32 screen)
{
    uint32 first = 0u, head, inversion, config, vibration, player, base, item, index;
    static const uint32 offsets[4] = {0u, 4u, 16u, 12u};
    FUNCTION_MARKER(0x80043DF0u, "SHELL.BIN");
    if (screen && (sint32)(r_u32(screen + 40u) - 1u) >= 0)
        for (first = r_u32(screen + 32u); first && r_u32(first + 56u); first = r_u32(first + 180u)) {}
    w_u32(first + 132u, 0x800386D0u);
    head = r_u32(screen + 32u);
    inversion = shell_find_named_item(head, 0x8003A1FCu);
    config = shell_find_named_item(head, 0x8003A210u);
    player = r_u32(r_u32(first + 116u) + 216u) != 0u;
    base = player ? 0x800386BCu : 0x800386A8u;
    item = inversion;
    for (index = 0u; index < 4u; ++index)
    {
        w_u32(item + 132u, base + offsets[index]);
        if (index < 3u) item = r_u32(item + 180u);
    }
    vibration = shell_find_named_item(head, 0x8003A228u);
    w_u32(vibration + 132u, player ? 0x800386C4u : 0x800386B0u);
    w_u32(config + 132u, player ? 0x800386D8u : 0x800386D4u);
    w_u32(0x8007A258u, player);
    shell_sub_8004D838(screen);
}

static uint32 shell_names_equal(uint32 left, uint32 right)
{
    uint32 index = 0u;
    while (r_u8(left + index))
    {
        if ((uint8)moh_draft_missing_call(0x8001E2ACu, (uint32)r_u8(left + index)) !=
            (uint8)moh_draft_missing_call(0x8001E2ACu, (uint32)r_u8(right + index))) return 0u;
        ++index;
    }
    return r_u8(right + index) == 0u;
}

sint32 shell_sub_8003D8EC(uint32 node)
{
    uint32 owner, previous, next;
    FUNCTION_MARKER(0x8003D8ECu, "SHELL.BIN");
    if (!node) return -2;
    owner = r_u32(node + 68u);
    previous = r_u32(node + 60u); next = r_u32(node + 64u);
    if (next) w_u32(next + 60u, previous);
    if (previous) w_u32(previous + 64u, next);
    if (r_u32(owner) == node) w_u32(owner, next);
    if (r_u32(owner + 4u) == node) w_u32(owner + 4u, previous);
    shell_sub_80045328(node);
    w_u32(owner + 8u, r_u32(owner + 8u) - 1u);
    return 0;
}

sint32 shell_sub_8003D890(uint32 list)
{
    uint32 node;
    FUNCTION_MARKER(0x8003D890u, "SHELL.BIN");
    if (!list) return -2;
    while ((node = r_u32(list)) != 0u)
        if (shell_sub_8003D8EC(node) < 0) return -7;
    shell_sub_80045328(list);
    return 0;
}

uint32 shell_sub_8003D03C(void)
{
    uint32 pointer;
    FUNCTION_MARKER(0x8003D03Cu, "SHELL.BIN");
    pointer = r_u32(0x8007A228u);
    if (pointer)
    {
        shell_sub_80045328(pointer);
        w_u32(0x8007A228u, 0u);
    }
    return 0u;
}

void shell_sub_8004A8BC(void)
{
    uint32 screen, group, target;
    FUNCTION_MARKER(0x8004A8BCu, "SHELL.BIN");
    for (screen = r_u32(0x8007BA6Cu); screen; screen = r_u32(screen + 128u))
        for (group = r_u32(screen + 32u); group; group = r_u32(group + 180u))
        {
            if (shell_names_equal(group + 28u, 0x8003A2F4u)) continue;
            for (target = r_u32(0x8007BA6Cu); target; target = r_u32(target + 128u))
                if (shell_names_equal(group + 28u, target)) w_u32(group + 60u, target);
        }
}

static uint32 shell_register_screen_layout(uint32 script_name, uint32 screen_name,
    const uint32 callbacks[6], uint32 data_size, uint32 *data_out, uint32 animation_callbacks,
    uint32 bank_offset, uint32 screen_offset)
{
    uint32 index[2] = {0}, resource = r_u32(0x8007BFB8u), script, screen, item, data;
    script = shell_resource_find(resource, script_name, index);
    shell_sub_80049E90(script, r_u32(r_u32(resource + 68u) + index[0] * 80u + 68u));
    screen = shell_find_screen(screen_name);
    w_u32(screen + 88u, callbacks[0]);
    for (item = 1u; item < 6u; ++item)
        w_u32(screen + 88u + item * 4u + (item >= 4u ? 4u : 0u), callbacks[item]);
    if (animation_callbacks)
        for (item = r_u32(screen + 52u); item; item = r_u32(item + 136u))
            if (r_u32(item + 88u)) w_u32(r_u32(item + 88u) + 92u, 0x8004E290u);
    for (item = 0u; item < 6u; ++item)
        if (r_u32(screen + 92u + item * 4u)) w_u8(screen + 116u + item, 1u);
    data = shell_sub_80045260(data_size);
    if (!data) return (uint32)-1;
    w_u32(data + screen_offset, screen); w_u32(data + bank_offset, r_u32(0x8007BFB4u)); w_u32(screen + 80u, data);
    *data_out = data;
    return 0u;
}

static uint32 shell_register_screen_internal(uint32 script_name, uint32 screen_name,
    const uint32 callbacks[6], uint32 data_size, uint32 *data_out, uint32 animation_callbacks)
{
    return shell_register_screen_layout(script_name, screen_name, callbacks, data_size,
        data_out, animation_callbacks, 0u, 8u);
}

static uint32 shell_register_screen(uint32 script_name, uint32 screen_name,
    const uint32 callbacks[6], uint32 data_size, uint32 *data_out)
{
    return shell_register_screen_internal(script_name, screen_name, callbacks, data_size, data_out, 1u);
}

uint32 shell_sub_80043518(void)
{
    static const uint32 callbacks[6] = {0x80043874u, 0x800437A4u, 0x80043850u,
        0x800448BCu, 0x800448E0u, 0x800438A4u};
    uint32 data, result;
    FUNCTION_MARKER(0x80043518u, "SHELL.BIN");
    result = shell_register_screen(0x8003A1C8u, 0x8003A1E0u, callbacks, 20u, &data);
    if (!result) moh_draft_missing_call(0x80043DF0u, r_u32(data + 8u));
    return result;
}

static uint32 shell_find_indexed_item(uint32 owner, uint32 count_offset,
    uint32 head_offset, uint32 next_offset, uint32 index_offset, uint32 index)
{
    uint32 item;
    if (!owner || (sint32)(r_u32(owner + count_offset) - 1u) < (sint32)index) return 0u;
    for (item = r_u32(owner + head_offset); item; item = r_u32(item + next_offset))
        if (r_u32(item + index_offset) == index) return item;
    return 0u;
}

uint32 shell_sub_8003D9D8(void)
{
    static const uint32 callbacks[6] = {0x8003DF8Cu, 0x8003DF34u, 0x8003DF68u,
        0x8003E994u, 0x8003E9B8u, 0x8003DFBCu};
    static const uint32 settings[4] = {0x80038654u, 0x8003864Cu, 0x80038650u, 0x80038658u};
    uint32 data, screen, group, item, index, result;
    FUNCTION_MARKER(0x8003D9D8u, "SHELL.BIN");
    result = shell_register_screen(0x80039E18u, 0x80039E34u, callbacks, 24u, &data);
    if (result) return result;
    screen = r_u32(data + 8u);
    for (index = 0u; index < 4u; ++index)
    {
        item = shell_find_indexed_item(screen, 40u, 32u, 180u, 56u, index);
        w_u32(item + 132u, settings[index]);
    }
    w_u8(0x8007A244u, 1u);
    item = shell_find_indexed_item(screen, 40u, 32u, 180u, 56u, 3u);
    w_u8(item + 81u, 0u);
    group = r_u32(screen + 64u);
    item = shell_find_indexed_item(group, 128u, 120u, 208u, 216u, 3u);
    w_u8(item + 57u, 1u);
    item = shell_find_indexed_item(group, 128u, 120u, 208u, 216u, 4u);
    w_u8(item + 57u, 1u);
    w_u32(0x8007A248u, 1u);
    w_u32(0x8007A24Cu, 1u);
    return 0u;
}

uint32 shell_sub_8006CDF8(void)
{
    static const uint32 callbacks[6] = {0x8006D394u, 0x8006D050u, 0x8006D370u,
        0x8006D5DCu, 0x8006D600u, 0x8006D3C4u};
    uint32 data, result;
    FUNCTION_MARKER(0x8006CDF8u, "SHELL.BIN");
    result = shell_register_screen_internal(0x8003B678u, 0x8003B694u, callbacks, 28u, &data, 0u);
    if (!result)
    {
        w_u32(0x8007E870u, 1u);
        w_u32(0x8007E874u, 0u);
    }
    return result;
}

uint32 shell_sub_8006C600(void)
{
    static const uint32 callbacks[6] = {0x8006CA80u, 0x8006C858u, 0x8006CA5Cu,
        0x8006CC78u, 0x8006CC9Cu, 0x8006CAB0u};
    uint32 data, result;
    FUNCTION_MARKER(0x8006C600u, "SHELL.BIN");
    result = shell_register_screen_internal(0x8003B5F0u, 0x8003B60Cu, callbacks, 28u, &data, 0u);
    if (!result)
    {
        w_u32(0x8007E868u, 1u);
        w_u32(0x8007E86Cu, 1u);
    }
    return result;
}

uint32 shell_sub_8006A888(void)
{
    static const uint32 callbacks[6] = {0x8006ABC8u, 0x8006AB0Cu, 0x8006ABA4u,
        0x8006C018u, 0x8006C03Cu, 0x8006B664u};
    uint32 data, result;
    FUNCTION_MARKER(0x8006A888u, "SHELL.BIN");
    result = shell_register_screen(0x8003B518u, 0x8003B534u, callbacks, 24u, &data);
    if (!result) w_u8(0x8007E859u, 0u);
    return result;
}

uint32 shell_sub_80072D38(void)
{
    static const uint32 callbacks[6] = {0x80073138u, 0x80072FBCu, 0x800730F4u,
        0x80074A98u, 0x80074BD0u, 0x8007452Cu};
    uint32 data;
    FUNCTION_MARKER(0x80072D38u, "SHELL.BIN");
    return shell_register_screen_layout(0x8003BA00u, 0x8003BA1Cu, callbacks, 28u,
        &data, 1u, 4u, 12u);
}

uint32 shell_sub_8006D760(void)
{
    static const uint32 callbacks[6] = {0x8006DCA4u, 0x8006D9D0u, 0x8006DC58u,
        0x8006F030u, 0x8006F7C8u, 0x8006DCD4u};
    uint32 data;
    FUNCTION_MARKER(0x8006D760u, "SHELL.BIN");
    return shell_register_screen(0x8003B708u, 0x8003B720u, callbacks, 336u, &data);
}

uint32 shell_sub_80072038(void)
{
    static const uint32 callbacks[6] = {0x80072334u, 0x800722C8u, 0x80072310u,
        0x80072AE4u, 0x80072B08u, 0x80072754u};
    uint32 data, screen, result;
    FUNCTION_MARKER(0x80072038u, "SHELL.BIN");
    result = shell_register_screen(0x8003B990u, 0x8003B9ACu, callbacks, 24u, &data);
    if (result) return result;
    screen = r_u32(data + 8u);
    w_u32(screen + 28u, r_u32(r_u32(screen + 32u) + 180u));
    return 0u;
}

uint32 shell_sub_80070960(void)
{
    static const uint32 callbacks[6] = {0x80070D00u, 0x80070BDCu, 0x80070CBCu,
        0x80071DB0u, 0x80071EE8u, 0x80071B00u};
    uint32 data;
    FUNCTION_MARKER(0x80070960u, "SHELL.BIN");
    return shell_register_screen_layout(0x8003B8F0u, 0x8003B908u, callbacks, 28u,
        &data, 1u, 4u, 12u);
}

uint32 shell_sub_80069C68(void)
{
    static const uint32 callbacks[6] = {0x8006A2FCu, 0x80069EE4u, 0x8006A2D8u,
        0x8006A5DCu, 0x8006A600u, 0x8006A32Cu};
    uint32 data;
    FUNCTION_MARKER(0x80069C68u, "SHELL.BIN");
    return shell_register_screen(0x8003B4B0u, 0x8003B4CCu, callbacks, 24u, &data);
}

uint32 shell_sub_80063E30(void)
{
    static const uint32 callbacks[6] = {0x80064420u, 0x800640A0u, 0x800643D4u,
        0x80065C1Cu, 0x800663B4u, 0x80064450u};
    uint32 data;
    FUNCTION_MARKER(0x80063E30u, "SHELL.BIN");
    return shell_register_screen(0x8003B1B8u, 0x8003B1D0u, callbacks, 348u, &data);
}

uint32 shell_sub_80074D20(void)
{
    static const uint32 callbacks[6] = {0x80075130u, 0x80074F9Cu, 0x80074FF0u,
        0x80076330u, 0x80076354u, 0x80075160u};
    uint32 data;
    FUNCTION_MARKER(0x80074D20u, "SHELL.BIN");
    return shell_register_screen(0x8003BB78u, 0x8003BB94u, callbacks, 20u, &data);
}

uint32 shell_sub_80067820(void)
{
    static const uint32 callbacks[6] = {0x80067B08u, 0x80067A9Cu, 0x80067AE4u,
        0x800684C0u, 0x800684E4u, 0x80067C74u};
    uint32 data;
    FUNCTION_MARKER(0x80067820u, "SHELL.BIN");
    return shell_register_screen(0x8003B3E8u, 0x8003B404u, callbacks, 24u, &data);
}

uint32 shell_sub_800687A0(void)
{
    static const uint32 callbacks[6] = {0x80069254u, 0x80068C9Cu, 0x80069230u,
        0x80069924u, 0x80069948u, 0x80069284u};
    static const uint32 names[3] = {0x8003B458u, 0x8003B46Cu, 0x8003B480u};
    uint32 data, head, item, index, result;
    FUNCTION_MARKER(0x800687A0u, "SHELL.BIN");
    result = shell_register_screen(0x8003B430u, 0x8003B44Cu, callbacks, 24u, &data);
    if (result) return result;
    head = r_u32(r_u32(data + 8u) + 32u);
    for (index = 0u; index < 3u; ++index)
    {
        item = shell_find_named_item(head, names[index]);
        w_u32(item + 132u, 0x80039C6Cu + index * 4u);
    }
    return 0u;
}

uint32 shell_sub_80066830(void)
{
    static const uint32 callbacks[6] = {0x80066EC8u, 0x80066E08u, 0x80066EA4u,
        0x8006743Cu, 0x80067584u, 0x80066EF8u};
    static const uint32 pointers[6] = {0x8007E7CCu, 0x8007E7B8u, 0u,
        0x8007E7D4u, 0x8007E7FCu, 0x8007E7E0u};
    uint32 data, screen, group, item, index, selected, result;
    FUNCTION_MARKER(0x80066830u, "SHELL.BIN");
    result = shell_register_screen(0x8003B370u, 0x8003B38Cu, callbacks, 24u, &data);
    if (result) return result;
    screen = r_u32(data + 8u);
    for (item = r_u32(r_u32(screen + 64u) + 120u); item; item = r_u32(item + 208u))
        if (r_u16(item + 50u) < 4u) w_u8(item + 57u, 1u);
    for (index = 0u; index < 6u; ++index) w_u32(0x8007E81Cu + index * 4u, pointers[index]);
    group = r_u32(screen + 28u);
    index = r_u32(group + 56u);
    selected = index < 4u ? r_u32(0x8007E81Cu + index * 8u + r_u32(r_u32(group + 116u) + 216u) * 4u) : 0u;
    w_u32(0x8007E834u, selected);
    w_u32(0x8007E838u, 0u);
    group = r_u32(screen + 32u);
    item = shell_find_named_item(group, 0x8003B394u);
    w_u32(item + 132u, 0x80038680u);
    item = shell_find_named_item(group, 0x8003B3A4u);
    w_u32(item + 132u, 0x80038684u);
    item = shell_find_named_item(group, 0x8003B3B4u);
    w_u32(item + 132u, 0x8003867Cu);
    return 0u;
}

uint32 shell_sub_8004FB30(void)
{
    static const uint32 callbacks[6] = {
        0x8004FF30u, 0x8004FDACu, 0x8004FF0Cu,
        0x80051A50u, 0x80051A74u, 0x8004FF60u
    };
    uint32 data;
    FUNCTION_MARKER(0x8004FB30u, "SHELL.BIN");
    return shell_register_screen(0x8003A3E8u, 0x8003A404u, callbacks, 20u, &data);
}

uint32 shell_sub_8003FD60(void)
{
    static const uint32 callbacks[6] = {
        0x800400D0u, 0x8003FFDCu, 0x800400ACu,
        0x80040FBCu, 0x800410BCu, 0x80040100u
    };
    uint32 data;
    FUNCTION_MARKER(0x8003FD60u, "SHELL.BIN");
    return shell_register_screen(0x80039F70u, 0x80039F8Cu, callbacks, 28u, &data);
}

uint32 shell_sub_80042C58(void)
{
    static const uint32 callbacks[6] = {0x80042F64u, 0x80042ED4u, 0x80042F40u,
        0x80043138u, 0x8004315Cu, 0x80042F94u};
    uint32 data;
    FUNCTION_MARKER(0x80042C58u, "SHELL.BIN");
    return shell_register_screen(0x8003A188u, 0x8003A1A4u, callbacks, 28u, &data);
}

uint32 shell_sub_8003ECE0(void)
{
    static const uint32 callbacks[6] = {0x8003F038u, 0x8003EF24u, 0x8003F014u,
        0x8003FA7Cu, 0x8003FAA0u, 0x8003F6ECu};
    uint32 data;
    FUNCTION_MARKER(0x8003ECE0u, "SHELL.BIN");
    return shell_register_screen_internal(0x80039E58u, 0x80039E74u, callbacks, 28u, &data, 0u);
}

uint32 shell_sub_80046690(void)
{
    static const uint32 callbacks[6] = {0x80046D2Cu, 0x80046914u, 0x80046BECu,
        0x80048A10u, 0x80048A34u, 0x80046D5Cu};
    uint32 data, result;
    FUNCTION_MARKER(0x80046690u, "SHELL.BIN");
    result = shell_register_screen(0x8003A290u, 0x8003A2ACu, callbacks, 20u, &data);
    if (!result) w_u8(0x8007BA62u, 0u);
    return result;
}

uint32 shell_sub_8005A720(void)
{
    static const uint32 callbacks[6] = {0x8005AD70u, 0x8005A99Cu, 0x8005AD4Cu,
        0x8005BE70u, 0x8005BE94u, 0x8005ADA0u};
    uint32 data;
    FUNCTION_MARKER(0x8005A720u, "SHELL.BIN");
    return shell_register_screen(0x8003A808u, 0x8003A820u, callbacks, 20u, &data);
}

uint32 shell_sub_80056230(void)
{
    static const uint32 callbacks[6] = {0x800565FCu, 0x800564ACu, 0x800565D8u,
        0x80056A04u, 0x80056A28u, 0x8005662Cu};
    uint32 data;
    FUNCTION_MARKER(0x80056230u, "SHELL.BIN");
    return shell_register_screen(0x8003A688u, 0x8003A6A4u, callbacks, 24u, &data);
}

uint32 shell_sub_8005C120(void)
{
    static const uint32 callbacks[6] = {0x8005C424u, 0x8005C39Cu, 0x8005C400u,
        0x8005CDA8u, 0x8005CDCCu, 0x8005C5D0u};
    uint32 data;
    FUNCTION_MARKER(0x8005C120u, "SHELL.BIN");
    return shell_register_screen(0x8003A868u, 0x8003A880u, callbacks, 24u, &data);
}

uint32 shell_sub_80057B50(void)
{
    static const uint32 callbacks[6] = {0x80057F88u, 0x80057DCCu, 0x80057F64u,
        0x800583A8u, 0x80058444u, 0x80057FB8u};
    uint32 data;
    FUNCTION_MARKER(0x80057B50u, "SHELL.BIN");
    return shell_register_screen(0x8003A6F8u, 0x8003A710u, callbacks, 52u, &data);
}

uint32 shell_sub_8006FA48(void)
{
    static const uint32 callbacks[6] = {0x8006FD60u, 0x8006FCC4u, 0x8006FD3Cu,
        0x80070620u, 0x80070644u, 0x8006FD90u};
    uint32 data;
    FUNCTION_MARKER(0x8006FA48u, "SHELL.BIN");
    return shell_register_screen(0x8003B878u, 0x8003B890u, callbacks, 20u, &data);
}

uint32 shell_sub_8004E3E0(void)
{
    static const uint32 callbacks[6] = {0x8004E9A4u, 0x8004E664u, 0x8004E980u,
        0x8004F5A8u, 0x8004F72Cu, 0x8004E9D4u};
    uint32 data, result;
    FUNCTION_MARKER(0x8004E3E0u, "SHELL.BIN");
    result = shell_register_screen(0x8003A348u, 0x8003A360u, callbacks, 28u, &data);
    if (!result) w_u32(data + 20u, (uint32)-1);
    return result;
}

void shell_sub_80063510(uint32 bank, sint32 sample, uint32 voice, uint32 left,
    uint32 right, uint32 rate, uint32 variation)
{
    SpuVoiceAttr attr = {0};
    uint32 offset = 0u, index, product;
    sint32 spread;
    uint8 setting = r_u8(0x8007C369u);
    FUNCTION_MARKER(0x80063510u, "SHELL.BIN");
    attr.voice = 1u << (voice & 31u); attr.mask = 0xFF93u;
    attr.volume.left = (sint16)(left << 7u); attr.volume.right = (sint16)(right << 7u);
    if (setting == 0u) attr.volume.left = attr.volume.right = 0;
    else if (setting == 1u)
    {
        attr.volume.left = (sint16)((sint32)(left << 23u) >> 17);
        attr.volume.right = (sint16)((sint32)(right << 23u) >> 17);
    }
    else if (setting == 3u)
    {
        attr.volume.left = (sint16)(left << 8u); attr.volume.right = (sint16)(right << 8u);
    }
    if (attr.volume.left > 0x3F7F) attr.volume.left = 0x3F7F;
    if (attr.volume.right > 0x3F7F) attr.volume.right = 0x3F7F;
    product = (uint32)(sint32)r_s16(0x8007C390u + bank * 2u + (uint32)sample * 32u) * rate;
    attr.pitch = (uint16)((sint32)product >> 16);
    product = (uint32)attr.pitch * variation; spread = (sint32)product >> 15;
    if (spread)
    {
        sint32 random_value = (sint32)moh_draft_missing_call(0x8001552Cu);
        // TODO: Verify the SDK random adapter when pitch variation is used
        sint32 remainder = spread == -1 && random_value == (sint32)0x80000000u ? 0 : random_value % spread;
        attr.pitch = (uint16)(attr.pitch + ((sint32)product >> 16) - remainder);
    }
    if (sample >= 0)
        for (index = 0u; index <= (uint32)sample; ++index)
            offset += (uint32)r_u16(r_u32(0x8007C284u + bank * 4u) + index * 2u) * 8u;
    attr.addr = r_u32(0x8007C244u + bank * 4u) + offset;
    attr.a_mode = 5; attr.s_mode = 1; attr.r_mode = 3;
    attr.ar = 0u; attr.dr = 15u; attr.sr = 127u; attr.rr = 0u; attr.sl = 15u;
    SpuSetVoiceAttr(&attr); SpuSetKey(1, attr.voice); SpuGetVoiceAttr(&attr);
    w_u8(0x8007C22Cu + voice, 1u);
}

uint32 shell_sub_8006349C(void)
{
    uint32 voice;
    FUNCTION_MARKER(0x8006349Cu, "SHELL.BIN");
    for (voice = 0u; voice < 24u; ++voice)
        w_u8(0x8007C22Cu + voice, (uint8)SpuGetKeyStatus(1u << voice));
    for (voice = 12u; voice < 24u; ++voice)
        if (r_u8(0x8007C22Cu + voice) != 1u) { SpuSetKey(0, 1u << voice); return voice; }
    return (uint32)-1;
}

uint32 shell_sub_80058928(void)
{
    uint32 index[2] = {0}, resource = r_u32(0x8007BFB8u), screen, script, item, data, bank, saved;
    uint32 sequence = r_u32(0x80039C68u) + 1u, voice;
    FUNCTION_MARKER(0x80058928u, "SHELL.BIN");
    w_u32(0x80039C68u, sequence); w_u32(0x8007BBA8u, sequence & 1u ? 1u : 2u);
    script = shell_resource_find(resource, sequence & 1u ? 0x8003A730u : 0x8003A74Cu, index);
    shell_sub_80049E90(script, r_u32(r_u32(resource + 68u) + index[0] * 80u + 68u));
    screen = shell_find_screen(0x8003A768u);
    w_u32(screen + 92u, 0x80058CC8u); w_u32(screen + 96u, 0x80058F8Cu);
    w_u32(screen + 100u, 0x8005A1ECu); w_u32(screen + 112u, 0x80059030u);
    w_u32(screen + 108u, 0x8005A430u); w_u32(screen + 88u, 0x80058FB0u);
    for (item = r_u32(screen + 52u); item; item = r_u32(item + 136u))
        if (r_u32(item + 88u)) w_u32(r_u32(item + 88u) + 92u, 0x8004E290u);
    for (item = 0u; item < 6u; ++item)
        if (r_u32(screen + 92u + item * 4u)) w_u8(screen + 116u + item, 1u);
    data = shell_sub_80045260(28u);
    if (!data) return (uint32)-1;
    bank = r_u32(0x8007BFB4u);
    w_u32(data + 8u, screen); w_u32(data, bank); w_u32(screen + 80u, data);
    w_u32(data + 20u, r_u32(screen + 32u)); saved = r_u32(bank + 4u); w_u32(bank + 4u, 0x8000u);
    if (!bank) w_u32(0x8007C218u, (uint32)-2);
    else
    {
        voice = shell_sub_8006349C(); w_u32(0x8007C374u, voice);
        w_u32(r_u32(bank + 12u) + 16u, voice);
        moh_draft_missing_call(0x80063510u, (uint32)(sint32)r_s16(bank), 4u, voice,
            (uint32)r_u8(bank + 16u), (uint32)r_u8(bank + 17u), r_u32(bank + 4u), r_u32(bank + 8u));
    }
    voice = r_u32(0x8007C374u); w_u32(bank + 4u, saved); w_u32(0x8007BBC0u, voice);
    w_u8(0x8007C378u + voice, 1u); moh_draft_missing_call(0x8001E42Cu, (uint32)(sint32)(sint16)voice, 0u, 0u);
    return 0u;
}

void shell_sub_8005FE04(void)
{
    uint32 rectangle[2], media_config[4], sound_info[2];
    char screen_name[64];
    uint32 index, sound, name, level_mode, flow, selected;
    FUNCTION_MARKER(0x8005FE04u, "SHELL.BIN");
    w_u8(0x8007BFD3u, r_u8(0x80039D24u));
    w_u8(0x8007BFD4u, r_u8(0x80039D25u));
    w_u8(0x8007BFD5u, r_u8(0x80039D26u));
    if (r_u32(0x80039C5Cu))
    {
        rectangle[0] = 0u; rectangle[1] = 0x02000400u;
        ClearImage((PSX_RECT *)rectangle, 0u, 0u, 0u);
        DrawSync(0);
    }
    /* TODO Translate resource allocation and shell subsystems */
    moh_draft_missing_call(0x800451F0u, 0x57800u);
    moh_draft_missing_call(0x8003CFD8u, 0x96000u);
    moh_draft_missing_call(0x8005F7A0u, 0u, 0u);
    moh_draft_missing_call(0x8005F89Cu, 2u, 1u, 0u, 0u, 0u);
    if (r_u32(0x80039C5Cu))
    {
        rectangle[0] = 0u; rectangle[1] = 0x02000400u;
        ClearImage((PSX_RECT *)rectangle, 0u, 0u, 0u);
        DrawSync(0);
        moh_draft_missing_call(0x80045390u);
        moh_draft_missing_call(0x8001B228u, 0x8007BDF8u, 0x8007BE20u);
        moh_draft_missing_call(0x8001B4A8u);
        memset(moh_draft_guest_pointer(0x8007BCDCu, 0x88u), 0, 0x88u);
        memset(moh_draft_guest_pointer(0x8007BD64u, 0x88u), 0, 0x88u);
        moh_draft_missing_call(0x80019E20u);
        moh_draft_missing_call(0x8006118Cu, 0u);
    }
    else
    {
        moh_draft_missing_call(0x80045390u);
        moh_draft_missing_call(0x8001B228u, 0x8007BDF8u, 0x8007BE20u);
        moh_draft_missing_call(0x8001B4A8u);
        memset(moh_draft_guest_pointer(0x8007BCDCu, 0x88u), 0, 0x88u);
        memset(moh_draft_guest_pointer(0x8007BD64u, 0x88u), 0, 0x88u);
        moh_draft_missing_call(0x80019E20u);
        if (!r_u32(0x80039C5Cu))
        {
            rectangle[0] = r_u32(0x8003AA18u); rectangle[1] = r_u32(0x8003AA1Cu);
            sub_800170AC(moh_draft_local_address(rectangle, sizeof(rectangle)), 0x200u, 0u);
            sub_800170AC(moh_draft_local_address(rectangle, sizeof(rectangle)), 0u, 0xF0u);
            DrawSync(0);
        }
    }
    w_u32(0x8007BFC0u, 0u);
    moh_draft_missing_call(0x80056C58u);
    w_u8(0x8007C368u, 110u); w_u8(0x8007C369u, 110u);
    w_u8(0x8007C36Au, 1u);
    w_u16(0x8007C36Cu, 32u); w_u16(0x8007C36Eu, 32u);
    w_u8(0x8007C370u, 1u); w_u8(0x8007C371u, 0u); w_u8(0x8007C372u, 110u);
    for (index = 0u; index < 16u; ++index) w_u8(0x8007C21Cu + index, 1u);
    for (index = 0u; index < 24u; ++index) w_u8(0x8007C378u + index, 0u);
    moh_draft_missing_call(0x800631D0u);
    moh_draft_missing_call(0x8001555Cu, r_u32(0x80039C68u));
    memset(media_config, 0, sizeof(media_config)); memset(rectangle, 0, sizeof(rectangle));
    shell_sub_80063940(media_config, rectangle);
    w_u32(0x8007BFB8u, moh_draft_missing_call(0x8005F580u, 0x8003AA20u, 0u, 1u));
    if (!r_u32(0x80039C5Cu)) moh_draft_missing_call(0x8006118Cu, 4u);
    w_u32(0x8007BFBCu, moh_draft_missing_call(0x8005F580u, 0x8003AA30u, 0u, 0u));
    if (!r_u32(0x80039C5Cu)) moh_draft_missing_call(0x8006118Cu, 4u);
    if (!r_u32(0x80039C5Cu))
    {
        w_u8(0x8007BFD1u, 0u); w_u8(0x8007BFD0u, 0u); w_u8(0x8007BFD2u, 0u);
        w_u8(0x8007C006u, 0u); w_u8(0x8007C007u, 0u);
    }
    else shell_sub_80060B0C();
    sound = shell_resource_find(r_u32(0x8007BFB8u), 0x8003AA44u, sound_info);
    if (!r_u32(0x80039C5Cu)) moh_draft_missing_call(0x8006118Cu, 4u);
    w_u32(0x8007BFB4u, moh_draft_missing_call(0x80062F50u, sound));
    moh_draft_missing_call(0x8004DC40u);
    if (!r_u32(0x80039C5Cu)) moh_draft_missing_call(0x8006118Cu, 4u);
    moh_draft_missing_call(0x8004DC40u);
    w_u8(0x8007BFCFu, 1u);
    moh_draft_missing_call(0x80041790u); moh_draft_missing_call(0x80058928u);
    moh_draft_missing_call(0x8006FA48u); moh_draft_missing_call(0x8004E3E0u);
    moh_draft_missing_call(0x8005C120u); moh_draft_missing_call(0x80057B50u);
    moh_draft_missing_call(0x8005A720u); moh_draft_missing_call(0x80056230u);
    moh_draft_missing_call(0x8003ECE0u); moh_draft_missing_call(0x80046690u);
    moh_draft_missing_call(0x80043518u); moh_draft_missing_call(0x80042C58u);
    moh_draft_missing_call(0x8004FB30u); moh_draft_missing_call(0x8003FD60u);
    moh_draft_missing_call(0x8003D9D8u);
    if (!r_u32(0x80039C5Cu)) moh_draft_missing_call(0x8006118Cu, 4u);
    moh_draft_missing_call(0x80066830u); moh_draft_missing_call(0x80067820u);
    moh_draft_missing_call(0x800687A0u); moh_draft_missing_call(0x80074D20u);
    moh_draft_missing_call(0x80063E30u); moh_draft_missing_call(0x80069C68u);
    moh_draft_missing_call(0x80070960u); moh_draft_missing_call(0x80072038u);
    moh_draft_missing_call(0x8006D760u); moh_draft_missing_call(0x80072D38u);
    moh_draft_missing_call(0x8006A888u); moh_draft_missing_call(0x8006C600u);
    moh_draft_missing_call(0x8006CDF8u);
    w_u8(0x8007BFCFu, 0u);
    if (!r_u32(0x80039C5Cu)) moh_draft_missing_call(0x8006118Cu, 4u);
    moh_draft_missing_call(0x8004A8BCu);
    if (!r_u32(0x80039C5Cu)) moh_draft_missing_call(0x8006118Cu, 4u);
    if (r_u32(0x80039C5Cu)) name = 0x8003AA5Cu;
    else
    {
        level_mode = r_u32(0x80039D1Cu); flow = r_u32(0x80039C58u);
        if (level_mode == 1u && (flow == 0u || flow == 3u))
        {
            if (!r_u32(0x80038D58u)) name = 0x8003AA80u;
            else
            {
                uint32 group = r_u32(0x80039C88u);
                uint32 stage = r_u32(0x80039C84u);
                if ((sint32)(stage + 1u) < r_s32(0x80038D98u + group * 12u))
                    name = stage == 1u && group == 7u ? 0x8003AA74u : 0x8003AA80u;
                else
                {
                    memset(screen_name, 0, sizeof(screen_name));
                    name = moh_draft_local_address(screen_name, sizeof(screen_name));
                    moh_draft_missing_call(0x8001BE18u, name, 0x8003AA68u, group + 1u);
                }
                w_u8(0x8007BFD1u, 1u);
            }
        }
        else if (level_mode == 3u && (flow == 0u || flow == 3u)) name = 0x8003AA80u;
        else if (level_mode == 2u)
        {
            w_u32(0x80039C58u, r_u32(0x80039D20u)); name = 0x8003AA8Cu;
        }
        else name = 0x8003AA94u;
    }
    selected = shell_find_screen(name);
    w_u32(0x8007BA68u, selected);
    w_u32(0x8007BFB8u, 0u);
    moh_draft_missing_call(0x8003D03Cu);
    moh_draft_missing_call(0x8003CFD8u, 0x83400u);
    if (r_u32(0x80039C5Cu)) moh_draft_missing_call(0x80061F00u);
    if (!r_u32(0x80039C5Cu)) moh_draft_missing_call(0x8006118Cu, 4u);
    if (r_u32(0x80039C5Cu))
    {
        w_u32(0x80039C5Cu, 0u);
        moh_draft_missing_call(0x8003D890u, r_u32(0x8007C018u));
        moh_draft_missing_call(0x8003D890u, r_u32(0x8007C01Cu));
        w_u32(0x8007C01Cu, 0u); w_u32(0x8007C018u, 0u);
    }
    else
    {
        if (r_u32(0x8007BFB0u))
        {
            moh_draft_missing_call(0x8003D890u, r_u32(0x8007C018u));
            moh_draft_missing_call(0x8003D890u, r_u32(0x8007C01Cu));
            w_u32(0x8007C01Cu, 0u); w_u32(0x8007C018u, 0u);
        }
        if (!r_u32(0x80039C5Cu)) moh_draft_missing_call(0x8006118Cu, 4u);
    }
    w_u8(0x8007C010u, 0u);
    moh_draft_missing_call(0x8006118Cu, 1u);
}

uint32 shell_sub_8005FC28(void)
{
    uint32 frame, ordering_table, index, packet;
    FUNCTION_MARKER(0x8005FC28u, "SHELL.BIN");
    if (r_u32(0x80039C5Cu)) w_u32(0x8007BFB0u, 0u);
    shell_sub_8005FE04();
    w_u8(0x8007BFFAu, 0u);
    w_u8(0x8007BFFBu, 0u);
    w_u8(0x8007BFFCu, 0u);
    w_u8(0x8007BFFDu, 0u);
    w_u8(0x8007C00Cu, 1u);
    w_u32(0x8007C008u, 0u);
    for (;;)
    {
        uint32 alternate = r_u8(0x8007BF90u) == 0u;
        w_u8(0x8007BF90u, alternate);
        w_u32(0x8007BF4Cu, alternate ? 0x8007BED4u : 0x8007BE5Cu);
        frame = r_u32(0x8007BF4Cu);
        ordering_table = r_u32(frame + 0x74u);
        moh_draft_missing_call(0x8001722Cu, ordering_table, 0x600u);
        if (r_u8(0x8007BF91u))
        {
            ordering_table = r_u32(r_u32(0x8007BF4Cu) + 0x70u);
            if (ordering_table) moh_draft_missing_call(0x8001722Cu, ordering_table, 0x600u);
        }
        /* TODO Translate shell input and menu state updates */
        moh_draft_missing_call(0x8005D590u);
        w_u32(0x8007BB38u, 0u);
        w_u32(0x8007BB3Cu, 0u);
        w_u32(0x8007BFDCu, r_u32(0x8007BFDCu) + 1u);
        if (r_u8(0x8007BFFDu) || moh_draft_missing_call(0x8004D9D8u) == 1u) break;
        frame = r_u32(0x8007BF4Cu);
        w_u32(0x8007BF88u, moh_native_draw_sync(0u));
        if (r_u8(0x8007BF98u))
        {
            packet = r_u32(0x8007BF9Cu);
            for (index = 0u; (sint32)index < r_s32(0x8007BFA4u); ++index, packet += 12u)
                sub_8001704C(packet, r_u32(packet + 8u));
            w_u32(0x8007BFA4u, 0u);
        }
        w_u32(0x8007BF8Cu, moh_draft_missing_call(0x80015DB8u, 2u));
        moh_native_put_disp_env(frame + 0x5Cu);
        moh_native_put_draw_env(frame);
        moh_draft_missing_call(0x800172D8u, r_u32(frame + 0x74u) + 0x17FCu);
    }
    /* TODO Translate shell shutdown */
    moh_draft_missing_call(0x80060D1Cu);
    return 0u;
}

void shell_sub_80058FE0(uint32 screen)
{
    uint32 item = r_u32(screen + 32u);
    uint32 table = r_u32(r_u32(screen + 72u) + 4u) + r_u32(0x8007BBACu) * 28u;
    FUNCTION_MARKER(0x80058FE0u, "SHELL.BIN");
    for (; item; item = r_u32(item + 180u), table += 4u)
        w_u8(item + 81u, r_u32(table) == 0u);
}

void shell_sub_8004E0BC(uint32 data, uint32 name)
{
    uint32 screen = r_u32(data + 8u), resource;
    FUNCTION_MARKER(0x8004E0BCu, "SHELL.BIN");
    if (name) w_u32(data + 4u, shell_sub_8005F580(name, 0u, 1u));
    resource = r_u32(data + 4u);
    moh_draft_missing_call(0x8004B974u, screen, resource, r_u32(0x8007BFBCu));
    moh_draft_missing_call(0x8004C21Cu, screen, resource, r_u32(0x8007BFBCu));
    moh_draft_missing_call(0x8004B2DCu, screen, resource, r_u32(0x8007BFBCu));
    w_u8(screen + 116u, 0u); w_u8(screen + 117u, 0u);
    w_u8(screen + 118u, 1u); w_u8(screen + 121u, 1u); w_u8(screen + 120u, 1u);
    shell_sub_8004D838(screen);
    w_u32(0x8007BA8Cu, 6u);
    moh_draft_missing_call(0x8004DD9Cu);
}

void shell_sub_80058CC8(uint32 screen)
{
    uint32 data = r_u32(screen + 80u), selection, item, voice, offset;
    uint8 media[16], rectangle[8] = {0};
    sint32 volume, random;
    FUNCTION_MARKER(0x80058CC8u, "SHELL.BIN");
    w_u8(0x8007E859u, 0u); w_u32(0x8007BBD0u, 0x3C0u); w_u8(0x8007BBB1u, 0u);
    selection = r_u32(0x80039C58u) == 4u;
    w_u32(0x8007BBACu, selection);
    item = shell_find_indexed_item(screen, 40u, 32u, 180u, 56u, selection);
    w_u32(screen + 28u, item);
    shell_sub_80058FE0(screen);
    selection = r_u32(0x8007BBA8u);
    if (selection != 1u && selection != 2u) abort();
    shell_sub_8004E0BC(data, selection == 1u ? 0x8003A770u : 0x8003A788u);
    w_u8(0x8007BBB8u, 0u);
    for (offset = 0u; offset < sizeof(media); ++offset) media[offset] = r_u8(0x8003A7A0u + offset);
    shell_sub_80063940(media, rectangle);
    voice = r_u32(0x8007BBC0u); w_u8(0x8007C378u + voice, 0u);
    volume = (sint16)(r_u32(0x80038654u) * 21u);
    moh_draft_missing_call(0x8001E42Cu, (uint32)(sint32)(sint16)voice, (uint32)volume, (uint32)volume);
    random = (sint32)moh_draft_missing_call(0x8001552Cu); w_u16(0x8007BBBAu, (uint16)(random % 300 + 150));
    random = (sint32)moh_draft_missing_call(0x8001552Cu); w_u16(0x8007BBBEu, (uint16)(random % 300 + 150));
    random = (sint32)moh_draft_missing_call(0x8001552Cu); w_u16(0x8007BBBCu, (uint16)(random % 300 + 150));
    w_u8(0x8007BFD2u, 0u); w_u8(0x8007BBC4u, 0u);
}

uint32 shell_sub_8004B10C(uint32 image, uint32 primary, uint32 secondary)
{
    uint32 index[2] = {0}, data, flags, texture;
    FUNCTION_MARKER(0x8004B10Cu, "SHELL.BIN");
    if (image && shell_names_equal(image, 0x8003A300u)) { w_u32(image + 28u, 0u); return 0u; }
    if (r_u8(image + 56u)) primary = secondary;
    data = shell_resource_find(primary, image, index);
    if (!data) return (uint32)-7;
    flags = r_u8(image + 140u) ? (uint32)(sint32)(sint16)(r_u16(image + 142u) | 4u) : 8u;
    texture = shell_sub_8003D3A0(data, r_u32(r_u32(primary + 68u) + index[0] * 80u + 68u), flags);
    w_u32(image + 28u, texture);
    return texture ? 0u : (uint32)-7;
}

static uint32 shell_load_group_assets(uint32 group, uint32 primary, uint32 secondary)
{
    uint32 item, result;
    for (item = r_u32(group + 120u); item; item = r_u32(item + 208u))
    {
        result = shell_sub_8004B10C(item, primary, secondary);
        if ((sint32)result < 0) return result;
    }
    for (item = r_u32(group + 144u); item; item = r_u32(item + 312u))
    {
        result = moh_draft_missing_call(0x8004AC98u, item, primary, secondary);
        if ((sint32)result < 0) return result;
    }
    return 0u;
}

uint32 shell_sub_8004B974(uint32 screen, uint32 primary, uint32 secondary)
{
    uint32 group, item, offset, result;
    FUNCTION_MARKER(0x8004B974u, "SHELL.BIN");
    if (!screen || (!primary && !secondary)) return (uint32)-2;
    if (!primary) primary = secondary;
    if (!secondary) secondary = primary;
    /* Preserve image lists before widget lists across both fixed groups */
    for (offset = 64u; offset <= 68u; offset += 4u)
    {
        group = r_u32(screen + offset);
        if (!group) continue;
        for (item = r_u32(group + 120u); item; item = r_u32(item + 208u))
        {
            result = shell_sub_8004B10C(item, primary, secondary);
            if ((sint32)result < 0) return result;
        }
    }
    for (offset = 64u; offset <= 68u; offset += 4u)
    {
        group = r_u32(screen + offset);
        if (!group) continue;
        for (item = r_u32(group + 144u); item; item = r_u32(item + 312u))
        {
            result = moh_draft_missing_call(0x8004AC98u, item, primary, secondary);
            if ((sint32)result < 0) return result;
        }
    }
    return 0u;
}

uint32 shell_sub_8004C21C(uint32 screen, uint32 primary, uint32 secondary)
{
    uint32 group, item, result;
    FUNCTION_MARKER(0x8004C21Cu, "SHELL.BIN");
    if (!screen) return (uint32)-2;
    for (group = r_u32(screen + 32u); group; group = r_u32(group + 180u))
    {
        item = r_u32(group + 136u);
        if (item) {
            result = moh_draft_missing_call(0x8004AC98u, item, primary, secondary);
            if ((sint32)result < 0) return result;
        }
        item = r_u32(group + 112u);
        if (item) {
            result = shell_sub_8004B10C(item, primary, secondary);
            if ((sint32)result < 0) return result;
        }
        result = shell_load_group_assets(group, primary, secondary);
        if ((sint32)result < 0) return result;
    }
    return 0u;
}

void shell_sub_8004AAA4(uint32 widget, uint32 parameters, uint32 color)
{
    uint32 index, row, column;
    FUNCTION_MARKER(0x8004AAA4u, "SHELL.BIN");
    memset(moh_draft_guest_pointer(parameters, 160u), 0, 160u);
    for (index = 0u; index < 3u; ++index) {
        w_u32(parameters + 48u + index * 4u, r_u32(widget + 88u + index * 4u));
        w_u16(parameters + 64u + index * 2u, r_u16(widget + 100u + index * 4u));
    }
    widget += r_u32(widget + 200u) * 44u;
    for (index = 0u; index < 3u; ++index) w_u8(color + index, r_u8(widget + 112u + index));
    for (index = 0u; index < 9u; ++index)
        w_u16(parameters + 92u + index * 2u, (uint16)(0u - r_u16(widget + 116u + index * 2u)));
    for (row = 0u; row < 3u; ++row)
        for (column = 0u; column < 3u; ++column)
            w_u16(parameters + 124u + column * 6u + row * 2u, r_u16(widget + 134u + row * 6u + column * 2u));
    for (index = 0u; index < 3u; ++index) w_u8(parameters + 156u + index, r_u8(widget + 152u + index));
}

uint32 shell_sub_8004AC98(uint32 widget, uint32 primary, uint32 secondary)
{
    uint32 resource, index[2] = {0}, data, texture, model, temporary, flags;
    FUNCTION_MARKER(0x8004AC98u, "SHELL.BIN");
    resource = r_u8(widget + 74u) ? secondary : primary;
    if (!shell_names_equal(widget + 28u, 0x8003A300u)) {
        data = shell_resource_find(resource, widget + 28u, index);
        if (!data) return (uint32)-7;
        texture = shell_sub_8003D3A0(data, r_u32(r_u32(resource + 68u) + index[0] * 80u + 68u), r_u8(widget + 208u) ? 4u : 8u);
        w_u32(widget + 56u, texture);
        if (!texture) return (uint32)-7;
    }
    if (!shell_names_equal(widget + 272u, 0x8003A300u)) {
        data = shell_resource_find(resource, widget + 272u, index);
        if (!data) return (uint32)-7;
        texture = shell_sub_8003D3A0(data, r_u32(r_u32(resource + 68u) + index[0] * 80u + 68u), 4u);
        w_u32(widget + 60u, texture);
        if (!texture) return (uint32)-7;
    }
    if (shell_names_equal(widget, 0x8003A300u)) return 0u;
    resource = r_u8(widget + 73u) ? secondary : primary;
    data = shell_resource_find(resource, widget, index);
    if (!data) return (uint32)-7;
    /* Temporary parameter storage replaces the original local buffers */
    temporary = shell_sub_80045260(168u);
    if (!temporary) return (uint32)-7;
    shell_sub_8004AAA4(widget, temporary, temporary + 160u);
    if (r_u8(widget + 208u)) w_u32(widget + 76u, r_u32(widget + 76u) | 64u);
    flags = (uint32)(sint32)(sint16)r_u16(widget + 76u);
    model = moh_draft_missing_call(0x80051EC0u, data, temporary, flags, temporary + 160u, r_u32(widget + 212u));
    shell_sub_80045328(temporary);
    w_u32(widget + 64u, model);
    if (!model) return (uint32)-7;
    flags = r_u32(widget + 76u) & 63u;
    if (flags == 5u) w_u32(model + 128u, r_u32(widget + 56u));
    if (flags == 6u) {
        w_u32(model + 128u, r_u32(widget + 60u));
        moh_draft_missing_call(0x80055BFCu, model);
    }
    return 0u;
}

uint32 shell_sub_8004B2DC(uint32 screen, uint32 primary, uint32 secondary)
{
    uint32 item, resource, data, index[2] = {0}, texture, flags, model, copy, offset;
    FUNCTION_MARKER(0x8004B2DCu, "SHELL.BIN");
    if (!screen || (!primary && !secondary)) return (uint32)-2;
    item = r_u32(screen + 52u);
    if (!item) return 2u;
    if (!primary) primary = secondary;
    if (!secondary) secondary = primary;
    for (; item; item = r_u32(item + 136u)) {
        resource = r_u8(item + 109u) ? secondary : primary;
        if (!shell_names_equal(item + 28u, 0x8003A300u)) {
            data = shell_resource_find(resource, item + 28u, index);
            if (!data) return (uint32)-7;
            flags = (r_u32(item + 84u) & 64u) || r_u8(item + 110u) ? 4u : 8u;
            texture = shell_sub_8003D3A0(data, r_u32(r_u32(resource + 68u) + index[0] * 80u + 68u), flags);
            w_u32(item + 100u, texture);
            if (!texture) return (uint32)-7;
        }
        if (!shell_names_equal(item + 56u, 0x8003A300u)) {
            data = shell_resource_find(resource, item + 56u, index);
            if (!data) return (uint32)-7;
            texture = shell_sub_8003D3A0(data, r_u32(r_u32(resource + 68u) + index[0] * 80u + 68u), 4u);
            w_u32(item + 104u, texture);
            if (!texture) return (uint32)-7;
        }
        if (!shell_names_equal(item, 0x8003A300u)) {
            data = shell_resource_find(resource, item, index);
            if (!data) return (uint32)-7;
            flags = (uint32)(sint32)(sint16)r_u16(item + 84u);
            model = moh_draft_missing_call(0x80051EC0u, data, 0u, flags, 0u, 0u);
            w_u32(item + 92u, model);
            if (!model) return (uint32)-7;
            if ((r_u32(item + 84u) & 63u) == 5u) w_u32(model + 128u, r_u32(item + 100u));
            if ((r_u32(item + 84u) & 63u) == 6u) {
                w_u32(model + 128u, r_u32(item + 104u));
                moh_draft_missing_call(0x80055BFCu, model);
            }
            w_u32(model + 1236u, r_u32(item + 128u));
            if (r_u8(item + 110u)) {
                flags = (uint32)(sint32)(sint16)(r_u16(item + 84u) | 64u);
                copy = moh_draft_missing_call(0x80051EC0u, data, 0u, flags, 0u, 0u);
                w_u32(item + 96u, copy);
                if (!copy) return (uint32)-7;
                for (offset = 0u; offset < 16u; offset += 4u) w_u32(copy + 660u + offset, r_u32(item + 112u + offset));
                w_u8(copy + 656u, 1u); w_u32(copy + 1236u, r_u32(item + 128u));
                if ((r_u32(item + 84u) & 63u) == 5u) w_u32(copy + 128u, r_u32(item + 100u));
                if ((r_u32(item + 84u) & 63u) == 6u) {
                    w_u32(copy + 128u, r_u32(item + 104u));
                    moh_draft_missing_call(0x80055BFCu, copy);
                }
            }
        }
        w_u32(r_u32(item + 88u) + 84u, r_u32(item + 92u));
        moh_draft_missing_call(0x8003C110u, r_u32(item + 88u));
    }
    return 0u;
}
