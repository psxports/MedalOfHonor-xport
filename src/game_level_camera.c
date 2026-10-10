#include "game_draft_signatures.h"
#include "game_level_signatures.h"
#include "xport_trace.h"
#include "psx.h"

static uint32 moh_attachment_quaternion(uint32 quaternion, uint32 matrix)
{
    uint32 a, b, x, y, z, w, doubled_x, doubled_y, doubled_w, first[3], second[3], value;
    a = r_u32(quaternion); b = r_u32(quaternion + 4u);
    x = (uint32)(sint32)(sint16)a; y = (uint32)((sint32)a >> 16);
    z = (uint32)(sint32)(sint16)b; w = (uint32)((sint32)b >> 16);
    xport_gte_write_control(0u, (w << 16u) | (x & 0xFFFFu));
    value = (y << 16u) | (z & 0xFFFFu);
    xport_gte_write_control(1u, value); xport_gte_write_control(3u, value);
    xport_gte_write_control(2u, ((0u - z) & 0xFFFFu) | (w << 16u));
    xport_gte_write_control(4u, 0u - x);
    doubled_x = x + x; doubled_w = w + w; doubled_y = y + y;
    xport_gte_write_data(0u, (doubled_w << 16u) | (doubled_x & 0xFFFFu));
    xport_gte_write_data(1u, 0u); xport_gte_execute(0x486012u);
    first[0] = xport_gte_read_data(25u); first[1] = xport_gte_read_data(26u); first[2] = xport_gte_read_data(27u);
    xport_gte_write_data(0u, doubled_y & 0xFFFFu); xport_gte_write_data(1u, doubled_w);
    xport_gte_execute(0x486012u);
    w_u16(matrix, (uint16)(first[0] - 0x1000u)); w_u16(matrix + 6u, (uint16)first[1]);
    w_u16(matrix + 12u, (uint16)first[2]);
    second[0] = xport_gte_read_data(25u); second[1] = xport_gte_read_data(26u); second[2] = xport_gte_read_data(27u);
    xport_gte_write_data(0u, (0u - doubled_y) << 16u); xport_gte_write_data(1u, doubled_x);
    xport_gte_execute(0x486012u);
    w_u16(matrix + 2u, (uint16)second[0]); w_u16(matrix + 8u, (uint16)(second[1] - 0x1000u));
    w_u16(matrix + 14u, (uint16)second[2]);
    value = ((uint32)((sint32)(z * z) >> 12) << 1u) + ((uint32)((sint32)(w * w) >> 12) << 1u) - 0x1000u;
    a = xport_gte_read_data(25u); b = xport_gte_read_data(26u);
    w_u16(matrix + 4u, (uint16)a); w_u16(matrix + 10u, (uint16)b); w_u16(matrix + 16u, (uint16)value);
    return matrix;
}


uint32 level_sub_8008E354(uint32 object, uint32 bone, uint32 output, uint32 offset, uint32 matrix)
{
    FUNCTION_MARKER(0x8008E354u, "LEVEL.BIN");
    return moh_draft_missing_call(0x8008DABCu, object, r_u32(object + 0x100u), bone, output, offset, matrix);
}

static void moh_attachment_load_matrix(uint32 matrix)
{
    uint32 index;
    for (index = 0u; index < 8u; ++index) xport_gte_write_control(index, r_u32(matrix + index * 4u));
}

static void moh_attachment_save_matrix(void)
{
    MATRIX matrix;
    uint32 index, half, word, offset = r_u32(0x8009D320u);
    ReadRotMatrix(&matrix);
    for (index = 0u; index < 8u; ++index) {
        half = index * 2u;
        if (index < 4u) word = (uint16)matrix.m[half / 3u][half % 3u] |
            ((uint32)(uint16)matrix.m[(half + 1u) / 3u][(half + 1u) % 3u] << 16u);
        else if (index == 4u) word = (uint32)(sint32)matrix.m[2][2];
        else word = (uint32)matrix.t[index - 5u];
        w_u32(0x8009D324u + offset + index * 4u, word);
    }
    w_u32(0x8009D320u, offset + 32u);
}

static void moh_attachment_restore_matrix(void)
{
    uint32 offset = r_u32(0x8009D320u) - 32u;
    w_u32(0x8009D320u, offset); moh_attachment_load_matrix(0x8009D324u + offset);
}

static void moh_attachment_transform(uint32 output)
{
    uint32 x, y, z;
    xport_gte_write_data(0u, r_u32(output)); xport_gte_write_data(1u, r_u32(output + 4u));
    xport_gte_execute(0x480012u);
    x = xport_gte_read_data(25u); y = xport_gte_read_data(26u); z = xport_gte_read_data(27u);
    w_u16(output, (uint16)x); w_u16(output + 2u, (uint16)y); w_u16(output + 4u, (uint16)z);
}

static void moh_attachment_player_height(uint32 object, uint32 output)
{
    uint32 camera, state;
    if (r_u32(object + 8u) != 1u) return;
    camera = r_u32(0x8009D638u); state = r_u32(camera + 0x388u);
    w_u16(output + 2u, (uint16)(r_u16(output + 2u) + r_u16(state + 0x94u)));
}

uint32 level_sub_8008DABC(uint32 object, uint32 render, uint32 bone, uint32 output, uint32 offset, uint32 matrix)
{
    uint32 axis, source, local, animated;
    FUNCTION_MARKER(0x8008DABCu, "LEVEL.BIN");
    if (!render) return output;
    animated = r_u32(render + 0x84u);
    if (!animated) {
        for (axis = 0u; axis < 3u; ++axis)
            w_u16(output + axis * 2u, r_u16(r_u32(render + 0x2Cu) + bone * 8u + axis * 2u));
        moh_attachment_player_height(object, output);
    }
    moh_attachment_save_matrix();
    local = sub_80016718(48u);
    if (!local) moh_draft_fail_fast("Attachment matrix allocation", 0x8008DABCu);
    if (animated) {
        if (offset) {
            source = r_u32(render + 0x88u);
            if (!source) { source = r_u32(render + 0x8Cu); if (!source) source = r_u32(render + 0x90u); }
            moh_attachment_quaternion(source + bone * 8u, local);
            /* TODO Original unused matrix padding is unspecified */
            for (axis = 0u; axis < 3u; ++axis)
                w_u32(local + 20u + axis * 4u, (uint32)(sint32)(sint16)r_u16(r_u32(render + 0x94u) + bone * 8u + axis * 2u));
            moh_attachment_load_matrix(local);
            xport_gte_write_data(0u, r_u32(offset)); xport_gte_write_data(1u, r_u32(offset + 4u));
            xport_gte_execute(0x480012u);
            for (axis = 0u; axis < 3u; ++axis) w_u16(local + 32u + axis * 2u, (uint16)xport_gte_read_data(25u + axis));
            for (axis = 0u; axis < 3u; ++axis) w_u16(output + axis * 2u, r_u16(local + 32u + axis * 2u));
        } else {
            for (axis = 0u; axis < 3u; ++axis)
                w_u16(output + axis * 2u, r_u16(r_u32(render + 0x94u) + bone * 8u + axis * 2u));
        }
        moh_attachment_load_matrix(matrix ? matrix : object + 0x340u);
        moh_attachment_transform(output); moh_attachment_player_height(object, output);
        moh_attachment_restore_matrix();
    } else {
        moh_attachment_load_matrix(matrix ? matrix : object + 0x340u);
        moh_draft_missing_call(0x80016548u, output, local + 32u, local + 44u);
        moh_attachment_restore_matrix();
        for (axis = 0u; axis < 3u; ++axis) w_u16(output + axis * 2u, r_u16(local + 32u + axis * 4u));
    }
    sub_80036E44(local); return output;
}

void level_sub_80044E0C(uint32 object, uint32 anchor, uint32 first_bone, uint32 second_bone,
    uint32 model, uint32 first_offset, uint32 second_offset, uint32 first_out, uint32 second_out)
{
    FUNCTION_MARKER(0x80044E0Cu, "LEVEL.BIN");
    if (model) {
        moh_draft_missing_call(0x8008E38Cu, model, second_bone, second_out);
        moh_draft_missing_call(0x8008E38Cu, model, first_bone, first_out);
    } else {
        level_sub_8008E354(object, second_bone, second_out, second_offset, 0u);
        level_sub_8008E354(object, first_bone, first_out, first_offset, 0u);
    }
    w_u32(anchor, (uint32)(sint32)(sint16)r_u16(second_out) << 16u);
    w_u32(anchor + 4u, (uint32)(sint32)(sint16)r_u16(second_out + 2u) << 16u);
    w_u32(anchor + 8u, (uint32)(sint32)(sint16)r_u16(second_out + 4u) << 16u);
}

static void moh_ray_copy_pair(uint32 destination, uint32 source)
{
    uint32 first = r_u32(source), second = r_u32(source + 4u);
    w_u32(destination, first); w_u32(destination + 4u, second);
}

