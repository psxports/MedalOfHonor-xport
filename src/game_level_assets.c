#include "game_shell_signatures.h"
#include "game_level_signatures.h"
#include "xport_trace.h"
#include "psx.h"
#include <stdio.h>
#include <stdlib.h>

void level_sub_800563E8(void)
{
    FUNCTION_MARKER(0x800563E8u, "LEVEL.BIN");
}

uint32 level_sub_80072704(uint32 effects)
{
    uint32 index, result = 0u, lifetime, position, step, red, green, blue;
    FUNCTION_MARKER(0x80072704u, "LEVEL.BIN");
    for (index = 0u; index < 5u; ++index, effects += 0x24u) {
        if (!(r_u32(effects) & 1u)) continue;
        lifetime = r_u32(effects + 4u) - 1u;
        result = 1u;
        w_u32(effects + 4u, lifetime);
        if ((sint32)lifetime <= 0) {
            w_u32(effects, 0u);
            w_u32(effects + 4u, 0u);
        } else {
            position = r_u32(effects + 0x14u);
            step = r_u32(effects + 12u);
            w_u32(effects + 0x14u, position - 0x10000u);
            position = r_u32(effects + 8u);
            red = r_u8(effects + 0x20u);
            w_u32(effects + 8u, position + step);
            green = r_u8(effects + 0x21u);
            w_u8(effects + 0x20u, red - (red >> 3));
            blue = r_u8(effects + 0x22u);
            w_u8(effects + 0x21u, green - (green >> 3));
            w_u8(effects + 0x22u, blue - (blue >> 3));
        }
    }
    return result;
}

uint32 level_sub_800599E4(uint32 object, uint32 data, uint32 frame)
{
    uint32 value;
    FUNCTION_MARKER(0x800599E4u, "LEVEL.BIN");
    value = moh_draft_missing_call(0x8005D148u, 1u, frame, r_u32(data + 4u), object);
    w_u32(object + 0x234u, value);
    value = moh_draft_missing_call(0x8005D148u, 2u, frame, r_u32(data + 8u), object);
    w_u32(object + 0x238u, value);
    value = moh_draft_missing_call(0x8005D148u, 3u, frame, r_u32(data + 12u), object);
    w_u32(object + 0x23Cu, value);
    return data + 16u;
}

sint32 level_sub_80057C8C(uint32 object, uint32 route)
{
    uint32 record, point, vector, index, distance, best = 0x7FFFu;
    sint32 count, result = -1;
    FUNCTION_MARKER(0x80057C8Cu, "LEVEL.BIN");
    if ((sint32)r_u32(0x8009E28Cu) < (sint32)route || (sint32)route < 0)
        moh_draft_missing_call(0x80089B04u, 4u, 0x40u);
    record = r_u32(0x8009E290u) + route * 8u;
    count = (sint16)r_u16(record + 2u);
    point = r_u32(record + 4u);
    if (count <= 0) return result;
    vector = moh_draft_missing_call(0x80016718u, 8u);
    /* TODO: Original unused vector padding has no defined value */
    for (index = 0u; (sint32)index < count; ++index, point += 20u) {
        w_u16(vector, (uint32)(sint32)(sint16)r_u16(object + 0x20Eu) - r_u16(point));
        w_u16(vector + 2u, (uint32)(sint32)(sint16)r_u16(object + 0x212u) - r_u16(point + 2u));
        w_u16(vector + 4u, (uint32)(sint32)(sint16)r_u16(object + 0x216u) - r_u16(point + 4u));
        distance = moh_draft_missing_call(0x80056EB4u, vector);
        if ((uint16)distance < (uint16)best) { result = (sint32)index; best = distance; }
    }
    moh_draft_missing_call(0x80036E44u, vector);
    return result;
}

sint32 level_sub_80057C24(uint32 id)
{
    uint32 records, index, count;
    FUNCTION_MARKER(0x80057C24u, "LEVEL.BIN");
    count = r_u32(0x8009E28Cu);
    records = r_u32(0x8009E290u);
    for (index = 0u; (sint32)index < (sint32)count; ++index, records += 8u)
        if ((sint16)r_u16(records) == (sint16)id) return (sint32)index;
    moh_draft_missing_call(0x80089B04u, 4u, 0x40u);
    return -1;
}

void level_sub_8005ABEC(uint32 extra)
{
    FUNCTION_MARKER(0x8005ABECu, "LEVEL.BIN");
    w_u32(extra + 4u, r_u32(extra + 4u) & 0xFFFFA3FCu);
}

void level_sub_8005ABA0(uint32 object)
{
    uint32 extra, speed;
    FUNCTION_MARKER(0x8005ABA0u, "LEVEL.BIN");
    extra = r_u32(object + 0x38Cu);
    w_u16(object + 0x2ECu, r_u16(extra + 0xC4u));
    w_u16(object + 0x2E8u, r_u16(extra + 0xC6u));
    w_u16(object + 0x3C8u, r_u16(extra + 0xCAu));
    w_u16(extra + 0xC0u, 0u);
    w_u16(extra + 0xC2u, 0u);
    w_u32(object + 0x224u, 0x10000u);
    speed = r_u16(extra + 0xD0u);
    w_u32(extra + 0xD8u, 0u);
    w_u16(extra + 0xCCu, speed);
}

void level_sub_8005AB04(uint32 object)
{
    sint32 index;
    uint32 records, handle, owner_view;
    FUNCTION_MARKER(0x8005AB04u, "LEVEL.BIN");
    index = (sint16)r_u16(r_u32(object + 0x38Cu) + 0xB6u);
    if (index < 0) return;
    records = r_u32(r_u32(r_u32(0x8009D648u) + 0x388u) + 8u);
    handle = records + (uint32)index * 0x34u + 0x30u;
    owner_view = moh_draft_missing_call(0x80016718u, 4u);
    w_u32(owner_view, object);
    if (moh_draft_missing_call(0x8006D7D0u, handle, owner_view)) {
        moh_draft_missing_call(0x8006D84Cu, handle, 0u);
        object = r_u32(owner_view);
        w_u32(object + 0x10u, r_u32(object + 0x10u) & ~0x80u);
    }
    moh_draft_missing_call(0x80036E44u, owner_view);
}

uint32 level_sub_8005AC00(uint32 object, uint32 data, uint32 frame)
{
    uint32 extra, third, fourth, value, mask;
    sint32 command, mode;
    FUNCTION_MARKER(0x8005AC00u, "LEVEL.BIN");
    extra = r_u32(object + 0x38Cu);
    third = (uint32)(sint32)(sint16)r_u16(data + 12u);
    fourth = (uint32)(sint32)(sint16)r_u16(data + 14u);
    command = (sint32)moh_draft_missing_call(0x8005D148u, 1u, frame, r_u32(data + 4u), object);
    switch (command) {
    case -1: case -2: case -0x1000: case -0x4000:
        moh_draft_missing_call(0x8005ABA0u, object);
        mask = ~(uint32)(0u - (uint32)command);
        w_u32(extra + 4u, r_u32(extra + 4u) & mask);
        break;
    case -0x800:
        moh_draft_missing_call(0x8005ABA0u, object);
        moh_draft_missing_call(0x8005AB04u, object);
        w_u32(extra + 4u, r_u32(extra + 4u) & ~0x800u);
        break;
    case -4: case -0x400:
        w_u32(extra + 4u, r_u32(extra + 4u) & ~(uint32)(0u - (uint32)command));
        break;
    case -0x2000:
        w_u32(extra + 4u, r_u32(extra + 4u) | 2u);
        break;
    case 0x2000:
        w_u32(extra + 4u, r_u32(extra + 4u) & ~2u);
        break;
    case 0x8000:
        w_u32(extra + 4u, 0u);
        moh_draft_missing_call(0x8005AB04u, object);
        moh_draft_missing_call(0x8005ABA0u, object);
        break;
    case 4:
        value = r_u32(extra + 4u) | 4u;
        w_u32(extra + 4u, value);
        if (value == 4u) moh_draft_missing_call(0x8005ABA0u, object);
        value = moh_draft_missing_call(0x8005D148u, 2u, frame, r_u32(data + 8u), object);
        w_u32(extra + 0x18u, value);
        w_u32(extra + 0xDCu, value);
        w_u32(extra, 0xFFFFFFF6u);
        value = moh_draft_missing_call(0x8005D148u, 3u, frame, third, object);
        w_u16(extra + 0xC0u, value);
        value = moh_draft_missing_call(0x8005D148u, 4u, frame, fourth, object);
        w_u32(extra + 0xD8u, (uint32)(sint32)(sint16)value);
        break;
    case 1: case 2: case 0x800: case 0x1000: case 0x4000:
        moh_draft_missing_call(0x8005ABECu, extra);
        moh_draft_missing_call(0x8005AB04u, object);
        moh_draft_missing_call(0x8005ABA0u, object);
        w_u32(extra + 4u, r_u32(extra + 4u) | (uint32)command);
        value = moh_draft_missing_call(0x8005D148u, 2u, frame, r_u32(data + 8u), object);
        if (command == 2) {
            value = moh_draft_missing_call(0x80057C24u, value);
            w_u16(extra + 0xB8u, value);
            value = moh_draft_missing_call(0x8005D148u, 3u, frame, third, object);
            w_u16(extra + 0xBAu, value);
            value = moh_draft_missing_call(0x80057C8Cu, object, (uint32)(sint32)(sint16)r_u16(extra + 0xB8u));
            mode = (sint16)r_u16(extra + 0xBAu);
            w_u16(extra + 0xBCu, value);
            if (mode == 0 || mode == 2) w_u16(extra + 0xBEu, 1u);
            else if (mode == 1) w_u16(extra + 0xBEu, 0xFFFFu);
            else if (mode == -1) w_u16(extra + 0xBAu, 2u);
        } else if (command == 1) {
            w_u32(extra + 0x10u, value);
            value = moh_draft_missing_call(0x8005D148u, 3u, frame, third, object);
            w_u16(extra + 0xC2u, value);
            value = moh_draft_missing_call(0x8005D148u, 4u, frame, fourth, object);
            w_u16(extra + 0xD4u, value);
            if ((sint16)value <= 0) w_u16(extra + 0xD4u, 0x100u);
        } else if (command == 0x4000) {
            w_u32(extra + 0x10u, value);
            value = moh_draft_missing_call(0x8005D148u, 3u, frame, third, object);
            w_u32(extra + 0x50u, value);
        } else {
            w_u32(extra + (command == 0x800 ? 0x3Cu : 0x40u), value);
            value = moh_draft_missing_call(0x8005D148u, 3u, frame, third, object);
            w_u32(extra + (command == 0x800 ? 0x7Cu : 0x80u), value);
            value = moh_draft_missing_call(0x8005D148u, 4u, frame, fourth, object);
            w_u16(extra + 0xA8u, value);
            if (command == 0x800) {
                value = r_u32(object + 0x38Cu);
                moh_draft_missing_call(0x80057474u, object, value, r_u32(value + 0x3Cu));
            }
            value = moh_draft_missing_call(0x8005D148u, 5u, frame, r_u16(data), object);
            w_u16(extra + 0xC2u, value);
        }
        break;
    default:
        break;
    }
    return data + 16u;
}

uint32 level_sub_80056EA0(uint32 value)
{
    FUNCTION_MARKER(0x80056EA0u, "LEVEL.BIN");
    return (sint32)value < 0 ? 0u - value : value;
}

sint32 level_sub_80056EB4(uint32 vector)
{
    uint32 x, y, z, swap;
    FUNCTION_MARKER(0x80056EB4u, "LEVEL.BIN");
    x = moh_draft_missing_call(0x80056EA0u, (uint32)(sint32)(sint16)r_u16(vector));
    y = moh_draft_missing_call(0x80056EA0u, (uint32)(sint32)(sint16)r_u16(vector + 2u));
    z = moh_draft_missing_call(0x80056EA0u, (uint32)(sint32)(sint16)r_u16(vector + 4u));
    if ((sint32)x < (sint32)y) { swap = x; x = y; y = swap; }
    if ((sint32)x < (sint32)z) { swap = x; x = z; z = swap; }
    return (sint32)(x + (uint32)((sint32)(y + z) >> 2));
}

