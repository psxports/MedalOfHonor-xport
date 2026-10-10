#include "game_draft_signatures.h"
#include "game_level_signatures.h"
#include "xport_trace.h"
#include "psx.h"
#include <stdio.h>
#include <stdlib.h>

extern uint32 moh_native_put_disp_env(uint32 guest_environment);

static void moh_scene_corner(const uint16 bounds[6], const sint32 normal[3])
{
    uint32 x = bounds[normal[0] < 0 ? 0 : 3];
    uint32 y = bounds[normal[1] < 0 ? 1 : 4];
    uint32 z = bounds[normal[2] < 0 ? 2 : 5];
    /* The unused high half of the original Z padding is omitted */
    xport_gte_write_data(0u, x | (y << 16u)); xport_gte_write_data(1u, z);
    xport_gte_execute(0x180001u);
}

void level_sub_8008B3E8(uint32 root)
{
    uint32 source, width, height, far_limit, mode, mask = 0u, visible = 0u, nodes[32], count = 0u;
    uint32 node, child, index, component, screen, flags;
    sint32 normals[6][3], top = 0;
    uint16 bounds[6];
    PsxGteSnapshot snapshot;
    static const uint32 child_offsets[3] = {24u, 32u, 28u};
    FUNCTION_MARKER(0x8008B3E8u, "LEVEL.BIN");
    height = r_u32(0x8009C82Cu);
    source = r_u32(r_u32(r_u32(0x8009D638u) + 0x388u) + 0xC8u);
    width = r_u32(0x8009C828u);
    for (index = 0u; index < 6u; ++index)
        for (component = 0u; component < 3u; ++component)
            normals[index][component] = (sint32)r_u32(source + 0x50u + index * 28u + component * 4u);
    far_limit = r_u32(source + 0x40u); mode = r_u32(0x800AA180u); nodes[0] = root;
    if (!mode) { visible = r_u32(0x800AA178u); mask = r_u32(0x800AA17Cu); }
    xport_gte_write_control(26u, 400u);
    while (top >= 0) {
        node = nodes[top--];
        if (!mode && (!(r_u32(node + 16u) & visible) || !(r_u32(node + 20u) & mask))) continue;
        for (index = 0u; index < 6u; ++index) bounds[index] = r_u16(node + index * 2u);
        moh_scene_corner(bounds, normals[0]);
        psx_gte_snapshot(&snapshot); flags = (uint32)snapshot.flag;
        if (flags & 0x40000u) continue;
        moh_scene_corner(bounds, normals[5]);
        psx_gte_snapshot(&snapshot); flags = (uint32)snapshot.flag;
        if ((sint32)(xport_gte_read_data(27u) << 16u) > (sint32)far_limit || (flags & 0x400000u)) continue;
        moh_scene_corner(bounds, normals[2]); screen = xport_gte_read_data(14u);
        if ((sint16)screen > (sint32)width) continue;
        moh_scene_corner(bounds, normals[1]); screen = xport_gte_read_data(14u);
        if ((sint16)screen < 0) continue;
        moh_scene_corner(bounds, normals[4]); screen = xport_gte_read_data(14u);
        if ((sint16)(screen >> 16u) > (sint32)height) continue;
        moh_scene_corner(bounds, normals[3]); screen = xport_gte_read_data(14u);
        if ((sint16)(screen >> 16u) < 0) continue;
        if ((sint32)r_u32(node + 12u) > 0) {
            w_u32(0x800AA1A0u + count * 4u, node); ++count;
        } else {
            for (index = 0u; index < 3u; ++index) {
                child = r_u32(node + child_offsets[index]);
                if (!child) continue;
                /* TODO Define behavior for traversal exceeding the original 32 slots */
                if (top >= 31) moh_draft_fail_fast("Scene traversal overflow", 0x8008B3E8u);
                nodes[++top] = child;
            }
        }
    }
    xport_gte_write_control(26u, r_u32(0x8009A628u));
    w_u32(0x800AB140u, 0u); w_u32(0x800AA19Cu, count);
}

uint32 level_sub_8008BF00(uint32 region, uint32 ordering_table, uint32 buffer_index, uint32 region_index)
{
    uint32 output, packet, end;
    FUNCTION_MARKER(0x8008BF00u, "LEVEL.BIN");
    output = sub_80016718(4u);
    if (!output) moh_draft_fail_fast("Scene packet end allocation", 0x8008BF00u);
    packet = level_sub_80053BD4(buffer_index, output);
    w_u32(0x800AA194u, 0u); w_u32(0x800AA198u, 0u); w_u32(0x800AA19Cu, 0u);
    end = r_u32(output) - 40u; w_u32(0x800AB140u, 0u); w_u32(output, end);
    /* Native ABI omits the original CPU scratchpad stack switch */
    level_sub_8008B3E8(r_u32(r_u32(region + 8u) + 20u));
    if (r_u32(0x800AA174u))
        packet = moh_draft_missing_call(0x80089D68u, packet, r_u32(output), region, ordering_table, region_index);
    else packet = moh_draft_missing_call(0x80010B60u, packet, r_u32(output), region, ordering_table);
    level_sub_80053BF4(packet);
    sub_80036E44(output); return r_u32(0x80098958u);
}

void level_sub_8008D850(uint32 object)
{
    uint32 local, index, x, y, z, tx, ty, tz;
    FUNCTION_MARKER(0x8008D850u, "LEVEL.BIN");
    local = sub_80016718(44u);
    if (!local) moh_draft_fail_fast("Scene matrix allocation", 0x8008D850u);
    moh_draft_missing_call(0x800154ECu, local, object + 0x340u, 32u);
    sub_800163E8(0x8009C804u, local + 20u, local + 32u);
    moh_draft_missing_call(0x8001BD08u, 0x8009C804u, local);
    x = r_u32(local + 32u); tx = r_u32(0x8009C818u);
    w_u32(local + 20u, x + tx);
    y = r_u32(local + 36u); ty = r_u32(0x8009C81Cu);
    z = r_u32(local + 40u); tz = r_u32(0x8009C820u);
    w_u32(local + 24u, y + ty); w_u32(local + 28u, z + tz);
    for (index = 0u; index < 8u; ++index) xport_gte_write_control(index, r_u32(local + index * 4u));
    sub_80036E44(local);
}

uint32 level_sub_8008C964(uint32 node, uint32 x, uint32 y, uint32 z, uint32 bound_out)
{
    FUNCTION_MARKER(0x8008C964u, "LEVEL.BIN");
    if (!node || (sint16)r_u16(node + 8u) < (sint32)y ||
        (sint32)x < (sint16)r_u16(node) || (sint16)r_u16(node + 6u) < (sint32)x ||
        (sint32)z < (sint16)r_u16(node + 4u) || (sint16)r_u16(node + 10u) < (sint32)z) return 0u;
    return (sint16)r_u16(node + 2u) <= (sint32)r_u32(bound_out);
}

uint32 level_sub_8008C9F0(uint32 node, uint32 x, uint32 y, uint32 z, uint32 bound_out, uint32 result, uint32 visible_out)
{
    sint32 bound;
    uint32 bits, flags;
    FUNCTION_MARKER(0x8008C9F0u, "LEVEL.BIN");
    if (!level_sub_8008C964(node, x, y, z, bound_out)) return result;
    if ((sint32)r_u32(node + 12u) > 0) {
        bound = (sint16)r_u16(node + 8u);
        if (bound < (sint32)r_u32(bound_out)) {
            w_u32(bound_out, (uint32)bound);
            bits = r_u32(visible_out); flags = r_u32(node + 16u);
            result = r_u32(node + 20u);
            w_u32(visible_out, bits | flags);
        }
    } else {
        result = level_sub_8008C9F0(r_u32(node + 28u), x, y, z, bound_out, result, visible_out);
        result = level_sub_8008C9F0(r_u32(node + 24u), x, y, z, bound_out, result, visible_out);
        result = level_sub_8008C9F0(r_u32(node + 32u), x, y, z, bound_out, result, visible_out);
    }
    return result;
}

uint32 level_sub_8008CB10(uint32 node, uint32 position, uint32 visible_out)
{
    uint32 local, result, x, y, z;
    FUNCTION_MARKER(0x8008CB10u, "LEVEL.BIN");
    local = sub_80016718(4u);
    if (!local) moh_draft_fail_fast("Visibility bound allocation", 0x8008CB10u);
    w_u32(local, 0x8001u);
    x = (uint32)(sint32)(sint16)r_u16(position);
    y = (uint32)(sint32)(sint16)r_u16(position + 2u);
    z = (uint32)(sint32)(sint16)r_u16(position + 4u);
    result = level_sub_8008C9F0(node, x, y, z, local, 0u, visible_out);
    sub_80036E44(local); return result;
}

uint32 level_sub_8008CB58(uint32 region, uint32 visible_out)
{
    uint32 source, x, y, z, position, result;
    FUNCTION_MARKER(0x8008CB58u, "LEVEL.BIN");
    source = r_u32(r_u32(r_u32(0x8009D638u) + 0x388u) + 0xC8u);
    x = r_u32(source); y = r_u32(source + 4u); z = r_u32(source + 8u);
    position = sub_80016718(8u);
    if (!position) moh_draft_fail_fast("Visibility position allocation", 0x8008CB58u);
    w_u16(position, (uint16)(x >> 16u)); w_u16(position + 2u, (uint16)(y >> 16u));
    w_u16(position + 4u, (uint16)(z >> 16u));
    result = level_sub_8008CB10(r_u32(r_u32(region + 8u) + 20u), position, visible_out);
    sub_80036E44(position); return result;
}

uint32 level_sub_8008CBE4(uint32 visible_out)
{
    uint32 result = 0u, index = 0u, region = 0x80098D8Cu;
    FUNCTION_MARKER(0x8008CBE4u, "LEVEL.BIN");
    if (!r_u32(0x80098984u)) return 0u;
    while ((sint32)index < (sint32)(r_u32(0x80098984u) - 1u)) {
        if (index != r_u32(0x800AA184u)) result |= level_sub_8008CB58(region, visible_out);
        ++index; region += 0xB0u;
    }
    if (index != r_u32(0x800AA184u)) result |= level_sub_8008CB58(0x80098D8Cu + index * 0xB0u, visible_out);
    return result;
}

uint32 level_sub_80060924(uint32 object, uint32 buffer_index)
{
    uint32 matrix, packet = 0x800AD170u + buffer_index * 40u, index, screen[3], projection;
    sint32 angle, x, y, dx1, dx2, dy1, dy2;
    uint32 base_x, base_y;
    FUNCTION_MARKER(0x80060924u, "LEVEL.BIN");
    matrix = sub_80016718(32u);
    if (!matrix) moh_draft_fail_fast("Screen effect matrix allocation", 0x80060924u);
    w_u32(matrix, 0x1000u); w_u32(matrix + 8u, 0x1000u); w_u32(matrix + 16u, 0x1000u);
    w_u32(matrix + 4u, 0u); w_u32(matrix + 12u, 0u);
    w_u32(matrix + 20u, 0u); w_u32(matrix + 24u, 0u); w_u32(matrix + 28u, 201u);
    angle = -(sint32)(sint16)r_u16(r_u32(object + 0x388u) + 0x186u);
    moh_draft_missing_call(0x8001C768u, (uint32)angle, matrix);
    for (index = 0u; index < 8u; ++index) xport_gte_write_control(index, r_u32(matrix + index * 4u));
    xport_gte_write_control(26u, 400u);
    for (index = 0u; index < 6u; ++index) xport_gte_write_data(index, r_u32(0x8009A5A0u + index * 4u));
    xport_gte_execute(0x280030u);
    for (index = 0u; index < 3u; ++index) screen[index] = xport_gte_read_data(12u + index);
    x = (sint16)screen[0]; y = (sint16)(screen[0] >> 16u);
    dx1 = ((sint32)(sint16)screen[1] - x) * 2;
    dx2 = ((sint32)(sint16)screen[2] - x) * 2;
    dy1 = (sint32)(sint16)(screen[1] >> 16u) - y;
    dy2 = (sint32)(sint16)(screen[2] >> 16u) - y;
    base_x = (uint16)(screen[0] - 197u); base_y = (uint16)((screen[0] >> 16u) - 76u);
    w_u16(packet + 8u, (uint16)(base_x + (uint32)dx1));
    w_u16(packet + 16u, (uint16)(base_x + (uint32)dx2));
    w_u16(packet + 24u, (uint16)(base_x - (uint32)dx2));
    w_u16(packet + 32u, (uint16)(base_x - (uint32)dx1));
    w_u16(packet + 10u, (uint16)(base_y + (uint32)dy1));
    w_u16(packet + 18u, (uint16)(base_y + (uint32)dy2));
    w_u16(packet + 26u, (uint16)(base_y - (uint32)dy2));
    w_u16(packet + 34u, (uint16)(base_y - (uint32)dy1));
    projection = r_u32(0x8009A628u); xport_gte_write_control(26u, projection);
    sub_80036E44(matrix); return projection;
}

