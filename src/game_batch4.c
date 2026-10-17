#include "game_draft_signatures.h"
#include "xport_trace.h"
#include "psx.h"

void sub_8001874C(void)
{
    FUNCTION_MARKER(0x8001874C, "SLUS_009.74");
}

uint32 sub_8002B068(uint32 pattern, uint32 record)
{
    const char *native_pattern;
    void *native_record, *result;
    FUNCTION_MARKER(0x8002B068, "SLUS_009.74");
    native_pattern = (const char *)moh_draft_guest_pointer(pattern, 1);
    native_record = moh_draft_guest_pointer(record, 1);
    result = firstfile(native_pattern, native_record);
    return result ? moh_draft_local_address(result, 1) : 0;
}

void sub_8002B758(void)
{
    FUNCTION_MARKER(0x8002B758, "SLUS_009.74");
    w_u16(0x800D15D0, 0);
}

void sub_80024880(uint32 state, uint32 command, uint32 buffer, uint32 count)
{
    FUNCTION_MARKER(0x80024880, "SLUS_009.74");
    w_u8(state + 55, (uint8)command);
    w_u32(state + 44, buffer);
    w_u8(state + 54, (uint8)count);
}

uint32 sub_80025150(uint32 state)
{
    FUNCTION_MARKER(0x80025150, "SLUS_009.74");
    w_u8(state + 55, 69);
    w_u32(state + 44, 0);
    w_u8(state + 54, 0);
    return 69;
}

uint32 sub_800251C4(uint32 state)
{
    FUNCTION_MARKER(0x800251C4, "SLUS_009.74");
    w_u8(state + 55, 75);
    w_u32(state + 44, 0);
    w_u8(state + 54, 0);
    return 75;
}

uint32 sub_80028B90(uint32 callback)
{
    uint32 previous;
    FUNCTION_MARKER(0x80028B90, "SLUS_009.74");
    previous = r_u32(0x80035048);
    w_u32(0x80035048, callback);
    return previous;
}

uint32 sub_80017F54(void)
{
    uint32 pointer;
    FUNCTION_MARKER(0x80017F54, "SLUS_009.74");
    pointer = r_u32(0x8002E020);
    return r_u32(pointer);
}

uint32 sub_80019654(void)
{
    FUNCTION_MARKER(0x80019654, "SLUS_009.74");
    return moh_draft_missing_call(0x80019674, 0);
}

uint32 sub_80020CE8(void)
{
    FUNCTION_MARKER(0x80020CE8, "SLUS_009.74");
    return moh_draft_missing_call(0x8002A6F0);
}

uint32 sub_80022530(void)
{
    FUNCTION_MARKER(0x80022530, "SLUS_009.74");
    return moh_draft_missing_call(0x8002AE38, (uint32)-1);
}

uint32 sub_80022B7C(void)
{
    FUNCTION_MARKER(0x80022B7C, "SLUS_009.74");
    return moh_draft_missing_call(0x8002AE68) == 0;
}

uint32 sub_80025164(uint32 state, uint32 value)
{
    FUNCTION_MARKER(0x80025164, "SLUS_009.74");
    w_u8(state + 55, 76);
    w_u32(state + 44, state + 36);
    w_u8(state + 36, (uint8)value);
    w_u8(state + 54, 1);
    return 1;
}

uint32 sub_80025184(uint32 state, uint32 value)
{
    FUNCTION_MARKER(0x80025184, "SLUS_009.74");
    w_u8(state + 55, 70);
    w_u32(state + 44, state + 36);
    w_u8(state + 36, (uint8)value);
    w_u8(state + 54, 1);
    return 1;
}

uint32 sub_800251A4(uint32 state, uint32 value)
{
    FUNCTION_MARKER(0x800251A4, "SLUS_009.74");
    w_u8(state + 55, 71);
    w_u32(state + 44, state + 36);
    w_u8(state + 36, (uint8)value);
    w_u8(state + 54, 1);
    return 1;
}

uint32 sub_8001880C(uint32 callback, uint32 data, uint32 argument)
{
    FUNCTION_MARKER(0x8001880C, "SLUS_009.74");
    return (uint32)sub_80018830(callback, data, 0, argument);
}

uint32 sub_80028B60(uint32 callback)
{
    FUNCTION_MARKER(0x80028B60, "SLUS_009.74");
    return sub_8001CAD8(3, callback);
}

uint32 sub_800191D0(void)
{
    FUNCTION_MARKER(0x800191D0, "SLUS_009.74");
    return sub_8001CAD8(2, 0x80018AE0);
}

uint32 sub_800218B0(void)
{
    uint32 pointer, result;
    FUNCTION_MARKER(0x800218B0, "SLUS_009.74");
    pointer = r_u32(0x800302D0);
    result = (r_u32(pointer) & 0xF0FFFFFF) | 0x22000000;
    w_u32(pointer, result);
    return result;
}

uint32 sub_80024858(void)
{
    uint32 port, ready;
    FUNCTION_MARKER(0x80024858, "SLUS_009.74");
    port = r_u32(0x800347AC);
    do {
        ready = r_u16(port + 4) & 2;
    } while (!ready);
    return ready;
}

uint32 sub_8002BA68(uint32 first, uint32 second)
{
    FUNCTION_MARKER(0x8002BA68, "SLUS_009.74");
    return moh_draft_missing_call(0x8002C688, (uint32)(sint32)(sint16)first, (uint32)(sint32)(sint16)second);
}

uint32 sub_8001AB90(uint32 counter)
{
    uint32 index, table;
    FUNCTION_MARKER(0x8001AB90, "SLUS_009.74");
    index = counter & 0xFFFF;
    if (index >= 3) return 0;
    table = r_u32(0x8002E12C);
    w_u16(table + index * 16, 0);
    return 1;
}

uint32 sub_80024B84(uint32 state)
{
    uint32 first_count, second_count, extra;
    FUNCTION_MARKER(0x80024B84, "SLUS_009.74");
    first_count = r_u8(state + 227);
    second_count = r_u8(state + 233);
    extra = r_u16(state + 236);
    return (((first_count + 1) >> 1) << 2) + ((second_count * 5 + 3) & 0xFFC) + 4 + extra;
}

uint32 sub_80018754(uint32 source, uint32 count)
{
    uint32 control_port, data_port, value;
    FUNCTION_MARKER(0x80018754, "SLUS_009.74");
    control_port = r_u32(0x8002E020);
    w_u32(control_port, 0x04000000);
    while (count) {
        value = r_u32(source);
        source += 4;
        data_port = r_u32(0x8002E01C);
        --count;
        w_u32(data_port, value);
    }
    return 0;
}

uint32 sub_800201A4(void)
{
    uint32 callback;
    FUNCTION_MARKER(0x800201A4, "SLUS_009.74");
    if (!r_u32(0x8002FF70)) {
        w_u32(0x8002FF70, 1);
        return 1;
    }
    callback = r_u32(0x8002FF64);
    w_u32(0x8002FF70, 0);
    return moh_draft_indirect_call(callback);
}

uint32 sub_80020158(void)
{
    uint32 callback;
    FUNCTION_MARKER(0x80020158, "SLUS_009.74");
    callback = r_u32(0x8002FF68);
    if (callback) moh_draft_indirect_call(callback);
    callback = r_u32(0x8002FF64);
    return moh_draft_indirect_call(callback);
}

uint32 sub_800135F8(uint32 bounds, uint32 scratch)
{
    sint32 minimum[3], maximum[3], upper[3], lower[3];
    uint32 axis, accepted;
    FUNCTION_MARKER(0x800135F8, "SLUS_009.74");
    for (axis = 0; axis < 3; ++axis) minimum[axis] = (sint16)r_u16(scratch + 0x396 + axis * 2);
    for (axis = 0; axis < 3; ++axis) upper[axis] = (sint16)r_u16(bounds + 6 + axis * 2);
    if (minimum[0] >= upper[0]) return 0;
    if (minimum[1] >= upper[1]) return 0;
    accepted = minimum[2] < upper[2];
    maximum[0] = (sint16)r_u16(scratch + 0x39C);
    if (!accepted) return 0;
    maximum[1] = (sint16)r_u16(scratch + 0x39E);
    maximum[2] = (sint16)r_u16(scratch + 0x3A0);
    for (axis = 0; axis < 3; ++axis) lower[axis] = (sint16)r_u16(bounds + axis * 2);
    if (lower[0] >= maximum[0]) return 0;
    if (lower[1] >= maximum[1]) return 0;
    return lower[2] < maximum[2];
}

