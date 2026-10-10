#include "game_draft_signatures.h"
#include "game_level_signatures.h"
#include "xport_trace.h"
#include "psx.h"

uint32 level_sub_80078BCC(uint32 plane, uint32 position, uint32 height, uint32 radius)
{
    uint32 x, y, z, nx, ny, nz, offset, result;
    FUNCTION_MARKER(0x80078BCCu, "LEVEL.BIN");
    x = r_u32(position); nx = (uint32)(sint32)(sint16)r_u16(plane);
    result = nx * (uint32)((sint32)x >> 15);
    z = r_u32(position + 8u); nz = (uint32)(sint32)(sint16)r_u16(plane + 4u);
    result += nz * (uint32)((sint32)z >> 15);
    ny = (uint32)(sint32)(sint16)r_u16(plane + 2u);
    y = (uint32)((sint32)r_u32(position + 4u) >> 15);
    if ((sint32)ny >= 0) y -= height << 1;
    ny = (uint32)(sint32)(sint16)r_u16(plane + 2u);
    offset = (uint32)(sint32)(sint16)r_u16(plane + 6u) << 16;
    return result + ny * y + offset - (radius << 16);
}

uint32 level_sub_80073960(uint32 object, uint32 group)
{
    uint32 record, count, index, value, velocity, magnitude, limit, acceleration, x, y, z, vx, vy, vz, first, second, third;
    FUNCTION_MARKER(0x80073960u, "LEVEL.BIN");
    (void)object;
    record = r_u32(group + 40u); count = r_u32(group + 4u); w_u32(group, 2u);
    if ((sint32)count <= 0) return 1u;
    index = 0u;
    do {
        if (!(r_u8(record) & 2u)) {
            value = r_u8(record + 2u);
            if (value) w_u8(record + 2u, value - 1u); else w_u8(record, 2u);
            value = r_u8(record + 1u);
            if (value) w_u8(record + 1u, value - 1u);
            else {
                velocity = r_u32(record + 20u); limit = r_u32(group + 20u);
                magnitude = (sint32)velocity < 0 ? 0u - velocity : velocity;
                if ((sint32)limit < 0) limit = 0u - limit;
                if ((sint32)magnitude < (sint32)limit) {
                    acceleration = r_u32(group + 16u);
                    if (acceleration) w_u32(record + 20u, velocity + acceleration);
                }
                x = r_u32(record + 4u); vx = r_u32(record + 16u); vy = r_u32(record + 20u); vz = r_u32(record + 24u);
                w_u32(record + 4u, x + vx); y = r_u32(record + 8u); z = r_u32(record + 12u);
                w_u32(record + 12u, z + vz); w_u32(record + 8u, y + vy);
                limit = r_u32(group + 12u); magnitude = (sint32)vz < 0 ? 0u - vz : vz;
                if ((sint32)limit < (sint32)magnitude) w_u32(record + 24u, (sint32)vz > 0 ? vz - limit : vz + limit);
                else w_u32(record + 24u, 0u);
                vx = r_u32(record + 16u); limit = r_u32(group + 12u); magnitude = (sint32)vx < 0 ? 0u - vx : vx;
                if ((sint32)limit < (sint32)magnitude) w_u32(record + 16u, (sint32)vx > 0 ? vx - limit : vx + limit);
                else w_u32(record + 16u, 0u);
                value = r_u16(record + 28u); first = r_u16(record + 36u); second = r_u16(record + 38u); third = r_u16(record + 40u);
                w_u16(record + 28u, value + first); value = r_u16(record + 30u); first = r_u16(record + 32u);
                w_u16(record + 30u, value + second); w_u16(record + 32u, first + third);
            }
            w_u32(group, 1u);
        }
        ++index; record += 52u;
    } while ((sint32)index < (sint32)r_u32(group + 4u));
    return 1u;
}

void level_sub_80073B08(uint32 object)
{
    uint32 state;
    FUNCTION_MARKER(0x80073B08u, "LEVEL.BIN");
    state = r_u32(object + 0x388u);
    if (r_u32(state + 0xCu) == 2u) w_u32(state, 1u);
    else moh_draft_missing_call(0x80073960u, object, state + 0xCu);
    if (r_u32(state + 0x38u) == 2u) w_u32(state + 4u, 1u);
    else moh_draft_missing_call(0x80073960u, object, state + 0x38u);
    if (r_u32(state + 4u) && r_u32(state)) moh_draft_missing_call(0x8006B8F0u, object);
}

void level_sub_800701DC(uint32 object)
{
    uint32 state, lifetime, index, record, first, second;
    FUNCTION_MARKER(0x800701DCu, "LEVEL.BIN");
    state = r_u32(object + 0x388u); lifetime = r_u8(state + 1u);
    if (!lifetime) { moh_draft_missing_call(0x8006B8F0u, object); return; }
    lifetime = (lifetime - 1u) & 255u; w_u8(state + 1u, lifetime);
    if (lifetime >= 3u || !r_u8(state)) return;
    index = 0u; record = state + 16u;
    do {
        first = r_u8(record + 28u); second = r_u8(record + 29u); w_u8(record + 28u, first >> 1);
        first = r_u8(record + 30u); w_u8(record + 29u, second >> 1);
        second = r_u8(record + 32u); w_u8(record + 30u, first >> 1);
        first = r_u8(record + 33u); w_u8(record + 32u, second >> 1);
        second = r_u8(record + 34u); w_u8(record + 33u, first >> 1); w_u8(record + 34u, second >> 1);
        ++index; record += 36u;
    } while (index < r_u8(state));
}

void level_sub_8008D9D8(uint32 position, uint32 rotation, uint32 scale)
{
    uint32 local, x, y, z, tx, ty, tz, axis;
    FUNCTION_MARKER(0x8008D9D8u, "LEVEL.BIN");
    local = moh_draft_missing_call(0x80016718u, 44u);
    level_sub_8008D918(position, rotation, scale, local);
    moh_draft_missing_call(0x800163E8u, 0x8009C804u, local + 20u, local + 32u);
    moh_draft_missing_call(0x8001BD08u, 0x8009C804u, local);
    x = r_u32(local + 32u); tx = r_u32(0x8009C818u); w_u32(local + 20u, x + tx);
    y = r_u32(local + 36u); ty = r_u32(0x8009C81Cu); z = r_u32(local + 40u); tz = r_u32(0x8009C820u);
    w_u32(local + 24u, y + ty); w_u32(local + 28u, z + tz);
    for (axis = 0u; axis < 8u; ++axis) xport_gte_write_control(axis, r_u32(local + axis * 4u));
    moh_draft_missing_call(0x80036E44u, local);
}

