#include "game_level2p_signatures.h"
#include "xport_trace.h"
#include "psx.h"

uint32 level2p_sub_80057B04(uint32 owner, uint32 target, uint32 property, uint32 range, uint32 check_actors, uint32 store_result)
{
    uint8 origin[8], tip[8], offset[8], direction[8], relative[8];
    uint32 start[3], end[3], distance;
    uint32 origin_view, tip_view, offset_view, direction_view, relative_view, start_view, end_view, distance_view;
    uint32 height, flags, resource, bone, model, first, second, x, y, z, dx, dz, heading, delta, angle, sign, result, current, candidate, radius;
    FUNCTION_MARKER(0x80057B04, "LEVEL2P.BIN");
    height = r_u32(target + 0x2D4); flags = r_u32(target + 0x10); resource = r_u32(owner + 0x38C);
    if (flags & 0x800) height = (uint32)((sint32)height >> 1);
    if (!resource) {
        result = level2p_sub_80054718(owner, target, 3000);
        if (store_result) level2p_sub_8005A1B8(owner, property, result);
        return result;
    }
    /* TODO: Native local mapping and original opaque vector padding remain unresolved */
    origin_view = moh_draft_local_address(origin, sizeof(origin)); tip_view = moh_draft_local_address(tip, sizeof(tip));
    offset_view = moh_draft_local_address(offset, sizeof(offset)); direction_view = moh_draft_local_address(direction, sizeof(direction));
    relative_view = moh_draft_local_address(relative, sizeof(relative)); start_view = moh_draft_local_address(start, sizeof(start));
    end_view = moh_draft_local_address(end, sizeof(end)); distance_view = moh_draft_local_address(&distance, sizeof(distance));
    first = r_u32(0x8003C138); second = r_u32(0x8003C13C); w_u32(offset_view, first); w_u32(offset_view + 4, second);
    bone = (uint32)(sint32)r_s16(resource + 0xAA); model = r_u32(owner + 0x100);
    level2p_sub_8008BCF0(owner, model, bone, origin_view, 0, 0);
    bone = (uint32)(sint32)r_s16(resource + 0xAA); model = r_u32(owner + 0x100);
    level2p_sub_8008BCF0(owner, model, bone, tip_view, offset_view, 0);
    x = r_u16(tip_view); first = r_u16(origin_view); z = r_u16(tip_view + 4); y = r_u16(origin_view + 2);
    dx = x - first; second = r_u16(origin_view + 4); w_u16(offset_view, dx); dz = z - second;
    first = r_u16(tip_view + 2); w_u16(offset_view + 4, dz); w_u16(offset_view + 2, first - y);
    heading = moh_draft_missing_call(0x8001536C, (uint32)(sint32)(sint16)dx, (uint32)(sint32)(sint16)dz);
    x = (uint32)(sint32)r_s16(target + 0x20E); first = r_u16(origin_view); y = r_u16(origin_view + 2);
    dx = x - first; w_u16(direction_view, dx); first = r_u32(target + 0x210);
    second = (uint32)((sint32)(first - height) >> 16); w_u16(direction_view + 2, second - y);
    z = (uint32)(sint32)r_s16(target + 0x216); second = r_u16(origin_view + 4); dz = z - second; w_u16(direction_view + 4, dz);
    first = moh_draft_missing_call(0x8001536C, (uint32)(sint32)(sint16)dx, (uint32)(sint32)(sint16)dz);
    delta = (uint32)(sint32)(sint16)(first - heading);
    sign = level2p_sub_80054850(delta); angle = level2p_sub_80054850(delta);
    if ((sint16)sign < 0) angle = 0 - angle;
    first = level2p_sub_800548C4(direction_view); w_u32(distance_view, first); range = (uint32)(sint32)(sint16)range;
    if ((sint32)first >= (sint32)range) goto rejected;
    second = (uint32)(sint32)r_s16(resource + 0xB4);
    if ((sint16)angle >= (sint32)second) goto rejected;
    if (check_actors) {
        moh_draft_missing_call(0x80016A40, direction_view, direction_view); current = r_u32(0x80099434);
        /* TODO: Original object list assumes no cycles and does not exclude owner or target */
        while (current) {
            flags = r_u32(current + 0x10);
            if (flags & 0x4000) {
                flags = r_u32(current + 0xC);
                if (!(flags & 0x20080000)) {
                    first = r_u16(current + 0x2E0); second = r_u16(origin_view); w_u16(relative_view, first - second);
                    first = r_u16(current + 0x2E2); second = r_u16(origin_view + 2); w_u16(relative_view + 2, first - second);
                    first = r_u16(current + 0x2E4); second = r_u16(origin_view + 4); w_u16(relative_view + 4, first - second);
                    candidate = level2p_sub_800548C4(relative_view);
                    if ((sint32)candidate <= (sint32)range) {
                        first = r_u32(distance_view);
                        if ((sint32)candidate < (sint32)first) {
                            first = r_u32(relative_view); second = r_u32(relative_view + 4);
                            xport_gte_write_control(0, first); xport_gte_write_control(1, second);
                            x = r_u16(direction_view); y = r_u16(direction_view + 2); z = r_u16(direction_view + 4);
                            xport_gte_write_data(9, x); xport_gte_write_data(10, y); xport_gte_write_data(11, z); xport_gte_execute(0x41E012);
                            first = xport_gte_read_data(25);
                            if ((sint32)first > 0) {
                                x = (uint32)(sint32)r_s16(relative_view); y = (uint32)(sint32)r_s16(relative_view + 2);
                                xport_gte_write_control(0, x); z = (uint32)(sint32)r_s16(relative_view + 4);
                                xport_gte_write_control(2, y); xport_gte_write_control(4, z);
                                x = (uint32)(sint32)r_s16(direction_view); y = (uint32)(sint32)r_s16(direction_view + 2); z = (uint32)(sint32)r_s16(direction_view + 4);
                                xport_gte_write_data(9, x); xport_gte_write_data(10, y); xport_gte_write_data(11, z); xport_gte_execute(0x178000C);
                                x = xport_gte_read_data(9); y = xport_gte_read_data(10); z = xport_gte_read_data(11);
                                w_u16(relative_view, x); w_u16(relative_view + 2, y); w_u16(relative_view + 4, z);
                                x = r_u16(relative_view); y = r_u16(relative_view + 2); z = r_u16(relative_view + 4);
                                xport_gte_write_data(9, x); xport_gte_write_data(10, y); xport_gte_write_data(11, z); xport_gte_execute(0xA00428);
                                radius = r_u16(current + 0x2E8); radius *= radius;
                                x = xport_gte_read_data(25); y = xport_gte_read_data(26); z = xport_gte_read_data(27);
                                if ((sint32)(x + y + z) <= (sint32)radius) goto rejected;
                            }
                        }
                    }
                }
            }
            current = r_u32(current + 4);
        }
    }
    x = (uint32)(sint32)r_s16(origin_view); z = (uint32)(sint32)r_s16(origin_view + 4); w_u32(start_view, x << 16);
    y = (uint32)(sint32)r_s16(origin_view + 2); w_u32(start_view + 8, z << 16); w_u32(start_view + 4, y << 16);
    first = r_u32(target + 0x20C); w_u32(end_view, first); first = r_u32(target + 0x210); w_u32(end_view + 4, first - height);
    first = r_u32(target + 0x214); w_u32(end_view + 8, first);
    result = level2p_sub_8003F26C(start_view, end_view, offset_view, tip_view, distance_view, owner + 0x318, 0);
    if ((sint32)result < 0) {
        if (store_result) level2p_sub_8005A1B8(owner, property, 1);
        return 1;
    }
rejected:
    if (store_result) level2p_sub_8005A1B8(owner, property, 0);
    return 0;
}

uint32 level2p_sub_80057FB0(uint32 owner, uint32 command, uint32 flags)
{
    uint8 delta_first[8], delta_second[8], position[8];
    uint32 field, kind, identifier, target, operand, mode, value, first, second, sign, limit, check_actors;
    uint32 delta_view, position_view, owner_view, data, handle, index, admitted;
    FUNCTION_MARKER(0x80057FB0, "LEVEL2P.BIN");
    operand = r_u32(command + 4); field = level2p_sub_8005A984(1, flags, operand, owner);
    operand = r_u32(command + 8); kind = level2p_sub_8005A984(2, flags, operand, owner);
    operand = r_u8(command + 1); identifier = level2p_sub_8005A984(3, flags, operand, owner);
    operand = r_u32(command + 12); target = level2p_sub_8005A984(4, flags, operand, owner);
    if (!target) {
        if (kind == 0xFFFFFFFF) {
            if ((sint8)identifier == -1) target = owner;
        } else if ((sint8)identifier == -1) {
            target = level2p_sub_800551A4(owner, kind);
        } else {
            target = level2p_sub_800687CC(kind, (uint32)(sint32)(sint8)identifier);
        }
    }
    /* TODO: Original command modes and resolved object pointers are unchecked */
    mode = r_u8(command);
    switch (mode) {
    case 1:
        value = r_u32(target + 0xF4); level2p_sub_8005A1B8(owner, field, value);
        value = r_u32(target + 0xF4); /* Retain the original post-call read */
        break;
    case 3: value = r_u32(target + 0x2D4); goto store_value;
    case 4: value = r_u32(target + 0x290); goto store_value;
    case 5: value = r_u32(target + 0x44); goto store_value;
    case 6: value = r_u32(target + 0x48); goto store_value;
    case 8: value = target; goto store_value;
    case 9:
    case 10:
        if (mode == 10 && !kind) break;
        /* TODO: Native local mapping and original unused vector padding remain unresolved */
        delta_view = mode == 9 ? moh_draft_local_address(delta_first, sizeof(delta_first)) : moh_draft_local_address(delta_second, sizeof(delta_second));
        data = mode == 9 ? target : kind;
        first = r_u32(data + 0x20C); second = r_u32(owner + 0x20C); w_u16(delta_view, (uint32)((sint32)(second - first) >> 16));
        first = r_u32(owner + 0x210); second = r_u32(data + 0x210); w_u16(delta_view + 2, (uint32)((sint32)(first - second) >> 16));
        first = r_u32(owner + 0x214); second = r_u32(data + 0x214); w_u16(delta_view + 4, (uint32)((sint32)(first - second) >> 16));
        value = level2p_sub_800548C4(delta_view); goto store_value;
    case 11:
    case 16:
        first = r_u32(0x8009943C); first = r_u32(first + 0x388); first = r_u32(first);
        data = r_u32(owner + 0x38C);
        if (first & 2) {
            value = r_u16(data + (mode == 11 ? 0xB0 : 0xB2));
            second = (uint32)((sint32)(value << 16) >> 18); limit = (uint32)(sint32)(sint16)(value - second);
        } else {
            limit = (uint32)(sint32)r_s16(data + (mode == 11 ? 0xB0 : 0xB2));
        }
        check_actors = 1; goto check_visibility;
    case 12:
        sign = level2p_sub_8003FB68(owner, target); first = level2p_sub_8003FB68(owner, target);
        value = (uint32)(sint32)(sint16)first;
        if ((sint16)sign < 0) value = 0 - value;
        goto store_value;
    case 13: value = owner; goto store_value;
    case 14: first = r_u32(0x8009617C); value = (uint32)(sint32)r_s16(first); goto store_value;
    case 15: value = level2p_sub_80054948(owner); goto store_value;
    case 17:
        data = r_u32(owner + 0x38C); limit = 3000;
        if (data) limit = r_u16(data + 0xB0);
        level2p_sub_8005A1B8(owner, field, 0);
        position_view = moh_draft_local_address(position, sizeof(position));
        delta_view = moh_draft_local_address(delta_second, sizeof(delta_second)); owner_view = moh_draft_local_address(&owner, sizeof(owner));
        first = (uint32)(sint32)r_s16(owner + 0x20E); w_u16(position_view, first);
        first = (uint32)(sint32)r_s16(owner + 0x212); w_u16(position_view + 2, first);
        first = (uint32)(sint32)r_s16(owner + 0x216); w_u16(position_view + 4, first);
        data = 0x8008D474; handle = 0x8008D478;
        for (index = 1; index < 24; ++index, data += 0x34, handle += 0x34) {
            first = r_u8(data + 0xE);
            if (first) continue;
            first = r_u8(0x8008D3B4 + index);
            if (!(first & 1)) continue;
            first = r_u16(data + 0x28);
            if (first >= 60) continue;
            first = r_u32(data);
            if (first & 3) continue;
            admitted = level2p_sub_80068A64(handle);
            if (!admitted && level2p_sub_80068988(handle, owner_view)) continue;
            first = r_u16(position_view); second = r_u16(data + 0x2C); w_u16(delta_view, first - second);
            first = r_u16(position_view + 2); second = r_u16(data + 0x2E); w_u16(delta_view + 2, first - second);
            first = r_u16(position_view + 4); second = r_u16(data + 0x30); w_u16(delta_view + 4, first - second);
            value = level2p_sub_800548C4(delta_view);
            if ((sint32)(value & 0xFFFF) < (sint32)(sint16)limit) level2p_sub_8005A1B8(owner, field, 1);
        }
        break;
    case 18:
        data = r_u32(owner + 0x38C); limit = (uint32)(sint32)r_s16(data + 0xB2); check_actors = 0;
check_visibility:
        level2p_sub_80057B04(owner, target, field, limit, check_actors, 1);
        break;
    default: break;
store_value:
        level2p_sub_8005A1B8(owner, field, value);
        break;
    }
    return command + 16;
}

void level2p_sub_80058540(uint32 owner)
{
    uint32 resource, index, pointer, table, handle, value, owner_view;
    FUNCTION_MARKER(0x80058540, "LEVEL2P.BIN");
    resource = r_u32(owner + 0x38C); index = (uint32)(sint32)r_s16(resource + 0xB6);
    if ((sint32)index < 0) return;
    pointer = r_u32(0x80099430); pointer = r_u32(pointer + 0x388); table = r_u32(pointer + 8); handle = table + index * 0x34 + 0x30;
    /* TODO: Original index is unchecked; native owner local mapping remains unresolved */
    owner_view = moh_draft_local_address(&owner, sizeof(owner));
    if (level2p_sub_80068988(handle, owner_view)) {
        level2p_sub_80068A04(handle, 0); value = r_u32(owner + 0x10); w_u32(owner + 0x10, value & 0xFFFFFF7F);
    }
}

void level2p_sub_800585DC(uint32 owner)
{
    uint32 resource, value;
    FUNCTION_MARKER(0x800585DC, "LEVEL2P.BIN");
    /* TODO: Original resource pointer is unchecked */
    resource = r_u32(owner + 0x38C);
    value = r_u16(resource + 0xC4); w_u16(owner + 0x2EC, value);
    value = r_u16(resource + 0xC6); w_u16(owner + 0x2E8, value);
    value = r_u16(resource + 0xCA); w_u16(owner + 0x3CC, value);
    w_u16(resource + 0xC0, 0); w_u16(resource + 0xC2, 0); w_u32(owner + 0x224, 0x10000);
    value = r_u16(resource + 0xD0); w_u32(resource + 0xD8, 0); w_u16(resource + 0xCC, value);
}

void level2p_sub_80058628(uint32 state)
{
    uint32 value;
    FUNCTION_MARKER(0x80058628, "LEVEL2P.BIN");
    value = r_u32(state + 4); w_u32(state + 4, value & 0xFFFFA3FC);
}