sint32 level_sub_80056E40(uint32 angle)
{
    FUNCTION_MARKER(0x80056E40u, "LEVEL.BIN");
    while ((sint16)angle < -0x800) angle += 0x1000u;
    while ((sint16)angle > 0x800) angle -= 0x1000u;
    return (sint16)angle;
}

uint32 level_sub_8005A0C8(uint32 object, uint32 target, uint32 property, uint32 range, uint32 check_objects, uint32 write_property)
{
    uint32 height, extra, local, cursor, result = 0u, forward, angle, first, second;
    uint32 template0, template1, component, distance, other, radius, sum;
    sint32 limit = (sint16)range;
    VECTOR cross, squared;
    FUNCTION_MARKER(0x8005A0C8u, "LEVEL.BIN");
    height = r_u32(target + 0x2D4u);
    extra = r_u32(object + 0x38Cu);
    if (r_u32(target + 0x10u) & 0x800u) height = (uint32)((sint32)height >> 1);
    if (!extra) {
        result = moh_draft_missing_call(0x80056D08u, object, target, 3000u);
        if (write_property) moh_draft_missing_call(0x8005C97Cu, object, property, result);
        return result;
    }
    template0 = r_u32(0x8003C218u);
    template1 = r_u32(0x8003C21Cu);
    local = moh_draft_missing_call(0x80016718u, 80u);
    /* TODO: Original unused vector padding has no defined value */
    w_u32(local + 24u, template0);
    w_u32(local + 28u, template1);
    moh_draft_missing_call(0x8008DABCu, object, r_u32(object + 0x100u),
        (uint32)(sint32)(sint16)r_u16(extra + 0xAAu), local + 8u, 0u, 0u);
    moh_draft_missing_call(0x8008DABCu, object, r_u32(object + 0x100u),
        (uint32)(sint32)(sint16)r_u16(extra + 0xAAu), local + 16u, local + 24u, 0u);
    w_u16(local + 24u, r_u16(local + 16u) - r_u16(local + 8u));
    w_u16(local + 28u, r_u16(local + 20u) - r_u16(local + 12u));
    w_u16(local + 26u, r_u16(local + 18u) - r_u16(local + 10u));
    forward = moh_draft_missing_call(0x8001536Cu, (uint32)(sint32)(sint16)r_u16(local + 24u),
        (uint32)(sint32)(sint16)r_u16(local + 28u));
    w_u16(local, (uint32)(sint32)(sint16)r_u16(target + 0x20Eu) - r_u16(local + 8u));
    w_u16(local + 2u, (uint32)((sint32)(r_u32(target + 0x210u) - height) >> 16) - r_u16(local + 10u));
    w_u16(local + 4u, (uint32)(sint32)(sint16)r_u16(target + 0x216u) - r_u16(local + 12u));
    angle = moh_draft_missing_call(0x8001536Cu, (uint32)(sint32)(sint16)r_u16(local),
        (uint32)(sint32)(sint16)r_u16(local + 4u));
    angle = (uint32)(sint32)(sint16)(angle - forward);
    first = moh_draft_missing_call(0x80056E40u, angle);
    second = moh_draft_missing_call(0x80056E40u, angle);
    if ((sint16)first < 0) second = 0u - second;
    distance = moh_draft_missing_call(0x80056EB4u, local);
    w_u32(local + 72u, distance);
    if ((sint32)distance < limit && (sint16)second < (sint16)r_u16(extra + 0xB4u)) {
        if (check_objects) {
            moh_draft_missing_call(0x80016A40u, local, local);
            for (cursor = r_u32(0x8009D64Cu); cursor; cursor = r_u32(cursor + 4u)) {
                if (!(r_u32(cursor + 0x10u) & 0x4000u) || (r_u32(cursor + 12u) & 0x20080000u)) continue;
                for (component = 0u; component < 3u; ++component)
                    w_u16(local + 64u + component * 2u, r_u16(cursor + 0x2E0u + component * 2u) - r_u16(local + 8u + component * 2u));
                other = moh_draft_missing_call(0x80056EB4u, local + 64u);
                if ((sint32)other > limit || (sint32)other >= (sint32)r_u32(local + 72u)) continue;
                xport_gte_write_control(0u, r_u32(local + 64u));
                xport_gte_write_control(1u, r_u32(local + 68u));
                for (component = 0u; component < 3u; ++component)
                    xport_gte_write_data(9u + component, r_u16(local + component * 2u));
                xport_gte_execute(0x41E012u);
                if ((sint32)xport_gte_read_data(25u) <= 0) continue;
                for (component = 0u; component < 3u; ++component) {
                    xport_gte_write_control(component * 2u, (uint32)(sint32)(sint16)r_u16(local + 64u + component * 2u));
                    xport_gte_write_data(9u + component, (uint32)(sint32)(sint16)r_u16(local + component * 2u));
                }
                xport_gte_execute(0x178000Cu);
                for (component = 0u; component < 3u; ++component)
                    w_u16(local + 64u + component * 2u, xport_gte_read_data(9u + component));
                cross.vx = r_u16(local + 64u);
                cross.vy = r_u16(local + 66u);
                cross.vz = r_u16(local + 68u);
                Square0(&cross, &squared);
                radius = r_u16(cursor + 0x2E8u);
                sum = (uint32)squared.vx + (uint32)squared.vy + (uint32)squared.vz;
                if ((sint32)sum <= (sint32)(radius * radius)) {
                    if (write_property) moh_draft_missing_call(0x8005C97Cu, object, property, 0u);
                    moh_draft_missing_call(0x80036E44u, local);
                    return 0u;
                }
            }
        }
        w_u32(local + 32u, (uint32)(sint32)(sint16)r_u16(local + 8u) << 16);
        w_u32(local + 40u, (uint32)(sint32)(sint16)r_u16(local + 12u) << 16);
        w_u32(local + 36u, (uint32)(sint32)(sint16)r_u16(local + 10u) << 16);
        w_u32(local + 48u, r_u32(target + 0x20Cu));
        w_u32(local + 52u, r_u32(target + 0x210u) - height);
        w_u32(local + 56u, r_u32(target + 0x214u));
        if ((sint32)moh_draft_missing_call(0x8003EF5Cu, local + 32u, local + 48u, local + 24u,
                local + 16u, local + 72u, object + 0x318u, 0u) < 0) result = 1u;
    }
    if (write_property) moh_draft_missing_call(0x8005C97Cu, object, property, result);
    moh_draft_missing_call(0x80036E44u, local);
    return result;
}

uint32 level_sub_8005BF2C(uint32 object, uint32 data, uint32 frame)
{
    uint32 current, desired, animation;
    FUNCTION_MARKER(0x8005BF2Cu, "LEVEL.BIN");
    current = (uint32)(sint32)(sint16)r_u16(object + 0x192u);
    desired = (uint32)(sint32)(sint16)r_u16(data);
    if (current != desired) {
        animation = moh_draft_missing_call(0x8005D148u, 1u, frame, desired, object);
        moh_draft_missing_call(0x8003F454u, object, (uint32)(sint32)(sint16)animation, 0u, 1u);
    }
    return data + 4u;
}

void level_sub_80052AF4(uint32 object)
{
    uint32 cursor, destination = 0x80098160u, kind;
    sint32 count = -1;
    FUNCTION_MARKER(0x80052AF4u, "LEVEL.BIN");
    if (!(r_u32(object + 0x38u) & 0x60u)) return;
    moh_draft_missing_call(0x8006D84Cu, 0x80098178u, object);
    cursor = r_u32(0x8009D64Cu);
    w_u8(0x80098184u, 0u);
    for (; cursor; cursor = r_u32(cursor + 4u)) {
        kind = r_u32(cursor + 8u);
        if ((kind == 0x1402u || kind == 0x1405u) && cursor != object && count < 4 &&
            moh_draft_missing_call(0x8005CBA8u, 0x109u, cursor)) {
            destination += 4u;
            ++count;
            moh_draft_missing_call(0x8006D84Cu, destination, cursor);
            w_u8(0x80098184u, (uint32)count + 1u);
        }
    }
    w_u16(0x8009817Cu, r_u16(object + 0x20Eu));
    w_u16(0x8009817Eu, r_u16(object + 0x212u));
    w_u16(0x80098180u, r_u16(object + 0x216u));
}

static uint32 moh_enemy_rotate_toward(uint32 address, uint32 current, uint32 target)
{
    uint32 next;
    if (current == target) return 1u;
    if ((sint32)target < (sint32)current) {
        next = current - 0x2240000u;
        w_u32(address, next);
        if ((sint32)next < (sint32)target) w_u32(address, target);
    } else {
        next = current + 0x2240000u;
        w_u32(address, next);
        if ((sint32)target < (sint32)next) w_u32(address, target);
    }
    return 0u;
}

void level_sub_80055198(uint32 object)
{
    uint32 state, block, rotation, table, index, flags, resource, value, sound, bank, shift;
    uint32 random, current, desired, complete, second_complete, kind;
    FUNCTION_MARKER(0x80055198u, "LEVEL.BIN");
    state = r_u32(object + 0x388u);
    block = state + 0x1Cu;
    rotation = state + 0x7Cu;
    table = 0x800980B0u;
    for (index = 0u; (sint32)index < (sint32)r_u32(state + 0x18u);
         ++index, table += 0x3Cu, rotation += 0x144u, block += 0x144u) {
        flags = r_u32(block);
        if (flags & 2u) {
            value = moh_draft_missing_call(0x80050E34u, block, r_u32(object + 0x210u));
            if (value) {
                sound = r_u32(table + 0x24u);
                if (sound != 0xFFFFFFFFu) {
                    shift = r_u32(rotation - 0x50u) - 1u;
                    if ((sint32)shift < 0) shift = 0u;
                    if ((sint32)sound >= 0) {
                        if (r_u32(table + 0x28u)) {
                            random = moh_draft_missing_call(0x8001552Cu);
                            sound += (uint32)((sint32)(random * r_u32(table + 0x28u)) >> 15);
                        }
                        bank = r_u32(table + 0x2Cu);
                        if ((sint32)bank < 0) bank = r_u16(r_u32(r_u32(object + 0x100u) + 8u) + 0xC8u);
                        moh_draft_missing_call(0x80041CE8u, (uint32)(sint32)(sint16)bank, sound, object,
                            0x10000u >> (shift & 31u), 0u, 0x10000u, 0x1800u, 0u, 0u, 0u, 0u);
                    }
                }
            }
        } else if ((flags & 1u) && moh_draft_missing_call(0x8005CBA8u, 0xDu, object) == 0xFFFFFFFFu) {
            resource = r_u32(table + 0x20u);
            if (resource && r_u32(r_u32(rotation + 0x3Cu)) != resource) {
                moh_draft_missing_call(0x8006B234u, block + 0x94u, 0u, 0u);
                moh_draft_missing_call(0x8006B1C4u, block + 0x94u, resource, 0x156u, 0u);
            }
            moh_draft_missing_call(0x8006D84Cu, block + 0xE8u, 0u);
            w_u32(rotation + 0x8Cu, 0u);
            w_u32(block, r_u32(block) | 8u);
        }
        if (!(r_u32(block) & 0x10u)) continue;
        current = r_u32(rotation);
        desired = r_u32(table + 0x30u);
        if ((sint32)current < 0) {
            if ((sint32)current < -0x4000000) desired -= 0x8000000u;
        } else {
            if ((sint32)current > 0x4000000) desired += 0x8000000u;
            current = r_u32(rotation);
        }
        complete = moh_enemy_rotate_toward(rotation, current, desired);
        current = r_u32(rotation + 8u);
        desired = r_u32(table + 0x38u);
        if ((sint32)current < 0) desired = 0u - desired;
        second_complete = moh_enemy_rotate_toward(rotation + 8u, current, desired);
        if (!complete || !second_complete) continue;
        flags = r_u32(block);
        w_u32(block, flags & ~0x10u);
        if (!(flags & 0x40u) || !r_u32(r_u32(r_u32(object + 0x1Cu) + 4u) + 0x7Cu)) continue;
        kind = moh_draft_missing_call(0x8005CBA8u, 0xFu, object) & 0xFFu;
        w_u32(block, (r_u32(block) & ~1u) | 0x20u);
        moh_draft_missing_call(0x80083728u, object, block + 0x3Cu, rotation,
            r_u32(0x80097FF0u + kind * 0x30u), kind);
    }
}