uint32 sub_8001D054(void)
{
    uint32 status_port, saved_status, mask_port, saved_mask;
    FUNCTION_MARKER(0x8001D054, "SLUS_009.74");
    if (r_u16(0x8002E1D0)) return 0;
    /* TODO Native continuation bridge for the saved IRQ environment */
    moh_draft_missing_call(0x800266F8, 0x8002E208);
    status_port = r_u32(0x8002F260);
    saved_status = r_u16(0x8002E202);
    w_u16(0x8002E1D0, 1);
    w_u16(status_port, (uint16)saved_status);
    mask_port = r_u32(0x8002F264);
    saved_mask = r_u32(0x8002E204);
    w_u32(mask_port, saved_mask);
    sub_8001AD38();
    return 0x8002E1D0;
}

uint32 sub_80024890(uint32 state)
{
    uint32 phase, value;
    FUNCTION_MARKER(0x80024890, "SLUS_009.74");
    phase = r_u8(state + 70);
    if (phase == 3) {
        value = r_u8(state + 228);
        return sub_80025164(state, value);
    }
    if (phase < 4) return phase == 2 ? sub_80025150(state) : 2;
    if (phase != 4) return 4;
    value = r_u8(state + 71);
    return sub_800251A4(state, value);
}

uint32 sub_8002334C(void)
{
    uint32 attempts, command, delay, parameters;
    FUNCTION_MARKER(0x8002334C, "SLUS_009.74");
    sub_80027A18();
    attempts = r_u32(0x8003461C);
    command = r_u8(0x800345D8);
    w_u32(0x8003461C, attempts + 1);
    delay = r_u32(0x80034628 + command * 4);
    w_u32(0x80034618, delay ? delay * 60 : 30);
    command = r_u8(0x800345D8);
    parameters = r_u32(0x800345E0);
    sub_80027584(command, parameters, 0, 1);
    return 0;
}

uint32 sub_8001AA90(uint32 counter, uint32 target, uint32 flags)
{
    uint32 index, mode, table, record;
    FUNCTION_MARKER(0x8001AA90, "SLUS_009.74");
    index = counter & 0xFFFF;
    if (index >= 3) return 0;
    mode = 0x48;
    table = r_u32(0x8002E12C);
    record = table + index * 16;
    w_u16(record + 4, 0);
    w_u16(record + 8, (uint16)target);
    if (index < 2) {
        if (flags & 0x10) mode = 0x49;
        if (!(flags & 1)) mode |= 0x100;
    } else if (!(flags & 1)) mode = 0x248;
    if (flags & 0x1000) mode |= 0x10;
    table = r_u32(0x8002E12C);
    w_u16(table + index * 16 + 4, (uint16)mode);
    return 1;
}

uint32 sub_80024C90(uint32 state)
{
    uint32 phase, value;
    FUNCTION_MARKER(0x80024C90, "SLUS_009.74");
    phase = r_u8(state + 70);
    if (phase == 3) {
        value = r_u8(state + 71);
        return sub_80025184(state, value);
    }
    if (phase < 4) {
        if (phase != 2) return 2;
        value = r_u8(state + 71);
        return sub_80025164(state, value);
    }
    if (phase != 4) return 4;
    if (r_u8(state + 72)) return sub_800251C4(state);
    value = r_u8(state + 71);
    return sub_800251A4(state, value);
}

uint32 sub_80024BBC(uint32 state, uint32 bytes)
{
    uint32 callback, first_count, second_count, cursor;
    FUNCTION_MARKER(0x80024BBC, "SLUS_009.74");
    if (!bytes) return 0;
    if (r_u32(state + 4)) return 0;
    callback = r_u32(0x80034750);
    if (moh_draft_indirect_call(callback, state, bytes)) return 0;
    cursor = (uint32)((sint32)(bytes + 3) >> 2) << 2;
    w_u8(state + 73, 4);
    w_u8(state + 70, 1);
    w_u32(state + 20, 0x80024C90);
    first_count = r_u8(state + 227);
    w_u32(state + 24, 0x80024D38);
    second_count = r_u8(state + 233);
    w_u32(state, cursor);
    w_u8(state + 71, 0);
    cursor += ((first_count + 1) >> 1) << 2;
    w_u32(state + 4, cursor);
    cursor += (second_count * 5 + 3) & 0xFFC;
    w_u32(state + 8, cursor);
    return 1;
}

uint32 sub_8002BFC8(uint32 bank, uint32 sequence)
{
    static const uint32 masks[3] = {2, 8, 0x400};
    static const uint32 cleared[13] = {28, 24, 25, 30, 26, 27, 31, 23, 33, 28, 29, 21, 22};
    uint32 table, offset, state, current, flags, index, first, second, saved, begin, repeated_begin, packed;
    FUNCTION_MARKER(0x8002BFC8, "SLUS_009.74");
    table = 0x800D09F0 + (uint32)(sint32)(sint16)bank * 4;
    offset = (uint32)(sint32)(sint16)sequence * 176;
    state = r_u32(table) + offset;
    flags = r_u32(state + 152);
    w_u32(state + 152, flags & ~1u);
    for (index = 0; index < 3; ++index) {
        current = r_u32(table) + offset;
        flags = r_u32(current + 152);
        w_u32(current + 152, flags & ~masks[index]);
    }
    current = r_u32(table) + offset;
    flags = r_u32(current + 152);
    w_u32(current + 152, flags | 4);
    packed = (uint32)(sint32)(sint16)(bank | (sequence << 8));
    moh_draft_missing_call(0x8002D348, packed);
    sub_8002B758();
    first = r_u32(state + 132);
    second = r_u32(state + 140);
    saved = r_u16(state + 86);
    begin = r_u32(state + 4);
    repeated_begin = r_u32(state + 4);
    w_u8(state + 20, 0);
    w_u32(state + 136, 0);
    for (index = 0; index < 13; ++index) w_u8(state + cleared[index], 0);
    w_u32(state + 144, first);
    w_u32(state + 148, second);
    w_u16(state + 84, (uint16)saved);
    w_u32(state, begin);
    w_u32(state + 8, repeated_begin);
    for (index = 0; index < 16; ++index) {
        w_u8(state + 55 + index, (uint8)index);
        w_u8(state + 39 + index, 64);
        w_u16(state + 96 + index * 2, 127);
    }
    w_u16(state + 92, 127);
    w_u16(state + 94, 127);
    return 127;
}

uint32 sub_800145DC(uint32 ordering_tag, uint32 first, uint32 end)
{
    uint32 header, next, stop, result, old_tag, following, following_header;
    FUNCTION_MARKER(0x800145DC, "SLUS_009.74");
    header = r_u32(first);
    stop = end & 0xFFFFFF;
    for (;;) {
        next = header & 0xFFFFFF;
        result = header >> 24;
        if (result >= 2) break;
        first = next | 0x80000000;
        if (next == stop) return result;
        header = r_u32(first);
    }
    old_tag = r_u32(ordering_tag);
    w_u32(ordering_tag, first & 0xFFFFFF);
    while (next != stop) {
        following = next | 0x80000000;
        following_header = r_u32(following);
        next = following_header & 0xFFFFFF;
        result = following_header >> 24;
        if (result >= 2) {
            result = following & 0xFFFFFF;
            w_u32(first, (header & 0xFF000000) | result);
            header = following_header;
            first = following;
        }
    }
    w_u32(first, (header & 0xFF000000) | (old_tag & 0xFFFFFF));
    return result;
}