static sint32 moh_ray_triangle_edge(uint32 context, uint32 vertex_offset, uint32 next_offset)
{
    uint32 vertex, next, point[3], origin[3], endpoint[3], cross[3], axis, normal;
    vertex = r_u32(context + vertex_offset);
    for (axis = 0u; axis < 3u; ++axis) { point[axis] = r_u16(context + 16u + axis * 2u); origin[axis] = r_u16(vertex + axis * 2u); }
    w_u16(context + 32u, (uint16)(point[0] - origin[0]));
    w_u16(context + 36u, (uint16)(point[2] - origin[2])); w_u16(context + 34u, (uint16)(point[1] - origin[1]));
    vertex = r_u32(context + vertex_offset); next = r_u32(context + next_offset);
    for (axis = 0u; axis < 3u; ++axis) { endpoint[axis] = r_u16(next + axis * 2u); origin[axis] = r_u16(vertex + axis * 2u); }
    w_u16(context + 40u, (uint16)(endpoint[0] - origin[0]));
    w_u16(context + 44u, (uint16)(endpoint[2] - origin[2])); w_u16(context + 42u, (uint16)(endpoint[1] - origin[1]));
    for (axis = 0u; axis < 3u; ++axis) xport_gte_write_control(axis * 2u, (uint32)(sint32)(sint16)r_u16(context + 32u + axis * 2u));
    for (axis = 0u; axis < 3u; ++axis) xport_gte_write_data(9u + axis, (uint32)(sint32)(sint16)r_u16(context + 40u + axis * 2u));
    xport_gte_execute(0x178000Cu);
    for (axis = 0u; axis < 3u; ++axis) cross[axis] = xport_gte_read_data(9u + axis);
    for (axis = 0u; axis < 3u; ++axis) w_u16(context + 48u + axis * 2u, (uint16)cross[axis]);
    normal = r_u32(context + 96u);
    xport_gte_write_control(0u, r_u32(normal)); xport_gte_write_control(1u, r_u32(normal + 4u));
    for (axis = 0u; axis < 3u; ++axis) xport_gte_write_data(9u + axis, r_u16(context + 48u + axis * 2u));
    xport_gte_execute(0x41E012u); return (sint32)xport_gte_read_data(25u);
}

uint32 level_sub_8003DC10(uint32 context)
{
    SVECTOR scaled_input;
    VECTOR scaled_output;
    uint32 index = 0u, triangle, bounds, vertex, normal, axis, first, second, fraction, denominator, numerator;
    uint32 hit, material, list, output, values[3];
    sint32 start_y, end_y, minimum, maximum, value, plane_start, plane_end;
    FUNCTION_MARKER(0x8003DC10u, "LEVEL.BIN");
    start_y = (sint16)r_u16(context + 2u); end_y = (sint16)r_u16(context + 10u);
    if (start_y < end_y) { w_u32(context + 88u, (uint32)start_y); maximum = end_y; }
    else { maximum = (sint16)r_u16(context + 2u); w_u32(context + 88u, (uint32)end_y); }
    w_u32(context + 92u, (uint32)maximum);
    first = r_u16(context); second = r_u16(context + 8u);
    values[1] = r_u16(context + 10u); values[2] = r_u16(context + 12u);
    w_u16(context + 24u, (uint16)(second - first));
    first = r_u16(context + 4u); second = r_u16(context + 2u);
    w_u16(context + 28u, (uint16)(values[2] - first)); w_u16(context + 26u, (uint16)(values[1] - second));
    first = r_u32(context + 76u); w_u32(context + 84u, 0xFFFFFFFFu); w_u32(context + 80u, first);
    while ((sint32)index < (sint32)r_u32(context + 64u)) {
        triangle = r_u32(context + 56u) + (uint32)(sint32)(sint16)r_u16(r_u32(context + 60u) + index * 2u) * 10u;
        bounds = r_u32(0x800AB8F8u);
        normal = r_u32(bounds + 32u) + (uint32)(sint32)(sint16)r_u16(triangle + 6u) * 8u;
        w_u32(context + 96u, normal);
        for (axis = 0u; axis < 3u; ++axis) {
            vertex = r_u32(bounds + 28u) + (uint32)(sint32)(sint16)r_u16(triangle + axis * 2u) * 8u;
            w_u32(context + 100u + axis * 4u, vertex);
        }
        minimum = (sint16)r_u16(r_u32(context + 100u) + 2u);
        maximum = (sint16)r_u16(r_u32(context + 104u) + 2u);
        if (minimum > maximum) { value = minimum; minimum = maximum; maximum = value; }
        value = (sint16)r_u16(r_u32(context + 108u) + 2u);
        if (value < minimum) minimum = value; else if (maximum < value) maximum = value;
        if (maximum < (sint32)r_u32(context + 88u) || (sint32)r_u32(context + 92u) < minimum) { ++index; continue; }
        normal = r_u32(context + 96u);
        xport_gte_write_control(0u, r_u32(context)); xport_gte_write_control(1u, r_u32(context + 4u));
        xport_gte_write_control(3u, r_u32(context + 8u)); xport_gte_write_control(4u, r_u32(context + 12u));
        for (axis = 0u; axis < 3u; ++axis) xport_gte_write_data(9u + axis, r_u16(normal + axis * 2u));
        xport_gte_execute(0x41E012u);
        first = xport_gte_read_data(25u); second = xport_gte_read_data(27u);
        plane_start = (sint32)((uint32)((sint32)first >> 15) + (uint32)(sint32)(sint16)r_u16(triangle + 8u));
        plane_end = (sint32)((uint32)((sint32)second >> 15) + (uint32)(sint32)(sint16)r_u16(triangle + 8u));
        if (plane_start < 0 || (sint32)((uint32)plane_start ^ (uint32)plane_end) >= 0) { ++index; continue; }
        first = (uint32)plane_start; second = plane_end < 0 ? 0u - (uint32)plane_end : (uint32)plane_end;
        denominator = first + second; numerator = first << 12u;
        if (!denominator || (denominator == 0xFFFFFFFFu && numerator == 0x80000000u))
            moh_draft_fail_fast("Ray triangle division trap", 0x8003DC10u);
        fraction = (uint32)((sint32)numerator / (sint32)denominator);
        if ((sint32)fraction >= (sint32)r_u32(context + 80u)) { ++index; continue; }
        xport_gte_write_data(8u, fraction);
        for (axis = 0u; axis < 3u; ++axis) xport_gte_write_data(9u + axis, r_u16(context + 24u + axis * 2u));
        scaled_input.vx = (sint16)xport_gte_read_data(9u);
        scaled_input.vy = (sint16)xport_gte_read_data(10u);
        scaled_input.vz = (sint16)xport_gte_read_data(11u); scaled_input.pad = 0;
        gte_gpf12(&scaled_input, (sint32)xport_gte_read_data(8u), &scaled_output);
        for (axis = 0u; axis < 3u; ++axis) values[axis] = xport_gte_read_data(9u + axis);
        for (axis = 0u; axis < 3u; ++axis) w_u16(context + 16u + axis * 2u, (uint16)values[axis]);
        for (axis = 0u; axis < 3u; ++axis)
            w_u16(context + 16u + axis * 2u, (uint16)(r_u16(context + 16u + axis * 2u) + r_u16(context + axis * 2u)));
        if (moh_ray_triangle_edge(context, 100u, 104u) < 0 || moh_ray_triangle_edge(context, 104u, 108u) < 0 ||
            moh_ray_triangle_edge(context, 108u, 100u) < 0) { ++index; continue; }
        list = r_u32(context + 60u); material = r_u32(context + 112u);
        w_u32(context + 80u, fraction); w_u32(context + 76u, fraction);
        first = (uint32)(sint32)(sint16)r_u16(list + index * 2u); hit = r_u32(context + 72u); w_u32(context + 84u, first);
        moh_ray_copy_pair(hit, context + 16u); moh_ray_copy_pair(material, hit);
        output = r_u32(context + 68u); normal = r_u32(context + 96u); material = r_u32(context + 112u);
        moh_ray_copy_pair(output, normal); moh_ray_copy_pair(material + 8u, output);
        output = r_u32(context + 68u); w_u16(output + 6u, r_u16(triangle + 8u));
        for (axis = 0u; axis < 3u; ++axis) {
            material = r_u32(context + 112u); vertex = r_u32(context + 100u + axis * 4u);
            moh_ray_copy_pair(material + 16u + axis * 8u, vertex);
        }
        ++index;
    }
    return r_u32(context + 84u);
}

typedef struct MohRayBranch {
    sint16 node, minimum_x, maximum_x, minimum_z, maximum_z;
    uint32 start_x, end_x, start_z, end_z;
} MohRayBranch;

static uint32 moh_ray_split_fraction(uint32 split, uint32 start, uint32 end)
{
    uint32 denominator = end - start, numerator = (split - start) << 16u;
    if (!denominator) return 0u;
    if (denominator == 0xFFFFFFFFu && numerator == 0x80000000u)
        moh_draft_fail_fast("Ray split division overflow", 0x8003E3F0u);
    return (uint32)((sint32)numerator / (sint32)denominator);
}