static void moh_enemy_spread(uint32 object, uint32 offset, sint32 range)
{
    uint32 old = r_u32(object + offset), random, delta;
    if (!range) delta = 0u;
    else {
        random = moh_draft_missing_call(0x8001552Cu);
        delta = random * ((uint32)range * 2u);
        delta = (uint32)((sint32)delta >> 15) - (uint32)range;
    }
    w_u32(object + offset, old + (delta << 16));
}

static void moh_enemy_shot_sound(uint32 object, uint32 table, uint32 grenade_only)
{
    uint32 bank, sound = r_u32(table + 0x28u), mode;
    if ((sint32)sound < 0) return;
    bank = r_u32(table + 0x2Cu);
    if ((sint32)bank < 0) bank = r_u16(r_u32(r_u32(object + 0x100u) + 8u) + 0xC8u);
    mode = grenade_only ? 3u : (r_u32(table + 4u) == 0x1773u ? 3u : 0u);
    moh_draft_missing_call(0x80041CE8u, (uint32)(sint32)(sint16)bank, sound, object,
        0x10000u, 0u, 0x10000u, 0x1800u, 0u, 0u, mode, 0u);
}

static uint32 moh_enemy_timer(uint32 table, uint32 offset)
{
    uint32 range = r_u32(table + offset + 4u), base = r_u8(table + offset), random;
    if (range) {
        random = moh_draft_missing_call(0x8001552Cu);
        range = r_u32(table + offset + 4u);
        base += (uint32)((sint32)(random * range) >> 15);
    }
    return base;
}

void level_sub_8005163C(uint32 object, uint32 target, uint32 kind, uint32 origin)
{
    uint32 state, resource, spread, mode, table, value, projectile, local = 0u, grenade;
    uint32 aim[3], old, random, delta, projectile_state;
    sint32 first_angle, angle, range;
    FUNCTION_MARKER(0x8005163Cu, "LEVEL.BIN");
    state = r_u32(object + 0x388u);
    resource = r_u32(object + 0x1Cu);
    spread = r_u32(object + 0x38Cu);
    mode = r_u8(state + 0x11u);
    resource = r_u32(resource + 4u);
    spread = r_u16(spread + 0xC8u);
    table = 0x80097FF0u + (kind & 0xFFu) * 0x30u;
    range = (sint16)spread;
    if (!mode) {
        value = r_u8(state + 0x14u);
        w_u8(state + 0x14u, value ? value - 1u : 0u);
        if (!r_u8(state + 0x13u)) {
            value = r_u8(state + 0x12u);
            if (value) w_u8(state + 0x12u, value - 1u);
        }
        goto done;
    }
    if (mode != 2u) {
        if (mode > 4u) goto done;
        value = r_u8(state + 0x12u);
        if (!value) {
            w_u8(state + 0x12u, moh_enemy_timer(table, 0x20u));
            w_u8(state + 0x13u, moh_enemy_timer(table, 0x18u));
            w_u8(state + 0x14u, 0u);
            goto done;
        }
        value = r_u8(state + 0x14u);
        if (value) { w_u8(state + 0x14u, value - 1u); goto done; }
        if (!r_u8(state + 0x13u)) {
            w_u8(state + 0x12u, r_u8(state + 0x12u) - 1u);
            goto done;
        }
        w_u8(state + 0x14u, moh_enemy_timer(table, 0x10u));
        first_angle = (sint16)moh_draft_missing_call(0x8003F858u, object, target);
        angle = (sint16)moh_draft_missing_call(0x8003F858u, object, target);
        if (first_angle < 0) angle = -angle;
        mode = r_u8(state + 0x11u);
        if (angle >= 0x15E && mode != 3u && mode != 4u) goto done;
    }
    if (mode != 3u) {
        aim[0] = r_u16(target + 0x20Eu);
        aim[1] = (r_u32(target + 0x210u) - (uint32)((sint32)r_u32(target + 0x2D4u) >> 1)) >> 16;
        aim[2] = r_u16(target + 0x216u);
        local = moh_draft_missing_call(0x80016718u, 8u);
        w_u16(local, aim[0]); w_u16(local + 2u, aim[1]); w_u16(local + 4u, aim[2]);
        /* TODO: Original aim-vector padding is unspecified */
    }
    grenade = r_u32(table + 4u) == 0x1773u;
    if (grenade) {
        projectile = moh_draft_missing_call(0x80045CA0u, object, r_u32(table + 8u), r_u32(table + 12u),
            local, 0u, 0u, 0u, 0x13F1u, 0x800u, 0x10000u, 0x8000451u);
        if (mode == 2u && !projectile) goto finish_shot;
        if (mode == 3u && !projectile) goto continuous_shot;
        if (mode != 3u) {
            moh_enemy_spread(projectile, 0x228u, range);
            old = r_u32(projectile + 0x22Cu);
            delta = 0u;
            if (range) {
                random = moh_draft_missing_call(0x8001552Cu);
                delta = (uint32)((sint32)(random * ((uint32)range * 2u)) >> 15) - (uint32)range;
            }
            if (mode == 2u) {
                w_u32(projectile + 0x22Cu, old + (delta << 16));
                projectile_state = r_u32(projectile + 0x388u);
                w_u32(projectile + 0x200u, 0x820000u);
            } else {
                projectile_state = r_u32(projectile + 0x388u);
                w_u32(projectile + 0x22Cu, old + (delta << 16));
                w_u32(projectile + 0x200u, 0x960000u);
            }
        } else {
            projectile_state = r_u32(projectile + 0x388u);
            w_u32(projectile + 0x200u, 0x960000u);
        }
        w_u16(projectile_state + 0x3Au, r_u16(projectile_state + 0x3Au) << 1);
        if (mode == 2u) moh_enemy_shot_sound(object, table, 1u);
    } else {
        value = r_u8(resource + 0x64u) << 16;
        projectile = moh_draft_missing_call(0x80045A78u, object, r_u32(table + 8u), r_u32(table + 12u),
            local, origin, 0u, 0u, value, 0u, 0u, 0x13F4u, 0x800u, 0u, 0x8000401u);
        if (projectile) {
            moh_draft_missing_call(0x800514E8u, projectile, object);
            if (mode != 3u) {
                value = r_u32(r_u32(r_u32(0x8009D654u) + 0x388u)) & 2u;
                if (!value) {
                    moh_enemy_spread(projectile, 0x228u, range);
                    moh_enemy_spread(projectile, 0x22Cu, range);
                } else if (mode == 2u) {
                    old = r_u32(projectile + 0x228u);
                    delta = (uint32)(range >> 1);
                    if (range) {
                        random = moh_draft_missing_call(0x8001552Cu);
                        delta = (uint32)((-range) >> 1) + (uint32)((sint32)(random * (uint32)range) >> 15);
                    }
                    w_u32(projectile + 0x228u, old + (delta << 16));
                    moh_enemy_spread(projectile, 0x22Cu, range);
                } else {
                    random = moh_draft_missing_call(0x8001552Cu);
                    delta = (uint32)((sint32)(random * 40u) >> 15) - 20u;
                    w_u32(projectile + 0x228u, r_u32(projectile + 0x228u) + (delta << 16));
                    random = moh_draft_missing_call(0x8001552Cu);
                    delta = (uint32)((sint32)(random * 180u) >> 15) - 90u;
                    w_u32(projectile + 0x22Cu, r_u32(projectile + 0x22Cu) + (delta << 16));
                }
            }
        }
    }
    if (mode == 2u) goto finish_shot;
continuous_shot:
    moh_draft_missing_call(0x8006DCA0u, state + 0xB68u);
    moh_enemy_shot_sound(object, table, 0u);
    value = r_u8(state + 0x13u) - 1u;
    w_u8(state + 0x13u, value);
    if (!(value & 0xFFu)) {
        w_u32(object + 0x3Cu, r_u32(object + 0x3Cu) | 1u);
        w_u8(state + 0x13u, 0u);
    }
    goto done;
finish_shot:
    moh_draft_missing_call(0x8006DCA0u, state + 0xB68u);
    w_u8(state + 0x11u, 0u);
done:
    w_u8(state + 0x10u, r_u8(state + 0x11u));
    if (local) moh_draft_missing_call(0x80036E44u, local);
}

void level_sub_80055528(uint32 object)
{
    sint32 full, half, third;
    uint32 player, extra, flags;
    FUNCTION_MARKER(0x80055528u, "LEVEL.BIN");
    if (!r_u32(0x80099B2Cu)) {
        full = (sint16)r_u16(r_u32(object + 0x38Cu) + 0xB2u);
        w_u32(0x80099B2Cu, 1u);
        w_u16(0x800AB930u, (uint32)(full / 3));
    }
    player = r_u32(0x8009D654u);
    player = r_u32(player + 0x388u);
    extra = r_u32(object + 0x38Cu);
    flags = r_u32(player);
    full = (sint16)r_u16(extra + 0xB2u);
    if (flags & 2u) {
        third = full >> 2;
        half = (sint16)((full >> 1) - (full >> 3));
        full = (sint16)(full - third);
    } else {
        half = full >> 1;
        third = (sint16)r_u16(0x800AB930u);
    }
    moh_draft_missing_call(0x8005C97Cu, object, 5u, (uint32)full);
    moh_draft_missing_call(0x8005C97Cu, object, 6u, (uint32)half);
    moh_draft_missing_call(0x8005C97Cu, object, 7u, (uint32)(sint32)(sint16)third);
}

void level_sub_80055628(uint32 object)
{
    uint32 player, kind, state, handle, parent, checked, local = 0u, value, other;
    FUNCTION_MARKER(0x80055628u, "LEVEL.BIN");
    player = r_u32(0x8009D654u);
    kind = moh_draft_missing_call(0x8005CBA8u, 0xFu, object);
    state = r_u32(object + 0x388u);
    if ((sint16)r_u16(object + 0xF6u) <= 0) {
        handle = r_u32(object + 0x30u);
        w_u32(object + 0xF4u, 0u);
        parent = handle ? r_u32(0x8009D678u + (handle & 0xFFFFu) * 4u) : 0u;
        handle = r_u32(object + 0x30u);
        checked = handle ? r_u32(0x8009D678u + (handle & 0xFFFFu) * 4u) : 0u;
        if (checked) {
            local = moh_draft_missing_call(0x80016718u, 4u);
            w_u32(local, object);
            value = moh_draft_missing_call(0x8006D7D0u, parent + 0x30u, local);
            object = r_u32(local);
            if (value) moh_draft_missing_call(0x8006D84Cu, parent + 0x30u, 0u);
        }
    }
    moh_draft_missing_call(0x80055528u, object);
    value = moh_draft_missing_call(0x8005CBA8u, 0x16u, object);
    w_u8(state + 0x11u, value);
    moh_draft_missing_call(0x8005163Cu, object, player, kind & 0xFFu, state + 0x1Cu);
    moh_draft_missing_call(0x8005C97Cu, object, 0x16u, r_u8(state + 0x11u));
    moh_draft_missing_call(0x80055198u, object);
    if (moh_draft_missing_call(0x8005CBA8u, 0x1Au, object)) {
        value = moh_draft_missing_call(0x8005CBA8u, 0x1Au, object);
        other = moh_draft_missing_call(0x8005CBA8u, 0x1Bu, object);
        moh_draft_missing_call(0x800523E4u, object, value, 0x800980B0u, other);
    }
    moh_draft_missing_call(0x80052AF4u, object);
    moh_draft_missing_call(0x8003FBFCu, object);
    moh_draft_missing_call(0x8005C97Cu, object, 0x13u, r_u32(object + 0x22Cu));
    moh_draft_missing_call(0x8003F8F0u, object);
    value = moh_draft_missing_call(0x8005CBA8u, 0x13u, object);
    w_u32(object + 0x22Cu, value);
    if (moh_draft_missing_call(0x8005CBA8u, 0x1Cu, object)) {
        value = moh_draft_missing_call(0x8005CBA8u, 0xCu, object);
        moh_draft_missing_call(0x80052084u, object, object + 0xBCu, value, state + 0x3E8u, state + 0x49Cu);
    }
    value = moh_draft_missing_call(0x80072704u, state + 0x3E8u);
    w_u32(state + 4u, value);
    value = moh_draft_missing_call(0x800730D4u, object, state + 0x49Cu);
    w_u32(state, value);
    moh_draft_missing_call(0x800563E8u, object);
    if (local) moh_draft_missing_call(0x80036E44u, local);
}