uint32 level2p_sub_8005863C(uint32 owner, uint32 command, uint32 flags)
{
    uint32 operand, state, third, fourth, mode, value, mask, pointer, parameter;
    FUNCTION_MARKER(0x8005863C, "LEVEL2P.BIN");
    /* TODO: Original state pointers and command fields are unchecked */
    operand = r_u32(command + 4); state = r_u32(owner + 0x38C); third = r_u16(command + 12); fourth = r_u16(command + 14);
    mode = level2p_sub_8005A984(1, flags, operand, owner);
    switch (mode) {
    case 1:
    case 2:
    case 0x800:
    case 0x1000:
    case 0x4000:
        level2p_sub_80058628(state); level2p_sub_80058540(owner); level2p_sub_800585DC(owner);
        value = r_u32(state + 4); w_u32(state + 4, value | mode);
        operand = r_u32(command + 8); value = level2p_sub_8005A984(2, flags, operand, owner);
        if (mode == 2) {
            value = level2p_sub_80055660(value); w_u16(state + 0xB8, value);
            value = level2p_sub_8005A984(3, flags, (uint32)(sint32)(sint16)third, owner);
            parameter = (uint32)(sint32)r_s16(state + 0xB8); w_u16(state + 0xBA, value);
            value = level2p_sub_800556C8(owner, parameter);
            parameter = (uint32)(sint32)r_s16(state + 0xBA); w_u16(state + 0xBC, value);
            if (parameter == 0 || parameter == 2) w_u16(state + 0xBE, 1);
            else if (parameter == 1) w_u16(state + 0xBE, 0xFFFF);
            else if (parameter == 0xFFFFFFFF) w_u16(state + 0xBA, 2);
            break;
        }
        if (mode == 0x800) w_u32(state + 0x3C, value);
        else if (mode == 0x1000) w_u32(state + 0x40, value);
        else w_u32(state + 0x10, value);
        value = level2p_sub_8005A984(3, flags, (uint32)(sint32)(sint16)third, owner);
        if (mode == 0x4000) { w_u32(state + 0x50, value); break; }
        if (mode == 1) w_u16(state + 0xC2, value);
        else if (mode == 0x800) w_u32(state + 0x7C, value);
        else w_u32(state + 0x80, value);
        value = level2p_sub_8005A984(4, flags, (uint32)(sint32)(sint16)fourth, owner);
        if (mode == 1) {
            w_u16(state + 0xD4, value);
            if ((sint16)value <= 0) w_u16(state + 0xD4, 0x100);
            break;
        }
        w_u16(state + 0xA8, value);
        if (mode == 0x800) {
            pointer = r_u32(owner + 0x38C); parameter = r_u32(pointer + 0x3C); level2p_sub_80054EB0(owner, pointer, parameter);
        }
        operand = r_u16(command); value = level2p_sub_8005A984(5, flags, operand, owner); w_u16(state + 0xC2, value);
        break;
    case 4:
        value = r_u32(state + 4); value |= 4; w_u32(state + 4, value);
        if (value == 4) level2p_sub_800585DC(owner);
        operand = r_u32(command + 8); value = level2p_sub_8005A984(2, flags, operand, owner);
        w_u32(state + 0x18, value); w_u32(state + 0xDC, value); w_u32(state, 0xFFFFFFF6);
        value = level2p_sub_8005A984(3, flags, (uint32)(sint32)(sint16)third, owner); w_u16(state + 0xC0, value);
        value = level2p_sub_8005A984(4, flags, (uint32)(sint32)(sint16)fourth, owner); w_u32(state + 0xD8, (uint32)(sint32)(sint16)value);
        break;
    case 0x2000: mask = 0xFFFFFFFD; goto clear_bits;
    case 0xFFFFE000:
        value = r_u32(state + 4); w_u32(state + 4, value | 2); break;
    case 0x8000:
        w_u32(state + 4, 0); level2p_sub_80058540(owner); level2p_sub_800585DC(owner); break;
    case 0xFFFFFFFF: mask = 0xFFFFFFFE; goto reset_then_clear;
    case 0xFFFFFFFE: mask = 0xFFFFFFFD; goto reset_then_clear;
    case 0xFFFFC000: mask = 0xFFFFBFFF; goto reset_then_clear;
    case 0xFFFFF000: mask = 0xFFFFEFFF; goto reset_then_clear;
    case 0xFFFFFFFC: mask = 0xFFFFFFFB; goto clear_bits;
    case 0xFFFFFC00: mask = 0xFFFFFBFF; goto clear_bits;
    case 0xFFFFF800:
        level2p_sub_800585DC(owner); level2p_sub_80058540(owner); mask = 0xFFFFF7FF; goto clear_bits;
reset_then_clear:
        level2p_sub_800585DC(owner);
clear_bits:
        value = r_u32(state + 4); w_u32(state + 4, value & mask); break;
    default: break;
    }
    return command + 16;
}

static uint32 moh_record_distance_native(uint32 object, uint32 record, uint32 vector, uint32 record_x_first)
{
    uint32 first, second, x, y, z, temporary;
    if (record_x_first) { second = r_u16(record + 0x14); first = (uint32)(sint32)r_s16(object + 0x20E); }
    else { first = (uint32)(sint32)r_s16(object + 0x20E); second = r_u16(record + 0x14); }
    x = first - second; w_u16(vector, x); x = (uint32)(sint32)(sint16)x;
    first = (uint32)(sint32)r_s16(object + 0x212); second = r_u16(record + 0x16); w_u16(vector + 2, first - second);
    first = (uint32)(sint32)r_s16(object + 0x216); second = r_u16(record + 0x18); w_u16(vector + 4, first - second);
    y = r_u16(vector + 2); z = r_u16(vector + 4); y = (uint32)(sint32)(sint16)y; z = (uint32)(sint32)(sint16)z;
    if ((sint32)x < 0) x = 0 - x;
    if ((sint32)y < 0) y = 0 - y;
    if ((sint32)z < 0) z = 0 - z;
    if ((sint32)x < (sint32)y) { temporary = x; x = y; y = temporary; }
    if ((sint32)x < (sint32)z) { temporary = x; x = z; z = temporary; }
    return x + (uint32)((sint32)(y + z) >> 2);
}

uint32 level2p_sub_80041258(uint32 owner, uint32 range, uint32 excluded_index, uint32 mask, uint32 unused_parameter5)
{
    uint8 vector[8], axis[8];
    uint32 pointer, record, count, index, selected, distance, player, first_distance, second_distance, first, second, x, y, z, vector_view, axis_view;
    FUNCTION_MARKER(0x80041258, "LEVEL2P.BIN");
    pointer = r_u32(0x80099430); selected = 0xFFFFFFFF;
    if ((sint32)range < 0) range = 0 - range;
    pointer = r_u32(pointer + 0x388); record = r_u32(pointer + 8); count = r_u32(pointer + 12);
    /* TODO: Original record count/pointers and opaque vector padding are unchecked */
    /* TODO: Native local mapping remains unresolved */
    vector_view = moh_draft_local_address(vector, sizeof(vector)); axis_view = moh_draft_local_address(axis, sizeof(axis));
    (void)unused_parameter5;
    for (index = 0; (sint32)index < (sint32)count; ++index, record += 0x34) {
        if (index == excluded_index) continue;
        first = level2p_sub_80068A64(record + 0x30);
        if (!first && excluded_index != 0xFFFFFFFF) continue;
        distance = moh_record_distance_native(owner, record, vector_view, 0);
        if ((sint32)distance > (sint32)range) continue;
        first = r_u32(record + 0xC);
        if (!(first & mask) && mask != 0xFFFFFFFF) continue;
        player = r_u32(0x8009943C);
        first_distance = moh_record_distance_native(player, record, vector_view, 1);
        second_distance = moh_record_distance_native(player, record, vector_view, 0);
        if ((sint32)first_distance < (sint32)second_distance) second_distance = first_distance;
        if ((sint32)distance >= (sint32)second_distance && mask != 8) continue;
        if ((sint32)second_distance >= 10000) continue;
        first = r_u32(record + 0x24); second = r_u32(record + 0x28); w_u32(axis_view, first); w_u32(axis_view + 4, second);
        w_u16(vector_view + 2, 0); w_u16(axis_view + 2, 0);
        first = r_u32(vector_view); second = r_u32(vector_view + 4);
        xport_gte_write_control(0, first); xport_gte_write_control(1, second);
        x = r_u16(axis_view); y = r_u16(axis_view + 2); z = r_u16(axis_view + 4);
        xport_gte_write_data(9, x); xport_gte_write_data(10, y); xport_gte_write_data(11, z); xport_gte_execute(0x41E012);
        first = xport_gte_read_data(25);
        if (((sint32)first >> 12) > 100 || mask == 8) { selected = index; range = distance; }
    }
    return selected;
}

uint32 level2p_sub_80058BF4(uint32 owner, uint32 command, uint32 flags)
{
    uint32 operand, field, kind, target;
    FUNCTION_MARKER(0x80058BF4, "LEVEL2P.BIN");
    operand = r_u32(command + 4); field = level2p_sub_8005A984(1, flags, operand, owner);
    operand = r_u32(command + 8); kind = level2p_sub_8005A984(2, flags, operand, owner);
    target = level2p_sub_800551A4(owner, kind); level2p_sub_8005A1B8(owner, field, target);
    return command + 12;
}

uint32 level2p_sub_80058C78(uint32 owner, uint32 command, uint32 flags)
{
    uint32 operand, range, excluded_index, mask, parameter5, result, field;
    FUNCTION_MARKER(0x80058C78, "LEVEL2P.BIN");
    operand = r_u32(command + 8); range = level2p_sub_8005A984(2, flags, operand, owner);
    operand = r_u32(command + 12); excluded_index = level2p_sub_8005A984(3, flags, operand, owner);
    operand = r_u32(command + 16); mask = level2p_sub_8005A984(4, flags, operand, owner);
    operand = r_u32(command + 20); parameter5 = level2p_sub_8005A984(5, flags, operand, owner);
    result = level2p_sub_80041258(owner, range, excluded_index, mask, parameter5);
    operand = r_u32(command + 4); field = level2p_sub_8005A984(1, flags, operand, owner); level2p_sub_8005A1B8(owner, field, result);
    return command + 24;
}

static uint32 moh_saved_position_delta_native(uint32 object, uint32 position, uint32 vector, uint32 position_x_first)
{
    uint32 first, second, dy;
    if (position_x_first) { second = r_u16(position); first = (uint32)(sint32)r_s16(object + 0x20E); }
    else { first = (uint32)(sint32)r_s16(object + 0x20E); second = r_u16(position); }
    w_u16(vector, first - second);
    first = (uint32)(sint32)r_s16(object + 0x212); second = r_u16(position + 2); dy = first - second; w_u16(vector + 2, dy);
    first = (uint32)(sint32)r_s16(object + 0x216); second = r_u16(position + 4); w_u16(vector + 4, first - second);
    return dy;
}

uint32 level2p_sub_80058D68(uint32 owner, uint32 command, uint32 flags)
{
    uint8 vector[8], position[8];
    uint32 pointer, state, records, operand, index, field, count, mode, record, value, first, second, dy;
    uint32 vector_view, position_view, x, y, z, player, owner_distance, first_distance, minimum, heading, delta, sign, angle, limit;
    FUNCTION_MARKER(0x80058D68, "LEVEL2P.BIN");
    pointer = r_u32(0x80099430); state = r_u32(pointer + 0x388); operand = r_u32(command + 4); records = r_u32(state + 8);
    index = level2p_sub_8005A984(1, flags, operand, owner);
    operand = r_u32(command + 8); field = level2p_sub_8005A984(2, flags, operand, owner);
    count = r_u32(state + 12);
    if (index >= count) { value = 0xFFFFFFFF; goto store_value; }
    mode = r_u8(command);
    if (mode < 1 || mode > 7) return command + 12;
    record = records + index * 0x34;
    if (mode <= 5) { value = r_u32(record + (mode - 1) * 4); goto store_value; }
    if (mode == 6) {
        first = r_u32(record + 0x30); value = 0;
        if (first) value = r_u32(0x80099460 + (first & 0xFFFF) * 4);
        goto store_value;
    }
    /* TODO: Original record/resource pointers and table indices are unchecked */
    first = r_u32(record + 4);
    if (first) {
        first = r_u32(record + 0x2C);
        if ((sint32)first >= 0) {
            pointer = r_u32(0x80099430); pointer = r_u32(pointer + 0x388); pointer = r_u32(pointer + 0x14);
            first = r_u32(pointer + first * 8);
            if (first & 2) { value = 0x7FFF; goto store_value; }
        }
    }
    pointer = r_u32(owner + 0x38C); first = (uint32)(sint32)r_s16(pointer + 0xB6);
    pointer = (sint32)first == (sint32)(sint16)index ? pointer + 0xA0 : record + 0x14;
    /* TODO: Native local mapping and original vector padding remain unresolved */
    vector_view = moh_draft_local_address(vector, sizeof(vector)); position_view = moh_draft_local_address(position, sizeof(position));
    first = r_u32(pointer); second = r_u32(pointer + 4); w_u32(position_view, first); w_u32(position_view + 4, second);
    dy = moh_saved_position_delta_native(owner, position_view, vector_view, 0);
    first = r_u32(record + 4);
    if ((sint32)first >= 500) {
        dy = (uint32)(sint32)(sint16)dy;
        if ((sint32)dy < 0) dy = 0 - dy;
        if ((sint32)dy >= 201) { value = 0x7FFE; goto store_value; }
        first = r_u32(vector_view); second = r_u32(vector_view + 4);
        xport_gte_write_control(0, first); xport_gte_write_control(1, second);
        x = r_u16(record + 0x24); y = r_u16(record + 0x26); z = r_u16(record + 0x28);
        xport_gte_write_data(9, x); xport_gte_write_data(10, y); xport_gte_write_data(11, z); xport_gte_execute(0x41E012);
        first = xport_gte_read_data(25);
        if (((sint32)first >> 12) >= 101) { value = 0x7FFE; goto store_value; }
    }
    owner_distance = level2p_sub_800548C4(vector_view);
    player = r_u32(0x8009943C); moh_saved_position_delta_native(player, position_view, vector_view, 1);
    first_distance = level2p_sub_800548C4(vector_view);
    if ((sint32)first_distance < 600) { value = 0x7FFE; goto store_value; }
    player = r_u32(0x80099440); moh_saved_position_delta_native(player, position_view, vector_view, 1);
    minimum = level2p_sub_800548C4(vector_view);
    if ((sint32)first_distance < (sint32)minimum) minimum = first_distance;
    x = (uint32)(sint32)r_s16(record + 0x24); z = (uint32)(sint32)r_s16(record + 0x28);
    heading = moh_draft_missing_call(0x8001536C, x, z);
    x = (uint32)(sint32)r_s16(vector_view); z = (uint32)(sint32)r_s16(vector_view + 4);
    first = moh_draft_missing_call(0x8001536C, x, z); delta = (uint32)(sint32)(sint16)(first - heading);
    sign = level2p_sub_80054850(delta); angle = level2p_sub_80054850(delta);
    if ((sint16)sign < 0) angle = 0 - angle;
    first = r_u32(record + 12); limit = first == 16 ? 7000 : 3500;
    value = 0x7FFD;
    if ((sint32)minimum < (sint32)limit && (sint32)owner_distance < (sint32)minimum && (sint16)angle < 1024) value = (uint32)(sint32)(sint16)angle;
store_value:
    level2p_sub_8005A1B8(owner, field, value);
    return command + 12;
}

uint32 level2p_sub_80059288(uint32 owner, uint32 command, uint32 flags)
{
    uint32 pointer, state, records, operand, index, value, count, mode;
    FUNCTION_MARKER(0x80059288, "LEVEL2P.BIN");
    pointer = r_u32(0x80099430); state = r_u32(pointer + 0x388); operand = r_u32(command + 4); records = r_u32(state + 8);
    index = level2p_sub_8005A984(1, flags, operand, owner);
    operand = r_u32(command + 8); value = level2p_sub_8005A984(2, flags, operand, owner); count = r_u32(state + 12);
    if (index < count) {
        mode = r_u8(command);
        if (mode == 1) w_u32(records + index * 0x34, value);
        else if (mode == 6) level2p_sub_80068A04(records + index * 0x34 + 0x30, value);
    }
    return command + 12;
}