void level_sub_800732F8(uint32 object, uint32 render)
{
    uint32 state, ordering_table, record, local, packet, end, index, axis, mode, texture_x, texture_y, width, height, clut_x, clut_y, abr;
    uint32 scale, sxy[4], depth, link, slot, value, vertex;
    sint32 half_width, half_height;
    PsxGteSnapshot snapshot;
    FUNCTION_MARKER(0x800732F8u, "LEVEL.BIN");
    state = r_u32(object + 0x388u); ordering_table = r_u32(render + 0x2218u); record = r_u32(state + 0x34u);
    local = moh_draft_missing_call(0x80016718u, 96u);
    packet = moh_draft_missing_call(0x80053BD4u, r_u32(render + 0x221Cu), local); end = r_u32(local) - 40u;
    if (packet > end) { moh_draft_missing_call(0x80036E44u, local); return; }
    /* TODO: Original unused vector and texture padding remains undefined */
    if (!r_u32(state)) {
        mode = r_u16(state + 0x32u); texture_x = r_u16(state + 0x24u); clut_x = r_u16(state + 0x28u);
        texture_y = r_u16(state + 0x26u); clut_y = r_u16(state + 0x2Au); width = r_u16(state + 0x2Cu); height = r_u16(state + 0x2Eu);
        abr = ((mode ? mode - 1u : 1u) & 3u) << 5;
        moh_draft_missing_call(0x800158ACu, local + 52u, 0u, 12u);
        scale = r_u32(state + 0x14u); w_u32(local + 52u, scale);
        scale = r_u32(state + 0x14u); w_u32(local + 56u, scale);
        scale = r_u32(state + 0x14u); w_u32(local + 60u, scale);
        for (index = 0u; (sint32)index < (sint32)r_u32(state + 0x10u); ++index, record += 52u) {
            if (!(r_u8(record) & 1u) || r_u8(record + 1u)) continue;
            half_width = (sint16)r_u16(record + 48u); half_height = (sint16)r_u16(record + 50u);
            w_u16(local + 8u, (uint32)(0 - half_width)); w_u16(local + 10u, (uint32)(0 - half_height)); w_u16(local + 12u, 0u);
            w_u16(local + 16u, (uint32)half_width); w_u16(local + 18u, (uint32)(0 - half_height)); w_u16(local + 20u, 0u);
            w_u16(local + 24u, (uint32)(0 - half_width)); w_u16(local + 26u, (uint32)half_height); w_u16(local + 28u, 0u);
            w_u16(local + 32u, (uint32)half_width); w_u16(local + 34u, (uint32)half_height); w_u16(local + 36u, 0u);
            for (axis = 0u; axis < 3u; ++axis) w_u32(local + 40u + axis * 4u, (uint32)(sint32)(sint16)r_u16(record + 28u + axis * 2u) << 16);
            moh_draft_missing_call(0x8008D9D8u, record + 4u, local + 40u, local + 52u);
            for (axis = 0u; axis < 3u; ++axis) {
                vertex = local + 8u + axis * 8u;
                xport_gte_write_data(axis * 2u, r_u32(vertex)); xport_gte_write_data(axis * 2u + 1u, r_u16(vertex + 4u));
            }
            xport_gte_execute(0x280030u); moh_draft_missing_call(0x8001B9F8u, packet);
            w_u8(packet + 13u, texture_y); w_u8(packet + 12u, (texture_x & 63u) << 2);
            w_u8(packet + 21u, texture_y); w_u8(packet + 28u, (texture_x & 63u) << 2);
            w_u8(packet + 20u, ((texture_x & 63u) << 2) + (width & 255u));
            w_u8(packet + 36u, ((texture_x & 63u) << 2) + (width & 255u));
            w_u8(packet + 29u, height + (texture_y & 255u)); w_u8(packet + 37u, height + (texture_y & 255u));
            psx_gte_snapshot(&snapshot); w_u32(0x1F800190u, (uint32)snapshot.flag);
            if ((sint32)snapshot.flag < 0) continue;
            for (axis = 0u; axis < 3u; ++axis) { sxy[axis] = xport_gte_read_data(12u + axis); w_u32(0x1F8001BCu + axis * 4u, sxy[axis]); }
            depth = xport_gte_read_data(19u); w_u32(0x1F800198u, depth);
            xport_gte_write_data(0u, r_u32(local + 32u)); xport_gte_write_data(1u, r_u16(local + 36u)); xport_gte_execute(0x180001u);
            value = moh_draft_missing_call(0x8001BA18u, clut_x, clut_y); w_u16(packet + 14u, value);
            if (mode) moh_draft_missing_call(0x8001BA38u, packet, 1u);
            depth = r_u32(0x1F800198u);
            if ((sint32)depth < 0 || ((sint32)depth >> 4) >= 512) continue;
            w_u16(packet + 22u, abr | ((texture_y & 0x100u) >> 4) | ((texture_x & 0x3C0u) >> 6) | ((texture_y & 0x200u) << 2));
            sxy[3] = xport_gte_read_data(14u); w_u32(0x1F8001C8u, sxy[3]);
            if (r_u8(record + 3u)) sxy[3] = sxy[2];
            for (axis = 0u; axis < 4u; ++axis) { w_u16(packet + 8u + axis * 8u, sxy[axis]); w_u16(packet + 10u + axis * 8u, sxy[axis] >> 16); }
            for (axis = 0u; axis < 3u; ++axis) w_u8(packet + 4u + axis, r_u8(record + 44u + axis));
            depth = r_u32(0x1F800198u) - 160u; w_u32(0x1F800198u, depth);
            if ((sint32)depth < 0) { depth = 0u; w_u32(0x1F800198u, 0u); }
            slot = ordering_table + ((depth >> 4) * 4u);
            w_u32(packet, (r_u32(packet) & 0xFF000000u) | (r_u32(slot) & 0xFFFFFFu)); link = packet & 0xFFFFFFu;
            slot = ordering_table + ((uint32)((sint32)r_u32(0x1F800198u) >> 4) * 4u); packet += 40u;
            w_u32(slot, (r_u32(slot) & 0xFF000000u) | link);
            if (end < packet) goto finish;
        }
    }
    if (packet <= end) {
        record = r_u32(state + 0x60u);
        if (!r_u32(state + 4u)) {
            w_u16(local + 52u, 0u); w_u16(local + 58u, r_u16(state + 0x50u));
            w_u16(local + 60u, r_u16(state + 0x52u)); w_u16(local + 62u, r_u16(state + 0x5Cu));
            w_u16(local + 64u, r_u16(state + 0x54u)); w_u16(local + 66u, r_u16(state + 0x56u));
            w_u16(local + 68u, r_u16(state + 0x58u)); w_u16(local + 70u, r_u16(state + 0x5Au));
            value = r_u8(state + 0x5Eu); w_u8(local + 73u, 1u); w_u8(local + 74u, 0u); w_u8(local + 75u, 0u); w_u8(local + 72u, value);
            for (index = 0u; (sint32)index < (sint32)r_u32(state + 0x3Cu); ++index, record += 52u) {
                if (!(r_u8(record) & 1u) || r_u8(record + 1u)) continue;
                value = r_u8(record + 2u) << 1; w_u8(local + 90u, value); w_u8(local + 89u, value); w_u8(local + 88u, value);
                w_u16(local + 54u, r_u16(record + 48u)); w_u16(local + 56u, r_u16(record + 50u));
                value = moh_draft_missing_call(0x8006E058u, record + 4u, (uint32)((sint32)r_u32(state + 0x40u) >> 4), (uint32)(sint32)(sint16)r_u16(record + 32u), local + 88u, local + 52u, render, packet, 4u, 0u, 0u, 1u);
                if (value) { packet += 40u; if (end < packet) break; }
            }
        }
    }
finish:
    moh_draft_missing_call(0x80053BF4u, packet); moh_draft_missing_call(0x80036E44u, local);
}