uint32 level_sub_8005BF94(uint32 object, uint32 data)
{
    FUNCTION_MARKER(0x8005BF94u, "LEVEL.BIN");
    w_u16(object + 0x192u, 0xFFFFu);
    w_u16(object + 0x190u, 0xFFFFu);
    w_u32(object + 0x188u, 0u);
    return data + 4u;
}

uint32 level_sub_8005BD70(uint32 object, uint32 data)
{
    uint32 context, depth;
    FUNCTION_MARKER(0x8005BD70u, "LEVEL.BIN");
    context = r_u32(0x80099EC4u);
    depth = r_u16(context + 0x1Eu) - 1u;
    w_u16(context + 0x1Eu, depth);
    if ((sint16)depth < 0) {
        w_u16(context + 0x1Eu, 0u);
        return data + 4u;
    }
    data = r_u32(context + ((uint32)(sint32)(sint16)depth << 4) + 0x24u);
    w_u32(context + 0x14u, data);
    return data;
}

uint32 level_sub_8005BC74(uint32 object, uint32 data, uint32 frame)
{
    uint32 context, depth, index, saved, animation, row, return_frame;
    FUNCTION_MARKER(0x8005BC74u, "LEVEL.BIN");
    context = r_u32(0x80099EC4u);
    depth = r_u16(context + 0x1Eu) + 1u;
    w_u16(context + 0x1Eu, depth);
    if ((sint16)depth < 6) {
        index = moh_draft_missing_call(0x8005D148u, 1u, frame, r_u16(data), object) << 4;
        context = r_u32(0x80099EC4u);
        animation = r_u32(object + 0x100u);
        return_frame = r_u32(0x80099EBCu);
        depth = (uint32)(sint32)(sint16)r_u16(context + 0x1Eu);
        animation = r_u32(animation + 8u);
        saved = context + (depth << 4) + 0x14u;
        row = r_u32(animation + 0xB8u);
        w_u32(saved, data + 4u);
        row += index;
        w_u32(saved + 4u, r_u32(return_frame + 12u));
        w_u32(saved + 8u, r_u32(row + 12u));
        w_u16(saved + 12u, r_u16(row + 10u));
        data = r_u32(row + 12u);
        w_u32(context + 0x14u, data);
    } else {
        moh_draft_missing_call(0x80089B04u, 5u, 0x140u);
        data += 4u;
        w_u16(r_u32(0x80099EC4u) + 0x1Eu, 5u);
    }
    return data;
}

uint32 level_sub_8006D600(uint32 kind, uint32 id)
{
    uint32 object;
    FUNCTION_MARKER(0x8006D600u, "LEVEL.BIN");
    for (object = r_u32(0x8009D64Cu); object; object = r_u32(object + 4u))
        if (r_u32(object + 8u) == kind && (sint8)r_u8(object + 0x35u) == (sint8)id &&
            !(r_u32(object + 12u) & 0x20080000u)) return object;
    return 0u;
}

uint32 level_sub_8005A574(uint32 object, uint32 data, uint32 frame)
{
    uint32 property, identity, selector, target, operation, value, local, index, record, handle;
    uint32 position[3], axis, accepted, flag, offset;
    sint32 distance, range;
    FUNCTION_MARKER(0x8005A574u, "LEVEL.BIN");
    property = moh_draft_missing_call(0x8005D148u, 1u, frame, r_u32(data + 4u), object);
    identity = moh_draft_missing_call(0x8005D148u, 2u, frame, r_u32(data + 8u), object);
    selector = moh_draft_missing_call(0x8005D148u, 3u, frame, r_u8(data + 1u), object);
    target = moh_draft_missing_call(0x8005D148u, 4u, frame, r_u32(data + 12u), object);
    if (!target) {
        if (identity == 0xFFFFFFFFu) {
            if ((sint8)selector == -1) target = object;
        } else if ((sint8)selector == -1)
            target = moh_draft_missing_call(0x80057768u, object, identity);
        else target = moh_draft_missing_call(0x8006D600u, identity, (uint32)(sint32)(sint8)selector);
    }
    operation = r_u8(data);
    switch (operation) {
    case 1u:
        moh_draft_missing_call(0x8005C97Cu, object, property, r_u32(target + 0xF4u));
        (void)r_u32(target + 0xF4u);
        break;
    case 3u: value = r_u32(target + 0x2D4u); goto write_value;
    case 4u: value = r_u32(target + 0x290u); goto write_value;
    case 5u: value = r_u32(target + 0x44u); goto write_value;
    case 6u: value = r_u32(target + 0x48u); goto write_value;
    case 8u: value = target; goto write_value;
    case 9u:
    case 10u:
        if (operation == 10u) { if (!identity) break; target = identity; }
        local = moh_draft_missing_call(0x80016718u, 8u);
        for (axis = 0u; axis < 3u; ++axis)
            w_u16(local + axis * 2u, (r_u32(object + 0x20Cu + axis * 4u) - r_u32(target + 0x20Cu + axis * 4u)) >> 16);
        /* TODO: Original vector padding is unspecified */
        value = moh_draft_missing_call(0x80056EB4u, local);
        moh_draft_missing_call(0x8005C97Cu, object, property, value);
        moh_draft_missing_call(0x80036E44u, local);
        break;
    case 11u:
    case 16u:
        offset = operation == 11u ? 0xB0u : 0xB2u;
        flag = r_u32(r_u32(r_u32(0x8009D654u) + 0x388u)) & 2u;
        value = r_u16(r_u32(object + 0x38Cu) + offset);
        range = flag ? (sint16)(value - ((sint32)(sint16)value >> 2)) : (sint16)value;
        moh_draft_missing_call(0x8005A0C8u, object, target, property, (uint32)range, 1u, 1u);
        break;
    case 12u:
        distance = (sint16)moh_draft_missing_call(0x8003F858u, object, target);
        value = (uint32)(sint32)(sint16)moh_draft_missing_call(0x8003F858u, object, target);
        if (distance < 0) value = 0u - value;
        goto write_value;
    case 13u: value = object; goto write_value;
    case 14u: value = (uint32)(sint32)(sint16)r_u16(r_u32(0x80099EB4u)); goto write_value;
    case 15u: value = moh_draft_missing_call(0x80056F38u, object); goto write_value;
    case 17u:
        range = 3000;
        value = r_u32(object + 0x38Cu);
        if (value) range = (sint16)r_u16(value + 0xB0u);
        moh_draft_missing_call(0x8005C97Cu, object, property, 0u);
        for (axis = 0u; axis < 3u; ++axis) position[axis] = r_u16(object + 0x20Eu + axis * 4u);
        local = moh_draft_missing_call(0x80016718u, 16u);
        w_u32(local, object);
        /* TODO: Original distance-vector padding is unspecified */
        record = 0x80091190u;
        handle = 0x80091194u;
        for (index = 1u; index < 24u; ++index, record += 0x34u, handle += 0x34u) {
            if (r_u8(record + 14u) || !(r_u8(0x800910D0u + index) & 1u) ||
                r_u16(record + 0x28u) >= 0x3Cu || (r_u32(record) & 3u)) continue;
            accepted = moh_draft_missing_call(0x8006D8ACu, handle);
            if (!accepted && moh_draft_missing_call(0x8006D7D0u, handle, local)) continue;
            for (axis = 0u; axis < 3u; ++axis)
                w_u16(local + 8u + axis * 2u, position[axis] - r_u16(record + 0x2Cu + axis * 2u));
            value = moh_draft_missing_call(0x80056EB4u, local + 8u);
            if ((sint32)(value & 0xFFFFu) < range)
                moh_draft_missing_call(0x8005C97Cu, r_u32(local), property, 1u);
        }
        moh_draft_missing_call(0x80036E44u, local);
        break;
    case 18u:
        range = (sint16)r_u16(r_u32(object + 0x38Cu) + 0xB2u);
        moh_draft_missing_call(0x8005A0C8u, object, target, property, (uint32)range, 0u, 1u);
        break;
    default: break;
    }
    return data + 16u;
write_value:
    moh_draft_missing_call(0x8005C97Cu, object, property, value);
    return data + 16u;
}

uint32 level_sub_8008ED30(void)
{
    uint32 index, object, active, count;
    FUNCTION_MARKER(0x8008ED30u, "LEVEL.BIN");
    object = 0x800C4378u;
    for (index = 0u; index < 5u; ++index, object += 0x1308u) {
        if (!r_u32(object + 0x1304u)) continue;
        w_u32(object + 0x1300u, 0xFFFFFFBDu);
        w_u32(object + 0x1304u, 0u);
        moh_draft_missing_call(0x800158ACu, object, 0u, 0x1300u);
        count = r_u32(0x800AB8D0u);
        active = r_u32(0x800AB8CCu);
        count += 1u;
        w_u32(0x800AB8D0u, count);
        w_u32(0x800AB8C8u, (sint32)active < (sint32)count ? count : active);
        return object;
    }
    return 0u;
}

uint32 level_sub_8008EEF0(void)
{
    uint32 index, object, active, maximum, previous;
    FUNCTION_MARKER(0x8008EEF0u, "LEVEL.BIN");
    object = 0x800B7B08u;
    for (index = 0u; index < 5u; ++index, object += 0x2814u) {
        if (!r_u32(object + 0x2810u)) continue;
        w_u32(object + 0x280Cu, 0xFFFFFFBDu);
        w_u32(object + 0x2810u, 0u);
        moh_draft_missing_call(0x800158ACu, object, 0u, 0x280Cu);
        maximum = r_u32(0x800AB8D0u);
        previous = r_u32(0x800AB8C8u);
        active = r_u32(0x800AB8CCu);
        w_u32(0x800AB8C8u, previous + 1u);
        w_u32(0x800AB8C8u, (sint32)active < (sint32)maximum ? maximum : active);
        return object;
    }
    return 0u;
}

uint32 level_sub_80045A78(uint32 owner, uint32 yaw, uint32 pitch, uint32 roll, uint32 offset, uint32 first, uint32 second, uint32 flags, uint32 spread, uint32 damage, uint32 resource, uint32 mode, uint32 extra, uint32 final)
{
    uint32 object, state, random, speed, mask, midpoint, dx, dy, dz;
    FUNCTION_MARKER(0x80045A78u, "LEVEL.BIN");
    random = moh_draft_missing_call(0x8001552Cu);
    speed = (uint32)((sint32)random % 0x46 + 600) << 16;
    object = moh_draft_missing_call(0x8004532Cu, owner, resource, 0xFFFFFFFFu, speed,
        yaw, pitch, roll, 0x20u, flags, 0xFu, 0u, offset, first, second, damage, mode, extra, final);
    if (!object) return 0u;
    state = r_u32(object + 0x388u);
    w_u32(object + 0x37Cu, 0x80043AD4u);
    w_u16(state + 0x38u, 0u);
    if ((r_u32(owner + 12u) & 1u) && (r_u32(0x800386A4u) & 0x20u))
        w_u16(state + 0x38u, 0xA40u);
    moh_draft_missing_call(0x8004572Cu, object, 0xE6u, 0xB4u, 0x46u, 0x28u, 0x14u, 10u);
    w_u32(object + 16u, r_u32(object + 16u) | 0x90004u);
    if (spread) {
        random = moh_draft_missing_call(0x8001552Cu);
        mask = (2u << (spread & 31u)) - 1u;
        midpoint = 1u << (spread & 31u);
        dx = (random & mask) - midpoint;
        dy = (moh_draft_missing_call(0x8001552Cu) & mask) - midpoint;
        dz = (moh_draft_missing_call(0x8001552Cu) & mask) - midpoint;
        w_u32(object + 0x228u, r_u32(object + 0x228u) + (dx << 9));
        w_u32(object + 0x22Cu, r_u32(object + 0x22Cu) + (dy << 9));
        w_u32(object + 0x230u, r_u32(object + 0x230u) + (dz << 9));
    }
    return object;
}

