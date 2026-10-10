#include "game_draft_signatures.h"
#include "game_level_signatures.h"
#include "xport_trace.h"
#include "psx.h"
#include <stdlib.h>

void level_sub_80044470(uint32 object)
{
    uint32 state, value, callback, handle, actor, local, random, speed, decrement;
    FUNCTION_MARKER(0x80044470u, "LEVEL.BIN");
    state = r_u32(object + 0x388u);
    value = (r_u16(state + 0x3Au) - 1u) & 0xFFFFu; w_u16(state + 0x3Au, value);
    if (!value && r_u32(object + 8u)) { moh_draft_missing_call(0x8006B8F0u, object); return; }
    value = r_u8(state + 0x40u);
    if (value) { value = (value - 1u) & 255u; w_u8(state + 0x40u, value); if (!value) w_u32(object + 0x14u, r_u32(state + 0x3Cu)); }
    moh_draft_missing_call(0x8004CDB0u, object);
    if (r_u32(object + 8u) == 0x13F3u) moh_draft_missing_call(0x80044120u, object);
    if (r_u16(state + 0x38u) & 0x40u) {
        value = r_u8(state + 0x41u);
        if (value) {
            value = (value - 1u) & 255u; w_u8(state + 0x41u, value);
            if (!value) {
                callback = r_u32(state + 0x44u);
                if (callback) {
                    moh_draft_indirect_call(callback, object);
                    if (r_u32(object + 12u) & 0x80000u) return;
                } else {
                    if (r_u16(state + 0x38u) & 0x100u) {
                        w_u16(object + 0x368u, 0u); w_u16(object + 0x366u, 0u); w_u16(object + 0x364u, 0u);
                        moh_draft_missing_call(0x800719C0u, object, 8u, 10u, 0x55u, 0x10000u, 0x1000u, 0x40000u, 5u, 0u);
                        random = moh_draft_missing_call(0x8001552Cu);
                        value = (uint32)((sint32)(random * 5u) >> 15) + 15u;
                        moh_draft_missing_call(0x80073BA8u, object + 0x20Cu, 5u, value, 40u, 25u, 80u, 30u, 0x3FFu, 0x3Fu, 0x3E0u, 0xFFu, 1u, 0x10000u, 0u, 0u);
                        moh_draft_missing_call(0x80042DF8u, object);
                    }
                    moh_draft_missing_call(0x8006B8F0u, object); return;
                }
            }
        }
    }
    handle = r_u32(object + 0x30u); actor = handle ? r_u32(0x8009D678u + (handle & 0xFFFFu) * 4u) : 0u;
    if (actor && (sint8)r_u8(object + 0x34u) >= 0) {
        handle = r_u32(object + 0x30u); actor = handle ? r_u32(0x8009D678u + (handle & 0xFFFFu) * 4u) : 0u;
        local = moh_draft_missing_call(0x80016718u, 8u);
        moh_draft_missing_call(0x8008E354u, actor, (uint32)(sint32)(sint8)r_u8(object + 0x34u), local, 0u, 0u);
        w_u32(object + 0x23Cu, 0u); w_u32(object + 0x238u, 0u); w_u32(object + 0x234u, 0u);
        w_u32(object + 0x220u, 0u); w_u32(object + 0x21Cu, 0u); w_u32(object + 0x218u, 0u); w_u32(object + 0x200u, 0u);
        w_u16(state + 0x38u, r_u16(state + 0x38u) & 0xFDF6u); moh_draft_missing_call(0x80036E44u, local); return;
    }
    value = r_u32(state + 0x68u); decrement = r_u32(state + 0x74u);
    handle = r_u32(state + 0x6Cu); actor = r_u32(state + 0x78u);
    w_u32(state + 0x68u, value + decrement);
    value = r_u32(state + 0x70u); w_u32(state + 0x6Cu, handle + actor);
    handle = r_u32(state + 0x7Cu); actor = r_u16(state + 0x38u); w_u32(state + 0x70u, value + handle);
    if (actor & 0x400u) {
        if (r_u32(object + 0x284u)) {
            if (actor & 2u) {
                random = moh_draft_missing_call(0x8001552Cu);
                moh_draft_missing_call(0x80041CE8u, 1u, (uint32)((sint32)(random << 1) >> 15) + 7u, object, 0x40000u, 0u, 0x10000u, 0x40ACu, 0u, 0u, 0u, 0u);
            }
            value = r_u32(object + 0x228u); if (value) w_u32(object + 0x228u, (uint32)((sint32)value >> 1));
            value = r_u32(state + 0x78u); if (value) w_u32(state + 0x78u, (uint32)((sint32)value >> 1));
            w_u32(state + 0x70u, 0x4000000u);
            speed = r_u32(object + 0x200u); decrement = (sint32)speed > 0x18FFFF ? 3u : 1u;
            speed -= decrement << 16; w_u32(object + 0x200u, speed);
            if ((sint32)speed >= 0) goto gravity;
            w_u32(state + 0x70u, 0x4000000u);
            w_u32(object + 0x220u, 0u); w_u32(object + 0x218u, 0u);
            w_u32(object + 0x23Cu, 0u); w_u32(object + 0x238u, 0u); w_u32(object + 0x234u, 0u); w_u32(object + 0x200u, 0u);
            value = r_u16(state + 0x38u); actor = r_u32(state + 0x30u);
            w_u32(state + 0x7Cu, 0u); w_u32(state + 0x78u, 0u); w_u32(state + 0x74u, 0u); w_u32(state + 0x68u, 0u);
            w_u16(state + 0x38u, value & 0xFBFFu);
        } else actor = r_u32(state + 0x30u);
        if ((sint32)actor > 0) { moh_draft_missing_call(0x800425C4u, actor); w_u32(state + 0x30u, 0u); }
    }
gravity:
    value = r_u16(state + 0x38u);
    if ((value & 1u) && !(value & 2u)) w_u32(object + 0x21Cu, r_u32(object + 0x21Cu) + r_u32(state + 0x34u));
    if (r_u16(state + 0x38u) & 2u) w_u16(state + 0x38u, r_u16(state + 0x38u) & 0xFFFDu);
    if (r_u16(state + 0x38u) & 0x10u) {
        moh_draft_missing_call(0x8006F7ACu, r_u32(state), r_u32(object + 0x20Cu), r_u32(object + 0x210u), r_u32(object + 0x214u));
        if (!moh_draft_missing_call(0x8006EAC0u, r_u32(state))) moh_draft_missing_call(0x8006B8F0u, object);
    }
    w_u32(state + 0x80u, r_u32(object + 0x200u));
}

void level_sub_80087FA0(uint32 object)
{
    uint32 state, lifetime, random;
    FUNCTION_MARKER(0x80087FA0u, "LEVEL.BIN");
    state = r_u32(object + 0x388u);
    lifetime = r_u32(state) - 1u; w_u32(state, lifetime);
    if (!lifetime) {
        random = moh_draft_missing_call(0x8001552Cu);
        moh_draft_missing_call(0x80041CE8u, 0u, (random & 1u) | 0x10u, object, 0xCCCCu, 0u, r_u32(state + 8u), 0x199Bu, 0u, 0u, 0u, 0u);
        w_u32(0x8009EC60u, r_u32(0x8009EC60u) - 1u);
        moh_draft_missing_call(0x8006B8F0u, object);
    }
}

void level_sub_8004492C(uint32 object, uint32 render)
{
    uint32 state, ordering, packet, index, xy, z, source, flags, entry;
    PsxGteSnapshot snapshot;
    FUNCTION_MARKER(0x8004492Cu, "LEVEL.BIN");
    state = r_u32(object + 0x388u); ordering = r_u32(render + 0x2218u);
    for (index = 0u; index < 8u; ++index) xport_gte_write_control(index, r_u32(render + 0x21E4u + index * 4u));
    xy = r_u16(object + 0x20Eu) | ((uint32)r_u16(object + 0x212u) << 16);
    z = r_u16(object + 0x216u);
    packet = state + r_u32(render + 0x221Cu) * 20u + 4u;
    /* TODO: Original vector padding remains undefined and unused by RTPS */
    xport_gte_write_data(0u, xy); xport_gte_write_data(1u, z); xport_gte_execute(0x180001u);
    psx_gte_snapshot(&snapshot); w_u32(0x1F800190u, (uint32)snapshot.flag);
    if ((sint32)r_u32(0x1F800190u) < 0) return;
    w_u32(packet + 8u, xport_gte_read_data(14u)); w_u32(0x1F800198u, xport_gte_read_data(19u));
    if ((sint32)r_u32(0x1F800198u) < 0 || ((sint32)r_u32(0x1F800198u) >> 4) >= 512) return;
    flags = r_u16(state + 0x38u);
    if (flags & 0x2000u) {
        xy = r_u16(state + 0x86u);
        w_u16(state + 0x38u, flags & 0xDFFFu);
        xy |= (uint32)r_u16(state + 0x8Au) << 16; z = r_u16(state + 0x8Eu);
    } else {
        source = object + 0x25Au;
        xy = r_u16(source) | ((uint32)r_u16(source + 4u) << 16); z = r_u16(source + 8u);
    }
    xport_gte_write_data(0u, xy); xport_gte_write_data(1u, z); xport_gte_execute(0x180001u);
    psx_gte_snapshot(&snapshot); w_u32(0x1F800190u, (uint32)snapshot.flag);
    if ((sint32)r_u32(0x1F800190u) < 0) return;
    w_u32(packet + 16u, xport_gte_read_data(14u));
    entry = ordering + (r_u32(0x1F800198u) >> 4) * 4u;
    w_u32(packet, (r_u32(packet) & 0xFF000000u) | (r_u32(entry) & 0xFFFFFFu));
    entry = ordering + (r_u32(0x1F800198u) >> 4) * 4u;
    w_u32(entry, (r_u32(entry) & 0xFF000000u) | (packet & 0xFFFFFFu));
}

void level_sub_80044B0C(uint32 object, uint32 render)
{
    uint32 state, local;
    FUNCTION_MARKER(0x80044B0Cu, "LEVEL.BIN");
    state = r_u32(object + 0x388u);
    if (r_u32(object + 8u) == 0x13F1u) moh_draft_missing_call(0x80046304u, object, render);
    if (r_u16(state + 0x38u) & 0x80u) {
        moh_draft_missing_call(0x8004492Cu, object, render);
    } else if (r_u32(object + 8u) == 0x13F3u) {
        local = moh_draft_missing_call(0x80016718u, 32u);
        moh_draft_missing_call(0x800154ECu, local, object + 0x340u, 32u);
        moh_draft_missing_call(0x8008D918u, object + 0x20Cu, state + 0x68u, object + 0x270u, object + 0x340u);
        moh_draft_missing_call(0x8006A5B4u, object, render);
        moh_draft_missing_call(0x800154ECu, object + 0x340u, local, 32u);
        moh_draft_missing_call(0x80036E44u, local);
    } else moh_draft_missing_call(0x8006A5B4u, object, render);
}

static void moh_effect_gpf(uint32 vector, sint32 scale, uint32 output)
{
    SVECTOR input;
    VECTOR result;
    uint32 axis;
    input.vx = (sint16)r_u16(vector); input.vy = (sint16)r_u16(vector + 2u);
    input.vz = (sint16)r_u16(vector + 4u); input.pad = 0;
    gte_gpf12(&input, scale, &result);
    for (axis = 0u; axis < 3u; ++axis) w_u16(output + axis * 2u, xport_gte_read_data(9u + axis));
}

uint32 level_sub_8006E058(uint32 position, uint32 scale, uint32 rotation, uint32 color, uint32 texture, uint32 render, uint32 packet, uint32 rate, uint32 depth_bias, uint32 vertical_scale, uint32 centered)
{
    uint32 local, first, second, axis, index, ordering, value, u, screen[3], depth, drawn = 0u, mode, counter;
    sint32 first_y, second_y, x, y, dx1, dx2, dy1, dy2;
    PsxGteSnapshot snapshot;
    FUNCTION_MARKER(0x8006E058u, "LEVEL.BIN");
    if (!packet) return 0u;
    ordering = r_u32(render + 0x2218u);
    local = moh_draft_missing_call(0x80016718u, 64u); first = local + 48u; second = local + 56u;
    value = r_u16(texture + 2u); if (centered) value >>= 1;
    w_u16(second, value); w_u16(first, value);
    w_u16(first + 4u, 0u); w_u16(second + 4u, 0u);
    value = r_u16(texture + 4u); if (centered) value >>= 1;
    w_u16(second + 2u, value); w_u16(first + 2u, centered ? 0u - value : 0u);
    w_u32(local + 32u, scale); w_u32(local + 36u, vertical_scale ? vertical_scale : scale); w_u32(local + 40u, 0u);
    for (index = 0u; index < 9u; ++index) w_u16(local + index * 2u, index % 4u == 0u ? 0x1000u : 0u);
    /* TODO: Original matrix and vector padding remains undefined */
    moh_draft_missing_call(0x8001C768u, rotation, local);
    moh_draft_missing_call(0x800165D8u, local, local + 32u);
    for (index = 0u; index < 5u; ++index) xport_gte_write_control(index, r_u32(local + index * 4u));
    xport_gte_write_data(0u, r_u32(first)); xport_gte_write_data(1u, r_u32(first + 4u)); xport_gte_execute(0x486012u);
    moh_draft_missing_call(0x8001B9F8u, packet);
    for (axis = 0u; axis < 3u; ++axis) w_u16(first + axis * 2u, xport_gte_read_data(9u + axis));
    xport_gte_write_data(0u, r_u32(second)); xport_gte_write_data(1u, r_u32(second + 4u)); xport_gte_execute(0x486012u);
    if (r_u8(texture + 0x14u)) moh_draft_missing_call(0x8001BA38u, packet, 1u);
    for (axis = 0u; axis < 3u; ++axis) w_u16(second + axis * 2u, xport_gte_read_data(9u + axis));
    first_y = (sint16)r_u16(first + 2u) * 1536 / 960;
    second_y = (sint16)r_u16(second + 2u) * 1536 / 960;
    moh_effect_gpf(render + 0x21E4u, (sint16)r_u16(first), 0x1F8001ECu);
    moh_effect_gpf(render + 0x21EAu, first_y, 0x1F8001FCu);
    for (axis = 0u; axis < 3u; ++axis) w_u16(0x1F8001ECu + axis * 2u, r_u16(0x1F8001ECu + axis * 2u) + r_u16(0x1F8001FCu + axis * 2u));
    moh_effect_gpf(render + 0x21E4u, (sint16)r_u16(second), 0x1F8001F4u);
    moh_effect_gpf(render + 0x21EAu, second_y, 0x1F8001FCu);
    for (axis = 0u; axis < 3u; ++axis) w_u16(0x1F8001F4u + axis * 2u, r_u16(0x1F8001F4u + axis * 2u) + r_u16(0x1F8001FCu + axis * 2u));
    for (axis = 0u; axis < 3u; ++axis) w_u16(0x1F8001CCu + axis * 2u, r_u16(position + 2u + axis * 4u));
    for (axis = 0u; axis < 3u; ++axis) {
        w_u16(0x1F8001ECu + axis * 2u, r_u16(0x1F8001ECu + axis * 2u) + (sint16)r_u16(0x1F8001CCu + axis * 2u));
        w_u16(0x1F8001F4u + axis * 2u, r_u16(0x1F8001F4u + axis * 2u) + (sint16)r_u16(0x1F8001CCu + axis * 2u));
    }
    for (index = 0u; index < 8u; ++index) xport_gte_write_control(index, r_u32(render + 0x21E4u + index * 4u));
    xport_gte_write_data(0u, r_u32(0x1F8001CCu)); xport_gte_write_data(1u, r_u32(0x1F8001D0u));
    xport_gte_write_data(2u, r_u32(0x1F8001ECu)); xport_gte_write_data(3u, r_u32(0x1F8001F0u));
    xport_gte_write_data(4u, r_u32(0x1F8001F4u)); xport_gte_write_data(5u, r_u32(0x1F8001F8u)); xport_gte_execute(0x280030u);
    if (r_u8(texture + 0x15u) >= 2u) {
        u = ((r_u16(texture + 6u) + r_u8(texture + 0x16u) * r_u16(texture + 10u)) & 63u) * 4u;
        w_u8(packet + 12u, u); w_u8(packet + 13u, r_u8(texture + 8u));
        w_u8(packet + 20u, r_u8(texture + 16u) + u);
        value = r_u8(texture + 8u); w_u8(packet + 28u, u); w_u8(packet + 21u, value);
        w_u8(packet + 29u, r_u8(texture + 8u) + r_u8(texture + 18u));
        w_u8(packet + 36u, r_u8(texture + 16u) + u);
    } else {
        w_u8(packet + 12u, (r_u8(texture + 6u) & 63u) * 4u); w_u8(packet + 13u, r_u8(texture + 8u));
        w_u8(packet + 20u, r_u8(texture + 16u) + (r_u8(texture + 6u) & 63u) * 4u); w_u8(packet + 21u, r_u8(texture + 8u));
        w_u8(packet + 28u, (r_u8(texture + 6u) & 63u) * 4u); w_u8(packet + 29u, r_u8(texture + 8u) + r_u8(texture + 18u));
        w_u8(packet + 36u, r_u8(texture + 16u) + (r_u8(texture + 6u) & 63u) * 4u);
    }
    w_u8(packet + 37u, r_u8(texture + 8u) + r_u8(texture + 18u));
    value = moh_draft_missing_call(0x8001BA18u, r_u16(texture + 12u), r_u16(texture + 14u)); w_u16(packet + 14u, value);
    psx_gte_snapshot(&snapshot);
    if ((sint32)snapshot.flag >= 0) {
        depth = xport_gte_read_data(19u) - (depth_bias + 128u); w_u32(0x1F800198u, depth);
        for (index = 0u; index < 3u; ++index) { screen[index] = xport_gte_read_data(12u + index); w_u32(0x1F8001BCu + index * 4u, screen[index]); }
        if ((sint32)depth >= 0) {
            if ((sint32)depth >> 4 >= 512) { depth = 0x1FD0u; w_u32(0x1F800198u, depth); }
            mode = r_u8(texture + 20u); mode = mode ? mode - 1u : 1u;
            w_u16(packet + 22u, ((mode & 3u) << 5) | ((r_u16(texture + 8u) & 0x100u) >> 4) | ((r_u16(texture + 6u) & 0x3C0u) >> 6) | ((r_u16(texture + 8u) & 0x200u) << 2));
            x = (sint16)screen[0]; y = (sint16)(screen[0] >> 16);
            dx1 = (sint16)screen[1] - x; dx2 = (sint16)screen[2] - x;
            dy1 = (sint16)(screen[1] >> 16) - y; dy2 = (sint16)(screen[2] >> 16) - y;
            w_u8(packet + 4u, r_u8(color)); w_u8(packet + 5u, r_u8(color + 1u)); w_u8(packet + 6u, r_u8(color + 2u));
            if (centered) {
                w_u16(packet + 8u, x + dx1); w_u16(packet + 10u, y + dy1);
                w_u16(packet + 16u, x + dx2); w_u16(packet + 18u, y + dy2);
                w_u16(packet + 24u, x - dx2); w_u16(packet + 26u, y - dy2);
                w_u16(packet + 32u, x - dx1); w_u16(packet + 34u, y - dy1);
            } else {
                w_u16(packet + 8u, x); w_u16(packet + 10u, y); w_u16(packet + 16u, x + dx1); w_u16(packet + 18u, y);
                w_u16(packet + 24u, x); w_u16(packet + 26u, y + dy2); w_u16(packet + 32u, x + dx2); w_u16(packet + 34u, y + dy2);
            }
            value = ordering + (r_u32(0x1F800198u) >> 4) * 4u;
            w_u32(packet, (r_u32(packet) & 0xFF000000u) | (r_u32(value) & 0xFFFFFFu));
            value = ordering + (r_u32(0x1F800198u) >> 4) * 4u;
            w_u32(value, (r_u32(value) & 0xFF000000u) | (packet & 0xFFFFFFu)); drawn = 1u;
        }
    }
    if (r_u8(texture + 21u) >= 2u && !r_u32(0x8009A5E0u)) {
        counter = (r_u8(texture + 23u) + 1u) & 255u; w_u8(texture + 23u, counter);
        if (counter == (rate & 255u)) {
            value = r_u8(texture + 22u); mode = r_u8(texture + 21u); w_u8(texture + 23u, 0u);
            value = (value + 1u) & 255u; w_u8(texture + 22u, value); if (value == mode) w_u8(texture + 22u, 0u);
        }
    }
    moh_draft_missing_call(0x80036E44u, local);
    return drawn;
}