uint32 level_sub_8003E3F0(uint32 start, uint32 end, uint32 normal_out, uint32 hit_out,
    uint32 distance_out, uint32 material_out, uint32 flags)
{
    MohRayBranch branches[8], *branch;
    uint32 depth = 0u, bounds, root, node, first, second, sx, ex, sz, ez, split, fraction, interpolated;
    uint32 minimum_x, maximum_x, minimum_z, maximum_z, result, counter, child;
    sint32 leaf;
    FUNCTION_MARKER(0x8003E3F0u, "LEVEL.BIN");
    first = r_u32(start); second = r_u32(start + 4u);
    w_u32(0x1F800000u, first); w_u32(0x1F800004u, second);
    first = r_u32(end); second = r_u32(end + 4u);
    w_u32(0x1F800008u, first); w_u32(0x1F80000Cu, second);
    sx = (uint32)(sint32)(sint16)r_u16(0x1F800000u);
    ex = (uint32)(sint32)(sint16)r_u16(0x1F800008u);
    sz = (uint32)(sint32)(sint16)r_u16(0x1F800004u);
    ez = (uint32)(sint32)(sint16)r_u16(0x1F80000Cu);
    bounds = r_u32(0x800AB8F8u); first = r_u32(bounds + 0x24u);
    w_u32(0x1F800044u, normal_out); w_u32(0x1F800048u, hit_out);
    w_u32(0x1F800070u, material_out); w_u32(0x1F800074u, flags); w_u32(0x1F800038u, first);
    minimum_x = (uint32)(sint32)(sint16)r_u16(bounds);
    maximum_x = (uint32)(sint32)(sint16)r_u16(bounds + 4u);
    minimum_z = (uint32)(sint32)(sint16)r_u16(bounds + 2u);
    maximum_z = (uint32)(sint32)(sint16)r_u16(bounds + 6u);
    root = r_u32(bounds + 0x14u); node = root; w_u32(0x1F80004Cu, 0x7FFFu);
    for (;;) {
        leaf = (sint16)r_u16(node);
        if (leaf < 0) {
            child = (uint32)(sint32)(sint16)r_u16(node + 2u);
            bounds = r_u32(0x800AB8F8u); counter = r_u32(0x80090E88u);
            first = r_u32(bounds + 0x18u) + child * 2u;
            w_u32(0x1F80003Cu, first);
            second = (uint32)(sint32)(sint16)r_u16(node);
            w_u32(0x80090E88u, counter - 1u - (uint32)leaf); w_u32(0x1F800040u, ~second);
            /* Native C ABI omits the original temporary CPU stack switch */
            result = moh_draft_missing_call(0x8003DC10u, 0x1F800000u);
            if ((sint32)result >= 0 || !depth) {
                w_u32(distance_out, r_u32(0x1F80004Cu)); return (sint32)result >= 0 ? result : 0xFFFFFFFFu;
            }
            branch = &branches[--depth]; minimum_x = (uint32)(sint32)branch->minimum_x;
            maximum_x = (uint32)(sint32)branch->maximum_x; minimum_z = (uint32)(sint32)branch->minimum_z;
            maximum_z = (uint32)(sint32)branch->maximum_z;
            sx = branch->start_x; ex = branch->end_x; sz = branch->start_z; ez = branch->end_z;
            node = root + (uint32)(sint32)branch->node * 8u; continue;
        }
        if ((sint16)r_u16(node + 2u) >= 0) {
            split = minimum_x + r_u16(node + 4u);
            if ((sint32)sx < (sint32)split) {
                if ((sint32)split < (sint32)ex && depth < 8u) {
                    fraction = moh_ray_split_fraction(split, sx, ex);
                    interpolated = sz + (uint32)((sint32)((ez - sz) * fraction) >> 16);
                    branch = &branches[depth++]; branch->minimum_x = (sint16)split;
                    branch->maximum_x = (sint16)maximum_x; branch->minimum_z = (sint16)minimum_z;
                    branch->maximum_z = (sint16)maximum_z; branch->node = (sint16)r_u16(node + 2u);
                    branch->start_x = split; branch->end_x = ex; branch->start_z = interpolated; branch->end_z = ez;
                    ex = split; ez = interpolated;
                }
                maximum_x = split - 1u; child = (uint32)(sint32)(sint16)r_u16(node);
            } else {
                if ((sint32)ex < (sint32)split && depth < 8u) {
                    fraction = moh_ray_split_fraction(split, sx, ex);
                    interpolated = sz + (uint32)((sint32)((ez - sz) * fraction) >> 16);
                    branch = &branches[depth++]; branch->minimum_x = (sint16)minimum_x;
                    branch->maximum_x = (sint16)(split - 1u); branch->minimum_z = (sint16)minimum_z;
                    branch->maximum_z = (sint16)maximum_z; branch->node = (sint16)r_u16(node);
                    branch->start_x = split; branch->end_x = ex; branch->start_z = interpolated; branch->end_z = ez;
                    ex = split; ez = interpolated;
                }
                minimum_x = split; child = (uint32)(sint32)(sint16)r_u16(node + 2u);
            }
        } else {
            split = minimum_z + r_u16(node + 4u);
            if ((sint32)sz < (sint32)split) {
                if ((sint32)split < (sint32)ez && depth < 8u) {
                    fraction = moh_ray_split_fraction(split, sz, ez);
                    interpolated = sx + (uint32)((sint32)((ex - sx) * fraction) >> 16);
                    branch = &branches[depth++]; branch->minimum_x = (sint16)minimum_x;
                    branch->maximum_x = (sint16)maximum_x; branch->minimum_z = (sint16)split;
                    branch->maximum_z = (sint16)maximum_z; branch->node = (sint16)~r_u16(node + 2u);
                    branch->start_x = interpolated; branch->end_x = ex; branch->start_z = split; branch->end_z = ez;
                    ex = interpolated; ez = split;
                }
                maximum_z = split - 1u; child = (uint32)(sint32)(sint16)r_u16(node);
            } else {
                if ((sint32)ez < (sint32)split && depth < 8u) {
                    fraction = moh_ray_split_fraction(split, sz, ez);
                    interpolated = sx + (uint32)((sint32)((ex - sx) * fraction) >> 16);
                    branch = &branches[depth++]; branch->minimum_x = (sint16)minimum_x;
                    branch->maximum_x = (sint16)maximum_x; branch->minimum_z = (sint16)minimum_z;
                    branch->maximum_z = (sint16)(split - 1u); branch->node = (sint16)r_u16(node);
                    branch->start_x = interpolated; branch->end_x = ex; branch->start_z = split; branch->end_z = ez;
                    ex = interpolated; ez = split;
                }
                minimum_z = split; child = ~(uint32)(sint32)(sint16)r_u16(node + 2u);
            }
        }
        node = root + child * 8u;
    }
}

void level_sub_8003EC04(void)
{
    uint32 count, excluded, index, slot;
    FUNCTION_MARKER(0x8003EC04u, "LEVEL.BIN");
    count = r_u32(0x80098984u); w_u32(0x800AB8FCu, 0u);
    if ((sint32)count <= 0) return;
    excluded = r_u32(0x800AA184u);
    for (index = 0u; (sint32)index < (sint32)count; ++index) {
        if (index == excluded) continue;
        slot = r_u32(0x800AB8FCu);
        w_u32(0x800AB908u + slot * 4u, 0x80090DE8u + index * 40u);
        w_u32(0x800AB8FCu, slot + 1u);
    }
}

uint32 level_sub_8003EC78(uint32 x, uint32 z, uint32 index)
{
    uint32 bounds;
    FUNCTION_MARKER(0x8003EC78u, "LEVEL.BIN");
    bounds = r_u32(0x800AB908u + index * 4u);
    if ((sint32)x < (sint16)r_u16(bounds) || (sint16)r_u16(bounds + 4u) < (sint32)x) return 0u;
    if ((sint32)z < (sint16)r_u16(bounds + 2u) || (sint16)r_u16(bounds + 6u) < (sint32)z) return 0u;
    return bounds;
}