uint32 level_sub_80078350(uint32 point, uint32 radius, uint32 height, uint32 plane)
{
    uint32 distance, other, step, x, y, z;
    FUNCTION_MARKER(0x80078350u, "LEVEL.BIN");
    xport_gte_write_control(0u, r_u32(point));
    xport_gte_write_control(1u, r_u32(point + 4u));
    xport_gte_write_data(9u, r_u16(plane));
    xport_gte_write_data(10u, r_u16(plane + 2u));
    xport_gte_write_data(11u, r_u16(plane + 4u));
    xport_gte_execute(0x41E012u);
    distance = xport_gte_read_data(25u);
    step = height * (uint32)(sint32)(sint16)r_u16(plane + 2u);
    distance = (uint32)((sint32)distance >> 15) + (uint32)(sint32)(sint16)r_u16(plane + 6u);
    step = (uint32)((sint32)step >> 15);
    if ((sint32)(distance ^ (distance + step)) < 0) return 1u;
    if ((sint32)distance < 0) return 0u;
    x = (uint32)(sint32)(sint16)r_u16(point);
    x = (sint16)r_u16(plane) > 0 ? x - radius : x + radius;
    y = r_u16(point + 2u);
    z = (uint32)(sint32)(sint16)r_u16(point + 4u);
    z = (sint16)r_u16(plane + 4u) > 0 ? z - radius : z + radius;
    xport_gte_write_control(0u, (y << 16) | (x & 0xFFFFu));
    /* TODO: Original upper padding halfword is unspecified and unused by this dot product */
    xport_gte_write_control(1u, z & 0xFFFFu);
    xport_gte_write_data(9u, r_u16(plane));
    xport_gte_write_data(10u, r_u16(plane + 2u));
    xport_gte_write_data(11u, r_u16(plane + 4u));
    xport_gte_execute(0x41E012u);
    other = xport_gte_read_data(25u);
    step = height * (uint32)(sint32)(sint16)r_u16(plane + 2u);
    other = (uint32)((sint32)other >> 15) + (uint32)(sint32)(sint16)r_u16(plane + 6u);
    step = (uint32)((sint32)step >> 15);
    return (sint32)(other ^ (other + step)) < 0 || (sint32)(other ^ distance) < 0;
}

uint32 level_sub_8006A3BC(uint32 object, uint32 reference)
{
    uint32 state, handle, actor;
    sint32 dx, dy, dz, x, y, z, swap, distance;
    FUNCTION_MARKER(0x8006A3BCu, "LEVEL.BIN");
    dx = (sint16)((sint32)(sint16)r_u16(object + 0x20Eu) - (sint32)(sint16)r_u16(reference + 0x20Eu));
    state = r_u32(reference + 0x388u);
    dy = (sint16)((sint32)(sint16)r_u16(object + 0x212u) - (sint32)(sint16)r_u16(reference + 0x212u));
    dz = (sint16)((sint32)(sint16)r_u16(object + 0x216u) - (sint32)(sint16)r_u16(reference + 0x216u));
    x = dx < 0 ? -dx : dx;
    y = dy < 0 ? -dy : dy;
    z = dz < 0 ? -dz : dz;
    if (x < y) { swap = x; x = y; y = swap; }
    if (x < z) { swap = x; x = z; z = swap; }
    distance = x + ((y + z) >> 2);
    if ((r_u32(object + 16u) & 0x40000u) &&
        (sint32)r_u32(0x80097FE8u) >= (sint32)(r_u32(0x80097FECu) - 1u) &&
        r_u32(0x8009E288u) && distance > 0xDAC) {
        handle = r_u32(state + 12u);
        actor = handle ? r_u32(0x8009D678u + (handle & 0xFFFFu) * 4u) : 0u;
        x = r_u16(actor + 0x344u);
        y = r_u16(actor + 0x34Au);
        z = r_u16(actor + 0x350u);
        xport_gte_write_control(0u, ((uint32)dy << 16) | ((uint32)dx & 0xFFFFu));
        /* TODO: Original upper padding halfword is unspecified and unused by this dot product */
        xport_gte_write_control(1u, (uint32)dz & 0xFFFFu);
        xport_gte_write_data(9u, (uint32)x);
        xport_gte_write_data(10u, (uint32)y);
        xport_gte_write_data(11u, (uint32)z);
        xport_gte_execute(0x41E012u);
        if ((sint32)xport_gte_read_data(25u) < 0) {
            w_u32(object + 0x298u, 1u);
            w_u32(object + 0x290u, 1u);
            return 0u;
        }
    }
    return distance <= (sint32)r_u32(state + 0xC4u);
}

void level_sub_8006A570(uint32 object)
{
    uint32 life;
    FUNCTION_MARKER(0x8006A570u, "LEVEL.BIN");
    if (!moh_draft_missing_call(0x8006A3BCu, object, r_u32(0x8009D638u))) {
        life = r_u32(object + 0x290u);
        w_u32(object + 0x29Cu, 0u);
        w_u32(object + 0x290u, life - 1u);
    }
}

uint32 level_sub_80050720(uint32 object, uint32 resources)
{
    uint32 state, count, first, second, value;
    FUNCTION_MARKER(0x80050720u, "LEVEL.BIN");
    state = r_u32(object + 0x388u);
    first = state + 0x1Cu;
    second = state + 0x24u;
    w_u32(state + 0x18u, 0u);
    for (count = 0u; ; ++count) {
        if (!r_u32(resources)) return 1u;
        if (count == 3u) return 0u;
        w_u32(state + 0x18u, r_u32(state + 0x18u) + 1u);
        value = r_u32(resources);
        if (value != 0xFFFFFFFFu)
            moh_draft_missing_call(0x8006B1C4u, first + 0x94u, value, 0x157u, 0u);
        moh_draft_missing_call(0x8006D84Cu, first + 0xE8u, object);
        w_u32(second + 0xE4u, r_u32(object + 0x100u));
        w_u32(second + 0x1Cu, r_u32(object + 0x270u));
        value = r_u32(object + 0x274u);
        resources += 4u;
        w_u32(second + 0x20u, value);
        value = r_u32(object + 0x278u);
        w_u32(second, 0u);
        w_u32(second + 0x24u, value);
        second += 0x144u;
        w_u32(first, 0u);
        first += 0x144u;
    }
}

uint32 level_sub_8006D670(void)
{
    uint32 result;
    FUNCTION_MARKER(0x8006D670u, "LEVEL.BIN");
    result = moh_draft_missing_call(0x80036DA0u, 0x124u, 1u);
    if (result) w_u32(result + 4u, 0u);
    return result;
}

void level_sub_80083F64(uint32 object, uint32 packet)
{
    uint32 list, count, value;
    FUNCTION_MARKER(0x80083F64u, "LEVEL.BIN");
    list = r_u32(r_u32(packet + 4u) + 0x3Cu);
    if (!list) return;
    count = r_u32(list);
    list += 4u;
    while (count) {
        value = r_u32(list);
        list += 4u;
        --count;
        moh_draft_missing_call(0x800662CCu, value, 0u, packet, object);
    }
}

void level_sub_80083FE8(uint32 object, uint32 packet)
{
    uint32 list, count, value;
    FUNCTION_MARKER(0x80083FE8u, "LEVEL.BIN");
    list = r_u32(r_u32(packet + 4u) + 0x44u);
    if (!list) return;
    count = r_u32(list);
    list += 4u;
    while (count) {
        value = r_u32(list);
        list += 4u;
        --count;
        moh_draft_missing_call(0x800662CCu, value, 0u, packet, object);
    }
}

void level_sub_80074DC4(uint32 packet)
{
    FUNCTION_MARKER(0x80074DC4u, "LEVEL.BIN");
    if (r_u32(r_u32(packet + 4u)) == 0x3011B913u)
        moh_draft_missing_call(0x80061518u, packet);
}

void level_sub_8008406C(uint32 object, uint32 packet)
{
    uint32 flags;
    FUNCTION_MARKER(0x8008406Cu, "LEVEL.BIN");
    if (!r_u32(r_u32(packet + 4u) + 12u)) {
        level_sub_80074DC4(packet);
        return;
    }
    flags = r_u32(object + 12u);
    if (flags & 1u) level_sub_80083F64(object, packet);
    else if (flags & 0x50u) level_sub_80083FE8(object, packet);
}