uint32 level_sub_8004CD90(uint32 object, uint32 other, uint32 direction, uint32 distance_out)
{
    FUNCTION_MARKER(0x8004CD90u, "LEVEL.BIN");
    return level_sub_8004CBC8(object + 0x258u, other, direction, distance_out);
}

uint32 level_sub_80088040(uint32 owner, uint32 position, uint32 kind)
{
    uint32 object, state, random, count;
    FUNCTION_MARKER(0x80088040u, "LEVEL.BIN");
    if (r_u32(0x8009EC60u) >= 5u) return 0u;
    object = moh_draft_missing_call(0x8006B5B8u, 1u, 12u, 0xFFFFFFFFu, 0u, 0u, 0u, 0u, 0u);
    if (!object) return 0u;
    w_u32(object + 8u, 0x66u); w_u32(object + 12u, 0x7020000u); w_u32(object + 16u, 0x213u);
    w_u32(object + 0x36Cu, 0x80087FA0u);
    w_u32(object + 0x374u, 0u); w_u32(object + 0x370u, 0u);
    w_u32(object + 0x378u, 0u); w_u32(object + 0x37Cu, 0u);
    w_u32(object + 0x384u, 0x8006A318u); w_u32(object + 20u, 0u);
    w_u32(object + 0x20Cu, r_u32(position));
    w_u32(object + 0x210u, r_u32(position + 4u));
    state = r_u32(object + 0x388u);
    w_u32(object + 0x214u, r_u32(position + 8u));
    random = moh_draft_missing_call(0x8001552Cu);
    w_u32(state, (random & 15u) + 8u);
    moh_draft_missing_call(0x8006D84Cu, state + 4u, owner);
    count = r_u32(0x8009EC60u);
    w_u32(state + 8u, kind); w_u32(0x8009EC60u, count + 1u);
    moh_draft_missing_call(0x8006B834u, object);
    return object;
}

void level_sub_8006DCA0(uint32 effect)
{
    FUNCTION_MARKER(0x8006DCA0u, "LEVEL.BIN");
    w_u16(effect + 2u, 1u);
}

void level_sub_8004D8DC(uint32 controller, uint32 small_motor, uint32 strength, uint32 duration, uint32 shift)
{
    uint32 mode;
    FUNCTION_MARKER(0x8004D8DCu, "LEVEL.BIN");
    mode = r_u8(0x80093A04u);
    if (mode == 1u) strength = (uint32)((sint32)strength >> 1);
    else if (mode == 2u) strength -= (uint32)((sint32)strength >> 2);
    else if (mode != 3u) return;
    w_u8(0x80093BE8u + controller * 2u, small_motor);
    w_u8(0x80093BE9u + controller * 2u, strength);
    w_u32(0x80093BFCu + controller * 4u, duration);
    w_u32(0x80093BF4u + controller * 4u, (1u << (shift & 31u)) - 1u);
}

void level_sub_8004572C(uint32 object, uint32 red, uint32 green, uint32 blue, uint32 end_red, uint32 end_green, uint32 end_blue)
{
    uint32 state, position[3], axis;
    FUNCTION_MARKER(0x8004572Cu, "LEVEL.BIN");
    state = r_u32(object + 0x388u);
    w_u16(state + 0x38u, r_u16(state + 0x38u) | 0xA0u);
    moh_draft_missing_call(0x80016168u, state + 4u);
    moh_draft_missing_call(0x80016168u, state + 0x18u);
    w_u8(state + 8u, red); w_u8(state + 9u, green); w_u8(state + 10u, blue);
    w_u8(state + 0x1Cu, red); w_u8(state + 0x1Du, green); w_u8(state + 0x1Eu, blue);
    w_u8(state + 0x10u, end_red); w_u8(state + 0x11u, end_green); w_u8(state + 0x12u, end_blue);
    w_u8(state + 0x24u, end_red); w_u8(state + 0x25u, end_green); w_u8(state + 0x26u, end_blue);
    for (axis = 0u; axis < 3u; ++axis) position[axis] = r_u32(object + 0x20Cu + axis * 4u);
    for (axis = 0u; axis < 3u; ++axis) w_u32(object + 0x258u + axis * 4u, position[axis]);
}

void level_sub_8006B854(uint32 object)
{
    uint32 cursor;
    FUNCTION_MARKER(0x8006B854u, "LEVEL.BIN");
    cursor = r_u32(0x8009D64Cu);
    while (r_u32(cursor + 4u)) cursor = r_u32(cursor + 4u);
    w_u32(object + 4u, 0u);
    w_u32(cursor + 4u, object);
}

void level_sub_8004CDB0(uint32 object)
{
    uint32 index, record, axis, radius, value, bound;
    FUNCTION_MARKER(0x8004CDB0u, "LEVEL.BIN");
    w_u16(object + 0x308u, 0x7FFFu); w_u16(object + 0x30Au, 0x7FFFu); w_u16(object + 0x30Cu, 0x7FFFu);
    w_u16(object + 0x30Eu, 0x8001u); w_u16(object + 0x310u, 0x8001u); w_u16(object + 0x312u, 0x8001u);
    for (index = 0u; index < r_u16(object + 0x36u); ++index) {
        record = r_u32(object + 0x304u) + index * 0x48u;
        for (axis = 0u; axis < 3u; ++axis) {
            radius = (uint32)(sint32)(sint16)r_u16(record + 0x34u);
            value = r_u32(record + 0x20u + axis * 4u);
            bound = (uint32)(sint32)(sint16)r_u16(object + 0x308u + axis * 2u);
            value -= radius;
            if ((sint32)bound < (sint32)value) value = bound;
            w_u16(object + 0x308u + axis * 2u, value);
        }
        for (axis = 0u; axis < 3u; ++axis) {
            radius = (uint32)(sint32)(sint16)r_u16(record + 0x34u);
            value = r_u32(record + 0x20u + axis * 4u);
            bound = (uint32)(sint32)(sint16)r_u16(object + 0x30Eu + axis * 2u);
            value += radius;
            if ((sint32)bound >= (sint32)value) value = bound;
            w_u16(object + 0x30Eu + axis * 2u, value);
        }
    }
}

uint32 level_sub_8006D6A0(uint32 origin, uint32 target)
{
    uint32 x, z, minimum;
    FUNCTION_MARKER(0x8006D6A0u, "LEVEL.BIN");
    x = r_u32(target) - r_u32(origin);
    z = r_u32(target + 8u) - r_u32(origin + 8u);
    if ((sint32)x < 0) x = 0u - x;
    if ((sint32)z < 0) z = 0u - z;
    minimum = (sint32)z < (sint32)x ? z : x;
    return x + z - (uint32)((sint32)minimum >> 1);
}

void level_sub_80044D18(uint32 origin, uint32 target, uint32 angles, uint32 height)
{
    uint32 x, z, angle, y, distance;
    VECTOR input, output;
    FUNCTION_MARKER(0x80044D18u, "LEVEL.BIN");
    z = (uint32)((sint32)(r_u32(target + 8u) - r_u32(origin + 8u)) >> 16);
    x = (uint32)((sint32)(r_u32(target) - r_u32(origin)) >> 16);
    angle = moh_draft_missing_call(0x8001536Cu, x, z);
    w_u32(angles + 4u, angle << 16);
    x = (uint32)((sint32)(r_u32(target) - r_u32(origin)) >> 16);
    xport_gte_write_data(9u, x); xport_gte_write_data(10u, x); xport_gte_write_data(11u, z);
    input.vx = (sint16)x; input.vy = (sint16)x; input.vz = (sint16)z; input.pad = 0;
    Square0(&input, &output);
    y = r_u32(target + 4u) - r_u32(origin + 4u) - (uint32)((sint32)height >> 1);
    y = (uint32)((sint32)y >> 16);
    distance = moh_draft_missing_call(0x80016188u, (uint32)output.vx + (uint32)output.vz);
    angle = moh_draft_missing_call(0x8001536Cu, y, distance);
    w_u32(angles, 0u - (angle << 16));
}

void level_sub_80045084(uint32 owner, uint32 object)
{
    uint32 source, state, position[3], axis, value;
    sint32 distance, speed;
    FUNCTION_MARKER(0x80045084u, "LEVEL.BIN");
    source = r_u32(owner + 0x388u);
    state = r_u32(object + 0x388u);
    for (axis = 0u; axis < 3u; ++axis) position[axis] = r_u32(source + 0x198u + axis * 4u);
    for (axis = 0u; axis < 3u; ++axis) w_u32(object + 0x20Cu + axis * 4u, position[axis]);
    moh_draft_missing_call(0x80044D18u, object + 0x20Cu, source + 0x1A4u, object + 0x228u, 0u);
    distance = (sint32)moh_draft_missing_call(0x8006D6A0u, object + 0x20Cu, source + 0x1A4u);
    speed = (sint32)r_u32(object + 0x200u);
    /* TODO: Native failure represents original division BREAK */
    if (!speed || (speed == -1 && distance == (sint32)0x80000000u))
        moh_draft_fail_fast("Projectile flight division BREAK", 0x80045084u);
    w_u8(state + 0x40u, (uint32)(distance / speed) + 1u);
    value = r_u32(object + 0x14u);
    w_u32(state + 0x3Cu, value);
    w_u32(object + 0x14u, r_u32(object + 0x14u) & 0xF7FFFDFFu);
}

void level_sub_800451B0(uint32 owner, uint32 object, uint32 yaw, uint32 pitch, uint32 roll, uint32 offset, uint32 first, uint32 second, uint32 damage)
{
    uint32 anchor, resource, local, special;
    FUNCTION_MARKER(0x800451B0u, "LEVEL.BIN");
    if (damage) {
        moh_draft_missing_call(0x80045084u, owner, object);
        return;
    }
    if ((r_u32(owner + 0xCu) & 1u) && (r_u32(r_u32(owner + 0x388u)) & 0x100u)) {
        anchor = r_u32(0x8009D638u);
        moh_draft_missing_call(0x80045160u, anchor, object + 0x20Cu);
        moh_draft_missing_call(0x80045184u, anchor, object + 0x20Cu, object + 0x228u);
        return;
    }
    resource = r_u32(object + 8u);
    special = resource == 0x13F3u || (resource == 0x13F1u && (r_u32(owner + 0xCu) & 1u));
    local = moh_draft_missing_call(0x80016718u, 16u);
    moh_draft_missing_call(0x80044E0Cu, owner, object + 0x20Cu, yaw, pitch, offset, first, second, local, local + 8u);
    if (special)
        moh_draft_missing_call(0x80044EF0u, owner, object + 0x228u);
    else
        moh_draft_missing_call(0x80044F10u, owner, object + 0x20Cu, object + 0x228u, roll, offset, local, local + 8u);
    moh_draft_missing_call(0x80036E44u, local);
}