uint32 level2p_sub_80059384(uint32 owner, uint32 command, uint32 flags)
{
    uint32 pointer, state, records, operand, index, field, mode, value, resource, next_resource, x, z;
    FUNCTION_MARKER(0x80059384, "LEVEL2P.BIN");
    pointer = r_u32(0x80099430); state = r_u32(pointer + 0x388); operand = r_u32(command + 4); records = r_u32(state + 8);
    index = level2p_sub_8005A984(1, flags, operand, owner);
    operand = r_u32(command + 8); field = level2p_sub_8005A984(2, flags, operand, owner);
    operand = r_u32(command + 12); mode = level2p_sub_8005A984(3, flags, operand, owner); value = r_u32(state + 12);
    if (index >= value) return command + 16;
    /* TODO: Original resource and record pointers are unchecked */
    value = r_u32(records + index * 0x34 + 4);
    if (value) {
        resource = r_u32(owner + 0x38C); value = r_u16(resource + 0xC6); next_resource = r_u32(owner + 0x38C);
        w_u16(owner + 0x2E8, (uint32)((sint32)(value << 16) >> 17)); value = r_u16(next_resource + 0xCA);
        w_u16(owner + 0x3CC, (uint32)((sint32)(value << 16) >> 17));
    }
    if (!mode || (mode & 1)) {
        resource = r_u32(owner + 0x38C); value = (uint32)(sint32)r_s16(resource + 0xA0); next_resource = r_u32(owner + 0x38C);
        w_u32(owner + 0x20C, value << 16); value = (uint32)(sint32)r_s16(next_resource + 0xA4); w_u32(owner + 0x214, value << 16);
    }
    if (!mode || (mode & 2)) {
        value = (uint32)(sint32)r_s16(records + index * 0x34 + 0x1E); value <<= 16;
        w_u32(owner + 0x22C, value); level2p_sub_8005A1B8(owner, field, value);
    } else if (mode & 4) {
        pointer = records + index * 0x34; x = (uint32)(sint32)r_s16(pointer + 0x24); z = (uint32)(sint32)r_s16(pointer + 0x28);
        value = moh_draft_missing_call(0x8001536C, x, z); value <<= 16;
        w_u32(owner + 0x22C, value); level2p_sub_8005A1B8(owner, field, value);
    }
    value = r_u32(owner + 0x10); w_u32(owner + 0x10, value | 0x80);
    return command + 16;
}

uint32 level2p_sub_8005955C(uint32 owner, uint32 command, uint32 flags)
{
    uint8 source[8], point[8], angle[8];
    uint32 operand, kind, identifier, field, angle_field, target, value, first, second, source_view, point_view, angle_view;
    FUNCTION_MARKER(0x8005955C, "LEVEL2P.BIN");
    operand = r_u32(command + 4); kind = level2p_sub_8005A984(1, flags, operand, owner);
    operand = r_u32(command + 8); identifier = level2p_sub_8005A984(2, flags, operand, owner);
    operand = r_u32(command + 16); field = level2p_sub_8005A984(4, flags, operand, owner);
    operand = r_u32(command + 20); angle_field = level2p_sub_8005A984(5, flags, operand, owner);
    if (kind == 0xFFFFFFFF) {
        operand = r_u32(command + 12); target = level2p_sub_8005A984(3, flags, operand, owner);
    } else {
        identifier = (uint32)(sint32)(sint8)identifier;
        target = level2p_sub_800687CC(kind, identifier == 0xFFFFFFFF ? 0 : identifier);
    }
    if (!target) {
        level2p_sub_8005A1B8(owner, field, 0); value = 0;
    } else {
        /* TODO: Native local mapping remains unresolved; original target pointer is unchecked */
        source_view = moh_draft_local_address(source, sizeof(source)); point_view = moh_draft_local_address(point, sizeof(point));
        angle_view = moh_draft_local_address(angle, sizeof(angle)); moh_draft_missing_call(0x800158AC, source_view, 0, 8);
        first = (uint32)(sint32)r_s16(target + 0x20E); w_u16(source_view, first);
        first = (uint32)(sint32)r_s16(target + 0x212); w_u16(source_view + 2, first);
        first = (uint32)(sint32)r_s16(target + 0x216); w_u16(source_view + 4, first);
        first = r_u32(source_view); second = r_u32(source_view + 4); w_u32(point_view, first); w_u32(point_view + 4, second);
        value = level2p_sub_80054510(owner, point_view, angle_view); level2p_sub_8005A1B8(owner, field, value);
        value = r_u32(angle_view);
    }
    level2p_sub_8005A1B8(owner, angle_field, value);
    return command + 24;
}

uint32 level2p_sub_8005970C(uint32 owner, uint32 command, uint32 flags)
{
    uint32 context, depth, operand, index, model, saved, frame, table, value;
    FUNCTION_MARKER(0x8005970C, "LEVEL2P.BIN");
    context = r_u32(0x8009618C); depth = r_u16(context + 0x1E); ++depth; w_u16(context + 0x1E, depth);
    if ((sint16)depth < 6) {
        operand = r_u16(command); index = level2p_sub_8005A984(1, flags, operand, owner);
        /* TODO: Original signed command depth and model table indices are unchecked */
        context = r_u32(0x8009618C); model = r_u32(owner + 0x100); saved = r_u32(0x80096184);
        depth = (uint32)(sint32)r_s16(context + 0x1E); model = r_u32(model + 8); frame = context + (depth << 4) + 0x14;
        table = r_u32(model + 0xB8); w_u32(frame, command + 4);
        value = r_u32(saved + 12); table += index << 4; w_u32(frame + 4, value);
        value = r_u32(table + 12); w_u32(frame + 8, value); value = r_u16(table + 10); w_u16(frame + 12, value);
        value = r_u32(table + 12); w_u32(context + 0x14, value); return value;
    }
    level2p_sub_800885B8(5, 0x140); context = r_u32(0x8009618C); w_u16(context + 0x1E, 5);
    return command + 4;
}

uint32 level2p_sub_8005985C(uint32 owner, uint32 command, uint32 flags)
{
    uint8 state[0xB8], source[8], point[8], angle[8];
    uint32 pointer, records, operand, index, field, angle_field, state_view, source_view, point_view, angle_view, value, first, second;
    FUNCTION_MARKER(0x8005985C, "LEVEL2P.BIN");
    pointer = r_u32(0x80099430); pointer = r_u32(pointer + 0x388); operand = r_u32(command + 4); records = r_u32(pointer + 8);
    index = level2p_sub_8005A984(1, flags, operand, owner);
    operand = r_u32(command + 8); field = level2p_sub_8005A984(2, flags, operand, owner);
    operand = r_u32(command + 12); angle_field = level2p_sub_8005A984(3, flags, operand, owner);
    /* TODO: Native local mapping and untouched state/vector padding remain unresolved */
    /* TODO: Original record indices are unchecked */
    state_view = moh_draft_local_address(state, sizeof(state)); angle_view = moh_draft_local_address(angle, sizeof(angle));
    value = level2p_sub_80054EB0(owner, state_view, index);
    if (value) {
        source_view = moh_draft_local_address(source, sizeof(source)); point_view = moh_draft_local_address(point, sizeof(point));
        moh_draft_missing_call(0x800158AC, source_view, 0, 8);
        value = r_u16(state_view + 0xAC); w_u16(source_view, value);
        first = r_u16(records + index * 0x34 + 0x16); second = r_u16(state_view + 0xAE);
        w_u16(source_view + 4, second); w_u16(source_view + 2, first);
        first = r_u32(source_view); second = r_u32(source_view + 4); w_u32(point_view, first); w_u32(point_view + 4, second);
    } else point_view = state_view + 0xA0;
    value = level2p_sub_80054510(owner, point_view, angle_view); level2p_sub_8005A1B8(owner, field, value);
    value = r_u32(angle_view); level2p_sub_8005A1B8(owner, angle_field, value);
    return command + 16;
}

uint32 level2p_sub_800599C4(uint32 owner, uint32 command, uint32 flags)
{
    uint32 current, operand, value;
    FUNCTION_MARKER(0x800599C4, "LEVEL2P.BIN");
    current = (uint32)(sint32)r_s16(owner + 0x192); operand = (uint32)(sint32)r_s16(command);
    if (current != operand) {
        value = level2p_sub_8005A984(1, flags, operand, owner); level2p_sub_8003F764(owner, (uint32)(sint32)(sint16)value, 0, 1);
    }
    return command + 4;
}

uint32 level2p_sub_80059A44(uint32 owner, uint32 command, uint32 flags)
{
    uint32 operand, field, value;
    FUNCTION_MARKER(0x80059A44, "LEVEL2P.BIN");
    operand = r_u16(command); field = level2p_sub_8005A984(1, flags, operand, owner);
    value = (uint32)(sint32)r_s16(owner + 0x192); level2p_sub_8005A1B8(owner, field, value);
    return command + 4;
}

uint32 level2p_sub_80059A98(uint32 owner, uint32 command, uint32 flags)
{
    uint32 operand, field, mode, step, value;
    FUNCTION_MARKER(0x80059A98, "LEVEL2P.BIN");
    operand = r_u8(command); field = level2p_sub_8005A984(1, flags, operand, owner);
    operand = r_u8(command + 1); mode = level2p_sub_8005A984(2, flags, operand, owner); mode = (uint32)(sint32)(sint16)mode;
    operand = r_u32(command + 4); step = level2p_sub_8005A984(3, flags, operand, owner);
    value = level2p_sub_80055430(owner, mode, step); level2p_sub_8005A1B8(owner, field, value);
    return command + 8;
}

uint32 level2p_sub_80059B44(uint32 owner, uint32 command, uint32 flags)
{
    uint32 operand, target, value, current;
    FUNCTION_MARKER(0x80059B44, "LEVEL2P.BIN");
    operand = r_u32(command + 4); target = level2p_sub_8005A984(1, flags, operand, owner);
    if (target) {
        operand = r_u32(command + 8); value = level2p_sub_8005A984(2, flags, operand, owner);
        current = r_u32(target + 0x3C); w_u32(target + 0x3C, current | value);
    }
    return command + 12;
}

uint32 level2p_sub_80059BC8(uint32 owner, uint32 command, uint32 flags)
{
    uint32 operand, target, value, current;
    FUNCTION_MARKER(0x80059BC8, "LEVEL2P.BIN");
    operand = r_u32(command + 4); target = level2p_sub_8005A984(1, flags, operand, owner);
    if (target) {
        operand = r_u32(command + 8); value = level2p_sub_8005A984(2, flags, operand, owner);
        current = r_u32(target + 0x38); w_u32(target + 0x38, current | value);
    }
    return command + 12;
}

uint32 level2p_sub_80059C4C(uint32 owner, uint32 command, uint32 flags)
{
    uint32 model, operand, value;
    FUNCTION_MARKER(0x80059C4C, "LEVEL2P.BIN");
    model = r_u32(owner + 0x100);
    if (model) {
        operand = r_u8(command); value = level2p_sub_8005A984(1, flags, operand, owner);
        model = r_u32(owner + 0x100); w_u32(model + 0x6C, value);
    }
    return command + 4;
}

uint32 level2p_sub_80059CA8(uint32 owner, uint32 command, uint32 flags)
{
    uint32 operand, message;
    FUNCTION_MARKER(0x80059CA8, "LEVEL2P.BIN");
    operand = r_u16(command); message = level2p_sub_8005A984(1, flags, operand, owner);
    level2p_sub_80061410(message, owner, 0, 0);
    return command + 4;
}

uint32 level2p_sub_80059D00(uint32 owner, uint32 command, uint32 flags)
{
    uint32 mode, x, z, angle, operand, field;
    FUNCTION_MARKER(0x80059D00, "LEVEL2P.BIN");
    mode = r_u8(command + 1);
    if (mode < 2) {
        x = (uint32)(sint32)r_s16(owner + 0x3A4); z = (uint32)(sint32)r_s16(owner + 0x3A8);
        w_u32(owner + 0x20C, x << 16); w_u32(owner + 0x214, z << 16);
    }
    if (!mode || mode == 2) {
        angle = (uint32)(sint32)r_s16(owner + 0x3AE); w_u32(owner + 0x22C, angle << 16);
        operand = r_u8(command); field = level2p_sub_8005A984(1, flags, operand, owner);
        angle = r_u32(owner + 0x22C); level2p_sub_8005A1B8(owner, field, angle);
    }
    return command + 4;
}

uint32 level2p_sub_80059D98(uint32 owner, uint32 command, uint32 flags)
{
    uint32 operand, model, field, destination, index, table, value;
    FUNCTION_MARKER(0x80059D98, "LEVEL2P.BIN");
    operand = r_u32(command + 4); model = r_u32(owner + 0x100); field = level2p_sub_8005A984(1, flags, operand, owner);
    destination = level2p_sub_8005A974(field, owner);
    operand = r_u32(command + 8); index = level2p_sub_8005A984(2, flags, operand, owner);
    /* TODO: Original model table pointer and index are unchecked */
    table = r_u32(model + 0x78); value = r_u32(table + index * 4); w_u32(destination, value != 0);
    return command + 12;
}

uint32 level2p_sub_80059E30(uint32 owner, uint32 command, uint32 flags)
{
    uint8 direction[8], offset[8];
    uint32 operand, distance, target, packed, lateral, field, mode, direction_view, offset_view, x, y, z, first, second, third, value, product;
    FUNCTION_MARKER(0x80059E30, "LEVEL2P.BIN");
    operand = r_u16(command); distance = level2p_sub_8005A984(2, flags, operand, owner);
    operand = r_u32(command + 4); target = level2p_sub_8005A984(1, flags, operand, owner);
    packed = r_u32(command + 8); lateral = level2p_sub_8005A984(3, flags, packed & 0xFFFF, owner);
    field = level2p_sub_8005A984(4, flags, (packed >> 16) & 0xFFF, owner);
    mode = level2p_sub_8005A984(5, flags, packed >> 28, owner);
    /* TODO: Native local mapping and opaque direction padding remain unresolved */
    /* TODO: Original target pointer is unchecked */
    direction_view = moh_draft_local_address(direction, sizeof(direction)); offset_view = moh_draft_local_address(offset, sizeof(offset));
    moh_draft_missing_call(0x800158AC, offset_view, 0, 8);
    if (!mode) {
        first = (uint32)(sint32)r_s16(target + 0x20E); second = (uint32)(sint32)r_s16(owner + 0x20E);
        w_u16(direction_view + 2, 0); w_u16(direction_view, first - second);
        first = (uint32)(sint32)r_s16(target + 0x216); second = (uint32)(sint32)r_s16(owner + 0x216); w_u16(direction_view + 4, first - second);
        moh_draft_missing_call(0x80016A40, direction_view, direction_view);
        x = (uint32)(sint32)r_s16(direction_view); z = (uint32)(sint32)r_s16(direction_view + 4);
        value = moh_draft_missing_call(0x8001536C, x, z); value <<= 16;
        level2p_sub_8005A1B8(owner, field, value); w_u32(owner + 0x22C, value);
    } else if (mode & 1) {
        value = r_u16(target + 0x344); w_u16(direction_view + 2, 0); w_u16(direction_view, value);
        value = r_u16(target + 0x350); w_u16(direction_view + 4, value);
        value = r_u32(target + 0x22C); level2p_sub_8005A1B8(owner, field, value);
        value = r_u32(target + 0x22C); w_u32(owner + 0x22C, value);
    } else return command + 12;
    if ((sint16)lateral) {
        w_u16(offset_view, lateral);
        first = r_u32(target + 0x340); second = r_u32(target + 0x344);
        xport_gte_write_control(0, first); xport_gte_write_control(1, second);
        first = r_u32(target + 0x348); second = r_u32(target + 0x34C); third = r_u32(target + 0x350);
        xport_gte_write_control(2, first); xport_gte_write_control(3, second); xport_gte_write_control(4, third);
        first = r_u32(target + 0x354); second = r_u32(target + 0x358); xport_gte_write_control(5, first);
        third = r_u32(target + 0x35C); xport_gte_write_control(6, second); xport_gte_write_control(7, third);
        first = r_u32(offset_view); xport_gte_write_data(0, first); second = r_u32(offset_view + 4); xport_gte_write_data(1, second);
        xport_gte_execute(0x480012); x = xport_gte_read_data(25); y = xport_gte_read_data(26); z = xport_gte_read_data(27);
        w_u16(offset_view, x); w_u16(offset_view + 2, y); w_u16(offset_view + 4, z); xport_gte_read_control(31);
        value = (uint32)(sint32)r_s16(direction_view); product = (0 - value) * distance; value = (uint32)(sint32)r_s16(offset_view);
        w_u32(owner + 0x20C, ((product >> 12) + value) << 16);
        value = (uint32)(sint32)r_s16(direction_view + 4); product = (0 - value) * distance; value = (uint32)(sint32)r_s16(offset_view + 4);
        w_u32(owner + 0x214, ((product >> 12) + value) << 16);
    } else {
        value = (uint32)(sint32)r_s16(direction_view); product = (0 - value) * distance; value = r_u32(target + 0x20C);
        w_u32(owner + 0x20C, value + ((product >> 12) << 16));
        value = (uint32)(sint32)r_s16(direction_view + 4); product = (0 - value) * distance; value = r_u32(target + 0x214);
        w_u32(owner + 0x214, value + ((product >> 12) << 16));
    }
    return command + 12;
}