uint32 sub_8001449C(uint32 quaternion, uint32 matrix)
{
    uint32 packed_xy, packed_zw, x, y, z, w, doubled_x, doubled_y, doubled_w, packed, first[3], second[3], diagonal, row;
    FUNCTION_MARKER(0x8001449C, "SLUS_009.74");
    packed_xy = r_u32(quaternion);
    packed_zw = r_u32(quaternion + 4);
    x = (uint32)(sint32)(sint16)packed_xy;
    y = (uint32)((sint32)packed_xy >> 16);
    z = (uint32)(sint32)(sint16)packed_zw;
    w = (uint32)((sint32)packed_zw >> 16);
    xport_gte_write_control(0, (w << 16) | (x & 0xFFFF));
    packed = (y << 16) | (z & 0xFFFF);
    xport_gte_write_control(1, packed);
    xport_gte_write_control(3, packed);
    xport_gte_write_control(2, (w << 16) | ((0 - z) & 0xFFFF));
    xport_gte_write_control(4, 0 - x);
    doubled_x = x + x;
    doubled_w = w + w;
    xport_gte_write_data(0, (doubled_w << 16) | (doubled_x & 0xFFFF));
    xport_gte_write_data(1, 0);
    doubled_y = y + y;
    xport_gte_execute(0x486012);
    for (row = 0; row < 3; ++row) first[row] = xport_gte_read_data(25 + row);
    xport_gte_write_data(0, doubled_y & 0xFFFF);
    xport_gte_write_data(1, doubled_w);
    xport_gte_execute(0x486012);
    w_u16(matrix, (uint16)(first[0] - 4096));
    w_u16(matrix + 6, (uint16)first[1]);
    w_u16(matrix + 12, (uint16)first[2]);
    for (row = 0; row < 3; ++row) second[row] = xport_gte_read_data(25 + row);
    xport_gte_write_data(0, (0 - doubled_y) << 16);
    xport_gte_write_data(1, doubled_x);
    xport_gte_execute(0x486012);
    w_u16(matrix + 2, (uint16)second[0]);
    w_u16(matrix + 8, (uint16)(second[1] - 4096));
    w_u16(matrix + 14, (uint16)second[2]);
    diagonal = ((uint32)((sint32)(z * z) >> 12) << 1) + ((uint32)((sint32)(w * w) >> 12) << 1) - 4096;
    first[0] = xport_gte_read_data(25);
    first[1] = xport_gte_read_data(26);
    w_u16(matrix + 4, (uint16)first[0]);
    w_u16(matrix + 10, (uint16)first[1]);
    w_u16(matrix + 16, (uint16)diagonal);
    return matrix;
}

uint32 sub_80023950(uint32 event, uint32 response)
{
    uint32 current, previous, callback, result;
    FUNCTION_MARKER(0x80023950, "SLUS_009.74");
    event &= 255;
    if (event == 2 && r_u8(0x800345D8) == 14) {
        previous = r_u8(0x80034601);
        current = r_u8(0x800345D9);
        if ((previous ^ current) & 0x80) {
            w_u32(0x800345F8, 15);
            w_u32(0x800345F4, 2);
            w_u32(0x80034614, 3);
        }
        w_u8(0x80034601, (uint8)current);
    }
    if (event != 5) return 5;
    if (!(r_u8(0x800345EC) & 0x10)) {
        w_u32(0x800345F4, 1);
        w_u32(0x800345F8, 11);
        return 11;
    }
    callback = r_u32(0x800D0A74);
    w_u32(0x800345F4, 2);
    w_u32(0x800345F8, 12);
    if (!callback) return 12;
    result = r_u32(0x800345D4);
    if (!result) return result;
    callback = r_u32(0x800D0A74);
    return moh_draft_indirect_call(callback, 5, response);
}

uint32 sub_8001B2E0(uint32 port, uint32 property, uint32 index, uint32 entry_callback)
{
    uint32 object, count, table;
    FUNCTION_MARKER(0x8001B2E0, "SLUS_009.74");
    object = moh_draft_indirect_call(entry_callback, port);
    switch (property) {
    case 1: return r_u8(object + 232);
    case 2: return r_u16(object + 230);
    case 3: return r_u8(object + 228);
    case 4:
        if ((sint32)index < 0) return r_u8(object + 227);
        count = r_u8(object + 227);
        if ((sint32)index >= (sint32)count) return 0;
        table = r_u32(object);
        return r_u16(table + index * 2);
    case 100: return r_u32(object + 76);
    default: return 0;
    }
}

uint32 sub_8001B5C0(uint32 port, uint32 buffer, uint32 entry_callback)
{
    uint32 object, callback;
    FUNCTION_MARKER(0x8001B5C0, "SLUS_009.74");
    object = moh_draft_indirect_call(entry_callback, port);
    callback = r_u32(0x80034750);
    if (moh_draft_indirect_call(callback, object)) return 0;
    w_u8(object + 70, 1);
    w_u32(object + 20, 0x8001B634);
    w_u32(object + 32, buffer);
    w_u32(object + 24, 0x8001B65C);
    return 1;
}

uint32 sub_8001B890(uint32 port, uint32 buffer, uint32 length, uint32 entry_callback)
{
    uint32 object;
    FUNCTION_MARKER(0x8001B890, "SLUS_009.74");
    object = moh_draft_indirect_call(entry_callback, port);
    w_u32(object + 40, buffer);
    w_u8(object + 52, (uint8)length);
    return object;
}

uint32 sub_8001B2D8(uint32 port, uint32 property, uint32 index)
{
    uint32 callback;
    FUNCTION_MARKER(0x8001B2D8, "SLUS_009.74");
    callback = r_u32(0x80034748);
    return sub_8001B2E0(port, property, index, callback);
}

uint32 sub_8001B3D8(uint32 port)
{
    uint32 callback;
    FUNCTION_MARKER(0x8001B3D8, "SLUS_009.74");
    callback = r_u32(0x80034748);
    return sub_8001B3E0(port, callback);
}

uint32 sub_8001B5B8(uint32 port, uint32 buffer)
{
    uint32 callback;
    FUNCTION_MARKER(0x8001B5B8, "SLUS_009.74");
    callback = r_u32(0x80034748);
    return sub_8001B5C0(port, buffer, callback);
}

uint32 sub_8001B728(uint32 port, uint32 value, uint32 mode)
{
    uint32 callback;
    FUNCTION_MARKER(0x8001B728, "SLUS_009.74");
    callback = r_u32(0x80034748);
    return sub_8001B730(port, value, mode, callback);
}

uint32 sub_8001B888(uint32 port, uint32 buffer, uint32 length)
{
    uint32 callback;
    FUNCTION_MARKER(0x8001B888, "SLUS_009.74");
    callback = r_u32(0x80034748);
    return sub_8001B890(port, buffer, length, callback);
}

uint32 sub_80017900(uint32 packet, uint32 environment)
{
    uint32 x, y, width, height, first, second, third, result, red, green, blue;
    sint32 limit;
    FUNCTION_MARKER(0x80017900, "SLUS_009.74");
    x = (uint32)(sint32)(sint16)r_u16(environment);
    y = (uint32)(sint32)(sint16)r_u16(environment + 2);
    result = sub_80017D88(x, y);
    w_u32(packet + 4, result);
    width = r_u16(environment + 4);
    x = r_u16(environment);
    y = r_u16(environment + 2);
    x = (uint32)(sint32)(sint16)(width + x - 1);
    height = r_u16(environment + 6);
    y = (uint32)(sint32)(sint16)(y + height - 1);
    result = sub_80017E20(x, y);
    w_u32(packet + 8, result);
    x = (uint32)(sint32)(sint16)r_u16(environment + 8);
    y = (uint32)(sint32)(sint16)r_u16(environment + 10);
    result = sub_80017EB8(x, y);
    w_u32(packet + 12, result);
    first = r_u8(environment + 23);
    second = r_u8(environment + 22);
    third = r_u16(environment + 20);
    result = sub_80017D68(first, second, third);
    w_u32(packet + 16, result);
    result = sub_80017ED4(environment + 12);
    w_u32(packet + 20, result);
    w_u32(packet + 24, 0xE6000000);
    if (!r_u8(environment + 24)) {
        w_u8(packet + 3, 6);
        return 6;
    }
    x = r_u16(environment);
    y = r_u16(environment + 2);
    width = r_u16(environment + 4);
    height = r_u16(environment + 6);
    if ((sint16)width < 0) width = 0;
    else {
        limit = (sint16)r_u16(0x8002DF1C) - 1;
        first = r_u16(0x8002DF1C) - 1;
        if (limit < (sint16)width) width = first;
    }
    if ((sint16)height < 0) height = 0;
    else {
        limit = (sint16)r_u16(0x8002DF1E) - 1;
        first = r_u16(0x8002DF1E) - 1;
        if (limit < (sint16)height) height = first;
    }
    x = (uint16)(x - r_u16(environment + 8));
    y = (uint16)(y - r_u16(environment + 10));
    blue = r_u8(environment + 27);
    green = r_u8(environment + 26);
    red = r_u8(environment + 25);
    w_u32(packet + 28, 0x60000000 | (blue << 16) | (green << 8) | red);
    w_u32(packet + 32, x | (y << 16));
    w_u32(packet + 36, (width & 0xFFFF) | ((height & 0xFFFF) << 16));
    w_u8(packet + 3, 9);
    return 9;
}