uint32 level_sub_8004532C(uint32 owner, uint32 resource, uint32 model, uint32 speed, uint32 yaw, uint32 pitch, uint32 roll, uint32 unused, uint32 flags, uint32 effect, uint32 state_size, uint32 offset, uint32 first, uint32 second, uint32 damage, uint32 object_flags, uint32 extra_flags, uint32 final)
{
    uint32 object, state, value, index, distance, axis, position[3], velocity[3];
    sint32 dx, dz, product;
    FUNCTION_MARKER(0x8004532Cu, "LEVEL.BIN");
    (void)unused;
    object = moh_draft_missing_call(0x8006B5B8u, model != 0xFFFFFFFFu, state_size + 0x90u, model, model != 0xFFFFFFFFu ? 0x147u : 0u, 2u, 0u, 0u, 0u);
    if (!object) return 0u;
    w_u32(object + 0x36Cu, 0x80044470u); w_u32(object + 0x370u, 0x80044B0Cu);
    w_u32(object + 0x374u, 0x80043E38u); w_u32(object + 0x378u, 0x80043284u);
    w_u32(object + 0x384u, 0x80044BE4u); w_u32(object + 8u, resource);
    value = r_u32(object + 0x10u);
    w_u32(object + 0xCu, object_flags | 0x108u); w_u32(object + 0x14u, final);
    w_u32(object + 0x10u, value | 0x200u | extra_flags);
    if ((r_u32(owner + 0xCu) & 1u) && (r_u32(r_u32(owner + 0x388u)) & 0x20000000u)) flags <<= 2;
    value = r_u32(owner + 0xCu);
    w_u32(object + 0xF8u, flags); w_u32(object + 0xCu, r_u32(object + 0xCu) | (value & 4u));
    moh_draft_missing_call(0x8006D84Cu, object + 0x30u, 0u);
    state = r_u32(object + 0x388u);
    w_u8(object + 0x34u, 0xFFu); w_u16(state + 0x38u, 0u);
    moh_draft_missing_call(0x8006D84Cu, state + 0x54u, owner);
    w_u16(state + 0x3Au, effect); w_u32(state + 0x2Cu, 0u); w_u32(state + 0x30u, 0u);
    w_u8(state + 0x42u, 0u); w_u32(state + 0x34u, 0x10000u);
    w_u32(object + 0x218u, 0u); w_u32(object + 0x21Cu, 0u); w_u32(object + 0x220u, 0u);
    w_u32(object + 0x200u, speed);
    moh_draft_missing_call(0x800451B0u, owner, object, yaw, pitch, roll, offset, first, second, damage);
    w_u32(object + 0x284u, 0u); moh_draft_missing_call(0x8006D84Cu, state + 0x58u, 0u);
    w_u32(state + 0x44u, 0u); moh_draft_missing_call(0x8006A76Cu, object);
    for (axis = 0u; axis < 3u; ++axis) position[axis] = r_u32(object + 0x20Cu + axis * 4u);
    for (axis = 0u; axis < 3u; ++axis) w_u32(object + 0x258u + axis * 4u, position[axis]);
    if (r_u32(owner + 0xCu) & 1u) {
        index = r_u8(0x8009E878u + (uint32)(sint32)(sint8)r_u8(r_u32(owner + 0x388u) + 0x54u));
        if (r_u16(0x8009366Au + index * 0x44u) & 2u) {
            velocity[0] = (uint32)(sint32)(sint16)r_u16(object + 0x344u);
            velocity[1] = (uint32)(sint32)(sint16)r_u16(object + 0x34Au);
            velocity[2] = (uint32)(sint32)(sint16)r_u16(object + 0x350u);
            dx = (sint16)((sint16)r_u16(object + 0x20Eu) - (sint16)r_u16(owner + 0x20Eu));
            dz = (sint16)((sint16)r_u16(object + 0x216u) - (sint16)r_u16(owner + 0x216u));
            if (dx < 0) dx = -dx;
            if (dz < 0) dz = -dz;
            distance = dx >= dz ? (uint32)(dx + (dz >> 2)) : (uint32)(dz + (dx >> 2));
            distance -= distance >> 2;
            for (axis = 0u; axis < 3u; ++axis) {
                product = (sint32)((0u - velocity[axis]) * distance);
                value = r_u32(object + 0x20Cu + axis * 4u);
                w_u32(object + 0x20Cu + axis * 4u, value + ((uint32)(product >> 12) << 16));
            }
        }
    }
    if (model == 0u || model == 0xFFFFFFFFu) {
        w_u32(r_u32(object + 0x304u) + 0x3Cu, 0u);
        w_u32(r_u32(object + 0x304u) + 0x84u, 0u);
    }
    moh_draft_missing_call(0x8004CDB0u, object); moh_draft_missing_call(0x8006B854u, object);
    for (axis = 0u; axis < 3u; ++axis) position[axis] = r_u32(object + 0x20Cu + axis * 4u);
    for (axis = 0u; axis < 3u; ++axis) w_u32(state + 0x84u + axis * 4u, position[axis]);
    return object;
}

uint32 level_sub_80058420(uint32 object, uint32 data, uint32 frame)
{
    uint32 sound, first, second, volume, parameter, kind, flags, special, voice, model, base, left, right, mode;
    sint32 left_signed, right_signed;
    FUNCTION_MARKER(0x80058420u, "LEVEL.BIN");
    sound = moh_draft_missing_call(0x8005D148u, 1u, frame, r_u8(data), object);
    first = moh_draft_missing_call(0x8005D148u, 2u, frame, r_u32(data + 4u), object);
    second = moh_draft_missing_call(0x8005D148u, 3u, frame, r_u32(data + 8u), object);
    volume = moh_draft_missing_call(0x8005D148u, 4u, frame, r_u32(data + 12u), object);
    parameter = moh_draft_missing_call(0x8005D148u, 5u, frame, r_u32(data + 16u), object);
    kind = moh_draft_missing_call(0x8005D148u, 6u, frame, r_u32(data + 20u), object);
    if (r_u32(0x80099EB0u) || (sint32)sound < 0) return data + 24u;
    flags = r_u8(data + 1u);
    special = (flags & 0x20u) != 0u;
    if (!(flags & 3u)) {
        if (kind == 0xFFFFFFFFu) {
            model = r_u32(r_u32(object + 0x100u) + 8u);
            kind = model ? r_u16(model + 0xC8u) : 0u;
        }
        voice = moh_draft_missing_call(0x80041CE8u, (uint32)(sint32)(sint16)kind, sound, object, volume, parameter, first, second, 0u, special, 0u, 0u);
        w_u32(object + 0xFCu, voice);
    } else {
        voice = moh_draft_missing_call(0x80041CE8u, 0u, sound, object, volume, parameter, first, second, 0u, special, 0u, 0u);
        if ((sint32)voice < 0) return data + 24u;
        base = (uint32)((sint32)volume >> 2);
        left = base; right = base;
        if (r_u8(data + 1u) & 2u) {
            left = base ? (moh_draft_missing_call(0x8001552Cu) * base) >> 15 : 0u;
            right = base ? (moh_draft_missing_call(0x8001552Cu) * base) >> 15 : 0u;
        }
        mode = r_u8(0x800939FEu);
        if (mode == 0u) left = right = 0u;
        else if (mode == 1u) {
            left = (uint32)((sint16)left >> 1);
            right = (uint32)((sint16)right >> 1);
        } else if (mode == 3u) { left <<= 1; right <<= 1; }
        left_signed = (sint16)left; right_signed = (sint16)right;
        if (left_signed > 0x3FFF) left_signed = 0x3FFF;
        if (right_signed > 0x3FFF) right_signed = 0x3FFF;
        moh_draft_missing_call(0x80015C08u, voice, (uint32)left_signed, (uint32)right_signed);
    }
    return data + 24u;
}

uint32 level_sub_8003FFA4(uint32 object)
{
    uint32 extra, flags, vector, angle, clear;
    FUNCTION_MARKER(0x8003FFA4u, "LEVEL.BIN");
    extra = r_u32(object + 0x38Cu);
    flags = r_u32(extra + 8u);
    if ((flags & 0x18u) == 0x18u)
        flags &= (sint32)r_u32(extra + 0x10Cu) > (sint32)r_u32(extra + 0x11Cu) ? ~8u : ~16u;
    if (flags & 4u) {
        w_u32(object + 0x238u, 0u);
        vector = extra + 0xF4u;
        if (flags & 16u) {
            clear = 1u;
            if (!(flags & 2u)) {
                angle = moh_draft_missing_call(0x8001536Cu, (uint32)(sint32)(sint16)r_u16(vector), (uint32)(sint32)(sint16)r_u16(vector + 4u));
                w_u32(object + 0x22Cu, (angle + 0x400u) << 16);
            }
        } else if (flags & 8u) {
            clear = 2u;
            if (!(flags & 1u)) {
                angle = moh_draft_missing_call(0x8001536Cu, (uint32)(sint32)(sint16)r_u16(vector), (uint32)(sint32)(sint16)r_u16(vector + 4u));
                w_u32(object + 0x22Cu, (angle - 0x400u) << 16);
            }
        } else {
            angle = moh_draft_missing_call(0x8001536Cu, (uint32)(sint32)(sint16)r_u16(vector), (uint32)(sint32)(sint16)r_u16(vector + 4u));
            clear = (sint32)r_u32(extra + 0x100u) <= 0 ? 1u : 2u;
            angle += clear == 1u ? 0x400u : 0xFFFFFC00u;
            w_u32(extra + 8u, r_u32(extra + 8u) & ~clear);
            w_u32(object + 0x22Cu, angle << 16);
            return 0u;
        }
    } else if (flags & 8u) {
        clear = 2u;
        if (!(flags & 1u)) {
            angle = moh_draft_missing_call(0x8001536Cu, (uint32)(sint32)(sint16)r_u16(extra + 0x104u), (uint32)(sint32)(sint16)r_u16(extra + 0x108u));
            w_u32(object + 0x22Cu, (angle - 0x400u) << 16);
        }
    } else if (flags & 16u) {
        clear = 1u;
        if (!(flags & 2u)) {
            angle = moh_draft_missing_call(0x8001536Cu, (uint32)(sint32)(sint16)r_u16(extra + 0x114u), (uint32)(sint32)(sint16)r_u16(extra + 0x118u));
            w_u32(object + 0x22Cu, (angle + 0x400u) << 16);
        }
    } else return 1u;
    w_u32(extra + 8u, r_u32(extra + 8u) & ~clear);
    return 0u;
}

uint32 level_sub_80056250(uint32 object, uint32 flags)
{
    uint32 value;
    FUNCTION_MARKER(0x80056250u, "LEVEL.BIN");
    if (flags & 1u) {
        value = r_u32(object + 0x38u);
        w_u32(object + 0x200u, 0u);
        w_u32(object + 0x38u, value | 2u);
    } else {
        w_u32(object + 0x3Cu, r_u32(object + 0x3Cu) | 2u);
    }
    return 0u;
}

static void moh_attached_vertices(uint32 vertices, uint32 packed)
{
    uint32 first = vertices + (packed & 0x3FFu) * 8u;
    uint32 second = vertices + ((packed >> 7) & 0x1FF8u);
    uint32 third = vertices + ((packed >> 17) & 0x1FF8u);
    xport_gte_write_data(0u, r_u32(first)); xport_gte_write_data(1u, r_u32(first + 4u));
    xport_gte_write_data(2u, r_u32(second)); xport_gte_write_data(3u, r_u32(second + 4u));
    xport_gte_write_data(4u, r_u32(third)); xport_gte_write_data(5u, r_u32(third + 4u));
}

static void moh_attached_scale_color(uint32 packet, uint32 scale, uint32 render)
{
    uint32 product[9], component, value;
    if (scale == 128u) return;
    product[0] = r_u8(packet + 4u) * scale;
    product[1] = r_u8(packet + 5u) * scale;
    product[2] = r_u8(packet + 6u) * scale;
    product[3] = r_u8(packet + 16u) * scale;
    product[4] = r_u8(packet + 17u) * scale;
    product[5] = r_u8(packet + 18u) * scale;
    product[6] = r_u8(packet + 28u) * scale;
    product[7] = r_u8(packet + 29u) * scale;
    w_u8(packet + 4u, product[0] >> 7); w_u8(packet + 5u, product[1] >> 7);
    product[8] = r_u8(packet + 30u) * scale;
    for (component = 3u; component < 7u; ++component)
        w_u8(packet + 16u + (component - 3u) / 3u * 12u + (component - 3u) % 3u, product[component] >> 7);
    w_u8(packet + 6u, product[2] >> 7);
    w_u8(packet + 29u, product[7] >> 7); w_u8(packet + 30u, product[8] >> 7);
    value = r_u16(packet + 26u);
    w_u16(packet + 26u, (value & 0xFF9Fu) | ((r_u16(render + 0x68u) & 3u) << 5));
}

static void moh_attached_link(uint32 packet)
{
    uint32 ordering = r_u32(0x1F8001D0u) + r_u32(0x1F800198u) * 4u;
    w_u32(packet, (r_u32(packet) & 0xFF000000u) | (r_u32(ordering) & 0xFFFFFFu));
    ordering = r_u32(0x1F8001D0u) + r_u32(0x1F800198u) * 4u;
    w_u32(ordering, (r_u32(ordering) & 0xFF000000u) | (packet & 0xFFFFFFu));
}

uint32 level_sub_80081CDC(uint32 render, uint32 ordering_table, uint32 buffer_index, uint32 alternate, uint32 depth_out)
{
    uint32 vertices, colors, model, minimum, scale, code, local, packet, limit, level, table, count;
    uint32 triangle, current, packed, uv[3], screen[3], maximum, depth, value, shift, color, mask, index, drawn = 0u;
    sint32 fog, bucket;
    PsxGteSnapshot snapshot;
    FUNCTION_MARKER(0x80081CDCu, "LEVEL.BIN");
    vertices = r_u32(render + 0x2Cu);
    fog = (sint16)r_u16(r_u32(0x8009CC64u) + 0xCCu);
    model = r_u32(render + 0x58u);
    colors = r_u32(render + 0x48u);
    if (!model) {
        value = r_u32(render + 0x54u);
        model = value ? r_u32(0x8009D678u + (value & 0xFFFFu) * 4u) : 0u;
        model = r_u32(model + 0x100u);
    }
    minimum = (uint32)(sint32)(sint16)r_u16(model + 0xA8u);
    scale = r_u32(render + 0x6Cu);
    code = r_u32(render + 0x68u) ? 0x36u : 0x34u;
    local = moh_draft_missing_call(0x80016718u, 4u);
    packet = moh_draft_missing_call(0x80053BD4u, buffer_index, local);
    limit = r_u32(local) - 40u;
    level = r_u32(0x8009CC64u);
    w_u32(0x1F8001B0u, level + 0xDCu);
    w_u32(0x1F8001C0u, 0u); w_u32(0x1F8001C8u, 0xFFFFFFu);
    w_u32(0x1F8001D0u, ordering_table);
    w_u32(0x1F8001ACu, r_u32(level + 0xD8u) != 0u);
    table = r_u32(render + (alternate ? 0x28u : 0x20u));
    count = r_u32(table); triangle = table + 4u;
    moh_attached_vertices(vertices, r_u32(triangle + 12u));
    while ((sint32)count > 0 && packet < limit) {
        xport_gte_execute(0x280030u);
        w_u8(packet + 3u, 9u);
        current = triangle;
        uv[0] = r_u32(current); uv[1] = r_u32(current + 4u); uv[2] = r_u32(current + 8u);
        triangle += 16u; --count;
        w_u32(packet + 12u, uv[0]); w_u32(packet + 24u, uv[1]); w_u32(packet + 36u, uv[2]);
        psx_gte_snapshot(&snapshot); w_u32(0x1F800190u, (uint32)snapshot.flag);
        xport_gte_execute(0x1400006u);
        if ((sint32)count > 0) moh_attached_vertices(vertices, r_u32(triangle + 12u));
        if ((r_u32(0x1F800190u) & 0x80020000u) == 0x80000000u || (sint32)xport_gte_read_data(24u) <= 0) continue;
        maximum = r_u16(model + 0xAAu) & 0x7FFFu;
        screen[0] = xport_gte_read_data(12u); screen[1] = xport_gte_read_data(13u); screen[2] = xport_gte_read_data(14u);
        xport_gte_execute(0x158002Du);
        depth = xport_gte_read_data(7u) << 2;
        if ((sint32)depth < (sint32)minimum) depth = minimum;
        shift = (uint32)((sint32)(maximum - minimum) >> 9) & 31u;
        bucket = (sint32)(depth - minimum) >> shift;
        w_u32(0x1F800198u, (uint32)bucket);
        if (!r_u32(0x1F8001C0u)) {
            packed = r_u32(current + 12u);
            w_u32(0x1F8001B4u, r_u32(colors + (packed & 0x3FFu) * 4u));
            w_u32(0x1F8001B8u, r_u32(colors + ((packed >> 8) & 0xFFCu)));
            w_u32(0x1F8001BCu, r_u32(colors + ((packed >> 18) & 0xFFCu)));
        } else {
            value = r_u32(0x1F8001C8u);
            w_u32(0x1F8001B4u, value); w_u32(0x1F8001B8u, value); w_u32(0x1F8001BCu, value);
        }
        bucket = (sint32)((uint32)bucket - 15u);
        w_u32(0x1F800198u, (uint32)bucket);
        if (bucket <= 0) w_u32(0x1F800198u, 1u);
        if ((sint32)r_u32(0x1F800198u) >= 512) w_u32(0x1F800198u, 511u);
        if (!alternate && r_u32(0x1F8001ACu)) {
            mask = r_u32(0x1F8001C8u);
            for (index = 0u; index < 3u; ++index) {
                color = r_u32(0x1F8001B4u + index * 4u);
                if ((color & mask) < 40u) {
                    value = r_u32(r_u32(0x1F8001B0u) + (color & mask) * 20u + 12u);
                    w_u32(0x1F8001B4u + index * 4u, (color & 0xFF000000u) | (value & mask));
                }
            }
            if (!fog) {
                w_u32(packet + 4u, r_u32(0x1F8001B4u));
                w_u32(packet + 16u, r_u32(0x1F8001B8u));
                w_u8(packet + 7u, code);
                w_u32(packet + 28u, r_u32(0x1F8001BCu));
            }
        }
        /* TODO: Original no-fog/no-palette path retains existing packet color bytes */
        if (fog) {
            xport_gte_write_data(20u, r_u32(0x1F8001B4u));
            xport_gte_write_data(21u, r_u32(0x1F8001B8u));
            color = r_u32(0x1F8001BCu);
            xport_gte_write_data(22u, color); xport_gte_write_data(6u, color);
            xport_gte_execute(0xF8002Au);
            w_u32(packet + 4u, xport_gte_read_data(20u));
            w_u32(packet + 16u, xport_gte_read_data(21u)); w_u32(packet + 28u, xport_gte_read_data(22u));
            w_u8(packet + 7u, code);
            moh_attached_scale_color(packet, scale, render);
            ++drawn; moh_attached_link(packet);
        } else {
            moh_attached_link(packet); moh_attached_scale_color(packet, scale, render); ++drawn;
        }
        packet += 40u;
        w_u32(packet - 32u, screen[0]); w_u32(packet - 20u, screen[1]); w_u32(packet - 8u, screen[2]);
    }
    if (depth_out) w_u32(depth_out, minimum);
    moh_draft_missing_call(0x80053BF4u, packet);
    moh_draft_missing_call(0x80036E44u, local);
    return (sint32)drawn > 0;
}