void level_sub_8006F7BC(uint32 object, uint32 render)
{
    uint32 state, ordering_table, local, packet, end, texture_x, texture_y, width, height, clut_x, clut_y, mode, abr;
    uint32 index, axis, record, first_xy, first_z, second_xy, second_z, flag, sxy[3], depth, link, slot;
    sint32 x, y, end_x, end_y, dx, dy;
    PsxGteSnapshot snapshot;
    FUNCTION_MARKER(0x8006F7BCu, "LEVEL.BIN");
    state = r_u32(object + 0x388u); ordering_table = r_u32(render + 0x2218u);
    local = moh_draft_missing_call(0x80016718u, 4u);
    packet = moh_draft_missing_call(0x80053BD4u, r_u32(render + 0x221Cu), local);
    end = r_u32(local) - 40u;
    if (packet > end) { moh_draft_missing_call(0x80036E44u, local); return; }
    texture_x = r_u16(state + 6u); texture_y = r_u16(state + 8u);
    mode = r_u16(state + 14u); width = r_u16(state + 2u); height = r_u16(state + 4u);
    clut_x = r_u16(state + 10u); clut_y = r_u16(state + 12u);
    abr = ((mode ? mode - 1u : 1u) & 3u) << 5;
    for (axis = 0u; axis < 8u; ++axis) xport_gte_write_control(axis, r_u32(render + 0x21E4u + axis * 4u));
    for (index = 0u; index < r_u8(state); ++index) {
        record = state + 16u + index * 36u;
        first_xy = r_u16(record + 6u) | (r_u16(record + 10u) << 16); first_z = r_u16(record + 14u);
        second_xy = r_u16(record + 18u) | (r_u16(record + 22u) << 16); second_z = r_u16(record + 26u);
        /* TODO: Original vector padding is undefined and does not enter the GTE Z component */
        xport_gte_write_data(0u, first_xy); xport_gte_write_data(1u, first_z);
        xport_gte_write_data(2u, first_xy); xport_gte_write_data(3u, first_z);
        xport_gte_write_data(4u, second_xy); xport_gte_write_data(5u, second_z); xport_gte_execute(0x280030u);
        w_u8(packet + 3u, 9u); w_u8(packet + 7u, 0x34u);
        w_u8(packet + 24u, width + ((texture_x & 63u) << 2) - 1u);
        w_u8(packet + 13u, (texture_y & 255u) + height - 1u);
        w_u8(packet + 25u, (texture_y & 255u) + height - 1u);
        w_u8(packet + 36u, (width >> 1) + ((texture_x & 63u) << 2));
        w_u8(packet + 37u, texture_y); w_u8(packet + 12u, (texture_x & 63u) << 2);
        w_u16(packet + 26u, abr | ((texture_y & 0x100u) >> 4) | ((texture_x & 0x3C0u) >> 6) | ((texture_y & 0x200u) << 2));
        psx_gte_snapshot(&snapshot); flag = (uint32)snapshot.flag;
        if ((sint32)flag < 0) continue;
        for (axis = 0u; axis < 3u; ++axis) { sxy[axis] = xport_gte_read_data(12u + axis); w_u32(0x1F8001BCu + axis * 4u, sxy[axis]); }
        depth = xport_gte_read_data(19u); w_u32(0x1F800198u, depth);
        depth -= 150u; w_u32(0x1F800198u, depth);
        if ((sint32)depth < 0 || ((sint32)depth >> 4) >= 512) continue;
        x = (sint16)sxy[0]; y = (sint16)(sxy[0] >> 16);
        end_x = (sint16)sxy[2]; end_y = (sint16)(sxy[2] >> 16);
        dx = (x - end_x) >> 4; dy = (y - end_y) >> 4;
        w_u16(0x1F8001BCu, (uint32)(x + dy)); w_u16(0x1F8001BEu, (uint32)(y - dx));
        w_u16(0x1F8001C0u, (uint32)(x - dy)); w_u16(0x1F8001C2u, (uint32)(y + dx));
        w_u32(packet + 32u, sxy[2]); w_u32(packet + 8u, r_u32(0x1F8001BCu)); w_u32(packet + 20u, r_u32(0x1F8001C0u));
        for (axis = 0u; axis < 3u; ++axis) w_u8(packet + 4u + axis, r_u8(record + 28u + axis));
        for (axis = 0u; axis < 3u; ++axis) w_u8(packet + 16u + axis, r_u8(record + 28u + axis));
        for (axis = 0u; axis < 3u; ++axis) w_u8(packet + 28u + axis, r_u8(record + 32u + axis));
        link = packet & 0xFFFFFFu;
        if (mode) w_u8(packet + 7u, r_u8(packet + 7u) | 2u);
        w_u16(packet + 14u, (clut_y << 6) | ((clut_x >> 4) & 63u));
        slot = ordering_table + ((uint32)((sint32)r_u32(0x1F800198u) >> 4) * 4u);
        w_u32(packet, (r_u32(packet) & 0xFF000000u) | (r_u32(slot) & 0xFFFFFFu));
        packet += 40u;
        slot = ordering_table + ((uint32)((sint32)r_u32(0x1F800198u) >> 4) * 4u);
        w_u32(slot, (r_u32(slot) & 0xFF000000u) | link);
        if (end < packet) break;
    }
    moh_draft_missing_call(0x80053BF4u, packet);
    moh_draft_missing_call(0x80036E44u, local);
}

void level_sub_80044BE4(uint32 object)
{
    uint32 state, handle, owner = 0u, check = 0u, local, value;
    FUNCTION_MARKER(0x80044BE4u, "LEVEL.BIN");
    state = r_u32(object + 0x388u);
    if (r_u32(state + 0x2Cu)) return;
    handle = r_u32(object + 0x30u);
    if (handle) owner = r_u32(0x8009D678u + (handle & 0xFFFFu) * 4u);
    handle = r_u32(object + 0x30u);
    if (handle) check = r_u32(0x8009D678u + (handle & 0xFFFFu) * 4u);
    if (check) {
        local = moh_draft_missing_call(0x80016718u, 4u); w_u32(local, object);
        value = moh_draft_missing_call(0x8006D7D0u, owner + 0x30u, local);
        if (value) moh_draft_missing_call(0x8006D84Cu, owner + 0x30u, 0u);
        object = r_u32(local); moh_draft_missing_call(0x80036E44u, local);
    }
    value = r_u32(state + 0x30u); w_u32(state + 0x2Cu, 1u);
    if ((sint32)value > 0) { moh_draft_missing_call(0x800425C4u, value); w_u32(state + 0x30u, 0u); }
    if (r_u16(state + 0x38u) & 0x80u) moh_draft_missing_call(0x8006A318u, object);
    else moh_draft_missing_call(0x8006A37Cu, object);
    if (r_u32(0x8009D650u) == object) w_u32(0x8009D650u, 0u);
}