static void moh_level_effect_link(uint32 packet, uint32 ordering_table)
{
    uint32 header = r_u32(packet), next = r_u32(ordering_table);
    w_u32(packet, (header & 0xFF000000u) | (next & 0xFFFFFFu));
    w_u32(ordering_table, (r_u32(ordering_table) & 0xFF000000u) | (packet & 0xFFFFFFu));
}

void level_sub_80060AFC(uint32 object)
{
    static const uint32 timer_addresses[4] = {0x8009A594u, 0x8009A590u, 0x8009A598u, 0x8009A59Cu};
    static const uint32 packet_addresses[4] = {0x800AD1E8u, 0x800AD1C0u, 0x800AD1FCu, 0x800AD1D4u};
    uint32 buffer, ordering_table, packet, threshold, index = 0u, red = 0u, green = 64u, timer, value;
    FUNCTION_MARKER(0x80060AFCu, "LEVEL.BIN");
    if (r_u32(0x8009A5E0u)) return;
    buffer = r_u32(0x8009C83Cu);
    threshold = 32u - ((uint32)(sint32)(sint16)r_u16(object + 0xF6u) >> 2u);
    ordering_table = r_u32(0x8009C838u);
    packet = 0x800AB970u + buffer * 0xC00u;
    moh_draft_missing_call(0x80060924u, object, buffer);
    while ((sint32)index < (sint32)threshold) {
        w_u8(packet + 4u, 1u); w_u8(packet + 5u, 1u); w_u8(packet + 6u, 1u);
        if ((sint32)index < 16) { ++green; red += 5u; }
        else { green -= 5u; --red; }
        if (!(index & 1u)) moh_level_effect_link(packet, ordering_table);
        ++index; packet += 24u;
    }
    while ((sint32)index < 32) {
        w_u8(packet + 4u, (uint8)red); w_u8(packet + 5u, (uint8)green); w_u8(packet + 6u, 0u);
        if ((sint32)index < 16) { ++green; red += 5u; }
        else { green -= 5u; --red; }
        if (!(index & 1u)) moh_level_effect_link(packet, ordering_table);
        ++index; packet += 24u;
    }
    for (index = 0u; index < 4u; ++index) {
        timer = r_u32(timer_addresses[index]);
        if (!timer) continue;
        timer = timer - 1u - r_u32(0x8009A5D4u);
        w_u32(timer_addresses[index], timer);
        if ((sint32)timer < 0) timer = 0u;
        packet = packet_addresses[index] + buffer * 80u;
        w_u32(timer_addresses[index], timer);
        moh_level_effect_link(packet, ordering_table);
        value = r_u8(packet + 7u);
        w_u8(packet + 7u, (uint8)((sint32)timer < 25 ? value | 2u : value & 0xFDu));
    }
    moh_level_effect_link(0x800AD170u + buffer * 40u, ordering_table);
}

void level_sub_80053C20(uint32 object, uint32 render_context)
{
    uint32 index = 0u, region = 0x80098D8Cu, visible;
    FUNCTION_MARKER(0x80053C20u, "LEVEL.BIN");
    moh_draft_missing_call(0x80060AFCu, r_u32(0x8009D654u));
    visible = moh_draft_missing_call(0x8008CBE4u, 0x800AA17Cu);
    w_u32(0x800AA178u, visible);
    moh_draft_missing_call(0x8008D850u, object);
    while ((sint32)index < (sint32)r_u32(0x80098984u)) {
        if (index != r_u32(0x800AA184u))
            moh_draft_missing_call(0x8008BF00u, region,
                r_u32(render_context + 0x2218u), r_u32(render_context + 0x221Cu), index);
        ++index; region += 0xB0u;
    }
    w_u32(0x800AA160u, 0u);
    moh_draft_missing_call(0x8008C0C8u);
}

void level_sub_8004F670(uint32 object, uint32 render_context)
{
    uint32 state, packet, tag, header, next;
    FUNCTION_MARKER(0x8004F670u, "LEVEL.BIN");
    state = r_u32(object + 0x388u);
    if (!(r_u32(state) & 1u)) return;
    packet = state + r_u32(render_context + 0x221Cu) * 40u + 28u;
    tag = r_u32(render_context + 0x2218u);
    header = r_u32(packet); next = r_u32(tag);
    w_u32(packet, (header & 0xFF000000u) | (next & 0xFFFFFFu));
    packet = state + r_u32(render_context + 0x221Cu) * 40u + 28u;
    tag = r_u32(render_context + 0x2218u);
    w_u32(tag, (r_u32(tag) & 0xFF000000u) | (packet & 0xFFFFFFu));
}

void level_sub_8008DA98(void)
{
    FUNCTION_MARKER(0x8008DA98u, "LEVEL.BIN");
}

void level_sub_80086634(uint32 object)
{
    uint32 frame = r_u32(0x8009A5D8u), render;
    FUNCTION_MARKER(0x80086634u, "LEVEL.BIN");
    if (r_u32(object + 0x3A0u) != frame) {
        render = r_u32(object + 0x100u);
        w_u32(object + 0x3A0u, frame);
        moh_draft_missing_call(0x80085648u, object, render);
    }
}

void level_sub_80084718(uint32 render, uint32 geometry, uint32 material, uint32 packet)
{
    uint32 transform, model, x, y, z, count;
    FUNCTION_MARKER(0x80084718u, "LEVEL.BIN");
    transform = sub_80016718(32u);
    if (!transform) moh_draft_fail_fast("Render transform allocation", 0x80084718u);
    model = r_u32(render + 0x80u);
    w_u16(transform + 16u, 0x1000u);
    x = (uint32)(sint32)(sint16)r_u16(render + 0x98u) << 19u;
    y = (uint32)(sint32)(sint16)r_u16(render + 0x9Au) << 19u;
    z = (uint32)(sint32)(sint16)r_u16(render + 0x9Cu) << 19u;
    w_u32(transform, 0x1000u); w_u32(transform + 8u, 0x1000u);
    w_u32(transform + 4u, 0u); w_u32(transform + 12u, 0u);
    count = r_u8(material + 15u);
    w_u32(0x8009EABCu, 0x800AB148u);
    model = r_u32(model + 4u);
    w_u32(transform + 24u, y);
    w_u32(0x1F8003FCu, 0u);
    w_u32(transform + 20u, x); w_u32(transform + 28u, z);
    w_u32(0x8009EAB4u, count); w_u32(0x8009EAB8u, model);
    /* TODO Bind the original model renderer to the native adapter */
    moh_draft_missing_call(0x80013AE4u, geometry, transform, packet, render);
    sub_80036E44(transform);
}

void level_sub_80046DD4(uint32 object)
{
    uint32 state, render, geometry, model, material, output, packet;
    FUNCTION_MARKER(0x80046DD4u, "LEVEL.BIN");
    state = r_u32(object + 0x388u); render = r_u32(object + 0x100u);
    if (!(r_u32(state + 0x9Cu) & 0x800u) || !render || !r_u32(render + 8u)) return;
    if (!r_u32(render + 0x74u)) level_sub_8008DA98();
    else level_sub_80086634(object);
    material = r_u32(render + 0x74u);
    geometry = r_u32(render + 0x84u); model = r_u32(render + 0x80u);
    material = r_u32(r_u32(material + 4u) + 4u);
    output = sub_80016718(4u);
    if (!output) moh_draft_fail_fast("Object packet end output allocation", 0x80046DD4u);
    packet = level_sub_80053BD4(r_u32(0x8009C83Cu), output);
    if (packet + (r_u32(model + 4u) << 4u) <= r_u32(output))
        level_sub_80084718(render, geometry, material, packet);
    sub_80036E44(output);
}

void level_sub_8006C714(uint32 render_context, uint32 object)
{
    uint32 next, callback, render, flags, trigger;
    FUNCTION_MARKER(0x8006C714u, "LEVEL.BIN");
    while (object) {
        callback = r_u32(object + 0x370u);
        next = r_u32(object + 4u);
        if (callback) moh_draft_indirect_call(callback, object, render_context);
        render = r_u32(object + 0x100u);
        if (render) {
            flags = r_u32(render + 4u);
            if (!(flags & 0x10u)) w_u32(render + 4u, flags & ~0x20u);
        }
        if ((r_u32(object + 12u) & 0x8000u) &&
            (sint32)r_u32(object + 0x290u) <= 0 && r_u32(object + 0x298u)) {
            trigger = r_u32(object + 0x1Cu);
            if (trigger && r_u32(r_u32(trigger + 4u) + 12u))
                level_sub_800664A0(0u, trigger, 2u);
            level_sub_8006B8F0(object);
        }
        object = next;
    }
}

sint32 level_sub_8005F86C(uint32 ordering_table, uint32 buffer_index, uint32 view_index)
{
    uint32 clut, local, packet, cursor, end, index = 0u, count = 0u, record = 0x80099F4Cu;
    uint32 mask, text, offset, character, value, tag, page;
    FUNCTION_MARKER(0x8005F86Cu, "LEVEL.BIN");
    clut = moh_draft_missing_call(0x8001BA18u, 0x3F0u, 0xFAu) & 0xFFFFu;
    local = sub_80016718(4u); if (!local) moh_draft_fail_fast("Text packet end output allocation", 0x8005F86Cu);
    packet = level_sub_80053BD4(buffer_index, local); cursor = packet + 12u; end = r_u32(local) - 20u;
    mask = 1u << (view_index & 31u);
    while ((sint32)index < (sint32)r_u32(0x80099ED0u)) {
        if (r_u32(record + 24u) & mask) {
            text = r_u32(record); offset = 0u;
            while (r_u8(text)) {
                if (end < cursor) {
                    /* Original overflow call forwards the packet bound in live a0 */
                    level_sub_80053BF4(end); sub_80036E44(local); return -1;
                }
                character = r_u8(text);
                if (character != 0x20u) {
                    moh_draft_missing_call(0x8001C748u, cursor);
                    if (r_u8(record + 11u)) w_u8(cursor + 7u, (uint8)(r_u8(cursor + 7u) + 2u));
                    w_u16(cursor + 8u, (uint16)(r_u16(record + 4u) + offset));
                    value = r_u16(record + 6u); w_u16(cursor + 16u, 10u); w_u16(cursor + 18u, 7u); w_u16(cursor + 10u, (uint16)value);
                    w_u8(cursor + 12u, r_u8(0x80099EB3u + r_u8(text)));
                    w_u8(cursor + 13u, r_u8(0x80099F10u + r_u8(text) - 0x21u));
                    w_u8(cursor + 4u, r_u8(record + 8u)); ++count; w_u8(cursor + 5u, r_u8(record + 9u));
                    value = r_u8(record + 10u); w_u16(cursor + 14u, (uint16)clut); w_u8(cursor + 6u, (uint8)value);
                    value = r_u32(cursor); tag = r_u32(ordering_table);
                    w_u32(cursor, (value & 0xFF000000u) | (tag & 0xFFFFFFu)); tag = r_u32(ordering_table);
                    w_u32(ordering_table, (tag & 0xFF000000u) | (cursor & 0xFFFFFFu)); cursor += 20u;
                }
                ++text; offset = (uint16)(offset + 12u);
            }
        }
        ++index; record += 32u;
    }
    page = moh_draft_missing_call(0x8001B968u, 0u, 1u, 0x3F8u, 0u) & 0xFFFFu;
    moh_draft_missing_call(0x8001B8C8u, packet, 0u, 1u, page, 0u);
    tag = r_u32(ordering_table); value = r_u32(packet); w_u32(packet, (value & 0xFF000000u) | (tag & 0xFFFFFFu));
    tag = r_u32(ordering_table); w_u32(ordering_table, (tag & 0xFF000000u) | (packet & 0xFFFFFFu));
    level_sub_80053BF4(cursor); sub_80036E44(local); return (sint32)count;
}

