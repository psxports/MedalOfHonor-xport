#include "game_shell_signatures.h"
#include "game_level2p_signatures.h"
#include "xport_trace.h"

void shell_sub_8003D078(uint32 name, uint32 unused, uint32 size_out, uint32 reuse_buffer)
{
    uint32 buffer = 0;
    FUNCTION_MARKER(0x8003D078, "SHELL.BIN");
    if ((uint8)reuse_buffer) buffer = r_u32(0x8007A228);
    shell_sub_8003D0B4(name, size_out, buffer);
}

uint32 shell_sub_8003D0B4(uint32 name, uint32 size_out, uint32 buffer)
{
    uint32 record[2], path_storage[64], status_storage[64], location_storage[2];
    uint32 record_view, path, status_view, location, attempt = 0, length, rounded, sectors;
    uint32 requested, result, read_status = 1;
    FUNCTION_MARKER(0x8003D0B4, "SHELL.BIN");
    /* TODO Local guest views require the existing fail-fast buffer adapter */
    record_view = moh_draft_local_address(record, sizeof(record));
    path = moh_draft_local_address(path_storage, sizeof(path_storage));
    status_view = moh_draft_local_address(status_storage, sizeof(status_storage));
    location = moh_draft_local_address(location_storage, sizeof(location_storage));
    shell_sub_8003D2CC(path, name, 0);
    for (;;)
    {
        result = sub_800374EC(record_view, path);
        ++attempt;
        if (result + 1u >= 2) break;
        moh_draft_missing_call(0x80015B28, 0x80039D98u, path, attempt);
        moh_draft_missing_call(0x80015B28, 0x80039DBCu, result);
        sub_8001AD98();
        sub_80016E38(0);
        moh_draft_missing_call(0x80015DB8, 2u);
        sub_800374EC(record_view, 0x80039DC8);
    }
    length = r_u32(record_view + 4);
    rounded = length + 0x7FFu;
    if ((sint32)rounded < 0) rounded = length + 0xFFEu;
    sectors = (uint32)((sint32)rounded >> 11);
    if (buffer)
    {
        requested = (sectors << 11) + 1u;
        if (r_s32(0x8007A22C) < (sint32)requested || (sint32)requested < 2)
        {
            moh_draft_missing_call(0x80015B28, 0x80039DD8u);
            buffer = 0;
        }
    }
    else buffer = sub_80036BF0((sectors << 11) | 1u);
    if (!buffer) shell_sub_80045328(0);
    else
    {
        sub_8001ADD8(r_u32(record_view), location);
        for (;;)
        {
            result = moh_draft_missing_call(0x80019220, location, sectors, buffer, 0x80u);
            if ((sint32)result <= 0)
            {
                sub_8001AD98();
                sub_80016E38(0);
                moh_draft_missing_call(0x80015DB8, 0u);
                moh_draft_missing_call(0x80015DB8, 0u);
                moh_draft_missing_call(0x80015DB8, 0u);
                moh_draft_missing_call(0x80015B28, 0x80039D98u, path, 0u);
                moh_draft_missing_call(0x80015B28, 0x80039DBCu, read_status);
                continue;
            }
            do
            {
                read_status = moh_draft_missing_call(0x800195D0, status_view);
            } while ((sint32)read_status > 0);
            if (!read_status) break;
        }
    }
    /* TODO Original clears byte zero and reports zero size even after a successful read */
    if (buffer) w_u8(buffer, 0);
    if (size_out) w_u32(size_out, 0);
    return buffer;
}

uint32 shell_sub_8003D2CC(uint32 output, uint32 name, uint32 device_prefix)
{
    uint32 cursor, character;
    FUNCTION_MARKER(0x8003D2CC, "SHELL.BIN");
    moh_draft_missing_call(0x8001AD48, output, 0x80039E00u);
    if (device_prefix) moh_draft_missing_call(0x8001ABC8, output, 0x80039E04u);
    if (r_u8(name) != 0x5C) moh_draft_missing_call(0x8001ABC8, output, 0x80039E0Cu);
    cursor = output + sub_8001AC88(output);
    character = r_u8(name);
    while (character)
    {
        w_u8(cursor, (uint8)(character >= 0x61 ? character - 0x20 : character));
        ++name;
        character = r_u8(name);
        ++cursor;
    }
    w_u8(cursor, 0);
    moh_draft_missing_call(0x8001ABC8, output, 0x80039E10u);
    return output;
}

void shell_sub_8003DF34(uint32 screen)
{
    uint32 data;
    FUNCTION_MARKER(0x8003DF34, "SHELL.BIN");
    data = r_u32(screen + 0x50);
    shell_sub_8004E0BC(data, 0x80039E3C);
    w_u8(data + 0x14, 0);
}

void shell_sub_8003DF68(uint32 screen)
{
    FUNCTION_MARKER(0x8003DF68, "SHELL.BIN");
    shell_sub_8004E16C(r_u32(screen + 0x50));
}

void shell_sub_8003DF8C(uint32 screen)
{
    FUNCTION_MARKER(0x8003DF8C, "SHELL.BIN");
    shell_sub_8003DF68(screen);
    shell_sub_80045328(r_u32(screen + 0x50));
}

void shell_sub_8003E994(uint32 screen)
{
    FUNCTION_MARKER(0x8003E994, "SHELL.BIN");
    shell_sub_8004E1B8(r_u32(screen + 0x50), 0);
}

void shell_sub_800400AC(uint32 screen)
{
    FUNCTION_MARKER(0x800400AC, "SHELL.BIN");
    shell_sub_8004E16C(r_u32(screen + 0x50));
}

void shell_sub_800400D0(uint32 screen)
{
    FUNCTION_MARKER(0x800400D0, "SHELL.BIN");
    shell_sub_800400AC(screen);
    shell_sub_80045328(r_u32(screen + 0x50));
}

void shell_sub_80040E64(uint32 screen, uint32 enabled)
{
    uint32 element, data;
    FUNCTION_MARKER(0x80040E64, "SHELL.BIN");
    element = r_u32(screen + 0x20);
    while (element)
    {
        data = r_u32(element + 0x70);
        if (r_s16(data + 0x32) == 0x309)
            w_u8(data + 0x39, (uint8)enabled == 0);
        element = r_u32(element + 0xB4);
    }
}

void shell_sub_80041DF0(uint32 screen)
{
    FUNCTION_MARKER(0x80041DF0, "SHELL.BIN");
    shell_sub_8005F89C(2, 1, 0, 0, 0);
    w_u8(screen + 0x74, 1);
    w_u8(screen + 0x75, 0);
    shell_sub_80045328(r_u32(0x8007BFC0));
}

void shell_sub_80041E44(uint32 screen)
{
    FUNCTION_MARKER(0x80041E44, "SHELL.BIN");
    shell_sub_80045328(r_u32(screen + 0x50));
}

void shell_sub_80042ED4(uint32 screen)
{
    uint32 data, selection;
    FUNCTION_MARKER(0x80042ED4, "SHELL.BIN");
    selection = r_u32(0x800386D0);
    data = r_u32(screen + 0x50);
    selection = selection ? 0x800386D8 : 0x800386D4;
    w_u32(screen + 0x2C, selection);
    w_u32(data + 0x18, selection);
    w_u32(data + 0x14, r_u32(selection));
    shell_sub_8004E0BC(data, 0x8003A1B0);
}

void shell_sub_80042F40(uint32 screen)
{
    FUNCTION_MARKER(0x80042F40, "SHELL.BIN");
    shell_sub_8004E16C(r_u32(screen + 0x50));
}

void shell_sub_80042F64(uint32 screen)
{
    FUNCTION_MARKER(0x80042F64, "SHELL.BIN");
    shell_sub_80042F40(screen);
    shell_sub_80045328(r_u32(screen + 0x50));
}

void shell_sub_80043138(uint32 screen)
{
    FUNCTION_MARKER(0x80043138, "SHELL.BIN");
    shell_sub_8004E1B8(r_u32(screen + 0x50), 0);
}

void shell_sub_80043850(uint32 screen)
{
    FUNCTION_MARKER(0x80043850, "SHELL.BIN");
    shell_sub_8004E16C(r_u32(screen + 0x50));
}

void shell_sub_80043874(uint32 screen)
{
    FUNCTION_MARKER(0x80043874, "SHELL.BIN");
    shell_sub_80043850(screen);
    shell_sub_80045328(r_u32(screen + 0x50));
}

void shell_sub_800448BC(uint32 screen)
{
    FUNCTION_MARKER(0x800448BC, "SHELL.BIN");
    shell_sub_8004E1B8(r_u32(screen + 0x50), 0);
}

uint32 shell_sub_8004558C(void)
{
    uint32 command, result, status;
    FUNCTION_MARKER(0x8004558C, "SHELL.BIN");
    /* TODO The existing local buffer adapter must expose writable output words */
    status = shell_sub_80077C30(1, moh_draft_local_address(&command, sizeof(command)),
        moh_draft_local_address(&result, sizeof(result)));
    if (status) w_u8(0x8007B658, 1);
    return status;
}