uint32 sub_800184B8(uint32 rectangle, uint32 destination)
{
    uint32 width, height, maximum, product, adjusted, blocks, remainder, port, value;
    sint32 dimension, limit, words;
    FUNCTION_MARKER(0x800184B8, "SLUS_009.74");
    sub_80018FB8();
    dimension = (sint16)r_u16(rectangle + 4);
    width = r_u16(rectangle + 4);
    if (dimension < 0) width = 0;
    else {
        limit = (sint16)r_u16(0x8002DF1C);
        maximum = r_u16(0x8002DF1C);
        if (limit < dimension) width = maximum;
    }
    dimension = (sint16)r_u16(rectangle + 6);
    height = r_u16(rectangle + 6);
    w_u16(rectangle + 4, (uint16)width);
    if (dimension < 0) height = 0;
    else {
        limit = (sint16)r_u16(0x8002DF1E);
        maximum = r_u16(0x8002DF1E);
        if (limit < dimension) height = maximum;
    }
    product = (uint32)((sint16)r_u16(rectangle + 4) * (sint32)(sint16)height);
    w_u16(rectangle + 6, (uint16)height);
    adjusted = product + 1;
    adjusted += adjusted >> 31;
    words = (sint32)adjusted >> 1;
    blocks = (uint32)((sint32)adjusted >> 5);
    if (words <= 0) return (uint32)-1;
    remainder = (uint32)words - (blocks << 4);
    port = r_u32(0x8002E020);
    while (!(r_u32(port) & 0x04000000)) {
        if (sub_80018FEC()) return (uint32)-1;
        port = r_u32(0x8002E020);
    }
    port = r_u32(0x8002E020);
    w_u32(port, 0x04000000);
    port = r_u32(0x8002E01C);
    w_u32(port, 0x01000000);
    port = r_u32(0x8002E01C);
    w_u32(port, 0xC0000000);
    port = r_u32(0x8002E01C);
    value = r_u32(rectangle);
    w_u32(port, value);
    port = r_u32(0x8002E01C);
    value = r_u32(rectangle + 4);
    w_u32(port, value);
    port = r_u32(0x8002E020);
    while (!(r_u32(port) & 0x08000000)) {
        if (sub_80018FEC()) return (uint32)-1;
        port = r_u32(0x8002E020);
    }
    while (remainder) {
        port = r_u32(0x8002E01C);
        value = r_u32(port);
        --remainder;
        w_u32(destination, value);
        destination += 4;
    }
    if (blocks) {
        port = r_u32(0x8002E020);
        w_u32(port, 0x04000003);
        port = r_u32(0x8002E024);
        w_u32(port, destination);
        port = r_u32(0x8002E028);
        w_u32(port, (blocks << 16) | 16);
        port = r_u32(0x8002E02C);
        w_u32(port, 0x01000200);
    }
    return 0;
}

uint32 sub_80024914(uint32 state)
{
    uint32 phase, response, comparison, value, pointer, other, size, limit, progress, callback;
    FUNCTION_MARKER(0x80024914, "SLUS_009.74");
    phase = r_u8(state + 70);
    if (phase == 2) {
        response = r_u32(state + 60);
        if (r_u8(response + 7)) return 0;
        comparison = 0xFFFF;
        value = r_u8(state + 227);
        other = r_u8(response + 3);
        if (value == other) {
            value = r_u8(state + 228);
            other = r_u8(response + 4);
            if (value == other) {
                value = r_u8(state + 233);
                other = r_u8(response + 5);
                if (value == other) {
                    value = r_u8(state + 234);
                    other = r_u8(response + 6);
                    if (value == other) comparison = 0;
                }
            }
        }
        w_u16(state + 238, (uint16)comparison);
        pointer = r_u32(state + 60);
        response = r_u32(state + 60);
        value = r_u8(pointer + 3);
        w_u8(state + 227, (uint8)value);
        value = r_u8(response + 4);
        response = r_u32(state + 60);
        w_u16(state + 230, 0);
        w_u8(state + 228, (uint8)value);
        value = r_u8(response + 5);
        response = r_u32(state + 60);
        w_u8(state + 233, (uint8)value);
        value = r_u8(response + 6);
        comparison = r_u16(state + 238);
        w_u16(state + 236, 0);
        w_u8(state + 234, (uint8)value);
        if (comparison) return 0;
        w_u8(state + 235, 0);
        return 1;
    }
    if (phase == 3) {
        response = r_u32(state + 60);
        if (r_u8(response + 2)) return 0;
        if (r_u8(response + 3)) return 0;
        value = r_u8(response + 4);
        other = r_u8(response + 5);
        value = other + (value << 8);
        comparison = r_u16(state + 238);
        w_u16(state + 230, (uint16)value);
        if (comparison != (value & 0xFFFF)) {
            w_u16(state + 238, (uint16)value);
            return 0;
        }
        w_u16(state + 238, 0xFFFF);
        w_u8(state + 235, 0);
        w_u8(state + 71, 0);
        return 1;
    }
    if (phase != 4) return 1;
    response = r_u32(state + 60);
    if (r_u8(response + 2)) return 0;
    if (r_u8(response + 3)) return 0;
    size = r_u16(state + 236);
    value = r_u8(response + 4);
    progress = r_u8(state + 71);
    size += 8 + ((value + 3) & 0x1FC);
    limit = r_u8(state + 234);
    ++progress;
    w_u8(state + 71, (uint8)progress);
    w_u16(state + 236, (uint16)size);
    if ((progress & 255) < limit) return 0;
    if ((sint32)sub_80024B84(state) >= 129) {
        callback = r_u32(0x80034738);
        moh_draft_indirect_call(callback, state);
        w_u8(state + 70, 254);
        w_u8(state + 73, 2);
        return 0;
    }
    size = r_u16(state + 236);
    comparison = r_u16(state + 238);
    if (comparison != size) {
        w_u16(state + 238, (uint16)size);
        w_u8(state + 71, 0);
        w_u16(state + 236, 0);
        return 0;
    }
    w_u16(state + 238, 0);
    w_u8(state + 235, 0);
    w_u8(state + 70, 255);
    sub_80024BBC(state, state + 99);
    w_u8(state + 70, 2);
    return 0;
}