void level_sub_80046574(uint32 player)
{
    uint32 state = r_u32(player + 0x388u);
    sint32 kind = (sint32)(sint8)r_u8(state + 0x54u);
    uint32 clut, left, right, upper, lower;
    if (kind == 1) {
        clut = 0x3DFFu;
        left = 0xA0u;
        right = 0xC0u;
        upper = 0xA0u;
        lower = 0xB4u;
    } else if (kind == 2) {
        clut = 0x3FFFu;
        left = 0xC0u;
        right = 0xE0u;
        upper = 0xA0u;
        lower = 0xB4u;
    } else if (kind == 3) {
        clut = 0x3E7Fu;
        left = 0xA0u;
        right = 0xC0u;
        upper = 0xC0u;
        lower = 0xD4u;
    } else {
        clut = 0x3DFEu;
        left = 0x80u;
        right = 0xA0u;
        upper = 0xA0u;
        lower = 0xB4u;
    }
    w_u16(state + 0xEEu, (uint16)clut);
    w_u16(state + 0xC6u, (uint16)clut);
    w_u8(state + 0xC4u, (uint8)left);
    w_u8(state + 0xC5u, (uint8)upper);
    w_u8(state + 0xCCu, (uint8)right);
    w_u8(state + 0xCDu, (uint8)upper);
    w_u8(state + 0xD4u, (uint8)left);
    w_u8(state + 0xD5u, (uint8)lower);
    w_u8(state + 0xDCu, (uint8)right);
    w_u8(state + 0xDDu, (uint8)lower);
    w_u8(state + 0xECu, (uint8)left);
    w_u8(state + 0xEDu, (uint8)upper);
    w_u8(state + 0xF4u, (uint8)right);
    w_u8(state + 0xF5u, (uint8)upper);
    w_u8(state + 0xFCu, (uint8)left);
    w_u8(state + 0xFDu, (uint8)lower);
    w_u8(state + 0x104u, (uint8)right);
    w_u8(state + 0x105u, (uint8)lower);
    moh_draft_missing_call(0x800467ECu, player);
}

uint32 level_sub_8007555C(uint32 player)
{
    uint32 record = moh_draft_missing_call(0x800754CCu,
        (uint32)r_u8(r_u32(player + 0x388u) + 0x152u));
    uint32 pointer, state, x, y, z, rx, ry, rz;
    pointer = r_u32(record + 4u);
    w_u32(player + 0x20Cu, (uint32)(sint32)(sint16)r_u16(pointer + 0x10u) << 16);
    pointer = r_u32(record + 4u);
    w_u32(player + 0x210u, (uint32)(sint32)(sint16)r_u16(pointer + 0x12u) << 16);
    pointer = r_u32(record + 4u);
    w_u32(player + 0x214u, (uint32)(sint32)(sint16)r_u16(pointer + 0x14u) << 16);
    pointer = r_u32(record + 4u);
    state = r_u32(player + 0x388u);
    rx = (uint32)(sint32)(sint16)r_u16(pointer + 0x18u) << 16;
    y = r_u32(player + 0x210u);
    w_u32(player + 0x228u, rx);
    pointer = r_u32(record + 4u);
    z = r_u32(player + 0x214u);
    ry = (uint32)(sint32)(sint16)r_u16(pointer + 0x1Au) << 16;
    rx = r_u32(player + 0x228u);
    w_u32(player + 0x22Cu, ry);
    pointer = r_u32(record + 4u);
    x = r_u32(player + 0x20Cu);
    ry = r_u32(player + 0x22Cu);
    rz = (uint32)(sint32)(sint16)r_u16(pointer + 0x1Cu) << 16;
    w_u32(player + 0x258u, x);
    w_u32(player + 0x25Cu, y);
    w_u32(player + 0x260u, z);
    w_u32(player + 0x264u, rx);
    w_u32(player + 0x268u, ry);
    w_u32(player + 0x230u, rz);
    w_u32(player + 0x26Cu, rz);
    w_u32(state + 0x178u, rx);
    w_u32(state + 0x184u, r_u32(player + 0x22Cu));
    x = r_u32(player + 0x228u);
    y = r_u32(player + 0x234u);
    z = r_u32(state + 0x178u);
    w_u32(state + 0x180u, x + y - z);
    x = r_u32(player + 0x22Cu);
    y = r_u32(player + 0x238u);
    z = r_u32(state + 0x184u);
    w_u32(state + 0x18Cu, x + y - z);
    return 1u;
}

void level_sub_80075D1C(uint32 controller, uint32 slot, uint32 scale, uint32 mode)
{
    uint32 node = 0x800B74B0u + controller * 160u + slot * 40u;
    uint32 low, middle, high, difference, second_difference, quotient;
    if (mode == 2u) scale = r_u32(node + 28u);
    else if (mode == 3u) scale *= r_u32(node + 28u);
    else if (mode == 4u) scale = (uint32)((sint32)r_u32(node + 28u) >> (scale & 31u));
    else if (mode == 5u) scale = r_u32(node + 28u) << (scale & 31u);
    low = (uint32)r_u8(node + 12u);
    high = (uint32)r_u8(node + 14u);
    difference = (high - low) & 255u;
    if (!difference) difference = 1u;
    /* The original division traps are unreachable for the normalized byte denominator */
    quotient = (uint32)((sint32)32767 / (sint32)difference);
    middle = (uint32)r_u8(node + 13u);
    high = (uint32)r_u8(node + 14u);
    second_difference = (middle - high) & 255u;
    w_u32(node + 20u, quotient);
    if (!second_difference) second_difference = 1u;
    quotient = (uint32)((sint32)32767 / (sint32)second_difference);
    w_u32(node + 24u, quotient);
    if (!mode) w_u32(node + 28u, scale);
    w_u32(node + 32u, scale);
}

void level_sub_80075C6C(uint32 controller, uint32 slot, uint32 axis,
    uint32 threshold, uint32 mode, uint32 scale, uint32 invert)
{
    uint32 node = 0x800B74B0u + controller * 160u + slot * 40u;
    uint32 source = 0x800B7470u + controller * 32u + axis * 8u;
    uint8 final_byte;
    w_u32(node, axis);
    w_u32(node + 4u, threshold);
    w_u32(node + 8u, mode);
    w_u8(node + 12u, r_u8(source));
    w_u8(node + 13u, r_u8(source + 1u));
    w_u8(node + 14u, r_u8(source + 2u));
    w_u8(node + 15u, r_u8(source + 3u));
    final_byte = r_u8(source + 4u);
    w_u32(node + 36u, invert);
    w_u8(node + 16u, final_byte);
    moh_draft_missing_call(0x80075D1Cu, controller, slot, scale, 0u);
}

void level_sub_8007A050(uint32 player, uint32 state, uint32 controller)
{
    uint32 input = 0x80093B7Cu + controller * 4u;
    uint32 mode, configuration, invert, sensitivity, flags, diagnostic;
    if ((r_u32(input) & 0x101Fu) == 0x101Fu)
        w_u32(state, r_u32(state) | 0x10000000u);
    if ((r_u32(input) & 0x404Fu) == 0x404Fu)
        w_u32(state, r_u32(state) | 0x20000000u);
    if (r_u32(state + 0x134u)) {
        w_u32(player + 0x218u, 0u);
        w_u32(player + 0x21Cu, 0u);
        w_u32(player + 0x220u, 0u);
        w_u32(player + 0x200u, 0u);
        w_u32(state + 0x134u, r_u32(state + 0x134u) - 1u);
        return;
    }
    w_u8(state + 0x153u, r_u8(0x80093B78u + controller));
    if (controller) {
        mode = r_u32(0x800386D8u);
        configuration = r_u32(0x800386BCu);
        invert = r_u32(0x800386C8u);
        sensitivity = r_u32(0x800386CCu);
    } else {
        mode = r_u32(0x800386D4u);
        configuration = r_u32(0x800386A8u);
        invert = r_u32(0x800386B4u);
        sensitivity = r_u32(0x800386B8u);
    }
    if (sensitivity) {
        w_u16(state + 0xB4u, 0xD0u);
        w_u16(state + 0xB6u, 0x56u);
        flags = r_u32(state) | 0x400000u;
    } else {
        w_u16(state + 0xB4u, 0x40u);
        flags = r_u32(state);
        w_u16(state + 0xB6u, 0x1Eu);
        flags &= 0xFFBFFFFFu;
    }
    w_u32(state, flags);
    flags = r_u32(state);
    w_u32(state, invert ? flags & 0xFFF7FFFFu : flags | 0x80000u);
    if (mode != 1u && mode != 2u && mode != 3u && mode != 4u) mode = 0u;
    if (!r_u8(state + 0x153u)) {
        moh_draft_missing_call(0x80076240u, controller, 5u, configuration);
        moh_draft_missing_call(0x80076118u, controller);
        diagnostic = 0x8003D004u;
    } else {
        moh_draft_missing_call(0x80076240u, controller, mode, configuration);
        moh_draft_missing_call(0x80076118u, controller);
        diagnostic = 0x8003D020u;
    }
    moh_draft_missing_call(0x80015B28u, diagnostic);
    w_u32(state + 0x134u, 30u);
    w_u32(state, r_u32(state) & 0xFFFFFFEFu);
    flags = r_u32(state);
    level_sub_80079718(player, (sint32)controller);
    if (flags & 2u) moh_draft_missing_call(0x800798ACu, player, controller);
}

void level_sub_800798AC(uint32 object, uint32 controller)
{
    uint32 state;
    FUNCTION_MARKER(0x800798ACu, "LEVEL.BIN");
    state = r_u32(object + 0x388u); w_u32(state, (r_u32(state) | 2u) & 0xFFFFFFEFu);
    if (level_sub_8006D8AC(object + 0x2Cu)) { w_u32(object + 0x2D4u, 0x500000u); w_u16(object + 0x2DAu, 0u); }
    if ((sint32)controller < 0) return;
    moh_draft_missing_call(0x80075D1Cu, controller, 0u, 2u, 4u);
    moh_draft_missing_call(0x80075D1Cu, controller, 1u, 2u, 4u);
    moh_draft_missing_call(0x800758B4u, controller, 0u, 2u, 4u);
    moh_draft_missing_call(0x800758B4u, controller, 1u, 2u, 4u);
    if (r_u32(state) & 2u) {
        if (r_u32(0x800939F4u) == 5u && r_u32(0x800939F8u) == 4u) { w_u32(state + 0x3Cu, 0xFFFFFF6Au); return; }
        w_u32(state + 0x3Cu, 0xFFFFFFB0u);
        if ((sint32)r_u32(0x800AA168u) < 300) w_u32(0x800AA168u, 300u);
        if (r_u32(0x8009E830u)) return;
        if ((sint32)r_u32(0x800AA164u) < 2) w_u32(0x800AA164u, 2u);
        w_u32(0x8009E830u, 1u);
    } else {
        if ((sint32)r_u32(0x800AA168u) >= 251) w_u32(0x800AA168u, 250u);
        if (r_u32(0x8009E830u)) {
            if ((sint32)r_u32(0x800AA164u) >= 2) w_u32(0x800AA164u, 1u);
            w_u32(0x8009E830u, 0u);
        }
        w_u32(state + 0x3Cu, 0xFFFFFF10u);
    }
}

void level_sub_80079718(uint32 player, sint32 mode)
{
    uint32 state = r_u32(player + 0x388u);
    uint32 flags = r_u32(state);
    uint32 position = r_u32(state + 0x34u);
    w_u32(state, flags & 0xFFFFFFEDu);
    w_u32(player + 0x2D4u, position);
    w_u16(player + 0x2DAu, 0xFF9Cu);
    if (mode < 0) return;
    moh_draft_missing_call(0x80075D1Cu, (uint32)mode, 0u, 0u, 2u);
    moh_draft_missing_call(0x80075D1Cu, (uint32)mode, 1u, 0u, 2u);
    moh_draft_missing_call(0x800758B4u, (uint32)mode, 0u, 0u, 2u);
    moh_draft_missing_call(0x800758B4u, (uint32)mode, 1u, 0u, 2u);
    if (!(r_u32(state) & 2u)) {
        if ((sint32)r_u32(0x800AA168u) >= 251)
            w_u32(0x800AA168u, 250u);
        if (r_u32(0x8009E830u)) {
            if ((sint32)r_u32(0x800AA164u) >= 2)
                w_u32(0x800AA164u, 1u);
            w_u32(0x8009E830u, 0u);
        }
        w_u32(state + 0x3Cu, 0xFFFFFF10u);
        return;
    }
    if (r_u32(0x800939F4u) == 5u && r_u32(0x800939F8u) == 4u) {
        w_u32(state + 0x3Cu, 0xFFFFFF6Au);
        return;
    }
    position = r_u32(0x800AA168u);
    w_u32(state + 0x3Cu, 0xFFFFFFB0u);
    if ((sint32)position < 300) w_u32(0x800AA168u, 300u);
    if (r_u32(0x8009E830u)) return;
    if ((sint32)r_u32(0x800AA164u) < 2) w_u32(0x800AA164u, 2u);
    w_u32(0x8009E830u, 1u);
}