void level_sub_8006D398(uint32 part)
{
    uint32 local, value[3], component;
    FUNCTION_MARKER(0x8006D398u, "LEVEL.BIN");
    local = moh_draft_missing_call(0x80016718u, 48u);
    moh_draft_missing_call(0x800154ECu, local, part + 0x74u, 32u);
    moh_draft_missing_call(0x800163E8u, 0x8009C804u, local + 20u, local + 32u);
    moh_draft_missing_call(0x8001BD08u, 0x8009C804u, local);
    value[0] = r_u32(local + 32u) + r_u32(0x8009C818u);
    value[1] = r_u32(local + 36u) + r_u32(0x8009C81Cu);
    value[2] = r_u32(local + 40u) + r_u32(0x8009C820u);
    for (component = 0u; component < 3u; ++component)
        w_u32(local + 20u + component * 4u, value[component]);
    for (component = 0u; component < 5u; ++component)
        xport_gte_write_control(component, r_u32(local + component * 4u));
    for (component = 0u; component < 3u; ++component)
        xport_gte_write_control(5u + component, r_u32(local + 20u + component * 4u));
    moh_draft_missing_call(0x80036E44u, local);
}

void level_sub_8006D460(uint32 part, uint32 render_context)
{
    FUNCTION_MARKER(0x8006D460u, "LEVEL.BIN");
    if (!(r_u32(part) & 1u)) return;
    if (r_u32(part + 8u)) moh_draft_missing_call(0x8006D398u, part);
    else moh_draft_missing_call(0x8008D9D8u, part + 0x3Cu, part + 0x60u, part + 0x24u);
    moh_draft_missing_call(0x800824D0u, part + 0x94u, r_u32(render_context + 0x2218u),
        r_u32(render_context + 0x221Cu), 0x80u);
}

void level_sub_8006D4DC(uint32 parts, uint32 render_context)
{
    uint32 count, index, part;
    FUNCTION_MARKER(0x8006D4DCu, "LEVEL.BIN");
    part = parts + 4u;
    count = r_u32(parts);
    for (index = 0u; (sint32)index < (sint32)count; ++index, part += 0x144u)
        moh_draft_missing_call(0x8006D460u, part, render_context);
}

void level_sub_8005081C(uint32 object, uint32 model, uint32 part, uint32 bone)
{
    uint32 local, matrix, vector, transformed, component, offset, quaternion, x, y, z, w;
    uint32 first[3], second[3], value, axis_a, axis_b, cross, bone_a, bone_b;
    FUNCTION_MARKER(0x8005081Cu, "LEVEL.BIN");
    if (!part) return;
    first[0] = r_u32(part + 0x3Cu); first[1] = r_u32(part + 0x40u); first[2] = r_u32(part + 0x44u);
    w_u32(part + 0x48u, first[0]); w_u32(part + 0x4Cu, first[1]); w_u32(part + 0x50u, first[2]);
    local = moh_draft_missing_call(0x80016718u, 112u);
    matrix = local + 8u; vector = local + 40u; transformed = local + 56u;
    axis_a = local + 72u; axis_b = local + 80u; cross = local + 88u;
    bone_a = local + 96u; bone_b = local + 104u;
    /* TODO: Original unused vector and matrix padding has no defined value */
    if (r_u32(model + 0x84u)) {
        for (component = 0u; component < 3u; ++component)
            w_u32(part + 0x60u + component * 4u, r_u32(object + 0x228u + component * 4u));
        offset = bone * 8u;
        quaternion = r_u32(model + 0x88u);
        if (!quaternion) { moh_draft_missing_call(0x80036E44u, local); return; }
        value = r_u32(quaternion + offset);
        x = (uint32)(sint32)(sint16)value;
        y = (uint32)((sint32)value >> 16);
        value = r_u32(quaternion + offset + 4u);
        z = (uint32)(sint32)(sint16)value;
        w = (uint32)((sint32)value >> 16);
        xport_gte_write_control(0u, (w << 16) | (x & 0xFFFFu));
        xport_gte_write_control(1u, (y << 16) | (z & 0xFFFFu));
        xport_gte_write_control(3u, (y << 16) | (z & 0xFFFFu));
        xport_gte_write_control(2u, (w << 16) | ((0u - z) & 0xFFFFu));
        xport_gte_write_control(4u, 0u - x);
        xport_gte_write_data(0u, (w << 17) | ((x << 1) & 0xFFFFu));
        xport_gte_write_data(1u, 0u);
        xport_gte_execute(0x486012u);
        for (component = 0u; component < 3u; ++component) first[component] = xport_gte_read_data(25u + component);
        xport_gte_write_data(0u, (y << 1) & 0xFFFFu);
        xport_gte_write_data(1u, w << 1);
        xport_gte_execute(0x486012u);
        w_u16(matrix, first[0] - 0x1000u);
        w_u16(matrix + 6u, first[1]); w_u16(matrix + 12u, first[2]);
        for (component = 0u; component < 3u; ++component) second[component] = xport_gte_read_data(25u + component);
        xport_gte_write_data(0u, (0u - (y << 1)) << 16);
        xport_gte_write_data(1u, x << 1);
        xport_gte_execute(0x486012u);
        value = ((uint32)((sint32)(z * z) >> 12) + (uint32)((sint32)(w * w) >> 12)) * 2u - 0x1000u;
        w_u16(matrix + 2u, second[0]); w_u16(matrix + 8u, second[1] - 0x1000u);
        w_u16(matrix + 14u, second[2]);
        w_u16(matrix + 4u, xport_gte_read_data(25u));
        w_u16(matrix + 10u, xport_gte_read_data(26u)); w_u16(matrix + 16u, value);
        for (component = 0u; component < 3u; ++component)
            w_u32(matrix + 20u + component * 4u, (uint32)(sint32)(sint16)r_u16(r_u32(model + 0x94u) + offset + component * 2u));
        for (component = 0u; component < 5u; ++component)
            xport_gte_write_control(component, r_u32(matrix + component * 4u));
        for (component = 0u; component < 3u; ++component)
            xport_gte_write_control(5u + component, r_u32(matrix + 20u + component * 4u));
        xport_gte_write_data(0u, r_u32(part + 0x14u));
        xport_gte_write_data(1u, r_u32(part + 0x18u));
        xport_gte_execute(0x480012u);
        for (component = 0u; component < 3u; ++component)
            w_u32(matrix + 20u + component * 4u, xport_gte_read_data(25u + component));
        moh_draft_missing_call(0x800163E8u, object + 0x340u, matrix + 20u, transformed);
        moh_draft_missing_call(0x8001BD08u, object + 0x340u, matrix);
        for (component = 0u; component < 3u; ++component) {
            value = r_u32(transformed + component * 4u) + r_u32(object + 0x354u + component * 4u);
            w_u32(matrix + 20u + component * 4u, value);
            w_u32(part + 0x3Cu + component * 4u, value << 16);
        }
    } else {
        moh_draft_missing_call(0x8008DABCu, object, model, bone, local, 0u, 0u);
        for (component = 0u; component < 3u; ++component)
            w_u32(part + 0x3Cu + component * 4u, (uint32)(sint32)(sint16)r_u16(local + component * 2u) << 16);
        for (component = 0u; component < 3u; ++component)
            w_u32(part + 0x60u + component * 4u, r_u32(object + 0x228u + component * 4u));
        if (!bone) { moh_draft_missing_call(0x80036E44u, local); return; }
        moh_draft_missing_call(0x8008DABCu, object, model, (uint32)(sint32)(sint16)r_u16(part + 0x70u), bone_a, 0u, 0u);
        moh_draft_missing_call(0x8008DABCu, object, model, (uint32)(sint32)(sint16)r_u16(part + 0x72u), bone_b, 0u, 0u);
        for (component = 0u; component < 3u; ++component)
            w_u16(axis_a + component * 2u, r_u16(bone_b + component * 2u) - r_u16(bone_a + component * 2u));
        moh_draft_missing_call(0x80016A40u, axis_a, axis_a);
        moh_draft_missing_call(0x8008DABCu, object, model, (uint32)(sint32)(sint16)r_u16(part + 0x6Cu), bone_a, 0u, 0u);
        moh_draft_missing_call(0x8008DABCu, object, model, (uint32)(sint32)(sint16)r_u16(part + 0x6Eu), bone_b, 0u, 0u);
        for (component = 0u; component < 3u; ++component)
            w_u16(axis_b + component * 2u, r_u16(bone_b + component * 2u) - r_u16(bone_a + component * 2u));
        moh_draft_missing_call(0x80016A40u, axis_b, axis_b);
        for (component = 0u; component < 3u; ++component) {
            xport_gte_write_control(component * 2u, (uint32)(sint32)(sint16)r_u16(axis_a + component * 2u));
            xport_gte_write_data(9u + component, (uint32)(sint32)(sint16)r_u16(axis_b + component * 2u));
        }
        xport_gte_execute(0x178000Cu);
        for (component = 0u; component < 3u; ++component) w_u16(cross + component * 2u, xport_gte_read_data(9u + component));
        for (component = 0u; component < 3u; ++component) {
            xport_gte_write_control(component * 2u, (uint32)(sint32)(sint16)r_u16(axis_a + component * 2u));
            xport_gte_write_data(9u + component, (uint32)(sint32)(sint16)r_u16(cross + component * 2u));
        }
        xport_gte_execute(0x178000Cu);
        for (component = 0u; component < 3u; ++component) w_u16(axis_b + component * 2u, xport_gte_read_data(9u + component));
        for (component = 0u; component < 3u; ++component) {
            w_u16(matrix + component * 6u, r_u16(axis_b + component * 2u));
            w_u16(matrix + component * 6u + 2u, r_u16(cross + component * 2u));
            w_u16(matrix + component * 6u + 4u, r_u16(axis_a + component * 2u));
        }
        for (component = 0u; component < 3u; ++component)
            w_u32(vector + component * 4u, (uint32)(sint32)(sint16)r_u16(part + 0x3Eu + component * 4u));
        moh_draft_missing_call(0x80016218u, matrix, vector);
        for (component = 0u; component < 3u; ++component)
            w_u32(vector + component * 4u, (uint32)((sint32)r_u32(part + 0x24u + component * 4u) >> 4));
        moh_draft_missing_call(0x800165D8u, matrix, vector);
    }
    moh_draft_missing_call(0x800154ECu, part + 0x74u, matrix, 32u);
    moh_draft_missing_call(0x80036E44u, local);
}

void level_sub_80050E08(uint32 object, uint32 part, uint32 rotation)
{
    FUNCTION_MARKER(0x80050E08u, "LEVEL.BIN");
    moh_draft_missing_call(0x8005081Cu, object, r_u32(object + 0x100u), part, rotation);
}

uint32 level_sub_80082CA8(uint32 model, uint32 ordering_table, uint32 buffer_index)
{
    uint32 tags, result, end, start;
    sint32 range, step, depth, index;
    FUNCTION_MARKER(0x80082CA8u, "LEVEL.BIN");
    tags = r_u32(model + buffer_index * 4u + 0x4Cu);
    if (r_u16(model + 0xAAu) & 0x8000u) {
        if (!(r_u32(model + 4u) & 0x20u)) {
            moh_draft_missing_call(0x8001722Cu, tags, 512u);
            w_u32(model + 4u, r_u32(model + 4u) | 0x20u);
        }
        w_u16(model + 0xAAu, r_u16(model + 0xAAu) & 0x7FFFu);
        result = moh_draft_missing_call(0x80080DD4u, model, tags, buffer_index, 0u);
        w_u32(0x8009E8E4u, r_u32(0x1F8003FCu));
        return result != 0u;
    }
    range = ((sint16)r_u16(0x8009E8E6u) - (sint16)r_u16(0x8009E8E4u)) >> 4;
    step = range < 2 ? 511 : (sint16)(511 / range);
    depth = (sint16)r_u16(0x8009E8E4u) >> 4;
    if (depth < 2) depth = 2;
    for (index = 0; index < 512; index += step) {
        if (depth > 511) depth = 511;
        start = tags + (uint32)(index ? index - 1 : 0) * 4u;
        end = index + step - 1 < 511 ? tags + (uint32)(index + step) * 4u - 4u : tags + 0x7FCu;
        moh_draft_missing_call(0x80013F98u, ordering_table + (uint32)depth * 4u, end, start);
        ++depth;
    }
    return 0u;
}

void level_sub_8005608C(uint32 object, uint32 render_context)
{
    uint32 state, model, block, index, offset;
    FUNCTION_MARKER(0x8005608Cu, "LEVEL.BIN");
    state = r_u32(object + 0x388u);
    w_u32(object + 0x29Cu, 0u);
    if (moh_draft_missing_call(0x8005CBA8u, 13u, object) != 0xFFFFFFFFu) {
        model = r_u32(object + 0x100u);
        w_u16(model + 0xAAu, r_u16(model + 0xAAu) | 0x8000u);
        moh_draft_missing_call(0x8006A5B4u, object, render_context);
        for (index = 0u; index < 3u; ++index) {
            block = state + 0x1Cu + index * 0x144u;
            if ((r_u32(block) & 0x27u) == 1u)
                moh_draft_missing_call(0x80050E08u, object, block, r_u32(block + 8u));
        }
    }
    if (r_u32(object + 0x29Cu) || moh_draft_missing_call(0x8005CBA8u, 13u, object) == 0xFFFFFFFFu) {
        moh_draft_missing_call(0x8006D4DCu, r_u32(object + 0x388u) + 0x18u, render_context);
        if (r_u32(object + 0x29Cu)) {
            offset = (r_u32(object + 0x10u) & 0x80u) ? 0xFFFFFFC0u : 0u;
            moh_draft_missing_call(0x80082CA8u, r_u32(object + 0x100u),
                r_u32(render_context + 0x2218u), r_u32(render_context + 0x221Cu), offset);
        }
        if (r_u32(state + 4u)) moh_draft_missing_call(0x80072310u, state + 0x3E8u, render_context);
        if (r_u32(state)) moh_draft_missing_call(0x800729C0u, state + 0x49Cu, render_context);
        moh_draft_missing_call(0x8006DD54u, state + 0xB68u, render_context);
    }
}