uint32 shell_sub_80077C30(uint32 poll, uint32 command_out, uint32 result_out)
{
    uint32 command, result;
    FUNCTION_MARKER(0x80077C30, "SHELL.BIN");
    command = r_u32(0x8008F868);
    if (!command && !r_u32(0x8008F870)) return (uint32)-1;
    command = r_u32(0x8008F868);
    result = r_u32(0x8008F86C);
    if (!poll)
    {
        while (!r_u32(0x8008F870)) { }
    }
    else if (!r_u32(0x8008F870))
    {
        if (result_out) w_u32(result_out, result);
        if (command_out) w_u32(command_out, command);
        return 0;
    }
    if (result_out) w_u32(result_out, r_u32(0x8008F85C));
    if (command_out) w_u32(command_out, r_u32(0x8008F858));
    w_u32(0x8008F870, 0);
    return 1;
}

void shell_sub_80040DB0(uint32 screen, uint32 selection)
{
    uint32 list, element = 0;
    FUNCTION_MARKER(0x80040DB0, "SHELL.BIN");
    list = r_u32(screen + 0x40);
    if (selection == 3)
    {
        for (element = r_u32(list + 0x78); element; element = r_u32(element + 0xD0))
            if (r_s16(element + 0x32) == 0x309) w_u8(element + 0x39, 1);
        return;
    }
    if (list && (sint32)selection >= 0 && (sint32)selection <= (sint32)(r_u32(list + 0x80) - 1u))
        for (element = r_u32(list + 0x78); element; element = r_u32(element + 0xD0))
            if (r_u32(element + 0xD8) == selection) break;
    /* TODO The original writes guest address 0x39 when no matching element exists */
    w_u8(element + 0x39, 0);
}

uint32 shell_sub_80042BAC(void)
{
    FUNCTION_MARKER(0x80042BAC, "SHELL.BIN");
    shell_sub_8005D590();
    if ((uint8)shell_sub_8005F258(12))
    {
        w_u8(0x8007A274, 1);
        return 2;
    }
    if ((uint8)shell_sub_8005F258(6) || (uint8)shell_sub_8005F258(7) ||
        (uint8)shell_sub_8005F258(8) || (uint8)shell_sub_8005F258(9)) return 2;
    return 1;
}

void shell_sub_800437A4(uint32 screen)
{
    uint32 element = 0;
    FUNCTION_MARKER(0x800437A4, "SHELL.BIN");
    shell_sub_80043DF0(screen);
    shell_sub_8004E0BC(r_u32(screen + 0x50), 0x8003A1E8);
    if (screen && (sint32)(r_u32(screen + 0x28) - 1u) >= 0)
        for (element = r_u32(screen + 0x20); element && r_u32(element + 0x38);
            element = r_u32(element + 0xB4)) { }
    w_u32(0x8007A258, r_u32(r_u32(element + 0x74) + 0xD8) != 0);
}

void shell_sub_800454D0(void)
{
    uint32 index = 0, entry = 0x8007A608;
    FUNCTION_MARKER(0x800454D0, "SHELL.BIN");
    if (r_s32(0x8007A5FC) > 0)
    {
        do
        {
            shell_sub_80045328(r_u32(entry));
            ++index;
            w_u32(entry, 0);
            entry += 4;
        } while ((sint32)index < r_s32(0x8007A5FC));
    }
    shell_sub_80045328(r_u32(0x8007B654));
    w_u32(0x8007B654, 0);
    moh_draft_missing_call(0x800158AC, 0x8007A3A0u, 0u, 0x12B8u);
    moh_draft_missing_call(0x800158AC, 0x8007B668u, 0u, 7u);
    w_u8(0x8007B658, 0);
    w_u8(0x8007B664, 0);
}

uint32 shell_sub_80045F10(uint32 status)
{
    FUNCTION_MARKER(0x80045F10, "SHELL.BIN");
    switch (status)
    {
    case 0: return 0;
    case 1: return (uint32)-6;
    case 2: return (uint32)-5;
    case 3: return 2;
    case 4: return (uint32)-14;
    case 5: return (uint32)-8;
    case 6: return (uint32)-13;
    case 7: return (uint32)-16;
    default: return (uint32)-2;
    }
}

void shell_sub_800462CC(uint32 text, uint32 encoded)
{
    uint32 index = 0, character;
    FUNCTION_MARKER(0x800462CC, "SHELL.BIN");
    moh_draft_missing_call(0x8001E3FC, encoded, 0x40u);
    if (!r_u8(text)) return;
    do
    {
        character = r_u8(text + (index & 0xFFFF));
        ++index;
        character = shell_sub_800464A0(character);
        w_u8(encoded, (uint8)(character >> 8));
        w_u8(encoded + 1, (uint8)character);
        encoded += 2;
    } while (r_u8(text + (index & 0xFFFF)));
}

void shell_sub_80046358(uint32 encoded, uint32 text)
{
    uint32 offset, character;
    FUNCTION_MARKER(0x80046358, "SHELL.BIN");
    if (shell_sub_800465A8(encoded))
    {
        uint32 size = sub_8001AC88(encoded) + 1u;
        moh_draft_missing_call(0x800154EC, text, encoded, size);
        return;
    }
    for (offset = 0; offset < 0x40; offset += 2)
    {
        character = r_u8(encoded + offset);
        if (character)
        {
            character = (character << 8) | r_u8(encoded + offset + 1);
            w_u8(text, (uint8)shell_sub_800463F8(character));
        }
        else w_u8(text, 0);
        ++text;
    }
}

uint32 shell_sub_800463F8(uint32 character)
{
    uint32 index, low = character & 255;
    FUNCTION_MARKER(0x800463F8, "SHELL.BIN");
    character &= 0xFFFF;
    if ((character >> 8) != 0x81) return low - (low >= 0x81 ? 0x20u : 0x1Fu);
    for (index = 0; index < 32; ++index)
    {
        if (r_u16(0x8007A2A4 + index * 2) != character) continue;
        if (index < 16) return index + 0x20;
        if (index < 23) return index + 0x2A;
        if (index < 29) return index + 0x44;
        return index + 0x5E;
    }
    return 0;
}

uint32 shell_sub_800464A0(uint32 character)
{
    uint32 table = 0, group = 0, first, origin;
    FUNCTION_MARKER(0x800464A0, "SHELL.BIN");
    if ((uint8)(character - 0x20) < 16) group = 1;
    else if ((uint8)(character - 0x30) < 10) { }
    else if ((uint8)(character - 0x3A) < 7) group = 11;
    else if ((uint8)(character - 0x41) < 26) table = 1;
    else if ((uint8)(character - 0x5B) < 6) group = 37;
    else if ((uint8)(character - 0x61) < 26) table = 2;
    else if ((uint8)(character - 0x7B) < 4) group = 63;
    else return 0;
    character &= 255;
    if (group) return r_u16(0x8007A2A4 + (character - (group + 31)) * 2);
    first = r_u16(0x8007A298 + table * 4);
    origin = r_u16(0x8007A29A + table * 4);
    return first + character - origin;
}

uint32 shell_sub_800465A8(uint32 text)
{
    uint32 length, end, character, count = 0;
    FUNCTION_MARKER(0x800465A8, "SHELL.BIN");
    length = sub_8001AC88(text);
    if ((sint32)length > 64) length = 64;
    end = text + length;
    while (text < end)
    {
        character = r_u8(text);
        if (!(r_u8(0x8002F29D + character) & 0x97)) return 0;
        if (character - 0x41u < 26 || character - 0x61u < 26 || character - 0x30u < 10)
            ++count;
        character = r_u8(text);
        if (character == 0x82 || character == 0x60) --count;
        ++text;
    }
    return (sint32)count >= 4;
}

void shell_sub_80048830(uint32 unused, uint32 bit)
{
    uint32 flags;
    FUNCTION_MARKER(0x80048830, "SHELL.BIN");
    flags = r_u32(0x800386A0);
    w_u32(0x800386A0, flags | (1u << (bit & 31)));
}

void shell_sub_80048A10(uint32 screen)
{
    FUNCTION_MARKER(0x80048A10, "SHELL.BIN");
    shell_sub_8004E1B8(r_u32(screen + 0x50), 0);
}

uint32 shell_sub_8004BEEC(uint32 screen)
{
    uint32 data, element;
    FUNCTION_MARKER(0x8004BEEC, "SHELL.BIN");
    data = r_u32(screen + 0x44);
    if (!screen) return (uint32)-2;
    if (!data) return 1;
    for (element = r_u32(data + 0x78); element; element = r_u32(element + 0xD0))
        shell_sub_8004CFB4(element, 1);
    for (element = r_u32(data + 0x90); element; element = r_u32(element + 0x138))
        shell_sub_8004D508(element);
    return 0;
}

void shell_sub_8004C7A0(uint32 element, uint32 mode)
{
    uint32 current, primary;
    FUNCTION_MARKER(0x8004C7A0, "SHELL.BIN");
    if (!element) return;
    primary = r_u32(r_u32(element + 0xB8) + 0x1C) == element;
    current = r_u32(element + 0x70);
    if (current) shell_sub_8004CFB4(current, mode);
    if (primary)
    {
        current = r_u32(element + 0x74);
        if (current) shell_sub_8004CFB4(current, mode);
    }
    current = r_u32(element + 0x88);
    if (current) shell_sub_8004D508(current);
    if (primary)
    {
        current = r_u32(element + 0x8C);
        if (current) shell_sub_8004D508(current);
    }
}