uint32 level_sub_800702E0(uint32 x, uint32 y, uint32 z, uint32 direction, uint32 red, uint32 green, uint32 blue, uint32 unused_red, uint32 unused_green, uint32 unused_blue, uint32 count, uint32 lifetime, uint32 unused)
{
    uint32 object, state, record, local, index, axis, random, scale;
    SVECTOR input;
    VECTOR output;
    FUNCTION_MARKER(0x800702E0u, "LEVEL.BIN");
    (void)unused_red; (void)unused_green; (void)unused_blue; (void)unused;
    object = moh_draft_missing_call(0x8006B5B8u, 0u, 0x130u, 0xFFFFFFFFu, 0u, 0u, 0u, 0u, 0u);
    if (!object) return 0u;
    state = r_u32(object + 0x388u);
    w_u8(state + 1u, lifetime); w_u8(state, count);
    w_u32(object + 8u, 0x23u); w_u32(object + 0x10u, 0x203u);
    w_u32(object + 0x36Cu, 0x800701DCu); w_u32(object + 0x370u, 0x8006F7BCu);
    w_u32(object + 0x14u, 0u); w_u32(object + 0xCu, 0u); w_u32(object + 0x374u, 0u); w_u32(object + 0x378u, 0u); w_u32(object + 0x384u, 0x8006A318u);
    w_u32(object + 0x20Cu, x); w_u32(object + 0x210u, y);
    w_u32(object + 0x270u, 0x10000u); w_u32(object + 0x274u, 0x10000u); w_u32(object + 0x278u, 0x10000u);
    w_u32(object + 0x228u, 0u); w_u32(object + 0x22Cu, 0u); w_u32(object + 0x230u, 0u); w_u32(object + 0x284u, 0u); w_u32(object + 0x214u, z);
    w_u16(state + 6u, 0x3FFu); w_u16(state + 2u, 1u); w_u16(state + 4u, 1u);
    w_u16(state + 10u, 0x3E0u); w_u16(state + 8u, 0u); w_u16(state + 12u, 0xFDu); w_u16(state + 14u, 4u);
    local = moh_draft_missing_call(0x80016718u, 40u);
    for (index = 0u; (sint32)index < (sint32)count; ++index) {
        record = state + 16u + index * 36u;
        random = moh_draft_missing_call(0x8001552Cu);
        w_u32(record + 4u, x + (((random & 15u) - 8u) << 16));
        w_u32(record + 8u, y + ((((random >> 4) & 15u) - 8u) << 16));
        w_u32(record + 12u, z + ((((random >> 8) & 15u) - 8u) << 16));
        random = moh_draft_missing_call(0x8001552Cu); scale = (random & 0x4Bu) + 16u;
        random = moh_draft_missing_call(0x8001552Cu);
        for (axis = 0u; axis < 3u; ++axis) w_u16(local + axis * 2u, (((random >> (axis * 5u)) & 31u) - 16u) << 5);
        /* TODO: Original vector and matrix padding remains undefined */
        moh_draft_missing_call(0x8001676Cu, local, local + 8u);
        moh_draft_missing_call(0x8001DAE0u, local + 8u, direction, local);
        input.vx = (sint16)r_u16(local); input.vy = (sint16)r_u16(local + 2u); input.vz = (sint16)r_u16(local + 4u); input.pad = 0;
        xport_gte_write_data(8u, scale);
        xport_gte_write_data(9u, r_u16(local)); xport_gte_write_data(10u, r_u16(local + 2u)); xport_gte_write_data(11u, r_u16(local + 4u));
        gte_gpf12(&input, (sint32)scale, &output);
        for (axis = 0u; axis < 3u; ++axis) w_u16(local + axis * 2u, xport_gte_read_data(9u + axis));
        w_u32(record + 16u, r_u32(record + 4u) + ((uint32)(sint32)(sint16)r_u16(local) << 16));
        w_u32(record + 20u, r_u32(record + 8u) + ((uint32)(sint32)(sint16)r_u16(local + 2u) << 16));
        w_u8(record + 28u, red); w_u8(record + 30u, blue); w_u8(record + 29u, green);
        w_u8(record + 32u, red); w_u8(record + 34u, blue);
        w_u32(record + 24u, r_u32(record + 12u) + ((uint32)(sint32)(sint16)r_u16(local + 4u) << 16));
        w_u8(record + 33u, green);
    }
    moh_draft_missing_call(0x80036E44u, local);
    return object;
}

uint32 level_sub_800705EC(uint32 position, uint32 normal, uint32 strength, uint32 start_color, uint32 end_color)
{
    uint32 local, object, i, color[6];
    SVECTOR input;
    VECTOR output;
    FUNCTION_MARKER(0x800705ECu, "LEVEL.BIN");
    local = moh_draft_missing_call(0x80016718u, 8u);
    moh_draft_missing_call(0x80016A40u, normal, local);
    input.vx = (sint16)r_u16(local); input.vy = (sint16)r_u16(local + 2u); input.vz = (sint16)r_u16(local + 4u); input.pad = 0;
    xport_gte_write_data(8u, strength);
    xport_gte_write_data(9u, r_u16(local)); xport_gte_write_data(10u, r_u16(local + 2u)); xport_gte_write_data(11u, r_u16(local + 4u));
    gte_gpf12(&input, (sint32)strength, &output);
    w_u16(local, xport_gte_read_data(9u)); w_u16(local + 2u, xport_gte_read_data(10u)); w_u16(local + 4u, xport_gte_read_data(11u));
    for (i = 0u; i < 3u; ++i) color[i] = (uint32)(sint32)(sint8)r_u8(start_color + i);
    for (i = 0u; i < 3u; ++i) color[i + 3u] = (uint32)(sint32)(sint8)r_u8(end_color + i);
    object = moh_draft_missing_call(0x800702E0u, r_u32(position), r_u32(position + 4u), r_u32(position + 8u), local, color[0], color[1], color[2], color[3], color[4], color[5], 8u, 4u, 0u);
    if (object) moh_draft_missing_call(0x8006B834u, object);
    moh_draft_missing_call(0x80036E44u, local);
    return object;
}

void level_sub_80043140(uint32 object)
{
    uint32 colors, random;
    FUNCTION_MARKER(0x80043140u, "LEVEL.BIN");
    colors = moh_draft_missing_call(0x80016718u, 16u);
    w_u8(colors, 0xF0u); w_u8(colors + 1u, 0x48u); w_u8(colors + 8u, 0x40u);
    w_u8(colors + 2u, 0u); w_u8(colors + 9u, 0x10u); w_u8(colors + 10u, 0u);
    /* TODO: Original leaves color padding undefined */
    random = moh_draft_missing_call(0x8001552Cu);
    moh_draft_missing_call(0x80041CE8u, 0u, (uint32)((sint32)(random * 7u) >> 15), object, 0x30000u, 0u, 0x10000u, 0x7FFFu, 0u, 0u, 3u, 0u);
    moh_draft_missing_call(0x800705ECu, object + 0x20Cu, object + 0x364u, 0xC00u, colors, colors + 8u);
    moh_draft_missing_call(0x80036E44u, colors);
}