uint32 level_sub_8004CBC8(uint32 origin, uint32 object, uint32 direction, uint32 distance_out)
{
    uint32 component, radius, sum;
    sint32 delta[3], swap;
    VECTOR cross, squared;
    FUNCTION_MARKER(0x8004CBC8u, "LEVEL.BIN");
    if (!r_u16(object + 0x2E8u)) return 0u;
    for (component = 0u; component < 3u; ++component)
        delta[component] = (sint16)r_u16(object + 0x2E0u + component * 2u) - (sint16)r_u16(origin + 2u + component * 4u);
    for (component = 0u; component < 3u; ++component) {
        xport_gte_write_control(component * 2u, (uint32)(sint32)(sint16)delta[component]);
        xport_gte_write_data(9u + component, (uint32)(sint32)(sint16)r_u16(direction + component * 2u));
    }
    xport_gte_execute(0x178000Cu);
    cross.vx = (uint16)xport_gte_read_data(9u);
    cross.vy = (uint16)xport_gte_read_data(10u);
    cross.vz = (uint16)xport_gte_read_data(11u);
    Square0(&cross, &squared);
    radius = r_u16(object + 0x2E8u);
    sum = (uint32)squared.vx + (uint32)squared.vy + (uint32)squared.vz;
    if ((sint32)sum > (sint32)(radius * radius)) return 0u;
    if (distance_out) {
        for (component = 0u; component < 3u; ++component)
            if (delta[component] < 0) delta[component] = -delta[component];
        if (delta[0] < delta[1]) { swap = delta[0]; delta[0] = delta[1]; delta[1] = swap; }
        if (delta[0] < delta[2]) { swap = delta[0]; delta[0] = delta[2]; delta[2] = swap; }
        w_u32(distance_out, (uint32)(delta[0] + ((delta[1] + delta[2]) >> 2)));
    }
    return 1u;
}

void level_sub_80040180(uint32 object, uint32 velocity, uint32 direction, uint32 turn, uint32 distance)
{
    uint32 extra, component, coefficient, slot, scale, numerator, value, angle;
    sint32 cross_y, threshold, signed_turn;
    FUNCTION_MARKER(0x80040180u, "LEVEL.BIN");
    extra = r_u32(object + 0x38Cu);
    for (component = 0u; component < 3u; ++component) {
        xport_gte_write_control(component * 2u, (uint32)(sint32)(sint16)r_u16(velocity + component * 2u));
        xport_gte_write_data(9u + component, (uint32)(sint32)(sint16)r_u16(direction + component * 2u));
    }
    xport_gte_execute(0x178000Cu);
    cross_y = (sint32)xport_gte_read_data(26u);
    /* Other cross components are not consumed by the original callee */
    coefficient = (uint32)(sint32)(sint16)r_u16(extra + 0xC2u);
    if (coefficient) {
        if ((sint32)coefficient < (sint32)distance) {
            w_u32(object + 0x224u, 0x10000u);
            w_u32(extra + 0x18u, r_u32(extra + 0xDCu));
            w_u16(object + 0x2ECu, r_u16(extra + 0xC4u));
            w_u16(object + 0x2E8u, r_u16(extra + 0xC6u));
            slot = extra + (uint32)(sint32)(sint16)r_u16(extra + 0xC0u) * 2u;
            value = r_u16(slot + 0xD0u);
        } else {
            numerator = distance << 16;
            /* TODO: Native abort represents the original signed division BREAK */
            if (coefficient == 0xFFFFFFFFu && numerator == 0x80000000u) abort();
            scale = (uint32)((sint32)numerator / (sint32)coefficient);
            w_u32(object + 0x224u, scale);
            w_u32(extra + 0x18u, scale);
            value = (uint32)(sint32)(sint16)r_u16(extra + 0xC4u) * r_u32(object + 0x224u);
            w_u16(object + 0x2ECu, (uint32)((sint32)value >> 16));
            value = (uint32)(sint32)(sint16)r_u16(extra + 0xCAu) * r_u32(object + 0x224u);
            w_u16(object + 0x3C8u, (uint32)((sint32)value >> 16));
            slot = extra + (uint32)(sint32)(sint16)r_u16(extra + 0xC0u) * 2u;
            value = (uint32)(sint32)(sint16)r_u16(slot + 0xD0u) * r_u32(object + 0x224u);
            value = (uint32)((sint32)value >> 16);
        }
        w_u16(slot + 0xCCu, value);
    }
    cross_y >>= (sint32)distance < 2001 ? 1 : 2;
    signed_turn = (sint16)turn;
    if (cross_y < -32) {
        w_u32(extra + 8u, r_u32(extra + 8u) | 2u);
        threshold = (sint32)((uint32)cross_y + (uint32)signed_turn);
        turn = 0u - turn;
        if (threshold >= 0) turn = (uint16)cross_y;
    } else if (cross_y > 32) {
        w_u32(extra + 8u, r_u32(extra + 8u) | 1u);
        threshold = (sint32)((uint32)cross_y - (uint32)signed_turn);
        if (threshold <= 0) turn = (uint16)cross_y;
    } else {
        xport_gte_write_control(0u, r_u32(direction));
        xport_gte_write_control(1u, r_u32(direction + 4u));
        for (component = 0u; component < 3u; ++component)
            xport_gte_write_data(9u + component, r_u16(velocity + component * 2u));
        xport_gte_execute(0x41E012u);
        if ((sint32)xport_gte_read_data(25u) < 0) {
            if (cross_y < 0) {
                turn = 0u - turn;
                w_u32(extra + 8u, r_u32(extra + 8u) | 2u);
            } else w_u32(extra + 8u, r_u32(extra + 8u) | 1u);
        } else turn = 0u;
    }
    if (r_u32(extra + 4u) & 4u) moh_draft_missing_call(0x8003FFA4u, object);
    if (r_u32(extra + 8u) & 3u) {
        angle = r_u32(object + 0x22Cu) + (turn << 16);
        w_u32(object + 0x22Cu, angle);
        while ((sint32)angle < -0x8000000) angle += 0x10000000u;
        while ((sint32)angle > 0x8000000) angle -= 0x10000000u;
        w_u32(object + 0x22Cu, angle);
    }
}

uint32 level_sub_800730D4(uint32 object, uint32 effects)
{
    uint32 index, result = 0u, lifetime, x, y, z, floor, velocity, friction, acceleration, limit;
    FUNCTION_MARKER(0x800730D4u, "LEVEL.BIN");
    for (index = 0u; index < 15u; ++index, effects += 0x74u) {
        if (!(r_u32(effects) & 1u)) continue;
        result = 1u;
        lifetime = r_u32(effects + 4u) - 1u;
        w_u32(effects + 4u, lifetime);
        if ((sint32)lifetime <= 0) {
            w_u32(effects, 0u);
            w_u32(effects + 4u, 0u);
            continue;
        }
        x = r_u32(effects + 0x34u);
        y = r_u32(effects + 0x38u);
        z = r_u32(effects + 0x3Cu);
        w_u32(effects + 0x40u, x);
        w_u32(effects + 0x44u, y);
        w_u32(effects + 0x48u, z);
        y = r_u32(effects + 0x38u) + r_u32(effects + 0x20u);
        w_u32(effects + 0x38u, y);
        floor = r_u32(object + 0x208u);
        if ((sint32)floor < (sint32)y) {
            if (!(r_u32(effects) & 8u)) {
                w_u32(effects + 0x38u, floor);
                w_u32(effects + 0x24u, 0u);
                w_u32(effects + 0x20u, 0u);
                w_u32(effects + 0x1Cu, 0u);
                moh_draft_missing_call(0x80072EECu, effects);
                w_u32(effects, (r_u32(effects) & ~6u) | 8u);
            }
            continue;
        }
        if (r_u32(effects) & 8u) continue;
        w_u32(effects + 0x34u, r_u32(effects + 0x34u) + r_u32(effects + 0x1Cu));
        z = r_u32(effects + 0x3Cu);
        velocity = r_u32(effects + 0x24u);
        x = r_u32(effects + 0x1Cu);
        w_u32(effects + 0x3Cu, z + velocity);
        if ((sint32)x > 0) {
            friction = r_u32(effects + 0x14u);
            x -= friction;
            w_u32(effects + 0x1Cu, x);
            if ((sint32)x < 0) w_u32(effects + 0x1Cu, 0u);
        } else if ((sint32)x < 0) {
            x += r_u32(effects + 0x14u);
            w_u32(effects + 0x1Cu, x);
            if ((sint32)x > 0) w_u32(effects + 0x1Cu, 0u);
        }
        velocity = r_u32(effects + 0x24u);
        if ((sint32)velocity > 0) {
            velocity -= r_u32(effects + 0x14u);
            w_u32(effects + 0x24u, velocity);
            if ((sint32)velocity < 0) w_u32(effects + 0x24u, 0u);
        } else if ((sint32)velocity < 0) {
            velocity += r_u32(effects + 0x14u);
            w_u32(effects + 0x24u, velocity);
            if ((sint32)velocity > 0) w_u32(effects + 0x24u, 0u);
        }
        acceleration = r_u32(effects + 0x18u);
        velocity = r_u32(effects + 0x20u) + acceleration;
        limit = acceleration << 1;
        w_u32(effects + 0x20u, velocity);
        if ((sint32)limit < (sint32)velocity) w_u32(effects + 0x20u, limit);
        w_u32(effects + 8u, r_u32(effects + 8u) + r_u32(effects + 12u));
    }
    return result;
}

uint32 level_sub_80059500(uint32 object, uint32 data, uint32 frame)
{
    uint32 value, result;
    FUNCTION_MARKER(0x80059500u, "LEVEL.BIN");
    value = moh_draft_missing_call(0x8005D148u, 1u, frame, (uint32)r_u16(data), object);
    result = level_sub_8003F454(object, (sint16)value, 1u, r_u32(0x80099EB8u));
    if (result) return r_u32(object + r_u32(0x80099EB8u) * 116u + 0x118u);
    return data + 4u;
}

uint32 level_sub_8005898C(uint32 object, uint32 data)
{
    uint32 context, base, target;
    sint32 depth;
    FUNCTION_MARKER(0x8005898Cu, "LEVEL.BIN");
    (void)object;
    context = r_u32(0x80099EC4u); depth = (sint16)r_u16(context + 30u);
    if (depth) base = r_u32(context + ((uint32)depth - 1u) * 16u + 44u);
    else base = r_u32(r_u32(context + 16u) + 12u);
    target = base + (uint32)r_u16(data) * 4u; context = r_u32(0x80099EC4u);
    w_u32(context + 20u, target); return target;
}

uint32 level_sub_8004D670(uint32 buttons, uint32 mask, uint32 inverted)
{
    FUNCTION_MARKER(0x8004D670u, "LEVEL.BIN");
    return inverted ? (buttons & mask) == 0u : (buttons & mask) == mask;
}

uint32 level_sub_8004D694(uint32 count, uint32 controller, uint32 mask, uint32 edge, uint32 inverted)
{
    uint32 index, base = 0x80093C04u + controller * 40u;
    FUNCTION_MARKER(0x8004D694u, "LEVEL.BIN");
    count &= 0xFFFFu;
    for (index = 0u; index < count; ++index) {
        if (level_sub_8004D670(r_u32(base + index * 4u), mask, inverted)) {
            if (!edge || !level_sub_8004D670(r_u32(base + (index + 1u) * 4u), mask, inverted))
                return 1u;
        }
    }
    return 0u;
}

uint32 level_sub_8005D138(uint32 index, uint32 object)
{
    FUNCTION_MARKER(0x8005D138u, "LEVEL.BIN");
    return object + (index << 2u) + 0x4Cu;
}

uint32 level_sub_80059F38(uint32 object, uint32 data, uint32 frame)
{
    uint32 value, address;
    FUNCTION_MARKER(0x80059F38u, "LEVEL.BIN");
    value = moh_draft_missing_call(0x8005D148u, 1u, frame, r_u32(data + 4u), object);
    address = moh_draft_missing_call(0x8005D138u, value, object);
    value = moh_draft_missing_call(0x8005D148u, 2u, frame, r_u32(data + 8u), object);
    w_u32(address, r_u32(address) + value); return data + 12u;
}

uint32 level_sub_80059C2C(uint32 object, uint32 data)
{
    uint32 context, base, target;
    sint32 depth;
    FUNCTION_MARKER(0x80059C2Cu, "LEVEL.BIN");
    (void)object;
    context = r_u32(0x80099EC4u); depth = (sint16)r_u16(context + 30u);
    if (depth) base = r_u32(context + ((uint32)depth - 1u) * 16u + 44u);
    else base = r_u32(r_u32(context + 16u) + 12u);
    target = base + (uint32)r_u16(data) * 4u; context = r_u32(0x80099EC4u);
    w_u32(context + 20u, target); return target;
}

uint32 level_sub_8007B4B8(uint32 object, uint32 flags)
{
    FUNCTION_MARKER(0x8007B4B8u, "LEVEL.BIN");
    if (flags & 0x35u) {
        if (flags & 1u) {
            if (moh_draft_missing_call(0x8005CBA8u, 3u, object) & 2u)
                w_u32(object + 0x38u, r_u32(object + 0x38u) | 2u);
            w_u32(object + 0x218u, 0u); w_u32(object + 0x21Cu, 0u); w_u32(object + 0x220u, 0u);
        } else if (flags & 4u) w_u32(object + 0x21Cu, 0u);
    }
    return 0u;
}

static uint32 moh_player_distance(uint32 x, uint32 y, uint32 z)
{
    uint32 temporary;
    x = (sint32)x < 0 ? 0u - x : x; y = (sint32)y < 0 ? 0u - y : y; z = (sint32)z < 0 ? 0u - z : z;
    if ((sint32)x < (sint32)y) { temporary = x; x = y; y = temporary; }
    if ((sint32)x < (sint32)z) { temporary = x; x = z; z = temporary; }
    return x + (uint32)((sint32)(y + z) >> 2);
}

static uint32 moh_player_angle_step(uint32 difference, uint32 distance)
{
    uint32 value = difference & 0xFFFu;
    if (value > 0x800u) value -= 0x1000u;
    if ((sint32)distance <= 0x257FFFF) value = (uint32)((sint32)value >> 2);
    else if ((sint32)distance < 0x6400000) value = (uint32)((sint32)value >> 1);
    else if ((sint32)distance < 0xC800000) value -= (uint32)((sint32)value >> 2);
    if ((sint32)sub_8001DB60(value) < 8) value = (uint32)((sint32)value >> 2);
    return value;
}

static void moh_player_square(uint32 input, uint32 stride, uint32 result[3])
{
    VECTOR vector, square;
    vector.vx = (sint16)r_u16(input); vector.vy = (sint16)r_u16(input + stride); vector.vz = (sint16)r_u16(input + stride * 2u); vector.pad = 0;
    Square0(&vector, &square);
    result[0] = (uint32)square.vx; result[1] = (uint32)square.vy; result[2] = (uint32)square.vz;
}