uint32 level2p_sub_8005A130(uint32 owner, uint32 command, uint32 flags)
{
    uint32 operand, target, event;
    FUNCTION_MARKER(0x8005A130, "LEVEL2P.BIN");
    operand = r_u32(command + 4); target = level2p_sub_8005A984(2, flags, operand, owner);
    operand = r_u16(command); event = level2p_sub_8005A984(1, flags, operand, owner);
    level2p_sub_800615E4(target, 0, event);
    return command + 8;
}

void level2p_sub_8005ABD0(void)
{
    uint32 address = 0x80096834, remaining = 50;
    FUNCTION_MARKER(0x8005ABD0, "LEVEL2P.BIN");
    do { w_u32(address, 0); --remaining; address -= 0x20; } while (remaining);
    w_u32(0x80096198, 0);
}

uint32 level2p_sub_8005AD58(uint32 text, uint32 x, uint32 y, uint32 red, uint32 green, uint32 blue, uint32 parameter7, uint32 parameter8, uint32 parameter9, uint32 parameter10, uint32 tag, uint32 duration, uint32 red_step, uint32 green_step, uint32 blue_step, uint32 output)
{
    uint32 index, magnitude, numerator, blue_duration, green_duration, red_duration, entry;
    FUNCTION_MARKER(0x8005AD58, "LEVEL2P.BIN");
    red &= 0xFF; green &= 0xFF; blue &= 0xFF; parameter7 &= 0xFF; parameter9 &= 0xFF;
    red_step &= 0xFF; green_step &= 0xFF; blue_step &= 0xFF;
    index = level2p_sub_8005ABFC(text, x, y, red, green, blue, parameter7, parameter8, parameter9, parameter10, output) & 0xFF;
    /* TODO: Original low8 truncation makes the -1 allocation check unreachable and can select record255 */
    if (index != 0xFFFFFFFF) {
        if ((sint32)duration < 0) {
            magnitude = 0 - duration; numerator = magnitude - blue;
            if (!blue_step) moh_draft_fail_fast("Original division by zero BREAK", 0x8005AD58);
            blue_duration = (uint32)((sint32)numerator / (sint32)blue_step);
            numerator = magnitude - green;
            if (!green_step) moh_draft_fail_fast("Original division by zero BREAK", 0x8005AD58);
            green_duration = (uint32)((sint32)numerator / (sint32)green_step);
            numerator = magnitude - red;
            if (!red_step) moh_draft_fail_fast("Original division by zero BREAK", 0x8005AD58);
            red_duration = (uint32)((sint32)numerator / (sint32)red_step);
            duration = blue_duration;
            if ((sint32)green_duration < (sint32)duration) duration = green_duration;
            if ((sint32)red_duration < (sint32)duration) duration = red_duration;
        }
        entry = 0x80096214 + index * 0x20;
        w_u32(entry + 0x14, duration); w_u8(entry + 0x1C, red_step); w_u8(entry + 0x1D, green_step); w_u8(entry + 0x1E, blue_step); w_u32(entry + 0x10, tag);
    }
    return index;
}

void level2p_sub_8005B00C(void)
{
    uint32 count, index = 0, entry, value, lifetime, first, second, red, green, blue;
    FUNCTION_MARKER(0x8005B00C, "LEVEL2P.BIN");
    count = r_u32(0x80096198);
    /* TODO: Original signed record count and compacting removal indices are unchecked */
    while ((sint32)index < (sint32)count) {
        entry = 0x80096214 + index * 0x20; value = r_u32(entry + 12);
        if (value == 0xFFFFFFFF) ++index;
        else {
            --value; w_u32(entry + 12, value);
            if ((sint32)value > 0) ++index;
            else level2p_sub_8005AF10(index);
        }
        count = r_u32(0x80096198);
    }
    for (index = 0, entry = 0x80096214;; ++index, entry += 0x20) {
        count = r_u32(0x80096198);
        if ((sint32)index >= (sint32)count) return;
        value = r_u32(entry + 0x10);
        if ((sint32)value < 0) continue;
        if ((sint32)value > 0) {
            lifetime = r_u32(entry + 0x14); w_u32(entry + 0x10, value - 1);
            if ((sint32)lifetime <= 0) continue;
            if (lifetime != 0x7FFF) w_u32(entry + 0x14, lifetime - 1);
            first = r_u8(entry + 8); second = r_u8(entry + 0x1C); red = first + second;
            if (red > 255) red = 255;
            first = r_u8(entry + 9); second = r_u8(entry + 0x1D); w_u8(entry + 8, red); green = first + second;
            if (green > 255) green = 255;
            first = r_u8(entry + 10); second = r_u8(entry + 0x1E); blue = first + second; w_u8(entry + 9, green);
            if (blue > 255) blue = 255;
        } else {
            first = r_u8(entry + 8); second = r_u8(entry + 0x1C); red = first - second;
            if ((sint32)red <= 0) red = 0;
            first = r_u8(entry + 9); second = r_u8(entry + 0x1D); w_u8(entry + 8, red); green = first - second;
            if ((sint32)green <= 0) green = 0;
            first = r_u8(entry + 10); second = r_u8(entry + 0x1E); blue = first - second;
            w_u8(entry + 9, green); if ((sint32)blue <= 0) blue = 0;
        }
        w_u8(entry + 10, blue);
    }
}

void level2p_sub_8005BE40(uint32 buffer_index)
{
    uint32 group, index, packet, first, second;
    FUNCTION_MARKER(0x8005BE40, "LEVEL2P.BIN");
    /* TODO: Original buffer index is unchecked */
    for (group = 0; group < 2; ++group) {
        packet = 0x800AB9F8 + (buffer_index << 7) + group * 0x40;
        for (index = 0; index < 2; ++index, packet += 16) {
            moh_draft_missing_call(0x8001DB40, packet);
            w_u8(packet + 4, 0x40); w_u8(packet + 5, 0x40); w_u8(packet + 6, 0xDC);
            w_u16(packet + 8, 60); w_u16(packet + 10, 44); w_u16(packet + 12, 60); w_u16(packet + 14, 44);
            if (!index) {
                first = r_u16(packet + 8); second = r_u16(packet + 12); w_u16(packet + 8, first - 1); w_u16(packet + 12, second + 1);
            } else { w_u16(packet + 10, 43); w_u16(packet + 14, 45); }
        }
        for (; index < 4; ++index, packet += 16) {
            moh_draft_missing_call(0x8001DB40, packet); w_u8(packet + 4, 255); w_u8(packet + 5, 32); w_u8(packet + 6, 32);
        }
    }
}

void level2p_sub_8005BF8C(uint32 buffer_index)
{
    uint32 packet_first, packet_second, angle, toggle = 0, sine, cosine, next_sine, next_cosine, value;
    FUNCTION_MARKER(0x8005BF8C, "LEVEL2P.BIN");
    /* TODO: Original buffer index is unchecked */
    packet_first = 0x800AADF8 + buffer_index * 0x600; packet_second = packet_first + 0x300;
    for (angle = 0; angle < 0x1000; angle += 0x80, packet_first += 24, packet_second += 24) {
        sine = moh_draft_missing_call(0x8001C908, angle); cosine = moh_draft_missing_call(0x8001C9A8, angle);
        next_sine = moh_draft_missing_call(0x8001C908, angle + 0x80); next_cosine = moh_draft_missing_call(0x8001C9A8, angle + 0x80);
        if (!toggle) { w_u8(packet_first + 3, 11); w_u8(packet_second + 3, 11); }
        else { w_u32(packet_first, 0); w_u32(packet_second, 0); }
        toggle ^= 1; w_u8(packet_first + 7, 0x28); w_u8(packet_second + 7, 0x28);
        value = ((uint32)((sint32)((cosine << 4) + 0x800) >> 12) << 1) + 60; w_u16(packet_second + 8, value); w_u16(packet_first + 8, value);
        value = 44 - (uint32)((sint32)((sine << 4) + 0x800) >> 12); w_u16(packet_second + 10, value); w_u16(packet_first + 10, value);
        value = ((uint32)((sint32)(cosine * 12 + 0x800) >> 12) << 1) + 60; w_u16(packet_second + 12, value); w_u16(packet_first + 12, value);
        value = 44 - (uint32)((sint32)(sine * 12 + 0x800) >> 12); w_u16(packet_second + 14, value); w_u16(packet_first + 14, value);
        value = ((uint32)((sint32)((next_cosine << 4) + 0x800) >> 12) << 1) + 60; w_u16(packet_second + 16, value); w_u16(packet_first + 16, value);
        value = 44 - (uint32)((sint32)((next_sine << 4) + 0x800) >> 12); w_u16(packet_second + 18, value); w_u16(packet_first + 18, value);
        value = ((uint32)((sint32)(next_cosine * 12 + 0x800) >> 12) << 1) + 60; w_u16(packet_second + 20, value); w_u16(packet_first + 20, value);
        value = 44 - (uint32)((sint32)(next_sine * 12 + 0x800) >> 12); w_u16(packet_second + 22, value); w_u16(packet_first + 22, value);
    }
    level2p_sub_8005BE40(buffer_index);
}

void level2p_sub_8005C7F0(void)
{
    uint32 player;
    FUNCTION_MARKER(0x8005C7F0, "LEVEL2P.BIN");
    player = r_u32(0x8009943C); level2p_sub_80046C6C(player);
    player = r_u32(0x80099440); level2p_sub_80046C6C(player);
}

uint32 level2p_sub_8005C838(uint32 kind, uint32 parameter)
{
    uint32 value = 0;
    FUNCTION_MARKER(0x8005C838, "LEVEL2P.BIN");
    if (parameter) value = r_u32(parameter + 4);
    if (kind == 1) return level2p_sub_8007863C();
    if (kind == 0x4C) return level2p_sub_8005C968(parameter, value);
    return 0;
}

void level2p_sub_8005C8A0(uint32 owner)
{
    uint32 player, state, player_state, distance, limit, height, index;
    FUNCTION_MARKER(0x8005C8A0, "LEVEL2P.BIN");
    /* TODO: Original player and state pointers are unchecked */
    for (index = 0; index < 2; ++index) {
        player = r_u32(0x8009943C + index * 4); state = r_u32(owner + 0x388); player_state = r_u32(player + 0x388);
        distance = level2p_sub_800517E8(owner, player); limit = r_u32(state);
        if ((sint32)distance <= (sint32)limit) {
            height = r_u32(player + 0x210); limit = r_u32(state + 4);
            if ((sint32)limit <= (sint32)height) w_u32(player_state + 0x70, limit);
        }
    }
}

uint32 level2p_sub_8005C968(uint32 instance, uint32 parameter)
{
    uint32 previous, owner, flags, state, value, height;
    FUNCTION_MARKER(0x8005C968, "LEVEL2P.BIN");
    previous = r_u32(0x80096878);
    if (previous == instance) w_u32(0x80096878, 0);
    else level2p_sub_8005CAB8();
    owner = level2p_sub_80066934(0, 8, 0xFFFFFFFF, 0, 0, 0, 0, 0);
    if (!owner) { if (parameter) sub_80036E44(parameter); return 0; }
    w_u32(owner + 8, 0x4C); w_u32(owner + 0x10, 0x203);
    w_u32(owner + 0x36C, 0x8005C8A0); w_u32(owner + 0x384, 0x800653D8);
    flags = r_u32(owner + 12); state = r_u32(owner + 0x388);
    w_u32(owner + 0x14, 0); w_u32(owner + 0x370, 0); w_u32(owner + 0x374, 0); w_u32(owner + 0x378, 0); w_u32(owner + 12, flags | 0x20000);
    level2p_sub_8006EE70(owner, instance);
    /* TODO: Original successful allocation path assumes parameter is nonnull */
    value = (uint32)(sint32)r_s16(parameter + 0x2A); w_u32(state, value);
    value = (uint32)(sint32)r_s16(parameter + 0x2C); height = r_u32(owner + 0x210); w_u32(state + 4, height - (value << 16));
    level2p_sub_8006591C(owner); level2p_sub_8004DFBC(owner);
    return owner;
}

void level2p_sub_8005CAB8(void)
{
    uint32 instance, flags, current, value;
    FUNCTION_MARKER(0x8005CAB8, "LEVEL2P.BIN");
    instance = r_u32(0x80096878);
    if (!instance) return;
    flags = r_u32(instance); w_u32(0x80096878, 0);
    if (!(flags & 1)) return;
    current = r_u32(0x80099434);
    /* TODO: Original object list assumes no cycles */
    while (current) {
        value = r_u32(current + 0x1C);
        if (value == instance) { level2p_sub_80066C64(current); return; }
        current = r_u32(current + 4);
    }
}

void level2p_sub_800782D4(uint32 owner)
{
    FUNCTION_MARKER(0x800782D4, "LEVEL2P.BIN");
    w_u32(owner + 0x36C, 0x800767D8); w_u32(owner + 0x370, 0x80077D70); w_u32(owner + 0x374, 0x80078D50);
    w_u32(owner + 0x378, 0x8007774C); w_u32(owner + 0x37C, 0x80078958); w_u32(owner + 0x384, 0x80077F84);
}

uint32 level2p_sub_8007863C(void)
{
    uint32 index, model, owner;
    FUNCTION_MARKER(0x8007863C, "LEVEL2P.BIN");
    /* TODO: Original byte-selected model index is unchecked */
    index = r_u8(0x8009AB8C); model = r_u32(0x8008F920 + index * 68);
    owner = level2p_sub_80066934(3, 0x994, model, 0x147, 0, 0x14, 0, 0);
    if (!owner) return 0;
    w_u32(owner + 8, 1); w_u32(owner + 12, 1); w_u32(owner + 0x14, 0x88204651); w_u8(owner + 0x35, 0); w_u32(owner + 0x204, 0x240000);
    level2p_sub_800782D4(owner);
    if (level2p_sub_800784E4(owner)) return owner;
    sub_80036E44(owner);
    return 0;
}

uint32 level2p_sub_80078320(uint32 owner, uint32 state, uint32 reset)
{
    uint32 index, value, first, second;
    FUNCTION_MARKER(0x80078320, "LEVEL2P.BIN");
    /* TODO: Original state pointer and byte-selected table index are unchecked */
    if (reset) level2p_sub_800665BC(state + 0x288, 0, 2);
    index = r_u8(state + 0x55); value = r_u32(0x8009A668 + index * 16);
    level2p_sub_8006654C(state + 0x288, value, 0x57, 0); level2p_sub_80068A04(state + 0x2DC, owner);
    w_u32(state + 0x2E0, state + 0xA4);
    value = r_u32(owner + 0x270); w_u32(state + 0x218, value);
    value = r_u32(owner + 0x274); w_u32(state + 0x21C, value);
    value = r_u32(owner + 0x278); w_u32(state + 0x220, value);
    index = r_u8(state + 0x55); w_u32(state + 0x1F4, 1); value = r_u32(0x8009A66C + index * 16); w_u32(state + 0x1FC, value);
    index = r_u8(state + 0x55); first = r_u32(0x8009A670 + index * 16); second = r_u32(0x8009A674 + index * 16);
    w_u32(state + 0x208, first); w_u32(state + 0x20C, second);
    return 1;
}