void shell_sub_8004CA40(uint32 element)
{
    uint32 current, owner, mode;
    FUNCTION_MARKER(0x8004CA40, "SHELL.BIN");
    if (!element) return;
    current = r_u32(element + 0x78);
    owner = r_u32(element + 0xB8);
    while (current)
    {
        mode = 0;
        if (current == r_u32(element + 0x74)) mode = r_u32(owner + 0x1C) == element;
        shell_sub_8004CFB4(current, mode);
        current = r_u32(current + 0xD0);
    }
    for (current = r_u32(element + 0x90); current; current = r_u32(current + 0x138))
        shell_sub_8004D508(current);
}

void shell_sub_8004CCAC(uint32 element)
{
    uint32 current;
    FUNCTION_MARKER(0x8004CCAC, "SHELL.BIN");
    if (!element) return;
    current = r_u32(element + 0x70);
    if (current) shell_sub_8004CFB4(current, 1);
    current = r_u32(element + 0x88);
    if (current) shell_sub_8004D508(current);
    for (current = r_u32(element + 0x78); current; current = r_u32(current + 0xD0))
        shell_sub_8004CFB4(current, 1);
    for (current = r_u32(element + 0x90); current; current = r_u32(current + 0x138))
        shell_sub_8004D508(current);
}

void level2p_sub_80049D88(void)
{
    FUNCTION_MARKER(0x80049D88, "LEVEL2P.BIN");
}

uint32 level2p_sub_80049E24(void)
{
    FUNCTION_MARKER(0x80049E24, "LEVEL2P.BIN");
    return 0;
}

uint32 level2p_sub_8005A974(uint32 index, uint32 object)
{
    FUNCTION_MARKER(0x8005A974, "LEVEL2P.BIN");
    return object + (index << 2) + 0x4C;
}

void level2p_sub_8005C828(void)
{
    FUNCTION_MARKER(0x8005C828, "LEVEL2P.BIN");
}

void level2p_sub_8005C830(void)
{
    FUNCTION_MARKER(0x8005C830, "LEVEL2P.BIN");
}

void level2p_sub_8005CBA8(void)
{
    FUNCTION_MARKER(0x8005CBA8, "LEVEL2P.BIN");
    w_u32(0x800968D4, 120);
}

void level2p_sub_8005D788(void)
{
    FUNCTION_MARKER(0x8005D788, "LEVEL2P.BIN");
}

static void moh_level2p_property_result(uint32 object, uint32 property, uint32 extended)
{
    uint32 offset, width = 2, shift = 0, mask = ~0u, type = 10, result;
    if (!extended && property > 0x12) return;
    switch (property)
    {
    case 0: offset = 0x28; break;
    case 1: offset = 0x2A; break;
    case 2: offset = 0x2C; break;
    case 3: offset = 0x2E; break;
    case 4: offset = 0x32; break;
    case 5: offset = 0x34; break;
    case 0xD: offset = 0x54; width = 4; type = 11; break;
    case 0xE: offset = 0x30; width = 1; mask = 1; break;
    case 0xF: offset = 0x30; width = 4; shift = 1; mask = 1; break;
    case 0x10: offset = 0x30; width = 4; shift = 2; mask = 1; break;
    case 0x11: offset = 0x30; width = 4; shift = 3; mask = 1; break;
    case 0x12: offset = 0x36; break;
    case 0x13: offset = 0x58; width = 1; mask = 1; break;
    case 0x14: offset = 0x5E; break;
    case 0x15: offset = 0x59; width = 1; break;
    case 0x17: offset = 0x64; width = 4; type = 11; break;
    case 0x18: offset = 0x5A; width = 1; break;
    case 0x19: offset = 0x5B; width = 1; break;
    case 0x1A: offset = 0x5C; width = 1; break;
    case 0x1B: offset = 0x68; width = 4; type = 11; break;
    case 0x1C: offset = 0x5D; width = 1; break;
    default: return;
    }
    result = 0x800ABAF8 + (r_u32(0x80099388) << 3);
    w_u32(result, type);
    if (width == 1) w_u32(result + 4, (r_u8(object + offset) >> shift) & mask);
    else if (width == 2) w_u32(result + 4, (uint32)(sint32)r_s16(object + offset));
    else w_u32(result + 4, (r_u32(object + offset) >> shift) & mask);
}

void level2p_sub_8005F9D4(uint32 object, uint32 property)
{
    FUNCTION_MARKER(0x8005F9D4, "LEVEL2P.BIN");
    moh_level2p_property_result(object, property, 0);
}

void level2p_sub_8005FC2C(uint32 object, uint32 property)
{
    FUNCTION_MARKER(0x8005FC2C, "LEVEL2P.BIN");
    moh_level2p_property_result(object, property, 1);
}

void level2p_sub_80066C08(void)
{
    FUNCTION_MARKER(0x80066C08, "LEVEL2P.BIN");
}

void level2p_sub_800684D4(uint32 object, uint32 first, uint32 second, uint32 third)
{
    FUNCTION_MARKER(0x800684D4, "LEVEL2P.BIN");
    w_u32(object + 0x20C, first);
    w_u32(object + 0x210, second);
    w_u32(object + 0x214, third);
}

uint32 level2p_sub_80068974(uint32 value)
{
    uint32 previous;
    FUNCTION_MARKER(0x80068974, "LEVEL2P.BIN");
    previous = r_u32(0x80099400);
    w_u32(0x80099400, value);
    return previous;
}

uint32 level2p_sub_80068A40(uint32 unused, uint32 low, uint32 high)
{
    FUNCTION_MARKER(0x80068A40, "LEVEL2P.BIN");
    return (high << 16) | (low & 0xFFFF);
}

uint32 level2p_sub_80068A50(uint32 object)
{
    FUNCTION_MARKER(0x80068A50, "LEVEL2P.BIN");
    return r_u16(object);
}

void level2p_sub_80068A5C(uint32 object)
{
    FUNCTION_MARKER(0x80068A5C, "LEVEL2P.BIN");
    w_u32(object, 0);
}

void level2p_sub_80068E58(uint32 object)
{
    FUNCTION_MARKER(0x80068E58, "LEVEL2P.BIN");
    w_u16(object + 2, 1);
}

void level2p_sub_8006A9C4(uint32 object, uint32 first, uint32 second, uint32 third)
{
    FUNCTION_MARKER(0x8006A9C4, "LEVEL2P.BIN");
    w_u32(object + 4, first);
    w_u32(object + 8, second);
    w_u32(object + 12, third);
}

void level2p_sub_8006F44C(uint32 object)
{
    FUNCTION_MARKER(0x8006F44C, "LEVEL2P.BIN");
    w_u32(object + 0x284, 0);
}

void level2p_sub_80085F78(void)
{
    FUNCTION_MARKER(0x80085F78, "LEVEL2P.BIN");
}

void level2p_sub_800864D8(void)
{
    FUNCTION_MARKER(0x800864D8, "LEVEL2P.BIN");
}

uint32 level2p_sub_80087870(void)
{
    FUNCTION_MARKER(0x80087870, "LEVEL2P.BIN");
    return r_u32(0x800B60F0);
}

void level2p_sub_80089E74(void)
{
    FUNCTION_MARKER(0x80089E74, "LEVEL2P.BIN");
}

void level2p_sub_80089E7C(void)
{
    FUNCTION_MARKER(0x80089E7C, "LEVEL2P.BIN");
}

uint32 level2p_sub_80089E84(void)
{
    FUNCTION_MARKER(0x80089E84, "LEVEL2P.BIN");
    return 1;
}

void level2p_sub_80089E8C(void)
{
    FUNCTION_MARKER(0x80089E8C, "LEVEL2P.BIN");
}

uint32 level2p_sub_8008AFF4(uint32 object)
{
    FUNCTION_MARKER(0x8008AFF4, "LEVEL2P.BIN");
    w_u32(object + 0x18, 0);
    return object;
}

void level2p_sub_8008BCC4(void)
{
    FUNCTION_MARKER(0x8008BCC4, "LEVEL2P.BIN");
}

void level2p_sub_8008BCCC(void)
{
    FUNCTION_MARKER(0x8008BCCC, "LEVEL2P.BIN");
}

void level2p_sub_8008BCD4(void)
{
    FUNCTION_MARKER(0x8008BCD4, "LEVEL2P.BIN");
}