uint32 level_sub_8003ECE4(uint32 end, uint32 start, uint32 index)
{
    uint32 bounds, dx, dz, right_top, right_bottom, left_top, left_bottom;
    sint32 ex, ez, sx, sz, right, left, top, bottom;
    FUNCTION_MARKER(0x8003ECE4u, "LEVEL.BIN");
    bounds = r_u32(0x800AB908u + index * 4u);
    ex = (sint16)r_u16(end + 2u); sx = (sint16)r_u16(start + 2u);
    ez = (sint16)r_u16(end + 10u); top = (sint16)r_u16(bounds + 6u);
    dx = (uint32)sx - (uint32)ex; sz = (sint16)r_u16(start + 10u);
    right = (sint16)r_u16(bounds + 4u); dz = (uint32)sz - (uint32)ez;
    bottom = (sint16)r_u16(bounds + 2u);
    right_top = ((uint32)top - (uint32)ez) * dx - ((uint32)right - (uint32)ex) * dz;
    right_bottom = ((uint32)bottom - (uint32)ez) * dx - ((uint32)right - (uint32)ex) * dz;
    if ((sint32)(right_top ^ right_bottom) <= 0 && right < ex && sx < right) return bounds;
    top = (sint16)r_u16(bounds + 6u); left = (sint16)r_u16(bounds);
    left_top = ((uint32)top - (uint32)ez) * dx - ((uint32)left - (uint32)ex) * dz;
    if ((sint32)(right_top ^ left_top) <= 0 && top < ez && sz < top) return bounds;
    bottom = (sint16)r_u16(bounds + 2u); left = (sint16)r_u16(bounds);
    left_bottom = ((uint32)bottom - (uint32)ez) * dx - ((uint32)left - (uint32)ex) * dz;
    if ((sint32)(left_bottom ^ left_top) <= 0 && left < ex && sx < left) return bounds;
    if ((sint32)(left_bottom ^ right_bottom) <= 0 && bottom < ez && sz < bottom) return bounds;
    return 0u;
}

uint32 level_sub_8003EACC(uint32 end, uint32 start, uint32 normal_out, uint32 hit_out,
    uint32 distance_out, uint32 material_out, uint32 flags)
{
    uint32 local, result, axis;
    FUNCTION_MARKER(0x8003EACCu, "LEVEL.BIN");
    local = sub_80016718(16u);
    if (!local) moh_draft_fail_fast("Ray vector allocation", 0x8003EACCu);
    for (axis = 0u; axis < 3u; ++axis) w_u16(local + axis * 2u, r_u16(start + axis * 4u + 2u));
    for (axis = 0u; axis < 3u; ++axis) w_u16(local + 8u + axis * 2u, r_u16(end + axis * 4u + 2u));
    /* TODO Original unused vector padding is unspecified */
    result = moh_draft_missing_call(0x8003E3F0u, local, local + 8u, normal_out, hit_out, distance_out, material_out, flags);
    sub_80036E44(local); return result;
}

uint32 level_sub_8003EF5C(uint32 end, uint32 start, uint32 normal_out, uint32 hit_out,
    uint32 distance_out, uint32 material_out, uint32 flags)
{
    uint32 local, point, material, index = 0u, result = 0xFFFFFFFFu, count, candidate, bounds, offset;
    uint32 words[4];
    sint32 nearest = 0x7FFF, distance;
    FUNCTION_MARKER(0x8003EF5Cu, "LEVEL.BIN");
    local = sub_80016718(80u);
    if (!local) moh_draft_fail_fast("Ray result allocation", 0x8003EF5Cu);
    point = local + 56u; material = local + 16u;
    level_sub_8003EC04(); count = r_u32(0x800AB8FCu);
    words[0] = r_u32(end); words[1] = r_u32(end + 4u); words[2] = r_u32(end + 8u);
    for (offset = 0u; offset < 3u; ++offset) w_u32(point + offset * 4u, words[offset]);
    while ((sint32)index < (sint32)count) {
        bounds = level_sub_8003EC78((uint32)(sint32)(sint16)r_u16(point + 2u), (uint32)(sint32)(sint16)r_u16(point + 10u), index);
        w_u32(0x800AB8F8u, bounds);
        if (!bounds) {
            bounds = level_sub_8003EC78((uint32)(sint32)(sint16)r_u16(start + 2u), (uint32)(sint32)(sint16)r_u16(start + 10u), index);
            w_u32(0x800AB8F8u, bounds);
        }
        if (!bounds) { bounds = level_sub_8003ECE4(point, start, index); w_u32(0x800AB8F8u, bounds); }
        if (bounds) {
            candidate = level_sub_8003EACC(point, start, local, local + 8u, local + 72u, material, flags);
            distance = (sint32)r_u32(local + 72u);
            if (distance < nearest) {
                nearest = distance;
                words[0] = r_u32(local); words[1] = r_u32(local + 4u);
                w_u32(normal_out, words[0]); w_u32(normal_out + 4u, words[1]);
                words[0] = r_u32(local + 8u); words[1] = r_u32(local + 12u);
                w_u32(hit_out, words[0]); w_u32(hit_out + 4u, words[1]); w_u32(distance_out, (uint32)distance);
                for (offset = 0u; offset < 32u; offset += 16u) {
                    uint32 word;
                    for (word = 0u; word < 4u; ++word) words[word] = r_u32(material + offset + word * 4u);
                    for (word = 0u; word < 4u; ++word) w_u32(material_out + offset + word * 4u, words[word]);
                }
                words[0] = r_u32(material + 32u); words[1] = r_u32(material + 36u);
                w_u32(material_out + 32u, words[0]); w_u32(material_out + 36u, words[1]); result = candidate;
            }
        }
        ++index; count = r_u32(0x800AB8FCu);
    }
    sub_80036E44(local); return result;
}

uint32 sub_80016A10(uint32 input, uint32 output)
{
    uint32 vector[3], axis, sum, leading, shift, normalized;
    sint32 scale_shift;
    VECTOR source, squares, product;
    SVECTOR narrowed;
    FUNCTION_MARKER(0x80016A10u, "SLUS_009.74");
    for (axis = 0u; axis < 3u; ++axis) vector[axis] = r_u32(input + axis * 4u);
    source.vx = (sint32)vector[0]; source.vy = (sint32)vector[1]; source.vz = (sint32)vector[2]; source.pad = 0;
    Square0(&source, &squares);
    sum = (uint32)squares.vx + (uint32)squares.vy + (uint32)squares.vz;
    /* Native Lzc provides the normalization count without a guest CPU register frame */
    leading = (uint32)Lzc((sint32)sum) & 0xFFFFFFFEu;
    shift = (uint32)((sint32)(31u - leading) >> 1) & 31u;
    scale_shift = (sint32)(leading - 24u);
    normalized = scale_shift < 0 ? (uint32)((sint32)sum >> ((24u - leading) & 31u)) : sum << ((uint32)scale_shift & 31u);
    narrowed.vx = (sint16)vector[0]; narrowed.vy = (sint16)vector[1]; narrowed.vz = (sint16)vector[2]; narrowed.pad = 0;
    gte_gpf0(&narrowed, (sint16)r_u16(0x8002DD3Cu + (normalized - 64u) * 2u), &product);
    vector[0] = (uint32)(product.vx >> shift); vector[1] = (uint32)(product.vy >> shift); vector[2] = (uint32)(product.vz >> shift);
    for (axis = 0u; axis < 3u; ++axis) w_u32(output + axis * 4u, vector[axis]);
    /* TODO Architectural LZCS and LZCR side effects are outside this native adapter */
    return sum;
}

uint32 sub_80016578(uint32 left, uint32 right, uint32 output)
{
    MATRIX saved;
    uint32 controls[3], axis;
    FUNCTION_MARKER(0x80016578u, "SLUS_009.74");
    ReadRotMatrix(&saved);
    controls[0] = (uint16)saved.m[0][0] | ((uint32)(uint16)saved.m[0][1] << 16);
    controls[1] = (uint16)saved.m[1][1] | ((uint32)(uint16)saved.m[1][2] << 16);
    controls[2] = (uint32)(sint32)saved.m[2][2];
    for (axis = 0u; axis < 3u; ++axis) xport_gte_write_control(axis * 2u, r_u32(left + axis * 4u));
    xport_gte_write_data(11u, r_u32(right + 8u)); xport_gte_write_data(9u, r_u32(right)); xport_gte_write_data(10u, r_u32(right + 4u));
    xport_gte_execute(0x178000Cu);
    for (axis = 0u; axis < 3u; ++axis) w_u32(output + axis * 4u, xport_gte_read_data(25u + axis));
    for (axis = 0u; axis < 3u; ++axis) xport_gte_write_control(axis * 2u, controls[axis]);
    /* TODO Original V0 is not a defined return value */
    return 0u;
}

uint32 sub_800163E8(uint32 matrix, uint32 input, uint32 output)
{
    uint32 vector[3], high[3], low[3], axis, magnitude;
    FUNCTION_MARKER(0x800163E8u, "SLUS_009.74");
    for (axis = 0u; axis < 5u; ++axis) xport_gte_write_control(axis, r_u32(matrix + axis * 4u));
    for (axis = 0u; axis < 3u; ++axis) vector[axis] = r_u32(input + axis * 4u);
    for (axis = 0u; axis < 3u; ++axis) {
        if ((sint32)vector[axis] < 0) {
            magnitude = 0u - vector[axis]; high[axis] = 0u - (uint32)((sint32)magnitude >> 15); low[axis] = 0u - (magnitude & 0x7FFFu);
        } else { high[axis] = (uint32)((sint32)vector[axis] >> 15); low[axis] = vector[axis] & 0x7FFFu; }
        xport_gte_write_data(9u + axis, high[axis]);
    }
    xport_gte_execute(0x41E012u);
    for (axis = 0u; axis < 3u; ++axis) high[axis] = xport_gte_read_data(25u + axis);
    for (axis = 0u; axis < 3u; ++axis) xport_gte_write_data(9u + axis, low[axis]);
    xport_gte_execute(0x49E012u);
    for (axis = 0u; axis < 3u; ++axis) vector[axis] = xport_gte_read_data(25u + axis) + (high[axis] << 3);
    for (axis = 0u; axis < 3u; ++axis) w_u32(output + axis * 4u, vector[axis]);
    return output;
}