uint32 level_sub_800563F0(uint32 trigger, uint32 material)
{
    uint32 record, limit, defaults[4], object, pointer, state, extra, kind, mode;
    uint32 value, health, id, numerator, denominator, local;
    sint32 quotient;
    FUNCTION_MARKER(0x800563F0u, "LEVEL.BIN");
    record = r_u32(trigger + 4u);
    limit = r_u32(0x80097FECu);
    defaults[0] = r_u32(0x8003C19Cu);
    defaults[1] = r_u32(0x8003C1A0u);
    defaults[2] = r_u32(0x8003C1A4u);
    defaults[3] = r_u32(0x8003C1A8u);
    if ((sint32)limit > 0 && (!record || (sint32)r_u32(0x80097FE8u) >= (sint32)limit)) return 0u;
    if ((sint32)r_u32(0x800AB8C8u) >= 4 || !r_u32(0x80097FECu)) return 0u;
    value = r_u8(0x80093A06u) ? 0x285u : 0x85u;
    object = moh_draft_missing_call(0x8006B5B8u, 3u, 0xBF0u, r_u32(material), value, 0u, 0x28u, 0u, material);
    if (!object) return 0u;
    moh_draft_missing_call(0x8005C97Cu, object, 0x101u, 0u);
    pointer = r_u32(record + 0x74u);
    if (r_u32(pointer) >= 2u) {
        defaults[0] = r_u32(pointer + 8u);
        pointer = r_u32(record + 0x74u);
        if (r_u32(pointer) >= 3u) {
            value = r_u32(pointer + 12u);
            if (value != 0x4CBF0BA5u) defaults[1] = value;
        }
        pointer = r_u32(record + 0x74u);
        if (r_u32(pointer) >= 4u) {
            defaults[2] = 0x61282777u;
            moh_draft_missing_call(0x8005C97Cu, object, 0x101u, 1u);
        }
    }
    mode = defaults[1] == 0xFFFFFFFFu ? 0x20u : 1u;
    for (kind = 0u; kind < 4u; ++kind)
        if (r_u32(0x80097FF0u + kind * 0x30u) == defaults[0]) break;
    state = r_u32(object + 0x388u);
    extra = moh_draft_missing_call(0x8006D670u);
    w_u32(object + 0x38Cu, extra);
    if (!extra && !object) return 0u;
    w_u32(object + 12u, 0x1300903Cu);
    w_u32(object + 20u, 0x88204401u);
    w_u32(object + 8u, 0x1402u);
    w_u32(object + 16u, 0x68000u);
    value = (uint32)(sint32)(sint16)r_u16(record + 0x68u);
    w_u32(object + 0x294u, value);
    w_u32(object + 0x290u, value);
    health = r_u8(record + 0x61u);
    id = r_u8(0x80099B30u);
    w_u16(object + 0x2DAu, 0xFF88u);
    w_u16(object + 0x2E8u, 0x96u);
    w_u16(object + 0x2ECu, 0x3Cu);
    w_u16(object + 0x3C8u, 0x3Cu);
    w_u8(0x80099B30u, id + 1u);
    w_u8(object + 0x35u, id);
    w_u32(object + 0xF4u, health << 16);
    w_u32(object + 0xF0u, health << 16);
    w_u32(state + 0x18u, 0u);
    moh_draft_missing_call(0x8006D84Cu, object + 0x30u, 0u);
    w_u16(object + 0x2EAu, 0u);
    moh_draft_missing_call(0x8005C97Cu, object, 0xFu, kind);
    numerator = r_u32(object + 0xF4u) * 100u;
    denominator = r_u32(object + 0xF0u);
    if (!denominator || (numerator == 0x80000000u && denominator == 0xFFFFFFFFu)) {
        fprintf(stderr, "LEVEL563F0 invalid signed health division\n"); abort();
    }
    quotient = (sint32)numerator / (sint32)denominator;
    moh_draft_missing_call(0x8005C97Cu, object, 0xDu, (uint32)quotient);
    moh_draft_missing_call(0x8005C97Cu, object, 0x22u, (uint32)(sint32)(sint16)r_u16(record + 0x6Eu));
    moh_draft_missing_call(0x8005C97Cu, object, 0x23u, 0u);
    moh_draft_missing_call(0x8005C97Cu, object, 0x10u, r_u8(record + 0x5Eu));
    value = r_u8(record + 0x5Cu);
    value = value == 0u ? 10u : value == 1u ? 20u : value == 2u ? 1000u : 100u;
    moh_draft_missing_call(0x8005C97Cu, object, 0x24u, value);
    value = r_u8(record + 0x5Bu);
    value = value == 0u ? 33u : value == 2u ? 100u : 67u;
    moh_draft_missing_call(0x8005C97Cu, object, 8u, value);
    w_u32(object + 0x36Cu, 0x80055628u);
    w_u32(object + 0x370u, 0x8005608Cu);
    w_u32(object + 0x374u, 0x80055864u);
    w_u32(object + 0x378u, 0x80056250u);
    w_u32(object + 0x384u, 0x80056288u);
    w_u32(object + 0x37Cu, 0x8005291Cu);
    w_u16(extra + 0xAAu, 0xDu);
    w_u16(extra + 0xB0u, r_u16(record + 0x6Cu));
    w_u16(extra + 0xB2u, r_u16(record + 0x70u));
    if ((sint16)r_u16(extra + 0xB2u) < 10) w_u16(extra + 0xB2u, 0xAF0u);
    moh_draft_missing_call(0x8005C97Cu, object, 5u, (uint32)(sint32)(sint16)r_u16(extra + 0xB2u));
    moh_draft_missing_call(0x8005C97Cu, object, 6u, (uint32)((sint32)(sint16)r_u16(extra + 0xB2u) >> 1));
    moh_draft_missing_call(0x8005C97Cu, object, 7u, (uint32)(sint32)(sint16)((sint32)(sint16)r_u16(extra + 0xB2u) / 3));
    moh_draft_missing_call(0x8005C97Cu, object, 0x26u, r_u8(record + 0x65u));
    value = r_u8(record + 0x66u);
    value = value == 30u ? 1u : r_u8(record + 0x66u);
    moh_draft_missing_call(0x8005C97Cu, object, 0xBu, value);
    w_u16(extra + 0xC8u, r_u8(record + 0x63u));
    value = (uint32)(sint32)(sint16)r_u16(record + 0x6Au);
    w_u16(extra + 0xB6u, 0xFFFFu);
    w_u16(extra + 0xBEu, 1u);
    w_u16(extra + 0xBAu, 2u);
    w_u16(extra + 0xC0u, 0u);
    w_u16(extra + 0xC2u, 0u);
    w_u32(extra + 0xD8u, 0u);
    w_u16(extra + 0xB4u, value * 11u);
    w_u16(extra + 0xC4u, r_u16(object + 0x2ECu));
    w_u16(extra + 0xC6u, r_u16(object + 0x2E8u));
    value = r_u16(object + 0x3C8u);
    w_u32(extra + 0xDCu, 0x10000u);
    w_u16(extra + 0xD0u, 100u);
    w_u16(extra + 0xCCu, 100u);
    w_u16(extra + 0xD2u, 100u);
    w_u16(extra + 0xCEu, 100u);
    w_u16(extra + 0xCAu, value);
    local = moh_draft_missing_call(0x80016718u, 16u);
    for (value = 0u; value < 4u; ++value) w_u32(local + value * 4u, defaults[value]);
    moh_draft_missing_call(0x80050720u, object, local);
    w_u8(state + 0x11u, 0u);
    w_u8(state + 0x10u, 0u);
    w_u8(state + 0x14u, 0u);
    w_u8(state + 0x12u, 0u);
    w_u32(state + 0x1Cu, 0x41u);
    w_u32(state + 0x24u, 0x15u);
    if (kind < 4u) {
        w_u16(state + 0x30u, kind == 0u ? 15u : kind == 2u ? 0u : 5u);
        w_u16(state + 0x32u, 0u);
        w_u16(state + 0x34u, kind == 0u ? 15u : kind == 2u ? 50u : 25u);
    }
    w_u32(state + 0x168u, 0xDu);
    w_u16(state + 0x174u, 0xFFF9u);
    w_u16(state + 0x176u, 1u);
    w_u32(state + 0x160u, mode);
    w_u16(state + 0x178u, 0x24u);
    w_u32(0x8009810Cu, defaults[1] == 0x8A522920u ? 0xAA1714FEu : 0u);
    w_u32(state + 0x2ACu, 0x11u);
    moh_draft_missing_call(0x8006DCACu, state + 0xB68u, object, state + 0x1Cu, r_u16(0x80097FFCu + kind * 0x30u), 0u);
    w_u32(0x80097FE8u, r_u32(0x80097FE8u) + 1u);
    moh_draft_missing_call(0x80036E44u, local);
    return object;
}

uint32 level_sub_800540C8(uint32 texture)
{
    uint32 index;
    FUNCTION_MARKER(0x800540C8u, "LEVEL.BIN");
    for (index = 0u; index < 4u; ++index)
        if (r_u32(0x80098970u + index * 4u) == texture) return 1u;
    return 0u;
}

uint32 level_sub_80054238(uint32 texture)
{
    uint32 current, index;
    FUNCTION_MARKER(0x80054238u, "LEVEL.BIN");
    current = r_u32(0x80099070u);
    for (index = 0u; index < 4u; ++index)
        if ((uint32)(sint32)(sint16)r_u16(current + 0x30u + index * 2u) == texture) return 0u;
    return 1u;
}

void level_sub_80054100(void)
{
    uint32 index, count = 0u, texture;
    FUNCTION_MARKER(0x80054100u, "LEVEL.BIN");
    for (index = 4u; index; --index) w_u32(0x80099074u + (index - 1u) * 4u, 0xFFFFFFFFu);
    if (!r_u32(0x80099070u)) return;
    for (index = 0u; index < 4u; ++index) {
        texture = (uint32)(sint32)(sint16)r_u16(r_u32(0x80099070u) + 0x30u + index * 2u);
        if (!level_sub_800540C8(texture)) {
            texture = (uint32)(sint32)(sint16)r_u16(r_u32(0x80099070u) + 0x30u + index * 2u);
            w_u32(0x80099074u + count * 4u, texture); ++count;
        }
    }
    w_u32(0x80098964u, count);
}

void level_sub_80054298(void)
{
    uint32 index, texture, count;
    FUNCTION_MARKER(0x80054298u, "LEVEL.BIN");
    w_u32(0x80098968u, 0u);
    for (index = 4u; index; --index) w_u32(0x80099084u + (index - 1u) * 4u, 0xFFFFFFFFu);
    if (!r_u32(0x80099070u)) return;
    for (index = 0u; index < 4u; ++index) {
        texture = r_u32(0x80098970u + index * 4u);
        if (texture != 0xFFFFFFFFu && level_sub_80054238(texture)) {
            count = r_u32(0x80098968u);
            w_u32(0x80099084u + count * 4u, index); w_u32(0x80098968u, count + 1u);
        }
    }
}

static void level_asset_copy_pair(uint32 destination, uint32 source)
{
    uint32 first = r_u32(source), second = r_u32(source + 4u);
    w_u32(destination, first);
    w_u32(destination + 4u, second);
}

static void level_asset_inherit(uint32 material, uint32 parent)
{
    static const uint32 inherited_words[12] = {
        8u, 24u, 32u, 36u, 40u, 44u, 48u, 52u, 56u, 132u, 156u, 196u
    };
    uint32 index, field;
    if (!r_u32(material + 176u)) {
        w_u32(material + 176u, r_u32(parent + 176u));
        w_u32(material + 180u, r_u32(parent + 180u));
    }
    if (!r_u32(material + 160u)) {
        w_u32(material + 160u, r_u32(parent + 160u));
        if (!r_u32(material + 160u)) w_u32(material + 160u, r_u32(parent + 160u));
    }
    if (!r_u32(material + 164u) && !r_u32(material + 168u) && !r_u32(material + 172u)) {
        w_u32(material + 164u, r_u32(parent + 164u));
        w_u32(material + 168u, r_u32(parent + 168u));
        w_u32(material + 172u, r_u32(parent + 172u));
    }
    if (!r_u16(material + 200u)) w_u16(material + 200u, r_u16(parent + 200u));
    if (!r_u32(material + 4u)) {
        w_u32(material + 4u, r_u32(parent + 4u));
        level_asset_copy_pair(material + 60u, parent + 60u);
        for (index = 0u; index < 8u; ++index) {
            w_u16(material + 68u + index * 8u, r_u16(parent + 68u + index * 8u));
            w_u16(material + 70u + index * 8u, r_u16(parent + 70u + index * 8u));
            w_u16(material + 72u + index * 8u, r_u16(parent + 72u + index * 8u));
        }
        w_u32(material + 136u, r_u32(parent + 136u));
        w_u16(material + 140u, r_u16(parent + 140u));
        w_u16(material + 142u, r_u16(parent + 142u));
        w_u32(material + 144u, r_u32(parent + 144u));
        w_u16(material + 150u, r_u16(parent + 150u));
        w_u32(material + 152u, r_u32(parent + 152u));
        w_u16(material + 204u, r_u16(parent + 204u));
        w_u16(material + 206u, r_u16(parent + 206u));
        w_u16(material + 208u, r_u16(parent + 208u));
    }
    if (!r_u32(material + 12u)) {
        w_u32(material + 12u, r_u32(parent + 12u));
        w_u16(material + 148u, r_u16(parent + 148u));
    }
    if ((sint16)r_u16(material + 212u) < 0) {
        w_u16(material + 212u, r_u16(parent + 212u));
        level_asset_copy_pair(material + 216u, parent + 216u);
        if ((sint16)r_u16(material + 212u) < 0) {
            w_u16(material + 214u, r_u16(parent + 214u));
            level_asset_copy_pair(material + 224u, parent + 224u);
        }
    }
    if (!r_u32(material + 192u)) {
        w_u32(material + 192u, r_u32(parent + 192u));
        w_u16(material + 190u, r_u16(parent + 190u));
    }
    if (!r_u32(material + 184u)) {
        w_u32(material + 184u, r_u32(parent + 184u));
        w_u16(material + 188u, r_u16(parent + 188u));
    }
    if (!r_u32(material + 16u)) {
        w_u32(material + 16u, r_u32(parent + 16u));
        w_u32(material + 20u, r_u32(parent + 20u));
    }
    for (index = 0u; index < 12u; ++index) {
        field = inherited_words[index];
        if (!r_u32(material + field)) w_u32(material + field, r_u32(parent + field));
    }
    if (r_u32(material + 252u) == 0xFFFFFFFFu)
        w_u32(material + 252u, r_u32(parent + 252u));
    for (index = 0u; index < 4u; ++index) {
        if (r_u32(material + 236u + index * 4u) == 0xFFFFFFFFu) {
            for (index = 0u; index < 4u; ++index)
                w_u32(material + 236u + index * 4u, r_u32(parent + 236u + index * 4u));
            break;
        }
    }
}

static void level_asset_progress(sint16 width)
{
    PSX_RECT rectangle = {142, 207, width, 6};
    ClearImage(&rectangle, r_u8(0x80039D27u), r_u8(0x80039D28u), r_u8(0x80039D29u));
    DrawSync(0);
}