uint32 sub_80024D38(uint32 state)
{
    uint32 phase, response, index, table, record, value, other, comparison, limit, source, destination, remaining, bytes;
    static const uint32 offsets[5] = {4, 5, 6, 7, 5};
    FUNCTION_MARKER(0x80024D38, "SLUS_009.74");
    phase = r_u8(state + 70);
    if (phase == 2) {
        response = r_u32(state + 60);
        if (r_u8(response + 2)) return 0;
        if (r_u8(response + 3)) return 0;
        index = r_u8(state + 71);
        table = r_u32(state);
        record = table + index * 2;
        value = r_u8(response + 4);
        other = r_u8(response + 5);
        w_u16(record, (uint16)(other + (value << 8)));
        index = r_u8(state + 71);
        table = r_u32(state);
        value = r_u16(table + index * 2);
        comparison = r_u16(state + 238);
        if (comparison != value) {
            w_u16(state + 238, (uint16)value);
            return 0;
        }
        index = r_u8(state + 71);
        limit = r_u8(state + 227);
        w_u16(state + 238, 0);
        w_u8(state + 235, 0);
        ++index;
        w_u8(state + 71, (uint8)index);
        if ((index & 255) < limit) return 0;
        w_u8(state + 71, 0);
        return 1;
    }
    if (phase == 3) {
        response = r_u32(state + 60);
        if (r_u8(response + 2)) return 0;
        if (r_u8(response + 3)) return 0;
        index = r_u8(state + 71);
        table = r_u32(state + 4);
        record = table + index * 5;
        comparison = 0xFFFF;
        value = r_u8(record);
        other = r_u8(response + 4);
        if (value == other) {
            value = r_u8(response + 5) & 127;
            other = r_u8(record + 1);
            if (value == other) {
                value = r_u8(record + 2);
                other = r_u8(response + 6);
                if (value == other) {
                    value = r_u8(record + 3);
                    other = r_u8(response + 7);
                    if (value == other) {
                        value = r_u8(response + 5) >> 7;
                        other = r_u8(record + 4);
                        if (value == other) comparison = 0;
                    }
                }
            }
        }
        w_u16(state + 238, (uint16)comparison);
        for (index = 0; index < 5; ++index) {
            response = r_u32(state + 60);
            value = r_u8(response + offsets[index]);
            if (index == 1) value &= 127;
            else if (index == 4) value >>= 7;
            w_u8(record + index, (uint8)value);
        }
        if (r_u16(state + 238)) return 0;
        index = r_u8(state + 71);
        limit = r_u8(state + 233);
        w_u8(state + 235, 0);
        ++index;
        w_u8(state + 71, (uint8)index);
        if ((index & 255) < limit) return 0;
        w_u8(state + 71, 0);
        w_u8(state + 72, 0);
        return 1;
    }
    if (phase != 4) return 1;
    response = r_u32(state + 60);
    if (r_u8(response + 2)) {
        w_u8(state + 72, 0);
        return 0;
    }
    index = r_u8(state + 71);
    table = r_u32(state + 8);
    remaining = r_u8(state + 72);
    record = table + index * 8;
    if (!remaining) {
        value = r_u8(response + 4);
        w_u8(state + 72, (uint8)value);
        w_u8(record, (uint8)value);
        response = r_u32(state + 60);
        index = r_u8(state + 71);
        source = response + 5;
        if (!index) {
            value = r_u8(state + 234);
            table = r_u32(state + 8);
            value <<= 3;
        } else {
            value = r_u8(record - 8);
            table = r_u32(record - 4);
            value = (value + 3) & 0x1FC;
        }
        destination = table + value;
        w_u32(record + 4, destination);
        w_u32(0x800D0A8C, destination);
        bytes = 3;
    } else {
        source = response + 3;
        bytes = 5;
    }
    while (bytes) {
        if (!r_u8(state + 72)) break;
        destination = r_u32(0x800D0A8C);
        if (destination >= state + 227) {
            w_u8(state + 71, 0);
            w_u8(state + 72, 0);
            return 0;
        }
        value = r_u8(destination);
        other = r_u8(source);
        if (value != other) w_u16(state + 238, 0xFFFF);
        destination = r_u32(0x800D0A8C);
        value = r_u8(source);
        ++source;
        w_u8(destination, (uint8)value);
        w_u32(0x800D0A8C, destination + 1);
        remaining = r_u8(state + 72);
        --bytes;
        w_u8(state + 72, (uint8)(remaining - 1));
    }
    if (!bytes && r_u8(state + 72)) return 0;
    if (r_u16(state + 238)) {
        w_u16(state + 238, 0);
        w_u8(state + 72, 0);
        return 0;
    }
    index = r_u8(state + 71);
    limit = r_u8(state + 234);
    ++index;
    w_u8(state + 71, (uint8)index);
    if ((index & 255) < limit) w_u8(state + 72, 0);
    else {
        w_u8(state + 73, 6);
        w_u8(state + 70, 254);
    }
    w_u8(state + 235, 0);
    return 0;
}

static uint32 moh_text_window_append(uint32 window, uint32 character)
{
    uint32 index, destination, capacity, overflow;
    index = r_u32(window + 40);
    destination = r_u32(window + 36);
    w_u8(destination + index, (uint8)character);
    capacity = r_u32(window + 28);
    ++index;
    overflow = (sint32)capacity < (sint32)index;
    w_u32(window + 40, index);
    return overflow;
}

uint32 sub_8001F7AC(uint32 selector_or_format, uint32 arguments, uint32 initial_length)
{
    uint8 temporary[512];
    uint32 selector, format, window, used, capacity, character, width, zero_padding, source, length, value;
    uint32 negative, table, destination, index, overflow;
    FUNCTION_MARKER(0x8001F7AC, "SLUS_009.74");
    selector = selector_or_format;
    if ((sint32)selector >= 0 && (sint32)selector < (sint32)r_u32(0x8002F54C)) {
        format = r_u32(arguments);
        arguments += 4;
    } else {
        selector = r_u32(0x8002F550);
        format = selector_or_format;
        if (!r_u32(0x8002F3F0 + selector * 48)) return (uint32)-1;
    }
    window = 0x8002F3CC + selector * 48;
    used = r_u32(window + 40);
    capacity = r_u32(window + 28);
    if ((sint32)capacity < (sint32)used) return (uint32)-1;
    length = initial_length;
    character = (uint32)(sint32)(sint8)r_u8(format);
    while (character) {
        if (character == '%') {
            ++format;
            character = (uint32)(sint32)(sint8)r_u8(format);
            width = 0;
        } else {
            if (moh_text_window_append(window, character)) return (uint32)-1;
            ++format;
            character = (uint32)(sint32)(sint8)r_u8(format);
            continue;
        }
        if (character == '%') {
            if (moh_text_window_append(window, character)) return (uint32)-1;
        } else {
            zero_padding = character == '0';
            while (character - '0' < 10) {
                width = width * 10 + character - '0';
                ++format;
                character = (uint32)(sint32)(sint8)r_u8(format);
            }
            if ((sint32)width <= 0) width = 1;
            switch (character) {
            case 'd':
                value = r_u32(arguments);
                arguments += 4;
                negative = (sint32)value < 0;
                if (negative) value = 0 - value;
                /* TODO Addressable local views retain lifetime through conversion and output */
                source = moh_draft_local_address(temporary, sizeof(temporary)) + sizeof(temporary);
                length = 0;
                do {
                    --source;
                    ++length;
                    w_u8(source, (uint8)(value % 10 + '0'));
                    value /= 10;
                } while (value);
                if (negative) {
                    --source;
                    w_u8(source, '-');
                    ++length;
                }
                break;
            case 'X':
            case 'x':
                value = r_u32(arguments);
                arguments += 4;
                source = moh_draft_local_address(temporary, sizeof(temporary)) + sizeof(temporary);
                length = 0;
                do {
                    --source;
                    index = value & 15;
                    table = r_u32(0x8002FF58);
                    value >>= 4;
                    character = r_u8(table + index);
                    ++length;
                    w_u8(source, (uint8)character);
                } while (value);
                if (zero_padding) while ((sint32)length < (sint32)width) {
                    --source;
                    ++length;
                    w_u8(source, '0');
                }
                break;
            case 'c':
                value = r_u8(arguments);
                arguments += 4;
                source = moh_draft_local_address(temporary, sizeof(temporary)) + sizeof(temporary) - 1;
                w_u8(source, (uint8)value);
                length = 1;
                break;
            case 's':
                source = r_u32(arguments);
                arguments += 4;
                length = sub_8001AC88(source);
                break;
            default:
                /* TODO The original unsupported conversion reads its addressable cursor local */
                source = moh_draft_local_address(&arguments, sizeof(arguments));
                break;
            }
            while ((sint32)length < (sint32)width) {
                if (moh_text_window_append(window, ' ')) return (uint32)-1;
                --width;
            }
            --length;
            while (length != (uint32)-1) {
                index = r_u32(window + 40);
                destination = r_u32(window + 36);
                character = r_u8(source);
                ++source;
                w_u8(destination + index, (uint8)character);
                capacity = r_u32(window + 28);
                ++index;
                overflow = (sint32)capacity < (sint32)index;
                w_u32(window + 40, index);
                if (overflow) return (uint32)-1;
                --length;
            }
        }
        ++format;
        character = (uint32)(sint32)(sint8)r_u8(format);
    }
    destination = r_u32(window + 36);
    index = r_u32(window + 40);
    w_u8(destination + index, 0);
    return r_u32(window + 40);
}

uint32 sub_8001F79C(uint32 selector_or_format, uint32 arguments)
{
    uint32 initial_length;
    FUNCTION_MARKER(0x8001F79C, "SLUS_009.74");
    /* TODO Native caller packs the original variadic words into this semantic stream */
    initial_length = r_u32(arguments);
    return sub_8001F7AC(selector_or_format, arguments, initial_length);
}