uint32 sub_80016248(sint32 angle, uint32 matrix)
{
    uint32 absolute = angle < 0 ? 0u - (uint32)angle : (uint32)angle;
    uint32 packed = r_u32(0x8003040Cu + (absolute & 0xFFFu) * 4u), index;
    sint32 sine = (sint16)packed, cosine = (sint16)(packed >> 16);
    sint32 first[3], last[3];
    FUNCTION_MARKER(0x80016248u, "SLUS_009.74");
    if (angle >= 0) sine = -sine;
    for (index = 0u; index < 3u; ++index) {
        first[index] = (sint16)r_u16(matrix + index * 2u);
        last[index] = (sint16)r_u16(matrix + 12u + index * 2u);
    }
    for (index = 0u; index < 3u; ++index)
        w_u16(matrix + index * 2u, (uint16)((sint32)((uint32)cosine * (uint32)first[index] - (uint32)sine * (uint32)last[index]) >> 12));
    for (index = 0u; index < 3u; ++index)
        w_u16(matrix + 12u + index * 2u, (uint16)((sint32)((uint32)sine * (uint32)first[index] + (uint32)cosine * (uint32)last[index]) >> 12));
    return matrix;
}

uint32 sub_80016218(uint32 matrix, uint32 translation)
{
    uint32 x = r_u32(translation), y = r_u32(translation + 4u), z = r_u32(translation + 8u);
    FUNCTION_MARKER(0x80016218u, "SLUS_009.74");
    w_u32(matrix + 20u, x); w_u32(matrix + 24u, y); w_u32(matrix + 28u, z);
    return matrix;
}

uint32 sub_80016B30(uint32 input, uint32 output, uint32 flags_out)
{
    uint32 x, y;
    PsxGteSnapshot snapshot;
    FUNCTION_MARKER(0x80016B30u, "SLUS_009.74");
    xport_gte_write_data(0u, r_u32(input)); xport_gte_write_data(1u, r_u32(input + 4u));
    xport_gte_execute(0x480012u);
    x = xport_gte_read_data(9u); y = xport_gte_read_data(10u);
    w_u32(output + 4u, xport_gte_read_data(11u)); w_u16(output, (uint16)x); w_u16(output + 2u, (uint16)y);
    psx_gte_snapshot(&snapshot); w_u32(flags_out, (uint32)snapshot.flag);
    return (uint32)snapshot.flag;
}

static uint32 camera_resolve_handle(uint32 handle)
{
    return handle ? r_u32(0x8009D678u + (handle & 0xFFFFu) * 4u) : 0u;
}

static void camera_cross8(uint32 left, uint32 right, uint32 output)
{
    uint32 axis, first[3], second[3], result[3];
    for (axis = 0u; axis < 3u; ++axis) {
        first[axis] = (uint32)((sint32)r_u32(left + axis * 4u) >> 8);
        second[axis] = (uint32)((sint32)r_u32(right + axis * 4u) >> 8);
    }
    for (axis = 0u; axis < 3u; ++axis) result[axis] = first[(axis + 1u) % 3u] * second[(axis + 2u) % 3u] - first[(axis + 2u) % 3u] * second[(axis + 1u) % 3u];
    for (axis = 0u; axis < 3u; ++axis) w_u32(output + axis * 4u, result[axis]);
}

void level_sub_8008CCC8(uint32 input, uint32 output)
{
    uint32 mask = 0x40000000u, x = r_u32(input), axis, value, local;
    sint32 bit = 30;
    FUNCTION_MARKER(0x8008CCC8u, "LEVEL.BIN");
    if ((sint32)x < 0) x = 0u - x;
    for (;;) {
        if (x & mask) break;
        value = r_u32(input + 4u); if ((sint32)value < 0) value = 0u - value;
        if (value & mask) break;
        value = r_u32(input + 8u); if ((sint32)value < 0) value = 0u - value;
        if (value & mask) break;
        --bit; mask >>= 1; if (bit < 14) break;
    }
    bit -= 13; local = sub_80016718(28u);
    for (axis = 0u; axis < 3u; ++axis) {
        value = r_u32(input + axis * 4u);
        w_u32(local + axis * 4u, bit > 0 ? (uint32)((sint32)value >> ((uint32)bit & 31u)) : value);
    }
    sub_80016A10(local, local + 16u);
    for (axis = 0u; axis < 3u; ++axis) w_u32(output + axis * 4u, r_u32(local + 16u + axis * 4u) << 4);
    sub_80036E44(local);
}

void level_sub_8008D604(uint32 plane)
{
    uint32 axis, sum = 0u;
    FUNCTION_MARKER(0x8008D604u, "LEVEL.BIN");
    for (axis = 0u; axis < 3u; ++axis)
        sum += (uint32)((sint32)r_u32(plane + 12u + axis * 4u) >> 8) * (uint32)((sint32)r_u32(plane + axis * 4u) >> 8);
    w_u32(plane + 24u, 0u - sum);
}

void level_sub_8008CE28(uint32 plane, uint32 first, uint32 origin, uint32 second)
{
    uint32 local = sub_80016718(48u), axis, position[3];
    FUNCTION_MARKER(0x8008CE28u, "LEVEL.BIN");
    for (axis = 0u; axis < 3u; ++axis) position[axis] = r_u32(origin + axis * 4u);
    for (axis = 0u; axis < 3u; ++axis) w_u32(plane + axis * 4u, position[axis]);
    for (axis = 0u; axis < 3u; ++axis) w_u32(local + axis * 4u, r_u32(origin + axis * 4u) - r_u32(first + axis * 4u));
    level_sub_8008CCC8(local, local + 16u);
    for (axis = 0u; axis < 3u; ++axis) w_u32(local + axis * 4u, r_u32(second + axis * 4u) - r_u32(origin + axis * 4u));
    level_sub_8008CCC8(local, local + 32u);
    camera_cross8(local + 16u, local + 32u, local);
    level_sub_8008CCC8(local, plane + 12u); level_sub_8008D604(plane);
    sub_80036E44(local);
}

void level_sub_8008D0CC(uint32 view)
{
    uint32 local = sub_80016718(144u), axis, scale, direction, side, up, center, count;
    FUNCTION_MARKER(0x8008D0CCu, "LEVEL.BIN");
    for (axis = 0u; axis < 3u; ++axis) w_u32(local + axis * 4u, r_u32(view + 12u + axis * 4u) - r_u32(view + axis * 4u));
    level_sub_8008CCC8(local, local + 16u);
    for (axis = 0u; axis < 3u; ++axis) w_u32(local + axis * 4u, r_u32(view + 36u + axis * 4u) - r_u32(view + axis * 4u));
    level_sub_8008CCC8(local, local + 32u);
    camera_cross8(local + 16u, local + 32u, local); level_sub_8008CCC8(local, local + 48u);
    camera_cross8(local + 48u, local + 16u, local); level_sub_8008CCC8(local, local + 32u);
    for (axis = 0u; axis < 3u; ++axis) w_u32(local + 48u + axis * 4u, 0u - r_u32(local + 48u + axis * 4u));
    scale = (uint32)((sint32)r_u32(view + 48u) >> 9);
    for (axis = 0u; axis < 3u; ++axis) w_u32(local + 32u + axis * 4u, (uint32)((sint32)r_u32(local + 32u + axis * 4u) >> 8) * scale);
    scale = (uint32)((sint32)r_u32(view + 52u) >> 9);
    for (axis = 0u; axis < 3u; ++axis) w_u32(local + 48u + axis * 4u, (uint32)((sint32)r_u32(local + 48u + axis * 4u) >> 8) * scale);
    for (axis = 0u; axis < 3u; ++axis) {
        direction = (uint32)((sint32)r_u32(local + 16u + axis * 4u) >> 8); scale = (uint32)((sint32)r_u32(view + 60u) >> 8);
        center = r_u32(view + axis * 4u) + direction * scale; w_u32(local + 64u + axis * 4u, center);
        side = r_u32(local + 32u + axis * 4u); up = r_u32(local + 48u + axis * 4u);
        w_u32(local + 80u + axis * 4u, center + up - side); w_u32(local + 96u + axis * 4u, center + up + side);
        w_u32(local + 112u + axis * 4u, center - up - side); w_u32(local + 128u + axis * 4u, center - up + side);
    }
    level_sub_8008CE28(view + 96u, local + 80u, view, local + 112u);
    level_sub_8008CE28(view + 124u, local + 128u, view, local + 96u);
    level_sub_8008CE28(view + 152u, local + 96u, view, local + 80u);
    level_sub_8008CE28(view + 180u, local + 112u, view, local + 128u);
    for (axis = 0u; axis < 3u; ++axis) w_u32(local + axis * 4u, r_u32(view + axis * 4u));
    for (axis = 0u; axis < 3u; ++axis) w_u32(view + 68u + axis * 4u, r_u32(local + axis * 4u));
    for (axis = 0u; axis < 3u; ++axis) w_u32(view + 80u + axis * 4u, r_u32(local + 16u + axis * 4u));
    level_sub_8008D604(view + 68u);
    if ((sint32)r_u32(view + 64u) > 0) {
        for (axis = 0u; axis < 3u; ++axis) {
            direction = r_u32(local + 16u + axis * 4u); scale = (uint32)((sint32)r_u32(view + 64u) >> 8);
            w_u32(local + axis * 4u, r_u32(view + axis * 4u) + scale * (uint32)((sint32)direction >> 8));
        }
        for (axis = 0u; axis < 3u; ++axis) w_u32(local + 16u + axis * 4u, 0u - r_u32(local + 16u + axis * 4u));
        for (axis = 0u; axis < 3u; ++axis) w_u32(view + 208u + axis * 4u, r_u32(local + axis * 4u));
        for (axis = 0u; axis < 3u; ++axis) w_u32(view + 220u + axis * 4u, r_u32(local + 16u + axis * 4u));
        level_sub_8008D604(view + 208u); count = 6u;
    } else count = 5u;
    w_u32(view + 236u, count); sub_80036E44(local);
}

