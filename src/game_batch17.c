#include "game_level2p_signatures.h"
#include "xport_trace.h"
#include "psx.h"

static void moh_quad_matrix_native(uint32 matrix)
{
    uint32 a, b, c;
    a = r_u32(matrix); b = r_u32(matrix + 4); xport_gte_write_control(0, a); xport_gte_write_control(1, b);
    a = r_u32(matrix + 8); b = r_u32(matrix + 12); c = r_u32(matrix + 16);
    xport_gte_write_control(2, a); xport_gte_write_control(3, b); xport_gte_write_control(4, c);
}

static void moh_quad_axis_native(uint32 scalar, uint32 axis, uint32 output)
{
    uint32 a, b, c;
    xport_gte_write_data(8, scalar); a = r_u16(axis); b = r_u16(axis + 2); c = r_u16(axis + 4);
    xport_gte_write_data(9, a); xport_gte_write_data(10, b); xport_gte_write_data(11, c); xport_gte_execute(0x198003D);
    a = xport_gte_read_data(9); b = xport_gte_read_data(10); c = xport_gte_read_data(11);
    w_u16(output, a); w_u16(output + 2, b); w_u16(output + 4, c);
}

static void moh_quad_axis_sum_native(uint32 output)
{
    uint32 x, y, z, a, b, c;
    x = r_u16(output); a = r_u16(0x1F8001FC); b = r_u16(0x1F8001FE); c = r_u16(0x1F800200);
    w_u16(output, x + a); y = r_u16(output + 2); z = r_u16(output + 4);
    w_u16(output + 2, y + b); w_u16(output + 4, z + c);
}