static void moh_level2p_property_variant(uint32 object, uint32 property, uint32 layout)
{
    uint32 offset, width = 1, type = 10, mask = ~0u, result;
    if (property <= 0x12 || (layout == 6 && property <= 0x1C))
    {
        moh_level2p_property_result(object, property, layout == 6);
        return;
    }
    if (layout == 3)
    {
        switch (property)
        {
        case 0x13: offset = 0x58; mask = 1; break;
        case 0x14: offset = 0x68; width = 2; break;
        case 0x15: offset = 0x59; break;
        case 0x17: offset = 0x78; width = 4; type = 11; break;
        case 0x18: offset = 0x5F; break;
        case 0x19: offset = 0x60; break;
        case 0x1A: offset = 0x61; break;
        case 0x1B: offset = 0x7C; width = 4; type = 11; break;
        case 0x1C: offset = 0x62; break;
        case 0x1D: offset = 0x5A; break;
        case 0x1E: offset = 0x63; break;
        case 0x1F: offset = 0x5B; break;
        case 0x20: offset = 0x64; break;
        case 0x21: offset = 0x5C; break;
        case 0x22: offset = 0x6A; width = 2; break;
        case 0x23: offset = 0x65; break;
        case 0x24: offset = 0x66; break;
        case 0x25: offset = 0x5D; break;
        case 0x26: offset = 0x6C; width = 2; break;
        case 0x27: offset = 0x6E; width = 2; break;
        case 0x28: offset = 0x5E; break;
        case 0x29: offset = 0x70; width = 2; break;
        default: return;
        }
    }
    else if (layout == 4)
    {
        switch (property)
        {
        case 0x13: offset = 0x58; mask = 1; break;
        case 0x14: offset = 0x5E; width = 2; break;
        case 0x15: offset = 0x59; break;
        case 0x16: offset = 0x60; width = 4; type = 11; break;
        case 0x17: offset = 0x64; width = 4; type = 11; break;
        case 0x18: offset = 0x5A; break;
        case 0x19: offset = 0x5B; break;
        case 0x1A: offset = 0x5C; break;
        default: return;
        }
    }
    else if (layout == 5 && property == 0x13)
    {
        offset = 0x58; width = 4; type = 11;
    }
    else if (layout == 6 && property == 0x1D)
    {
        offset = 0x6C; width = 4; type = 11;
    }
    else return;
    result = 0x800ABAF8 + (r_u32(0x80099388) << 3);
    w_u32(result, type);
    if (width == 1) w_u32(result + 4, r_u8(object + offset) & mask);
    else if (width == 2) w_u32(result + 4, (uint32)(sint32)r_s16(object + offset));
    else w_u32(result + 4, r_u32(object + offset));
}

void level2p_sub_80060010(uint32 object, uint32 property)
{
    FUNCTION_MARKER(0x80060010, "LEVEL2P.BIN");
    moh_level2p_property_variant(object, property, 3);
}

void level2p_sub_80060630(uint32 object, uint32 property)
{
    FUNCTION_MARKER(0x80060630, "LEVEL2P.BIN");
    moh_level2p_property_variant(object, property, 4);
}

void level2p_sub_800609E8(uint32 object, uint32 property)
{
    FUNCTION_MARKER(0x800609E8, "LEVEL2P.BIN");
    moh_level2p_property_variant(object, property, 5);
}

void level2p_sub_80060C6C(uint32 object, uint32 property)
{
    FUNCTION_MARKER(0x80060C6C, "LEVEL2P.BIN");
    moh_level2p_property_variant(object, property, 6);
}

void level2p_sub_8006107C(uint32 owner, uint32 instance, uint32 property)
{
    uint32 object;
    FUNCTION_MARKER(0x8006107C, "LEVEL2P.BIN");
    if (instance) object = r_u32(instance + 4);
    else
    {
        if (!owner) return;
        object = r_u32(r_u32(owner + 0x1C) + 4);
    }
    switch (r_u32(object + 0xC))
    {
    case 1: level2p_sub_8005F9D4(object, property); break;
    case 2: level2p_sub_8005FC2C(object, property); break;
    case 3: level2p_sub_80060010(object, property); break;
    case 4: level2p_sub_80060630(object, property); break;
    case 5: level2p_sub_800609E8(object, property); break;
    case 6: level2p_sub_80060C6C(object, property); break;
    }
}

uint32 level2p_sub_8005309C(uint32 owner, uint32 instance, uint32 category)
{
    uint32 list, end, value;
    FUNCTION_MARKER(0x8005309C, "LEVEL2P.BIN");
    if (owner) instance = r_u32(owner + 0x1C);
    if (!instance) return 0;
    list = r_u32(r_u32(instance + 4) + 0x48);
    if (!list) return 0;
    end = list + 4 + (r_u32(list) << 2);
    list += 4;
    while (list < end)
    {
        value = r_u8(list);
        list += 4;
        if (value == category) return 1;
    }
    return 0;
}

uint32 level2p_sub_8005311C(uint32 left_owner, uint32 left_instance, uint32 right_owner, uint32 right_instance, uint32 category)
{
    uint32 left_object = 0, right_object = 0, left, right, left_count, right_count;
    uint32 left_end, right_end, left_value, right_value;
    FUNCTION_MARKER(0x8005311C, "LEVEL2P.BIN");
    if (left_owner) left_instance = r_u32(left_owner + 0x1C);
    if (left_instance) left_object = r_u32(left_instance + 4);
    if (right_owner) right_instance = r_u32(right_owner + 0x1C);
    if (right_instance) right_object = r_u32(right_instance + 4);
    left = r_u32(left_object + 0x48);
    if (!left) return 0;
    right = r_u32(right_object + 0x48);
    if (!right) return 0;
    left_count = r_u32(left);
    right_count = r_u32(right);
    if (!left_count || !right_count) return 0;
    left += 4;
    right += 4;
    left_end = left + (left_count << 2);
    right_end = right + (right_count << 2);
    while (left < left_end && r_u16(left + 2) < category) left += 4;
    while (right < right_end && r_u16(right + 2) < category) right += 4;
    while (left < left_end && right < right_end)
    {
        left_value = r_u32(left);
        if ((left_value >> 16) != category) return 0;
        right_value = r_u32(right);
        if ((right_value >> 16) != category) return 0;
        if (left_value < right_value) left += 4;
        else
        {
            right += 4;
            if (left_value == right_value) return 1;
        }
    }
    return 0;
}

void level2p_sub_8005F984(void)
{
    FUNCTION_MARKER(0x8005F984, "LEVEL2P.BIN");
    w_u32(0x80098B8C, r_u32(0x80098B8C) | 0x40);
}

void level2p_sub_8005F9A0(uint32 format, ...)
{
    FUNCTION_MARKER(0x8005F9A0, "LEVEL2P.BIN");
    /* Original body only stores unused argument home words */
}

uint32 level2p_sub_8005F9B8(uint32 object)
{
    FUNCTION_MARKER(0x8005F9B8, "LEVEL2P.BIN");
    return object ? r_u32(object + 0xC) & 0x80000u : 1u;
}

uint32 level2p_sub_80061148(uint32 owner, uint32 instance, uint32 offset)
{
    FUNCTION_MARKER(0x80061148, "LEVEL2P.BIN");
    if (instance) owner = r_u32(instance + 4);
    else if (!owner) return 0;
    return owner + 0x20 + offset;
}

uint32 level2p_sub_80061174(uint32 owner, uint32 instance)
{
    uint32 object, type;
    FUNCTION_MARKER(0x80061174, "LEVEL2P.BIN");
    if (instance) return r_u32(r_u32(instance + 4) + 0x54);
    if (!owner || level2p_sub_80068A64(owner)) return ~0u;
    object = r_u32(r_u32(owner + 0x1C) + 4);
    type = r_u32(object + 0xC);
    if (type == 3) return r_u32(object + 0x78);
    if (type == 2 || type == 4 || type == 6) return r_u32(object + 0x64);
    return ~0u;
}

void level2p_sub_8006123C(uint32 flags, uint32 object)
{
    FUNCTION_MARKER(0x8006123C, "LEVEL2P.BIN");
    w_u32(object + 0x40, r_u32(object + 0x40) | flags);
}

uint32 level2p_sub_80061250(void)
{
    FUNCTION_MARKER(0x80061250, "LEVEL2P.BIN");
    w_u32(0x8009938C, r_u32(r_u32(0x80098FB0) + 0x59C));
    if (!level2p_sub_800618D4()) return 0;
    w_u32(0x8009939C, 0);
    w_u32(0x80099398, 0);
    moh_draft_missing_call(0x800158AC, 0x800993A0u, 0u, 0x58u);
    w_u32(0x800993A0, 0x80064658);
    w_u32(0x800993A4, 0x80064858);
    w_u32(0x800993A8, 0x80064A34);
    w_u32(0x800993AC, 0x80064B4C);
    w_u32(0x800993B0, 0x80064BB8);
    w_u32(0x800993B4, 0x80064C10);
    w_u32(0x800993B8, 0x80064C58);
    w_u32(0x800993D4, 0x80064E48);
    w_u32(0x800993D8, 0x80064F30);
    w_u32(0x800993DC, 0x80064F88);
    w_u32(0x800993E0, 0x80064FC8);
    w_u32(0x800993E4, 0x80065008);
    w_u32(0x800993BC, 0x80064EA0);
    w_u32(0x800993C0, 0x80065108);
    w_u32(0x800993C4, 0x8006517C);
    w_u32(0x800993C8, 0x8006520C);
    w_u32(0x800993CC, ~0u);
    w_u32(0x800993D0, 0x80064AAC);
    w_u32(0x800993E8, 0x80065070);
    w_u32(0x800993F4, 0x800650B0);
    w_u32(0x800993EC, 0x80064CCC);
    w_u32(0x800993F0, 0x80064E08);
    w_u32(0x80094908, 120);
    return 1;
}