void level2p_sub_80078428(uint32 owner, uint32 state)
{
    uint32 value;
    FUNCTION_MARKER(0x80078428, "LEVEL2P.BIN");
    level2p_sub_80068A04(state + 0x5D8, owner); w_u32(state + 0x5DC, state + 0xA4);
    value = r_u32(owner + 0x270); w_u32(state + 0x514, value);
    value = r_u32(owner + 0x274); w_u32(state + 0x518, value);
    value = r_u32(owner + 0x278); w_u32(state + 0x4F8, 13);
    w_u16(state + 0x504, 0); w_u16(state + 0x506, 0); w_u16(state + 0x508, 0); w_u32(state + 0x51C, value);
}

void level2p_sub_800784A8(uint32 owner, uint32 state)
{
    FUNCTION_MARKER(0x800784A8, "LEVEL2P.BIN");
    (void)owner;
    w_u32(state + 0x634, 13); w_u16(state + 0x638, 0xFF38); w_u16(state + 0x63C, 55);
    w_u16(state + 0x640, 30); w_u16(state + 0x642, 200); w_u16(state + 0x63A, 0); w_u16(state + 0x644, 25);
}

uint32 level2p_sub_800784E4(uint32 owner)
{
    uint32 parity, state, id, resource, value, index, table, pointer, first, second, destination, bytes;
    FUNCTION_MARKER(0x800784E4, "LEVEL2P.BIN");
    parity = r_u32(0x8009AB88); state = r_u32(owner + 0x388); id = (parity & 1) ? 0x96FE167F : 0xB94537A4;
    resource = level2p_sub_80065BF8(id); parity = r_u32(0x8009AB88); w_u32(0x8009AB88, parity + 1);
    resource = level2p_sub_80065BB8(resource); value = level2p_sub_80065C9C(state + 0xA4, resource, 0x85, 0);
    if (!value) return 0;
    /* TODO: Original state/sequence pointers and signed sequence index are unchecked */
    w_u16(state + 0x100, 0); w_u16(state + 0x102, 0); w_u16(state + 0x104, 3);
    index = (uint32)(sint32)r_s16(state + 0x100); table = r_u32(state + 0x11C); pointer = r_u32(table + index * 4);
    value = r_u8(pointer); w_u16(state + 0x660, value - 1);
    first = r_u32(state + 0x13C); second = r_u32(state + 0x140); w_u32(state + 0x144, first); w_u32(state + 0x148, second);
    destination = r_u32(state + 0x12C);
    if (destination) {
        bytes = r_u16(state + 0x116); pointer = r_u32(state + 0x130); moh_draft_missing_call(0x800154EC, pointer, destination, bytes << 3);
    }
    w_u16(state + 0xE2, 4); w_u16(state + 0xE4, 0xFFFF);
    if (!level2p_sub_80078320(owner, state, 0)) return 0;
    level2p_sub_80078428(owner, state); level2p_sub_800784A8(owner, state);
    return 1;
}

void level2p_sub_8005CB34(uint32 instance)
{
    uint32 previous;
    FUNCTION_MARKER(0x8005CB34, "LEVEL2P.BIN");
    previous = r_u32(0x80096878);
    if (previous == instance) return;
    if (previous) { level2p_sub_8005CAB8(); w_u32(0x80096878, 0); }
    w_u32(0x80096878, instance);
}

void level2p_sub_8005CB80(void)
{
    FUNCTION_MARKER(0x8005CB80, "LEVEL2P.BIN");
    moh_draft_missing_call(0x80016B60, 3); moh_draft_missing_call(0x80016CD4, 0);
}

void level2p_sub_8004EB7C(void)
{
    FUNCTION_MARKER(0x8004EB7C, "LEVEL2P.BIN");
    moh_draft_missing_call(0x80015B28, 0x8003A374); moh_draft_missing_call(0x8001B574);
}

void level2p_sub_8005CC20(void)
{
    uint32 event;
    FUNCTION_MARKER(0x8005CC20, "LEVEL2P.BIN");
    moh_draft_missing_call(0x80015B28, 0x8003C200); moh_draft_missing_call(0x8001AB5C, 0xF2000002);
    moh_draft_missing_call(0x80015B28, 0x8003C21C); event = r_u32(0x8008FCEC); moh_draft_missing_call(0x8001D1A0, event);
    moh_draft_missing_call(0x80015B28, 0x8003C240); moh_draft_missing_call(0x80016B60, 3);
    moh_draft_missing_call(0x80015B28, 0x8003C254); moh_draft_missing_call(0x8001CB6C);
    moh_draft_missing_call(0x80015B28, 0x8003C26C); moh_draft_missing_call(0x8001D0F0, 0);
    moh_draft_missing_call(0x80019640, 0); moh_draft_missing_call(0x8001D1B0, 0); moh_draft_missing_call(0x8001D140, 0);
    moh_draft_missing_call(0x8001D180); level2p_sub_8004EB7C(); moh_draft_missing_call(0x8001D160);
    moh_draft_missing_call(0x80015DB8, 0); moh_draft_missing_call(0x80015DB8, 0); moh_draft_missing_call(0x80015B28, 0x8003C288);
}

void level2p_sub_80089004(void)
{
    uint32 remaining = 0x113, address = 0x8009F45C;
    FUNCTION_MARKER(0x80089004, "LEVEL2P.BIN");
    do { w_u32(address, 0); --remaining; address -= 8; } while (remaining);
    w_u32(0x8009EBC8, 0);
}

void level2p_sub_8004EB18(void)
{
    uint32 index;
    FUNCTION_MARKER(0x8004EB18, "LEVEL2P.BIN");
    moh_draft_missing_call(0x8001B228, 0x8008FE80, 0x8008FEA4); moh_draft_missing_call(0x8001B4A8);
    w_u32(0x8008FE64, 0); w_u32(0x8008FE68, 0);
    for (index = 0; index < 10; ++index) { w_u32(0x8008FEE4 + index * 4, 0); w_u32(0x8008FF0C + index * 4, 0); }
}

void level2p_sub_80077BFC(void)
{
    uint32 first, second, text, inverted;
    FUNCTION_MARKER(0x80077BFC, "LEVEL2P.BIN");
    first = r_u32(0x800386D4); second = r_u32(0x800386D8);
    switch (first) {
    case 1: text = 0x8003CFA8; break;
    case 2: text = 0x8003CFC4; break;
    case 3: text = 0x8003CFE0; break;
    case 4: text = 0x8003CFFC; break;
    default: text = 0x8003D018; break;
    }
    moh_draft_missing_call(0x80015B28, text); inverted = r_u32(0x800386A8);
    moh_draft_missing_call(0x80015B28, inverted ? 0x8003D034 : 0x8003D048);
    switch (second) {
    case 1: text = 0x8003D058; break;
    case 2: text = 0x8003D074; break;
    case 3: text = 0x8003D090; break;
    case 4: text = 0x8003D0AC; break;
    default: text = 0x8003D0C8; break;
    }
    moh_draft_missing_call(0x80015B28, text); inverted = r_u32(0x800386BC);
    moh_draft_missing_call(0x80015B28, inverted ? 0x8003D034 : 0x8003D048);
}

void level2p_sub_8005CBB8(void)
{
    FUNCTION_MARKER(0x8005CBB8, "LEVEL2P.BIN");
    level2p_sub_80089004(); level2p_sub_8004EB18(); level2p_sub_8005CBA8(); level2p_sub_8005CB80();
    level2p_sub_80049DF8(); level2p_sub_80041628(); level2p_sub_8005ABD0();
    level2p_sub_8005BF8C(0); level2p_sub_8005BF8C(1); level2p_sub_80077BFC();
}

void level2p_sub_80089030(void)
{
    uint32 count, index = 0, entry = 0x8009EBCC, value, pointer, tail;
    FUNCTION_MARKER(0x80089030, "LEVEL2P.BIN");
    count = r_u32(0x8009EBC8);
    /* TODO: Original signed count and record pointers are unchecked */
    while ((sint32)index < (sint32)count) {
        value = r_u32(entry + 4); --value; w_u32(entry + 4, value);
        if ((sint32)value <= 0) {
            pointer = r_u32(entry); sub_80036E44(pointer); count = r_u32(0x8009EBC8); --count; tail = 0x8009EBCC + count * 8;
            value = r_u32(tail); w_u32(entry, value); value = r_u32(tail + 4); w_u32(0x8009EBC8, count); w_u32(entry + 4, value);
        }
        count = r_u32(0x8009EBC8); ++index; entry += 8;
    }
}

void level2p_sub_8008ACFC(uint32 index)
{
    uint32 id, frame, data;
    FUNCTION_MARKER(0x8008ACFC, "LEVEL2P.BIN");
    /* TODO: Original compartment index is unchecked */
    id = r_u32(0x80094C50 + index * 4); frame = r_u32(0x800968E0); moh_draft_missing_call(0x80015B28, 0x8003D944, id, frame);
    data = 0x80094C6C + (index << 8);
    if (!level2p_sub_8008AA78(data, index)) moh_draft_missing_call(0x80015B28, 0x8003D974);
    level2p_sub_80065C9C(0x8009506C + index * 176, data, 0x16, 0); w_u32(0x800A94C8, 0xFFFFFFFF);
}

void level2p_sub_8005CCF4(void)
{
    uint32 frame, gate, value, pointer, count, player, mode, timer;
    FUNCTION_MARKER(0x8005CCF4, "LEVEL2P.BIN");
    frame = r_u32(0x800968E0); gate = r_u32(0x80099058); w_u32(0x800968E0, frame + 1);
    if (!gate) {
        value = (uint32)(sint32)r_s8(0x8008FE10);
        if (!value) { value = r_u32(0x800A94C8); if (value != 0xFFFFFFFF) level2p_sub_8008ACFC(value); }
        value = r_u32(0x8009F468);
        if (value) {
            value = r_u8(0x8009F490);
            if (!value) {
                value = r_u32(0x8009F46C); pointer = r_u32(0x8009F488);
                /* TODO: Original buffer cursor can address before the buffer base */
                w_u8(pointer + ((sint32)value < 0x5000 ? value - 15 : 0x4FF1), 3);
            }
            pointer = r_u32(0x8009F488); moh_draft_missing_call(0x8001575C, pointer, 0x5000); w_u32(0x8009F468, 0);
        }
    }
    level2p_sub_80089030(); level2p_sub_8004EBA8(); mode = r_u32(0x800968E8);
    if (mode) {
        count = r_u32(0x800968D8); w_u32(0x80096880, 0);
        if ((sint32)count > 3) count = 3;
        w_u32(0x800968D8, count); w_u32(0x800968DC, count);
        do { level2p_sub_8004294C(); if ((sint32)count > 0) level2p_sub_8004EBA8(); --count; } while ((sint32)count >= 0);
    } else {
        count = r_u32(0x800968D8); if ((sint32)count > 3) count = 3;
        w_u32(0x800968D8, count); w_u32(0x800968DC, count);
        do {
            level2p_sub_8004294C(); frame = r_u32(0x800968E4); player = r_u32(0x80099434); w_u32(0x800968E4, frame + 1);
            level2p_sub_80066D94(player); player = r_u32(0x80099434); level2p_sub_80067404(player); level2p_sub_8005B00C();
            count = r_u32(0x800968D8); if ((sint32)count > 0) level2p_sub_8004EBA8();
            value = r_u32(0x80096880);
            if ((sint32)value >= 10) { w_u32(0x80096880, value - 20); level2p_sub_80061410(0x59, 0, 0, 0); }
            count = r_u32(0x800968D8); --count; w_u32(0x800968D8, count);
        } while ((sint32)count >= 0);
        mode = r_u32(0x800968CC); w_u32(0x800968D8, 0);
        if (mode) {
            value = r_u32(0x8008FE5C);
            if (value & 0x100) level2p_sub_8005F984();
            else { value = r_u8(0x8008FE61); if (value & 1) level2p_sub_8005F984(); }
        } else {
            timer = r_u32(0x800968C4);
            if (timer != 0xFFFFFFFF) { frame = r_u32(0x800968E4); if (frame >= timer) level2p_sub_8005F760(); }
        }
    }
    level2p_sub_8005134C(); level2p_sub_80041744(); level2p_sub_80066C08();
}

void level2p_sub_8004E66C(void)
{
    uint32 player, word_offset, byte_offset, port, packet, kind, status, analog, buttons, value, mode, pending, first, second;
    FUNCTION_MARKER(0x8004E66C, "LEVEL2P.BIN");
    for (player = 0; player < 2; ++player) {
        word_offset = player * 4; byte_offset = player * 2; port = player * 16; packet = 0x8008FE80 + player * 36;
        status = r_u8(packet); analog = r_u32(0x8008FE50 + word_offset); buttons = r_u32(0x8008FE5C + word_offset); kind = 0;
        if (!status) { value = r_u8(packet + 1); kind = value >> 4; }
        w_u32(0x8008FE64 + word_offset, buttons); w_u32(0x8008FE78 + word_offset, analog);
        w_u8(0x8008FE74 + player, kind); w_u8(0x8008FE58 + player, 0);
        value = r_u32(0x8008FE48 + word_offset);
        if (!value) {
            w_u32(0x8008FE48 + word_offset, 1); value = moh_draft_missing_call(0x8001B728, port, 1, 0);
            moh_draft_missing_call(0x80015B28, value ? 0x8003A310 : 0x8003A344, player + 1);
        }
        if (kind == 2 || kind == 7) {
            value = r_u32(packet + 4); w_u32(0x8008FE50 + word_offset, value); w_u8(0x8008FE58 + player, 1);
            value = moh_draft_missing_call(0x8001B2D8, port, 2, 0); w_u32(0x8008FECC + word_offset, value);
            value = r_u32(player && value ? 0x800386C4 : 0x800386B0); w_u32(0x8008FECC + word_offset, value);
            mode = moh_draft_missing_call(0x8001B3D8, port);
            if (mode == 1) { w_u32(0x8008FE50 + word_offset, 0x80808080); w_u32(0x8008FE40 + word_offset, 1); }
            value = r_u32(0x8008FE40 + word_offset);
            if (value) {
                moh_draft_missing_call(0x8001B888, port, 0x8008FEC8 + byte_offset, 2);
                if (mode == 2) { w_u32(0x8008FE40 + word_offset, 0); w_u32(0x8008FECC + word_offset, 0); }
                else if (mode == 6) {
                    value = r_u32(player ? 0x800386C4 : 0x800386B0);
                    if (value) {
                        value = moh_draft_missing_call(0x8001B5B8, port, 0x8008FE38);
                        if (value) { w_u32(0x8008FE40 + word_offset, 0); w_u32(0x8008FECC + word_offset, 1); }
                    }
                }
            }
        }
        if (kind == 2 || kind == 4 || kind == 7) {
            pending = player ? 0x80096908 : 0x80096910; value = r_u32(pending);
            if (value) {
                w_u32(pending, 0); value = moh_draft_missing_call(0x8001B728, port, 1, 0);
                moh_draft_missing_call(0x80015B28, value ? 0x8003A310 : 0x8003A344, player + 1);
            }
            first = r_u8(packet + 2); second = r_u8(packet + 3); buttons = ~((first << 8) | second);
            w_u32(0x8008FE6C + word_offset, buttons); w_u32(0x8008FE5C + word_offset, buttons);
        } else if (!kind) {
            w_u32(player ? 0x80096908 : 0x80096910, 1); w_u32(0x8008FE6C + word_offset, 0); w_u32(0x8008FE5C + word_offset, 0);
        }
        value = r_u32(0x8008FEDC + word_offset);
        if (!value) { w_u8(0x8008FEC8 + byte_offset, 0); w_u8(0x8008FEC9 + byte_offset, 0); }
        else {
            --value; w_u32(0x8008FEDC + word_offset, value); first = r_u32(0x8008FED4 + word_offset);
            if (value & first) { value = r_u8(0x8008FEC9 + byte_offset); w_u8(0x8008FEC9 + byte_offset, value >> 1); }
        }
    }
}