uint32 level2p_sub_80069270(uint32 position, uint32 scale, uint32 angle, uint32 color, uint32 texture, uint32 scene, uint32 packet, uint32 period, uint32 depth_bias, uint32 vertical_scale, uint32 centered)
{
    uint32 matrix[8], scale_vector[3]; uint16 edges[8];
    uint32 matrix_view, scale_view, edges_view, table, value, a, b, c, x, y, z, x_second, y_first, y_second;
    uint32 u, v, offset, depth, page, first, second, slot, sx, sy, dx_first, dx_second, dy_first, dy_second, result = 0;
    FUNCTION_MARKER(0x80069270, "LEVEL2P.BIN");
    period &= 0xFF; if (!packet) return 0;
    table = r_u32(scene + 0x2218);
    /* TODO: Native semantic matrix/scale/edge mappings remain fail-fast; untouched padding stays opaque */
    /* TODO: Original packet, texture, scene and ordering-table spans are unchecked */
    edges_view = moh_draft_local_address(edges, sizeof(edges));
    value = r_u16(texture + 2);
    if (centered) {
        value >>= 1; w_u16(edges_view + 8, value); w_u16(edges_view, value); value = r_u16(texture + 4);
        w_u16(edges_view + 12, 0); w_u16(edges_view + 4, 0); value >>= 1;
        w_u16(edges_view + 10, value); w_u16(edges_view + 2, 0 - value);
    } else {
        w_u16(edges_view + 2, 0); w_u16(edges_view + 8, value); w_u16(edges_view, value); value = r_u16(texture + 4);
        w_u16(edges_view + 12, 0); w_u16(edges_view + 4, 0); w_u16(edges_view + 10, value);
    }
    scale_vector[0] = scale; scale_vector[1] = vertical_scale ? vertical_scale : scale; scale_vector[2] = 0;
    matrix_view = moh_draft_local_address(matrix, sizeof(matrix));
    w_u16(matrix_view + 16, 0x1000); w_u16(matrix_view + 8, 0x1000); w_u16(matrix_view, 0x1000);
    w_u16(matrix_view + 14, 0); w_u16(matrix_view + 12, 0); w_u16(matrix_view + 10, 0);
    w_u16(matrix_view + 6, 0); w_u16(matrix_view + 4, 0); w_u16(matrix_view + 2, 0);
    moh_draft_missing_call(0x8001C768, angle, matrix_view); scale_view = moh_draft_local_address(scale_vector, sizeof(scale_vector));
    moh_draft_missing_call(0x800165D8, matrix_view, scale_view); moh_quad_matrix_native(matrix_view);
    a = r_u32(edges_view); xport_gte_write_data(0, a); a = r_u32(edges_view + 4); xport_gte_write_data(1, a); xport_gte_execute(0x486012);
    moh_draft_missing_call(0x8001B9F8, packet);
    a = xport_gte_read_data(9); b = xport_gte_read_data(10); c = xport_gte_read_data(11);
    w_u16(edges_view, a); w_u16(edges_view + 2, b); w_u16(edges_view + 4, c);
    a = r_u32(edges_view + 8); xport_gte_write_data(0, a); a = r_u32(edges_view + 12); xport_gte_write_data(1, a); xport_gte_execute(0x486012);
    value = r_u8(texture + 0x14); if (value) moh_draft_missing_call(0x8001BA38, packet, 1);
    a = xport_gte_read_data(9); b = xport_gte_read_data(10); c = xport_gte_read_data(11);
    w_u16(edges_view + 8, a); w_u16(edges_view + 10, b); w_u16(edges_view + 12, c);
    y_first = (uint32)(sint32)r_s16(edges_view + 2); y_second = (uint32)(sint32)r_s16(edges_view + 10);
    x_second = (uint32)(sint32)r_s16(edges_view + 8);
    y_first = (uint32)((sint32)(y_first * 1536) / 960); y_second = (uint32)((sint32)(y_second * 1536) / 960);
    x = (uint32)(sint32)r_s16(edges_view);
    moh_quad_axis_native(x, scene + 0x21E4, 0x1F8001EC); moh_quad_axis_native(y_first, scene + 0x21EA, 0x1F8001FC);
    moh_quad_axis_sum_native(0x1F8001EC);
    moh_quad_axis_native(x_second, scene + 0x21E4, 0x1F8001F4); moh_quad_axis_native(y_second, scene + 0x21EA, 0x1F8001FC);
    moh_quad_axis_sum_native(0x1F8001F4);
    x = (uint32)(sint32)r_s16(position + 2); y = (uint32)(sint32)r_s16(position + 6); z = (uint32)(sint32)r_s16(position + 10);
    w_u16(0x1F8001CC, x); w_u16(0x1F8001CE, y); w_u16(0x1F8001D0, z);
    a = r_u16(0x1F8001EC); b = r_u16(0x1F8001EE); w_u16(0x1F8001EC, a + x);
    a = r_u16(0x1F8001F0); w_u16(0x1F8001EE, b + y); b = r_u16(0x1F8001F4); w_u16(0x1F8001F0, a + z);
    a = r_u16(0x1F8001F6); w_u16(0x1F8001F4, b + x); b = r_u16(0x1F8001F8);
    w_u16(0x1F8001F6, a + y); w_u16(0x1F8001F8, b + z);
    moh_quad_matrix_native(scene + 0x21E4);
    a = r_u32(scene + 0x21F8); b = r_u32(scene + 0x21FC); xport_gte_write_control(5, a); c = r_u32(scene + 0x2200);
    xport_gte_write_control(6, b); xport_gte_write_control(7, c);
    a = r_u32(0x1F8001CC); xport_gte_write_data(0, a); a = r_u32(0x1F8001D0); xport_gte_write_data(1, a);
    a = r_u32(0x1F8001EC); xport_gte_write_data(2, a); a = r_u32(0x1F8001F0); xport_gte_write_data(3, a);
    a = r_u32(0x1F8001F4); xport_gte_write_data(4, a); a = r_u32(0x1F8001F8); xport_gte_write_data(5, a); xport_gte_execute(0x280030);
    value = r_u8(texture + 0x15);
    if (value >= 2) {
        a = r_u8(texture + 0x16); b = r_u16(texture + 10); c = r_u16(texture + 6); u = ((c + a * b) & 0x3F) << 2; w_u8(packet + 12, u);
        v = r_u8(texture + 8); w_u8(packet + 13, v); offset = r_u8(texture + 0x10); w_u8(packet + 20, offset + u);
        v = r_u8(texture + 8); w_u8(packet + 28, u); w_u8(packet + 21, v);
        v = r_u8(texture + 8); offset = r_u8(texture + 0x12); w_u8(packet + 29, v + offset);
        offset = r_u8(texture + 0x10); w_u8(packet + 36, offset + u);
    } else {
        u = (r_u8(texture + 6) & 0x3F) << 2; w_u8(packet + 12, u); v = r_u8(texture + 8); w_u8(packet + 13, v);
        u = r_u8(texture + 6); offset = r_u8(texture + 0x10); w_u8(packet + 20, offset + ((u & 0x3F) << 2));
        v = r_u8(texture + 8); w_u8(packet + 21, v); u = (r_u8(texture + 6) & 0x3F) << 2; w_u8(packet + 28, u);
        v = r_u8(texture + 8); offset = r_u8(texture + 0x12); w_u8(packet + 29, v + offset);
        u = r_u8(texture + 6); offset = r_u8(texture + 0x10); w_u8(packet + 36, offset + ((u & 0x3F) << 2));
    }
    v = r_u8(texture + 8); offset = r_u8(texture + 0x12); w_u8(packet + 37, v + offset);
    a = r_u16(texture + 12); b = r_u16(texture + 14); value = moh_draft_missing_call(0x8001BA18, a, b); w_u16(packet + 14, value);
    value = xport_gte_read_control(31);
    if ((sint32)value >= 0) {
        value = xport_gte_read_data(19); w_u32(0x1F800198, value); depth = r_u32(0x1F800198); depth -= depth_bias + 128; w_u32(0x1F800198, depth);
        value = xport_gte_read_data(12); w_u32(0x1F8001BC, value); value = xport_gte_read_data(13); w_u32(0x1F8001C0, value);
        value = xport_gte_read_data(14); w_u32(0x1F8001C4, value); depth = r_u32(0x1F800198);
        if ((sint32)depth >= 0) {
            if (((sint32)depth >> 4) >= 512) w_u32(0x1F800198, 0x1FD0);
            value = r_u8(texture + 0x14); value = value ? value - 1 : 1;
            a = r_u16(texture + 8); b = r_u16(texture + 6);
            page = ((value & 3) << 5) | ((a & 0x100) >> 4) | ((b & 0x3C0) >> 6) | ((a & 0x200) << 2); w_u16(packet + 22, page);
            value = r_u8(color); dx_first = (uint32)(sint32)r_s16(0x1F8001C0); sx = r_u16(0x1F8001BC);
            dx_second = (uint32)(sint32)r_s16(0x1F8001C4); dy_first = (uint32)(sint32)r_s16(0x1F8001C2); sy = r_u16(0x1F8001BE);
            dy_second = (uint32)(sint32)r_s16(0x1F8001C6); sx = (uint32)(sint32)(sint16)sx;
            dx_first -= sx; dx_second -= sx; w_u8(packet + 4, value); value = r_u8(color + 1); sy = (uint32)(sint32)(sint16)sy; w_u8(packet + 5, value);
            value = r_u8(color + 2); dy_first -= sy; w_u8(packet + 6, value); dy_second -= sy;
            if (centered) {
                value = r_u16(0x1F8001BC); w_u16(packet + 8, value + dx_first); value = r_u16(0x1F8001BE); w_u16(packet + 10, value + dy_first);
                value = r_u16(0x1F8001BC); w_u16(packet + 16, value + dx_second); value = r_u16(0x1F8001BE); w_u16(packet + 18, value + dy_second);
                value = r_u16(0x1F8001BC); w_u16(packet + 24, value - dx_second); value = r_u16(0x1F8001BE); w_u16(packet + 26, value - dy_second);
                value = r_u16(0x1F8001BC); w_u16(packet + 32, value - dx_first); value = r_u16(0x1F8001BE); w_u16(packet + 34, value - dy_first);
            } else {
                value = r_u16(0x1F8001BC); w_u16(packet + 8, value); value = r_u16(0x1F8001BE); w_u16(packet + 10, value);
                value = r_u16(0x1F8001BC); w_u16(packet + 16, value + dx_first); value = r_u16(0x1F8001BE); w_u16(packet + 18, value);
                value = r_u16(0x1F8001BC); w_u16(packet + 24, value); value = r_u16(0x1F8001BE); w_u16(packet + 26, value + dy_second);
                value = r_u16(0x1F8001BC); w_u16(packet + 32, value + dx_second); value = r_u16(0x1F8001BE); w_u16(packet + 34, value + dy_second);
            }
            depth = r_u32(0x1F800198); first = r_u32(packet); slot = table + ((uint32)((sint32)depth >> 4) << 2); second = r_u32(slot);
            w_u32(packet, (first & 0xFF000000) | (second & 0xFFFFFF)); depth = r_u32(0x1F800198); result = 1;
            slot = table + ((uint32)((sint32)depth >> 4) << 2); second = r_u32(slot); w_u32(slot, (second & 0xFF000000) | (packet & 0xFFFFFF));
        }
    }
    value = r_u8(texture + 0x15);
    if (value >= 2) {
        value = r_u32(0x800968E8);
        if (!value) {
            value = r_u8(texture + 0x17) + 1; w_u8(texture + 0x17, value);
            if ((value & 0xFF) == period) {
                value = r_u8(texture + 0x16); a = r_u8(texture + 0x15); w_u8(texture + 0x17, 0); value += 1; w_u8(texture + 0x16, value);
                if ((value & 0xFF) == a) w_u8(texture + 0x16, 0);
            }
        }
    }
    return result;
}