uint32 level_sub_8006692C(uint32 message, uint32 actor, uint32 trigger,
    uint32 filter, uint32 filter_actor, uint32 filter_trigger,
    uint32 filter_id, uint32 script, uint32 selected)
{
    uint32 table = r_u32(0x8009D5ACu), node, head, matched, temporary = 0u;
    uint32 result = 0u;
    if (table ? (sint32)message >= (sint32)r_u32(table) : (sint32)message >= 0) {
        moh_draft_missing_call(0x80015B28u, 0x8003CA50u, message);
        return 0u;
    }
    if (!filter) {
        moh_draft_missing_call(0x80066C14u, message, actor, trigger, 0u, 0u, 0u, 0u);
    } else {
        node = r_u32(r_u32(0x8009D5B0u) + message * 4u);
        while (node) {
            matched = 0u;
            if (actor) {
                if (!temporary) {
                    temporary = sub_80036BF0(8u);
                    if (!temporary) moh_draft_fail_fast("Message handler semantic words allocation", 0x8006692Cu);
                    w_u32(temporary, actor);
                    w_u32(temporary + 4u, filter_actor);
                }
                matched = moh_draft_missing_call(0x8006D7D0u, node, temporary);
                actor = r_u32(temporary);
                filter_actor = r_u32(temporary + 4u);
            }
            if (!matched && trigger) matched = trigger == r_u32(node + 4u);
            if (matched) {
                if (!r_u32(node + 8u)) {
                    w_u32(node + 8u, filter);
                    moh_draft_missing_call(0x8006D84Cu, node + 12u, filter_actor);
                    w_u32(node + 16u, filter_trigger);
                    w_u32(node + 20u, filter_id);
                    w_u32(node + 24u, script);
                    w_u32(node + 28u, selected);
                    result = 1u;
                    goto complete;
                }
                if (r_u32(node + 8u) == filter) {
                    switch (filter) {
                    case 0u:
                        break;
                    case 1u:
                    case 2u:
                        if (r_u32(node + 20u) == filter_id) {
                            result = 1u;
                            goto complete;
                        }
                        break;
                    case 3u:
                    case 4u:
                    case 5u:
                        if (filter == 5u && r_u32(node + 20u) != filter_id) break;
                        if (filter_actor) {
                            if (!temporary) {
                                temporary = sub_80036BF0(8u);
                                if (!temporary) moh_draft_fail_fast("Message handler semantic words allocation", 0x8006692Cu);
                                w_u32(temporary, actor);
                                w_u32(temporary + 4u, filter_actor);
                            }
                            matched = moh_draft_missing_call(0x8006D7D0u, node + 12u, temporary + 4u);
                            actor = r_u32(temporary);
                            filter_actor = r_u32(temporary + 4u);
                            if (matched) {
                                result = 1u;
                                goto complete;
                            }
                        }
                        if (filter_trigger && filter_trigger == r_u32(node + 16u)) {
                            result = 1u;
                            goto complete;
                        }
                        break;
                    default:
                        moh_draft_missing_call(0x80015B28u, 0x8003CA7Cu, filter);
                        goto complete;
                    }
                }
            }
            node = r_u32(node + 32u);
        }
    }
    node = moh_draft_missing_call(0x80066894u);
    if (!node) {
        moh_draft_missing_call(0x80015B28u, 0x8003CAB8u);
        goto complete;
    }
    moh_draft_missing_call(0x8006D84Cu, node, actor);
    w_u32(node + 4u, trigger);
    w_u32(node + 8u, filter);
    moh_draft_missing_call(0x8006D84Cu, node + 12u, filter_actor);
    head = r_u32(0x8009D5B0u) + message * 4u;
    w_u32(node + 16u, filter_trigger);
    w_u32(node + 20u, filter_id);
    w_u32(node + 24u, script);
    w_u32(node + 28u, selected);
    w_u32(node + 32u, r_u32(head));
    w_u32(head, node);
    result = 1u;
complete:
    if (temporary) sub_80036E44(temporary);
    return result;
}

void level_sub_800664A0(uint32 object, uint32 trigger, uint32 message)
{
    uint32 selected, base, table, index = 0u, record, packet;
    if (!object && !trigger) {
        printf((const char *)moh_draft_guest_pointer(0x8003C94Cu, 1u));
        return;
    }
    selected = moh_draft_missing_call(0x8006602Cu, object, trigger);
    if (selected == 0xFFFFFFFFu) return;
    base = r_u32(0x8009D5ACu);
    table = base + r_u32(base + selected * 4u + 8u);
    for (;;) {
        if (index >= r_u32(table)) return;
        record = moh_draft_missing_call(0x800694C8u, index, table);
        ++index;
        if ((uint32)r_u16(record) == message) break;
    }
    packet = sub_80036BF0(24u);
    if (!packet) abort();
    w_u32(packet, table + (uint32)r_u16(record + 2u));
    moh_draft_missing_call(0x8006D84Cu, packet + 4u, object);
    w_u32(packet + 16u, trigger);
    moh_draft_missing_call(0x8006D84Cu, packet + 8u, 0u);
    w_u32(packet + 20u, 0u);
    moh_draft_missing_call(0x8006D84Cu, packet + 12u, 0u);
    moh_draft_missing_call(0x800672E0u, packet);
    /* TODO Confirm that the message dispatcher does not retain the temporary packet */
    sub_80036E44(packet);
}

void level_sub_8003F1F8(uint32 object)
{
    uint32 state = r_u32(object + 0x100u), frame, animation, record, refreshed;
    uint32 packed, temporary, scale, index;
    sint32 x, y, z;
    if (!(r_u32(object + 12u) & 0x1000u)) return;
    frame = (uint32)(sint32)(sint16)r_u16(state + 0x5Eu);
    if ((sint32)frame < 0) return;
    animation = (uint32)(sint32)(sint16)r_u16(state + 0x5Cu);
    if (!r_u32(state + 0x74u)) {
        record = r_u32(r_u32(r_u32(state + 8u) + 0x98u) + animation * 16u + 8u);
        packed = r_u32(record + frame * 24u + 4u);
    } else {
        record = r_u32(r_u32(r_u32(state + 0x78u) + animation * 4u) + 4u) + frame * 20u;
        packed = r_u32(record + 4u);
        w_u16(state + 0x98u, (uint16)((sint32)(packed << 22) >> 22));
        refreshed = r_u32(object + 0x100u);
        frame = (uint32)(sint32)(sint16)r_u16(state + 0x5Eu);
        packed = r_u32(r_u32(r_u32(r_u32(refreshed + 0x78u) + animation * 4u) + 4u) + frame * 20u + 4u);
        w_u16(refreshed + 0x9Au, (uint16)((sint32)(packed << 11) >> 21));
        animation = (uint32)(sint32)(sint16)r_u16(state + 0x5Cu);
        refreshed = r_u32(object + 0x100u);
        frame = (uint32)(sint32)(sint16)r_u16(state + 0x5Eu);
        packed = r_u32(r_u32(r_u32(r_u32(refreshed + 0x78u) + animation * 4u) + 4u) + frame * 20u + 4u);
        w_u16(refreshed + 0x9Cu, (uint16)((sint32)(packed << 1) >> 22));
        packed = r_u32(record);
    }
    x = (sint32)(packed << 22) >> 22;
    y = (sint32)(packed << 11) >> 21;
    z = (sint32)(packed << 1) >> 22;
    if (y < 0) moh_draft_missing_call(0x80075118u, object);
    for (index = 0u; index < 5u; ++index)
        xport_gte_write_control(index, r_u32(object + 0x340u + index * 4u));
    scale = r_u32(object + 0x224u);
    if (scale != 0x10000u) {
        x = (sint32)((uint32)x * scale) >> 16;
        y = (sint32)((uint32)y * scale) >> 16;
        z = (sint32)((uint32)z * scale) >> 16;
    }
    temporary = sub_80036BF0(24u);
    if (!temporary) abort();
    /* TODO Original SVECTOR padding was undefined and is left uninitialized */
    w_u16(temporary, (uint16)x); w_u16(object + 0x27Cu, (uint16)x);
    w_u16(temporary + 2u, (uint16)y); w_u16(object + 0x27Eu, (uint16)y);
    w_u16(temporary + 4u, (uint16)z); w_u16(object + 0x280u, (uint16)z);
    moh_draft_missing_call(0x8001556Cu, temporary, temporary + 8u);
    w_u32(object + 0x1F0u, r_u32(object + 0x1F0u) + (r_u32(temporary + 8u) << 16));
    w_u32(object + 0x1F4u, r_u32(object + 0x1F4u) + (r_u32(temporary + 12u) << 16));
    w_u32(object + 0x1F8u, r_u32(object + 0x1F8u) + (r_u32(temporary + 16u) << 16));
    sub_80036E44(temporary);
}

uint32 level_sub_800589E8(uint32 object, uint32 instruction, uint32 flags)
{
    uint32 animation = moh_draft_missing_call(0x8005D148u,
        1u, flags, (uint32)r_u16(instruction), object);
    uint32 result = moh_draft_missing_call(0x8003F454u, object,
        (uint32)(sint32)(sint16)animation, 0u, r_u32(0x80099EB8u));
    if (result) return r_u32(object + r_u32(0x80099EB8u) * 116u + 0x118u);
    return instruction + 4u;
}

uint32 level_sub_80058C78(uint32 object, uint32 instruction)
{
    uint32 operands[8] = {0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u};
    uint32 word = r_u32(instruction), decoded = 1u, index, count, material, block;
    uint32 table, entry, selected = 0xFFFFFFFFu, payload, words, tag, priority;
    count = r_u32(object + 0x1ECu);
    for (index = 0u; index < 8u; ++index) {
        if (word & (1u << index)) {
            operands[index] = r_u32(instruction + decoded * 4u);
            ++decoded;
        }
    }
    material = r_u32(r_u32(object + 0x100u) + 8u);
    block = r_u32(0x80099EC4u);
    if (material && (sint16)r_u16(block + 26u) == (sint16)r_u16(material + 0x94u)) {
        table = r_u32(material + 0xB4u);
        count = r_u32(material + 0xB0u);
    } else if (r_u8(instruction + 3u) == 14u) table = r_u32(object + 0x124u);
    else table = r_u32(block + 32u);
    tag = word & 0xFFu;
    entry = table;
    for (index = 0u; index < count; ++index, entry += 60u) {
        if (r_u32(entry) == 0u && selected == 0xFFFFFFFFu) {
            selected = index;
            continue;
        }
        if (r_u32(entry) == tag && r_u32(entry + 8u) == operands[0] &&
            r_u32(entry + 12u) == operands[1] && r_u32(entry + 16u) == operands[2] &&
            r_u32(entry + 24u) == operands[4] && r_u32(entry + 20u) == operands[3] &&
            r_u32(entry + 28u) == operands[5] && r_u32(entry + 36u) == operands[7] &&
            r_u32(entry + 32u) == operands[6]) {
            selected = index;
            break;
        }
    }
    payload = instruction + decoded * 4u;
    index = r_u8(payload + 3u);
    words = r_u32(0x80090EA0u + index * 4u);
    if ((sint32)selected >= 0) {
        entry = table + selected * 60u;
        w_u32(entry, index == 15u ? 0u : tag);
        w_u32(entry + 8u, operands[0]); w_u32(entry + 12u, operands[1]);
        w_u32(entry + 16u, operands[2]); w_u32(entry + 20u, operands[3]);
        w_u32(entry + 28u, operands[5]); w_u32(entry + 36u, operands[7]);
        w_u32(entry + 24u, operands[4]);
        priority = (uint32)r_u8(instruction + 1u);
        w_u32(entry + 32u, operands[6]);
        w_u32(entry + 4u, priority);
        moh_draft_missing_call(0x800154ECu, entry + 40u, payload, words << 2);
    }
    return instruction + (decoded + words) * 4u;
}

uint32 level_sub_8007BD68(void)
{
    uint32 selected = r_u32(0x80093640u + (uint32)r_u8(0x8009E878u) * 68u);
    uint32 object = moh_draft_missing_call(0x8006B5B8u,
        3u, 784u, selected, 327u, 0u, 20u, 0u, 0u);
    if (object) {
        w_u32(object + 8u, 1u);
        w_u32(object + 12u, 1u);
        w_u32(object + 20u, 0x88204650u);
        w_u8(object + 0x35u, 0u);
        w_u32(object + 0x204u, 0x240000u);
        moh_draft_missing_call(0x8007BD1Cu, object);
    }
    return object;
}