void level2p_sub_8004E4D4(uint32 player, uint32 buttons)
{
    uint32 base, index = 9, first, second, third, fourth;
    FUNCTION_MARKER(0x8004E4D4, "LEVEL2P.BIN");
    /* TODO: Original history player index is unchecked */
    base = 0x8008FEE4 + player * 40;
    do {
        first = r_u32(base + (index - 1) * 4); second = r_u32(base + (index - 2) * 4);
        third = r_u32(base + (index - 3) * 4); fourth = r_u32(base + (index - 4) * 4);
        w_u32(base + index * 4, first); w_u32(base + (index - 1) * 4, second);
        w_u32(base + (index - 2) * 4, third); w_u32(base + (index - 3) * 4, fourth); index -= 4;
    } while ((sint32)index >= 4);
    while ((sint32)index > 0) { first = r_u32(base + (index - 1) * 4); w_u32(base + index * 4, first); --index; }
    w_u32(base, buttons);
}

void level2p_sub_8004EBA8(void)
{
    uint32 buttons;
    FUNCTION_MARKER(0x8004EBA8, "LEVEL2P.BIN");
    level2p_sub_8004E66C(); buttons = r_u32(0x8008FE5C); level2p_sub_8004E4D4(0, buttons);
    buttons = r_u32(0x8008FE60); level2p_sub_8004E4D4(1, buttons);
}

void level2p_sub_80065A20(uint32 owner)
{
    uint8 angles[8], translation[16], scale[16];
    uint32 angles_view, translation_view, scale_view, state, matrix, first, second, third, value;
    FUNCTION_MARKER(0x80065A20, "LEVEL2P.BIN");
    /* TODO: Native local mapping and original state pointer remain unresolved */
    angles_view = moh_draft_local_address(angles, sizeof(angles)); translation_view = moh_draft_local_address(translation, sizeof(translation));
    scale_view = moh_draft_local_address(scale, sizeof(scale));
    moh_draft_missing_call(0x800158AC, angles_view, 0, 8); moh_draft_missing_call(0x800158AC, translation_view, 0, 16);
    first = r_u32(0x8003CD48); second = r_u32(0x8003CD4C); third = r_u32(0x8003CD50);
    w_u32(scale_view, first); w_u32(scale_view + 4, second); w_u32(scale_view + 8, third);
    value = r_u32(0x8003CD54); w_u32(scale_view + 12, value);
    state = r_u32(owner + 0x388); w_u16(angles_view, 0); value = (uint32)(sint32)r_s16(owner + 0x22E); matrix = state + 0x154;
    w_u16(angles_view + 2, value); value = (uint32)(sint32)r_s16(owner + 0x232); w_u16(angles_view + 4, value);
    moh_draft_missing_call(0x8001AF98, angles_view, matrix);
    value = (uint32)(sint32)r_s16(state + 0x4E2); w_u32(translation_view, value);
    value = (uint32)(sint32)r_s16(state + 0x4E6); w_u32(translation_view + 4, value);
    value = (uint32)(sint32)r_s16(state + 0x4EA); w_u32(translation_view + 8, value);
    moh_draft_missing_call(0x80016218, matrix, translation_view);
    value = r_u32(owner + 0x270); w_u32(scale_view, (uint32)((sint32)value >> 4));
    value = r_u32(owner + 0x274); w_u32(scale_view + 4, (uint32)((sint32)value >> 4));
    value = r_u32(owner + 0x278); w_u32(scale_view + 8, (uint32)((sint32)value >> 4));
    moh_draft_missing_call(0x800165D8, matrix, scale_view);
}

void level2p_sub_8004E178(uint32 owner)
{
    uint32 handle, pointer = 0, value, tick, stamp, bone, x, old_x, y, old_y, old_z, delta_x, delta_y, delta_z;
    FUNCTION_MARKER(0x8004E178, "LEVEL2P.BIN");
    handle = r_u32(owner + 0x30);
    if (handle) pointer = r_u32(0x80099460 + (handle & 0xFFFF) * 4);
    if (pointer) return;
    value = r_u32(owner + 12); if (value & 0x20000) return;
    tick = r_u32(0x800993FC); stamp = r_u32(owner + 0x3BC);
    if (tick - stamp >= 30) { level2p_sub_8004DFBC(owner); return; }
    bone = (uint32)(sint32)r_s16(owner + 0x2EA);
    if (bone != 0xFFFFFFFF) { level2p_sub_8008C5E4(owner, bone, owner + 0x2E0, 0, 0); return; }
    x = r_u32(owner + 0x20C); old_x = r_u32(owner + 0x258); y = r_u32(owner + 0x210); old_y = r_u32(owner + 0x25C);
    delta_x = (uint32)((sint32)(x - old_x) >> 16); delta_y = (uint32)((sint32)(y - old_y) >> 16);
    value = r_u16(owner + 0x2E0); old_z = r_u32(owner + 0x260); w_u16(owner + 0x2E0, value + delta_x);
    value = r_u16(owner + 0x2E2); x = r_u32(owner + 0x214); delta_z = x - old_z; w_u16(owner + 0x2E2, value + delta_y);
    value = r_u16(owner + 0x2E4); w_u16(owner + 0x2E4, value + (uint32)((sint32)delta_z >> 16));
}

static uint32 moh_update_handle_native(uint32 handle)
{
    return handle ? r_u32(0x80099460 + (handle & 0xFFFF) * 4) : 0;
}

void level2p_sub_80066D94(uint32 head)
{
    uint32 pending[16], depth = 0;
    uint8 matrix[32], angles[8];
    uint32 projected[3];
    uint32 current = head, next, marker, seen, parent, other, handle, flags, value, first, second, third, y, z, dx, dy, dz, callback, result, bone, model;
    uint32 matrix_view = 0, angles_view = 0, projected_view = 0;
    FUNCTION_MARKER(0x80066D94, "LEVEL2P.BIN");
    value = r_u32(0x800993FC); w_u32(0x800993FC, value + 1);
    /* TODO: Native local mapping and original vector/matrix padding remain unresolved */
    /* TODO: Original lists, parent chains, handle indices and sixteen pending entries are unchecked */
    while (current) {
        marker = r_u32(0x800993FC); next = r_u32(current + 4); seen = r_u32(current + 0x28);
        if (seen != marker) {
            for (;;) {
                handle = r_u32(current + 0x2C); parent = moh_update_handle_native(handle);
                handle = r_u32(current + 0x2C); other = moh_update_handle_native(handle);
                if (!other) { marker = r_u32(0x800993FC); break; }
                seen = r_u32(parent + 0x28); marker = r_u32(0x800993FC);
                if (seen == marker) break;
                pending[depth++] = current; current = parent;
            }
            w_u32(current + 0x28, marker); level2p_sub_8004B180(current);
            handle = r_u32(current + 0x2C); parent = moh_update_handle_native(handle);
            if (parent) {
                handle = r_u32(current + 0x2C); parent = moh_update_handle_native(handle); flags = r_u32(current + 0x18);
                value = 0;
                if (flags & 1) value = r_u32(parent + 12) & 0x80000;
                if (!(flags & 1) || value) { w_u32(current + 0x18, 0); level2p_sub_80068A04(current + 0x2C, 0); }
                else if (!(flags & 0x20)) {
                    handle = r_u32(current + 0x2C); parent = moh_update_handle_native(handle);
                    first = r_u32(parent + 0x20C); second = r_u32(parent + 0x258); value = r_u32(current + 0x1F0); w_u32(current + 0x1F0, value + first - second);
                    first = r_u32(parent + 0x210); second = r_u32(parent + 0x25C); value = r_u32(current + 0x1F4); w_u32(current + 0x1F4, value + first - second);
                    first = r_u32(parent + 0x214); second = r_u32(parent + 0x260); value = r_u32(current + 0x1F8); w_u32(current + 0x1F8, value + first - second);
                    first = r_u32(parent + 0x228); second = r_u32(parent + 0x264); y = r_u32(parent + 0x22C); z = r_u32(parent + 0x230);
                    dx = first - second; second = r_u32(parent + 0x268); third = r_u32(parent + 0x26C); dy = y - second; dz = z - third;
                    if (dx || dy || dz) {
                        if (!matrix_view) matrix_view = moh_draft_local_address(matrix, sizeof(matrix));
                        if (!angles_view) angles_view = moh_draft_local_address(angles, sizeof(angles));
                        if (!projected_view) projected_view = moh_draft_local_address(projected, sizeof(projected));
                        w_u16(angles_view, (uint32)((sint32)dx >> 16)); w_u16(angles_view + 2, (uint32)((sint32)dy >> 16)); w_u16(angles_view + 4, (uint32)((sint32)dz >> 16));
                        moh_draft_missing_call(0x8001676C, angles_view, matrix_view);
                        w_u32(matrix_view + 28, 0); w_u32(matrix_view + 24, 0); w_u32(matrix_view + 20, 0);
                        first = r_u32(matrix_view); second = r_u32(matrix_view + 4); xport_gte_write_control(0, first); xport_gte_write_control(1, second);
                        first = r_u32(matrix_view + 8); second = r_u32(matrix_view + 12); third = r_u32(matrix_view + 16);
                        xport_gte_write_control(2, first); xport_gte_write_control(3, second); xport_gte_write_control(4, third);
                        first = r_u32(matrix_view + 20); second = r_u32(matrix_view + 24); xport_gte_write_control(5, first);
                        third = r_u32(matrix_view + 28); xport_gte_write_control(6, second); xport_gte_write_control(7, third);
                        first = r_u32(current + 0x20C); second = r_u32(parent + 0x258); w_u16(angles_view, (uint32)((sint32)(first - second) >> 16));
                        first = r_u32(current + 0x210); second = r_u32(parent + 0x25C); w_u16(angles_view + 2, (uint32)((sint32)(first - second) >> 16));
                        first = r_u32(current + 0x214); second = r_u32(parent + 0x260); w_u16(angles_view + 4, (uint32)((sint32)(first - second) >> 16));
                        first = r_u32(angles_view); xport_gte_write_data(0, first); second = r_u32(angles_view + 4); xport_gte_write_data(1, second); xport_gte_execute(0x486012);
                        value = xport_gte_read_data(25); w_u32(projected_view, value); value = xport_gte_read_data(26); w_u32(projected_view + 4, value);
                        value = xport_gte_read_data(27); w_u32(projected_view + 8, value);
                        first = r_u32(current + 0x228); second = r_u32(current + 0x230); w_u32(current + 0x228, first + dx);
                        first = r_u32(current + 0x22C); w_u32(current + 0x230, second + dz); w_u32(current + 0x22C, first + dy);
                        first = (uint32)(sint32)r_s16(angles_view); second = r_u32(projected_view); value = r_u32(current + 0x1F0); w_u32(current + 0x1F0, value + ((second - first) << 16));
                        first = (uint32)(sint32)r_s16(angles_view + 2); second = r_u32(projected_view + 4); value = r_u32(current + 0x1F4); w_u32(current + 0x1F4, value + ((second - first) << 16));
                        first = (uint32)(sint32)r_s16(angles_view + 4); second = r_u32(projected_view + 8); value = r_u32(current + 0x1F8); w_u32(current + 0x1F8, value + ((second - first) << 16));
                    }
                    flags = r_u32(current + 0x18); w_u32(current + 0x18, flags & 0xFFFFFFFB);
                }
            }
            callback = r_u32(current + 0x36C);
            if (callback) {
                moh_draft_indirect_call(callback, current); handle = r_u32(current + 0x30);
                w_u32(current + 0x38, 0); w_u32(current + 0x3C, 0); w_u32(current + 0x40, 0); parent = moh_update_handle_native(handle);
                if (parent) {
                    handle = r_u32(current + 0x30); parent = moh_update_handle_native(handle); value = level2p_sub_80068A64(current + 0x30);
                    flags = 0; if (!value) flags = r_u32(parent + 12) & 0x80000;
                    if (value || flags) level2p_sub_80068A04(current + 0x30, 0);
                    else {
                        handle = r_u32(current + 0x30); parent = moh_update_handle_native(handle); handle = r_u32(current + 0x30); other = moh_update_handle_native(handle);
                        bone = (uint32)(sint32)r_s8(current + 0x34); model = r_u32(parent + 0x100);
                        if (!matrix_view) matrix_view = moh_draft_local_address(matrix, sizeof(matrix));
                        level2p_sub_8008BCF0(other, model, bone, matrix_view, 0, 0);
                        value = (uint32)(sint32)r_s16(matrix_view); w_u32(current + 0x20C, value << 16);
                        value = (uint32)(sint32)r_s16(matrix_view + 2); w_u32(current + 0x210, value << 16);
                        value = (uint32)(sint32)r_s16(matrix_view + 4); w_u32(current + 0x214, value << 16);
                    }
                }
                handle = r_u32(current + 0x30); parent = moh_update_handle_native(handle);
                if (!parent) {
                    flags = r_u32(current + 12);
                    if (!(flags & 0x20000)) {
                        result = level2p_sub_80067924(current); flags = r_u32(current + 12);
                        if (flags & 5) level2p_sub_80083550(current);
                        if (result) { callback = r_u32(current + 0x378); if (callback) moh_draft_indirect_call(callback, current, result); }
                    }
                }
                flags = r_u32(current + 12);
                if (!(flags & 0x80000)) { value = r_u32(current + 0x38C); if (value) level2p_sub_80041104(current); }
            }
            flags = r_u32(current + 0x18); if (!(flags & 2)) w_u32(current + 0x18, flags & 0xFFFFFFFE);
            level2p_sub_8006591C(current); flags = r_u32(current + 12);
            if (flags & 1) { level2p_sub_80065A20(current); level2p_sub_800775E4(current); }
            level2p_sub_8004E178(current);
        }
        value = r_u32(current + 8); if (value == 2) level2p_sub_80049B20(current);
        current = depth ? pending[--depth] : next;
    }
}

void level2p_sub_800775E4(uint32 owner)
{
    uint32 flags, state, kind, value, index, first, second, parameter, magnitude;
    FUNCTION_MARKER(0x800775E4, "LEVEL2P.BIN");
    flags = r_u32(owner + 0x38); state = r_u32(owner + 0x388);
    /* TODO: Original state pointer and signed player/table indices are unchecked */
    if (!(flags & 0x4000)) {
        kind = r_u8(state + 0x55);
        if (kind == 3 || kind == 10) {
            value = level2p_sub_8005A3E4(1, owner); index = r_u8(state + 0x55) * 68;
            first = (uint32)(sint32)r_s16(0x8008F92C + index); second = (uint32)(sint32)r_s16(0x8008F92E + index); parameter = r_u32(state + 0x6F0);
            level2p_sub_80079AC0(owner, first, second, value & 0xFF, parameter);
        } else if (kind == 7) {
            value = level2p_sub_8005A3E4(1, owner);
            if (value) { value = r_u32(state + 0x40); magnitude = (sint32)value < 0 ? 0 - value : value; w_u32(state + 0x44, value); w_u32(state + 0x48, magnitude); }
        } else {
            index = (uint32)(sint32)r_s8(state + 0x54); index = r_u8(0x8009AB8C + index); flags = r_u16(0x8008F94A + index * 68);
            if (!(flags & 1)) {
                value = level2p_sub_8005A3E4(1, owner); index = r_u8(state + 0x55) * 68;
                first = (uint32)(sint32)r_s16(0x8008F92C + index); second = (uint32)(sint32)r_s16(0x8008F92E + index);
                level2p_sub_8007A150(owner, first, second, value & 0xFF);
            }
        }
    }
    level2p_sub_8005A1B8(owner, 1, 0);
}