void level2p_sub_800613D4(void)
{
    FUNCTION_MARKER(0x800613D4, "LEVEL2P.BIN");
    if (r_u32(0x8009938C)) w_u32(0x8009938C, 0);
    level2p_sub_800619A0();
    w_u32(0x8009939C, 0);
    w_u32(0x80099398, 0);
}

uint32 level2p_sub_80061410(uint32 event, uint32 owner, uint32 instance, uint32 payload)
{
    FUNCTION_MARKER(0x80061410, "LEVEL2P.BIN");
    return level2p_sub_80061FE4(event, owner, instance, payload, 0);
}

uint32 level2p_sub_80061430(void)
{
    uint32 tail, head, count;
    FUNCTION_MARKER(0x80061430, "LEVEL2P.BIN");
    tail = r_u32(0x8009939C);
    head = r_u32(0x80099398);
    count = tail - head;
    if ((sint32)count < 0) count += 128;
    return count;
}

uint32 level2p_sub_8006145C(void)
{
    uint32 head, tail, next;
    FUNCTION_MARKER(0x8006145C, "LEVEL2P.BIN");
    head = r_u32(0x80099398);
    tail = r_u32(0x8009939C);
    if (head != tail)
    {
        if (level2p_sub_800652C8(0x800B4CF8 + (head << 5)))
            level2p_sub_80062424(0x800B4CF8 + (r_u32(0x80099398) << 5));
        next = r_u32(0x80099398) + 1u;
        w_u32(0x80099398, (sint32)next < 128 ? next : 0);
    }
    head = r_u32(0x80099398);
    tail = r_u32(0x8009939C);
    return head == tail;
}

uint32 level2p_sub_800614FC(void)
{
    uint32 count, processed = 0, next, head, tail;
    FUNCTION_MARKER(0x800614FC, "LEVEL2P.BIN");
    count = level2p_sub_80061430();
    if ((sint32)count > 0)
    {
        do
        {
            ++processed;
            level2p_sub_8005F9A0(0x8003C780, processed, count);
            if (level2p_sub_800652C8(0x800B4CF8 + (r_u32(0x80099398) << 5)))
                level2p_sub_80062424(0x800B4CF8 + (r_u32(0x80099398) << 5));
            next = r_u32(0x80099398) + 1u;
            w_u32(0x80099398, (sint32)next < 128 ? next : 0);
        } while ((sint32)processed < (sint32)count);
    }
    head = r_u32(0x80099398);
    tail = r_u32(0x8009939C);
    return head == tail;
}

void level2p_sub_800615E4(uint32 owner, uint32 instance, uint32 event)
{
    uint32 handler_id, data, handlers, index = 0, record, message, storage[8];
    FUNCTION_MARKER(0x800615E4, "LEVEL2P.BIN");
    if (!owner && !instance)
    {
        moh_draft_missing_call(0x80015B28, 0x8003C7ACu);
        return;
    }
    handler_id = level2p_sub_80061174(owner, instance);
    if (handler_id == ~0u) return;
    data = r_u32(0x8009938C);
    handlers = data + r_u32(data + (handler_id << 2) + 8);
    while (index < r_u32(handlers))
    {
        record = level2p_sub_8006460C(index, handlers);
        ++index;
        if (r_u16(record) != event) continue;
        /* TODO Original leaves the last two message words uninitialized */
        message = moh_draft_local_address(storage, sizeof(storage));
        w_u32(message, handlers + r_u16(record + 2));
        level2p_sub_80068A04(message + 4, owner);
        w_u32(message + 0x10, instance);
        level2p_sub_80068A04(message + 8, 0);
        w_u32(message + 0x14, 0);
        level2p_sub_80068A04(message + 0xC, 0);
        level2p_sub_80062424(message);
        return;
    }
}

void level2p_sub_80061700(uint32 owner, uint32 instance)
{
    FUNCTION_MARKER(0x80061700, "LEVEL2P.BIN");
    if (!owner && !instance) moh_draft_missing_call(0x80015B28, 0x8003C7E8u);
    else level2p_sub_800615E4(owner, instance, 0);
}

void level2p_sub_80061740(uint32 owner, uint32 instance)
{
    uint32 data, count = 0, index, offset = 0, current, previous, removed, matches, next;
    FUNCTION_MARKER(0x80061740, "LEVEL2P.BIN");
    data = r_u32(owner + 0x1C);
    if (data && r_u32(r_u32(data + 4) + 0xC)) level2p_sub_800615E4(owner, 0, 0x1A);
    data = r_u32(0x8009938C);
    if (data) count = r_u32(data);
    for (index = 0; index < count; ++index, offset += 4)
    {
        previous = 0;
        current = r_u32(r_u32(0x80099390) + offset);
        while (current)
        {
            matches = 0;
            if (owner)
            {
                /* TODO Owner addressability uses the permitted missing local adapter */
                matches = level2p_sub_80068988(current, moh_draft_local_address(&owner, sizeof(owner)));
                if (!matches)
                    matches = level2p_sub_80068988(current + 0xC, moh_draft_local_address(&owner, sizeof(owner)));
            }
            if (!matches && instance)
            {
                matches = instance == r_u32(current + 4);
                if (!matches) matches = instance == r_u32(current + 0x10);
            }
            if (matches)
            {
                if (!previous)
                {
                    data = r_u32(0x80099390);
                    next = r_u32(current + 0x20);
                    w_u32(data + offset, next);
                }
                else w_u32(previous + 0x20, r_u32(current + 0x20));
                removed = current;
                current = r_u32(current + 0x20);
                level2p_sub_80061A5C(removed);
            }
            else
            {
                previous = current;
                current = r_u32(current + 0x20);
            }
        }
    }
}

uint32 level2p_sub_800618D4(void)
{
    uint32 data, count = 0, table, index, entry;
    FUNCTION_MARKER(0x800618D4, "LEVEL2P.BIN");
    data = r_u32(0x8009938C);
    if (data) count = r_u32(data);
    level2p_sub_800619A0();
    table = sub_80036BF0(count << 2);
    w_u32(0x80099390, table);
    if (!table)
    {
        moh_draft_missing_call(0x80015B28, 0x8003C828u);
        return 0;
    }
    for (index = 0; index < count; ++index) w_u32(table + (index << 2), 0);
    w_u32(0x80099394, 0x800ABCF8);
    entry = 0x800ABCF8;
    for (index = 0; index < 1023; ++index)
    {
        w_u32(entry + 0x20, entry + 0x24);
        entry += 0x24;
    }
    w_u32(0x800B4CF4, 0);
    return 1;
}

void level2p_sub_800619A0(void)
{
    uint32 table;
    FUNCTION_MARKER(0x800619A0, "LEVEL2P.BIN");
    table = r_u32(0x80099390);
    if (table)
    {
        sub_80036E44(table);
        w_u32(0x80099390, 0);
    }
}

uint32 level2p_sub_800619D8(void)
{
    uint32 entry;
    FUNCTION_MARKER(0x800619D8, "LEVEL2P.BIN");
    entry = r_u32(0x80099394);
    if (!entry)
    {
        moh_draft_missing_call(0x80015B28, 0x8003C86Cu);
        return 0;
    }
    w_u32(0x80099394, r_u32(entry + 0x20));
    level2p_sub_80068A04(entry, 0);
    w_u32(entry + 4, 0);
    w_u32(entry + 8, 0);
    level2p_sub_80068A04(entry + 0xC, 0);
    w_u32(entry + 0x10, 0);
    w_u32(entry + 0x14, ~0u);
    w_u32(entry + 0x18, 0);
    w_u32(entry + 0x1C, ~0u);
    w_u32(entry + 0x20, 0);
    return entry;
}

void level2p_sub_80061A5C(uint32 entry)
{
    uint32 previous;
    FUNCTION_MARKER(0x80061A5C, "LEVEL2P.BIN");
    previous = r_u32(0x80099394);
    w_u32(0x80099394, entry);
    w_u32(entry + 0x20, previous);
}