void level_sub_8007E034(uint32 object, uint32 state)
{
    uint32 camera_object, camera_matrix, flags, mode, value, yaw, pitch, local, matrix, saved_offset, saved_base;
    uint32 axis, offset, origin, transformed, target, near_point, middle, direction, delta_vector, anchor, endpoint;
    uint32 x, y, z, dx, dy, dz, length, closest, denominator, cursor, next, candidate, material;
    uint32 distance, flat_target, flat_anchor, ax, az, bx, bz, difference, angle, projected, controls[8], squares[3];
    sint32 result;
    MATRIX saved;
    FUNCTION_MARKER(0x8007E034u, "LEVEL.BIN");
    /* TODO Unverified full C; collision and pose dependencies fail fast */
    if (r_u32(state + 0xA4u) > 400u && !(r_u32(state) & 0x200000u)) {
        w_u32(object + 0x234u, 0u); w_u32(object + 0x238u, 0u); return;
    }
    if (r_u32(state) & 0x800000u) {
        mode = r_u32(state + 0x1C8u);
        if (mode < 2u) {
            w_u32(object + 0x234u, r_u32(state + 0x178u) - r_u32(state + 0x17Cu) * 4u + r_u32(state + 0x180u) - r_u32(object + 0x228u));
            yaw = r_u32(state + 0x184u) + r_u32(state + 0x188u) * 8u;
        } else {
            w_u32(object + 0x234u, r_u32(state + 0x178u) + r_u32(state + 0x180u) - r_u32(object + 0x228u));
            yaw = r_u32(state + 0x184u);
        }
        w_u32(object + 0x238u, yaw + r_u32(state + 0x18Cu) - r_u32(object + 0x22Cu)); return;
    }
    camera_object = r_u32(0x8009D638u); camera_matrix = r_u32(camera_object + 0x388u) + 0x100u;
    /* Addressable vectors and matrices retain overlapping output views, not a guest CPU frame */
    local = sub_80016718(0xF8u);
    if (!local) moh_draft_fail_fast("Player camera local allocation", 0x8007E034u);
    origin = local; target = local + 0x10u; endpoint = local + 0x28u; matrix = local + 0x68u;
    transformed = local + 0x58u; near_point = local + 0x88u; middle = local + 0x98u;
    direction = local + 0x90u; delta_vector = local + 0xA8u; anchor = local + 0xB8u;
    /* TODO Original unused matrix and SVECTOR padding is unspecified */
    moh_draft_missing_call(0x8001BC28u, camera_matrix, matrix);
    w_u32(matrix + 20u, 0u); w_u32(matrix + 24u, 0u); w_u32(matrix + 28u, 0u);
    w_u16(matrix + 2u, (uint16)(((sint32)(sint16)r_u16(matrix + 2u) * 64) / 25));
    w_u16(matrix + 8u, (uint16)(((sint32)(sint16)r_u16(matrix + 8u) * 64) / 25));
    w_u16(matrix + 14u, (uint16)(((sint32)(sint16)r_u16(matrix + 14u) * 64) / 25));
    x = ((uint32)(sint32)(sint16)r_u16(state + 0x190u) - 0x100u) * 64u - r_u32(camera_matrix + 20u);
    y = ((uint32)(sint32)(sint16)r_u16(state + 0x192u) - ((uint32)(sint32)(sint16)r_u16(state + 0x4Eu) + 0x78u)) * 64u - r_u32(camera_matrix + 24u);
    z = r_u32(state + 0xA4u) * 64u - r_u32(camera_matrix + 28u);
    w_u16(endpoint, (uint16)((sint32)x >> 2)); w_u16(endpoint + 2u, (uint16)((sint32)y >> 2)); w_u16(endpoint + 4u, (uint16)((sint32)z >> 2));
    ReadRotMatrix(&saved);
    for (axis = 0u; axis < 4u; ++axis) {
        offset = axis * 2u;
        controls[axis] = (uint16)saved.m[offset / 3u][offset % 3u] | ((uint32)(uint16)saved.m[(offset + 1u) / 3u][(offset + 1u) % 3u] << 16);
    }
    controls[4] = (uint32)(sint32)saved.m[2][2];
    for (axis = 0u; axis < 3u; ++axis) controls[5u + axis] = (uint32)saved.t[axis];
    saved_offset = r_u32(0x8009D320u); saved_base = 0x8009D324u + saved_offset;
    for (axis = 0u; axis < 8u; ++axis) w_u32(saved_base + axis * 4u, controls[axis]);
    w_u32(0x8009D320u, saved_offset + 32u);
    for (axis = 0u; axis < 8u; ++axis) xport_gte_write_control(axis, r_u32(matrix + axis * 4u));
    xport_gte_write_data(0u, r_u32(endpoint)); xport_gte_write_data(1u, r_u32(endpoint + 4u)); xport_gte_execute(0x486012u);
    for (axis = 0u; axis < 3u; ++axis) w_u32(transformed + axis * 4u, xport_gte_read_data(25u + axis));
    saved_offset = r_u32(0x8009D320u) - 32u; saved_base = 0x8009D324u + saved_offset;
    w_u32(0x8009D320u, saved_offset);
    for (axis = 0u; axis < 8u; ++axis) xport_gte_write_control(axis, r_u32(saved_base + axis * 4u));
    for (axis = 0u; axis < 3u; ++axis) w_u32(origin + axis * 4u, r_u32(camera_object + 0x20Cu + axis * 4u));
    result = 0;
    for (axis = 0u; axis < 3u; ++axis) {
        if ((sint32)sub_8001DB60(r_u32(transformed + axis * 4u)) > 0x4000) { result = 1; break; }
    }
    for (axis = 0u; axis < 3u; ++axis) {
        value = r_u32(transformed + axis * 4u); x = r_u32(origin + axis * 4u);
        if (result) value = (((uint32)((sint32)value >> 2) << 16) - (uint32)((sint32)x >> 2)) * 2u;
        else value = (uint32)((sint32)((value << 16) - x) >> 1);
        w_u32(target + axis * 4u, x + value);
    }
    for (axis = 0u; axis < 3u; ++axis) {
        x = r_u32(origin + axis * 4u); value = r_u32(target + axis * 4u) - x;
        w_u32(near_point + axis * 4u, x + (uint32)((sint32)value >> 3));
        w_u32(middle + axis * 4u, x + (uint32)((sint32)value >> 1));
    }
    result = (sint32)moh_draft_missing_call(0x8003EF5Cu, near_point, origin, local + 0x20u, endpoint, local + 0xE8u, local + 0x30u, 0u);
    if (result < 0) {
        result = (sint32)moh_draft_missing_call(0x8003EF5Cu, middle, near_point, local + 0x20u, endpoint, local + 0xE8u, local + 0x30u, 0u);
        if (result < 0) {
            result = (sint32)moh_draft_missing_call(0x8003EF5Cu, target, middle, local + 0x20u, endpoint, local + 0xE8u, local + 0x30u, 0u);
            if (result < 0) for (axis = 0u; axis < 3u; ++axis) w_u16(endpoint + axis * 2u, r_u16(target + axis * 4u + 2u));
        }
    }
    w_u32(state, r_u32(state) & 0x7FEFFFFFu);
    for (axis = 0u; axis < 3u; ++axis) {
        value = (uint32)((sint32)(sint16)r_u16(endpoint + axis * 2u) - (sint32)(sint16)r_u16(origin + axis * 4u + 2u));
        w_u32(direction + axis * 4u, value); w_u16(near_point + axis * 2u, (uint16)value);
    }
    length = moh_player_distance(r_u32(direction), r_u32(direction + 4u), r_u32(direction + 8u));
    moh_player_square(near_point, 2u, squares); value = squares[0] + squares[1] + squares[2];
    moh_draft_missing_call(0x80016A40u, near_point, near_point);
    cursor = r_u32(0x8009D64Cu); closest = length;
    while (cursor) {
        next = r_u32(cursor + 4u);
        if (cursor != object && !(r_u32(cursor + 12u) & 0x20000000u) && !(r_u32(cursor + 0x10u) & 0x206u) &&
            moh_draft_missing_call(0x8004CBC8u, origin, cursor, near_point, 0u)) {
            result = 1;
            if (r_u32(cursor + 12u) & 0x10400u) {
                if (!r_u16(cursor + 0x2EEu)) result = (sint32)moh_draft_missing_call(0x8004C76Cu, origin, near_point, value, cursor, local + 0xF4u);
                else {
                    for (axis = 0u; axis < 3u; ++axis) w_u16(anchor + axis * 2u, r_u16(origin + axis * 4u + 2u));
                    result = (sint32)moh_draft_missing_call(0x8004B0F0u, anchor, endpoint, cursor, local + 0xECu, anchor + 8u, anchor + 16u, local + 0xF0u);
                }
            }
            if (result) {
                for (axis = 0u; axis < 3u; ++axis) w_u32(delta_vector + axis * 4u,
                    (uint32)((sint32)(r_u32(origin + axis * 4u) - r_u32(cursor + 0x20Cu + axis * 4u)) >> 16));
                candidate = moh_player_distance(r_u32(delta_vector), r_u32(delta_vector + 4u), r_u32(delta_vector + 8u));
                if ((sint32)candidate < (sint32)closest) {
                    closest = candidate;
                    if ((sint32)candidate >= 0x4B0 && (r_u32(cursor + 12u) & 0x50u)) {
                        flags = r_u32(state) | 0x100000u; w_u32(state, flags);
                        if ((sint32)candidate >= 0xABEu) w_u32(state, flags | 0x80000000u);
                    }
                }
            }
        }
        cursor = next;
    }
    if (closest != length) {
        x = (uint32)(sint32)(sint16)r_u16(origin + 2u); y = (uint32)(sint32)(sint16)r_u16(origin + 6u); z = (uint32)(sint32)(sint16)r_u16(origin + 10u);
        dx = (uint32)(sint32)(sint16)r_u16(endpoint) - x; dy = (uint32)(sint32)(sint16)r_u16(endpoint + 2u) - y; dz = (uint32)(sint32)(sint16)r_u16(endpoint + 4u) - z;
        w_u32(delta_vector, dx); w_u32(delta_vector + 4u, dy); w_u32(delta_vector + 8u, dz);
        denominator = moh_player_distance(dx, dy, dz) + 1u;
        if (!denominator) moh_draft_fail_fast("Player camera division by zero", 0x8007E034u);
        if (denominator == 0xFFFFFFFFu && (dx * closest == 0x80000000u || dy * closest == 0x80000000u || dz * closest == 0x80000000u))
            moh_draft_fail_fast("Player camera division overflow", 0x8007E034u);
        dx = (uint32)((sint32)(dx * closest) / (sint32)denominator); dy = (uint32)((sint32)(dy * closest) / (sint32)denominator); dz = (uint32)((sint32)(dz * closest) / (sint32)denominator);
        w_u16(endpoint, (uint16)(x + dx)); w_u16(endpoint + 2u, (uint16)(y + dy)); w_u16(endpoint + 4u, (uint16)(z + dz));
    }
    for (axis = 0u; axis < 3u; ++axis) w_u32(state + 0x1A4u + axis * 4u, (uint32)(sint32)(sint16)r_u16(endpoint + axis * 2u) << 16);
    material = r_u32(r_u32(object + 0x100u) + 8u);
    x = (uint32)(sint32)(sint16)r_u16(material + 0xD4u); y = (uint32)(sint32)(sint16)r_u16(material + 0xD6u);
    material = r_u32(r_u32(object + 0x100u) + 8u) + 0xD8u;
    z = r_u32(r_u32(object + 0x100u) + 8u) + 0xE0u;
    moh_draft_missing_call(0x80044E0Cu, object, anchor, x, y, 0u, material, z, near_point, direction);
    dx = r_u32(camera_object + 0x20Cu) - r_u32(state + 0x1A4u); dz = r_u32(camera_object + 0x214u) - r_u32(state + 0x1ACu);
    ax = sub_8001DB60((uint32)((sint32)dx >> 2)); az = sub_8001DB60((uint32)((sint32)dz >> 2));
    flat_target = ax + az - (uint32)((sint32)((sint32)ax < (sint32)az ? ax : az) >> 1);
    bx = sub_8001DB60((uint32)((sint32)(r_u32(camera_object + 0x20Cu) - r_u32(anchor)) >> 2));
    bz = sub_8001DB60((uint32)((sint32)(r_u32(camera_object + 0x214u) - r_u32(anchor + 8u)) >> 2));
    flat_anchor = bx + bz - (uint32)((sint32)((sint32)bx < (sint32)bz ? bx : bz) >> 1);
    w_u32(local + 0xD8u, dx); dy = r_u32(camera_object + 0x210u) - r_u32(state + 0x1A8u); w_u32(local + 0xDCu, dy);
    dz = r_u32(camera_object + 0x214u) - r_u32(state + 0x1ACu); w_u32(local + 0xE0u, dz);
    distance = moh_player_distance(dx, dy, dz);
    for (axis = 0u; axis < 3u; ++axis) w_u32(local + 0xD8u + axis * 4u, r_u32(camera_object + 0x20Cu + axis * 4u) - r_u32(anchor + axis * 4u));
    if ((sint32)flat_target < (sint32)(flat_anchor + 0x140000u)) {
        for (axis = 0u; axis < 3u; ++axis) w_u32(state + 0x198u + axis * 4u, r_u32(origin + axis * 4u));
        w_u32(state, r_u32(state) | 0x20000u);
        for (axis = 0u; axis < 8u; ++axis) xport_gte_write_control(axis, r_u32(camera_matrix + axis * 4u));
        xport_gte_write_data(0u, r_u32(direction)); xport_gte_write_data(1u, r_u32(direction + 4u)); xport_gte_execute(0x180001u);
        projected = xport_gte_read_data(14u); w_u32(local + 0xD0u, projected);
        difference = (uint32)((sint32)(sint16)r_u16(state + 0x190u) - (sint32)(sint16)projected);
        value = difference << 16;
        if ((sint32)sub_8001DB60(difference) < 9) value = (uint32)((sint32)value >> 1);
        w_u32(object + 0x238u, value);
        difference = (uint32)((sint32)(sint16)(projected >> 16) - (sint32)(sint16)r_u16(state + 0x192u));
        x = sub_8001DB60(difference);
        if ((sint32)x >= 9) pitch = (difference + (uint32)((sint32)difference >> 2)) << 16;
        else if ((sint32)sub_8001DB60(difference) > 4) pitch = difference << 16;
        else pitch = (uint32)((sint32)(difference << 16) >> 1);
    } else {
        value = (r_u32(state) & 0x200000u) ? origin : anchor;
        for (axis = 0u; axis < 3u; ++axis) w_u32(state + 0x198u + axis * 4u, r_u32(value + axis * 4u));
        w_u32(state, r_u32(state) & 0xFFFDFFFFu);
        for (axis = 0u; axis < 3u; ++axis) {
            x = (uint32)(sint32)(sint16)r_u16(near_point + axis * 2u);
            y = (uint32)(sint32)(sint16)r_u16(direction + axis * 2u);
            z = (uint32)(sint32)(sint16)r_u16(endpoint + axis * 2u);
            w_u32(middle + axis * 4u, y - x); w_u32(delta_vector + axis * 4u, z - x);
        }
        sub_80016A10(delta_vector, delta_vector); sub_80016A10(middle, middle);
        angle = moh_draft_missing_call(0x8001536Cu, r_u32(middle + 8u), r_u32(middle));
        value = moh_draft_missing_call(0x8001536Cu, r_u32(delta_vector + 8u), r_u32(delta_vector));
        difference = moh_player_angle_step(angle - value, distance); w_u32(object + 0x238u, difference << 16);
        moh_player_square(middle, 4u, squares);
        value = moh_draft_missing_call(0x80016188u, squares[0] + squares[2]);
        angle = moh_draft_missing_call(0x8001536Cu, r_u32(middle + 4u), value);
        moh_player_square(delta_vector, 4u, squares);
        value = moh_draft_missing_call(0x80016188u, squares[0] + squares[2]);
        value = moh_draft_missing_call(0x8001536Cu, r_u32(delta_vector + 4u), value);
        difference = moh_player_angle_step(angle - value, distance);
        value = r_u32(object + 0x228u); pitch = value + (difference << 16);
        if ((sint32)pitch > 0x2AA0000) pitch = 0x2AA0000u;
        if ((sint32)pitch <= -0x2AA0000) pitch = 0xFD560000u;
        pitch = (uint32)((sint32)(value + (pitch << 3) - pitch) >> 3) - value;
    }
    if (r_u32(object + 8u) == 1u) {
        // TODO Restore muzzle alignment feedback after its native coordinate path is repaired
        pitch = r_u32(state + 0x178u);
        yaw = r_u32(state + 0x184u);
        if (r_u32(state + 0x1C8u) < 2u) {
            pitch -= r_u32(state + 0x17Cu) * 4u;
            yaw += r_u32(state + 0x188u) * 8u;
        }
        pitch -= r_u32(object + 0x228u);
        w_u32(object + 0x238u, yaw - r_u32(object + 0x22Cu));
    }
    w_u32(object + 0x234u, pitch);
    if (r_u32(state + 0x1C8u) < 2u) {
        w_u32(state + 0x180u, r_u32(object + 0x228u) + r_u32(object + 0x234u) - (r_u32(state + 0x178u) - r_u32(state + 0x17Cu) * 4u));
        w_u32(state + 0x18Cu, r_u32(object + 0x22Cu) + r_u32(object + 0x238u) - (r_u32(state + 0x184u) + r_u32(state + 0x188u) * 8u));
    } else {
        w_u32(state + 0x180u, r_u32(object + 0x228u) + r_u32(object + 0x234u) - r_u32(state + 0x178u));
        w_u32(state + 0x18Cu, r_u32(object + 0x22Cu) + r_u32(object + 0x238u) - r_u32(state + 0x184u));
    }
    sub_80036E44(local);
}