void level_sub_80048EEC(uint32 view, uint32 object)
{
    uint32 state = r_u32(object + 0x388u), local = sub_80016718(144u), axis, value;
    FUNCTION_MARKER(0x80048EECu, "LEVEL.BIN");
    for (axis = 0u; axis < 4u; ++axis) w_u32(local + 48u + axis * 4u, r_u32(0x80039DC0u + axis * 4u));
    for (axis = 0u; axis < 3u; ++axis) w_u32(local + axis * 4u, r_u32(view + axis * 4u) - r_u32(view + 12u + axis * 4u));
    for (axis = 0u; axis < 3u; ++axis) w_u32(local + 96u + axis * 4u, (uint32)(sint32)(sint16)r_u16(state + 110u + axis * 4u) - r_u32(view + 12u + axis * 4u));
    moh_draft_missing_call(0x80016A10u, local + 96u, local + 112u);
    moh_draft_missing_call(0x80016A10u, local, local + 16u);
    moh_draft_missing_call(0x80016578u, local + 16u, local + 112u, local + 48u);
    moh_draft_missing_call(0x80016578u, local + 48u, local + 16u, local + 32u);
    for (axis = 0u; axis < 3u; ++axis) w_u16(view + 28u + axis * 2u, (uint16)(r_u32(local + 32u + axis * 4u) << 1));
    for (axis = 0u; axis < 3u; ++axis) {
        value = r_u32(local + 48u + axis * 4u) * 5u;
        if ((sint32)value < 0) value += 7u;
        w_u16(view + 34u + axis * 2u, (uint16)((uint32)((sint32)value >> 3) << 1));
    }
    w_u16(view + 40u, (uint16)(r_u32(local + 16u) << 1));
    w_u16(view + 42u, (uint16)(r_u32(local + 20u) << 1));
    value = r_u32(local + 24u);
    w_u32(view + 48u, 0u); w_u32(view + 52u, 0u); w_u32(view + 56u, 0u);
    w_u16(view + 44u, (uint16)(value << 1));
    for (axis = 0u; axis < 3u; ++axis) w_u32(local + 64u + axis * 4u, 0u - r_u32(view + 12u + axis * 4u));
    moh_draft_missing_call(0x800163E8u, view + 28u, local + 64u, local + 80u);
    for (axis = 0u; axis < 3u; ++axis) w_u32(view + 48u + axis * 4u, r_u32(local + 80u + axis * 4u));
    /* TODO Unspecified vector and matrix padding retain original unused-byte uncertainty */
    sub_80036E44(local);
}

void level_sub_8004913C(uint32 object)
{
    uint32 state = r_u32(object + 0x388u), axis, source, output, target, height;
    uint32 position[3], focus[3], ahead[3];
    FUNCTION_MARKER(0x8004913Cu, "LEVEL.BIN");
    source = r_u32(state + 156u) & 0x1800u ? state + 62u : object + 0x20Eu;
    for (axis = 0u; axis < 3u; ++axis) w_u32(state + 240u + axis * 4u, (uint32)(sint32)(sint16)r_u16(source + axis * 4u));
    for (axis = 0u; axis < 3u; ++axis) w_u32(state + 228u + axis * 4u, (uint32)(sint32)(sint16)r_u16(state + 2u + axis * 4u));
    w_u32(state + 252u, (uint32)(sint32)(sint16)r_u16(state + 146u));
    level_sub_80048EEC(state + 228u, object);
    source = r_u32(state + 156u) & 0x1800u ? state + 60u : object + 0x20Cu;
    for (axis = 0u; axis < 3u; ++axis) position[axis] = r_u32(source + axis * 4u);
    for (axis = 0u; axis < 3u; ++axis) focus[axis] = r_u32(state + axis * 4u);
    for (axis = 0u; axis < 3u; ++axis) ahead[axis] = r_u32(state + 108u + axis * 4u);
    output = r_u32(state + 200u);
    for (axis = 0u; axis < 3u; ++axis) w_u32(output + 12u + axis * 4u, focus[axis]);
    output = r_u32(state + 200u);
    for (axis = 0u; axis < 3u; ++axis) w_u32(output + axis * 4u, position[axis]);
    output = r_u32(state + 200u);
    for (axis = 0u; axis < 3u; ++axis) w_u32(output + 36u + axis * 4u, ahead[axis]);
    height = r_u32(state + 196u); output = r_u32(state + 200u); w_u32(output + 64u, height << 16);
    target = camera_resolve_handle(r_u32(state + 12u));
    /* TODO Original null target dereference is not a defined fallback */
    if (!target) moh_draft_fail_fast("Camera view target missing", 0x8004913Cu);
    height = r_u32(r_u32(target + 0x388u) + 164u); output = r_u32(state + 200u); w_u32(output + 60u, height << 16);
    moh_draft_missing_call(0x8008D0CCu, r_u32(state + 200u));
}

static void camera_matrix_push(uint32 matrix)
{
    uint32 offset = r_u32(0x8009D320u), index;
    MATRIX saved;
    ReadRotMatrix(&saved);
    for (index = 0u; index < 4u; ++index) {
        uint32 first = index * 2u, second = first + 1u;
        w_u32(0x8009D324u + offset + index * 4u, (uint16)saved.m[first / 3u][first % 3u] | ((uint32)(uint16)saved.m[second / 3u][second % 3u] << 16));
    }
    w_u32(0x8009D334u + offset, (uint32)(sint32)saved.m[2][2]);
    for (index = 0u; index < 3u; ++index) w_u32(0x8009D338u + offset + index * 4u, (uint32)saved.t[index]);
    w_u32(0x8009D320u, offset + 32u);
    for (index = 0u; index < 8u; ++index) xport_gte_write_control(index, r_u32(matrix + index * 4u));
}

static void camera_matrix_pop(void)
{
    uint32 offset = r_u32(0x8009D320u) - 32u, index;
    w_u32(0x8009D320u, offset);
    for (index = 0u; index < 8u; ++index) xport_gte_write_control(index, r_u32(0x8009D324u + offset + index * 4u));
}

static void camera_turn_towards_target(uint32 state, uint32 target)
{
    uint32 direction = r_u32(target + 0x288u), angle = r_u32(state + 176u);
    sint32 difference;
    if (r_u32(state + 156u) & 0x200u) {
        uint32 x = r_u32(state + 96u) * direction, y = r_u32(state + 100u), z = r_u32(state + 104u);
        w_u32(state + 88u, y); w_u32(state + 92u, z); w_u32(state + 84u, x);
        direction = r_u32(target + 0x288u);
    }
    difference = (sint32)(r_u32(target + 0x22Cu) - (angle + (direction == 1u ? 0u : 0x08000000u)));
    while (difference > 0x08000000) difference = (sint32)((uint32)difference - 0x10000000u);
    while (difference < -0x08000000) difference = (sint32)((uint32)difference + 0x10000000u);
    w_u32(state + 176u, r_u32(state + 176u) + (uint32)(difference >> (r_u32(state + 20u) & 31u)));
}

static void camera_make_y_matrix(uint32 matrix, uint32 translation, uint32 state)
{
    uint32 index;
    for (index = 0u; index < 9u; ++index) w_u16(matrix + index * 2u, index % 4u ? 0u : 0x1000u);
    sub_80016248((sint16)r_u16(state + 178u), matrix); sub_80016218(matrix, translation);
}