uint32 level2p_sub_80061FE4(uint32 event, uint32 owner, uint32 instance, uint32 payload, uint32 receiver)
{
    uint32 data, registration, filter, match, target, handle, other_owner, other_instance, category;
    FUNCTION_MARKER(0x80061FE4, "LEVEL2P.BIN");
    data = r_u32(0x8009938C);
    if (data ? (sint32)event >= r_s32(data) : (sint32)event >= 0)
    {
        moh_draft_missing_call(0x80015B28, 0x8003C9F0u, event);
        return 0;
    }
    registration = r_u32(r_u32(0x80099390) + (event << 2));
    while (registration)
    {
        filter = r_u32(registration + 8);
        match = 0;
        switch (filter)
        {
        case 0: match = 1; break;
        case 1:
        case 4:
            target = instance ? instance : r_u32(owner + 0x1C);
            if (target)
            {
                target = (uint32)(sint32)r_s16(r_u32(target + 4) + 0x34);
                match = r_u32(registration + 0x14) == target;
            }
            break;
        case 2:
            match = level2p_sub_8005309C(owner, instance, r_u32(registration + 0x14)) != 0;
            break;
        case 3:
            if (owner)
                match = level2p_sub_80068988(registration + 0xC,
                    moh_draft_local_address(&owner, sizeof(owner))) != 0;
            if (!match && instance) match = instance == r_u32(registration + 0x10);
            break;
        case 5:
            handle = r_u32(registration + 0xC);
            other_owner = handle ? r_u32(0x80099460 + ((handle & 0xFFFF) << 2)) : 0;
            category = r_u32(registration + 0x14);
            other_instance = r_u32(registration + 0x10);
            match = level2p_sub_8005311C(owner, instance, other_owner, other_instance, category) != 0;
            break;
        default:
            moh_draft_missing_call(0x80015B28, 0x8003CA18u, filter);
            return 0;
        }
        if (match && (!receiver || receiver == r_u32(registration + 4)))
        {
            level2p_sub_8005F9A0(0x8003CA54, 0u);
            level2p_sub_8006222C(registration, owner, instance, payload, event);
        }
        registration = r_u32(registration + 0x20);
    }
    return 1;
}

uint32 level2p_sub_8006222C(uint32 registration, uint32 owner, uint32 instance, uint32 sender, uint32 event)
{
    uint32 next, head, handle, object = 0, index, message, value;
    FUNCTION_MARKER(0x8006222C, "LEVEL2P.BIN");
    next = r_u32(0x8009939C) + 1u;
    head = r_u32(0x80099398);
    if (head == ((sint32)next < 128 ? next : 0))
    {
        moh_draft_missing_call(0x80015B28, 0x8003CA90u);
        return 0;
    }
    handle = r_u32(registration);
    if (handle) object = r_u32(0x80099460 + ((handle & 0xFFFF) << 2));
    if (object)
    {
        handle = r_u32(registration);
        object = handle ? r_u32(0x80099460 + ((handle & 0xFFFF) << 2)) : 0;
        if (level2p_sub_8005F9B8(object)) return 1;
    }
    if (owner && level2p_sub_8005F9B8(owner)) owner = 0;
    if (sender && level2p_sub_8005F9B8(sender)) sender = 0;
    index = r_u32(0x8009939C);
    value = r_u32(registration + 0x18);
    message = 0x800B4CF8 + (index << 5);
    w_u32(message, value);
    w_u32(message + 4, r_u32(registration));
    w_u32(message + 0x10, r_u32(registration + 4));
    level2p_sub_80068A04(message + 8, owner);
    message = 0x800B4CF8 + (r_u32(0x8009939C) << 5);
    w_u32(message + 0x14, instance);
    level2p_sub_80068A04(message + 0xC, sender);
    index = r_u32(0x8009939C);
    value = r_u32(registration + 0x1C);
    message = 0x800B4CF8 + (index << 5);
    w_u32(message + 0x1C, value);
    next = index + 1u;
    w_u32(message + 0x18, event);
    w_u32(0x8009939C, (sint32)next < 128 ? next : 0);
    return 1;
}

uint32 level2p_sub_8006460C(uint32 index, uint32 handlers)
{
    FUNCTION_MARKER(0x8006460C, "LEVEL2P.BIN");
    if (index < r_u32(handlers)) return handlers + 4 + (index << 2);
    moh_draft_missing_call(0x80015B28, 0x8003CC30u, index);
    return 0;
}

uint32 level2p_sub_800652C8(uint32 message)
{
    uint32 object = 0, handle, instance = 0, event, registration;
    FUNCTION_MARKER(0x800652C8, "LEVEL2P.BIN");
    if (!level2p_sub_80068A64(message + 4))
    {
        handle = r_u32(message + 4);
        if (handle) object = r_u32(0x80099460 + ((handle & 0xFFFF) << 2));
    }
    if (!object)
    {
        instance = r_u32(message + 0x10);
        if (!instance) return 0;
    }
    event = r_u32(message + 0x18);
    registration = r_u32(r_u32(0x80099390) + (event << 2));
    if (!event) return 1;
    while (registration)
    {
        if (object)
        {
            if (level2p_sub_80068988(registration, moh_draft_local_address(&object, sizeof(object)))) return 1;
        }
        else if (r_u32(registration + 4) == instance) return 1;
        registration = r_u32(registration + 0x20);
    }
    return 0;
}

uint32 level2p_sub_80068988(uint32 handle, uint32 object_view)
{
    uint32 value, object = 0;
    FUNCTION_MARKER(0x80068988, "LEVEL2P.BIN");
    if (level2p_sub_80068A64(handle)) return r_u32(object_view) == 0;
    value = r_u32(handle);
    if (value) object = r_u32(0x80099460 + ((value & 0xFFFF) << 2));
    return r_u32(object_view) == object;
}

uint32 level2p_sub_80068A04(uint32 destination, uint32 source)
{
    FUNCTION_MARKER(0x80068A04, "LEVEL2P.BIN");
    if (source) w_u32(destination, r_u32(source));
    else level2p_sub_80068A5C(destination);
    return source;
}

uint32 level2p_sub_80068A64(uint32 handle)
{
    uint32 index, generation, current_generation, object;
    FUNCTION_MARKER(0x80068A64, "LEVEL2P.BIN");
    if (!r_u32(handle)) return 1;
    index = level2p_sub_80068A50(handle) << 2;
    generation = r_u16(handle + 2);
    current_generation = r_u32(0x80099860 + index);
    if (current_generation != generation) return 1;
    object = r_u32(0x80099460 + index);
    return r_u32(object) == 0xDEDEDEDE;
}

typedef struct
{
    uint32 pair_address;
    uint32 pair_redirected;
    uint32 result[2];
    uint32 result_ready;
} MOH_MESSAGE_VALUE_CONTEXT;

uint32 level2p_sub_800645E0(uint32 offset, uint32 alignment)
{
    uint32 remainder;
    FUNCTION_MARKER(0x800645E0, "LEVEL2P.BIN");
    remainder = (sint32)alignment < 2 ? 0 : offset & (alignment - 1);
    return remainder ? offset + alignment - remainder : offset;
}

static uint32 moh_message_handle(uint32 message, uint32 offset)
{
    uint32 handle = r_u32(message + offset);
    return handle ? r_u32(0x80099460 + ((handle & 0xFFFF) << 2)) : 0;
}

uint32 sub_8001D590(uint32 value)
{
    uint32 sign = 0, exponent = 0x9D;
    FUNCTION_MARKER(0x8001D590, "SLUS_009.74");
    if (!value) return 0;
    if (value == 0x80000000) return 0xCF000000;
    if ((sint32)value < 0)
    {
        sign = 0x80000000;
        value = 0 - value;
    }
    while ((sint32)value <= 0xFFFFFF)
    {
        value <<= 4;
        exponent -= 4;
    }
    while ((sint32)value <= 0x3FFFFFFF)
    {
        value <<= 1;
        --exponent;
    }
    value += 0x40;
    if ((sint32)value < 0)
    {
        ++exponent;
        value >>= 8;
    }
    else value >>= 7;
    return sign | (exponent << 23) | (value & 0xFF7FFFFF);
}

uint32 sub_8001D7B0(uint32 first, uint32 second)
{
    FUNCTION_MARKER(0x8001D7B0, "SLUS_009.74");
    if (first == second) return 0;
    return (first & 0x7FFFFFFF) != 0 || (second & 0x7FFFFFFF) != 0;
}

uint32 sub_8001D880(uint32 first, uint32 second)
{
    uint32 sign, first_exponent, second_exponent, first_fraction, second_fraction;
    FUNCTION_MARKER(0x8001D880, "SLUS_009.74");
    if (first == second || (!(first & 0x7FFFFFFF) && !(second & 0x7FFFFFFF))) return 0;
    sign = first & 0x80000000;
    if (sign != (second & 0x80000000)) return sign ? 0xFFFFFFFF : 0;
    first_exponent = (first >> 23) & 255;
    second_exponent = (second >> 23) & 255;
    if (first_exponent < second_exponent) return sign ? 0 : 0xFFFFFFFF;
    if (first_exponent == second_exponent)
    {
        first_fraction = (first & 0x7FFFFF) | 0x800000;
        second_fraction = (second & 0x7FFFFF) | 0x800000;
        if (first_fraction < second_fraction) return sign ? 0 : 0xFFFFFFFF;
    }
    return (sint32)first < 0 ? 0xFFFFFFFF : 0;
}

uint32 sub_8001D7E0(uint32 first, uint32 second)
{
    uint32 sign, first_exponent, second_exponent, first_fraction, second_fraction;
    FUNCTION_MARKER(0x8001D7E0, "SLUS_009.74");
    if (first == second || (!(first & 0x7FFFFFFF) && !(second & 0x7FFFFFFF))) return 0;
    sign = first & 0x80000000;
    if (sign != (second & 0x80000000)) return sign == 0;
    first_exponent = (first >> 23) & 255;
    second_exponent = (second >> 23) & 255;
    if (first_exponent > second_exponent) return sign == 0;
    if (first_exponent != second_exponent) return (sint32)first < 0;
    first_fraction = (first & 0x7FFFFF) | 0x800000;
    second_fraction = (second & 0x7FFFFF) | 0x800000;
    if (first_fraction > second_fraction) return sign == 0;
    return (sint32)first < 0;
}