uint32 level_sub_80073BA8(uint32 position, uint32 spread, uint32 count, uint32 velocity_base, uint32 velocity_range, uint32 color, uint32 color_range, uint32 texture_x, uint32 texture_y, uint32 clut_x, uint32 clut_y, uint32 texture_mode, uint32 scale, uint32 smoke_enabled, uint32 smoke_count)
{
    uint32 object, state, cursor, size, value, random, delta, base, index, axis;
    uint32 xyz[3], spread2 = spread << 1, velocity2 = velocity_range << 1;
    FUNCTION_MARKER(0x80073BA8u, "LEVEL.BIN");
    size = count * 52u + 100u;
    if (smoke_enabled) size += smoke_count * 52u;
    object = moh_draft_missing_call(0x8006B5B8u, 0u, size, 0xFFFFFFFFu, 0u, 0u, 0u, 0u, 0u);
    if (!object) return 0u;
    w_u32(object + 8u, 0x31u); w_u32(object + 0x294u, 180u); w_u32(object + 0x290u, 180u);
    w_u32(object + 0x36Cu, 0x80073B08u); w_u32(object + 0x370u, 0x800732F8u);
    state = r_u32(object + 0x388u);
    w_u32(object + 0x38u, 0u); w_u32(object + 0xCu, 0x01028000u);
    w_u32(object + 0x374u, 0u); w_u32(object + 0x378u, 0u); w_u32(object + 0x384u, 0x8006A318u);
    value = r_u32(object + 0x10u); w_u32(object + 0x284u, 0u); w_u32(object + 0x10u, value | 0x203u);
    cursor = state + 100u;
    for (axis = 0u; axis < 3u; ++axis) xyz[axis] = r_u32(position + axis * 4u);
    for (axis = 0u; axis < 3u; ++axis) w_u32(object + 0x20Cu + axis * 4u, xyz[axis]);
    for (axis = 0u; axis < 3u; ++axis) xyz[axis] = r_u32(object + 0x20Cu + axis * 4u);
    for (axis = 0u; axis < 3u; ++axis) w_u32(object + 0x258u + axis * 4u, xyz[axis]);
    w_u32(state, 0u); w_u32(state + 4u, 0u); w_u32(state + 0x34u, cursor);
    w_u32(state + 0x10u, count); w_u32(state + 0xCu, 1u);
    w_u32(state + 0x18u, 0x20000u); w_u32(state + 0x1Cu, 0x20000u); w_u32(state + 0x20u, 0x4B0000u); w_u32(state + 0x14u, scale);
    w_u16(state + 0x24u, texture_x); w_u16(state + 0x26u, texture_y);
    w_u16(state + 0x28u, clut_x); w_u16(state + 0x2Au, clut_y);
    w_u16(state + 0x2Cu, texture_mode); w_u16(state + 0x2Eu, texture_mode);
    w_u16(state + 0x30u, 8u); w_u16(state + 0x32u, 0u);
    for (index = 0u; (sint32)index < (sint32)count; ++index, cursor += 52u) {
        w_u8(cursor, 1u); w_u8(cursor + 1u, 0u);
        random = moh_draft_missing_call(0x8001552Cu);
        w_u8(cursor + 2u, (uint32)((sint32)(random * 10u) >> 15) + 25u);
        for (axis = 0u; axis < 3u; ++axis) {
            base = r_u32(object + 0x20Cu + axis * 4u); delta = (0u - spread) << 16;
            if (spread2) { random = moh_draft_missing_call(0x8001552Cu); delta = ((0u - spread) + (uint32)((sint32)(random * spread2) >> 15)) << 16; }
            w_u32(cursor + 4u + axis * 4u, base + delta);
        }
        for (axis = 0u; axis < 3u; ++axis) {
            delta = (0u - velocity_base) << 16;
            if (velocity2) { random = moh_draft_missing_call(0x8001552Cu); delta = ((0u - velocity_base) + (uint32)((sint32)(random * velocity2) >> 15)) << 16; }
            w_u32(cursor + 0x10u + axis * 4u, delta);
        }
        for (axis = 0u; axis < 3u; ++axis) { random = moh_draft_missing_call(0x8001552Cu); w_u16(cursor + 0x1Cu + axis * 2u, random >> 3); }
        w_u16(cursor + 0x24u, 150u); w_u16(cursor + 0x26u, 100u); w_u16(cursor + 0x28u, 200u);
        value = color & 0xFFu;
        if (color_range & 0xFFu) { random = moh_draft_missing_call(0x8001552Cu); value += (uint32)((sint32)(random * (color_range & 0xFFu)) >> 15); }
        w_u8(cursor + 0x2Eu, value); w_u8(cursor + 0x2Du, value); w_u8(cursor + 0x2Cu, value);
        random = moh_draft_missing_call(0x8001552Cu); w_u8(cursor + 3u, ((sint32)(random * 100u) >> 15) >= 51);
        random = moh_draft_missing_call(0x8001552Cu); w_u16(cursor + 0x30u, (uint32)((sint32)(random * 7u) >> 15) + 5u);
        random = moh_draft_missing_call(0x8001552Cu); w_u16(cursor + 0x32u, (uint32)((sint32)(random * 7u) >> 15) + 5u);
    }
    if (!smoke_enabled) {
        w_u32(state + 0x3Cu, 0u); w_u32(state + 0x60u, 0u); w_u32(state + 0x38u, 2u);
    } else {
        w_u32(state + 0x3Cu, smoke_count); w_u32(state + 0x60u, cursor); w_u32(state + 0x38u, 1u);
        w_u32(state + 0x40u, scale); w_u32(state + 0x48u, 0xFFFFFFFCu); w_u32(state + 0x4Cu, 0x960000u);
        w_u16(state + 0x50u, 0x1000u); w_u16(state + 0x54u, 0x3E0u); w_u16(state + 0x56u, 0xFAu);
        w_u16(state + 0x58u, 0x1Fu); w_u16(state + 0x5Au, 0x1Fu);
        w_u32(state + 0x44u, 2u); w_u16(state + 0x52u, 0u); w_u16(state + 0x5Cu, 8u); w_u16(state + 0x5Eu, 2u);
        for (index = 0u; (sint32)index < (sint32)smoke_count; ++index, cursor += 52u) {
            w_u8(cursor, 1u); w_u8(cursor + 1u, index);
            random = moh_draft_missing_call(0x8001552Cu); w_u8(cursor + 2u, (uint32)((sint32)(random << 1) >> 15) + 20u);
            w_u32(cursor + 4u, r_u32(object + 0x20Cu)); w_u32(cursor + 8u, r_u32(object + 0x210u));
            value = r_u32(object + 0x214u); w_u32(cursor + 0x10u, 0u); w_u32(cursor + 0xCu, value);
            random = moh_draft_missing_call(0x8001552Cu); w_u32(cursor + 0x14u, ((uint32)((sint32)(0u - (random << 1)) >> 15) - 2u) << 16);
            w_u16(cursor + 0x30u, 5u); w_u32(cursor + 0x18u, 0u);
            w_u16(cursor + 0x1Cu, 0u); w_u16(cursor + 0x1Eu, 0u); w_u16(cursor + 0x20u, 0u);
            w_u16(cursor + 0x24u, 0u); w_u16(cursor + 0x26u, 0u); w_u16(cursor + 0x28u, 0u); w_u16(cursor + 0x32u, 6u);
            /* TODO: Original leaves smoke color, mode and record padding uninitialized */
        }
    }
    moh_draft_missing_call(0x8006B834u, object);
    return object;
}