void level_sub_800482A4(uint32 object, uint32 target)
{
    uint32 state = r_u32(object + 0x388u), flags = r_u32(state + 156u), index, local, shift, desired, current;
    uint32 x, y, z, result;
    sint32 distance, vertical, floor_y;
    FUNCTION_MARKER(0x800482A4u, "LEVEL.BIN");
    if (flags & 0x800u) {
        x = r_u16(state + 74u); y = r_u16(state + 78u); z = r_u16(state + 82u);
        w_u32(state + 148u, (uint32)(sint32)(sint16)y);
        for (index = 0u; index < 8u; ++index) xport_gte_write_control(index, r_u32(object + 0x340u + index * 4u));
        /* TODO Original vector padding is unspecified and unused by this command */
        xport_gte_write_data(0u, x | (y << 16)); xport_gte_write_data(1u, z); xport_gte_execute(0x480012u);
        x = (uint16)xport_gte_read_data(25u); y = (uint16)xport_gte_read_data(26u); z = (uint16)xport_gte_read_data(27u);
        w_u32(state + 60u, x << 16); w_u32(state + 68u, z << 16);
        current = r_u32(object + 0x208u);
        w_u32(state + 64u, (sint32)((uint32)((sint32)current >> 16) - (uint32)(sint32)(sint16)y) < 85 ? current - 0x550000u : y << 16);
        return;
    }
    if (flags & 0x80u) {
        w_u32(object + 0x20Cu, r_u32(target + 0x20Cu));
        desired = r_u32(state + 148u) << 16; current = r_u32(target + 0x210u);
        w_u32(object + 0x210u, current + desired); w_u32(object + 0x214u, r_u32(target + 0x214u)); return;
    }
    if (flags & 3u) {
        for (index = 0u; index < 3u; ++index) {
            desired = r_u32(state + (flags & 2u ? 72u : 84u) + index * 4u);
            if (flags & 2u) desired += r_u32(target + 0x20Cu + index * 4u);
            current = r_u32(object + 0x20Cu + index * 4u); shift = r_u16(state + 216u) & 31u;
            w_u32(object + 0x1F0u + index * 4u, (uint32)((sint32)(desired - current) >> shift));
        }
        return;
    }
    if (flags & 0x204u) {
        local = sub_80016718(120u);
        for (index = 0u; index < 16u; index += 4u) w_u32(local + 8u + index, 0u);
        xport_gte_write_control(0u, 0u); xport_gte_write_control(2u, 0x1000u); xport_gte_write_control(4u, 0u);
        for (index = 0u; index < 3u; ++index)
            xport_gte_write_data(9u + index, (uint32)(sint32)(sint16)(r_u16(state + 26u + index * 4u) - r_u16(state + 74u + index * 4u)));
        xport_gte_execute(0x178000Cu);
        for (index = 0u; index < 3u; ++index) w_u16(local + 24u + index * 2u, (uint16)xport_gte_read_data(9u + index));
        w_u16(local + 26u, r_u16(state + 78u));
        for (index = 0u; index < 3u; ++index) w_u16(local + index * 2u, r_u16(state + 74u + index * 4u));
        camera_turn_towards_target(state, target); camera_make_y_matrix(local + 64u, local + 8u, state);
        camera_matrix_push(local + 64u);
        moh_draft_missing_call(0x80016548u, local, local + 32u, local + 112u);
        moh_draft_missing_call(0x80016548u, local + 24u, local + 48u, local + 112u);
        camera_matrix_pop();
        x = r_u32(target + 0x20Cu) + (r_u32(local + 32u) << 16);
        y = r_u32(target + 0x210u) + r_u32(state + 76u); z = r_u32(target + 0x214u) + (r_u32(local + 40u) << 16);
        moh_draft_missing_call(0x8006D2D4u, object, x, y, z);
        w_u32(state + 108u, r_u32(target + 0x20Cu) + (r_u32(local + 48u) << 16));
        w_u32(state + 112u, r_u32(object + 0x210u));
        w_u32(state + 116u, r_u32(target + 0x214u) + (r_u32(local + 56u) << 16));
        sub_80036E44(local); return;
    }
    if (!(flags & 0x40u)) return;
    local = sub_80016718(120u);
    for (index = 0u; index < 16u; index += 4u) w_u32(local + index, 0u);
    distance = (sint32)(r_u32(target + 0x20Cu) - r_u32(target + 0x258u)) >> 16;
    if (distance < 0) distance = -distance;
    vertical = (sint32)(r_u32(target + 0x214u) - r_u32(target + 0x260u)) >> 16;
    if (vertical < 0) vertical = -vertical;
    if (distance + vertical > 20) {
        static const uint32 offsets[] = {84u, 92u, 88u};
        for (index = 0u; index < 3u; ++index) {
            uint32 offset = offsets[index]; current = r_u32(state + offset); desired = r_u32(state + offset + 12u);
            if (current != desired) w_u32(state + offset, desired);
        }
    }
    for (index = 0u; index < 3u; ++index) w_u16(local + 16u + index * 2u, (uint16)(r_u16(state + 208u + index * 2u) + r_u16(state + 74u + index * 4u)));
    vertical = (sint32)(r_u32(target + 0x25Cu) - r_u32(target + 0x210u)) >> 16;
    if (vertical < 0) vertical = -vertical;
    camera_turn_towards_target(state, target); camera_make_y_matrix(local + 64u, local, state);
    camera_matrix_push(local + 64u);
    moh_draft_missing_call(0x80016548u, local + 16u, local + 24u, local + 116u);
    camera_matrix_pop();
    w_u32(local + 96u, r_u32(target + 0x20Cu) + (r_u32(local + 24u) << 16));
    w_u32(local + 100u, r_u32(target + 0x210u) + r_u32(state + 88u));
    w_u32(local + 104u, r_u32(target + 0x214u) + (r_u32(local + 32u) << 16));
    result = moh_draft_missing_call(0x8003EE30u, local + 96u, 0u, 1u, 0u, 0u, 0u) - 256u;
    w_u16(state + 220u, (uint16)result);
    floor_y = (sint16)r_u16(object + 0x212u);
    if ((sint32)(result << 16) < (sint32)(((uint32)floor_y - 256u) << 16)) w_u16(state + 220u, r_u16(state + 222u));
    if (floor_y - (sint16)r_u16(state + 220u) < 64) w_u16(state + 220u, (uint16)(r_u16(state + 220u) - 64u));
    floor_y = (sint16)r_u16(target + 0x212u);
    if ((sint16)(floor_y + 512) < (sint16)r_u16(state + 220u)) w_u16(state + 220u, (uint16)floor_y);
    w_u16(state + 222u, r_u16(state + 220u));
    if (r_u32(object + 56u) & 0x10000u) {
        desired = r_u32(target + 0x20Cu); shift = r_u16(state + 216u) & 31u; current = r_u32(object + 0x20Cu);
        w_u32(object + 0x1F4u, 0u);
        desired += r_u32(local + 24u) << 16;
        w_u32(object + 0x1F0u, (uint32)((sint32)(desired - current) >> shift));
        desired = r_u32(target + 0x214u) + (r_u32(local + 32u) << 16); current = r_u32(object + 0x214u); shift = r_u16(state + 216u) & 31u;
        w_u32(object + 0x1F8u, (uint32)((sint32)(desired - current) >> shift));
        shift = r_u16(state + 224u) & 31u;
        desired = r_u32(object + 0x20Cu) + (uint32)((sint32)r_u32(object + 0x1F0u) >> shift);
        flags = r_u32(object + 56u); w_u32(object + 0x20Cu, desired);
        shift = r_u16(state + 224u) & 31u; w_u32(object + 56u, flags & 0xFFFEFFFFu);
        w_u32(object + 0x214u, r_u32(object + 0x214u) + (uint32)((sint32)r_u32(object + 0x1F8u) >> shift));
    } else {
        desired = r_u32(target + 0x20Cu) + (r_u32(local + 24u) << 16); current = r_u32(object + 0x20Cu); shift = r_u16(state + 216u) & 31u;
        w_u32(object + 0x1F0u, (uint32)((sint32)(desired - current) >> shift));
        desired = ((uint32)(sint32)(sint16)r_u16(state + 220u) + r_u32(local + 28u)) << 16;
        desired = desired - r_u32(object + 0x210u) + r_u32(state + 88u); shift = r_u16(state + 216u) & 31u;
        desired = (uint32)((sint32)desired >> shift) + ((uint32)r_u16(state + 210u) << 16) - ((uint32)vertical << 16);
        w_u32(object + 0x1F4u, desired);
        desired = r_u32(target + 0x214u) + (r_u32(local + 32u) << 16); current = r_u32(object + 0x214u); shift = r_u16(state + 216u) & 31u;
        w_u32(object + 0x1F8u, (uint32)((sint32)(desired - current) >> shift));
    }
    /* TODO Original vector padding and matrix padding are unspecified */
    sub_80036E44(local);
}