uint32 sub_8001D700(uint32 first, uint32 second)
{
    uint32 sign, first_exponent, second_exponent, first_fraction, second_fraction;
    FUNCTION_MARKER(0x8001D700, "SLUS_009.74");
    if (first == second || (!(first & 0x7FFFFFFF) && !(second & 0x7FFFFFFF))) return 0;
    sign = first & 0x80000000;
    if (sign != (second & 0x80000000)) return sign == 0;
    first_exponent = (first >> 23) & 255;
    second_exponent = (second >> 23) & 255;
    if (first_exponent < second_exponent) return sign != 0;
    if (first_exponent != second_exponent) return sign == 0;
    first_fraction = (first & 0x7FFFFF) | 0x800000;
    second_fraction = (second & 0x7FFFFF) | 0x800000;
    if (first_fraction > second_fraction) return sign == 0;
    return sign != 0;
}

uint32 sub_8001D660(uint32 first, uint32 second)
{
    uint32 sign, first_exponent, second_exponent, first_fraction, second_fraction;
    FUNCTION_MARKER(0x8001D660, "SLUS_009.74");
    if (first == second || (!(first & 0x7FFFFFFF) && !(second & 0x7FFFFFFF))) return 0;
    sign = first & 0x80000000;
    if (sign == (second & 0x80000000))
    {
        first_exponent = (first >> 23) & 255;
        second_exponent = (second >> 23) & 255;
        if (first_exponent < second_exponent) return (sint32)first < 0 ? 0 : 0xFFFFFFFF;
        if (first_exponent == second_exponent)
        {
            first_fraction = (first & 0x7FFFFF) | 0x800000;
            second_fraction = (second & 0x7FFFFF) | 0x800000;
            if (first_fraction < second_fraction) return (sint32)first < 0 ? 0 : 0xFFFFFFFF;
        }
    }
    return sign ? 0xFFFFFFFF : 0;
}

static uint32 moh_message_unsigned_float(uint32 value)
{
    uint32 converted;
    if ((sint32)value >= 0) return sub_8001D590(value);
    converted = sub_8001D590((value & 1) | (value >> 1));
    return moh_draft_missing_call(0x8001D930, converted, converted);
}

static uint32 moh_message_float_pair(uint32 pair, uint32 top, uint32 below, uint32 *first, uint32 *second)
{
    uint32 value;
    switch (pair)
    {
    case 0xA0E:
        *first = sub_8001D590(r_u32(top + 4));
        *second = r_u32(below + 4);
        return 1;
    case 0xE0A:
        *second = sub_8001D590(r_u32(below + 4));
        *first = r_u32(top + 4);
        return 1;
    case 0xB0E:
        *first = moh_message_unsigned_float(r_u32(top + 4));
        value = r_u32(0x80099388) - 1;
        *second = r_u32(0x800ABAF8 + (value << 3) + 4);
        return 1;
    case 0xE0B:
        *second = moh_message_unsigned_float(r_u32(below + 4));
        *first = r_u32(top + 4);
        return 1;
    case 0xE0E:
        *first = r_u32(top + 4);
        *second = r_u32(below + 4);
        return 1;
    default: return 0;
    }
}

static uint32 moh_message_compare(uint32 opcode, uint32 pair, uint32 top, uint32 below)
{
    uint32 first, second, index, comparison;
    if (pair == 0xA0A || pair == 0xA0B || pair == 0xB0A || pair == 0xB0B ||
        (opcode == 4 && (pair == 0x1010 || pair == 0xF0F)))
    {
        if (pair == 0xA0A || pair == 0xA0B || pair == 0xB0A)
        {
            index = r_u32(0x80099388);
            top = 0x800ABAF8 + (index << 3);
            below = 0x800ABAF8 + ((index - 1) << 3);
        }
        first = r_u32(top + 4);
        second = r_u32(below + 4);
        if (opcode == 4)
        {
            if (pair == 0xF0F) return moh_draft_missing_call(0x8001ACC8, first, second) == 0;
            return first == second;
        }
        if (pair == 0xB0B)
        {
            if (opcode == 8) return first < second;
            if (opcode == 9) return second < first;
            if (opcode == 10) return !(second < first);
            return !(first < second);
        }
        if (opcode == 8) return (sint32)first < (sint32)second;
        if (opcode == 9) return (sint32)second < (sint32)first;
        if (opcode == 10) return !((sint32)second < (sint32)first);
        return !((sint32)first < (sint32)second);
    }
    if (!moh_message_float_pair(pair, top, below, &first, &second)) return 0;
    if (opcode == 4) return sub_8001D7B0(first, second) == 0;
    if (opcode == 8) return (sint32)sub_8001D880(first, second) < 0;
    if (opcode == 9) return (sint32)sub_8001D7E0(first, second) > 0;
    if (opcode == 10) return (sint32)sub_8001D700(first, second) <= 0;
    comparison = sub_8001D660(first, second);
    return (sint32)comparison >= 0;
}

static void moh_message_arithmetic(MOH_MESSAGE_VALUE_CONTEXT *context, uint32 opcode, uint32 pair, uint32 top, uint32 below)
{
    uint32 first, second, index, view, function;
    if (pair == 0xA0A || pair == 0xA0B || pair == 0xB0A || pair == 0xB0B)
    {
        context->result[0] = pair == 0xB0B ? 11 : 10;
        if (pair != 0xB0B)
        {
            index = r_u32(0x80099388);
            top = 0x800ABAF8 + (index << 3);
            below = 0x800ABAF8 + ((index - 1) << 3);
        }
        first = r_u32(top + 4);
        second = r_u32(below + 4);
        if (opcode == 12) context->result[1] = first + second;
        else if (opcode == 13) context->result[1] = first - second;
        else if (opcode == 14) context->result[1] = first * second;
        else
        {
            if (!second)
            {
                moh_draft_fail_fast("Original message division BREAK 0x1C00",
                    pair == 0xB0B ? 0x800638F8 : 0x80063890);
                return;
            }
            if (pair != 0xB0B && first == 0x80000000 && second == 0xFFFFFFFF)
            {
                moh_draft_fail_fast("Original message division BREAK 0x1800", 0x800638A8);
                return;
            }
            context->result[1] = pair == 0xB0B ? first / second :
                (uint32)((sint32)first / (sint32)second);
        }
        context->result_ready = 1;
    }
    else if (pair == 0xA0E || pair == 0xE0A || pair == 0xB0E || pair == 0xE0B || pair == 0xE0E)
    {
        context->result[0] = 14;
        moh_message_float_pair(pair, top, below, &first, &second);
        function = opcode == 12 ? 0x8001D930 : opcode == 13 ? 0x8001D3F0 :
            opcode == 14 ? 0x8001D450 : 0x8001D1F0;
        context->result[1] = moh_draft_missing_call(function, first, second);
        context->result_ready = 1;
    }
    else if (!context->result_ready)
    {
        /* TODO Original first unsupported pair reads untouched local result words */
        view = moh_draft_local_address(context->result, sizeof(context->result));
        context->result[0] = r_u32(view);
        context->result[1] = r_u32(view + 4);
        context->result_ready = 1;
    }
}

static uint32 moh_message_operand(uint32 handlers, uint32 *cursor, uint32 wide, MOH_MESSAGE_VALUE_CONTEXT *context)
{
    uint32 value;
    if (wide)
    {
        *cursor = level2p_sub_800645E0(*cursor, 2);
        /* TODO Original wide decoder replaces the later type-pair read pointer */
        context->pair_address = r_u8(handlers + *cursor);
        context->pair_redirected = 1;
        value = r_u16(context->pair_address);
        *cursor += 2;
    }
    else
    {
        value = r_u8(handlers + *cursor);
        ++*cursor;
    }
    return value;
}

static uint32 moh_message_field_address(uint32 message, uint32 handlers, uint32 kind, uint32 *cursor, uint32 *field_type, MOH_MESSAGE_VALUE_CONTEXT *context)
{
    uint32 owner, instance, offset;
    *field_type = r_u8(handlers + *cursor);
    ++*cursor;
    owner = moh_message_handle(message, 4);
    instance = r_u32(message + 0x10);
    offset = moh_message_operand(handlers, cursor, kind == 0xF1 || kind == 0xF3, context);
    if (kind - 0xF0 < 2) return level2p_sub_80061148(owner, instance, offset);
    return handlers + (r_u32(handlers) << 2) + 4 + offset;
}

static void moh_message_pop_binary(uint32 type, uint32 value)
{
    uint32 index = r_u32(0x80099388) - 1;
    uint32 entry = 0x800ABAF8 + (index << 3);
    w_u32(0x80099388, index);
    w_u32(entry, type);
    w_u32(entry + 4, value);
}