void level_sub_8004DE80(uint32 width, uint32 height, uint32 position)
{
    sint32 a, b, c, minimum_x, maximum_x, minimum_z, maximum_z, range_x, range_z;
    uint32 upper_x, lower_x, upper_z, lower_z, size;
    FUNCTION_MARKER(0x8004DE80u, "LEVEL.BIN");
    a = (sint16)r_u16(position + 24u); b = (sint16)r_u16(position + 16u); c = (sint16)r_u16(position + 32u);
    minimum_x = a < b ? a : b; if (c < minimum_x) minimum_x = c;
    maximum_x = a > b ? a : b; if (c > maximum_x) maximum_x = c;
    a = (sint16)r_u16(position + 28u); b = (sint16)r_u16(position + 20u); c = (sint16)r_u16(position + 36u);
    minimum_z = a < b ? a : b; if (c < minimum_z) minimum_z = c;
    maximum_z = a > b ? a : b; if (c > maximum_z) maximum_z = c;
    range_x = maximum_x - minimum_x; if (range_x < 0) range_x = -range_x;
    range_z = maximum_z - minimum_z; if (range_z < 0) range_z = -range_z;
    upper_z = (uint32)(sint32)(sint16)r_u16(position + 4u) + height;
    upper_x = (uint32)(sint32)(sint16)r_u16(position) + width;
    lower_x = (uint32)(sint32)(sint16)r_u16(position) - width;
    lower_z = (uint32)(sint32)(sint16)r_u16(position + 4u) - height;
    if (range_x < (sint32)(width << 1)) width = (uint32)(range_x >> 1);
    if (range_z < (sint32)(height << 1)) height = (uint32)(range_z >> 1);
    size = (sint32)height < (sint32)width ? height : width;
    if (maximum_z < (sint32)upper_z) {
        if ((sint32)lower_z < minimum_z) w_u16(position + 4u, (uint32)minimum_z + size);
        else w_u16(position + 4u, r_u16(position + 4u) - (upper_z - (uint32)maximum_z));
    } else if ((sint32)lower_z < minimum_z)
        w_u16(position + 4u, r_u16(position + 4u) - (lower_z - (uint32)minimum_z));
    if (maximum_x < (sint32)upper_x) {
        if ((sint32)lower_x < minimum_x) w_u16(position, (uint32)minimum_x + size);
        else w_u16(position, r_u16(position) - (upper_x - (uint32)maximum_x));
    } else if ((sint32)lower_x < minimum_x)
        w_u16(position, r_u16(position) - (lower_x - (uint32)minimum_x));
}

static void level_impact_clamp(uint32 width, uint32 height, uint32 position, uint32 horizontal, uint32 vertical)
{
    sint32 minimum[2], maximum[2], range[2];
    uint32 offsets[2], upper[2], lower[2], dimensions[2], size, axis, vertex;
    offsets[0] = horizontal; offsets[1] = vertical;
    dimensions[0] = width; dimensions[1] = height;
    for (axis = 0u; axis < 2u; ++axis) {
        minimum[axis] = maximum[axis] = (sint16)r_u16(position + 16u + offsets[axis]);
        for (vertex = 1u; vertex < 3u; ++vertex) {
            sint32 value = (sint16)r_u16(position + 16u + vertex * 8u + offsets[axis]);
            if (value < minimum[axis]) minimum[axis] = value;
            if (value > maximum[axis]) maximum[axis] = value;
        }
        range[axis] = maximum[axis] - minimum[axis];
        upper[axis] = (uint32)(sint32)(sint16)r_u16(position + offsets[axis]) + dimensions[axis];
        lower[axis] = (uint32)(sint32)(sint16)r_u16(position + offsets[axis]) - dimensions[axis];
        if (range[axis] < (sint32)(dimensions[axis] << 1)) dimensions[axis] = (uint32)(range[axis] >> 1);
    }
    size = (sint32)dimensions[1] < (sint32)dimensions[0] ? dimensions[1] : dimensions[0];
    for (axis = 2u; axis-- > 0u;) {
        if (maximum[axis] < (sint32)upper[axis]) {
            if ((sint32)lower[axis] < minimum[axis]) w_u16(position + offsets[axis], (uint32)minimum[axis] + size);
            else w_u16(position + offsets[axis], r_u16(position + offsets[axis]) - (upper[axis] - (uint32)maximum[axis]));
        } else if ((sint32)lower[axis] < minimum[axis])
            w_u16(position + offsets[axis], r_u16(position + offsets[axis]) - (lower[axis] - (uint32)minimum[axis]));
    }
}

void level_sub_8004E01C(uint32 width, uint32 height, uint32 position)
{
    FUNCTION_MARKER(0x8004E01Cu, "LEVEL.BIN");
    level_impact_clamp(width, height, position, 4u, 2u);
}

void level_sub_8004E1B8(uint32 width, uint32 height, uint32 position)
{
    FUNCTION_MARKER(0x8004E1B8u, "LEVEL.BIN");
    level_impact_clamp(width, height, position, 0u, 2u);
}