void level_sub_800479D8(uint32 object, uint32 target)
{
    uint32 state = r_u32(object + 0x388u), flags = r_u32(state + 156u);
    uint32 index, current, desired, shift, local, angle, direction;
    sint32 difference;
    FUNCTION_MARKER(0x800479D8u, "LEVEL.BIN");
    if (flags & 0x800u) {
        for (index = 0u; index < 8u; ++index) xport_gte_write_control(index, r_u32(object + 0x340u + index * 4u));
        for (index = 0u; index < 2u; ++index) {
            uint32 source = state + (index ? 120u : 24u), x, y, z;
            xport_gte_write_data(0u, r_u16(source + 2u) | ((uint32)r_u16(source + 6u) << 16));
            /* TODO Original vector padding is unspecified and unused by this GTE command */
            xport_gte_write_data(1u, r_u16(source + 10u));
            xport_gte_execute(0x480012u);
            x = (uint16)xport_gte_read_data(25u); y = (uint16)xport_gte_read_data(26u); z = (uint16)xport_gte_read_data(27u);
            w_u32(state + (index ? 108u : 0u), x << 16);
            w_u32(state + (index ? 112u : 4u), y << 16);
            w_u32(state + (index ? 116u : 8u), z << 16);
            if (!index) w_u32(state + 152u, (uint32)(sint32)(sint16)y);
        }
        return;
    }
    if (flags & 0x480u) {
        local = sub_80016718(112u);
        for (index = 0u; index < 16u; index += 4u) w_u32(local + 16u + index, 0u);
        w_u16(local, r_u16(state + 26u)); w_u16(local + 2u, 0u); w_u16(local + 4u, r_u16(state + 34u));
        w_u16(local + 8u, r_u16(state + 122u)); w_u16(local + 10u, 0u); w_u16(local + 12u, r_u16(state + 130u));
        w_u32(state + 188u, r_u32(r_u32(target + 0x388u) + 0x184u));
        for (index = 0u; index < 9u; ++index) w_u16(local + 32u + index * 2u, index % 4u ? 0u : 0x1000u);
        moh_draft_missing_call(0x80016248u, (uint32)(sint32)(sint16)r_u16(state + 190u), local + 32u);
        moh_draft_missing_call(0x80016218u, local + 32u, local + 16u);
        camera_matrix_push(local + 32u);
        moh_draft_missing_call(0x80016B30u, local, local, local + 96u);
        moh_draft_missing_call(0x80016B30u, local + 8u, local + 8u, local + 96u);
        camera_matrix_pop();
        w_u32(state, r_u32(target + 0x20Cu) + ((uint32)r_u16(local) << 16));
        w_u32(state + 8u, r_u32(target + 0x214u) + ((uint32)r_u16(local + 4u) << 16));
        w_u32(state + 4u, r_u32(target + 0x210u) + r_u32(state + 28u));
        w_u32(state + 108u, r_u32(target + 0x20Cu) + ((uint32)r_u16(local + 8u) << 16));
        w_u32(state + 116u, r_u32(target + 0x214u) + ((uint32)r_u16(local + 12u) << 16));
        w_u32(state + 112u, r_u32(target + 0x210u) + (r_u32(state + 148u) << 16));
        sub_80036E44(local); return;
    }
    if (flags & 0x10u) {
        shift = r_u16(state + 218u) & 31u;
        for (index = 0u; index < 3u; ++index) {
            current = r_u32(state + index * 4u); desired = r_u32(target + 0x20Cu + index * 4u) + r_u32(state + 24u + index * 4u);
            w_u32(state + index * 4u, current + (uint32)((sint32)(desired - current) >> shift));
        }
        for (index = 0u; index < 3u; ++index) {
            desired = r_u32(target + 0x20Cu + index * 4u) + r_u32(state + 120u + index * 4u) - r_u32(state + index * 4u);
            shift = r_u16(state + 218u) & 31u; current = r_u32(state + 108u + index * 4u);
            w_u32(state + 108u + index * 4u, current + (uint32)((sint32)desired >> shift));
        }
        return;
    }
    if (flags & 8u) {
        for (index = 0u; index < 6u; ++index) {
            uint32 axis = index % 3u, output = state + (index < 3u ? 0u : 108u) + axis * 4u;
            desired = r_u32(state + (index < 3u ? 36u : 132u) + axis * 4u) - r_u32(state + 24u + axis * 4u);
            shift = r_u16(state + 218u) & 31u; current = r_u32(output);
            w_u32(output, current + (uint32)((sint32)desired >> shift));
        }
        return;
    }
    if (!(flags & 0x120u)) return;
    local = sub_80016718(112u);
    w_u32(local, 0u); w_u32(local + 4u, 0u);
    for (index = 0u; index < 16u; index += 4u) { w_u32(local + 16u + index, 0u); w_u32(local + 32u + index, r_u32(0x80039DB0u + index)); }
    for (index = 0u; index < 3u; ++index) w_u16(local + 8u + index * 2u, r_u16(state + 26u + index * 4u));
    direction = r_u32(target + 0x288u); angle = r_u32(state + 188u);
    if (r_u32(state + 156u) & 0x100u) {
        desired = r_u32(state + 48u) * direction;
        current = r_u32(state + 52u); flags = r_u32(state + 56u);
        w_u32(state + 40u, current); w_u32(state + 44u, flags); w_u32(state + 36u, desired);
        direction = r_u32(target + 0x288u);
    }
    difference = (sint32)(r_u32(target + 0x22Cu) - (angle + (direction == 1u ? 0u : 0x08000000u)));
    while (difference > 0x08000000) difference = (sint32)((uint32)difference - 0x10000000u);
    while (difference < -0x08000000) difference = (sint32)((uint32)difference + 0x10000000u);
    w_u32(state + 188u, r_u32(state + 188u) + (uint32)(difference >> (r_u32(state + 20u) & 31u)));
    w_u16(local, 0u); w_u16(local + 2u, r_u16(state + 190u)); w_u16(local + 4u, 0u);
    /* TODO Matrix tail and vector padding originate from unspecified local bytes */
    moh_draft_missing_call(0x8001676Cu, local, local + 64u);
    moh_draft_missing_call(0x80016218u, local + 64u, local + 16u);
    moh_draft_missing_call(0x800165D8u, local + 64u, local + 32u);
    camera_matrix_push(local + 64u);
    moh_draft_missing_call(0x80016548u, local + 8u, local + 48u, local + 100u);
    camera_matrix_pop();
    shift = r_u16(state + 218u) & 31u;
    current = r_u32(state); desired = r_u32(target + 0x20Cu) + (r_u32(local + 48u) << 16);
    w_u32(state, current + (uint32)((sint32)(desired - current) >> shift));
    current = r_u32(state + 8u); desired = r_u32(target + 0x214u) + (r_u32(local + 56u) << 16);
    w_u32(state + 8u, current + (uint32)((sint32)(desired - current) >> shift));
    current = r_u32(state + 4u); desired = r_u32(target + 0x210u) + (r_u32(local + 52u) << 16);
    w_u32(state + 4u, current + (uint32)((sint32)(desired - current) >> 1));
    sub_80036E44(local);
}

void level_sub_80046C64(uint32 state)
{
    static const uint32 offsets[] = {72u, 76u, 80u, 24u, 28u, 32u, 120u, 124u, 128u};
    uint32 index, current, target, offset;
    FUNCTION_MARKER(0x80046C64u, "LEVEL.BIN");
    for (index = 0u; index < 9u; ++index) {
        offset = offsets[index]; current = r_u32(state + offset); target = r_u32(state + offset + 12u);
        if (current != target)
            w_u32(state + offset, current + (uint32)((sint32)(target - current) >> (r_u32(state + 16u) & 31u)));
    }
}

void level_sub_800473DC(uint32 object)
{
    uint32 state = r_u32(object + 0x388u), material, delta, first, second;
    sint32 displacement;
    FUNCTION_MARKER(0x800473DCu, "LEVEL.BIN");
    if (camera_resolve_handle(r_u32(state + 12u))) {
        if (r_u32(state + 156u) & 0x800u) {
            material = r_u32(object + 0x100u);
            if (!material || !r_u32(material + 8u)) w_u32(state + 156u, (r_u32(state + 156u) & 0xFFFFF7FFu) | 0x80u);
            else {
                if (!r_u32(0x8009A5E0u)) { level_sub_8003FBFC(object); level_sub_8003F8F0(object); }
                moh_draft_missing_call(0x80046EB4u, object, camera_resolve_handle(r_u32(state + 12u)));
            }
        }
        level_sub_80046C64(state);
        level_sub_800479D8(object, camera_resolve_handle(r_u32(state + 12u)));
        level_sub_800482A4(object, camera_resolve_handle(r_u32(state + 12u)));
    }
    if ((sint16)r_u16(state + 206u) > 0) {
        delta = (uint32)r_u16(state + 204u) << 16;
        w_u32(object + 0x210u, r_u32(object + 0x210u) + delta);
        delta = (uint32)r_u16(state + 204u) << 16; first = r_u32(state + 4u);
        second = (uint32)r_u16(state + 204u) << 16; delta += first;
        first = r_u32(state + 112u); w_u32(state + 4u, delta);
        displacement = (sint16)r_u16(state + 204u);
        w_u32(state + 112u, first + second); second = r_u16(state + 206u);
        w_u16(state + 204u, (uint16)(-(displacement >> 1))); w_u16(state + 206u, (uint16)(second - 1u));
    }
}