void level2p_sub_80062424(uint32 message)
{
    MOH_MESSAGE_VALUE_CONTEXT context;
    uint32 owner, handler_id, data, handlers, cursor, opcode, kind, field_type, field;
    uint32 index, top, below, pair, first, second, value, length, base, selector, instance;
    uint32 target, callback, aligned, wide;
    FUNCTION_MARKER(0x80062424, "LEVEL2P.BIN");
    context.pair_redirected = 0;
    context.result_ready = 0;
    owner = moh_message_handle(message, 4);
    handler_id = level2p_sub_80061174(owner, r_u32(message + 0x10));
    if (handler_id == 0xFFFFFFFF) return;
    data = r_u32(0x8009938C);
    w_u32(0x80099388, 0xFFFFFFFF);
    handlers = data + r_u32(data + (handler_id << 2) + 8);
    cursor = r_u32(message) - handlers;
    for (;;)
    {
        opcode = r_u8(handlers + cursor);
        ++cursor;
        switch (opcode)
        {
        case 0: break;
        case 1:
            index = r_u32(0x80099388);
            if ((sint32)index >= 0) w_u32(0x80099388, index - 1);
            break;
        case 2:
            index = r_u32(0x80099388) + 1;
            w_u32(0x80099388, index);
            kind = r_u8(handlers + cursor);
            ++cursor;
            if (kind - 0xF4 < 12)
            {
                field_type = r_u8(handlers + cursor);
                ++cursor;
                wide = field_type - 0xF7 < 3 || field_type - 0xFD < 3;
                selector = moh_message_operand(handlers, &cursor, wide, &context);
                base = kind >= 0xFA ? (wide ? 0xFD : 0xFA) : (wide ? 0xF7 : 0xF4);
                owner = 0;
                instance = 0;
                if (kind - base < 3) owner = moh_message_handle(message, 4 + ((kind - base) << 2));
                if (kind == base) instance = r_u32(message + 0x10);
                else if (kind == base + 1) instance = r_u32(message + 0x14);
                if (field_type == 16)
                {
                    top = 0x800ABAF8 + (r_u32(0x80099388) << 3);
                    w_u32(top, 16);
                    w_u32(top + 4, kind >= 0xFA ? owner : instance);
                }
                else level2p_sub_8006107C(owner, instance, selector);
                break;
            }
            if (kind - 0xF0 < 4)
            {
                field = moh_message_field_address(message, handlers, kind, &cursor, &field_type, &context);
                if (field_type >= 1 && field_type <= 9)
                {
                    top = 0x800ABAF8 + (r_u32(0x80099388) << 3);
                    w_u32(top, 11);
                    value = r_u8(field);
                    if (field_type < 9) value = (value >> ((field_type - 1) & 31)) & 1;
                    w_u32(top + 4, value);
                }
                break;
            }
            if (kind == 9)
            {
                value = r_u8(handlers + cursor);
                level2p_sub_8005F9A0(0x8003CAB4, value);
                top = 0x800ABAF8 + (r_u32(0x80099388) << 3);
                w_u32(top, 11);
                w_u32(top + 4, r_u8(handlers + cursor));
                ++cursor;
            }
            else if ((kind >= 10 && kind <= 14) || kind == 16)
            {
                top = 0x800ABAF8 + (r_u32(0x80099388) << 3);
                w_u32(top, kind);
                cursor = level2p_sub_800645E0(cursor, 4);
                if (kind == 14)
                {
                    value = r_u32(handlers + cursor);
                    cursor += 4;
                    top = 0x800ABAF8 + (r_u32(0x80099388) << 3);
                }
                else
                {
                    index = r_u32(0x80099388);
                    value = kind == 16 ? handlers + cursor : r_u32(handlers + cursor);
                    cursor += 4;
                    top = 0x800ABAF8 + (index << 3);
                }
                w_u32(top + 4, value);
            }
            else if (kind == 15)
            {
                top = 0x800ABAF8 + (r_u32(0x80099388) << 3);
                w_u32(top, 15);
                length = r_u8(handlers + cursor);
                ++cursor;
                w_u32(top + 4, handlers + cursor);
                cursor += length + 1;
            }
            else return;
            break;
        case 3:
            kind = r_u8(handlers + cursor);
            ++cursor;
            if (kind - 0xF0 < 4)
            {
                field = moh_message_field_address(message, handlers, kind, &cursor, &field_type, &context);
                if (field_type >= 1 && field_type <= 9)
                {
                    top = 0x800ABAF8 + (r_u32(0x80099388) << 3);
                    if (field_type < 9)
                    {
                        value = r_u32(top + 4);
                        if (value) w_u8(field, (uint8)(r_u8(field) | (1 << ((field_type - 1) & 31))));
                        else w_u8(field, (uint8)(r_u8(field) & ~(1 << ((field_type - 1) & 31))));
                    }
                    else
                    {
                        value = r_u32(top);
                        if ((sint32)value >= 10 && (sint32)value < 12) w_u8(field, r_u8(top + 4));
                        else if (value == 14)
                        {
                            value = moh_draft_missing_call(0x8001D340, r_u32(top + 4));
                            w_u8(field, (uint8)value);
                        }
                    }
                }
            }
            w_u32(0x80099388, r_u32(0x80099388) - 1);
            break;
        case 4:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            index = r_u32(0x80099388);
            below = 0x800ABAF8 + ((index - 1) << 3);
            top = 0x800ABAF8 + (index << 3);
            first = r_u8(below);
            second = r_u8(top);
            pair = context.pair_redirected ? r_u16(context.pair_address) : first | (second << 8);
            if (opcode == 4 || opcode < 12)
            {
                value = moh_message_compare(opcode, pair, top, below);
                moh_message_pop_binary(11, value);
            }
            else
            {
                moh_message_arithmetic(&context, opcode, pair, top, below);
                moh_message_pop_binary(context.result[0], context.result[1]);
            }
            break;
        case 5:
            index = r_u32(0x80099388);
            below = 0x800ABAF8 + ((index - 1) << 3);
            value = r_u32(below + 4);
            w_u32(0x80099388, index - 1);
            w_u32(below, 11);
            value = value ? r_u32(0x800ABAF8 + (index << 3) + 4) != 0 : 0;
            w_u32(below + 4, value);
            break;
        case 6:
            index = r_u32(0x80099388);
            below = 0x800ABAF8 + ((index - 1) << 3);
            w_u32(below, 11);
            value = r_u32(below + 4);
            w_u32(0x80099388, index - 1);
            if (!value) value = r_u32(0x800ABAF8 + (index << 3) + 4);
            w_u32(below + 4, value != 0);
            break;
        case 7:
            top = 0x800ABAF8 + (r_u32(0x80099388) << 3);
            value = r_u32(top + 4);
            w_u32(top, 11);
            w_u32(top + 4, value == 0);
            break;
        case 16:
            cursor = level2p_sub_800645E0(cursor, 2);
            field = handlers + cursor;
            index = r_u32(0x80099388);
            target = r_u16(field);
            top = 0x800ABAF8 + (index << 3);
            value = r_u32(top + 4);
            level2p_sub_8005F9A0(0x8003CB10, target, value);
            index = r_u32(0x80099388);
            value = r_u32(0x800ABAF8 + (index << 3) + 4);
            w_u32(0x80099388, index - 1);
            cursor = value ? cursor + 2 : r_u16(field);
            break;
        case 17:
            aligned = level2p_sub_800645E0(cursor, 2);
            field = handlers + aligned;
            level2p_sub_8005F9A0(0x8003CAB4, r_u16(field));
            cursor = r_u16(field);
            break;
        case 18:
            selector = r_u8(handlers + cursor);
            ++cursor;
            callback = r_u32(0x800993A0 + (selector << 2));
            moh_draft_indirect_call(callback, message + 4, handlers, handler_id);
            break;
        case 19:
            cursor = level2p_sub_800645E0(cursor, 2);
            selector = r_u16(handlers + cursor);
            cursor += 2;
            callback = r_u32(0x800993A0 + (selector << 2));
            moh_draft_indirect_call(callback, message + 4, handlers, handler_id);
            break;
        case 20: return;
        case 21:
            index = r_u32(0x80099388) + 1;
            w_u32(0x80099388, index);
            top = 0x800ABAF8 + (index << 3);
            w_u32(top, 11);
            value = r_u8(handlers + (r_u32(handlers) << 2) + 4);
            w_u32(top + 4, value == 0);
            break;
        case 22:
            w_u8(handlers + (r_u32(handlers) << 2) + 4, 1);
            break;
        case 23:
            kind = r_u8(handlers + cursor);
            ++cursor;
            if (kind == 9)
            {
                value = r_u8(handlers + cursor);
                ++cursor;
            }
            else if (kind == 10 || kind == 11)
            {
                cursor = level2p_sub_800645E0(cursor, 4);
                value = r_u8(handlers + cursor);
                cursor += 4;
            }
            else
            {
                moh_draft_missing_call(0x80015B28, 0x8003CAB8, kind);
                return;
            }
            level2p_sub_8005F9A0(0x8003CAB4, value);
            break;
        default:
            moh_draft_missing_call(0x80015B28, 0x8003CB20, opcode);
            return;
        }
    }
}