uint32 sub_8001DB60(uint32 value)
{
    FUNCTION_MARKER(0x8001DB60u, "SLUS_009.74");
    return (sint32)value < 0 ? 0u - value : value;
}

uint32 level_sub_8004D590(uint32 controller, uint32 mask, uint32 edge, uint32 released)
{
    uint32 current = r_u32(0x80093B7Cu + controller * 4u) & mask;
    FUNCTION_MARKER(0x8004D590u, "LEVEL.BIN");
    if (released) {
        if (!edge) return current == 0u;
        if (current) return 0u;
        return (r_u32(0x80093B84u + controller * 4u) & mask) != 0u;
    }
    if (!edge) return current == mask;
    if (current != mask) return 0u;
    return ((r_u32(0x80093B84u + controller * 4u) & mask) ^ mask) != 0u;
}

uint32 level_sub_800760AC(uint32 controller)
{
    FUNCTION_MARKER(0x800760ACu, "LEVEL.BIN");
    return r_u32(0x8009E3A0u + controller * 4u);
}

uint32 level_sub_800759EC(uint32 controller, uint32 axis)
{
    FUNCTION_MARKER(0x800759ECu, "LEVEL.BIN");
    return r_u32(0x800B760Cu + controller * 0xD0u + axis * 0x34u);
}

uint32 level_sub_80075FB4(uint32 controller, uint32 axis, uint32 enabled)
{
    uint32 record = 0x800B74B0u + controller * 0xA0u + axis * 0x28u;
    FUNCTION_MARKER(0x80075FB4u, "LEVEL.BIN");
    return moh_draft_missing_call(0x80075E7Cu, controller, record, r_u32(record), enabled);
}

uint32 level_sub_80076008(uint32 controller, uint32 axis, uint32 modifier, uint32 enabled)
{
    uint32 record = 0x800B74B0u + controller * 0xA0u;
    FUNCTION_MARKER(0x80076008u, "LEVEL.BIN");
    if (!r_u32(0x8009E398u + controller * 4u)) return 0u;
    return moh_draft_missing_call(0x80075E7Cu, controller, record + modifier * 0x28u, r_u32(record + axis * 0x28u), enabled);
}

uint32 level_sub_80075A30(uint32 controller, uint32 axis, uint32 gated)
{
    uint32 record = 0x800B75F0u + controller * 0xD0u + axis * 0x34u;
    uint32 modifier, mask, buttons, flags, value, step, limit, current;
    sint32 direction = 0;
    FUNCTION_MARKER(0x80075A30u, "LEVEL.BIN");
    modifier = gated ? r_u32(record + 8u) : 0u;
    if (!modifier || moh_draft_missing_call(0x8004D590u, controller, modifier, 0u, r_u32(record + 12u) == 0u)) {
        mask = r_u32(record); buttons = r_u32(0x80093B7Cu + controller * 4u);
        if ((buttons & mask) == mask) direction = r_u32(record + 0x20u) ? 1 : -1;
        else {
            mask = r_u32(record + 4u);
            if ((buttons & mask) == mask) direction = r_u32(record + 0x20u) ? -1 : 1;
        }
    }
    if (!direction) {
        flags = r_u32(record + 0x30u) & 6u;
        if (flags == 2u) w_u32(record + 0x24u, 0u);
        flags = r_u32(record + 0x30u) & 0x18u;
        if (!flags) w_u32(record + 0x2Cu, 0u);
        else if (flags == 8u) {
            step = r_u32(record + 0x28u); value = r_u32(record + 0x2Cu) - step * 4u;
            w_u32(record + 0x2Cu, (sint32)value > 0 ? value : 0u);
        }
    } else {
        value = r_u32(record + 0x2Cu); step = r_u32(record + 0x28u); limit = r_u32(record + 0x1Cu);
        value += step; if ((sint32)value > (sint32)limit) value = limit;
        current = r_u32(record + 0x24u);
        if (direction < 0) {
            w_u32(record + 0x2Cu, value); current -= value; limit = 0u - r_u32(record + 0x1Cu);
            if ((sint32)current <= (sint32)limit) current = limit;
        } else {
            limit = r_u32(record + 0x1Cu); w_u32(record + 0x2Cu, value); current += value;
            if ((sint32)current > (sint32)limit) current = limit;
        }
        w_u32(record + 0x24u, current);
    }
    return r_u32(record + 0x24u);
}

static uint32 moh_player_axis(uint32 controller, uint32 axis)
{
    uint32 value = moh_draft_missing_call(0x80075A30u, controller, axis, 1u);
    if (!value && r_u8(0x80093B78u + controller)) value = moh_draft_missing_call(0x80075FB4u, controller, axis, 1u);
    return value;
}

static uint32 moh_player_precision(uint32 state, uint32 value)
{
    uint32 flags = r_u32(state);
    value = (uint32)((sint32)value >> 1) + ((flags & 0x80000u) ? 0u - (uint32)((sint32)value >> 3) : (uint32)((sint32)value >> 4));
    flags = r_u32(state);
    if (flags & 0x100000u) value = (uint32)((sint32)value >> ((sint32)flags < 0 ? 2 : 1));
    return value;
}

static uint32 moh_player_scaled_axis(uint32 state, uint32 value)
{
    uint32 flags = r_u32(state);
    if (flags & 0x100u) return (uint32)((sint32)value >> 3);
    if (r_u32(state + 0x1C8u) < 2u) return moh_player_precision(state, value);
    if (flags & 0x100000u) value = (uint32)((sint32)value >> ((sint32)flags < 0 ? 2 : 1));
    return value;
}

static sint32 moh_player_divide(uint32 numerator, sint32 denominator)
{
    if (!denominator) moh_draft_fail_fast("Player division by zero", 0x8007F300u);
    if (denominator == -1 && numerator == 0x80000000u) moh_draft_fail_fast("Player division overflow", 0x8007F300u);
    return (sint32)numerator / denominator;
}

void level_sub_8007F300(uint32 object, uint32 controller)
{
    uint32 state, camera, flags, value, mode, accumulator, position_x, position_y, position_z, object_flags;
    uint32 local, current, product, absolute, quotient;
    sint32 handled = 0, divisor, control_mode;
    sint16 limit;
    FUNCTION_MARKER(0x8007F300u, "LEVEL.BIN");
    /* TODO Unverified full translation; missing axis and weapon services fail fast */
    state = r_u32(object + 0x388u); flags = r_u32(state);
    camera = r_u32(r_u32(0x8009D638u) + 0x388u); w_u32(state, flags & 0xFFFFFFF3u);
    if (!(flags & 0x20u)) {
        if (r_u32(state + 4u) & 0x20u) {
            position_x = r_u32(state + 0x90u); position_y = r_u32(state + 0x94u); position_z = r_u32(state + 0x98u);
            w_u32(object + 0x20Cu, position_x); w_u32(object + 0x210u, position_y); w_u32(object + 0x214u, position_z);
            w_u32(state + 0x9Cu, 0u); w_u32(object + 12u, r_u32(object + 12u) | 0x20u);
            w_u32(object + 0x10u, r_u32(object + 0x10u) & 0xFFFFFFEFu);
        }
        if (r_u32(state + 0x70u)) handled = (sint32)moh_draft_missing_call(0x8007F1E0u, object, state, controller);
        if (!handled) { local = moh_player_axis(controller, 0u); w_u32(object + 0x200u, 0u - local); }
        local = moh_player_axis(controller, 1u); w_u32(object + 0x218u, local);
    } else {
        if (!(r_u32(state + 4u) & 0x20u)) {
            position_x = r_u32(object + 0x20Cu); position_y = r_u32(object + 0x210u); position_z = r_u32(object + 0x214u);
            w_u32(state + 0x90u, position_x); w_u32(state + 0x94u, position_y); w_u32(state + 0x98u, position_z);
            object_flags = r_u32(object + 12u); value = r_u32(object + 0x10u);
            w_u32(object + 12u, object_flags & 0xFFFFFFDFu); w_u32(object + 0x10u, value | 0x10u);
        }
        w_u32(object + 0x200u, 0u); local = moh_player_axis(controller, 1u);
        position_x = r_u32(state + 0x90u); current = r_u32(object + 0x20Cu); value = r_u32(object + 0x1F0u);
        w_u32(object + 0x1F0u, value + position_x - current);
        position_y = r_u32(state + 0x94u); position_z = r_u32(object + 0x214u);
        w_u32(object + 0x1F4u, 0u); w_u32(object + 0x210u, position_y);
        current = r_u32(state + 0x98u); value = r_u32(object + 0x1F8u);
        w_u32(object + 0x1F8u, value + current - position_z);
        value = (uint32)((sint32)(local * 2u + r_u32(state + 0x9Cu) * 3u) >> 2);
        w_u32(object + 0x218u, value); w_u32(state + 0x9Cu, value);
    }
    mode = r_u32(state + 0x1C8u);
    if (mode < 2u) {
        if (mode != r_u32(state + 0x1CCu)) {
            flags = r_u32(state); w_u32(state + 0x188u, 0u); w_u32(state, flags & 0x7FEFFFFFu);
            w_u32(object + 0x238u, 0u); w_u32(state + 0x17Cu, 0u); w_u32(object + 0x234u, 0u);
        }
        if (level_sub_8007570C(controller, 7u)) moh_draft_missing_call(0x8007F084u, object, state, controller);
        else {
            divisor = (sint32)moh_draft_missing_call(0x800759ECu, controller, 3u);
            local = moh_player_precision(state, moh_player_axis(controller, 3u));
            accumulator = r_u32(state + 0x188u) + (uint32)((sint32)local >> 3);
            quotient = (uint32)moh_player_divide(accumulator << 8, divisor); w_u32(state + 0x188u, accumulator);
            absolute = moh_draft_missing_call(0x8001573Cu, (uint32)(sint32)(sint16)quotient);
            limit = (sint16)r_u16(state + 0xB4u);
            if ((sint32)limit < (sint32)absolute) {
                if ((sint32)r_u32(state + 0x188u) > 0) {
                    product = (uint32)(sint32)(sint16)r_u16(state + 0xB4u) * (uint32)divisor;
                    w_u16(state + 0x190u, (uint16)(r_u16(state + 0xB4u) + 0x100u));
                } else {
                    product = 0u - (uint32)(sint32)(sint16)r_u16(state + 0xB4u) * (uint32)divisor;
                    w_u16(state + 0x190u, (uint16)(0x100u - r_u16(state + 0xB4u)));
                }
                if ((sint32)product < 0) product += 0xFFu;
                w_u32(state + 0x188u, (uint32)((sint32)product >> 8));
                w_u32(state + 0x184u, r_u32(state + 0x184u) + local); w_u32(object + 0x22Cu, r_u32(object + 0x22Cu) + local);
            } else w_u16(state + 0x190u, (uint16)(quotient + 0x100u));
            divisor = (sint32)moh_draft_missing_call(0x800759ECu, controller, 2u);
            local = moh_draft_missing_call(0x80075A30u, controller, 2u, 1u);
            if (!local && r_u8(0x80093B78u + controller)) {
                local = moh_draft_missing_call(0x80075FB4u, controller, 2u, 1u);
                if (!local) local = moh_draft_missing_call(0x80076008u, controller, 0u, 2u, 1u);
            }
            local = moh_player_precision(state, local); accumulator = r_u32(state + 0x17Cu) + (uint32)((sint32)local >> 3);
            w_u32(state + 0x17Cu, accumulator); quotient = (uint32)moh_player_divide(accumulator * 120u, divisor);
            local = (uint32)((sint32)local >> 1); absolute = moh_draft_missing_call(0x8001573Cu, (uint32)(sint32)(sint16)quotient);
            limit = (sint16)r_u16(state + 0xB6u);
            if ((sint32)limit < (sint32)absolute) {
                if ((sint32)r_u32(state + 0x17Cu) > 0) {
                    product = (uint32)(sint32)(sint16)r_u16(state + 0xB6u) * (uint32)divisor;
                    w_u16(state + 0x192u, (uint16)(r_u16(state + 0xB6u) + 0x83u));
                } else {
                    product = 0u - (uint32)(sint32)(sint16)r_u16(state + 0xB6u) * (uint32)divisor;
                    w_u16(state + 0x192u, (uint16)(0x83u - r_u16(state + 0xB6u)));
                }
                /* Original signed multiply-high sequence divides the wrapped product by 120 */
                w_u32(state + 0x17Cu, (uint32)((sint32)product / 120));
                w_u32(state + 0x178u, r_u32(state + 0x178u) - local);
                value = r_u32(object + 0x228u) - local; w_u32(object + 0x228u, value); current = r_u32(state + 0x178u);
                if ((sint32)current > 0x2AA0000) {
                    w_u32(object + 0x228u, value + 0x2AA0000u - current); w_u32(state + 0x178u, 0x2AA0000u);
                    current = r_u32(state + 0x178u);
                }
                if ((sint32)current < -0x2AA0000) {
                    w_u32(object + 0x228u, r_u32(object + 0x228u) - 0x2AA0000u - current); w_u32(state + 0x178u, 0xFD560000u);
                }
                level_sub_8007BADC(object, state, camera);
            } else w_u16(state + 0x192u, (uint16)(quotient + 0x83u));
        }
        moh_draft_missing_call(0x8007E034u, object, state); w_u32(state + 0x1CCu, mode); return;
    }
    if (mode != r_u32(state + 0x1CCu)) {
        flags = r_u32(state); w_u32(state + 0x188u, 0u); w_u32(state, flags & 0x7FEFFFFFu);
        w_u32(object + 0x238u, 0u); w_u32(state + 0x17Cu, 0u); w_u32(object + 0x234u, 0u);
        w_u16(state + 0x190u, mode == 5u ? 0x10Eu : 0x100u); w_u16(state + 0x192u, mode == 5u ? 0x73u : 0x83u);
    }
    local = moh_player_scaled_axis(state, moh_player_axis(controller, 3u));
    if (local + 0x10000u <= 0x20000u) value = (uint32)((sint32)(r_u32(state + 0x188u) + 1u) >> 4);
    else value = (uint32)((sint32)(r_u32(state + 0x188u) * 3u + local) >> 2);
    w_u32(state + 0x188u, value); w_u32(state + 0x184u, r_u32(state + 0x184u) + local);
    control_mode = (sint32)moh_draft_missing_call(0x800760ACu, controller);
    if (control_mode == 1) {
        if (level_sub_8007570C(controller, 7u)) goto recenter;
        goto pitch_axis;
    }
    if (control_mode == 0) {
        if (level_sub_80075778(controller, 7u)) w_u32(state, r_u32(state) ^ 0x10u);
        if (!(r_u32(state) & 0x10u)) {
            local = moh_draft_missing_call(0x80075A30u, controller, 2u, 1u);
            if (!local && r_u8(0x80093B78u + controller)) {
                local = moh_draft_missing_call(0x80075FB4u, controller, 2u, 1u);
                if (!local && (r_u32(state) & 0x20u)) local = moh_draft_missing_call(0x80076008u, controller, 0u, 2u, 1u);
            }
            w_u32(state + 0x178u, (uint32)((sint32)(r_u32(state + 0x178u) * 3u - local) >> 2));
        }
        goto finish_look;
    }
    if (control_mode != 2) goto finish_look;
    if (level_sub_80075778(controller, 7u)) w_u32(state, r_u32(state) ^ 0x10u);
    if (r_u32(state) & 0x10u) goto finish_look;
    if (!(r_u32(state) & 0x20u)) goto recenter;
pitch_axis:
    local = moh_draft_missing_call(0x80075A30u, controller, 2u, 1u);
    if (!local && r_u8(0x80093B78u + controller)) {
        local = moh_draft_missing_call(0x80075FB4u, controller, 2u, 1u);
        if (!local && (control_mode == 2 || (r_u32(state) & 0x20u))) local = moh_draft_missing_call(0x80076008u, controller, 0u, 2u, 1u);
    }
    local = moh_player_scaled_axis(state, local);
    if (local) {
        local = r_u32(state + 0x178u) - (uint32)((sint32)local >> 1);
        if ((sint32)local > 0x2AA0000) local = 0x2AA0000u;
        if ((sint32)local < -0x2AA0000) local = 0xFD560000u;
        w_u32(state + 0x178u, local);
    }
    goto finish_look;
recenter:
    absolute = moh_draft_missing_call(0x8001DB60u, (uint32)((sint32)r_u32(state + 0x178u) >> 2));
    value = r_u32(state + 0x178u);
    if ((sint32)absolute > 0x27FFFF) value = (uint32)((sint32)(value * 7u + 1u) >> 3);
    else value = (uint32)((sint32)(value * 3u + 1u) >> 2);
    w_u32(state + 0x178u, value);
finish_look:
    if (mode - 7u < 2u) moh_draft_missing_call(0x8007E034u, object, state);
    else { w_u32(object + 0x228u, r_u32(state + 0x178u)); w_u32(object + 0x22Cu, r_u32(state + 0x184u)); }
    level_sub_8007BADC(object, state, camera); w_u32(state + 0x1CCu, mode);
}