void level_sub_8005C97C(uint32 object, uint32 property, uint32 value)
{
    uint32 state, first, second, third, address, mask, word;
    if (property - 0x60u < 0x20u) {
        if (property != 0x6Au) return;
        state = r_u32(object + 0x38Cu);
        if (!state) return;
        if (value) {
            first = r_u32(state + 0xE0u); second = r_u32(state + 0xE4u); third = r_u32(state + 0xE8u);
            w_u32(object + 0x20Cu, first); w_u32(object + 0x210u, second); w_u32(object + 0x214u, third);
        } else {
            first = r_u32(object + 0x20Cu); second = r_u32(object + 0x210u); third = r_u32(object + 0x214u);
            w_u32(state + 0xE0u, first); w_u32(state + 0xE4u, second); w_u32(state + 0xE8u, third);
        }
        return;
    }
    if ((property & 0x1C0u) == 0u) {
        w_u32(object + property * 4u + 0x4Cu, value);
        return;
    }
    if (property & 0x100u) {
        property &= 0xFFFFFEFFu;
        address = object + (property >> 5) * 4u + 0xECu;
        mask = 1u << (property & 31u);
    } else if (property & 0x40u) {
        address = object + 12u;
        mask = 1u << ((property - 0x40u) & 31u);
    } else if (property & 0x80u) {
        address = object + 16u;
        mask = 1u << ((property - 0x80u) & 31u);
    } else return;
    word = r_u32(address) | mask;
    w_u32(address, word);
    if (!value) word ^= mask;
    w_u32(address, word);
}

void level_sub_8003F8F0(uint32 object)
{
    uint32 state = r_u32(object + 0x100u), layer, offset, block, stream, cursor;
    uint32 start, limit, animation, material, choices, candidate, random_word, product;
    uint32 current_state;
    sint32 selection, high;
    sint16 old_animation;
    int selected;
    cursor = sub_80036BF0(4u);
    if (!cursor) abort();
    for (layer = 0u, offset = 0x104u; layer < 2u; ++layer, offset += 0x74u) {
        block = object + offset;
        if (r_u32(block + 16u) == 0u) {
            if (layer == 0u) moh_draft_missing_call(0x8003F454u, object, 0u, 0u, 0u);
            continue;
        }
        w_u32(cursor, r_u32(block + 20u));
        if ((sint16)r_u16(block + 28u) > 0) {
            w_u16(block + 28u, r_u16(block + 28u) - 1u);
            continue;
        }
        if ((sint16)r_u16(state + 0x5Eu) < (sint16)r_u16(block + 24u)) {
            w_u16(state + 0x5Eu, r_u16(state + 0x5Eu) + 1u);
            current_state = r_u32(object + 0x100u);
            if ((sint16)r_u16(current_state + 0x3Eu) > 0)
                w_u16(current_state + 0x3Eu, r_u16(current_state + 0x3Eu) - 1u);
            w_u16(block + 28u, 0u);
            moh_draft_missing_call(0x8003F1F8u, object);
            continue;
        }
        w_u16(block + 24u, 0xFFFFu);
        for (;;) {
            animation = (uint32)(sint32)(sint16)r_u16(block + 30u);
            if (animation == 0u) start = r_u32(r_u32(block + 16u) + 12u);
            else start = r_u32(block + (animation - 1u) * 16u + 44u);
            animation = (uint32)(sint32)(sint16)r_u16(block + 30u);
            if (animation == 0u) limit = r_u16(r_u32(block + 16u) + 10u);
            else limit = r_u16(block + (animation - 1u) * 16u + 48u);
            if ((sint32)((sint32)(r_u32(cursor) - start) >> 2) >= (sint32)limit) {
                w_u32(object + offset + 16u, 0u);
                w_u16(object + offset + 26u, 0xFFFFu);
                w_u16(r_u32(object + 0x100u) + 0x5Cu, 0xFFFFu);
                w_u16(r_u32(object + 0x100u) + 0x5Eu, 0u);
                if (layer != 0u) break;
                selected = 0;
                random_word = moh_draft_missing_call(0x8001552Cu, cursor);
                product = random_word * 100u;
                high = (sint32)(((long long)(sint32)product * (long long)(sint32)0x80010003u) >> 32);
                selection = ((sint32)((uint32)high + product) >> 14) - ((sint32)product >> 31);
                material = r_u32(state + 8u);
                choices = r_u16(material + 0xBCu);
                if (choices) {
                    stream = r_u32(material + 0xB8u);
                    for (candidate = 0u; candidate < choices; ++candidate, stream += 16u) {
                        if (selection < (sint32)r_u16(stream + 4u)) {
                            moh_draft_missing_call(0x8003F454u, object,
                                (uint32)(sint32)(sint16)candidate, 0u, 0u);
                            selected = 1;
                            break;
                        }
                    }
                }
                if (!selected) moh_draft_missing_call(0x8003F454u, object, 0u, 0u, 0u);
                block = object + offset;
                w_u32(cursor, r_u32(block + 20u));
            }
            old_animation = (sint16)r_u16(object + offset + 26u);
            candidate = moh_draft_missing_call(0x8005D18Cu, cursor, object, block,
                state + 0x5Cu, 0u, layer);
            if ((sint16)r_u16(object + offset + 26u) != old_animation) block = object + offset;
            w_u32(block + 20u, r_u32(cursor));
            if (candidate != 0u) break;
        }
    }
    sub_80036E44(cursor);
}

void level_sub_8004770C(uint32 camera)
{
    uint32 state, vector, x, y, z, distance;
    w_u32(camera + 12u, 0x20002u);
    w_u32(camera + 0x36Cu, 0x800473DCu);
    w_u32(camera + 0x384u, 0x800475C4u);
    state = r_u32(camera + 0x388u);
    w_u32(camera + 8u, 2u);
    w_u32(camera + 20u, 0u);
    w_u32(camera + 0x374u, 0u); w_u32(camera + 0x378u, 0u); w_u32(camera + 0x37Cu, 0u);
    w_u32(camera + 0x284u, 0u);
    w_u32(camera + 16u, r_u32(camera + 16u) | 0x213u);
    x = r_u32(camera + 0x20Cu); y = r_u32(camera + 0x210u); z = r_u32(camera + 0x214u);
    w_u32(camera + 0x3BCu, x); w_u32(camera + 0x3C0u, y); w_u32(camera + 0x3C4u, z);
    w_u32(state + 0x9Cu, 128u); w_u32(state + 20u, 0u);
    w_u16(state + 0xD8u, 3u); w_u16(state + 0xDAu, 3u); w_u16(state + 0xE0u, 2u);
    moh_draft_missing_call(0x80046C38u, camera, 0u, 0u, 0u);
    moh_draft_missing_call(0x8004760Cu, state);
    w_u32(state + 16u, 0u);
    moh_draft_missing_call(0x8006D84Cu, state + 0xA0u, 0u);
    w_u16(state + 0xCEu, 0u);
    distance = r_u32(r_u32(0x8009CC64u) + 0x590u);
    if ((sint32)distance <= 0) distance = 4500u;
    w_u32(state + 0xC4u, distance);
    w_u32(state + 0x90u, 0u);
    vector = r_u32(state + 0xC8u);
    w_u32(vector + 24u, 0u); w_u32(vector + 28u, 0xFFFF0000u); w_u32(vector + 32u, 0u);
    vector = r_u32(state + 0xC8u);
    w_u32(vector + 48u, 0x2000000u); w_u32(vector + 52u, 0x1800000u); w_u32(vector + 56u, 0u);
    vector = r_u32(state + 0xC8u);
    w_u32(vector + 60u, r_u32(0x8009A628u) << 16);
    vector = r_u32(state + 0xC8u);
    w_u32(vector + 64u, r_u32(state + 0xC4u) << 16);
}

void level_sub_80048DC8(uint32 camera, uint32 player, uint32 position)
{
    uint32 state = r_u32(camera + 0x388u), handle, previous, present;
    handle = r_u32(state + 12u);
    previous = handle ? r_u32(0x8009D678u + (handle & 0xFFFFu) * 4u) : 0u;
    handle = r_u32(state + 12u);
    present = handle ? r_u32(0x8009D678u + (handle & 0xFFFFu) * 4u) : 0u;
    if (present && (r_u32(previous + 12u) & 1u))
        moh_draft_missing_call(0x8006D84Cu, r_u32(previous + 0x388u) + 0xA0u, 0u);
    moh_draft_missing_call(0x8006D84Cu, state + 12u, player);
    if (position) {
        w_u32(state + 24u, r_u32(position));
        w_u32(state + 28u, r_u32(position + 4u));
        w_u32(state + 32u, r_u32(position + 8u));
    }
    if (r_u32(player + 12u) & 1u)
        moh_draft_missing_call(0x8006D84Cu, r_u32(player + 0x388u) + 0xA0u, camera);
}

void level_sub_8007BE04(uint32 player)
{
    uint32 state = r_u32(player + 0x388u), kind, material;
    sint32 index;
    w_u8(state + 0x152u, 0u);
    w_u32(player + 0xF0u, 0x800000u); w_u32(player + 0xF4u, 0x800000u);
    w_u32(state + 0x118u, 0xFFFFFFFFu);
    moh_draft_missing_call(0x8007BC0Cu, player, 0u);
    w_u32(player + 0x2D4u, 0xF00000u); w_u32(state + 0x34u, 0xF00000u);
    material = r_u8(0x80093A00u);
    w_u32(state, 0x400000u);
    w_u16(state + 0xB4u, 0xD0u); w_u8(state + 0x153u, 0xFFu);
    w_u16(state + 0xB6u, 0x56u); w_u32(state + 8u, material);
    moh_draft_missing_call(0x8007A050u, player, state, (uint32)r_u8(state + 0x152u));
    w_u32(state + 0x11Cu, 0u); w_u32(state + 0x124u, 0u); w_u32(state + 0x12Cu, 0u);
    w_u32(state + 0x4Cu, 0u); w_u32(state + 0x50u, 0u); w_u32(state + 0x128u, 0u);
    w_u32(state + 0x134u, 0u); w_u32(state + 0x138u, 0u);
    moh_draft_missing_call(0x8005C97Cu, player, 14u, 0u);
    moh_draft_missing_call(0x8005C97Cu, player, 2u, 0u);
    w_u32(player + 0x240u, 0u); w_u32(player + 0x244u, 0u); w_u32(player + 0x248u, 0u);
    w_u32(player + 0x284u, 1u); w_u16(state + 0x140u, 0u);
    moh_draft_missing_call(0x800469E0u);
    moh_draft_missing_call(0x8007555Cu, player);
    if (r_u32(state + 0x1C8u) < 2u) {
        w_u32(state + 0x180u, r_u32(player + 0x228u) + r_u32(player + 0x234u) -
            (r_u32(state + 0x178u) - (r_u32(state + 0x17Cu) << 2)));
        w_u32(state + 0x18Cu, r_u32(player + 0x22Cu) + r_u32(player + 0x238u) -
            (r_u32(state + 0x184u) + (r_u32(state + 0x188u) << 3)));
    } else {
        w_u32(state + 0x180u, r_u32(player + 0x228u) + r_u32(player + 0x234u) - r_u32(state + 0x178u));
        w_u32(state + 0x18Cu, r_u32(player + 0x22Cu) + r_u32(player + 0x238u) - r_u32(state + 0x184u));
    }
    moh_draft_missing_call(0x8006A76Cu, player);
    index = (sint32)moh_draft_missing_call(0x8006A910u, 0x3C8662u);
    if (index < 1) w_u32(state + 0x130u, 0u);
    else w_u32(state + 0x130u, moh_draft_missing_call(0x8006A8FCu, (uint32)index));
    material = r_u32(player + 0x370u);
    w_u32(state + 0xA4u, 400u); w_u32(state + 0xACu, 1u); w_u32(state + 0xB0u, material);
    kind = moh_draft_missing_call(0x8007BA20u, player, state);
    w_u32(state + 0x1C8u, kind);
    w_u16(state + 0x190u, kind == 5u ? 0x10Eu : 0x100u);
    w_u16(state + 0x192u, kind == 5u ? 0x73u : 0x83u);
    w_u16(state + 0x194u, 0u); w_u16(state + 0x196u, 0u);
    w_u32(state + 0x1CCu, kind);
    w_u16(state + 0x1D0u, 0u); w_u16(state + 0x1D2u, 0u);
}

void level_sub_80086F98(uint32 record)
{
    uint32 current = record + 4u, next = record + 6u, output, index = 0u;
    if (r_u32(record) == 0xFFFFFFFFu) {
        printf((const char *)moh_draft_guest_pointer(0x8003D340u, 1u));
        w_u32(0x8009EC5Cu, 0u);
        return;
    }
    output = sub_80036BF0((r_u32(record) + 1u) * 4u);
    w_u32(0x8009EC5Cu, output);
    if (!output) {
        printf((const char *)moh_draft_guest_pointer(0x8003D374u, 1u));
        abort();
    }
    while ((sint32)index < (sint32)(r_u32(record) + 1u)) {
        w_u32(output, current);
        output += 4u;
        current = next + (255u - r_u8(current)) * 2u;
        ++index;
        next = current + 2u;
    }
}