uint32 sub_80014128(uint32 node, uint32 matrix, uint32 output, uint32 render)
{
    uint32 local_words[8];
    uint32 result;
    uint32 id, quaternions, local, axis, word, magnitude, high[3], low[3], product[3], translation[3];
    uint32 table, vertices, count, destination, index, point, child, positions, x, y, z;
    FUNCTION_MARKER(0x80014128u, "SLUS_009.74");
    id = r_u16(node); word = r_u32(0x8009B4F4u); quaternions = r_u32(render + 0x88u);
    if (id > word) return 1;
    if (!quaternions) return 0;
    /* TODO Native address bridge for the original addressable local matrix */
    local = moh_draft_local_address(local_words, sizeof(local_words));
    sub_8001449C(quaternions + id * 8u, local);
    for (axis = 0u; axis < 3u; ++axis) {
        word = (uint32)(sint32)(sint16)r_u16(node + 2u + axis * 2u) << 19u;
        if ((sint32)word < 0) {
            magnitude = 0u - word;
            high[axis] = 0u - (uint32)((sint32)magnitude >> 15);
            low[axis] = 0u - (magnitude & 0x7FFFu);
        } else { high[axis] = (uint32)((sint32)word >> 15); low[axis] = word & 0x7FFFu; }
    }
    for (axis = 0u; axis < 3u; ++axis) xport_gte_write_data(9u + axis, high[axis]);
    for (axis = 0u; axis < 5u; ++axis) xport_gte_write_control(axis, r_u32(matrix + axis * 4u));
    xport_gte_execute(0x41E012u);
    for (axis = 0u; axis < 3u; ++axis) product[axis] = xport_gte_read_data(25u + axis);
    for (axis = 0u; axis < 3u; ++axis) xport_gte_write_data(9u + axis, low[axis]);
    xport_gte_execute(0x49E012u);
    for (axis = 0u; axis < 3u; ++axis) translation[axis] = r_u32(matrix + 20u + axis * 4u);
    for (axis = 0u; axis < 3u; ++axis)
        translation[axis] += xport_gte_read_data(25u + axis) + (product[axis] << 3u);
    for (axis = 0u; axis < 3u; ++axis)
        xport_gte_write_control(5u + axis, (uint32)((sint32)((translation[axis] << 1u) + 0x80000u) >> 20));
    table = r_u32(render + 0x80u);
    for (axis = 0u; axis < 4u; ++axis) xport_gte_write_control(axis, r_u32(local + axis * 4u));
    table = r_u32(table); xport_gte_write_control(4u, r_u32(local + 16u));
    word = r_u32(0x8009B4F8u); w_u32(local + 20u, translation[0]);
    if (id > word) return 1;
    result = 0;
    w_u32(local + 24u, translation[1]); w_u32(local + 28u, translation[2]);
    vertices = r_u32(table + id * 8u); count = r_u32(table + id * 8u + 4u);
    word = r_u32(0x1F8003FCu); destination = r_u32(0x8009B4FCu);
    w_u32(0x1F8003FCu, word + count); w_u32(output + id * 4u, destination);
    w_u32(0x8009B4FCu, destination + count * 8u);
    index = 0u;
    while (index != count) {
        point = vertices + index * 8u;
        xport_gte_write_data(0u, r_u32(point)); xport_gte_write_data(1u, r_u32(point + 4u));
        xport_gte_execute(0x480012u);
        point = destination + index * 8u; ++index;
        x = xport_gte_read_data(25u); y = xport_gte_read_data(26u); z = xport_gte_read_data(27u);
        w_u16(point, (uint16)x); w_u16(point + 2u, (uint16)y); w_u16(point + 4u, (uint16)z);
    }
    child = r_u32(node + 16u); if (child) result = sub_80014128(child, matrix, output, render);
    child = r_u32(node + 12u); if (child) result = sub_80014128(child, local, output, render);
    positions = r_u32(render + 0x94u) + id * 8u;
    for (axis = 0u; axis < 3u; ++axis)
        w_u16(positions + axis * 2u, (uint16)((sint32)((translation[axis] << 1u) + 0x80000u) >> 20));
    return result;
}

uint32 sub_800125C4(uint32 triangle, uint32 packet, uint32 first_depth, uint32 second_depth, uint32 third_depth, uint32 scratch)
{
    uint32 d0, d1, d2, high, low, average, index, screen[3], uv0, uv1, uv2, colors[3], tag, header;
    FUNCTION_MARKER(0x800125C4u, "SLUS_009.74");
    d0 = first_depth; d1 = second_depth; d2 = third_depth;
    high = d0 > d1 ? d0 : d1; low = d0 > d1 ? d1 : d0;
    if (d2 > high) high = d2; if (d2 < low) low = d2;
    average = (uint32)((sint32)(high * 3u + low) >> 4);
    if ((sint32)average <= 0) return packet;
    for (index = 0u; index < 3u; ++index) screen[index] = r_u32(triangle + 24u + index * 4u);
    uv0 = r_u32(triangle + 36u); uv1 = r_u32(triangle + 40u); uv2 = r_u16(triangle + 44u);
    for (index = 0u; index < 3u; ++index) colors[index] = r_u32(triangle + 48u + index * 4u);
    for (index = 0u; index < 3u; ++index) w_u32(packet + 8u + index * 12u, screen[index]);
    w_u32(packet + 12u, uv0); w_u32(packet + 24u, uv1); w_u32(packet + 36u, uv2);
    for (index = 0u; index < 3u; ++index) w_u32(packet + 4u + index * 12u, colors[index]);
    tag = r_u32(scratch + 4);
    w_u32(scratch + 12, r_u32(scratch + 12) + 1u);
    tag += average & ~3u; header = r_u32(tag);
    w_u32(packet, header | 0x09000000u); w_u32(tag, packet & 0xFFFFFFu);
    return packet + 40u;
}

uint32 sub_80012D5C(uint32 next_triangle, uint32 vertices, uint32 scratch)
{
    uint32 packed, ids[3], points[3], depths[3], screens[3], vertex_xy, vertex_z[3], flags;
    uint32 minimum, maximum, near, average, split, width, height, index, clipping[3];
    sint32 area, x, y;
    FUNCTION_MARKER(0x80012D5C, "SLUS_009.74");
    packed = r_u32(next_triangle);
    ids[0] = packed & 0xFFFF;
    w_u16(scratch + 0x3C0, (uint16)ids[0]);
    ids[1] = packed >> 16;
    points[0] = vertices + ids[0] * 8;
    w_u16(scratch + 0x3C2, (uint16)ids[1]);
    ids[2] = r_u16(next_triangle + 4);
    points[1] = vertices + ids[1] * 8;
    flags = xport_gte_read_data(31);
    w_u16(scratch + 0x3C4, (uint16)ids[2]);
    xport_gte_execute(0x1400006);
    points[2] = vertices + ids[2] * 8;
    vertex_z[0] = r_u32(points[0] + 4);
    vertex_xy = r_u32(points[0]);
    area = (sint32)xport_gte_read_data(24);
    xport_gte_write_data(0, vertex_xy);
    xport_gte_write_data(1, vertex_z[0]);
    vertex_z[1] = r_u32(points[1] + 4);
    depths[0] = xport_gte_read_data(17);
    screens[0] = xport_gte_read_data(12);
    w_u16(scratch + 0x1A, (uint16)depths[0]);
    w_u32(scratch + 0x2C, screens[0]);
    vertex_xy = r_u32(points[1]);
    vertex_z[2] = r_u32(points[2] + 4);
    xport_gte_write_data(2, vertex_xy);
    xport_gte_write_data(3, vertex_z[1]);
    depths[1] = xport_gte_read_data(18);
    screens[1] = xport_gte_read_data(13);
    vertex_xy = r_u32(points[2]);
    w_u16(scratch + 0x22, (uint16)depths[1]);
    w_u32(scratch + 0x30, screens[1]);
    xport_gte_write_data(4, vertex_xy);
    xport_gte_write_data(5, vertex_z[2]);
    if ((flags & 0x80000000) && (flags & 0x7FF98000)) return 0;
    depths[2] = xport_gte_read_data(19);
    if (area <= 0) return 0;
    screens[2] = xport_gte_read_data(14);
    minimum = depths[0] < depths[1] ? depths[0] : depths[1];
    maximum = depths[0] < depths[1] ? depths[1] : depths[0];
    if (minimum >= depths[2]) minimum = depths[2];
    near = r_u32(scratch + 0x3A4);
    if (maximum < depths[2]) maximum = depths[2];
    average = (maximum * 3 + minimum) >> 4;
    w_u16(scratch + 0x2A, (uint16)depths[2]);
    w_u32(scratch + 0x34, screens[2]);
    w_u32(scratch + 0x3A8, average);
    split = 0;
    if (near) {
        w_u16(scratch + 0x42, (uint16)average);
        near <<= 2;
        for (index = 0; index < 3; ++index) if (depths[index] < near) split |= 1 << index;
    }
    if (maximum < 10 && average < 16) return 0;
    width = r_u16(scratch + 0x3CC);
    if ((average >> 2) >= 512) return 0;
    height = r_u16(scratch + 0x3CE);
    for (index = 0; index < 3; ++index) {
        x = (sint16)screens[index];
        y = (sint16)(screens[index] >> 16);
        clipping[index] = (x < 0 ? 1 : (uint32)x >= width ? 2 : 0)
            | (y < 0 ? 4 : (uint32)y >= height ? 8 : 0);
    }
    if (clipping[0] & clipping[1] & clipping[2]) return 0;
    return split ? split << 1 : 1;
}