uint32 level_sub_8007570C(uint32 controller, uint32 action)
{
    uint32 offset = (controller * 9u + action) * 4u;
    uint32 required, buttons, excluded;
    FUNCTION_MARKER(0x8007570Cu, "LEVEL.BIN");
    required = r_u32(0x800B7790u + offset);
    buttons = r_u32(0x80093B7Cu + controller * 4u);
    excluded = r_u32(0x800B77E0u + offset);
    return (buttons & required) == required && (!excluded || (buttons & excluded) != excluded);
}

uint32 level_sub_80075778(uint32 controller, uint32 action)
{
    uint32 offset = (controller * 9u + action) * 4u;
    uint32 required, buttons, excluded, matched;
    FUNCTION_MARKER(0x80075778u, "LEVEL.BIN");
    required = r_u32(0x800B7790u + offset);
    buttons = r_u32(0x80093B7Cu + controller * 4u);
    excluded = r_u32(0x800B77E0u + offset); matched = buttons & required;
    if (matched != required || (r_u32(0x80093B84u + controller * 4u) & matched) == matched) return 0u;
    return !excluded || (buttons & excluded) != excluded;
}

void level_sub_80075118(uint32 object)
{
    FUNCTION_MARKER(0x80075118u, "LEVEL.BIN");
    w_u32(object + 0x284u, 0u);
}

void level_sub_8007A6B0(uint32 object)
{
    uint32 state, controller, flags, value, camera, current, target, delta, previous, index, kind;
    uint32 changed, selection, camera_y, camera_other_y, fixed, camera_first, camera_second;
    sint16 animation;
    FUNCTION_MARKER(0x8007A6B0u, "LEVEL.BIN");
    /* TODO Unverified full translation; unresolved player services fail fast */
    state = r_u32(object + 0x388u); controller = r_u8(state + 0x152u);
    if ((sint8)r_u8(state + 0x153u) != (sint8)r_u8(0x80093B78u + controller))
        level_sub_8007A050(object, state, controller);
    if ((sint32)r_u32(object + 0x210u) > 0x13880000) {
        moh_draft_missing_call(0x8004AC58u, object, 0x27100000u, 0u, 0xFFFFFFFFu);
        moh_draft_missing_call(0x80015B28u, 0x8003D03Cu);
    }
    if (r_u32(object + 0x38u) & 0x4000u) {
        moh_draft_missing_call(0x80079D10u, object, controller, r_u32(state + 0x1C0u));
        goto finish;
    }
    if (!r_u32(object + 0x284u)) w_u32(state, r_u32(state) & 0xFFDFFF9Fu);
    else {
        value = level_sub_8005CBA8(3u, object);
        if (value & 2u) w_u32(object + 0x38u, r_u32(object + 0x38u) | 2u);
        if (!r_u32(object + 0x284u) || !level_sub_8007570C(controller, 8u))
            w_u32(state, r_u32(state) & 0xFFDFFF9Fu);
        else {
            flags = r_u32(state); w_u32(state, flags | 0x20u);
            if (r_u8(state + 0x55u) == 5u) {
                w_u32(state, (flags & 0xFFDFFFFFu) | 0x160u); w_u32(state + 0xA8u, 0x98Fu);
            } else if ((flags & 0x400000u) && (sint16)r_u16(0x80093668u + r_u8(state + 0x55u) * 0x44u)) {
                w_u32(state, (flags & 0xFFFFFFBFu) | 0x200020u); w_u32(state + 0xA8u, 0x1DBu);
            }
        }
    }
    value = level_sub_8007BA20(object, state); w_u32(state + 0x1C8u, value);
    animation = (sint16)r_u16(object + 0x11Eu);
    flags = r_u32(state);
    if (animation == 2 || animation == 9 || animation == 10 || animation == 13 || animation == 16 ||
        (animation == 8 && (sint32)r_u32(state + 0x48u) > 0)) flags |= 0x800000u;
    else flags &= 0xFF7FFFFFu;
    w_u32(state, flags);
    if (!r_u32(state + 0x13Cu) && !r_u32(state + 0x124u) && level_sub_80075778(controller, 2u))
        moh_draft_missing_call(0x80080380u, object, state);
    changed = level_sub_8005CBA8(0x11u, object);
    if (changed) {
        selection = moh_draft_missing_call(0x8007A290u, object);
        level_sub_8005C97C(object, 0x11u, 0u); w_u8(state + 0x54u, (uint8)selection);
    }
    index = (uint32)(sint32)(sint8)r_u8(state + 0x54u); kind = r_u8(0x8009E878u + index);
    if ((kind == 3u || kind == 10u) && !r_u16(state + index * 2u + 0x66u) && !r_u16(state + index * 2u + 0x56u)) {
        selection = moh_draft_missing_call(0x8007A290u, object); w_u8(state + 0x54u, (uint8)selection);
        changed = 1u;
    }
    if (changed) level_sub_8007DD4C(object);
    if (level_sub_80075778(controller, 5u)) {
        moh_draft_missing_call(0x80084670u, object);
        if (!r_u32(state + 0x13Cu) && !(r_u16(0x8009366Au + r_u8(0x8009E878u + (uint32)(sint32)(sint8)r_u8(state + 0x54u)) * 0x44u) & 1u) &&
            !moh_draft_missing_call(0x800460F4u, object)) level_sub_8005C97C(object, 0x10u, 1u);
    }
    value = r_u32(state + 0x4Cu) + r_u32(state + 0x50u); w_u32(state + 0x4Cu, value);
    if ((sint32)value > 0xF0000) w_u32(state + 0x4Cu, 0xF0000u);
    value = r_u32(state + 0x4Cu);
    w_u32(state + 0x4Cu, (sint32)value > 0 ? value - 0x7FFFu : ((sint32)value < -0x2000 ? value + 0x1FFFu : 0u));
    value = r_u32(state + 0x50u); w_u32(state + 0x50u, (sint32)value > 0x2000 ? value - 0x2000u : 0u);
    w_u32(state, r_u32(state) & 0xFFFBFFFFu);
    kind = r_u8(state + 0x55u);
    if (kind == 3u || kind == 10u) {
        if ((sint32)r_u32(state + 0x48u) > 0) {
            w_u32(state + 0x48u, r_u32(state + 0x48u) - 1u); w_u32(state + 0x128u, 0u);
        } else if (!r_u32(state + 0x124u)) {
            value = level_sub_8007570C(controller, 0u);
            if (value) {
                if (!r_u32(state + 0x128u)) w_u32(state + 0x11Cu, 0u);
                else if ((sint32)r_u32(state + 0x11Cu) < 30) w_u32(state + 0x11Cu, r_u32(state + 0x11Cu) + 1u);
            } else if (r_u32(state + 0x128u)) w_u32(object + 0x3Cu, r_u32(object + 0x3Cu) | 2u);
            w_u32(state + 0x128u, value);
        }
    } else if ((sint32)r_u32(state + 0x48u) > 0) w_u32(state + 0x48u, r_u32(state + 0x48u) - 1u);
    else {
        value = (sint32)r_u32(state + 0x44u) > 0 ? level_sub_8007570C(controller, 0u) : level_sub_80075778(controller, 0u);
        if (value) w_u32(object + 0x3Cu, r_u32(object + 0x3Cu) | 2u);
    }
    if (!(r_u32(state) & 0x20u) && r_u32(object + 0x284u) && level_sub_80075778(controller, 1u) && !(r_u32(state) & 2u)) {
        level_sub_80075118(object); w_u32(object + 0x21Cu, 0xFFE60000u);
        if (r_u32(state) & 0x10000000u) w_u32(object + 0x21Cu, 0xFFF98000u);
    }
    camera = r_u32(r_u32(0x8009D638u) + 0x388u);
    if (level_sub_80075778(controller, 3u)) {
        if (!(r_u32(state) & 2u)) {
            w_u32(state, r_u32(state) | 2u);
            /* The original branch after OR 2 cannot reach the standing block */
            if (r_u32(0x800939F4u) == 5u && r_u32(0x800939F8u) == 4u) w_u32(state + 0x3Cu, 0xFFFFFF6Au);
            else {
                value = r_u32(0x800AA168u); w_u32(state + 0x3Cu, 0xFFFFFFB0u);
                if ((sint32)value < 300) w_u32(0x800AA168u, 300u);
                if (!r_u32(0x8009E830u)) {
                    if ((sint32)r_u32(0x800AA164u) < 2) w_u32(0x800AA164u, 2u);
                    w_u32(0x8009E830u, 1u);
                }
            }
        } else if (!(r_u32(object + 0x10u) & 0x800u)) level_sub_80079718(object, (sint32)controller);
    }
    current = r_u32(camera + 0x94u); target = r_u32(state + 0x3Cu);
    if (current == target) {
        if ((r_u32(state) & 2u) && level_sub_8006D8AC(object + 0x2Cu)) {
            w_u32(object + 0x2D4u, 0x500000u); w_u16(object + 0x2DAu, 0u);
        }
    } else {
        if ((sint32)(target + 0x12u) < (sint32)current) delta = (uint32)-20;
        else if ((sint32)current < (sint32)(target - 0x12u)) delta = 20u;
        else if ((r_u32(state) & 2u) && (r_u32(object + 0x200u) || r_u32(object + 0x218u) || r_u32(object + 0x238u)))
            delta = r_u32(camera + 0x94u) - r_u32(state + 0x3Cu);
        else delta = r_u32(state + 0x3Cu) - r_u32(camera + 0x94u);
        camera_y = r_u32(camera + 0x94u); camera_other_y = r_u32(camera + 0x98u);
        camera_y += delta; w_u32(camera + 0x94u, camera_y); fixed = camera_y << 16;
        w_u32(camera + 0x98u, camera_other_y + delta); camera_first = r_u32(camera + 0x1Cu);
        w_u32(camera + 0x4Cu, fixed); w_u32(camera + 0x58u, fixed); camera_second = r_u32(camera + 0x28u);
        w_u32(camera + 0x1Cu, camera_first + (delta << 16)); current = r_u32(camera + 0x94u);
        w_u32(camera + 0x28u, camera_second + (delta << 16));
        if (current == r_u32(state + 0x3Cu) && (r_u32(state) & 2u)) moh_draft_missing_call(0x800798ACu, object, controller);
    }
    if (r_u32(state + 0x70u) && (r_u32(state) & 2u)) level_sub_80079718(object, (sint32)controller);
    moh_draft_missing_call(0x8007F300u, object, controller);
    if (!r_u32(state + 0x70u)) w_u32(object + 12u, r_u32(object + 12u) | 0x20u);
    if (!(r_u32(state) & 0x20u) && (r_u32(object + 0x200u) || r_u32(object + 0x218u))) {
        level_sub_8005C97C(object, 2u, 1u); w_u32(object + 0x3Cu, r_u32(object + 0x3Cu) | 0x40u);
    } else {
        level_sub_8005C97C(object, 2u, 0u); w_u32(object + 0x3Cu, r_u32(object + 0x3Cu) | 0x80u);
    }
    index = (uint32)(sint32)(sint8)r_u8(state + 0x54u); kind = r_u8(0x8009E878u + index);
    if (!(r_u16(0x8009366Au + kind * 0x44u) & 1u) && !r_u16(state + index * 2u + 0x66u) &&
        (sint32)r_u32(state + 0x48u) <= 0 && r_u16(state + index * 2u + 0x56u)) moh_draft_missing_call(0x800460F4u, object);
    value = r_u32(state + 0x38u);
    if (value) {
        --value; w_u32(state + 0x38u, value);
        if (value) level_sub_8004C64C(object, 0xFFFFu, 8, 0, 0);
        else level_sub_8004C64C(object, 0xFFFFu, -1, -1, -1);
    } else if ((r_u32(r_u32(object + 0x114u)) & 8u) && !r_u16(r_u32(object + 0x100u) + 0x5Eu)) {
        w_u32(state + 0x38u, 4u); level_sub_8004C64C(object, 0xFFFFu, 8, 0, 0);
    }
    if (r_u16(state + 0x154u)) {
        if (r_u32(state) & 2u) w_u16(state + 0x154u, 0u);
        else {
            w_u32(object + 0x1F0u, r_u32(object + 0x1F0u) - (uint32)((sint32)r_u32(object + 0x218u) >> 2));
            w_u32(object + 0x1F4u, r_u32(object + 0x1F4u) - (uint32)((sint32)r_u32(object + 0x21Cu) >> 2));
            w_u32(object + 0x1F8u, r_u32(object + 0x1F8u) - (uint32)((sint32)r_u32(object + 0x220u) >> 2));
            w_u32(object + 0x1F0u, r_u32(object + 0x1F0u) + r_u32(state + 0x80u));
            w_u32(object + 0x1F4u, r_u32(object + 0x1F4u) + r_u32(state + 0x84u));
            w_u32(object + 0x1F8u, r_u32(object + 0x1F8u) + r_u32(state + 0x88u));
            if (!(r_u16(state + 0x154u) & 1u)) {
                w_u32(object + 0x24Cu, (uint32)((sint32)r_u32(object + 0x24Cu) >> 1));
                w_u32(object + 0x254u, (uint32)((sint32)r_u32(object + 0x254u) >> 1));
                w_u32(object + 0x250u, (uint32)((sint32)r_u32(object + 0x250u) >> 1));
            }
            w_u16(state + 0x154u, (uint16)(r_u16(state + 0x154u) - 1u));
        }
    }
    if (r_u32(state) & 0x200040u) {
        if (r_u32(state + 0xA4u) < r_u32(state + 0xA8u)) {
            if (r_u32(state + 0xACu) < 0x40u) w_u32(state + 0xACu, r_u32(state + 0xACu) + 2u);
            current = r_u32(state + 0xA4u); delta = r_u32(state + 0xACu); target = r_u32(0x800AA168u);
            w_u32(state + 0xA4u, current + delta); w_u32(0x800AA168u, target + delta);
        }
    } else {
        w_u32(state + 0xA4u, 400u); w_u32(0x800AA168u, (r_u32(r_u32(object + 0x388u)) & 2u) ? 300u : 250u);
        w_u32(state, r_u32(state) & 0xFFFFFEFFu);
    }
    if (r_u32(state + 0xA4u) == 400u) w_u32(state + 0xACu, 1u);
finish:
    level_sub_8003FBFC(object);
    if ((sint32)r_u32(object + 0x288u) < 0) {
        w_u32(object + 0x288u, 1u); w_u32(object + 0x28Cu, 1u); w_u32(object + 0x22Cu, r_u32(object + 0x22Cu) + 0x08000000u);
    }
    level_sub_8003F8F0(object); flags = r_u32(state); previous = r_u32(state + 0x70u); w_u32(state + 4u, flags);
    if (previous) { w_u32(state + 0x74u, previous); w_u32(state + 0x70u, 0u); }
    else if (r_u32(state + 0x74u)) { w_u32(object + 0x21Cu, 0u); w_u32(state + 0x74u, 0u); }
    w_u32(state + 0x8Cu, r_u32(object + 0x284u)); w_u32(state, r_u32(state) & 0xFFFFF1FFu);
}