uint32 level_sub_8007540C(uint32 data)
{
    uint32 output, current = data + 2u, index = 0u, count, entries, end;
    count = (uint32)(sint32)(sint8)r_u8(data);
    w_u32(0x8009E28Cu, count);
    output = sub_80036BF0(count << 3);
    w_u32(0x8009E290u, output);
    if (!output) {
        w_u32(0x8009E28Cu, 0u);
        return 0xFFFFFFFFu;
    }
    count = r_u32(0x8009E28Cu);
    while ((sint32)index < (sint32)count) {
        entries = (uint32)(sint32)(sint8)r_u8(current);
        current += 2u;
        ++index;
        w_u32(output + 4u, current);
        end = current + entries * 20u;
        w_u16(output + 2u, (uint16)entries);
        current = end + 4u;
        w_u16(output, r_u16(end));
        output += 8u;
    }
    return 0u;
}

uint32 level_sub_80049444(uint32 destination, uint32 filename)
{
    uint32 source = filename, output;
    uint8 byte;
    moh_draft_missing_call(0x8001AD48u, destination, 0x80039DD0u);
    if (r_u8(filename) != 0x5Cu)
        moh_draft_missing_call(0x8001ABC8u, destination, 0x80039DD4u);
    output = destination + moh_draft_missing_call(0x8001AC88u, destination);
    byte = r_u8(source);
    while (byte != 0u) {
        w_u8(output, byte < 0x61u ? byte : (uint8)(byte - 0x20u));
        ++source; ++output;
        if ((sint32)(source - filename) >= 126) {
            printf((const char *)moh_draft_guest_pointer(0x80039DD8u, 1u),
                (const char *)moh_draft_guest_pointer(filename, 1u), 128);
        }
        byte = r_u8(source);
    }
    w_u8(output, 0u);
    moh_draft_missing_call(0x8001ABC8u, destination, 0x80039E14u);
    return destination;
}

uint32 level_sub_8006B5B8(uint32 flags, uint32 extra_bytes, uint32 resource_id,
    uint32 resource_arg, uint32 count72, uint32 count120, uint32 count28, uint32 material)
{
    uint32 size, object, cursor, kind, index;
    sint32 found;
    if (resource_id != 0xFFFFFFFFu && material == 0u) {
        found = (sint32)moh_draft_missing_call(0x8006A910u, resource_id);
        if (found < 1) return 0u;
        material = moh_draft_missing_call(0x8006A8FCu, (uint32)found);
    }
    size = extra_bytes + ((flags & 5u) ? 0x47Cu : 0x3CCu);
    if ((flags & 2u) && resource_id != 0xFFFFFFFFu) count72 = r_u16(material + 0xBEu);
    size += count72 * 72u + count28 * 28u + count120 * 120u;
    kind = r_u32(material + 0xFCu);
    if (kind == 0x1402u || kind == 0x1405u) object = moh_draft_missing_call(0x8008EEF0u);
    else object = moh_draft_missing_call(0x80036DA0u, size, 1u);
    if (!object) return 0u;
    moh_draft_missing_call(0x8006D930u, object, object);
    cursor = object + 0x3CCu;
    w_u32(object + 0x390u, 0xFFFFu); w_u32(object + 0x394u, 0u);
    w_u32(object + 0x398u, 0xFFFFu); w_u32(object + 0x39Cu, 0u);
    if (extra_bytes) {
        w_u32(object + 0x388u, cursor);
        cursor += extra_bytes;
    } else w_u32(object + 0x388u, 0u);
    if (flags & 5u) {
        w_u32(object + 0x100u, cursor);
        cursor += 0xB0u;
    } else w_u32(object + 0x100u, 0u);
    if (count72) {
        w_u32(object + 0x304u, cursor);
        cursor += count72 * 72u;
    } else w_u32(object + 0x304u, 0u);
    if (count28) {
        w_u32(object + 0x300u, cursor);
        cursor += count28 * 28u;
        w_u16(object + 0x2EEu, (uint16)count28);
    } else {
        w_u32(object + 0x300u, 0u); w_u16(object + 0x2EEu, 0u);
    }
    for (index = 0u; index < 2u; ++index) {
        if (count120) {
            w_u32(object + 0x124u + index * 0x74u, cursor);
            cursor += count120 * 60u;
            w_u32(object + 0x1ECu, count120);
        } else {
            w_u32(object + 0x124u + index * 0x74u, 0u);
            w_u32(object + 0x1ECu, 0u);
        }
    }
    moh_draft_missing_call(0x8006B350u, object);
    if (resource_id == 0xFFFFFFFFu || ((flags & 5u) &&
        moh_draft_missing_call(0x8006A96Cu, r_u32(object + 0x100u), material, resource_arg, object))) {
        w_u32(object + 0x38Cu, 0u);
        return object;
    }
    sub_80036E44(object);
    return 0u;
}

uint32 level_sub_80049540(uint32 filename)
{
    uint32 temporary, path, cursor, tail = r_u32(0x80093B10u), node, basename = 0u;
    uint32 attempt = 0u;
    sint32 status;
    temporary = sub_80036BF0(128u);
    if (!temporary) abort();
    path = moh_draft_missing_call(0x80049444u, temporary, filename);
    if (tail) while (r_u32(tail)) tail = r_u32(tail);
    node = sub_80036BF0(28u);
    if (!node) {
        printf((const char *)moh_draft_guest_pointer(0x80039E18u, 1u));
        sub_80036E44(temporary);
        return 0u;
    }
    w_u32(node, 0u);
    for (;;) {
        status = (sint32)moh_native_find_asset(node + 4u, path);
        ++attempt;
        if ((uint32)status + 1u > 1u) break;
        printf((const char *)moh_draft_guest_pointer(0x80039E34u, 1u),
            (const char *)moh_draft_guest_pointer(temporary, 1u), attempt);
        printf((const char *)moh_draft_guest_pointer(0x80039E5Cu, 1u), status);
        sub_8001AD98();
        moh_draft_missing_call(0x80016E38u, 0u);
        moh_draft_missing_call(0x80015DB8u, 0u);
        moh_draft_missing_call(0x80015DB8u, 0u);
        moh_draft_missing_call(0x80015DB8u, 0u);
    }
    if (r_u32(0x80093B10u)) w_u32(tail, node);
    else w_u32(0x80093B10u, node);
    cursor = path;
    while (r_u8(cursor)) {
        if (r_u8(cursor) == '\\') basename = cursor + 1u;
        ++cursor;
    }
    moh_draft_missing_call(0x8001AD48u, node + 12u, basename);
    sub_80036E44(temporary);
    return node;
}

uint32 level_sub_80053E68(void)
{
    uint32 scene, bsd, filename, sequence = 1u, texture, texture_index = 0u;
    uint32 selected = 0u, last_sequence, slot = 0u;
    int length;
    w_u32(0x80098960u, 0u);
    scene = moh_draft_missing_call(0x8006B5B8u, 0u, 28u, 0xFFFFFFFFu, 0u, 0u, 0u, 0u, 0u);
    if (!scene) return 0u;
    w_u32(scene + 8u, 3u); w_u32(scene + 16u, 0x203u);
    w_u32(scene + 0x36Cu, 0x80053C00u);
    w_u32(scene + 0x370u, 0x80053C20u);
    w_u32(scene + 0x384u, 0x80053D40u);
    w_u32(scene + 0x270u, 0x10000u);
    w_u32(scene + 0x274u, 0x10000u);
    w_u32(scene + 0x278u, 0x10000u);
    bsd = r_u32(0x8009CC64u);
    w_u32(scene + 20u, 0u);
    w_u32(scene + 0x374u, 0u); w_u32(scene + 0x378u, 0u);
    w_u32(scene + 0x20Cu, 0u); w_u32(scene + 0x210u, 0u); w_u32(scene + 0x214u, 0u);
    w_u32(scene + 0x228u, 0u); w_u32(scene + 0x22Cu, 0u); w_u32(scene + 0x230u, 0u);
    w_u32(scene + 0x284u, 1u);
    w_u32(scene + 12u, r_u32(scene + 12u) | 0x08020000u);
    w_u32(0x80098980u, sub_80036BF0(r_u32(bsd + 0x98u) << 1));
    bsd = r_u32(0x8009CC64u);
    w_u32(0x8009905Cu, r_u32(bsd + 0x80u));
    w_u32(0x80098984u, r_u32(bsd + 0x84u));
    filename = sub_80036BF0(40u);
    if (!filename) abort();
    while ((sint32)texture_index < (sint32)r_u32(0x8009905Cu)) {
        do {
            last_sequence = sequence++;
            length = snprintf((char *)moh_draft_guest_pointer(filename, 40u), 40u,
                (const char *)moh_draft_guest_pointer(0x8003C118u, 1u),
                (const char *)moh_draft_guest_pointer(r_u32(0x8009CC64u), 1u),
                (int)(sint32)last_sequence);
            /* TODO: Confirm custom BSD path prefixes fit the original 40-byte filename */
            if (length < 0 || length >= 40) abort();
            texture = moh_draft_missing_call(0x80049540u, filename);
        } while (!texture);
        bsd = r_u32(0x8009CC64u);
        if ((sint32)r_u32(bsd + 0x88u) < (sint32)sequence &&
            (sint32)selected < (sint32)r_u32(bsd + 0x84u)) {
            w_u32(0x80099060u + selected * 4u, last_sequence);
            ++selected;
        }
        if (r_u32(0x80098960u) < r_u32(texture + 8u))
            w_u32(0x80098960u, r_u32(texture + 8u));
        ++texture_index;
    }
    sub_80036E44(filename);
    while ((sint32)slot < (sint32)r_u32(0x80098984u)) {
        w_u32(0x8009904Cu + slot * 4u, sub_80036BF0(r_u32(0x80098960u) + 0x7FFu));
        ++slot;
    }
    return scene;
}

uint32 level_sub_8007529C(uint32 scene)
{
    uint32 data = r_u32(scene + 0x388u);
    uint32 geometry = r_u32(data), count, slot = 0u, record, type;
    if (!geometry) return 0xFFFFFFFFu;
    count = r_u32(geometry);
    w_u32(data + 20u, geometry + 16u);
    w_u32(data + 16u, count);
    w_u32(0x80098988u, geometry + count * 8u + 16u);
    record = r_u32(data + 20u) + 4u;
    w_u32(data + 24u, r_u32(0x80098988u));
    while ((sint32)slot < (sint32)r_u32(data + 16u)) {
        w_u32(record, r_u32(0x80098988u) + r_u32(record));
        type = r_u32(r_u32(record) + 12u);
        switch (type) {
        case 1u: case 4u: case 5u:
            moh_draft_missing_call(0x80075120u, r_u32(record));
            break;
        case 2u:
            moh_draft_missing_call(0x80075120u, r_u32(record));
            moh_draft_missing_call(0x800751B4u, r_u32(record));
            break;
        case 3u:
            moh_draft_missing_call(0x80075120u, r_u32(record));
            moh_draft_missing_call(0x800751ECu, r_u32(record));
            break;
        case 6u:
            moh_draft_missing_call(0x80075120u, r_u32(record));
            moh_draft_missing_call(0x800751D0u, r_u32(record));
            break;
        default: break;
        }
        ++slot; record += 8u;
    }
    return 0u;
}