uint32 level_sub_8004E354(uint32 position, uint32 record)
{
    uint32 random, size, local, angle, distance, index, vertex, values[3];
    sint32 x, y, z, maximum;
    VECTOR input, output;
    FUNCTION_MARKER(0x8004E354u, "LEVEL.BIN");
    random = moh_draft_missing_call(0x8001552Cu); size = (uint32)((sint32)(random << 2) >> 15) + 5u;
    z = (sint16)r_u16(position + 12u); y = (sint16)r_u16(position + 10u); x = (sint16)r_u16(position + 8u);
    if (z < 0) z = -z; if (y < 0) y = -y; if (x < 0) x = -x;
    maximum = z > x ? z : x;
    if (maximum < y) moh_draft_missing_call(0x8004DE80u, size, size, position);
    else if (z < x) moh_draft_missing_call(0x8004E01Cu, size, size, position);
    else moh_draft_missing_call(0x8004E1B8u, size, size, position);
    w_u16(record + 20u, 0u - size); w_u16(record + 4u, 0u - size);
    w_u16(record + 14u, 0u - size); w_u16(record + 6u, 0u - size);
    w_u16(record + 28u, size); w_u16(record + 12u, size); w_u16(record + 30u, size); w_u16(record + 22u, size);
    w_u16(record + 32u, 0u); w_u16(record + 24u, 0u); w_u16(record + 16u, 0u); w_u16(record + 8u, 0u);
    local = moh_draft_missing_call(0x80016718u, 56u);
    angle = moh_draft_missing_call(0x8001536Cu, (uint32)(sint32)(sint16)r_u16(position + 8u), (uint32)(sint32)(sint16)r_u16(position + 12u));
    w_u16(local + 2u, angle + 0x800u); w_u16(local + 4u, 0u);
    z = (sint16)r_u16(position + 12u); x = (sint16)r_u16(position + 8u);
    xport_gte_write_data(9u, (uint32)x); xport_gte_write_data(10u, (uint32)x); xport_gte_write_data(11u, (uint32)z);
    input.vx = x; input.vy = x; input.vz = z; input.pad = 0; Square0(&input, &output);
    y = (sint16)r_u16(position + 10u);
    distance = moh_draft_missing_call(0x80016188u, (uint32)output.vx + (uint32)output.vz);
    angle = moh_draft_missing_call(0x8001536Cu, (uint32)y, distance); w_u16(local, angle + 0x1000u);
    for (index = 0u; index < 3u; ++index) w_u32(local + 8u + index * 4u, (uint32)(sint32)(sint16)r_u16(position + index * 2u));
    /* TODO: Original matrix and vector padding remains undefined */
    moh_draft_missing_call(0x8001AF98u, local, local + 24u);
    moh_draft_missing_call(0x80016218u, local + 24u, local + 8u);
    for (index = 0u; index < 8u; ++index) xport_gte_write_control(index, r_u32(local + 24u + index * 4u));
    for (index = 0u; index < 4u; ++index) {
        vertex = record + 4u + index * 8u;
        xport_gte_write_data(0u, r_u32(vertex)); xport_gte_write_data(1u, r_u32(vertex + 4u)); xport_gte_execute(0x480012u);
        values[0] = xport_gte_read_data(9u); values[1] = xport_gte_read_data(10u); values[2] = xport_gte_read_data(11u);
        w_u16(vertex, values[0]); w_u16(vertex + 2u, values[1]); w_u16(vertex + 4u, values[2]);
    }
    moh_draft_missing_call(0x80036E44u, local); return 1u;
}

uint32 level_sub_8004E614(uint32 position)
{
    uint32 index, offset, record, flags, first, second;
    sint32 x, y, z, previous;
    FUNCTION_MARKER(0x8004E614u, "LEVEL.BIN");
    index = r_u32(0x80093C58u);
    x = (sint16)r_u16(position); previous = (sint16)r_u16(0x800AB918u);
    if (previous - 16 < x && x < previous + 16) {
        y = (sint16)r_u16(position + 2u); previous = (sint16)r_u16(0x800AB91Au);
        if (previous - 16 < y && y < previous + 16) {
            z = (sint16)r_u16(position + 4u); previous = (sint16)r_u16(0x800AB91Cu);
            if (previous - 16 < z && z < previous + 16) return 0u;
        }
    }
    if (!r_u16(position + 8u) && !r_u16(position + 10u) && !r_u16(position + 12u)) return 0u;
    index += 1u; offset = index << 7;
    if ((sint32)index > 99) { index = 0u; offset = 0u; }
    record = 0x80093C5Cu + offset;
    moh_draft_missing_call(0x8004E354u, position, record);
    w_u16(0x80093C82u + offset, 0xC0u); w_u16(0x80093C84u + offset, 7u); w_u16(0x80093C86u + offset, 31u);
    w_u16(0x80093C8Au + offset, 0xFCu); flags = r_u32(record);
    w_u16(0x80093C80u + offset, 0x3E0u); w_u16(0x80093C88u + offset, 0x3E0u);
    w_u32(0x80093C58u, index); w_u32(record, flags | 7u);
    first = r_u32(position); second = r_u32(position + 4u);
    w_u32(0x800AB918u, first); w_u32(0x800AB91Cu, second);
    return 1u;
}