typedef struct MOH_PROJECT_SUBDIVISION_VERTEX {
    sint16 x, y, z;
    uint16 depth, uv;
    uint32 screen, color, flags;
} MOH_PROJECT_SUBDIVISION_VERTEX;

static MOH_PROJECT_SUBDIVISION_VERTEX moh_project_subdivision_midpoint(MOH_PROJECT_SUBDIVISION_VERTEX first, MOH_PROJECT_SUBDIVISION_VERTEX second)
{
    MOH_PROJECT_SUBDIVISION_VERTEX result;
    PsxGteSnapshot snapshot;
    uint32 axis, value;
    result.x = (sint16)(((sint32)first.x + second.x) >> 1);
    result.y = (sint16)(((sint32)first.y + second.y) >> 1);
    result.z = (sint16)(((sint32)first.z + second.z) >> 1);
    xport_gte_write_data(0u, (uint16)result.x | ((uint32)(uint16)result.y << 16u));
    xport_gte_write_data(1u, (uint32)(sint32)result.z);
    xport_gte_execute(0x180001u);
    psx_gte_snapshot(&snapshot); result.flags = (uint32)snapshot.flag;
    result.screen = xport_gte_read_data(14u); result.depth = (uint16)xport_gte_read_data(19u);
    result.uv = (uint16)((((first.uv & 255u) + (second.uv & 255u)) >> 1u) |
        (((((uint32)first.uv >> 8u) + ((uint32)second.uv >> 8u)) >> 1u) << 8u));
    result.color = 0u;
    for (axis = 0u; axis < 3u; ++axis) {
        value = (((first.color >> (axis * 8u)) & 255u) + ((second.color >> (axis * 8u)) & 255u)) >> 1u;
        result.color |= value << (axis * 8u);
    }
    return result;
}

static void moh_project_subdivision_record(uint32 record, const MOH_PROJECT_SUBDIVISION_VERTEX vertices[3], uint16 clut, uint16 page, uint8 code)
{
    uint32 index;
    for (index = 0u; index < 3u; ++index) {
        w_u16(record + index * 8u, (uint16)vertices[index].x);
        w_u16(record + index * 8u + 2u, (uint16)vertices[index].y);
        w_u16(record + index * 8u + 4u, (uint16)vertices[index].z);
        w_u16(record + index * 8u + 6u, vertices[index].depth);
        w_u32(record + 24u + index * 4u, vertices[index].screen);
        w_u16(record + 36u + index * 4u, vertices[index].uv);
        w_u32(record + 48u + index * 4u, vertices[index].color);
    }
    w_u16(record + 38u, clut); w_u16(record + 42u, page); w_u8(record + 51u, code);
}


uint32 sub_80011B48(uint32 triangle, uint32 packet, uint32 mask, uint32 depth)
{
    static const uint32 permutations[8] = {0u, 0x24u, 0x09u, 0x12u, 0x12u, 0x09u, 0x24u, 0x64u};
    MOH_PROJECT_SUBDIVISION_VERTEX input[3], a, b, c, ab, ac, bc, output[3];
    uint32 mode = mask & 7u, permutation, order[3], records, count, index, point, generated, child_mask, near;
    uint16 clut, page;
    uint8 code;
    FUNCTION_MARKER(0x80011B48u, "SLUS_009.74");
    /* TODO Original jump-table entry zero transfers to address zero */
    if (!mode) moh_draft_fail_fast("Invalid subdivision mask", 0x80011B48u);
    permutation = permutations[mode];
    near = r_u32(0x1F800010u) << 2u;
    for (index = 0u; index < 3u; ++index) {
        point = triangle + index * 8u;
        input[index].x = (sint16)r_u16(point); input[index].y = (sint16)r_u16(point + 2u);
        input[index].z = (sint16)r_u16(point + 4u); input[index].depth = r_u16(point + 6u);
        input[index].screen = r_u32(triangle + 24u + index * 4u);
        input[index].uv = r_u16(triangle + 36u + index * 4u);
        input[index].color = r_u32(triangle + 48u + index * 4u); input[index].flags = 0u;
        order[index] = (permutation >> (index * 2u)) & 3u;
    }
    a = input[order[0]]; b = input[order[1]]; c = input[order[2]];
    clut = r_u16(triangle + 38u); page = r_u16(triangle + 42u); code = r_u8(triangle + 51u);
    count = mode == 1u || mode == 2u || mode == 4u ? 3u : 4u;
    records = r_u32(0x1F800000u);
    w_u32(0x1F800000u, records + count * 60u);
    if (count == 3u) {
        ab = moh_project_subdivision_midpoint(a, b); ac = moh_project_subdivision_midpoint(a, c);
        output[0] = a; output[1] = ab; output[2] = ac;
        moh_project_subdivision_record(records, output, clut, page, code);
        output[0] = ab; output[1] = b; output[2] = ac;
        moh_project_subdivision_record(records + 60u, output, clut, page, code);
        output[0] = b; output[1] = c; output[2] = ac;
        moh_project_subdivision_record(records + 120u, output, clut, page, code);
        generated = ((sint32)ab.flags < 0 ? 16u : 0u) | (ab.depth < near ? 2u : 0u) |
            ((sint32)ac.flags < 0 ? 32u : 0u) | (ac.depth < near ? 4u : 0u);
        child_mask = generated | (mask & 9u);
        packet = (child_mask & 6u) && depth ? sub_80011B48(records, packet, child_mask, depth - 1u) : sub_800125C4(records, packet, ab.depth, ac.depth, ac.depth, 0x1F800000);
        child_mask = ((generated & 0x12u) >> 1u) | (generated & 0x24u);
        packet = (child_mask & 5u) && depth ? sub_80011B48(records + 60u, packet, child_mask, depth - 1u) : sub_800125C4(records + 60u, packet, r_u16(records + 66u), r_u16(records + 74u), r_u16(records + 82u), 0x1F800000);
        child_mask = generated & 0x24u;
        packet = (child_mask & 4u) && depth ? sub_80011B48(records + 120u, packet, child_mask, depth - 1u) : sub_800125C4(records + 120u, packet, r_u16(records + 126u), r_u16(records + 134u), r_u16(records + 142u), 0x1F800000);
    } else {
        ac = moh_project_subdivision_midpoint(a, c); bc = moh_project_subdivision_midpoint(b, c); ab = moh_project_subdivision_midpoint(a, b);
        output[0] = a; output[1] = ab; output[2] = ac;
        moh_project_subdivision_record(records, output, clut, page, code);
        output[0] = ab; output[1] = b; output[2] = bc;
        moh_project_subdivision_record(records + 60u, output, clut, page, code);
        output[0] = ac; output[1] = bc; output[2] = c;
        moh_project_subdivision_record(records + 120u, output, clut, page, code);
        output[0] = ac; output[1] = ab; output[2] = bc;
        moh_project_subdivision_record(records + 180u, output, clut, page, code);
        generated = ((sint32)ac.flags < 0 ? 8u : 0u) | (ac.depth < near ? 1u : 0u) |
            ((sint32)ab.flags < 0 ? 16u : 0u) | (ab.depth < near ? 2u : 0u) |
            ((sint32)bc.flags < 0 ? 32u : 0u) | (bc.depth < near ? 4u : 0u);
        packet = (generated & 7u) && (sint32)(depth - 1u) >= 0 ? sub_80011B48(records + 180u, packet, generated, depth - 1u) : sub_800125C4(records + 180u, packet, ac.depth, bc.depth, ab.depth, 0x1F800000);
        child_mask = (generated & 0x12u) | ((generated & 9u) << 2u);
        if ((child_mask & 6u) && depth) {
            child_mask |= mask & (9u << order[0]);
            packet = sub_80011B48(records, packet, child_mask, depth - 1u);
        } else packet = sub_800125C4(records, packet, r_u16(records + 6u), r_u16(records + 14u), r_u16(records + 22u), 0x1F800000);
        child_mask = ((generated & 0x12u) >> 1u) | (generated & 0x24u) | (mask & (9u << order[1]));
        packet = (child_mask & 5u) && (sint32)(depth - 1u) >= 0 ? sub_80011B48(records + 60u, packet, child_mask, depth - 1u) : sub_800125C4(records + 60u, packet, r_u16(records + 66u), r_u16(records + 74u), r_u16(records + 82u), 0x1F800000);
        child_mask = (generated & 9u) | ((generated & 0x24u) >> 1u) | (mask & (9u << order[2]));
        packet = (child_mask & 3u) && (sint32)(depth - 1u) >= 0 ? sub_80011B48(records + 120u, packet, child_mask, depth - 1u) : sub_800125C4(records + 120u, packet, r_u16(records + 126u), r_u16(records + 134u), r_u16(records + 142u), 0x1F800000);
    }
    w_u32(0x1F800000u, r_u32(0x1F800000u) - count * 60u);
    return packet;
}