uint32 level_sub_80087308(uint32 mission, uint32 level)
{
    static const uint32 relocation_fields[9] = {
        0x544u, 0x54Cu, 0x554u, 0x55Cu, 0x564u, 0x56Cu, 0x574u, 0x57Cu, 0x584u
    };
    uint32 temporary, file, asset, base, node, end, field, value, index;
    uint32 material, parent, parent_index, selected_count = 0u, scene, scene_data, object;
    uint32 prefix, mode, color[3], axis;
    sint32 fog;
    int length;
    FUNCTION_MARKER(0x80087308u, "LEVEL.BIN");
    moh_draft_missing_call(0x80086B20u, 0u);
    temporary = sub_80036BF0(136u);
    if (!temporary) abort();
    prefix = 0x800939E0u;
    if (!r_u8(prefix)) {
        mode = r_u32(0x8009A618u);
        length = snprintf((char *)moh_draft_guest_pointer(temporary, 128u), 128u,
            (const char *)moh_draft_guest_pointer(0x8003D3BCu, 1u),
            (long)(sint32)mission, (long)(sint32)level, (long)(sint32)mission,
            (long)(sint32)level, (int)mode);
    } else {
        length = snprintf((char *)moh_draft_guest_pointer(temporary, 128u), 128u,
            (const char *)moh_draft_guest_pointer(0x8003D3E0u, 1u),
            (const char *)moh_draft_guest_pointer(prefix, 1u), (long)(sint32)mission, (long)(sint32)level);
    }
    /* TODO: Confirm the 128-byte original path capacity for custom resource prefixes */
    if (length < 0 || length >= 128) abort();
    file = moh_draft_missing_call(0x800363FCu, temporary, 0u, 0u);
    if (file) {
        value = moh_draft_missing_call(0x800869E0u, file);
        moh_draft_missing_call(0x80086A94u, value, 0u);
        sub_80036E44(file);
    }
    level_asset_progress(90);
    if (!r_u8(prefix)) {
        length = snprintf((char *)moh_draft_guest_pointer(temporary, 128u), 128u,
            (const char *)moh_draft_guest_pointer(0x8003D3F0u, 1u),
            (long)(sint32)mission, (long)(sint32)level, (long)(sint32)mission, (long)(sint32)level);
    } else {
        length = snprintf((char *)moh_draft_guest_pointer(temporary, 128u), 128u,
            (const char *)moh_draft_guest_pointer(0x8003D410u, 1u),
            (const char *)moh_draft_guest_pointer(prefix, 1u), (long)(sint32)mission, (long)(sint32)level);
    }
    if (length < 0 || length >= 128) abort();
    asset = moh_draft_missing_call(0x8004A374u, temporary, temporary + 128u);
    level_asset_progress(120);
    w_u32(0x8009CC64u, asset);
    moh_draft_missing_call(0x80087078u);
    base = r_u32(0x8009CC64u);
    if (r_u32(base + 160u)) {
        for (index = 0u; index < 4u; ++index) w_u16(base + 176u + index * 2u, 0u);
    }
    w_u32(0x8009EC58u, asset + 1448u);
    base = r_u32(0x8009CC64u);
    end = base + 800u;
    w_u32(0x8009EC54u, r_u32(temporary + 128u) + 1u);
    node = base;
    do {
        if (r_u32(node + 220u)) w_u32(node + 224u, asset + r_u32(node + 224u));
        else w_u32(node + 224u, 0u);
        w_u32(node + 236u, 0u);
        node += 20u;
    } while (node < end);
    base = r_u32(0x8009CC64u);
    for (index = 0u; index < 20u; ++index) {
        w_u16(base + 1034u + index * 16u, 0u);
        w_u16(base + 1032u + index * 16u, 0u);
    }
    for (index = 0u; index < 9u; ++index) {
        base = r_u32(0x8009CC64u);
        field = relocation_fields[index];
        value = r_u32(base + field);
        if (value) w_u32(base + field, asset + value);
    }
    level_asset_progress(150);
    moh_draft_missing_call(0x80086C9Cu, r_u32(0x8009CC64u));
    moh_draft_missing_call(0x80086D18u, r_u32(0x8009CC64u));
    moh_draft_missing_call(0x80086D8Cu, r_u32(0x8009CC64u));
    moh_draft_missing_call(0x80086E34u, r_u32(0x8009CC64u));
    moh_draft_missing_call(0x80086EB0u, r_u32(0x8009CC64u));
    moh_draft_missing_call(0x80086F24u, r_u32(0x8009CC64u));
    for (index = 0u; index < r_u32(0x8009EC54u); ++index) {
        material = moh_draft_missing_call(0x8006A8FCu, index);
        moh_draft_missing_call(0x80087080u, material, r_u32(0x8009CC64u), asset);
    }
    for (index = 0u; index < r_u32(0x8009EC54u); ++index) {
        material = moh_draft_missing_call(0x8006A8FCu, index);
        if (r_u32(material + 232u) != 0xFFFFFFFFu) {
            parent = 0u;
            for (parent_index = 0u; parent_index < r_u32(0x8009EC54u); ++parent_index) {
                parent = moh_draft_missing_call(0x8006A8FCu, parent_index);
                if (r_u32(parent) == r_u32(material + 232u)) break;
            }
            level_asset_inherit(material, parent);
        }
        if (r_u32(material + 252u) == 1u) {
            object = 0x80093640u;
            for (parent_index = 0u; parent_index < 13u; ++parent_index, object += 68u) {
                if (r_u32(object) == r_u32(material)) {
                    w_u8(0x8009E878u + selected_count, (uint8)parent_index);
                    ++selected_count;
                    w_u32(object + 64u, material);
                    break;
                }
            }
        }
    }
    level_asset_progress(180);
    w_u32(0x8009E874u, selected_count);
    scene = moh_draft_missing_call(0x80053E68u);
    scene_data = r_u32(scene + 904u);
    w_u32(0x8009D648u, scene);
    base = r_u32(0x8009CC64u);
    value = r_u32(base + 1436u);
    if (value) w_u32(base + 1436u, asset + value);
    base = r_u32(0x8009CC64u);
    value = r_u32(base + 1340u);
    if (value) {
        value += asset;
        w_u32(base + 1340u, value);
        w_u32(scene_data, value);
        moh_draft_missing_call(0x8007529Cu, r_u32(0x8009D648u));
    }
    base = r_u32(0x8009CC64u);
    value = r_u32(base + 1432u);
    if (value) {
        value += asset;
        w_u32(base + 1432u, value);
        moh_draft_missing_call(0x80086F98u, value);
    }
    base = r_u32(0x8009CC64u);
    value = r_u32(base + 1344u);
    if (value) {
        value += asset;
        w_u32(base + 1344u, value);
        w_u32(scene_data + 4u, value);
        w_u32(scene_data + 8u, value + 8u);
        w_u32(scene_data + 12u, r_u32(value));
    }
    base = r_u32(0x8009CC64u);
    value = r_u32(base + 1440u);
    if (value) {
        value += asset;
        w_u32(base + 1440u, value);
        moh_draft_missing_call(0x8007540Cu, value);
    } else {
        w_u32(0x8009E290u, 0u);
        w_u32(0x8009E28Cu, 0u);
    }
    base = r_u32(0x8009CC64u);
    fog = (sint16)r_u16(base + 204u);
    for (axis = 0u; axis < 3u; ++axis) color[axis] = r_u8(base + 206u + axis);
    if (fog) {
        SetBackColor((sint32)color[0], (sint32)color[1], (sint32)color[2]);
        if ((sint16)r_u16(base + 212u)) {
            SetFarColor(128, 128, 128);
        } else {
            SetFarColor((sint32)color[0], (sint32)color[1], (sint32)color[2]);
        }
    } else {
        SetBackColor(0, 0, 0);
        SetFarColor((sint32)color[0], (sint32)color[1], (sint32)color[2]);
    }
    SetFogNear(fog, 400);
    moh_draft_missing_call(0x80066108u);
    moh_draft_missing_call(0x80087294u);
    moh_draft_missing_call(0x8006B834u, r_u32(0x8009D648u));
    moh_draft_missing_call(0x80061130u);
    level_asset_progress(210);
    moh_draft_missing_call(0x800412E0u);
    w_u32(0x8009EC50u, 1u);
    /* TODO: Confirm loaders do not retain pointers to temporary filename/count storage */
    sub_80036E44(temporary);
    return 0u;
}



void level_sub_80086B20(uint32 mode)
{
    uint32 object, next, callback;
    FUNCTION_MARKER(0x80086B20u, "LEVEL.BIN");
    if (!r_u32(0x8009EC50u)) return;
    if ((sint32)r_u32(0x800939F8u) > 0) {
        moh_draft_missing_call(0x80015B28u, 0x8003D2ACu, r_u32(0x80039D88u) << 4);
        moh_draft_missing_call(0x80016B60u, 1u);
        moh_draft_missing_call(0x80061160u);
        moh_draft_missing_call(0x80086B18u);
        moh_draft_missing_call(0x80015B28u, 0x8003D2C4u);
        moh_draft_missing_call(0x8005F260u);
        for (object = r_u32(0x8009D64Cu); object; object = next) {
            callback = r_u32(object + 900u);
            next = r_u32(object + 4u);
            if (callback) {
                moh_draft_missing_call(0x80015B28u, 0x8003D2E0u, r_u32(object + 8u));
                callback = r_u32(object + 900u);
                moh_draft_indirect_call(callback, object);
            }
        }
        moh_draft_missing_call(0x80015B28u, 0x8003D2ACu, r_u32(0x80039D88u) << 4);
        moh_draft_missing_call(0x80066290u);
        w_u32(0x8009D64Cu, 0u);
        w_u32(0x8009EC54u, 0u);
        moh_draft_missing_call(0x80015B28u, 0x8003D2FCu);
        moh_draft_missing_call(0x8008A634u);
        moh_draft_missing_call(0x8008EA70u);
        moh_draft_missing_call(0x80015B28u, 0x8003D318u);
        moh_draft_missing_call(0x80036AF8u);
        moh_draft_missing_call(0x80015B28u, 0x8003D2ACu, r_u32(0x80039D88u) << 4);
        moh_draft_missing_call(0x8006463Cu, mode & 255u);
        moh_draft_missing_call(0x80015B28u, 0x8003D328u);
        moh_draft_missing_call(0x800427CCu);
    }
    w_u32(0x8009EC50u, 0u);
}

uint32 level_sub_8004A300(uint32 header, uint32 count_out)
{
    sint32 count;
    uint32 cursor, allocation;
    FUNCTION_MARKER(0x8004A300u, "LEVEL.BIN");
    w_u32(count_out, 0u);
    count = (sint32)r_u32(header);
    if (count <= 0) return 0u;
    cursor = header + ((uint32)count << 2);
    do {
        allocation = sub_80036BF0(r_u32(cursor));
        if (allocation) {
            w_u32(count_out, (uint32)count);
            return allocation;
        }
        --count;
        cursor -= 4u;
    } while (count > 0);
    return 0u;
}

sint32 level_sub_800418E4(uint32 vab, uint32 bank)
{
    sint32 signed_bank = (sint32)(sint16)(bank & 0xFFFFu);
    uint32 table_offset = (uint32)signed_bank * 4u;
    uint32 header_bytes, sample_bytes, sample_data, index, result;
    FUNCTION_MARKER(0x800418E4u, "LEVEL.BIN");
    header_bytes = (uint32)r_u16(vab + 18u) * 512u + 0xA20u;
    sample_bytes = r_u32(vab + 12u) - header_bytes;
    if (!r_u32(0x80091128u + table_offset)) {
        if ((bank & 0xFFFFu) >= 8u) return -1;
        if (r_u32(vab) != 0x56414270u) return -1;
        if ((sint32)(r_u32(0x800A0130u) + sample_bytes) > 0x7FFFF) {
            moh_draft_missing_call(0x80089B04u, 5u, 16u);
            return -1;
        }
        sample_data = vab + header_bytes;
        moh_draft_missing_call(0x800156DCu, r_u32(0x800A0130u));
        moh_draft_missing_call(0x8001575Cu, sample_data, sample_bytes);
        moh_draft_missing_call(0x80015C88u, 1u);
        result = 0x8009263Cu + (uint32)signed_bank * 512u;
        w_u32(0x80091108u + table_offset, result);
        moh_draft_missing_call(0x800154ECu, result, sample_data - 512u, 512u);
        w_u32(0x800910E8u + table_offset, r_u32(0x800A0130u));
        for (index = 0u; index < 256u; ++index) {
            result = moh_draft_missing_call(0x8004270Cu, index, (uint32)r_u16(vab + 18u), vab);
            w_u16(0x8009163Cu + (uint32)signed_bank * 2u + index * 16u, (uint16)result);
        }
        w_u32(0x80091128u + table_offset, 1u);
        w_u32(0x800A0130u, r_u32(0x800A0130u) + sample_bytes);
    }
    return (sint32)(sint16)r_u16(0x80091148u + (uint32)signed_bank * 2u);
}

static void level_asset_fixup_word(uint32 field, uint32 base, uint32 limit, uint32 function)
{
    uint32 offset = r_u32(field);
    w_u32(field, offset == 0xFFFFFFFFu ? 0u : base + offset);
    if (r_u32(field) >= limit) {
        fprintf(stderr, "LEVEL 0x%08X: BSD relocation break 0x3FF at 0x%08X\n", function, field);
        abort();
    }
}