uint32 level_sub_80043284(uint32 object, uint32 reaction)
{
    uint32 state, flags, value, random, random2, count, nearest, closest, cursor, distance, resource, speed;
    uint32 local, x, z, pitch, yaw, roll;
    SVECTOR normal;
    VECTOR output;
    sint32 factor;
    FUNCTION_MARKER(0x80043284u, "LEVEL.BIN");
    flags = r_u32(object + 16u); state = r_u32(object + 0x388u);
    if (flags & 4u) {
        if (!(flags & 8u)) {
            w_u32(object + 16u, flags | 8u); w_u32(state + 0x5Cu, reaction); return 1u;
        }
        w_u32(object + 16u, flags & ~8u); reaction = r_u32(state + 0x5Cu);
        if (flags & 0x20u) {
            if (moh_draft_missing_call(0x8004E614u, object + 0x318u)) {
                x = r_u16(object + 0x318u); z = r_u16(object + 0x31Cu);
                w_u32(object + 0x20Cu, x << 16);
                value = r_u16(object + 0x31Au); w_u32(object + 0x214u, z << 16); w_u32(object + 0x210u, value << 16);
            }
            if (!(r_u16(state + 0x38u) & 0x200u)) {
                random = moh_draft_missing_call(0x8001552Cu); random = (uint32)((sint32)(random * 3u) >> 15) + 2u;
                random2 = moh_draft_missing_call(0x8001552Cu);
                moh_draft_missing_call(0x80073BA8u, object + 0x20Cu, 1u, random, 30u, 25u, 15u, 100u, 0x3F4u, 0xE0u, 0x3E0u, 0xFEu, 15u, 0x10000u, 1u, (uint32)((sint32)(random2 << 2) >> 15) + 6u);
            }
        }
    }
    if (!(reaction & 3u) && !(r_u16(state + 0x38u) & 0x200u)) goto finish;
    value = r_u16(state + 0x38u); w_u16(state + 0x38u, value | 2u);
    if ((value & 4u) && (reaction & 1u)) w_u32(object + 12u, r_u32(object + 12u) & ~0x80u);
    else if ((r_u32(state + 0x38u) & 0x208u) && !(r_u16(state + 0x38u) & 0x400u)) {
        if (!(r_u16(state + 0x38u) & 0x800u)) {
            speed = r_u32(object + 0x200u); speed -= (uint32)((sint32)speed / 3);
            w_u32(object + 0x200u, speed); if ((sint32)speed < 0) w_u32(object + 0x200u, 0u);
            if (reaction & 1u) {
                value = r_u32(object + 0x210u) + r_u32(object + 0x21Cu) + r_u32(state + 0x34u) + r_u32(object + 0x1F4u);
                if ((sint32)r_u32(object + 0x208u) < (sint32)value) {
                    w_u16(state + 0x38u, (r_u16(state + 0x38u) & 0xFFF6u) | 0x400u);
                    w_u32(object + 0x284u, 1u);
                    flags = r_u32(object + 12u); w_u32(object + 0x21Cu, 0u); w_u32(object + 0x228u, 0u);
                    w_u32(object + 12u, (flags & ~0x80u) | 0x28u); w_u32(state + 0x74u, 0u);
                    if ((sint32)r_u32(object + 0x200u) <= 0x1DFFFF) { w_u32(state + 0x80u, 0x1E0000u); w_u32(object + 0x200u, 0x1E0000u); }
                }
            }
        }
        resource = r_u32(object + 8u);
        random = moh_draft_missing_call(0x8001552Cu);
        if (resource == 0x13F3u)
            moh_draft_missing_call(0x80041CE8u, 1u, (uint32)((sint32)(random << 1) >> 15) + 7u, object, 0x40000u, 0u, 0x10000u, 0x40ACu, 0u, 0u, 0u, 0u);
        else
            moh_draft_missing_call(0x80041CE8u, 0u, (uint32)((sint32)(random * 7u) >> 15), object, 0x30000u, 0u, 0x10000u, 0x7FFFu, 0u, 0u, 3u, 0u);
        if (!(r_u16(state + 0x38u) & 0x800u)) { w_u32(state + 0x34u, 0xA0000u); w_u32(object + 0x21Cu, (uint32)((sint32)r_u32(object + 0x21Cu) >> 2)); }
    }
    if ((r_u16(state + 0x38u) & 0x10u) && (reaction & 1u)) {
        w_u16(r_u32(state) + 0x4A4u, 0u); w_u16(r_u32(state) + 0x4A6u, 0xFF9Cu); w_u16(r_u32(state) + 0x4A8u, 0u);
    }
    if (!(r_u16(state + 0x38u) & 0x200u)) goto finish;
    if (!(r_u32(object + 12u) & 0x80u)) w_u32(object + 0x200u, r_u32(state + 0x80u));
    flags = r_u16(state + 0x38u);
    if (!(flags & 0x800u)) {
        if (flags & 0x400u) {
            speed = r_u32(object + 0x200u); speed -= (uint32)((sint32)speed >> 1);
            w_u32(object + 0x200u, speed); if ((sint32)speed < 0) w_u32(object + 0x200u, 0u);
            if (r_u16(state + 0x38u) & 0x4000u) {
                cursor = r_u32(0x8009D64Cu); nearest = 0u; closest = 0x7FFFFFFFu; count = 0u;
                while (cursor) {
                    resource = r_u32(cursor + 8u);
                    if ((resource == 0x1402u || resource == 0x1405u) && !(r_u32(cursor + 12u) & 0x20080000u)) {
                        distance = moh_draft_missing_call(0x80050658u, object, cursor);
                        if (distance < 1200u) {
                            count++; w_u32(cursor + 0x3Cu, r_u32(cursor + 0x3Cu) | 0x40u);
                            if (!nearest || distance < closest) { nearest = cursor; closest = distance; }
                        }
                    }
                    cursor = r_u32(cursor + 4u);
                }
                if (nearest) {
                    if (r_u32(nearest + 8u) == 0x1405u || (count & 255u) < 3u) {
                        if (r_u32(nearest + 8u) != 0x1405u && closest >= 1000u) goto clear_nearest;
                        value = r_u32(nearest + 0x3Cu) | 0x80u;
                    } else value = r_u32(nearest + 0x3Cu) | 0x20u;
                    w_u32(nearest + 0x3Cu, value);
                    moh_draft_missing_call(0x8004C64Cu, object, 0xFFFFu, 7u, 0xFFFFFFFFu, 0xFFFFFFFFu);
                }
clear_nearest:
                w_u16(state + 0x38u, r_u16(state + 0x38u) & 0xBFFFu);
                /* Original nearest scan replaces the carried reaction value */
                reaction = nearest;
            }
        } else if (reaction & 2u) {
            speed = r_u32(object + 0x200u); speed -= (uint32)((sint32)speed / 3);
            w_u32(object + 0x200u, speed); if ((sint32)speed < 0) w_u32(object + 0x200u, 0u);
        }
        x = r_u32(state + 0x74u); z = r_u32(state + 0x78u);
        w_u32(state + 0x74u, (uint32)((sint32)x >> 1)); w_u32(state + 0x78u, (uint32)((sint32)z >> 1));
    }
    local = moh_draft_missing_call(0x80016718u, 8u);
    x = r_u16(object + 0x344u); z = r_u16(object + 0x350u);
    w_u16(local, x); w_u16(local + 2u, 0u); w_u16(local + 4u, z);
    /* TODO: Original vector padding is undefined and irrelevant to GTE matrix elements */
    xport_gte_write_control(0u, r_u32(local)); xport_gte_write_control(1u, r_u32(local + 4u));
    for (value = 0u; value < 3u; ++value) xport_gte_write_data(9u + value, r_u16(object + 0x364u + value * 2u));
    xport_gte_execute(0x41E012u);
    factor = -((sint32)xport_gte_read_data(25u) >> 17);
    normal.vx = (sint16)r_u16(object + 0x364u); normal.vy = (sint16)r_u16(object + 0x366u); normal.vz = (sint16)r_u16(object + 0x368u); normal.pad = 0;
    gte_gpf12(&normal, factor, &output);
    w_u16(local, x + xport_gte_read_data(9u)); w_u16(local + 4u, z + xport_gte_read_data(11u));
    moh_draft_missing_call(0x800431ECu, local, object + 0x228u, object + 0x22Cu);
    moh_draft_missing_call(0x80036E44u, local);
    value = r_u32(object + 0x228u); w_u32(object + 0x230u, 0u);
    if ((sint32)(value ^ r_u32(state + 0x68u)) >= 0) w_u32(state + 0x74u, 0u - r_u32(state + 0x74u));
    if ((sint32)(r_u32(object + 0x22Cu) ^ r_u32(state + 0x6Cu)) >= 0) w_u32(state + 0x78u, 0u - r_u32(state + 0x78u));
    if (!r_u32(state + 0x74u) && !r_u32(state + 0x78u)) {
        pitch = r_u32(object + 0x228u); yaw = r_u32(object + 0x22Cu); roll = r_u32(object + 0x230u);
        w_u32(state + 0x68u, pitch); w_u32(state + 0x6Cu, yaw); w_u32(state + 0x70u, roll);
    }
    if ((sint32)r_u32(object + 0x228u) > 0) {
        value = r_u32(object + 0x21Cu); if ((sint32)value < 0) value = 0u - value; w_u32(object + 0x21Cu, value);
    }
finish:
    if (!(r_u16(state + 0x38u) & 0x40u)) {
        if (r_u16(state + 0x38u) & 0x100u) {
            moh_draft_missing_call(0x800719C0u, object, 10u, 12u, 0x82u, 0x10000u, 0x1000u, 0x40000u, 5u, 0u);
            random = moh_draft_missing_call(0x8001552Cu);
            moh_draft_missing_call(0x80073BA8u, object + 0x20Cu, 5u, (uint32)((sint32)(random * 5u) >> 15) + 15u, 40u, 25u, 80u, 30u, 0x3FFu, 0x3Fu, 0x3E0u, 0xFFu, 1u, 0x17FFFu, 0u, 0u);
            moh_draft_missing_call(0x80042DF8u, object);
        } else moh_draft_missing_call(0x80043140u, object);
        random = moh_draft_missing_call(0x8001552Cu);
        value = r_u32(object + 0x22Cu) + (((random & 0x3FFu) - 0x200u) << 16) + 0x8000000u;
        w_u32(object + 0x22Cu, value); if ((sint32)value > 0x10000000) w_u32(object + 0x22Cu, value - 0x10000000u);
        moh_draft_missing_call(0x8006B8F0u, object);
    }
    return 1u;
}