static uint32 moh_effect_owner_native(uint32 effect)
{
    uint32 handle = r_u32(effect + 0x18);
    return handle ? r_u32(0x80099460 + (handle & 0xFFFF) * 4) : 0;
}

void level2p_sub_80068F0C(uint32 effect, uint32 scene)
{
    uint8 color[3]; uint16 point[4]; uint32 position[3], matrix[8], origin[3];
    uint32 life, value, angle, model, owner, flags, state, handle, primary, rotation, scale, bone;
    uint32 color_view, point_view, position_view, matrix_view = 0, origin_view, x, y, z, height, packet;
    FUNCTION_MARKER(0x80068F0C, "LEVEL2P.BIN");
    life = (uint32)(sint32)r_s16(effect + 2);
    if ((sint32)life <= 0) { w_u16(effect + 2, 0); return; }
    value = r_u8(effect + 2); color[0] = (uint8)(0xFFFFFF80 - (value << 5));
    value = r_u8(effect + 2); color[1] = (uint8)(0xFFFFFF80 - (value << 5));
    value = r_u8(effect + 2); color[2] = (uint8)(0xFFFFFF80 - (value << 5));
    value = r_u32(0x800993FC); model = r_u32(effect + 0x1C); angle = ((value & 7) << 9) + (life << 7);
    /* TODO: Native semantic color, point, vector and matrix mappings remain fail-fast */
    /* TODO: Original handle, owner, model and scene spans are unchecked */
    point_view = moh_draft_local_address(point, sizeof(point));
    if (model) { bone = r_u16(effect); level2p_sub_8008C61C(model, bone, point_view); }
    else {
        owner = moh_effect_owner_native(effect); flags = r_u32(owner + 12);
        if (flags & 1) {
            owner = moh_effect_owner_native(effect); state = r_u32(owner + 0x388); handle = r_u32(state + 0xA0);
            primary = handle ? r_u32(0x80099460 + (handle & 0xFFFF) * 4) : 0;
            origin_view = moh_draft_local_address(origin, sizeof(origin));
            x = r_u32(primary + 0x20C); w_u32(origin_view, x);
            state = r_u32(primary + 0x388); height = r_u32(state + 0x94); y = r_u32(primary + 0x210);
            w_u32(origin_view + 4, y - (height << 16)); z = r_u32(primary + 0x214); w_u32(origin_view + 8, z);
            owner = moh_effect_owner_native(effect); rotation = owner + 0x228;
            owner = moh_effect_owner_native(effect); scale = owner + 0x270;
            matrix_view = moh_draft_local_address(matrix, sizeof(matrix)); level2p_sub_8008BB44(origin_view, rotation, scale, matrix_view);
        }
        owner = moh_effect_owner_native(effect); bone = r_u16(effect);
        level2p_sub_8008C5E4(owner, bone, point_view, effect + 4, matrix_view);
    }
    position_view = moh_draft_local_address(position, sizeof(position));
    x = (uint32)(sint32)r_s16(point_view); z = (uint32)(sint32)r_s16(point_view + 4); w_u32(position_view, x << 16);
    y = (uint32)(sint32)r_s16(point_view + 2); w_u32(position_view + 8, z << 16); w_u32(position_view + 4, y << 16);
    value = r_u32(scene + 0x221C); packet = effect + 0x20 + value * 40;
    color_view = moh_draft_local_address(color, sizeof(color));
    level2p_sub_80069270(position_view, 0x800, angle, color_view, effect + 0x70, scene, packet, 0, 0, 0, 1);
    value = r_u16(effect + 2); w_u16(effect + 2, value - 1);
}