void level2p_sub_80083550(uint32 owner)
{
    uint32 flags;
    FUNCTION_MARKER(0x80083550, "LEVEL2P.BIN");
    flags = r_u32(owner + 12); if (flags & 0x51) level2p_sub_800833A8(owner);
}

void level2p_sub_800833A8(uint32 owner)
{
    uint8 point[8];
    uint32 value, index, flags, pointer, state, table, entry, end, count, bit, result = 0, descriptor, data, check, point_view;
    FUNCTION_MARKER(0x800833A8, "LEVEL2P.BIN");
    /* TODO: Native local mapping and original point padding remain unresolved */
    /* TODO: Original table/list pointers and signed indices are unchecked */
    point_view = moh_draft_local_address(point, sizeof(point));
    value = (uint32)(sint32)r_s16(owner + 0x20E); index = r_u32(owner + 0x390); flags = r_u32(owner + 12); w_u16(point_view, value);
    value = (uint32)(sint32)r_s16(owner + 0x212); w_u16(point_view + 2, value); value = (uint32)(sint32)r_s16(owner + 0x216); w_u16(point_view + 4, value);
    pointer = r_u32(0x80099430); state = r_u32(pointer + 0x388); table = r_u32(state + 0x14);
    if (index != 0xFFFF) {
        pointer = r_u32(0x8009DF9C); pointer = r_u32(pointer + index * 4); entry = pointer + 2; count = r_u8(pointer); count = 255 - count; end = entry + count * 2;
        if (entry != end) {
            bit = 1u << (count & 31);
            do {
                value = (uint32)(sint32)r_s16(entry); descriptor = table + value * 8; data = r_u32(descriptor + 4); value = r_u32(data + 12); check = 0;
                if (flags & 1) {
                    if (!value) check = 1;
                    else {
                        value = r_u32(data + 0x38);
                        if (value) check = 1;
                        else { value = r_u32(data + 0x3C); if (value) check = 1; else { value = r_u32(data + 0x4C); check = value != 0; } }
                    }
                } else if (value) {
                    value = r_u32(data + 0x40); if (value) check = 1; else { value = r_u32(data + 0x44); check = value != 0; }
                }
                if (check && level2p_sub_80082CD8(point_view, 0, descriptor)) result |= bit;
                entry += 2; result >>= 1;
            } while (entry != end);
        }
    }
    w_u32(owner + 0x394, result); level2p_sub_80083088(owner);
    index = r_u32(owner + 0x390); value = r_u32(owner + 0x394); w_u32(owner + 0x398, index); w_u32(owner + 0x39C, value);
}

void level2p_sub_80083088(uint32 owner)
{
    uint32 current, previous, pointers, new_list, old_list, current_list, pointer, state, table, new_end, old_end, new_mask, old_mask, changed, entering, leaving, first, second, value, old_value;
    FUNCTION_MARKER(0x80083088, "LEVEL2P.BIN");
    /* TODO: Original sentinel indices are dereferenced before checks; list reads can precede end checks */
    current = r_u32(owner + 0x390); pointers = r_u32(0x8009DF9C); previous = r_u32(owner + 0x398);
    new_list = r_u32(pointers + current * 4); pointer = r_u32(0x80099430); old_list = r_u32(pointers + previous * 4);
    state = r_u32(pointer + 0x388); table = r_u32(state + 0x14); current_list = new_list + 2;
    if (current == 0xFFFF) {
        if (previous == current) return;
        leaving = r_u32(owner + 0x39C); old_list += 2;
        while (leaving) { if (leaving & 1) { value = (uint32)(sint32)r_s16(old_list); level2p_sub_80083014(owner, table + value * 8); } leaving >>= 1; old_list += 2; }
        return;
    }
    if (previous == 0xFFFF) {
        entering = r_u32(owner + 0x394);
        while (entering) { if (entering & 1) { value = (uint32)(sint32)r_s16(current_list); level2p_sub_80082E98(owner, table + value * 8); } entering >>= 1; current_list += 2; }
        return;
    }
    if (current == previous) {
        first = r_u32(owner + 0x394); second = r_u32(owner + 0x39C); changed = first ^ second; leaving = second & changed; entering = first & changed; old_list = current_list;
        while (leaving) { if (leaving & 1) { value = (uint32)(sint32)r_s16(old_list); level2p_sub_80083014(owner, table + value * 8); } leaving >>= 1; old_list += 2; }
        while (entering) { if (entering & 1) { value = (uint32)(sint32)r_s16(current_list); level2p_sub_80082E98(owner, table + value * 8); } entering >>= 1; current_list += 2; }
        return;
    }
    new_mask = 1; old_mask = 1; pointer = old_list + 2;
    first = r_u8(new_list); second = r_u8(old_list); new_end = current_list + (255 - first) * 2; old_end = pointer + (255 - second) * 2;
    for (;;) {
        value = (uint32)(sint32)r_s16(current_list); old_value = (uint32)(sint32)r_s16(pointer);
        if (current_list == new_end || pointer == old_end) break;
        if (value == old_value) {
            first = r_u32(owner + 0x394); second = r_u32(owner + 0x39C);
            if (((first & new_mask) != 0) != ((second & old_mask) != 0)) {
                if (second & old_mask) level2p_sub_80083014(owner, table + value * 8);
                else level2p_sub_80082E98(owner, table + value * 8);
            }
        }
        if (value <= old_value) { current_list += 2; new_mask <<= 1; }
        if (old_value <= value) { pointer += 2; old_mask <<= 1; }
    }
    if (current_list == new_end) {
        while (pointer != old_end) {
            second = r_u32(owner + 0x39C);
            if (second & old_mask) { value = (uint32)(sint32)r_s16(pointer); level2p_sub_80083014(owner, table + value * 8); }
            pointer += 2; old_mask <<= 1;
        }
    }
    while (current_list != new_end) {
        first = r_u32(owner + 0x394);
        if (first & new_mask) { value = (uint32)(sint32)r_s16(current_list); level2p_sub_80082E98(owner, table + value * 8); }
        current_list += 2; new_mask <<= 1;
    }
}

uint32 level2p_sub_80082930(uint32 point, uint32 radius, uint32 center, uint32 shape_radius)
{
    uint32 first, second, y, z, x, center_y, center_z, mac_x, mac_y, mac_z, square;
    FUNCTION_MARKER(0x80082930, "LEVEL2P.BIN");
    first = (uint32)(sint32)r_s16(point); second = (uint32)(sint32)r_s16(center);
    y = (uint32)(sint32)r_s16(point + 2); z = (uint32)(sint32)r_s16(point + 4); x = first - second;
    center_y = (uint32)(sint32)r_s16(center + 2); center_z = (uint32)(sint32)r_s16(center + 4);
    xport_gte_write_data(9, x); xport_gte_write_data(10, y - center_y); xport_gte_write_data(11, z - center_z); xport_gte_execute(0xA00428);
    radius += shape_radius; square = radius * radius;
    mac_x = xport_gte_read_data(25); mac_y = xport_gte_read_data(26); mac_z = xport_gte_read_data(27);
    return (sint32)(square - (mac_x + mac_y + mac_z)) > 0;
}

uint32 level2p_sub_80082998(uint32 point, uint32 center, uint32 radius, uint32 first_height, uint32 second_height)
{
    uint32 lower, upper, temporary, first, second, x, z, dy;
    FUNCTION_MARKER(0x80082998, "LEVEL2P.BIN");
    lower = 0 - first_height; upper = 0 - second_height;
    if ((sint32)upper < (sint32)lower) { temporary = lower; lower = upper; upper = temporary; }
    first = (uint32)(sint32)r_s16(point + 2); second = (uint32)(sint32)r_s16(center + 2); dy = first - second;
    if ((sint32)dy < (sint32)lower || (sint32)dy > (sint32)upper) return 0;
    first = (uint32)(sint32)r_s16(point); second = (uint32)(sint32)r_s16(center);
    z = (uint32)(sint32)r_s16(point + 4); temporary = (uint32)(sint32)r_s16(center + 4); x = first - second; z -= temporary;
    if ((sint32)(0 - x) > (sint32)radius || (sint32)x > (sint32)radius || (sint32)(0 - z) > (sint32)radius || (sint32)z > (sint32)radius) return 0;
    return (sint32)(radius * radius - (x * x + z * z)) > 0;
}

uint32 level2p_sub_80082A60(uint32 point, uint32 descriptor)
{
    uint8 center[8], angles[8], matrix[32];
    uint32 projected[3];
    uint32 data, extent_x, extent_y, extent_z, first, second, third, cx, cy, cz, x, y, z, dx, dy, dz;
    uint32 center_view, angles_view, matrix_view, projected_view;
    FUNCTION_MARKER(0x80082A60, "LEVEL2P.BIN");
    /* TODO: Original descriptor pointers and native local mapping/padding remain unresolved */
    center_view = moh_draft_local_address(center, sizeof(center));
    data = r_u32(descriptor + 4); extent_x = (uint32)(sint32)r_s16(data + 0x2A); extent_y = (uint32)(sint32)r_s16(data + 0x2C); extent_z = (uint32)(sint32)r_s16(data + 0x2E);
    cx = r_u16(data + 0x10); w_u16(center_view, cx); data = r_u32(descriptor + 4); cy = r_u16(data + 0x12); w_u16(center_view + 2, cy);
    data = r_u32(descriptor + 4); cz = r_u16(data + 0x14); w_u16(center_view + 4, cz);
    y = (uint32)(sint32)r_s16(point + 2); dy = y - (uint32)(sint32)(sint16)cy;
    z = (uint32)(sint32)r_s16(point + 4); x = (uint32)(sint32)r_s16(point); dz = z - (uint32)(sint32)(sint16)cz;
    dx = moh_draft_missing_call(0x8001573C, x - (uint32)(sint32)(sint16)cx);
    dy = moh_draft_missing_call(0x8001573C, dy); dz = moh_draft_missing_call(0x8001573C, dz);
    first = moh_draft_missing_call(0x8001573C, extent_x); extent_x = (uint32)((sint32)first >> 1);
    first = moh_draft_missing_call(0x8001573C, extent_y); extent_y = (uint32)((sint32)first >> 1);
    first = moh_draft_missing_call(0x8001573C, extent_z); extent_z = (uint32)((sint32)first >> 1);
    if ((sint32)dx > (sint32)extent_x || (sint32)dy > (sint32)extent_y || (sint32)dz > (sint32)extent_z) return 0;
    angles_view = moh_draft_local_address(angles, sizeof(angles));
    data = r_u32(descriptor + 4); first = r_u16(data + 0x18); x = 0 - first; w_u16(angles_view, x);
    data = r_u32(descriptor + 4); first = r_u16(data + 0x1A); y = 0 - first; w_u16(angles_view + 2, y);
    data = r_u32(descriptor + 4); first = r_u16(data + 0x1C); z = 0 - first; w_u16(angles_view + 4, z);
    if (!(sint16)x && !(sint16)y && !(sint16)z) return 1;
    matrix_view = moh_draft_local_address(matrix, sizeof(matrix)); projected_view = moh_draft_local_address(projected, sizeof(projected));
    moh_draft_missing_call(0x8001AF98, angles_view, matrix_view);
    first = r_u32(matrix_view); second = r_u32(matrix_view + 4); xport_gte_write_control(0, first); xport_gte_write_control(1, second);
    first = r_u32(matrix_view + 8); second = r_u32(matrix_view + 12); third = r_u32(matrix_view + 16);
    xport_gte_write_control(2, first); xport_gte_write_control(3, second); xport_gte_write_control(4, third);
    first = r_u16(center_view); second = r_u16(point); w_u16(center_view, first - second);
    first = r_u16(center_view + 2); second = r_u16(point + 2); w_u16(center_view + 2, first - second);
    first = r_u16(center_view + 4); second = r_u16(point + 4); w_u16(center_view + 4, first - second);
    first = r_u32(center_view); xport_gte_write_data(0, first); second = r_u32(center_view + 4); xport_gte_write_data(1, second); xport_gte_execute(0x486012);
    first = xport_gte_read_data(25); w_u32(projected_view, first); first = xport_gte_read_data(26); w_u32(projected_view + 4, first); first = xport_gte_read_data(27); w_u32(projected_view + 8, first);
    first = r_u32(projected_view); dx = moh_draft_missing_call(0x8001573C, first);
    first = r_u32(projected_view + 4); dy = moh_draft_missing_call(0x8001573C, first);
    first = r_u32(projected_view + 8); dz = moh_draft_missing_call(0x8001573C, first);
    return (sint32)dx <= (sint32)extent_x && (sint32)dy <= (sint32)extent_y && (sint32)dz <= (sint32)extent_z;
}

uint32 level2p_sub_80082CD8(uint32 point, uint32 parameter, uint32 descriptor)
{
    uint32 data, kind, first, second, third, value;
    FUNCTION_MARKER(0x80082CD8, "LEVEL2P.BIN");
    /* TODO: Original descriptor and shape pointers are unchecked */
    data = r_u32(descriptor + 4); kind = (uint32)(sint32)r_s16(data + 0x28);
    if (!kind) { first = (uint32)(sint32)r_s16(data + 0x2A); value = level2p_sub_80082930(point, parameter, data + 0x10, first); }
    else if (kind == 1) {
        first = (uint32)(sint32)r_s16(data + 0x2A); second = (uint32)(sint32)r_s16(data + 0x2C); third = (uint32)(sint32)r_s16(data + 0x2E);
        value = level2p_sub_80082998(point, data + 0x10, first, second, third);
    } else if (kind == 2) value = level2p_sub_80082A60(point, descriptor);
    else return 0;
    return value != 0;
}

void level2p_sub_80082D90(uint32 owner, uint32 descriptor)
{
    uint32 data, pointer, count, message;
    FUNCTION_MARKER(0x80082D90, "LEVEL2P.BIN");
    data = r_u32(descriptor + 4); pointer = r_u32(data + 0x38); if (!pointer) return;
    /* TODO: Original count and message list span are unchecked */
    count = r_u32(pointer); pointer += 4;
    while (count) { --count; message = r_u32(pointer); pointer += 4; level2p_sub_80061410(message, 0, descriptor, owner); }
}

void level2p_sub_80082E14(uint32 owner, uint32 descriptor)
{
    uint32 data, pointer, count, message;
    FUNCTION_MARKER(0x80082E14, "LEVEL2P.BIN");
    data = r_u32(descriptor + 4); pointer = r_u32(data + 0x40); if (!pointer) return;
    /* TODO: Original count and message list span are unchecked */
    count = r_u32(pointer); pointer += 4;
    while (count) { --count; message = r_u32(pointer); pointer += 4; level2p_sub_80061410(message, 0, descriptor, owner); }
}

void level2p_sub_80082F0C(uint32 owner, uint32 descriptor)
{
    uint32 data, pointer, count, message;
    FUNCTION_MARKER(0x80082F0C, "LEVEL2P.BIN");
    data = r_u32(descriptor + 4); pointer = r_u32(data + 0x3C); if (!pointer) return;
    /* TODO: Original count and message list span are unchecked */
    count = r_u32(pointer); pointer += 4;
    while (count) { --count; message = r_u32(pointer); pointer += 4; level2p_sub_80061410(message, 0, descriptor, owner); }
}

void level2p_sub_80082F90(uint32 owner, uint32 descriptor)
{
    uint32 data, pointer, count, message;
    FUNCTION_MARKER(0x80082F90, "LEVEL2P.BIN");
    data = r_u32(descriptor + 4); pointer = r_u32(data + 0x44); if (!pointer) return;
    /* TODO: Original count and message list span are unchecked */
    count = r_u32(pointer); pointer += 4;
    while (count) { --count; message = r_u32(pointer); pointer += 4; level2p_sub_80061410(message, 0, descriptor, owner); }
}

void level2p_sub_8006F0F8(uint32 descriptor)
{
    uint32 data, kind;
    FUNCTION_MARKER(0x8006F0F8, "LEVEL2P.BIN");
    data = r_u32(descriptor + 4); kind = r_u32(data);
    if (kind != 0x8877B646 && kind < 0x8877B646 && kind == 0x3011B913) level2p_sub_8005CB34(descriptor);
}