static sint32 moh_project_light_quotient(uint32 distance, uint32 divisor)
{
    sint32 numerator = (sint32)(distance << 8u), denominator = (sint32)divisor;
    /* Retain the defined MIPS quotient for zero and overflow */
    if (!denominator) return numerator < 0 ? 1 : -1;
    if (numerator == (sint32)0x80000000u && denominator == -1) return numerator;
    return numerator / denominator;
}

void sub_80012FF8(uint32 region, uint32 vertices, uint32 colors_out, uint32 scratch)
{
    uint32 original_colors[3], distances[3], ids[3], index, axis, vertex, packed, xy, radius, table, color, factor, output, channel;
    sint32 center[3], scale, value;
    FUNCTION_MARKER(0x80012FF8u, "SLUS_009.74");
    for (index = 0; index < 3; ++index) original_colors[index] = r_u32(colors_out + index * 4);
    for (axis = 0u; axis < 3u; ++axis) center[axis] = (sint16)r_u16(scratch + 0x390 + axis * 2u);
    for (index = 0u; index < 3u; ++index) {
        ids[index] = r_u16(scratch + 0x3C6 + index * 2u); vertex = vertices + ids[index] * 8u;
        xy = r_u32(vertex);
        xport_gte_write_data(9u, (uint32)((sint32)(sint16)xy - center[0]));
        xport_gte_write_data(10u, (uint32)((sint32)(sint16)(xy >> 16u) - center[1]));
        xport_gte_write_data(11u, (uint32)((sint32)(sint16)r_u16(vertex + 4u) - center[2]));
        xport_gte_execute(0xA00428u);
        distances[index] = xport_gte_read_data(25u) + xport_gte_read_data(26u) + xport_gte_read_data(27u);
    }
    if (r_u32(0x800A94A8)) {
        table = r_u32(r_u32(region + 8u) + 24u); radius = r_u32(0x800A94D4);
        for (index = 0u; index < 3u; ++index) if (distances[index] <= radius) {
            vertex = table + ids[index] * 4u; color = r_u32(vertex);
            scale = (sint32)(0x10000u - (uint32)moh_project_light_quotient(distances[index],
                index == 0u ? (uint32)((sint32)radius >> 8) : radius >> 8u)) >> 3;
            for (axis = 0u; axis < 3u; ++axis) {
                channel = (color >> (axis * 8u)) & 255u;
                value = (sint32)(channel - (uint32)((sint32)(channel * (uint32)scale) >> 16));
                if (value < 1) value = 1;
                w_u8(vertex + axis, (uint8)value);
            }
            distances[index] = (distances[index] << 8u) >> 8u;
        }
    }
    radius = r_u32(0x800A94D4);
    for (index = 0u; index < 3u; ++index) if (distances[index] <= radius) {
        factor = 0x10000u - (uint32)moh_project_light_quotient(distances[index], radius >> 8u);
        packed = original_colors[index]; output = 0u;
        for (axis = 0u; axis < 3u; ++axis) {
            channel = ((packed >> (axis * 8u)) & 255u) + ((factor * r_u8(0x800A94CC + axis)) >> 16u);
            if (channel >= 256u) channel = 255u;
            output |= channel << (axis * 8u);
        }
        w_u32(colors_out + index * 4u, output);
    }
}

typedef struct MOH_VISIBILITY_CONTEXT {
    uint32 normal[6][3];
    uint32 width, height, depth, cursor;
} MOH_VISIBILITY_CONTEXT;

static uint32 moh_visibility_signed_arithmetic(uint32 first, uint32 second, uint32 subtract)
{
    sint64 value = (sint64)(sint32)first;
    value += subtract ? -(sint64)(sint32)second : (sint64)(sint32)second;
    if (value < -2147483647 - 1 || value > 2147483647)
        moh_draft_fail_fast("Unsupported original signed arithmetic overflow exception", 0x80013698);
    return (uint32)value;
}

static void moh_visibility_vertex(const sint32 bounds[6], const uint32 normal[3])
{
    uint32 x, y, z, packed;
    z = (uint32)bounds[(sint32)normal[2] < 0 ? 2 : 5];
    y = (uint32)bounds[(sint32)normal[1] < 0 ? 1 : 4];
    x = (uint32)bounds[(sint32)normal[0] < 0 ? 0 : 3];
    xport_gte_write_data(1, z);
    packed = moh_visibility_signed_arithmetic(y << 16, x, 0);
    xport_gte_write_data(0, packed);
}

static void moh_visibility_walk(uint32 node, MOH_VISIBILITY_CONTEXT *context)
{
    sint32 bounds[6];
    PsxGteSnapshot snapshot;
    uint32 axis, screen, flags, depth, first_child, second_child, third_child, delta;
    for (axis = 0; axis < 6; ++axis) bounds[axis] = (sint16)r_u16(node + axis * 2);
    moh_visibility_vertex(bounds, context->normal[0]);
    xport_gte_execute(0x180001);
    moh_visibility_vertex(bounds, context->normal[1]);
    screen = xport_gte_read_data(14);
    xport_gte_execute(0x180001);
    if (((sint32)screen >> 16) < 0) return;
    moh_visibility_vertex(bounds, context->normal[2]);
    screen = xport_gte_read_data(14);
    xport_gte_execute(0x180001);
    delta = moh_visibility_signed_arithmetic((uint32)(sint32)(sint16)screen, context->width, 1);
    if ((sint32)delta > 0) return;
    moh_visibility_vertex(bounds, context->normal[3]);
    screen = xport_gte_read_data(14);
    xport_gte_execute(0x180001);
    if ((sint16)screen < 0) return;
    moh_visibility_vertex(bounds, context->normal[4]);
    psx_gte_snapshot(&snapshot);
    flags = (uint32)snapshot.flag;
    depth = xport_gte_read_data(27);
    xport_gte_execute(0x180001);
    if (flags & 0x400000) return;
    depth = (uint32)((sint32)(depth << 16) >> 2);
    delta = moh_visibility_signed_arithmetic(depth, context->depth, 1);
    if ((sint32)delta > 0) return;
    moh_visibility_vertex(bounds, context->normal[5]);
    screen = xport_gte_read_data(14);
    xport_gte_execute(0x180001);
    delta = moh_visibility_signed_arithmetic((uint32)((sint32)screen >> 16), context->height, 1);
    if ((sint32)delta > 0) return;
    psx_gte_snapshot(&snapshot);
    if ((uint32)snapshot.flag & 0x40000) return;
    if ((sint32)r_u32(node + 12) > 0) {
        w_u32(context->cursor, node);
        context->cursor += 4;
        return;
    }
    /* Cache the pushed children before visiting them in original LIFO order */
    first_child = r_u32(node + 24);
    second_child = r_u32(node + 32);
    third_child = r_u32(node + 28);
    if (third_child) moh_visibility_walk(third_child, context);
    if (second_child) moh_visibility_walk(second_child, context);
    if (first_child) moh_visibility_walk(first_child, context);
}

uint32 sub_80013698(uint32 root)
{
    static const uint32 offsets[6] = {164, 136, 108, 220, 192, 80};
    MOH_VISIBILITY_CONTEXT context;
    uint32 view, plane, axis, saved_h;
    FUNCTION_MARKER(0x80013698, "SLUS_009.74");
    view = r_u32(0x80099428);
    view = r_u32(view + 0x388);
    view = r_u32(view + 0xC8);
    for (plane = 0; plane < 6; ++plane)
        for (axis = 0; axis < 3; ++axis) context.normal[plane][axis] = r_u32(view + offsets[plane] + axis * 4);
    context.depth = r_u32(view + 64);
    context.height = r_u32(0x80098B78);
    context.width = r_u32(0x80098B74);
    context.depth = (uint32)((sint32)context.depth >> 2);
    xport_gte_write_control(26, 400);
    context.cursor = 0x800A94E4;
    moh_visibility_walk(root, &context);
    saved_h = r_u32(0x80096974);
    xport_gte_write_control(26, saved_h);
    w_u32(0x800A94E0, (context.cursor - 0x800A94E4) >> 2);
    w_u32(0x800AA484, 0);
    return 0x8009696C;
}