uint32 level_sub_8004A374(uint32 filename, uint32 count_out)
{
    uint32 temporary, header, path, file_info, result_buffer, location;
    uint32 attempt, previous_attempt, asset = 0u;
    sint32 status = 0;
    int again;
    w_u32(count_out, 0u);
    temporary = sub_80036BF0(2200u);
    if (!temporary) abort();
    header = temporary;
    path = temporary + 2048u;
    file_info = temporary + 2176u;
    result_buffer = temporary + 2184u;
    location = temporary + 2192u;
    sub_800362E8(path, filename, 0u);
    attempt = 1u;
    do {
        status = (sint32)sub_800374EC(file_info, path);
        if ((uint32)status + 1u > 1u) goto read_header;
        printf((const char *)moh_draft_guest_pointer(0x80039E34u, 1u),
            (const char *)moh_draft_guest_pointer(path, 1u), attempt);
        printf((const char *)moh_draft_guest_pointer(0x80039E5Cu, 1u), status);
        sub_8001AD98();
        moh_draft_missing_call(0x80016E38u, 0u);
        moh_draft_missing_call(0x80015DB8u, 0u);
        moh_draft_missing_call(0x80015DB8u, 0u);
        moh_draft_missing_call(0x80015DB8u, 0u);
        again = attempt < 32u;
        ++attempt;
    } while (again);
    if (attempt >= 32u) {
        printf((const char *)moh_draft_guest_pointer(0x8003A128u, 1u),
            (const char *)moh_draft_guest_pointer(path, 1u), attempt);
        goto failure;
    }
read_header:
    attempt = 0u;
    sub_8001ADD8(r_u32(file_info), location);
    for (;;) {
        previous_attempt = attempt++;
        if (previous_attempt >= 32u) break;
        status = 1;
        if (moh_draft_missing_call(0x80019220u, location, 1u, header, 0x80u)) {
            do {
                status = (sint32)moh_draft_missing_call(0x800195D0u, result_buffer);
            } while (status > 0);
            if (status == 0) break;
        }
        printf((const char *)moh_draft_guest_pointer(0x80039E34u, 1u),
            (const char *)moh_draft_guest_pointer(path, 1u), attempt);
        printf((const char *)moh_draft_guest_pointer(0x80039E5Cu, 1u), status);
        sub_8001AD98();
        moh_draft_missing_call(0x80016E38u, 0u);
        moh_draft_missing_call(0x80015DB8u, 0u);
        moh_draft_missing_call(0x80015DB8u, 0u);
        moh_draft_missing_call(0x80015DB8u, 0u);
    }
    if (attempt >= 32u) goto failure;
    asset = moh_draft_missing_call(0x8004A300u, header, count_out);
    if (!asset) goto failure;
    attempt = 0u;
    sub_8001ADD8(r_u32(file_info) + 1u, location);
    for (;;) {
        previous_attempt = attempt++;
        if (previous_attempt >= 32u) break;
        status = 1;
        if (moh_draft_missing_call(0x80019220u, location,
            r_u32(header + r_u32(count_out) * 4u) >> 11, asset, 0x80u)) {
            do {
                status = (sint32)moh_draft_missing_call(0x800195D0u, result_buffer);
            } while (status > 0);
            if (status == 0) break;
        }
        printf((const char *)moh_draft_guest_pointer(0x80039E34u, 1u),
            (const char *)moh_draft_guest_pointer(path, 1u), attempt);
        printf((const char *)moh_draft_guest_pointer(0x80039E5Cu, 1u), status);
        sub_8001AD98();
        moh_draft_missing_call(0x80016E38u, 0u);
        moh_draft_missing_call(0x80015DB8u, 0u);
        moh_draft_missing_call(0x80015DB8u, 0u);
        moh_draft_missing_call(0x80015DB8u, 0u);
    }
    if (attempt < 32u) {
        sub_80036E44(temporary);
        return asset;
    }
failure:
    printf((const char *)moh_draft_guest_pointer(0x8003A150u, 1u));
    if (asset) sub_80036E44(asset);
    sub_80036E44(temporary);
    return 0u;
}

void level_sub_80062358(void)
{
    uint32 node = r_u32(0x8009CC64u);
    if (r_u32(node + 0xA0u) == 0u) {
        w_u8(0x8009A649u, r_u8(node + 0xCEu));
        w_u8(0x8009A64Au, r_u8(node + 0xCFu));
        w_u8(0x8009A64Bu, r_u8(node + 0xD0u));
        w_u8(0x8009A701u, r_u8(node + 0xCEu));
        w_u8(0x8009A702u, r_u8(node + 0xCFu));
        w_u8(0x8009A703u, r_u8(node + 0xD0u));
        w_u8(0x8009A6A5u, r_u8(node + 0xCEu));
        w_u8(0x8009A6A6u, r_u8(node + 0xCFu));
        w_u8(0x8009A6A7u, r_u8(node + 0xD0u));
        w_u8(0x8009A75Du, r_u8(node + 0xCEu));
        w_u8(0x8009A75Eu, r_u8(node + 0xCFu));
        w_u8(0x8009A75Fu, r_u8(node + 0xD0u));
    }
}

static uint32 level_pause_button(uint32 mask)
{
    return moh_draft_missing_call(0x8004D694u,
        (r_u16(0x8009A5D4u) + 1u) & 0xFFFFu, r_u32(0x8009A5F0u), mask, 1u, 0u);
}

static uint32 level_pause_direction(void)
{
    if (level_pause_button(0x8000u) || level_pause_button(0x2000u)) return 1u;
    if (r_u8(0x80093B78u + r_u32(0x8009A5F0u)) != 0u) {
        if (moh_draft_missing_call(0x800624CCu)) return 1u;
        if (moh_draft_missing_call(0x80062544u, r_u32(0x8009A5F0u))) return 1u;
    }
    return 0u;
}

static void level_pause_sound(uint32 sound, uint32 volume, uint32 pitch)
{
    moh_draft_missing_call(0x80041CE8u, 0u, sound, r_u32(0x8009D654u),
        volume, 0u, pitch, 0u, 0u, 0u, 0u, 0u);
}

static void level_pause_text(uint32 text, uint32 x, uint32 y, uint32 red, uint32 green, uint32 blue)
{
    moh_draft_missing_call(0x8005F28Cu, text, x, y, red, green, blue,
        0u, 0xFFFFFFFFu, 1u, 1u, 0u);
}

static void level_pause_clear(void)
{
    moh_draft_missing_call(0x8004EB98u, r_u32(0x8009A5C4u));
    moh_draft_missing_call(0x8005F60Cu, 1u);
}

static void level_pause_background(void)
{
    w_u32(0x8009A5C4u, moh_draft_missing_call(0x8004E798u,
        0u, 0u, 512u, 240u, 1u, 109u, 119u, 82u, 2u, 0u, 1u));
}

static void level_pause_copy_record(uint32 destination, uint32 source)
{
    uint32 a, b, c, d;
    a = r_u32(source); b = r_u32(source + 4u);
    c = r_u32(source + 8u); d = r_u32(source + 12u);
    w_u32(destination, a); w_u32(destination + 4u, b);
    w_u32(destination + 8u, c); w_u32(destination + 12u, d);
    a = r_u32(source + 16u); b = r_u32(source + 20u);
    c = r_u32(source + 24u); d = r_u32(source + 28u);
    w_u32(destination + 16u, a); w_u32(destination + 20u, b);
    w_u32(destination + 24u, c); w_u32(destination + 28u, d);
}

static void level_pause_capture(void)
{
    uint32 slot, destination;
    if (r_u32(0x8009A61Cu) == 0u) {
        destination = 0x8009CD10u;
        w_u32(0x8009D310u, r_u32(0x80099ED0u));
        while ((sint32)r_u32(0x80099ED0u) > 2) {
            slot = r_u32(0x80099ED0u) - 1u;
            w_u32(0x80099ED0u, slot);
            level_pause_copy_record(destination, 0x80099F4Cu + slot * 32u);
            destination += 32u;
        }
        w_u32(0x8009A61Cu, 1u);
    }
    if (r_u32(0x8009CC68u) == 0u) {
        for (slot = 0u; slot < 24u; ++slot) {
            moh_draft_missing_call(0x80015B88u, slot, 0x8009CC80u + slot * 4u, 0x8009CC82u + slot * 4u);
            moh_draft_missing_call(0x8001D120u, slot, 0x8009CCE0u + slot * 2u);
            moh_draft_missing_call(0x80015C08u, slot, 0u, 0u);
            moh_draft_missing_call(0x800157BCu, slot, 0u);
        }
        w_u32(0x8009CC68u, 1u);
    }
    if (r_u32(0x8009CC6Cu) == 0u) w_u32(0x8009CC6Cu, 1u);
}

static void level_pause_draw_menu(void)
{
    uint32 slot = 0u, y = 88u;
    level_pause_capture();
    level_pause_background();
    level_pause_text(0x8003C524u, 220u, 28u, 128u, 128u, 0u);
    level_pause_text(0x8003C52Cu, 148u, 48u, 128u, 128u, 128u);
    level_pause_text(0x8003C540u, 100u, 68u, 100u, 100u, 100u);
    moh_draft_missing_call(0x8001BE18u, 0x8009D314u, 0x8003C55Cu, r_u32(0x80039CB8u));
    level_pause_text(0x8009D314u, 416u, 68u, 128u, 96u, 20u);
    level_pause_text(0x8003C560u, 48u, 188u, 94u, 94u, 94u);
    if (r_u32(0x8009A600u) != 0u) {
        level_pause_text(0x8003C584u, 28u, 58u, 100u, 100u, 100u);
        w_u32(0x8009A604u, 1u);
    }
    while ((sint32)slot < (sint32)r_u32(0x80098620u)) {
        if (r_u32(0x800988DCu + slot * 4u) != 0u) {
            level_pause_text(0x8003C5ACu, 48u, y, 128u, 96u, 20u);
            level_pause_text(0x8003C5B0u, 48u, y, 128u, 0u, 0u);
        } else level_pause_text(0x8003C5ACu, 48u, y, 128u, 96u, 20u);
        level_pause_text(r_u32(0x80098904u + slot * 4u), 72u, y, 128u, 96u, 20u);
        ++slot; y += 16u;
    }
    if (r_u32(0x8009A5F4u) == 0u) {
        level_pause_text(0x8003C5B4u, 100u, 208u, 63u, 63u, 63u);
        level_pause_text(0x8003C5C0u, 388u, 208u, 128u, 64u, 64u);
    } else {
        level_pause_text(0x8003C5B4u, 100u, 208u, 128u, 64u, 64u);
        level_pause_text(0x8003C5C0u, 388u, 208u, 63u, 63u, 63u);
    }
}

static void level_pause_draw_confirmation(void)
{
    level_pause_background();
    level_pause_text(0x8003C5C0u, 232u, 68u, 128u, 96u, 20u);
    level_pause_text(0x8003C5C8u, 178u, 102u, 128u, 96u, 20u);
    level_pause_text(0x8003C560u, 48u, 188u, 94u, 94u, 94u);
    if (r_u32(0x8009A5ECu) == 0u) {
        level_pause_text(0x8003C5D8u, 310u, 136u, 128u, 64u, 64u);
        level_pause_text(0x8003C5DCu, 178u, 136u, 63u, 63u, 63u);
    } else {
        level_pause_text(0x8003C5D8u, 310u, 136u, 63u, 63u, 63u);
        level_pause_text(0x8003C5DCu, 178u, 136u, 128u, 64u, 64u);
    }
}