void level2p_sub_8006F144(uint32 descriptor)
{
    uint32 data, kind, flags;
    FUNCTION_MARKER(0x8006F144, "LEVEL2P.BIN");
    data = r_u32(descriptor + 4); kind = r_u32(data);
    if (kind == 0x4D078F23) level2p_sub_80053DD0(data);
    else if (kind < 0x4D078F23) { if (kind != 0x271BD574 && kind == 0x3011B913) level2p_sub_8006EF50(0x4C, descriptor); }
    else if (kind == 0xD1BA7D1E) { flags = r_u32(0x80098B8C); w_u32(0x80098B8C, flags | 0x24); }
}

void level2p_sub_80082E98(uint32 owner, uint32 descriptor)
{
    uint32 flags, data, value;
    FUNCTION_MARKER(0x80082E98, "LEVEL2P.BIN");
    flags = r_u32(owner + 12);
    if (flags & 1) {
        data = r_u32(descriptor + 4); value = r_u32(data + 12);
        if (!value) level2p_sub_8006F144(descriptor); else level2p_sub_80082D90(owner, descriptor);
    } else if (flags & 0x50) level2p_sub_80082E14(owner, descriptor);
}

void level2p_sub_80083014(uint32 owner, uint32 descriptor)
{
    uint32 data, value, flags;
    FUNCTION_MARKER(0x80083014, "LEVEL2P.BIN");
    data = r_u32(descriptor + 4); value = r_u32(data + 12);
    if (!value) level2p_sub_8006F0F8(descriptor);
    else { flags = r_u32(owner + 12); if (flags & 1) level2p_sub_80082F0C(owner, descriptor); else if (flags & 0x50) level2p_sub_80082F90(owner, descriptor); }
}

uint32 level2p_sub_8006EF50(uint32 kind, uint32 descriptor)
{
    uint32 flags, owner, data, value;
    FUNCTION_MARKER(0x8006EF50, "LEVEL2P.BIN");
    /* TODO: Original descriptor and resource pointers are unchecked */
    flags = r_u32(descriptor); if (flags & 1) return 0;
    owner = level2p_sub_80068740(kind, descriptor); if (!owner) return 0;
    level2p_sub_80066BA8(owner); flags = r_u32(descriptor); w_u32(descriptor, flags | 1); w_u32(owner + 0x1C, descriptor);
    data = r_u32(descriptor + 4); value = r_u32(data + 12);
    if (value) level2p_sub_80061700(owner, 0);
    level2p_sub_8006EE70(owner, descriptor);
    return owner;
}

uint32 level2p_sub_8006FDCC(uint32 group, uint32 index)
{
    uint32 offset, required, current, forbidden;
    FUNCTION_MARKER(0x8006FDCC, "LEVEL2P.BIN");
    /* TODO: Original requirement table indices are unchecked */
    offset = (group * 9 + index) * 4; required = r_u32(0x800B6028 + offset);
    current = r_u32(0x8008FE5C + group * 4); forbidden = r_u32(0x800B6078 + offset);
    return (current & required) == required && (!forbidden || (current & forbidden) != forbidden);
}

uint32 level2p_sub_8005A3E4(uint32 field, uint32 owner)
{
    uint8 delta[8], direction[8], normal[8], fraction[8];
    uint32 start[3], end[3], player, pointer, value, first, second, third, bit, delta_view, direction_view, start_view, end_view, normal_view, fraction_view;
    uint32 current, count, distance, voice, row;
    sint32 vertical, angle;
    FUNCTION_MARKER(0x8005A3E4, "LEVEL2P.BIN");
    if (field - 0x60 >= 32)
    {
        if (!(field & 0x1C0)) return r_u32(owner + (field << 2) + 0x4C);
        if (field & 0x100)
        {
            bit = field ^ 0x100; value = r_u32(owner + ((uint32)((sint32)bit >> 5) << 2) + 0xEC);
        }
        else if (field & 0x40) { value = r_u32(owner + 12); bit = field - 0x40; }
        else if (field & 0x80) { value = r_u32(owner + 16); bit = field - 0x80; }
        else return 0;
        return (value & (1u << (bit & 31))) != 0;
    }
    switch (field)
    {
    case 0x60:
        player = r_u32(0x8009943C); pointer = r_u32(player + 0x100); pointer = r_u32(pointer + 8); return r_u16(pointer + 200);
    case 0x61:
        player = r_u32(0x8009943C); return r_u32(player + 16) & 0x400;
    case 0x62: return owner + 0x3A4;
    case 0x63:
        player = r_u32(0x8009943C); first = (uint32)(sint32)r_s16(owner + 0x20E); second = (uint32)(sint32)r_s16(player + 0x20E);
        /* TODO: Semantic vectors preserve original opaque padding through fail-fast local mapping */
        delta_view = moh_draft_local_address(delta, sizeof(delta)); direction_view = moh_draft_local_address(direction, sizeof(direction));
        w_u16(delta_view + 2, 0); w_u16(delta_view, second - first);
        first = (uint32)(sint32)r_s16(player + 0x216); second = (uint32)(sint32)r_s16(owner + 0x216); w_u16(delta_view + 4, first - second);
        value = r_u16(owner + 0x344); w_u16(direction_view + 2, 0); w_u16(direction_view, value);
        value = r_u16(owner + 0x350); w_u16(direction_view + 4, value);
        first = r_u32(delta_view); second = r_u32(delta_view + 4); xport_gte_write_control(0, first); xport_gte_write_control(1, second);
        first = r_u16(direction_view); second = r_u16(direction_view + 2); third = r_u16(direction_view + 4);
        xport_gte_write_data(9, first); xport_gte_write_data(10, second); xport_gte_write_data(11, third); xport_gte_execute(0x41E012);
        return xport_gte_read_data(25) >> 31;
    case 0x64: return r_u32(owner + 0x29C);
    case 0x65:
        player = r_u32(0x8009943C); pointer = r_u32(player + 0x388); return r_u8(pointer + 0x55);
    case 0x66:
        player = r_u32(0x8009943C); return (uint32)(sint32)r_s16(player + 0x11E);
    case 0x67: return level2p_sub_8006FDCC(0, 0);
    case 0x68:
        voice = r_u32(owner + 0xFC); if (voice - 1 >= 23) return 0;
        row = 0x8008D440 + voice * 52; if (r_u8(row + 14)) return 0;
        if (!(r_u8(0x8008D3B4 + voice) & 1)) return 0;
        if (r_u16(row + 40) >= 120) return 0;
        return (r_u32(row) & 1) == 0;
    case 0x69: return r_u32(0x800942C0);
    case 0x6B:
        player = r_u32(0x8009943C); return r_u32(player + 16) & 0x800;
    case 0x6C:
        player = r_u32(0x8009943C); pointer = r_u32(player + 0x388); return (r_u32(pointer) >> 1) & 1;
    case 0x6D:
        current = r_u32(0x80099434); count = 0;
        /* TODO: Original object walk has no cycle bound */
        delta_view = 0;
        while (current)
        {
            if (r_u32(current + 8) == 0x1402 && current != owner && !(r_u32(current + 12) & 0x20080000))
            {
                if (!delta_view) delta_view = moh_draft_local_address(delta, sizeof(delta));
                first = r_u32(owner + 0x20C); second = r_u32(current + 0x20C); w_u16(delta_view, (uint32)((sint32)(first - second) >> 16));
                first = r_u32(owner + 0x210); second = r_u32(current + 0x210); w_u16(delta_view + 2, (uint32)((sint32)(first - second) >> 16));
                first = r_u32(owner + 0x214); second = r_u32(current + 0x214); w_u16(delta_view + 4, (uint32)((sint32)(first - second) >> 16));
                distance = level2p_sub_800548C4(delta_view);
                if ((uint16)distance < 3000) { vertical = r_s16(delta_view + 2); if (vertical < 0) vertical = -vertical; if (vertical < 1000) ++count; }
            }
            current = r_u32(current + 4);
        }
        return count;
    case 0x6E: return 0x8009445C;
    case 0x6F:
        player = r_u32(0x8009943C); first = (uint32)(sint32)r_s16(owner + 0x212); second = (uint32)(sint32)r_s16(player + 0x212); return first - second;
    case 0x70:
        pointer = r_u32(owner + 0x100); pointer = r_u32(pointer + 8); return r_u32(pointer);
    case 0x71:
        player = r_u32(0x8009943C); first = level2p_sub_8003FB68(player, owner);
        player = r_u32(0x8009943C); second = level2p_sub_8003FB68(player, owner); angle = (sint16)second;
        if ((sint16)first < 0) angle = -angle;
        if (angle >= 600) return 0;
        player = r_u32(0x8009943C);
        start_view = moh_draft_local_address(start, sizeof(start)); end_view = moh_draft_local_address(end, sizeof(end));
        normal_view = moh_draft_local_address(normal, sizeof(normal)); fraction_view = moh_draft_local_address(fraction, sizeof(fraction));
        value = r_u32(player + 0x20C); w_u32(start_view, value);
        first = r_u32(player + 0x210); second = r_u32(player + 0x2D4); w_u32(start_view + 4, first - second);
        value = r_u32(player + 0x214); w_u32(start_view + 8, value); value = r_u32(owner + 0x20C); w_u32(end_view, value);
        first = r_u32(owner + 0x210); second = r_u32(owner + 0x2D4); w_u32(end_view + 4, first - second);
        value = r_u32(owner + 0x214); w_u32(end_view + 8, value);
        value = level2p_sub_8003F26C(start_view, end_view, normal_view, normal_view, fraction_view, owner + 0x318, 0); return value >> 31;
    case 0x72: return r_u32(0x8008FCD4);
    case 0x73: return r_u32(0x8008FCD8);
    case 0x74:
        player = r_u32(0x8009943C); first = r_u16(owner + 0x3A4); second = (uint32)(sint32)r_s16(player + 0x20E);
        delta_view = moh_draft_local_address(delta, sizeof(delta)); w_u16(delta_view, second - first);
        first = (uint32)(sint32)r_s16(player + 0x212); second = r_u16(owner + 0x3A6); w_u16(delta_view + 2, first - second);
        first = (uint32)(sint32)r_s16(player + 0x216); second = r_u16(owner + 0x3A8); w_u16(delta_view + 4, first - second);
        return level2p_sub_800548C4(delta_view);
    default: return 0;
    }
}

uint32 level2p_sub_800576C4(uint32 owner, uint32 command, uint32 flags)
{
    uint32 operand, first, second, mode, state, yes, no, base, pointer, selected;
    sint32 depth;
    FUNCTION_MARKER(0x800576C4, "LEVEL2P.BIN");
    operand = r_u32(command + 4); first = level2p_sub_8005A984(1, flags, operand, owner);
    operand = r_u32(command + 8); mode = r_u16(command); second = level2p_sub_8005A984(3, flags, operand, owner);
    state = r_u32(0x8009618C); yes = r_u16(command + 12); depth = r_s16(state + 0x1E); no = r_u16(command + 14);
    /* TODO: Original branch-depth and command offsets are unchecked */
    if (depth) base = r_u32(state + ((uint32)(depth - 1) << 4) + 0x2C);
    else { pointer = r_u32(state + 0x10); base = r_u32(pointer + 12); }
    switch ((sint16)mode)
    {
    case 1: selected = first == second ? yes : no; break;
    case 2: selected = (sint32)second < (sint32)first ? yes : no; break;
    case 3: selected = (sint32)first < (sint32)second ? yes : no; break;
    case 4: selected = first != second ? yes : no; break;
    default: return command;
    }
    return base + ((uint32)(sint32)(sint16)selected << 2);
}

uint32 level2p_sub_8005A984(uint32 bit_index, uint32 flags, uint32 value, uint32 owner)
{
    FUNCTION_MARKER(0x8005A984, "LEVEL2P.BIN");
    if (flags & (1u << ((bit_index - 1) & 31))) return level2p_sub_8005A3E4(value, owner);
    return value;
}

void level2p_sub_8005A2E8(uint32 owner, uint32 field, uint32 value)
{
    uint32 current;
    FUNCTION_MARKER(0x8005A2E8, "LEVEL2P.BIN");
    current = level2p_sub_8005A3E4(field, owner); level2p_sub_8005A1B8(owner, field, value ^ current);
}

void level2p_sub_8005A33C(uint32 owner, uint32 field, uint32 value)
{
    uint32 current;
    FUNCTION_MARKER(0x8005A33C, "LEVEL2P.BIN");
    current = level2p_sub_8005A3E4(field, owner); level2p_sub_8005A1B8(owner, field, value | current);
}

void level2p_sub_8005A390(uint32 owner, uint32 field, uint32 value)
{
    uint32 current;
    FUNCTION_MARKER(0x8005A390, "LEVEL2P.BIN");
    current = level2p_sub_8005A3E4(field, owner); level2p_sub_8005A1B8(owner, field, value & current);
}

uint32 level2p_sub_80057804(uint32 owner, uint32 command, uint32 flags)
{
    uint32 operand, field, value;
    FUNCTION_MARKER(0x80057804, "LEVEL2P.BIN");
    operand = r_u32(command + 4); field = level2p_sub_8005A984(1, flags, operand, owner);
    operand = r_u32(command + 8); value = level2p_sub_8005A984(2, flags, operand, owner); level2p_sub_8005A2E8(owner, field, value);
    return command + 12;
}

uint32 level2p_sub_8005787C(uint32 owner, uint32 command, uint32 flags)
{
    uint32 operand, field, value;
    FUNCTION_MARKER(0x8005787C, "LEVEL2P.BIN");
    operand = r_u32(command + 4); field = level2p_sub_8005A984(1, flags, operand, owner);
    operand = r_u32(command + 8); value = level2p_sub_8005A984(2, flags, operand, owner); level2p_sub_8005A33C(owner, field, value);
    return command + 12;
}

uint32 level2p_sub_800578F4(uint32 owner, uint32 command, uint32 flags)
{
    uint32 operand, field, value;
    FUNCTION_MARKER(0x800578F4, "LEVEL2P.BIN");
    operand = r_u32(command + 4); field = level2p_sub_8005A984(1, flags, operand, owner);
    operand = r_u32(command + 8); value = level2p_sub_8005A984(2, flags, operand, owner); level2p_sub_8005A390(owner, field, value);
    return command + 12;
}

uint32 level2p_sub_80057974(uint32 owner, uint32 command, uint32 flags)
{
    uint32 operand, field, destination, value, current;
    FUNCTION_MARKER(0x80057974, "LEVEL2P.BIN");
    operand = r_u32(command + 4); field = level2p_sub_8005A984(1, flags, operand, owner); destination = level2p_sub_8005A974(field, owner);
    operand = r_u32(command + 8); value = level2p_sub_8005A984(2, flags, operand, owner); current = r_u32(destination); w_u32(destination, current + value);
    return command + 12;
}

uint32 level2p_sub_800579F8(uint32 owner, uint32 command, uint32 flags)
{
    uint32 operand, field, destination, value, current;
    FUNCTION_MARKER(0x800579F8, "LEVEL2P.BIN");
    operand = r_u32(command + 4); field = level2p_sub_8005A984(1, flags, operand, owner); destination = level2p_sub_8005A974(field, owner);
    operand = r_u32(command + 8); value = level2p_sub_8005A984(2, flags, operand, owner); current = r_u32(destination); w_u32(destination, current - value);
    return command + 12;
}

uint32 level2p_sub_80057A7C(uint32 owner, uint32 command, uint32 flags)
{
    uint32 operand, field, destination, value, current;
    FUNCTION_MARKER(0x80057A7C, "LEVEL2P.BIN");
    operand = r_u32(command + 4); field = level2p_sub_8005A984(1, flags, operand, owner); destination = level2p_sub_8005A974(field, owner);
    operand = r_u32(command + 8); value = level2p_sub_8005A984(2, flags, operand, owner); current = r_u32(destination); w_u32(destination, current * value);
    return command + 12;
}