void level_sub_80086C9C(uint32 bsd)
{
    uint32 size = r_u32(bsd + 0x550u), base = r_u32(bsd + 0x54Cu);
    uint32 count = r_u32(bsd + 0x548u), record = r_u32(bsd + 0x544u), index;
    uint32 limit = base + size * 20u;
    FUNCTION_MARKER(0x80086C9Cu, "LEVEL.BIN");
    for (index = 0u; index < count; ++index, record += 8u)
        level_asset_fixup_word(record + 4u, base, limit, 0x80086C9Cu);
}

void level_sub_80086D18(uint32 bsd)
{
    uint32 size = r_u32(bsd + 0x558u), base = r_u32(bsd + 0x554u);
    uint32 count = r_u32(bsd + 0x550u), record = r_u32(bsd + 0x54Cu), index;
    uint32 limit = base + size * 8u;
    FUNCTION_MARKER(0x80086D18u, "LEVEL.BIN");
    for (index = 0u; index < count; ++index, record += 20u)
        level_asset_fixup_word(record + 16u, base, limit, 0x80086D18u);
}

void level_sub_80086D8C(uint32 bsd)
{
    uint32 count = r_u32(bsd + 0x560u), base = r_u32(bsd + 0x55Cu);
    uint32 limit = base + count * 20u, record = base, index;
    FUNCTION_MARKER(0x80086D8Cu, "LEVEL.BIN");
    for (index = 0u; index < count; ++index, record += 20u) {
        level_asset_fixup_word(record + 12u, base, limit, 0x80086D8Cu);
        level_asset_fixup_word(record + 16u, base, limit, 0x80086D8Cu);
    }
}

void level_sub_80086E34(uint32 bsd)
{
    uint32 size = r_u32(bsd + 0x570u), base = r_u32(bsd + 0x56Cu);
    uint32 count = r_u32(bsd + 0x568u), record = r_u32(bsd + 0x564u), index;
    uint32 limit = base + size * 28u;
    FUNCTION_MARKER(0x80086E34u, "LEVEL.BIN");
    for (index = 0u; index < count; ++index, record += 8u)
        level_asset_fixup_word(record, base, limit, 0x80086E34u);
}

void level_sub_80086EB0(uint32 bsd)
{
    uint32 size = r_u32(bsd + 0x580u), base = r_u32(bsd + 0x57Cu);
    uint32 count = r_u32(bsd + 0x578u), record = r_u32(bsd + 0x574u), index;
    uint32 limit = base + size * 8u;
    FUNCTION_MARKER(0x80086EB0u, "LEVEL.BIN");
    for (index = 0u; index < count; ++index, record += 8u)
        level_asset_fixup_word(record, base, limit, 0x80086EB0u);
}

void level_sub_80086F24(uint32 bsd)
{
    uint32 size = r_u32(bsd + 0x588u), base = r_u32(bsd + 0x584u);
    uint32 count = r_u32(bsd + 0x580u), record = r_u32(bsd + 0x57Cu), index;
    uint32 limit = base + size * 8u;
    FUNCTION_MARKER(0x80086F24u, "LEVEL.BIN");
    for (index = 0u; index < count; ++index, record += 8u)
        level_asset_fixup_word(record, base, limit, 0x80086F24u);
}

sint32 level_sub_8003F788(uint32 material)
{
    uint32 source, index, record;
    FUNCTION_MARKER(0x8003F788u, "LEVEL.BIN");
    source = r_u32(material + 12u);
    if (!source) {
        w_u32(material + 12u, 0u);
        return -1;
    }
    for (index = 0u; (sint32)index < (sint32)r_u32(material + 176u); ++index)
        w_u32(r_u32(material + 180u) + index * 60u, 0u);
    w_u16(material + 188u, r_u16(source));
    w_u32(material + 184u, source + 8u);
    w_u16(material + 148u, r_u16(source + 4u));
    for (index = 0u; index < (uint32)r_u16(material + 188u); ++index) {
        record = r_u32(material + 184u) + index * 16u;
        w_u32(record + 12u, source + r_u32(record + 12u));
    }
    return 0;
}

uint32 level_sub_8006D888(uint32 slot, uint32 generation)
{
    FUNCTION_MARKER(0x8006D888u, "LEVEL.BIN");
    return (generation << 16) | (slot & 0xFFFFu);
}

void level_sub_8006D930(uint32 handle_out, uint32 object)
{
    uint32 slot = r_u32(0x8009D674u), generation, index;
    FUNCTION_MARKER(0x8006D930u, "LEVEL.BIN");
    if (slot < 256u) {
        w_u32(0x8009D678u + slot * 4u, object);
        generation = r_u32(0x8009D670u) + 1u;
        w_u32(0x8009D670u, generation);
        w_u32(0x8009DA78u + slot * 4u, generation);
        w_u32(object, level_sub_8006D888(slot, generation));
        slot = r_u32(0x8009D674u);
        w_u32(0x8009DE78u + slot * 4u, 0u);
        w_u32(0x8009D674u, slot + 1u);
    } else {
        for (index = 0u; index < 256u; ++index) {
            if (!r_u32(0x8009D678u + index * 4u)) {
                generation = r_u32(0x8009D670u) + 1u;
                w_u32(0x8009D678u + index * 4u, object);
                w_u32(0x8009D670u, generation);
                w_u32(0x8009DA78u + index * 4u, generation);
                w_u32(object, level_sub_8006D888(index, generation));
                w_u32(0x8009DE78u + index * 4u, 0u);
                break;
            }
        }
    }
    w_u32(handle_out, r_u32(object));
}

void level_sub_80075120(uint32 geometry)
{
    uint32 field, offset;
    FUNCTION_MARKER(0x80075120u, "LEVEL.BIN");
    for (field = 0x38u; field <= 0x50u; field += 4u) {
        offset = r_u32(geometry + field);
        if (offset) w_u32(geometry + field, geometry + offset);
    }
}

void level_sub_800751B4(uint32 geometry)
{
    uint32 offset;
    FUNCTION_MARKER(0x800751B4u, "LEVEL.BIN");
    offset = r_u32(geometry + 0x60u);
    if (offset) w_u32(geometry + 0x60u, geometry + offset);
}

void level_sub_800751D0(uint32 geometry)
{
    uint32 offset;
    FUNCTION_MARKER(0x800751D0u, "LEVEL.BIN");
    offset = r_u32(geometry + 0x60u);
    if (offset) w_u32(geometry + 0x60u, geometry + offset);
}

void level_sub_800751EC(uint32 geometry)
{
    uint32 offset;
    FUNCTION_MARKER(0x800751ECu, "LEVEL.BIN");
    offset = r_u32(geometry + 0x74u);
    if (offset) w_u32(geometry + 0x74u, geometry + offset);
}

uint32 level_sub_80066108(void)
{
    FUNCTION_MARKER(0x80066108u, "LEVEL.BIN");
    w_u32(0x8009D5ACu, r_u32(r_u32(0x8009CC64u) + 0x59Cu));
    if (!moh_draft_missing_call(0x80066790u)) return 0u;
    w_u32(0x8009D5BCu, 0u);
    w_u32(0x8009D5B8u, 0u);
    moh_draft_missing_call(0x800158ACu, 0x8009D5C0u, 0u, 0x58u);
    w_u32(0x8009D5C0u, 0x80069514u);
    w_u32(0x8009D5C4u, 0x80069714u);
    w_u32(0x8009D5C8u, 0x800698F0u);
    w_u32(0x8009D5CCu, 0x80069A08u);
    w_u32(0x8009D5D0u, 0x80069A74u);
    w_u32(0x8009D5D4u, 0x80069ACCu);
    w_u32(0x8009D5D8u, 0x80069B14u);
    w_u32(0x8009D5F4u, 0x80069D04u);
    w_u32(0x8009D5F8u, 0x80069DECu);
    w_u32(0x8009D5FCu, 0x80069E44u);
    w_u32(0x8009D600u, 0x80069E84u);
    w_u32(0x8009D604u, 0x80069EC4u);
    w_u32(0x8009D5DCu, 0x80069D5Cu);
    w_u32(0x8009D5E0u, 0x80069FC4u);
    w_u32(0x8009D5E4u, 0x8006A038u);
    w_u32(0x8009D5E8u, 0x8006A0C8u);
    w_u32(0x8009D5ECu, 0x8006A184u);
    w_u32(0x8009D5F0u, 0x80069968u);
    w_u32(0x8009D608u, 0x80069F2Cu);
    w_u32(0x8009D614u, 0x80069F6Cu);
    w_u32(0x8009D60Cu, 0x80069B88u);
    w_u32(0x8009D610u, 0x80069CC4u);
    w_u32(0x80098628u, 120u);
    return 1u;
}

uint32 level_sub_80066790(void)
{
    uint32 source, count = 0u, handlers, index;
    FUNCTION_MARKER(0x80066790u, "LEVEL.BIN");
    source = r_u32(0x8009D5ACu);
    if (source) count = r_u32(source);
    moh_draft_missing_call(0x8006685Cu);
    handlers = sub_80036BF0(count << 2);
    w_u32(0x8009D5B0u, handlers);
    if (!handlers) {
        moh_draft_missing_call(0x80015B28u, 0x8003C9C8u);
        return 0u;
    }
    for (index = 0u; index < count; ++index)
        w_u32(handlers + index * 4u, 0u);
    w_u32(0x8009D5B4u, 0x800AD460u);
    for (index = 0u; index < 1023u; ++index)
        w_u32(0x800AD460u + index * 36u + 32u, 0x800AD460u + (index + 1u) * 36u);
    w_u32(0x800B645Cu, 0u);
    return 1u;
}

void level_sub_8006685C(void)
{
    uint32 handlers;
    FUNCTION_MARKER(0x8006685Cu, "LEVEL.BIN");
    handlers = r_u32(0x8009D5B0u);
    if (handlers) {
        sub_80036E44(handlers);
        w_u32(0x8009D5B0u, 0u);
    }
}

uint32 level_sub_8004787C(void)
{
    sint32 resource, animation;
    uint32 object, material, data, allocation;
    FUNCTION_MARKER(0x8004787Cu, "LEVEL.BIN");
    resource = (sint32)moh_draft_missing_call(0x8006A910u, 0xCBE42983u);
    if (resource <= 0) {
        object = moh_draft_missing_call(0x8006B5B8u, 0u, 0x120u, 0xFFFFFFFFu, 0u, 0u, 0u, 0u, 0u);
        if (!object) return 0u;
        w_u32(object + 0x370u, 0u);
        w_u32(object + 16u, r_u32(object + 16u) | 0x200u);
    } else {
        material = moh_draft_missing_call(0x8006A8FCu, (uint32)resource);
        object = moh_draft_missing_call(0x8006B5B8u, 1u, 0x120u, 0xCBE42983u, 0x157u, 0u, 5u, 0u, material);
        if (!object) return 0u;
        w_u32(object + 16u, r_u32(object + 16u) | 0x200u);
        material = r_u32(object + 0x100u);
        if (!material || !r_u32(material + 8u)) {
            w_u32(object + 0x370u, 0u);
        } else {
            moh_draft_missing_call(0x8006A76Cu, object);
            material = r_u32(r_u32(object + 0x100u) + 8u);
            animation = (sint32)(sint16)r_u16(material + 0x94u);
            if (animation >= 0) {
                moh_draft_missing_call(0x8003F454u, object, (uint32)animation, 1u, 0u);
                moh_draft_missing_call(0x8003F8F0u, object);
            }
            w_u32(object + 0x370u, 0x80046DD4u);
        }
    }
    data = r_u32(object + 0x388u);
    allocation = moh_draft_missing_call(0x80016718u, 240u);
    allocation = moh_draft_missing_call(0x8008CFE8u, allocation);
    w_u32(data + 200u, allocation);
    moh_draft_missing_call(0x8004770Cu, object);
    return object;
}

uint32 level_sub_800754CC(sint32 id)
{
    uint32 data, record, geometry;
    sint32 count, index;
    FUNCTION_MARKER(0x800754CCu, "LEVEL.BIN");
    data = r_u32(r_u32(0x8009D648u) + 0x388u);
    count = (sint32)r_u32(data + 16u);
    record = r_u32(data + 20u);
    for (index = 0; index < count; ++index, record += 8u) {
        geometry = r_u32(record + 4u);
        if (r_u32(geometry) == 0x8877B646u && (sint32)(sint16)r_u16(geometry + 52u) == id)
            return record;
    }
    moh_draft_missing_call(0x80089B04u, 4u, 56u);
    return 0u;
}