uint32 level_sub_800625BC(void)
{
    uint32 disconnected, pressed, count, source, slot;
    if (moh_draft_missing_call(0x8004F700u) == 0u && r_u32(0x8009A5E0u) == 0u) {
        disconnected = r_u32(0x8009A600u) != 0u;
        if (disconnected) w_u32(0x8009A5F0u, 0u);
        pressed = moh_draft_missing_call(0x8004D694u,
            (r_u16(0x8009A5D4u) + 1u) & 0xFFFFu, 0u, 0x800u, 1u, 0u) != 0u;
        if (pressed) w_u32(0x8009A5F0u, 0u);
        if (disconnected || pressed) {
            w_u32(0x8009CC78u, !pressed);
            w_u32(0x8009CC7Cu, pressed ? 0u : 10u);
            w_u32(0x8009A5E0u, 1u);
            w_u32(0x8009A5F4u, 1u);
            w_u32(0x8009A5ECu, 1u);
            level_pause_draw_menu();
        }
    }
    if (r_u32(0x8009A5E0u) == 0u) return 0u;
    if (r_u32(0x8009A5E4u) == 0u) {
        if (level_pause_direction()) {
            level_pause_sound(0x21u, 0x10000u, 0x10000u);
            w_u32(0x8009A5F4u, r_u32(0x8009A5F4u) ^ 1u);
            level_pause_clear();
            level_pause_draw_menu();
        } else if (level_pause_button(0x40u) || r_u32(0x8009A5E8u) != 0u) {
            w_u32(0x8009A5E8u, 1u);
            if (r_u32(0x8009A5FCu) == 0u) {
                level_pause_sound(0x13u, 0x20000u, 0x8000u);
                w_u32(0x8009A5FCu, 1u);
            }
            if ((r_u32(0x80093B7Cu + r_u32(0x8009A5F0u) * 4u) & 0x40u) == 0u) {
                w_u32(0x8009A5FCu, 0u); w_u32(0x8009A5E8u, 0u);
                if (r_u32(0x8009A5F4u) != 0u) goto resume;
                w_u32(0x8009A5E4u, 1u);
                w_u32(0x8009A610u, 0u); w_u32(0x8009A614u, 0u);
            }
        } else if (r_u32(0x8009CC78u) == 0u) {
            count = r_u32(0x8009CC7Cu);
            w_u32(0x8009CC7Cu, count + 1u);
            if ((sint32)count >= 10) w_u32(0x8009CC78u, 1u);
        } else if (level_pause_button(0x800u)) goto resume;
        if (r_u32(0x8009A600u) != r_u32(0x8009A604u)) {
            level_pause_clear(); level_pause_draw_menu();
        }
        if (r_u32(0x8009A600u) != 0u) return 0u;
        w_u32(0x8009A604u, 0u);
        return 0u;
    }
    if (r_u32(0x8009A5F8u) == 0u) {
        w_u32(0x8009A5F8u, 1u);
        level_pause_clear(); level_pause_draw_confirmation();
        return 0u;
    }
    if (level_pause_button(0x40u) || r_u32(0x8009A5E8u) != 0u) {
        w_u32(0x8009A5E8u, 1u);
        if (r_u32(0x8009A5FCu) == 0u) {
            level_pause_sound(0x13u, 0x20000u, 0x8000u);
            w_u32(0x8009A5FCu, 1u);
        }
        if ((r_u32(0x80093B7Cu + r_u32(0x8009A5F0u) * 4u) & 0x40u) != 0u) return 0u;
        w_u32(0x8009A5FCu, 0u);
        if (r_u32(0x8009A5ECu) == 0u) {
            w_u32(0x8009A5F8u, 0u);
            level_pause_clear();
            moh_draft_missing_call(0x8006450Cu);
            w_u32(0x8009C840u, r_u32(0x8009C840u) | 0x60u);
            return 1u;
        }
        w_u32(0x8009A5FCu, 0u); w_u32(0x8009A5E8u, 0u);
    } else {
        if (level_pause_direction()) {
            level_pause_sound(0x21u, 0x10000u, 0x10000u);
            w_u32(0x8009A5ECu, r_u32(0x8009A5ECu) ^ 1u);
            level_pause_clear(); level_pause_draw_confirmation();
            return 0u;
        }
        if (r_u32(0x8009CC78u) == 0u) {
            count = r_u32(0x8009CC7Cu);
            w_u32(0x8009CC7Cu, count + 1u);
            if ((sint32)count >= 10) w_u32(0x8009CC78u, 1u);
            return 0u;
        }
        if (level_pause_button(0x800u) == 0u) return 0u;
    }
resume:
    w_u32(0x8009A5F8u, 0u);
    source = 0x8009CD10u + (r_u32(0x8009D310u) - 3u) * 32u;
    level_pause_clear();
    w_u32(0x8009A5E4u, 0u); w_u32(0x8009A5E0u, 0u); w_u32(0x8009A5E8u, 0u);
    w_u32(0x8009A610u, 0u); w_u32(0x8009A614u, 0u);
    if (r_u32(0x8009CC68u) != 0u) {
        for (slot = 0u; slot < 24u; ++slot) {
            moh_draft_missing_call(0x80015C08u, slot,
                (uint32)(sint32)(sint16)r_u16(0x8009CC80u + slot * 4u),
                (uint32)(sint32)(sint16)r_u16(0x8009CC82u + slot * 4u));
            moh_draft_missing_call(0x800157BCu, slot, (uint32)r_u16(0x8009CCE0u + slot * 2u));
        }
        w_u32(0x8009CC68u, 0u);
    }
    if (r_u32(0x8009CC6Cu) != 0u) w_u32(0x8009CC6Cu, 0u);
    w_u32(0x80099ED0u, 2u);
    while ((sint32)r_u32(0x80099ED0u) < (sint32)r_u32(0x8009D310u)) {
        slot = r_u32(0x80099ED0u);
        level_pause_copy_record(0x80099F4Cu + slot * 32u, source);
        source -= 32u;
        w_u32(0x80099ED0u, slot + 1u);
    }
    w_u32(0x8009A61Cu, 0u);
    return 0u;
}

void level_sub_80090D38(void)
{
    w_u32(0x8009A5C0u, r_u32(0x8009A5C0u) + 1u);
    if (r_u32(0x800AB8E4u) == 0u && (r_u32(0x800AB8E0u) & 1u) != 0u) {
        uint32 buffer;
        w_u32(0x800AB8E4u, 1u);
        w_u32(0x800AB8E0u, 0u);
        buffer = r_u32(0x8009C824u) == 0u;
        w_u32(0x8009C824u, buffer);
        moh_native_put_disp_env(0x8009A7A0u + buffer * 20u);
    }
}

static void level_loading_bar(uint32 width)
{
    PSX_RECT rectangle = {142, 207, (sint16)width, 6};
    ClearImage(&rectangle, r_u8(0x80039D27u), r_u8(0x80039D28u), r_u8(0x80039D29u));
    DrawSync(0);
}

void level_sub_800640B4(void)
{
    uint32 mode;
    FUNCTION_MARKER(0x800640B4u, "LEVEL.BIN");
    /* TODO Link the level loading and frame services through the native dispatcher */
    if (!moh_draft_missing_call(0x8008B3D4u, 30u)) {
        moh_draft_missing_call(0x8008B3DCu);
        return;
    }
    while (!(r_u32(0x8009C840u) & 0xF1u)) {
        while (!(r_u32(0x8009C840u) & 0xF9u)) {
            moh_draft_missing_call(0x8006240Cu);
            level_loading_bar(60u);
            moh_draft_missing_call(0x8006B894u);
            level_loading_bar(180u);
            while (!(r_u32(0x8009C840u) & 0xF5u)) {
                if (!(r_u32(0x8009C840u) & 2u)) {
                    moh_draft_missing_call(0x80061E14u);
                    level_loading_bar(241u);
                } else {
                    PSX_RECT rectangle;
                    moh_draft_missing_call(0x800624ACu);
                    rectangle.x = (sint16)r_u16(0x8003C5E0u);
                    rectangle.y = (sint16)r_u16(0x8003C5E2u);
                    rectangle.w = (sint16)r_u16(0x8003C5E4u);
                    rectangle.h = (sint16)r_u16(0x8003C5E6u);
                    ClearImage(&rectangle, 0u, 0u, 0u);
                    DrawSync(0);
                }
                while (!(r_u32(0x8009C840u) & 0xF7u)) {
                    if (!moh_draft_missing_call(0x800625BCu)) {
                        moh_draft_missing_call(0x800616D4u);
                        if (r_u32(0x8009C840u) & 0xF5u) moh_draft_missing_call(0x800663B8u);
                    } else {
                        moh_draft_missing_call(0x8004137Cu);
                    }
                }
            }
        }
        if (r_u32(0x8009C840u) & 8u) {
            moh_draft_missing_call(0x80062298u, 1u);
            w_u32(0x8009C840u, r_u32(0x8009C840u) | 0x20u);
        } else if (r_u32(0x8009C840u) & 4u) {
            moh_draft_missing_call(0x80062298u, 4u);
            w_u32(0x8009C840u, r_u32(0x8009C840u) | 0x20u);
        }
    }
    if (r_u32(0x8009C840u) & 1u) {
        moh_draft_missing_call(0x8008B3C4u);
        mode = 3u;
    } else if (r_u32(0x8009C840u) & 16u) {
        moh_draft_missing_call(0x8008B3CCu);
        mode = 4u;
    } else {
        if (!(r_u32(0x8009C840u) & 0xC0u)) return;
        mode = 2u;
    }
    moh_draft_missing_call(0x80062298u, mode);
    w_u32(0x8009C840u, r_u32(0x8009C840u) | 0x60u);
}

void level_sub_800502B4(uint32 display, uint32 width, uint32 height, uint32 depth)
{
    sint32 secondary_x = 0, secondary_y = 0;
    FUNCTION_MARKER(0x800502B4u, "LEVEL.BIN");
    printf("%s", (const char *)moh_draft_guest_pointer(0x8003A390u, 1u));
    printf("%s", (const char *)moh_draft_guest_pointer(0x8003A3A8u, 1u));
    w_u32(display + 8u, depth);
    w_u32(display, width);
    w_u32(display + 4u, height);
    level_sub_800501B8(display);
    printf("%s", (const char *)moh_draft_guest_pointer(0x8003A3BCu, 1u));
    InitGeom();
    SetGeomOffset((sint32)r_u32(display) / 2, (sint32)r_u32(display + 4u) / 2);
    SetGeomScreen((sint32)r_u32(display + 8u));
    if (r_u32(display + 4u) == 480u) {
        if ((sint32)r_u32(display) < 321 && !r_u32(0x80097FD4u)) secondary_x = (sint32)r_u32(display);
    } else {
        secondary_y = 240;
    }
    level_sub_800501CC(display, secondary_x, secondary_y);
    w_u32(0x8009C83Cu, 0u);
    w_u32(0x8009C824u, 0u);
    w_u32(0x80097FCCu, moh_draft_missing_call(0x80015DB8u, (uint32)-1));
}

void level_sub_8007B780(void)
{
    static const uint32 first_names[5] = {0x8003D0D8u, 0x8003D068u, 0x8003D084u, 0x8003D0A0u, 0x8003D0BCu};
    static const uint32 second_names[5] = {0x8003D188u, 0x8003D118u, 0x8003D134u, 0x8003D150u, 0x8003D16Cu};
    uint32 first = r_u32(0x800386D4u), second = r_u32(0x800386D8u), name;
    FUNCTION_MARKER(0x8007B780u, "LEVEL.BIN");
    name = first_names[first >= 1u && first <= 4u ? first : 0u];
    printf("%s", (const char *)moh_draft_guest_pointer(name, 1u));
    name = r_u32(0x800386A8u) ? 0x8003D0F4u : 0x8003D108u;
    printf("%s", (const char *)moh_draft_guest_pointer(name, 1u));
    name = second_names[second >= 1u && second <= 4u ? second : 0u];
    printf("%s", (const char *)moh_draft_guest_pointer(name, 1u));
    name = r_u32(0x800386BCu) ? 0x8003D0F4u : 0x8003D108u;
    printf("%s", (const char *)moh_draft_guest_pointer(name, 1u));
}

void level_sub_8008EA70(void)
{
    uint32 slot;
    FUNCTION_MARKER(0x8008EA70u, "LEVEL.BIN");
    w_u32(0x800AB8C8u, 0u);
    w_u32(0x800AB8CCu, 0u);
    w_u32(0x800AB8D0u, 0u);
    for (slot = 0u; slot < 5u; ++slot) {
        /* TODO Link the per-slot initialization services */
        moh_draft_missing_call(0x8008EDDCu, slot);
        moh_draft_missing_call(0x8008EC14u, slot);
    }
}

void level_sub_80060548(void)
{
    uint32 angle, first = 0x800AD1C0u, second = 0x800AD210u;
    sint32 cosine_a, sine_a, cosine_b, sine_b;
    uint16 x_a, y_a, x_b, y_b;
    FUNCTION_MARKER(0x80060548u, "LEVEL.BIN");
    for (angle = 0u; angle < 4096u; angle += 1024u) {
        cosine_a = rcos((sint32)(angle + 512u));
        sine_a = rsin((sint32)(angle + 512u));
        cosine_b = rcos((sint32)(angle + 1536u));
        sine_b = rsin((sint32)(angle + 1536u));
        w_u8(first + 3u, 4u); w_u8(first + 7u, 0x20u);
        w_u8(second + 3u, 4u); w_u8(second + 7u, 0x20u);
        w_u8(first + 4u, 64u); w_u8(first + 5u, 0u); w_u8(first + 6u, 0u);
        w_u8(second + 4u, 64u); w_u8(second + 5u, 0u); w_u8(second + 6u, 0u);
        w_u8(first + 7u, r_u8(first + 7u) | 2u);
        w_u16(second + 8u, 60u);
        w_u8(second + 7u, r_u8(second + 7u) | 2u);
        w_u16(first + 8u, 60u);
        w_u16(second + 10u, 44u); w_u16(first + 10u, 44u);
        x_a = (uint16)(((uint32)((sint32)((uint32)sine_a * 20u) >> 12) << 1) + 60u);
        y_a = (uint16)(44u - (uint32)((sint32)((uint32)cosine_a * 20u) >> 12));
        x_b = (uint16)(((uint32)((sint32)((uint32)sine_b * 20u) >> 12) << 1) + 60u);
        y_b = (uint16)(44u - (uint32)((sint32)((uint32)cosine_b * 20u) >> 12));
        w_u16(second + 12u, x_a); w_u16(first + 12u, x_a);
        w_u16(second + 14u, y_a); w_u16(first + 14u, y_a);
        w_u16(second + 16u, x_b); w_u16(first + 16u, x_b);
        w_u16(second + 18u, y_b); w_u16(first + 18u, y_b);
        first += 20u; second += 20u;
    }
}
