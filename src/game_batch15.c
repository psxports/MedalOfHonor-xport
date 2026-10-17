#include "game_level2p_signatures.h"
#include "xport_trace.h"
#include "psx.h"

static uint32 moh_contact_corner_native(uint32 first_axis, uint32 first_other, uint32 second_axis, uint32 second_other, uint32 point_axis, uint32 point_other, uint32 radius_axis, uint32 radius_other, uint32 nonnegative)
{
    uint32 axis, other, descending_other, descending_axis;
    descending_other = (sint32)first_other > (sint32)second_other;
    descending_axis = (sint32)first_axis > (sint32)second_axis;
    axis = descending_other == nonnegative ? point_axis + radius_axis : point_axis - radius_axis;
    other = descending_axis == nonnegative ? point_other - radius_other : point_other + radius_other;
    return (axis & 0xFFFF) | (other << 16);
}

static void moh_contact_edge_native(uint32 first, uint32 second, uint32 corner)
{
    xport_gte_write_data(12, first); xport_gte_write_data(13, second); xport_gte_write_data(14, corner);
    xport_gte_execute(0x1400006);
}

uint32 level2p_sub_80071CCC(uint32 point, uint32 radius, uint32 height, uint32 first_vertex, uint32 second_vertex, uint32 third_vertex, uint32 plane)
{
    uint32 half_height, middle, nx, nz, ax, az, axis, nonnegative, point_axis;
    uint32 first_axis, first_other, second_axis, second_other, third_axis, third_other, first, second, third, corner, result;
    FUNCTION_MARKER(0x80071CCC, "LEVEL2P.BIN");
    /* TODO: Original point, vertex and plane spans are unchecked */
    half_height = (uint32)((sint32)height >> 1); middle = (uint32)(sint32)r_s16(point + 2) + half_height;
    nx = (uint32)(sint32)r_s16(plane); nz = (uint32)(sint32)r_s16(plane + 4);
    ax = (sint32)nx < 0 ? 0 - nx : nx; az = (sint32)nz < 0 ? 0 - nz : nz;
    axis = (sint32)ax < (sint32)az ? 0 : 4;
    nonnegative = !axis ? (sint32)nz < 0 : (sint32)nx > 0;
    /* Keep full32 radius and half-height until the original short packing */
    first_axis = (uint32)(sint32)r_s16(first_vertex + axis); first_other = (uint32)(sint32)r_s16(first_vertex + 2);
    point_axis = (uint32)(sint32)r_s16(point + axis);
    second_axis = (uint32)(sint32)r_s16(second_vertex + axis); second_other = (uint32)(sint32)r_s16(second_vertex + 2);
    first = (first_axis & 0xFFFF) | (first_other << 16); second = (second_axis & 0xFFFF) | (second_other << 16);
    corner = moh_contact_corner_native(first_axis, first_other, second_axis, second_other, point_axis, middle, radius, half_height, nonnegative);
    moh_contact_edge_native(first, second, corner);
    third_axis = (uint32)(sint32)r_s16(third_vertex + axis); third_other = (uint32)(sint32)r_s16(third_vertex + 2);
    third = (third_axis & 0xFFFF) | (third_other << 16);
    result = xport_gte_read_data(24);
    if (nonnegative ? (sint32)result < 0 : (sint32)result > 0) return 0;
    third_other = (uint32)(sint32)r_s16(third_vertex + 2); third_axis = (uint32)(sint32)r_s16(third_vertex + axis);
    corner = moh_contact_corner_native(second_axis, second_other, third_axis, third_other, point_axis, middle, radius, half_height, nonnegative);
    moh_contact_edge_native(second, third, corner);
    first_other = (uint32)(sint32)r_s16(first_vertex + 2); third_other = (uint32)(sint32)r_s16(third_vertex + 2);
    first_axis = (uint32)(sint32)r_s16(first_vertex + axis); third_axis = (uint32)(sint32)r_s16(third_vertex + axis);
    corner = moh_contact_corner_native(third_axis, third_other, first_axis, first_other, point_axis, middle, radius, half_height, nonnegative);
    result = xport_gte_read_data(24);
    if (nonnegative ? (sint32)result < 0 : (sint32)result > 0) return 0;
    moh_contact_edge_native(third, first, corner); result = xport_gte_read_data(24);
    return nonnegative ? (sint32)result >= 0 : (sint32)result <= 0;
}

uint32 level2p_sub_80072270(uint32 point, uint32 radius, uint32 height, uint32 mode, uint32 first_vertex, uint32 second_vertex, uint32 third_vertex, uint32 plane)
{
    uint8 work[8];
    uint32 selected[3];
    uint32 work_view, px, pz, x, y, z, first_x, first_y, second_x, second_y, third_x, third_y;
    uint32 minimum, maximum, bottom, top, nx, ny, nz, wall, magnitude, square, first, second, third, value, result;
    uint32 index, count = 0, mask = 0, vertex, nonnegative, corner;
    FUNCTION_MARKER(0x80072270, "LEVEL2P.BIN");
    /* TODO: Original first vertex X bound and all point/vertex spans are unchecked */
    px = (uint32)(sint32)r_s16(point); pz = (uint32)(sint32)r_s16(point + 4);
    first_x = (uint32)(sint32)r_s16(first_vertex);
    if ((sint32)(px + radius) < (sint32)first_x) return 0;
    third_x = (uint32)(sint32)r_s16(third_vertex); second_x = (uint32)(sint32)r_s16(second_vertex);
    maximum = (sint32)second_x < (sint32)third_x ? third_x : second_x;
    if ((sint32)maximum < (sint32)(px - radius)) return 0;
    second_y = (uint32)(sint32)r_s16(second_vertex + 4); first_y = (uint32)(sint32)r_s16(first_vertex + 4);
    third_y = (uint32)(sint32)r_s16(third_vertex + 4);
    minimum = (sint32)first_y < (sint32)second_y ? first_y : second_y;
    if ((sint32)third_y < (sint32)minimum) minimum = third_y;
    if ((sint32)(pz + radius) < (sint32)minimum) return 0;
    maximum = (sint32)first_y < (sint32)second_y ? second_y : first_y;
    if ((sint32)maximum < (sint32)third_y) maximum = third_y;
    if ((sint32)maximum < (sint32)(pz - radius)) return 0;
    first_y = (uint32)(sint32)r_s16(first_vertex + 2); second_y = (uint32)(sint32)r_s16(second_vertex + 2);
    bottom = (uint32)(sint32)r_s16(point + 2);
    minimum = (sint32)first_y < (sint32)second_y ? first_y : second_y;
    maximum = (sint32)first_y < (sint32)second_y ? second_y : first_y;
    third_y = (uint32)(sint32)r_s16(third_vertex + 2);
    if ((sint32)third_y < (sint32)minimum) minimum = third_y;
    if ((sint32)maximum < (sint32)third_y) maximum = third_y;
    if ((sint32)maximum < (sint32)bottom) return 0;
    top = bottom + height;
    if ((sint32)top < (sint32)minimum) return 0;
    /* TODO: Native local mapping and original untouched work halfword remain unresolved */
    work_view = moh_draft_local_address(work, sizeof(work)); magnitude = (sint32)radius < 0 ? 0 - radius : radius;
    w_u32(work_view, magnitude);
    x = r_u16(work_view); y = r_u16(work_view + 2); z = r_u16(work_view + 4);
    xport_gte_write_data(9, x); xport_gte_write_data(10, y); xport_gte_write_data(11, z); xport_gte_execute(0xA00428);
    square = xport_gte_read_data(25);
    nz = (uint32)(sint32)r_s16(plane + 4); nx = (uint32)(sint32)r_s16(plane);
    if ((sint32)nz < 0) nz = 0 - nz;
    ny = (uint32)(sint32)r_s16(plane + 2);
    if ((sint32)nx < 0) nx = 0 - nx;
    if ((sint32)ny < 0) ny = 0 - ny;
    maximum = (sint32)nx < (sint32)nz ? nz : nx; wall = (sint32)ny < (sint32)maximum;
    first_y = (uint32)(sint32)r_s16(first_vertex + 2);
    if ((sint32)first_y < (sint32)top && (sint32)bottom < (sint32)first_y) mask |= 1;
    second_y = (uint32)(sint32)r_s16(second_vertex + 2);
    if ((sint32)second_y < (sint32)top && (sint32)bottom < (sint32)second_y) mask |= 2;
    third_y = (uint32)(sint32)r_s16(third_vertex + 2);
    if ((sint32)third_y < (sint32)top && (sint32)bottom < (sint32)third_y) mask |= 4;
    /* The seven original targets visit admitted vertices in first/second/third order */
    if (mask & 1) selected[count++] = first_vertex;
    if (mask & 2) selected[count++] = second_vertex;
    if (mask & 4) selected[count++] = third_vertex;
    if (count) {
        value = r_u16(selected[0]); w_u16(work_view, value - px);
        value = r_u16(selected[0] + 4); w_u16(work_view + 2, value - pz);
        for (index = 0; index < count; ++index) {
            x = r_u16(work_view); y = r_u16(work_view + 2); z = r_u16(work_view + 4);
            xport_gte_write_data(9, x); xport_gte_write_data(10, y); xport_gte_write_data(11, z); xport_gte_execute(0xA00428);
            if (index + 1 < count) {
                vertex = selected[index + 1]; value = r_u16(vertex); w_u16(work_view, value - px);
                value = r_u16(vertex + 4); w_u16(work_view + 2, value - pz);
            }
            first = xport_gte_read_data(25); second = xport_gte_read_data(26); value = first + second;
            if (value < square) {
                if (wall || !mode) return 1;
                value = (uint32)(sint32)r_s16(plane + 2); return (sint32)value >= 0;
            }
        }
    }
    if (wall) return level2p_sub_80071CCC(point, radius, height, first_vertex, second_vertex, third_vertex, plane);
    if (mode) { value = (uint32)(sint32)r_s16(plane + 2); if ((sint32)value < 0) return 0; }
    first_x = (uint32)(sint32)r_s16(first_vertex); first_y = (uint32)(sint32)r_s16(first_vertex + 4);
    second_x = (uint32)(sint32)r_s16(second_vertex); second_y = (uint32)(sint32)r_s16(second_vertex + 4);
    first = (first_x & 0xFFFF) | (first_y << 16); second = (second_x & 0xFFFF) | (second_y << 16);
    value = (uint32)(sint32)r_s16(plane + 2); nonnegative = (sint32)value >= 0;
    second_y = (uint32)(sint32)r_s16(second_vertex + 4); first_y = (uint32)(sint32)r_s16(first_vertex + 4);
    second_x = (uint32)(sint32)r_s16(second_vertex); first_x = (uint32)(sint32)r_s16(first_vertex);
    corner = moh_contact_corner_native(first_x, first_y, second_x, second_y, px, pz, radius, radius, nonnegative);
    moh_contact_edge_native(first, second, corner);
    third_x = (uint32)(sint32)r_s16(third_vertex); third_y = (uint32)(sint32)r_s16(third_vertex + 4);
    third = (third_x & 0xFFFF) | (third_y << 16); result = xport_gte_read_data(24);
    if (nonnegative ? (sint32)result < 0 : (sint32)result > 0) return 0;
    third_y = (uint32)(sint32)r_s16(third_vertex + 4); second_y = (uint32)(sint32)r_s16(second_vertex + 4);
    third_x = (uint32)(sint32)r_s16(third_vertex); second_x = (uint32)(sint32)r_s16(second_vertex);
    corner = moh_contact_corner_native(second_x, second_y, third_x, third_y, px, pz, radius, radius, nonnegative);
    moh_contact_edge_native(second, third, corner);
    first_y = (uint32)(sint32)r_s16(first_vertex + 4); third_y = (uint32)(sint32)r_s16(third_vertex + 4);
    first_x = (uint32)(sint32)r_s16(first_vertex); third_x = (uint32)(sint32)r_s16(third_vertex);
    corner = moh_contact_corner_native(third_x, third_y, first_x, first_y, px, pz, radius, radius, nonnegative);
    result = xport_gte_read_data(24);
    if (nonnegative ? (sint32)result < 0 : (sint32)result > 0) return 0;
    moh_contact_edge_native(third, first, corner); result = xport_gte_read_data(24);
    return nonnegative ? (sint32)result >= 0 : (sint32)result <= 0;
}

void level2p_sub_80071C58(void)
{
    uint32 count, excluded, index, output, record;
    FUNCTION_MARKER(0x80071C58, "LEVEL2P.BIN");
    count = r_u32(0x80094C64); w_u32(0x800B60C0, 0);
    if ((sint32)count <= 0) return;
    excluded = r_u32(0x800A94C8); record = 0x8008D0B8;
    /* TODO: Original source count and output table span are unchecked */
    for (index = 0; (sint32)index < (sint32)count; ++index, record += 0x28) {
        if (index != excluded) {
            output = r_u32(0x800B60C0);
            w_u32(0x800B60D0 + output * 4, record); w_u32(0x800B60C0, output + 1);
        }
    }
}

uint32 level2p_sub_8007377C(uint32 plane, uint32 position, uint32 height, uint32 radius)
{
    uint32 value, component, total, normal;
    FUNCTION_MARKER(0x8007377C, "LEVEL2P.BIN");
    value = r_u32(position); normal = (uint32)(sint32)r_s16(plane);
    total = normal * (uint32)((sint32)value >> 15);
    value = r_u32(position + 8); normal = (uint32)(sint32)r_s16(plane + 4);
    total += normal * (uint32)((sint32)value >> 15);
    normal = (uint32)(sint32)r_s16(plane + 2);
    value = r_u32(position + 4); component = (uint32)((sint32)value >> 15);
    if ((sint32)normal >= 0) component -= height << 1;
    normal = (uint32)(sint32)r_s16(plane + 2);
    component *= normal; value = (uint32)(sint32)r_s16(plane + 6);
    return total + component + (value << 16) - (radius << 16);
}

void level2p_sub_800737FC(uint32 plane, uint32 position, uint32 previous_position, uint32 height, uint32 radius)
{
    uint32 distance, normal, move_x, move_z, x, z, previous;
    FUNCTION_MARKER(0x800737FC, "LEVEL2P.BIN");
    distance = level2p_sub_8007377C(plane, position, height, radius); distance = (uint32)((sint32)distance >> 8);
    normal = r_u16(plane); normal <<= 16; move_x = (uint32)((sint32)normal >> 23) * distance;
    normal = r_u16(plane + 4); normal <<= 16; move_z = (uint32)((sint32)normal >> 23) * distance;
    x = r_u32(position); previous = r_u32(previous_position); x -= previous; x -= move_x;
    z = r_u32(position + 8); previous = r_u32(previous_position + 8); z -= previous; z -= move_z;
    previous = r_u32(previous_position); w_u32(position, x + previous);
    previous = r_u32(previous_position + 8); w_u32(position + 8, z + previous);
}

uint32 level2p_sub_800738CC(uint32 begin, uint32 count, uint32 previous_position, uint32 position, uint32 radius, uint32 height, uint32 planes, uint32 distances)
{
    uint32 found = 0xFFFFFFFF, closest = 0x7FFFFFFF, plane, value, result;
    FUNCTION_MARKER(0x800738CC, "LEVEL2P.BIN");
    /* TODO: Original signed indices and plane/output spans are unchecked */
    planes += begin * 4; distances += begin * 2;
    while ((sint32)begin < (sint32)count) {
        plane = r_u32(planes);
        if (plane) {
            value = r_u16(distances); w_u16(plane + 6, value);
            result = level2p_sub_8007377C(plane, position, height, radius);
            if ((sint32)result < 0) {
                result = level2p_sub_8007377C(plane, previous_position, height, 0);
                if ((sint32)result < (sint32)closest) { closest = result; found = begin; }
            }
        }
        distances += 2; begin += 1; planes += 4;
    }
    return found;
}

uint32 level2p_sub_80073370(uint32 point, uint32 radius, uint32 height, uint32 normal, uint32 auxiliary_output, uint32 plane_output, uint32 distance_output, uint32 capacity, uint32 parameter9, uint32 parameter10)
{
    struct { uint16 next, minimum_x, maximum_x, minimum_z, maximum_z; } pending[8];
    uint32 depth = 0, total = 0, x, z, region, tree, node, triangles, minimum_x, maximum_x, minimum_z, maximum_z;
    uint32 value, next, split, count, data, result;
    FUNCTION_MARKER(0x80073370, "LEVEL2P.BIN");
    x = (uint32)(sint32)r_s16(point); region = r_u32(0x800B60E0); z = (uint32)(sint32)r_s16(point + 4);
    minimum_x = (uint32)(sint32)r_s16(region); maximum_x = (uint32)(sint32)r_s16(region + 4);
    minimum_z = (uint32)(sint32)r_s16(region + 2); maximum_z = (uint32)(sint32)r_s16(region + 6);
    tree = r_u32(region + 0x14); triangles = r_u32(region + 0x24); node = tree;
    /* TODO: Original tree pointers and signed child indices are unchecked */
    for (;;) {
        value = (uint32)(sint32)r_s16(node);
        if ((sint32)value < 0) {
            region = r_u32(0x800B60E0); next = (uint32)(sint32)r_s16(node + 2);
            data = r_u32(region + 0x18); data += next * 2;
            value = r_u16(node); count = (uint32)(sint32)(sint16)~value;
            result = level2p_sub_8007303C(point, radius, height, triangles, data, count, normal, parameter9, auxiliary_output, plane_output, distance_output, capacity, total, parameter10);
            if ((sint32)result > 0) {
                total += result;
                if ((sint32)total >= (sint32)capacity) return total;
            }
            if (!depth) return total;
            depth -= 1;
            minimum_x = (uint32)(sint32)(sint16)pending[depth].minimum_x;
            maximum_x = (uint32)(sint32)(sint16)pending[depth].maximum_x;
            minimum_z = (uint32)(sint32)(sint16)pending[depth].minimum_z;
            next = (uint32)(sint32)(sint16)pending[depth].next;
            maximum_z = (uint32)(sint32)(sint16)pending[depth].maximum_z;
            node = tree + next * 8;
            continue;
        }
        next = (uint32)(sint32)r_s16(node + 2);
        if ((sint32)next >= 0) {
            value = r_u16(node + 4); split = minimum_x + value;
            if ((sint32)(x - radius) < (sint32)split) {
                if ((sint32)split < (sint32)(x + radius) && depth < 8) {
                    pending[depth].minimum_x = (uint16)split; pending[depth].maximum_x = (uint16)maximum_x;
                    pending[depth].minimum_z = (uint16)minimum_z; pending[depth].maximum_z = (uint16)maximum_z;
                    value = r_u16(node + 2); pending[depth].next = (uint16)value; depth += 1;
                }
                next = (uint32)(sint32)r_s16(node); maximum_x = split - 1;
            } else minimum_x = split;
        } else {
            value = r_u16(node + 4); split = minimum_z + value;
            if ((sint32)(z - radius) < (sint32)split) {
                if ((sint32)split < (sint32)(z + radius) && depth < 8) {
                    pending[depth].minimum_x = (uint16)minimum_x; pending[depth].maximum_x = (uint16)maximum_x;
                    pending[depth].minimum_z = (uint16)split; pending[depth].maximum_z = (uint16)maximum_z;
                    value = r_u16(node + 2); pending[depth].next = (uint16)~value; depth += 1;
                }
                next = (uint32)(sint32)r_s16(node); maximum_z = split - 1;
            } else { minimum_z = split; next = ~next; }
        }
        node = tree + next * 8;
    }
}

uint32 level2p_sub_800735E8(uint32 x, uint32 z, uint32 index)
{
    uint32 record, minimum, maximum;
    FUNCTION_MARKER(0x800735E8, "LEVEL2P.BIN");
    /* TODO: Original record index and pointer are unchecked */
    record = r_u32(0x800B60D0 + index * 4);
    minimum = (uint32)(sint32)r_s16(record); maximum = (uint32)(sint32)r_s16(record + 4);
    if ((sint32)x < (sint32)minimum || (sint32)maximum < (sint32)x) return 0;
    minimum = (uint32)(sint32)r_s16(record + 2); maximum = (uint32)(sint32)r_s16(record + 6);
    if ((sint32)z < (sint32)minimum || (sint32)maximum < (sint32)z) return 0;
    return record;
}

uint32 level2p_sub_80073654(uint32 x, uint32 z, uint32 radius, uint32 index)
{
    uint32 record, minimum_x, maximum_x, minimum_z, maximum_z, near_x, near_z, first, second, sum, square;
    FUNCTION_MARKER(0x80073654, "LEVEL2P.BIN");
    record = level2p_sub_800735E8(x, z, index);
    if (record) return record;
    /* TODO: Original table index and signed wrapping coordinate range are unchecked */
    record = r_u32(0x800B60D0 + index * 4);
    minimum_x = (uint32)(sint32)r_s16(record); maximum_x = (uint32)(sint32)r_s16(record + 4);
    minimum_z = (uint32)(sint32)r_s16(record + 2); maximum_z = (uint32)(sint32)r_s16(record + 6);
    near_x = minimum_x;
    if ((sint32)minimum_x < (sint32)x) {
        near_x = maximum_x;
        if ((sint32)x < (sint32)maximum_x) {
            first = x - minimum_x; second = maximum_x - x;
            if ((sint32)second >= (sint32)first) near_x = minimum_x;
        }
    }
    near_z = minimum_z;
    if ((sint32)minimum_z < (sint32)z) {
        near_z = maximum_z;
        if ((sint32)z < (sint32)maximum_z) {
            first = z - minimum_z; second = maximum_z - z;
            if ((sint32)second >= (sint32)first) near_z = minimum_z;
        }
    }
    first = z - near_z; second = x - near_x; sum = first * first + second * second; square = radius * radius;
    return (sint32)sum < (sint32)square ? record : 0;
}

uint32 level2p_sub_800739DC(uint32 point, uint32 radius, uint32 height, uint32 normal, uint32 auxiliary_output, uint32 plane_output, uint32 distance_output, uint32 capacity, uint32 parameter9, uint32 parameter10)
{
    uint32 total = 0, index = 0, count, x, z, record, result;
    uint32 auxiliary = auxiliary_output, planes = plane_output, distances = distance_output;
    FUNCTION_MARKER(0x800739DC, "LEVEL2P.BIN");
    count = r_u32(0x800B60C0);
    if ((sint32)count <= 0) return 0;
    /* TODO: Original live count, capacity and output spans are unchecked */
    do {
        x = (uint32)(sint32)r_s16(point); z = (uint32)(sint32)r_s16(point + 4);
        record = level2p_sub_80073654(x, z, radius, index); w_u32(0x800B60E0, record);
        if (record) {
            result = level2p_sub_80073370(point, radius, height, normal, auxiliary, planes, distances, capacity, parameter9, parameter10);
            if (result) {
                total += result; auxiliary = auxiliary_output + total * 2;
                distances = distance_output + total * 2; planes = plane_output + total * 4;
            }
        }
        if ((sint32)total >= (sint32)capacity) break;
        count = r_u32(0x800B60C0); index += 1;
    } while ((sint32)index < (sint32)count);
    return total;
}

static uint32 moh_contact_dot_native(uint32 point, uint32 plane)
{
    uint32 first, second, x, y, z;
    first = r_u32(point); second = r_u32(point + 4);
    xport_gte_write_control(0, first); xport_gte_write_control(1, second);
    x = r_u16(plane); y = r_u16(plane + 2); z = r_u16(plane + 4);
    xport_gte_write_data(9, x); xport_gte_write_data(10, y); xport_gte_write_data(11, z);
    xport_gte_execute(0x41E012);
    return xport_gte_read_data(25);
}

uint32 level2p_sub_80072F00(uint32 point, uint32 radius, uint32 height, uint32 plane)
{
    uint8 shifted[8];
    uint32 shifted_view, mac, normal, constant, product, first_distance, second_distance, value, coordinate;
    FUNCTION_MARKER(0x80072F00, "LEVEL2P.BIN");
    mac = moh_contact_dot_native(point, plane);
    normal = (uint32)(sint32)r_s16(plane + 2); product = height * normal;
    constant = (uint32)(sint32)r_s16(plane + 6); first_distance = (uint32)((sint32)mac >> 15) + constant;
    value = first_distance + (uint32)((sint32)product >> 15);
    if ((sint32)(first_distance ^ value) < 0) return 1;
    if ((sint32)first_distance < 0) return 0;
    /* TODO: Native local mapping and original high padding halfword remain unresolved */
    shifted_view = moh_draft_local_address(shifted, sizeof(shifted));
    normal = (uint32)(sint32)r_s16(plane); coordinate = (uint32)(sint32)r_s16(point);
    value = (sint32)normal > 0 ? coordinate - radius : coordinate + radius; w_u16(shifted_view, value);
    value = r_u16(point + 2); w_u16(shifted_view + 2, value);
    normal = (uint32)(sint32)r_s16(plane + 4); coordinate = (uint32)(sint32)r_s16(point + 4);
    value = (sint32)normal > 0 ? coordinate - radius : coordinate + radius; w_u16(shifted_view + 4, value);
    mac = moh_contact_dot_native(shifted_view, plane);
    normal = (uint32)(sint32)r_s16(plane + 2); product = height * normal;
    constant = (uint32)(sint32)r_s16(plane + 6); second_distance = (uint32)((sint32)mac >> 15) + constant;
    value = second_distance + (uint32)((sint32)product >> 15);
    if ((sint32)(second_distance ^ value) < 0) return 1;
    return (sint32)(second_distance ^ first_distance) < 0;
}

static uint32 moh_contact_word_load_native(uint32 address)
{
    uint32 high, low, shift;
    high = r_u32((address + 3) & 0xFFFFFFFC); low = r_u32(address & 0xFFFFFFFC); shift = (address & 3) * 8;
    return shift ? (low >> shift) | (high << (32 - shift)) : low;
}

static void moh_contact_word_store_native(uint32 address, uint32 value)
{
    uint32 aligned, index, mask, previous;
    aligned = (address + 3) & 0xFFFFFFFC; index = (address + 3) & 3;
    if (index == 3) w_u32(aligned, value);
    else {
        mask = (1 << ((index + 1) * 8)) - 1;
        previous = r_u32(aligned); w_u32(aligned, (previous & ~mask) | (value >> (24 - index * 8)));
    }
    aligned = address & 0xFFFFFFFC; index = address & 3;
    if (!index) w_u32(aligned, value);
    else {
        mask = 0xFFFFFFFF << (index * 8);
        previous = r_u32(aligned); w_u32(aligned, (previous & ~mask) | (value << (index * 8)));
    }
}

uint32 level2p_sub_8007303C(uint32 point, uint32 radius, uint32 height, uint32 triangles, uint32 indices, uint32 index_count, uint32 normal, uint32 mode, uint32 auxiliary_output, uint32 plane_output, uint32 distance_output, uint32 capacity, uint32 previous_count, uint32 filter)
{
    uint32 total = previous_count, item = 0, index, triangle, region, vertices, plane_table, plane_index, plane, tag;
    uint32 first_vertex, second_vertex, third_vertex, first_index, second_index, third_index, result, found, cursor, value, first, second;
    uint32 auxiliary = auxiliary_output + total * 2, distances = distance_output + total * 2;
    uint32 output = plane_output + total * 4, geometry = 0x8009A220 + total * 32;
    FUNCTION_MARKER(0x8007303C, "LEVEL2P.BIN");
    index_count = (uint32)(sint32)(sint16)index_count;
    /* TODO: Original signed triangle/vertex indices and all output spans are unchecked */
    while ((sint32)item < (sint32)index_count) {
        index = (uint32)(sint32)r_s16(indices); triangle = triangles + index * 10;
        plane_index = (uint32)(sint32)r_s16(triangle + 6); region = r_u32(0x800B60E0);
        first_index = (uint32)(sint32)r_s16(triangle); second_index = (uint32)(sint32)r_s16(triangle + 2);
        third_index = (uint32)(sint32)r_s16(triangle + 4); tag = r_u16(triangle + 8);
        vertices = r_u32(region + 0x1C); plane_table = r_u32(region + 0x20);
        plane = plane_table + plane_index * 8;
        first_vertex = vertices + first_index * 8; second_vertex = vertices + second_index * 8; third_vertex = vertices + third_index * 8;
        w_u16(plane + 6, tag);
        result = level2p_sub_80072270(point, radius, height, mode, first_vertex, second_vertex, third_vertex, plane);
        if (!result) goto next_leaf_triangle;
        result = 1;
        if (filter) { value = moh_contact_dot_native(normal, plane); result = value >> 31; }
        if (!result) goto next_leaf_triangle;
        result = level2p_sub_80072F00(point, radius, height, plane);
        if (!result) goto next_leaf_triangle;
        found = 0;
        if ((sint32)total > 0) {
            index = (uint32)(sint32)r_s16(indices); cursor = auxiliary_output;
            while ((sint32)found < (sint32)total) {
                value = (uint32)(sint32)r_s16(cursor);
                if (value == index) break;
                found += 1; cursor += 2;
            }
            if ((sint32)found < (sint32)total) goto next_leaf_triangle;
        }
        if (!filter) {
            first = moh_contact_word_load_native(first_vertex); second = moh_contact_word_load_native(first_vertex + 4);
            moh_contact_word_store_native(geometry, first); moh_contact_word_store_native(geometry + 4, second);
            first = moh_contact_word_load_native(second_vertex); second = moh_contact_word_load_native(second_vertex + 4);
            moh_contact_word_store_native(geometry + 8, first); moh_contact_word_store_native(geometry + 12, second);
            first = moh_contact_word_load_native(third_vertex); second = moh_contact_word_load_native(third_vertex + 4);
            moh_contact_word_store_native(geometry + 16, first); moh_contact_word_store_native(geometry + 20, second);
            first = moh_contact_word_load_native(plane); second = moh_contact_word_load_native(plane + 4);
            moh_contact_word_store_native(geometry + 24, first); moh_contact_word_store_native(geometry + 28, second);
        }
        w_u32(output, plane); output += 4; geometry += 32;
        value = r_u16(triangle + 8); total += 1; w_u16(distances, value);
        value = r_u16(indices); distances += 2; w_u16(auxiliary, value); auxiliary += 2;
        if ((sint32)total >= (sint32)capacity) return total - previous_count;
next_leaf_triangle:
        indices += 2; item += 1;
    }
    return total - previous_count;
}

uint32 level2p_sub_80073B10(uint32 owner, uint32 normal, uint32 previous_position, uint32 contact_output)
{
    uint8 point[8], auxiliary[64], facing[8], movement[8];
    uint32 planes[32];
    uint16 distances[32];
    uint32 point_view, auxiliary_view, facing_view, movement_view, planes_view, distances_view;
    uint32 radius, height, resource, lower_height, query_height, value, second, flags, mode, offset, bound;
    uint32 count, index, slot, plane, tag, x, y, z, first, third, cross_y, magnitude, distance, second_dot;
    uint32 wall_distance = 0x7FFFFFFF, ceiling_distance = 0x7FFFFFFF, floor_distance = 0x7FFFFFFF;
    uint32 category, field, applied, remaining, selected, result;
    FUNCTION_MARKER(0x80073B10, "LEVEL2P.BIN");
    radius = (uint32)(sint32)r_s16(owner + 0x2EC); height = (uint32)(sint32)r_s16(owner + 0x2D6);
    resource = r_u32(owner + 0x38C); lower_height = height - 16; query_height = height - 20;
    /* TODO: Native local mapping and original opaque vector padding remain unresolved */
    point_view = moh_draft_local_address(point, sizeof(point)); auxiliary_view = moh_draft_local_address(auxiliary, sizeof(auxiliary));
    facing_view = moh_draft_local_address(facing, sizeof(facing)); movement_view = moh_draft_local_address(movement, sizeof(movement));
    planes_view = moh_draft_local_address(planes, sizeof(planes)); distances_view = moh_draft_local_address(distances, sizeof(distances));
    value = (uint32)(sint32)r_s16(owner + 0x20E); w_u16(point_view, value);
    value = (uint32)(sint32)r_s16(owner + 0x212); w_u16(point_view + 2, value - lower_height);
    value = (uint32)(sint32)r_s16(owner + 0x216); w_u16(point_view + 4, value);
    w_u32(contact_output, 0); level2p_sub_80071C58();
    mode = 1; bound = radius;
    /* TODO: Original resource indices and gathered count are unchecked */
    if (resource) {
        flags = r_u32(resource + 8); value = r_u32(resource + 4); w_u32(resource + 8, flags & 0xFFFFFFE3);
        if (value & 4) {
            offset = (uint32)(sint32)r_s16(resource + 0xC0); value = r_u32(resource + 0xD8);
            bound = (uint32)(sint32)r_s16(resource + offset * 2 + 0xCC);
            x = r_u16(owner + 0x344); w_u16(facing_view + 2, 0);
            if (value) x = 0 - x;
            w_u16(facing_view, x); z = r_u16(owner + 0x350);
            if (value) z = 0 - z;
            w_u16(facing_view + 4, z);
            offset = (uint32)(sint32)r_s16(resource + 0xC0); offset = resource + offset * 2;
            value = r_u16(offset + 0xD0); w_u16(offset + 0xCC, value);
            if ((sint32)bound < (sint32)radius) bound = radius;
            mode = 0;
        }
    }
    count = level2p_sub_800739DC(point_view, bound, query_height, normal, auxiliary_view, planes_view, distances_view, 32, 1, mode);
    if (!count) return 0;
    if (resource) {
        value = r_u32(resource + 4);
        if (value & 4) {
            for (index = 0; (sint32)index < (sint32)count; ++index) {
                slot = planes_view + index * 4; plane = r_u32(slot);
                if (!plane) continue;
                value = (uint32)(sint32)r_s16(plane + 2); tag = r_u16(distances_view + index * 2);
                magnitude = (sint32)value < 0 ? 0 - value : value; w_u16(plane + 6, tag);
                if ((sint32)magnitude < 0x5A80) {
                    value = moh_contact_dot_native(facing_view, plane);
                    if (((sint32)value >> 12) < 12000) {
                        x = (uint32)(sint32)r_s16(facing_view); y = (uint32)(sint32)r_s16(facing_view + 2);
                        xport_gte_write_control(0, x); z = (uint32)(sint32)r_s16(facing_view + 4);
                        xport_gte_write_control(2, y); xport_gte_write_control(4, z);
                        x = (uint32)(sint32)r_s16(plane); y = (uint32)(sint32)r_s16(plane + 2); z = (uint32)(sint32)r_s16(plane + 4);
                        xport_gte_write_data(9, x); xport_gte_write_data(10, y); xport_gte_write_data(11, z); xport_gte_execute(0x178000C);
                        first = xport_gte_read_data(25); cross_y = xport_gte_read_data(26); third = xport_gte_read_data(27);
                        value = moh_contact_dot_native(point_view, plane); tag = (uint32)(sint32)r_s16(distances_view + index * 2);
                        second_dot = moh_contact_dot_native(point_view, plane); distance = second_dot + tag;
                        if ((sint32)(value + tag) < 0) distance = 0 - distance;
                        magnitude = (sint32)cross_y < 0 ? 0 - cross_y : cross_y;
                        category = 0; field = 0;
                        if ((sint32)magnitude < 0x5001) {
                            if ((sint32)distance < (sint32)wall_distance) { category = 4; field = 0xF4; wall_distance = distance; }
                        } else if ((sint32)cross_y >= 0x5001) {
                            if ((sint32)distance < (sint32)ceiling_distance) { category = 8; field = 0x104; ceiling_distance = distance; }
                        } else if ((sint32)cross_y < -0x5000 && (sint32)distance < (sint32)floor_distance) {
                            category = 0x10; field = 0x114; floor_distance = distance;
                        }
                        if (category) {
                            first = moh_contact_word_load_native(facing_view); second = moh_contact_word_load_native(facing_view + 4);
                            moh_contact_word_store_native(resource + 0xEC, first); moh_contact_word_store_native(resource + 0xF0, second);
                            first = moh_contact_word_load_native(plane); second = moh_contact_word_load_native(plane + 4);
                            moh_contact_word_store_native(resource + field, first); moh_contact_word_store_native(resource + field + 4, second);
                            w_u32(resource + field + 8, distance); flags = r_u32(resource + 8);
                            w_u32(resource + field + 12, cross_y); w_u32(resource + 8, flags | category);
                        }
                        (void)first; (void)third;
                    }
                }
                offset = index * 32;
                result = level2p_sub_80072270(point_view, radius, query_height, 1, 0x8009A220 + offset, 0x8009A228 + offset, 0x8009A230 + offset, plane);
                if (result) {
                    value = moh_contact_dot_native(normal, plane);
                    if ((sint32)value < 0) {
                        result = level2p_sub_80072F00(point_view, radius, query_height, plane);
                        if (result) continue;
                    }
                }
                w_u32(slot, 0);
            }
        }
    }
    applied = 0; remaining = count;
    for (;;) {
        selected = level2p_sub_800738CC(0, count, previous_position, owner + 0x20C, radius, lower_height, planes_view, distances_view);
        if ((sint32)selected < 0) break;
        applied = 1; remaining -= 1; plane = r_u32(planes_view + selected * 4);
        value = r_u16(distances_view + selected * 2); w_u16(plane + 6, value);
        level2p_sub_800737FC(plane, owner + 0x20C, previous_position, lower_height, radius + 4);
        if ((sint32)remaining < 0) break;
    }
    value = (uint32)(sint32)r_s16(owner + 0x20E); second = (uint32)(sint32)r_s16(previous_position + 2); w_u16(movement_view, value - second);
    value = (uint32)(sint32)r_s16(owner + 0x212); second = (uint32)(sint32)r_s16(previous_position + 6); w_u16(movement_view + 2, value - second);
    value = (uint32)(sint32)r_s16(owner + 0x216); second = (uint32)(sint32)r_s16(previous_position + 10); w_u16(movement_view + 4, value - second);
    if (!applied) { w_u32(contact_output, 2); return 0; }
    value = (uint32)(sint32)r_s16(owner + 0x20E); w_u16(point_view, value);
    value = (uint32)(sint32)r_s16(owner + 0x212); w_u16(point_view + 2, value);
    value = (uint32)(sint32)r_s16(owner + 0x216); w_u16(point_view + 4, value);
    count = level2p_sub_800739DC(point_view, radius, query_height, movement_view, auxiliary_view, planes_view, distances_view, 32, 1, 1);
    if (!count) w_u32(contact_output, 2);
    else {
        selected = level2p_sub_800738CC(0, count, previous_position, owner + 0x20C, radius, lower_height, planes_view, distances_view);
        w_u32(contact_output, (sint32)selected < 0 ? 2 : 1);
    }
    return count;
}

void level2p_sub_800678F0(uint32 owner)
{
    uint32 x, y, z, pitch, yaw, roll;
    FUNCTION_MARKER(0x800678F0, "LEVEL2P.BIN");
    x = r_u32(owner + 0x20C); y = r_u32(owner + 0x210); z = r_u32(owner + 0x214);
    pitch = r_u32(owner + 0x228); yaw = r_u32(owner + 0x22C); roll = r_u32(owner + 0x230);
    w_u32(owner + 0x258, x); w_u32(owner + 0x25C, y); w_u32(owner + 0x260, z);
    w_u32(owner + 0x264, pitch); w_u32(owner + 0x268, yaw); w_u32(owner + 0x26C, roll);
}

void level2p_sub_80065B2C(uint32 owner, uint32 output)
{
    uint8 angles[8], translation[16];
    uint32 angles_view, translation_view, value;
    FUNCTION_MARKER(0x80065B2C, "LEVEL2P.BIN");
    /* TODO: Native local mapping remains a fail-fast boundary */
    angles_view = moh_draft_local_address(angles, sizeof(angles)); translation_view = moh_draft_local_address(translation, sizeof(translation));
    moh_draft_missing_call(0x800158AC, angles_view, 0, 8);
    moh_draft_missing_call(0x800158AC, translation_view, 0, 16);
    value = (uint32)(sint32)r_s16(owner + 0x22A); w_u16(angles_view, value);
    value = (uint32)(sint32)r_s16(owner + 0x22E); w_u16(angles_view + 2, value);
    value = (uint32)(sint32)r_s16(owner + 0x232); w_u16(angles_view + 4, value);
    moh_draft_missing_call(0x8001AF98, angles_view, output);
    moh_draft_missing_call(0x80016218, output, translation_view);
}

uint32 level2p_sub_80067924(uint32 owner)
{
    uint8 velocity[8], matrix[32], normal[8], impact[8];
    uint32 saved_position[3], rotation[3], contact, ray_distance;
    uint32 velocity_view, matrix_view, normal_view, impact_view, saved_view, rotation_view, contact_view, ray_distance_view;
    uint32 saved_x, saved_y, saved_z, old_grounded, result = 0, collision = 0;
    uint32 x, y, z, value, second, third, delta, dx, dy, dz, scale, flags, roles, state;
    uint32 model, animation, frame, table, entry, offset, mask, mode, player, admitted, reduced, limit;
    uint32 first, fourth, fifth, sixth, seventh, eighth, player_contact, generic_contact, ray_contact, contact_kind, height, difference, vertical;
    FUNCTION_MARKER(0x80067924, "LEVEL2P.BIN");
    level2p_sub_800678F0(owner);
    saved_x = r_u32(owner + 0x20C); saved_y = r_u32(owner + 0x210); saved_z = r_u32(owner + 0x214);
    x = r_u32(owner + 0x228); delta = r_u32(owner + 0x234);
    dy = r_u32(owner + 0x238); dz = r_u32(owner + 0x23C); old_grounded = r_u32(owner + 0x284);
    z = r_u32(owner + 0x230); w_u32(owner + 0x228, x + delta);
    y = r_u32(owner + 0x22C); w_u32(owner + 0x230, z + dz);
    model = r_u32(owner + 0x100); w_u32(owner + 0x22C, y + dy);
    /* TODO: Original animation/resource indices and player pointers are unchecked */
    if (model) {
        frame = (uint32)(sint32)r_s16(model + 0x5E);
        if ((sint32)frame >= 0) {
            animation = (uint32)(sint32)r_s16(model + 0x5C);
            if ((sint32)animation >= 0) {
                mode = r_u32(model + 0x74);
                if (mode) {
                    table = r_u32(model + 0x78); entry = r_u32(table + animation * 4);
                    entry = r_u32(entry + 4); entry += frame * 20 + 8;
                } else {
                    table = r_u32(model + 8); table = r_u32(table + 0x98);
                    entry = r_u32(table + animation * 16 + 8); entry += frame * 24 + 16;
                }
                value = r_u32(owner + 8);
                offset = value == 1 ? 0x240 : 0x228;
                mask = value == 1 ? 0x0FFFFFFF : 0x0FFF0000;
                value = (uint32)(sint32)r_s16(entry); x = r_u32(owner + offset);
                w_u32(owner + offset, x + (value << 16));
                value = (uint32)(sint32)r_s16(entry + 2); y = r_u32(owner + offset + 4); y += value << 16;
                x = r_u32(owner + offset); w_u32(owner + offset + 4, y);
                value = (uint32)(sint32)r_s16(entry + 4); w_u32(owner + offset, x & mask);
                z = r_u32(owner + offset + 8); w_u32(owner + offset + 8, z + (value << 16));
                y = r_u32(owner + offset + 4); z = r_u32(owner + offset + 8);
                w_u32(owner + offset + 4, y & mask); w_u32(owner + offset + 8, z & mask);
            }
        }
    }
    delta = r_u32(owner + 0x1FC);
    if (delta) {
        value = r_u32(owner + 0x200); limit = r_u32(owner + 0x204); value += delta;
        if ((sint32)limit < (sint32)value) value = limit;
        w_u32(owner + 0x200, value);
    }
    value = r_u32(owner + 0x200); mode = r_u32(0x80099400); w_u32(owner + 0x220, value);
    if (mode) {
        admitted = 0; player = r_u32(0x8009943C); state = r_u32(player + 0x388); flags = r_u32(state + 0x72C);
        if (flags & 0x4000) {
            if (owner == player) admitted = 1;
            else { roles = r_u32(owner + 0x10); if (roles & 0x1000) admitted = 1; }
        }
        player = r_u32(0x80099440); state = r_u32(player + 0x388); flags = r_u32(state + 0x72C);
        if (flags & 0x4000) {
            if (owner == player) admitted = 1;
            else { roles = r_u32(owner + 0x10); if (roles & 0x2000) admitted = 1; }
        }
        if (admitted) {
            mode = r_u32(0x80099400);
            switch (mode) {
            case 2: value = r_u32(owner + 0x1F0); w_u32(owner + 0x1F0, value + 0xA0000); break;
            case 3: value = r_u32(owner + 0x1F0); w_u32(owner + 0x1F0, value - 0xA0000); break;
            case 4: value = r_u32(owner + 0x1F8); w_u32(owner + 0x1F8, value + 0xA0000); break;
            case 5: value = r_u32(owner + 0x1F8); w_u32(owner + 0x1F8, value - 0xA0000); break;
            default: break;
            }
        }
    }
    value = r_u32(owner + 0x284);
    if (!value) {
        flags = r_u32(owner + 0xC);
        if (flags & 0x20) {
            reduced = 0;
            if (flags & 1) { state = r_u32(owner + 0x388); value = r_u32(state + 0x72C); reduced = value & 0x100; }
            value = r_u32(owner + 0x21C);
            if (reduced) { value += 0x2000; limit = 0x4B000; }
            else { value += 0x20000; limit = 0x4B0000; }
            w_u32(owner + 0x21C, value);
            if ((sint32)limit < (sint32)value) w_u32(owner + 0x21C, limit);
        }
    }
    scale = r_u32(owner + 0x224);
    if (scale == 0x10000) {
        x = r_u32(owner + 0x218); z = r_u32(owner + 0x220); y = r_u32(owner + 0x21C);
        x >>= 12; z >>= 12; y >>= 12;
    } else {
        value = (uint32)(sint32)r_s16(owner + 0x21A); x = value * scale;
        value = (uint32)(sint32)r_s16(owner + 0x21E); y = value * scale;
        value = (uint32)(sint32)r_s16(owner + 0x222); z = value * scale;
        x >>= 12; y >>= 12; z >>= 12;
    }
    /* TODO: Native local mapping and original opaque vector padding remain unresolved */
    velocity_view = moh_draft_local_address(velocity, sizeof(velocity)); matrix_view = moh_draft_local_address(matrix, sizeof(matrix));
    normal_view = moh_draft_local_address(normal, sizeof(normal)); impact_view = moh_draft_local_address(impact, sizeof(impact));
    saved_view = moh_draft_local_address(saved_position, sizeof(saved_position)); rotation_view = moh_draft_local_address(rotation, sizeof(rotation));
    contact_view = moh_draft_local_address(&contact, sizeof(contact)); ray_distance_view = moh_draft_local_address(&ray_distance, sizeof(ray_distance));
    w_u32(saved_view, saved_x); w_u32(saved_view + 4, saved_y); w_u32(saved_view + 8, saved_z);
    w_u16(velocity_view, x); w_u16(velocity_view + 2, y); w_u16(velocity_view + 4, z);
    flags = r_u32(owner + 0xC);
    if (flags & 1) {
        state = r_u32(owner + 0x388); w_u32(rotation_view, 0); value = r_u32(state + 0x800);
        w_u32(rotation_view + 4, value); value = r_u32(owner + 0x230); w_u32(rotation_view + 8, value);
        level2p_sub_8008BB44(owner + 0x20C, rotation_view, owner + 0x270, matrix_view);
    } else level2p_sub_80065B2C(owner, matrix_view);
    first = r_u32(matrix_view); second = r_u32(matrix_view + 4);
    xport_gte_write_control(0, first); xport_gte_write_control(1, second);
    third = r_u32(matrix_view + 8); fourth = r_u32(matrix_view + 12); fifth = r_u32(matrix_view + 16);
    xport_gte_write_control(2, third); xport_gte_write_control(3, fourth); xport_gte_write_control(4, fifth);
    sixth = r_u32(matrix_view + 20); seventh = r_u32(matrix_view + 24);
    xport_gte_write_control(5, sixth); eighth = r_u32(matrix_view + 28);
    xport_gte_write_control(6, seventh); xport_gte_write_control(7, eighth);
    first = r_u32(velocity_view); xport_gte_write_data(0, first);
    second = r_u32(velocity_view + 4); xport_gte_write_data(1, second); xport_gte_execute(0x486012);
    dx = xport_gte_read_data(25); dy = xport_gte_read_data(26); dz = xport_gte_read_data(27);
    delta = r_u32(owner + 0x1F0); value = r_u32(owner + 0x20C); w_u32(owner + 0x20C, value + (dx << 12) + delta);
    delta = r_u32(owner + 0x1F4); value = r_u32(owner + 0x210); w_u32(owner + 0x210, value + (dy << 12) + delta);
    delta = r_u32(owner + 0x1F8); value = r_u32(owner + 0x214); w_u32(owner + 0x214, value + (dz << 12) + delta);
    vertical = (uint32)((sint32)dy >> 4);
    model = r_u32(0x80099430); mask = r_u32(owner + 0x14); flags = r_u32(model + 0xC);
    if (!(mask & flags)) goto movement_done;
    value = (uint32)(sint32)r_s16(owner + 0x20E); second = (uint32)(sint32)r_s16(owner + 0x3C2); w_u16(normal_view, value - second);
    value = (uint32)(sint32)r_s16(owner + 0x212); second = (uint32)(sint32)r_s16(owner + 0x3C6); w_u16(normal_view + 2, value - second);
    value = (uint32)(sint32)r_s16(owner + 0x216); second = (uint32)(sint32)r_s16(owner + 0x3CA); w_u16(normal_view + 4, value - second);
    player_contact = 0; generic_contact = 0; ray_contact = 0;
    flags = r_u32(owner + 0xC); w_u32(contact_view, 0);
    if (flags & 8) {
        if (flags & 1) player_contact = 1;
        else {
            roles = r_u32(owner + 0x10);
            if (!(roles & 1)) { if (roles & 0x44) ray_contact = 1; else generic_contact = 1; }
        }
    }
    if (player_contact || generic_contact) {
        value = level2p_sub_80073B10(owner, normal_view, owner + 0x3C0, contact_view);
        collision = (sint32)value > 0;
    } else if (ray_contact) {
        value = level2p_sub_8003F26C(owner + 0x20C, saved_view, normal_view, impact_view, ray_distance_view, owner + 0x318, 0);
        collision = (~value) >> 31;
    }
    contact_kind = r_u32(contact_view);
    if (player_contact && contact_kind == 1) {
        x = r_u32(owner + 0x3C0); y = r_u32(owner + 0x3C4); z = r_u32(owner + 0x3C8);
        w_u32(owner + 0x20C, x); w_u32(owner + 0x210, y); w_u32(owner + 0x214, z); w_u32(owner + 0x284, old_grounded);
    } else if (contact_kind == 2) {
        flags = r_u32(owner + 0x38); result |= 2; w_u32(owner + 0x38, flags | 0x8000);
    }
    if (collision) {
        value = (uint32)(sint32)r_s16(normal_view + 2);
        if ((sint32)value > 0x800 && (sint32)vertical < 0) {
            value = r_u32(owner + 0x21C);
            if ((sint32)value > 0) { value = r_u32(owner + 0x3C4); w_u32(owner + 0x21C, 0); w_u32(owner + 0x210, value); }
        }
        result |= 2; w_u32(owner + 0x218, 0); w_u32(owner + 0x220, 0);
        first = r_u32(normal_view); second = r_u32(normal_view + 4);
        w_u32(owner + 0x364, first); w_u32(owner + 0x368, second);
        flags = r_u32(owner + 0x38);
        if (!player_contact) {
            roles = r_u32(owner + 0xC); w_u32(owner + 0x38, flags | 0x8000);
            if (roles & 0x80) {
                value = (uint32)(sint32)r_s16(normal_view); x = r_u32(saved_view);
                x += ((value << 5) + 0x8000) & 0xFFFF0000; w_u32(owner + 0x20C, x);
                value = (uint32)(sint32)r_s16(normal_view + 4); z = r_u32(saved_view + 8);
                z += ((value << 5) + 0x8000) & 0xFFFF0000; w_u32(owner + 0x214, z);
            } else {
                roles = r_u32(owner + 0x10);
                if (roles & 0x44) {
                    value = (uint32)(sint32)r_s16(impact_view); w_u32(owner + 0x20C, value << 16);
                    value = (uint32)(sint32)r_s16(impact_view + 2); w_u32(owner + 0x210, value << 16);
                    value = (uint32)(sint32)r_s16(impact_view + 4); w_u32(owner + 0x214, value << 16);
                } else {
                    value = r_u32(contact_view);
                    if ((sint32)value < 0) {
                        x = r_u32(owner + 0x3C0); y = r_u32(owner + 0x3C4); z = r_u32(owner + 0x3C8);
                        w_u32(owner + 0x20C, x); w_u32(owner + 0x210, y); w_u32(owner + 0x214, z); w_u32(owner + 0x284, old_grounded);
                    }
                }
            }
        } else w_u32(owner + 0x38, flags | 0x8000);
    }
    flags = r_u32(owner + 0x18);
    if (flags & 1) goto movement_done;
    roles = r_u32(owner + 0x10);
    if (roles & 0x10) goto movement_done;
    value = r_u32(owner + 0x284);
    if (!value) { flags = r_u32(owner + 0xC); if (!(flags & 8)) goto movement_done; }
    height = level2p_sub_8005BDE4(owner, velocity_view); w_u16(0x800B5CF8, height); height = (uint32)(sint32)(sint16)height;
    if ((sint32)height == -0x7FFF) {
        flags = r_u32(owner + 0xC);
        if (!(flags & 7)) goto movement_done;
        roles = r_u32(owner + 0x10);
        if (roles & 0x10000) goto movement_done;
        x = r_u32(owner + 0x3C0); y = r_u32(owner + 0x3C4); z = r_u32(owner + 0x3C8);
        w_u32(owner + 0x20C, x); w_u32(owner + 0x210, y); w_u32(owner + 0x214, z);
        if (!old_grounded) { value = r_u32(owner + 0x208); w_u32(owner + 0x210, value); }
        result |= 2; flags = r_u32(owner + 0x38); w_u32(owner + 0x284, 1); w_u32(owner + 0x38, flags | 0x80008000);
        goto movement_done;
    }
    value = (uint32)(sint32)r_s16(owner + 0x212); state = r_u32(owner + 0x284);
    w_u32(owner + 0x208, height << 16); difference = value - height;
    if (state) {
        if (difference + 32 < 0x121 || (sint32)difference >= 0) { w_u32(owner + 0x210, height << 16); goto movement_done; }
        roles = r_u32(owner + 0x10);
        if ((roles & 0x200000) && (sint32)difference < -57) {
            result |= 0x80;
            w_u32(owner + 0x220, 0); w_u32(owner + 0x21C, 0); w_u32(owner + 0x218, 0); w_u32(owner + 0x200, 0);
            x = r_u32(owner + 0x3C0); y = r_u32(owner + 0x3C4); z = r_u32(owner + 0x3C8);
            w_u32(owner + 0x20C, x); w_u32(owner + 0x210, y); w_u32(owner + 0x214, z);
            flags = r_u32(owner + 0x38); w_u32(owner + 0x284, old_grounded); w_u32(owner + 0x38, flags | 0x80000000);
        } else {
            flags = r_u32(owner + 0x38); w_u32(owner + 0x284, 0); w_u32(owner + 0x38, flags | 1);
        }
        goto movement_done;
    }
    flags = r_u32(owner + 0xC);
    if (!(flags & 0x20)) goto movement_done;
    if ((sint32)difference < 0) {
        difference = 0 - difference; limit = 64;
        if ((sint32)vertical < 48) limit = vertical + 16;
        if ((sint32)limit < (sint32)difference) goto movement_done;
    }
    flags = r_u32(owner + 0xC);
    if (flags & 1) { if ((sint32)vertical < 0) goto movement_done; }
    else if (flags & 0x80) {
        value = (sint32)vertical < 0 ? 0 - vertical : vertical;
        if ((sint32)value >= 8) {
            if (!collision) {
                first = r_u32(velocity_view); second = r_u32(velocity_view + 4);
                w_u32(owner + 0x364, first); w_u32(owner + 0x368, second);
            }
            height = (uint32)(sint32)r_s16(0x800B5CF8); value = r_u32(owner + 0x21C);
            result |= 1; w_u32(owner + 0x284, 0);
            if ((sint32)value < 0) value = 0 - value;
            w_u32(owner + 0x210, height << 16); w_u32(owner + 0x21C, 0 - value);
            goto movement_done;
        }
    }
    result |= 1; height = (uint32)(sint32)r_s16(0x800B5CF8);
    w_u32(owner + 0x284, 1); w_u32(owner + 0x210, height << 16);
    flags = r_u32(owner + 0x38); w_u32(owner + 0x1F4, 0); w_u32(owner + 0x21C, 0); w_u32(owner + 0x38, flags & 0xFFFFFFFE);
movement_done:
    w_u32(owner + 0x1F4, 0); w_u32(owner + 0x1F0, 0); w_u32(owner + 0x1F8, 0);
    x = r_u32(owner + 0x20C); y = r_u32(owner + 0x210); z = r_u32(owner + 0x214);
    w_u32(owner + 0x3C0, x); w_u32(owner + 0x3C4, y); w_u32(owner + 0x3C8, z);
    return result;
}

void level2p_sub_80067404(uint32 head)
{
    uint8 direction[8], distance[8];
    uint32 owner_shape, target_shape;
    uint32 direction_view, distance_view, owner_shape_view, target_shape_view;
    uint32 owner = head, next_owner, target, next_target, flags, roles, value, mask, callback, result;
    uint32 x, y, z, previous, first, second, third, squared_length, distance_output, first_shape, second_shape;
    FUNCTION_MARKER(0x80067404, "LEVEL2P.BIN");
    if (!head) return;
    /* TODO: Native local mapping, opaque vector padding and original acyclic lists */
    direction_view = moh_draft_local_address(direction, sizeof(direction)); distance_view = moh_draft_local_address(distance, sizeof(distance));
    owner_shape_view = moh_draft_local_address(&owner_shape, sizeof(owner_shape)); target_shape_view = moh_draft_local_address(&target_shape, sizeof(target_shape));
    while (owner) {
        flags = r_u32(owner + 0xC); next_owner = r_u32(owner + 4);
        if (flags & 0x80000) goto next_collision_owner;
        roles = r_u32(owner + 0x10);
        if (roles & 2) goto next_collision_owner;
        if (!(roles & 4)) {
            target = next_owner;
            while (target) {
                flags = r_u32(target + 0xC); next_target = r_u32(target + 4);
                if (flags & 0x80000) goto next_pair;
                value = r_u32(target + 0x10);
                if (value & 6) goto next_pair;
                mask = r_u32(owner + 0x14);
                if (!(mask & flags)) goto next_pair;
                result = level2p_sub_8004D63C(owner, target);
                if (!result) goto next_pair;
                callback = r_u32(owner + 0x37C);
                if (callback) moh_draft_indirect_call(callback, owner, target);
                callback = r_u32(target + 0x37C);
                if (callback) moh_draft_indirect_call(callback, target, owner);
                level2p_sub_8004DFBC(owner); level2p_sub_8004DFBC(target);
                result = level2p_sub_8004D4B4(owner, owner_shape_view, target, target_shape_view);
                if (!result) goto next_pair;
                callback = r_u32(owner + 0x374);
                if (callback) {
                    first_shape = r_u32(owner_shape_view); second_shape = r_u32(target_shape_view);
                    moh_draft_indirect_call(callback, owner, first_shape, target, second_shape);
                }
                callback = r_u32(target + 0x374);
                if (callback) {
                    second_shape = r_u32(target_shape_view); first_shape = r_u32(owner_shape_view);
                    moh_draft_indirect_call(callback, target, second_shape, owner, first_shape);
                }
                flags = r_u32(owner + 0xC);
                if (flags & 0x80000) goto next_collision_owner;
next_pair:
                target = next_target;
            }
            goto next_collision_owner;
        }
        distance_output = roles & 0x80000 ? distance_view : 0;
        x = r_u32(owner + 0x20C); previous = r_u32(owner + 0x258); x -= previous;
        w_u16(direction_view, (uint32)((sint32)x >> 16));
        y = r_u32(owner + 0x210); previous = r_u32(owner + 0x25C); y -= previous;
        w_u16(direction_view + 2, (uint32)((sint32)y >> 16));
        z = r_u32(owner + 0x214); previous = r_u32(owner + 0x260); z -= previous;
        w_u16(direction_view + 4, (uint32)((sint32)z >> 16));
        x = r_u16(direction_view); y = r_u16(direction_view + 2); z = r_u16(direction_view + 4);
        xport_gte_write_data(9, x); xport_gte_write_data(10, y); xport_gte_write_data(11, z);
        xport_gte_execute(0xA00428);
        first = xport_gte_read_data(25); second = xport_gte_read_data(26); third = xport_gte_read_data(27);
        squared_length = first + second + third;
        moh_draft_missing_call(0x80016A40, direction_view, direction_view);
        x = (uint32)(sint32)r_s16(direction_view);
        if (!x) {
            y = (uint32)(sint32)r_s16(direction_view + 2);
            if (!y) {
                z = (uint32)(sint32)r_s16(direction_view + 4);
                if (!z) {
                    value = r_u16(owner + 0x344); w_u16(direction_view, value);
                    value = r_u16(owner + 0x34A); w_u16(direction_view + 2, value);
                    value = r_u16(owner + 0x350); w_u16(direction_view + 4, value);
                }
            }
        }
        target = head;
        while (target) {
            flags = r_u32(target + 0xC); next_target = r_u32(target + 4);
            if (flags & 0x80000) goto next_projectile_pair;
            value = r_u32(target + 0x10);
            if (value & 6) goto next_projectile_pair;
            mask = r_u32(owner + 0x14);
            if (!(mask & flags) || target == owner) goto next_projectile_pair;
            result = level2p_sub_8004D8B4(owner, target, direction_view, distance_output);
            if (!result) goto next_projectile_pair;
            callback = r_u32(owner + 0x37C);
            if (callback) moh_draft_indirect_call(callback, owner, target);
            level2p_sub_8004DFBC(owner); level2p_sub_8004DFBC(target);
            result = level2p_sub_8004D48C(owner, direction_view, squared_length, target, target_shape_view);
            if (!result) goto next_projectile_pair;
            callback = r_u32(owner + 0x374);
            if (callback) {
                second_shape = r_u32(target_shape_view);
                moh_draft_indirect_call(callback, owner, 0xFFFFFFFF, target, second_shape);
            }
            callback = r_u32(target + 0x374);
            if (callback) {
                second_shape = r_u32(target_shape_view);
                moh_draft_indirect_call(callback, target, second_shape, owner, 0xFFFFFFFF);
            }
            flags = r_u32(owner + 0xC);
            if (flags & 0x80000) goto next_collision_owner;
next_projectile_pair:
            target = next_target;
        }
        flags = r_u32(owner + 0xC);
        if (!(flags & 0x80000)) {
            roles = r_u32(owner + 0x10);
            if (roles & 8) {
                /* TODO: Original completion callback is called without a null guard */
                callback = r_u32(owner + 0x378); moh_draft_indirect_call(callback, owner, 0);
            }
        }
next_collision_owner:
        owner = next_owner;
    }
}

void level2p_sub_80070378(uint32 player, uint32 axis, uint32 source, uint32 inverted)
{
    uint32 record;
    FUNCTION_MARKER(0x80070378, "LEVEL2P.BIN");
    record = 0x800B5D48 + player * 0xA0 + axis * 0x28;
    w_u32(record, source); w_u32(record + 0x24, inverted);
}

void level2p_sub_8006FED0(uint32 player, uint32 axis, uint32 positive, uint32 negative, uint32 inverted)
{
    uint32 record;
    FUNCTION_MARKER(0x8006FED0, "LEVEL2P.BIN");
    record = 0x800B5E88 + player * 0xD0 + axis * 0x34;
    w_u32(record, positive); w_u32(record + 4, negative); w_u32(record + 0x20, inverted);
}

uint32 level2p_sub_800717E4(uint32 player, uint32 layout, uint32 inverted, uint32 mode)
{
    uint32 first = 0xFFFFFFFF, second = 0xFFFFFFFF, third = 0xFFFFFFFF, fourth = 0xFFFFFFFF;
    uint32 first_positive = 0xFFFFFFFF, first_negative = 0xFFFFFFFF, second_positive = 0xFFFFFFFF, second_negative = 0xFFFFFFFF;
    uint32 third_positive = 0xFFFFFFFF, third_negative = 0xFFFFFFFF, fourth_positive = 0xFFFFFFFF, fourth_negative = 0xFFFFFFFF;
    uint32 common_inverted = 0, third_inverted = inverted, value;
    FUNCTION_MARKER(0x800717E4, "LEVEL2P.BIN");
    if (mode == 2) {
        third_inverted = sub_8001552C() & 1; common_inverted = sub_8001552C() & 1;
        first = sub_8001552C() & 3;
        do { second = sub_8001552C() & 3; } while (second == first);
        do { third = sub_8001552C() & 3; } while (third == first || third == second);
        do { fourth = sub_8001552C() & 3; } while (fourth == first || fourth == second || fourth == third);
        if (first == 1 || first == 3) { first_positive = 0x1000; first_negative = 0x4000; }
        else { first_positive = 0x8000; first_negative = 0x2000; }
        third_positive = first_positive; third_negative = first_negative;
        if (second < 2) { second_positive = 4; second_negative = 8; }
        else if (second == 2) { second_positive = 0x8000; second_negative = 0x2000; }
        else { second_positive = 0x1000; second_negative = 0x4000; }
        if (fourth == 1 || fourth == 3) { fourth_positive = 0x1000; fourth_negative = 0x4000; }
        else { fourth_positive = 0x8000; fourth_negative = 0x2000; }
    } else {
        if (mode == 1) { third_inverted = !inverted; common_inverted = 1; }
        if (layout == 0xFFFFFFFF) {
            value = (uint32)(sint32)r_s8(0x8008FE58 + player);
            return level2p_sub_80070980(player, value ? 3 : 2, 0);
        }
        if (layout < 6) {
            first = 3; third = 1;
            second = layout == 1 || layout == 2 ? 0 : 2;
            fourth = layout == 1 || layout == 2 ? 2 : 0;
            first_positive = 0x1000; first_negative = 0x4000;
            third_positive = 0x1000; third_negative = 0x4000;
            if (layout == 1 || layout == 5) { second_positive = 4; second_negative = 8; }
            else { second_positive = 0x8000; second_negative = 0x2000; }
            if (layout == 4) { fourth_positive = 4; fourth_negative = 8; }
            else { fourth_positive = 0x8000; fourth_negative = 0x2000; }
        }
    }
    /* TODO: Original unknown layouts pass FFFFFFFF axes and masks through unchecked indices */
    level2p_sub_80070378(player, 0, first, common_inverted);
    level2p_sub_80070378(player, 1, second, common_inverted);
    level2p_sub_80070378(player, 2, third, third_inverted);
    level2p_sub_80070378(player, 3, fourth, common_inverted);
    level2p_sub_8006FED0(player, 0, first_positive, first_negative, common_inverted);
    level2p_sub_8006FED0(player, 1, second_positive, second_negative, common_inverted);
    level2p_sub_8006FED0(player, 2, third_positive, third_negative, third_inverted);
    level2p_sub_8006FED0(player, 3, fourth_positive, fourth_negative, common_inverted);
    return 0;
}

void level2p_sub_8007045C(uint32 player, uint32 axis, uint32 value, uint32 mode)
{
    uint32 record, saved, low, middle, high, denominator, first_scale, second_scale;
    FUNCTION_MARKER(0x8007045C, "LEVEL2P.BIN");
    /* TODO: Original player and axis indices are unchecked */
    record = 0x800B5D48 + player * 0xA0 + axis * 0x28;
    switch (mode) {
    case 2: value = r_u32(record + 0x1C); break;
    case 3: saved = r_u32(record + 0x1C); value *= saved; break;
    case 4: saved = r_u32(record + 0x1C); value = (uint32)((sint32)saved >> (value & 31)); break;
    case 5: saved = r_u32(record + 0x1C); value = saved << (value & 31); break;
    default: break;
    }
    low = r_u8(record + 0xC); middle = r_u8(record + 0xE); denominator = (middle - low) & 0xFF;
    if (!denominator) denominator = 1;
    first_scale = 0x7FFF / denominator;
    high = r_u8(record + 0xD); middle = r_u8(record + 0xE); denominator = (high - middle) & 0xFF;
    w_u32(record + 0x14, first_scale);
    if (!denominator) denominator = 1;
    second_scale = 0x7FFF / denominator; w_u32(record + 0x18, second_scale);
    if (!mode) w_u32(record + 0x1C, value);
    w_u32(record + 0x20, value);
}

void level2p_sub_8006FFC0(uint32 player, uint32 axis, uint32 value, uint32 mode)
{
    uint32 record, saved, orientation, negative;
    FUNCTION_MARKER(0x8006FFC0, "LEVEL2P.BIN");
    /* TODO: Original player and axis indices are unchecked */
    record = 0x800B5E88 + player * 0xD0 + axis * 0x34;
    switch (mode) {
    case 2: value = r_u32(record + 0x18); break;
    case 3: saved = r_u32(record + 0x18); value *= saved; break;
    case 4: saved = r_u32(record + 0x18); value = (uint32)((sint32)saved >> (value & 31)); break;
    case 5: saved = r_u32(record + 0x18); value = saved << (value & 31); break;
    default: break;
    }
    orientation = r_u32(record + 0x20); negative = 0 - value;
    if (orientation) { w_u32(record + 0x10, value); w_u32(record + 0x14, negative); }
    else { w_u32(record + 0x10, negative); w_u32(record + 0x14, value); }
    if (!mode) w_u32(record + 0x18, value);
    w_u32(record + 0x1C, value);
}

uint32 level2p_sub_800700F8(uint32 player, uint32 axis)
{
    FUNCTION_MARKER(0x800700F8, "LEVEL2P.BIN");
    return r_u32(0x800B5EA4 + player * 0xD0 + axis * 0x34);
}

void level2p_sub_800742C8(uint32 owner, uint32 player)
{
    uint32 state, flags, height, port, denominator, quotient;
    FUNCTION_MARKER(0x800742C8, "LEVEL2P.BIN");
    state = r_u32(owner + 0x388); flags = r_u32(state); height = r_u32(state + 0x34);
    w_u32(state, flags & 0xFFFFFFED); w_u32(owner + 0x2D4, height); w_u16(owner + 0x2DA, 0xFF9C);
    if ((sint32)player < 0) return;
    level2p_sub_8007045C(player, 0, 0, 2); level2p_sub_8007045C(player, 1, 0, 2);
    level2p_sub_8006FFC0(player, 0, 0, 2); level2p_sub_8006FFC0(player, 1, 0, 2);
    port = r_u8(state + 0x726); denominator = level2p_sub_800700F8(port, 1) + 1;
    if (!denominator) { moh_draft_fail_fast("MIPS BREAK division by zero", 0x8007437C); return; }
    quotient = (uint32)(0x10000000 / (sint32)denominator);
    port = r_u8(state + 0x726);
    w_u32(state + 0x4B4, 0); w_u32(state + 0x4C0, 0); w_u32(state + 0x4B0, quotient); w_u32(state + 0x4BC, 0 - quotient);
    denominator = level2p_sub_800700F8(port, 0) + 1;
    if (!denominator) { moh_draft_fail_fast("MIPS BREAK division by zero", 0x800743CC); return; }
    quotient = (uint32)(0x10000000 / (sint32)denominator);
    port = r_u8(state + 0x726);
    w_u32(state + 0x4C8, 0); w_u32(state + 0x4D4, 0); w_u32(state + 0x4B8, quotient); w_u32(state + 0x4C4, 0 - quotient);
    denominator = level2p_sub_800700F8(port, 3) + 1;
    if (!denominator) { moh_draft_fail_fast("MIPS BREAK division by zero", 0x8007441C); return; }
    quotient = (uint32)(0x10000000 / (sint32)denominator); flags = r_u32(state);
    w_u32(state + 0x4D0, 0); w_u32(state + 0x4DC, 0); w_u32(state + 0x4CC, quotient); w_u32(state + 0x4D8, 0 - quotient);
    w_u32(state + 0x3C, flags & 2 ? 0xFFFFFF6A : 0xFFFFFF10);
}

void level2p_sub_80081A70(uint32 owner)
{
    uint32 state, port, tag, index = 0, count, record, timer, raw_timer, color, x = 80, y = 18, mask;
    FUNCTION_MARKER(0x80081A70, "LEVEL2P.BIN");
    state = r_u32(owner + 0x388); port = r_u8(state + 0x726); tag = (port + 6) & 0xFF;
    level2p_sub_8005AF7C(tag); count = (uint32)(sint32)r_s16(state + 0x760);
    /* TODO: Original live count and signed low16 record index are unchecked */
    while ((sint32)(sint16)index < (sint32)count) {
        record = state + (uint32)(sint32)(sint16)index * 12;
        timer = (uint32)(sint32)r_s16(record + 0x734); raw_timer = r_u16(record + 0x734);
        port = r_u8(state + 0x726);
        /* Unsigned shift retains the original bit31 player mask */
        mask = 1u << (port & 31);
        if ((sint32)timer < 0 || (sint32)timer > 200) {
            color = (sint32)timer < 0 ? 32 : 200;
            level2p_sub_8005ABFC(record + 0x738, x, y, color, color, color, 0, 0xFFFFFFFF, tag, mask, 0);
        } else {
            color = raw_timer & 0xFF;
            level2p_sub_8005AD58(record + 0x738, x, y, color, color, color, 1, timer, tag, mask, 0, 0, 1, 1, 1, 0);
        }
        x += 10; index += 1; count = (uint32)(sint32)r_s16(state + 0x760); y += 10;
    }
}

void level2p_sub_80082300(uint32 owner)
{
    uint32 state, index = 0, count, record, timer, raw_timer, changed = 0;
    FUNCTION_MARKER(0x80082300, "LEVEL2P.BIN");
    state = r_u32(owner + 0x388); count = (uint32)(sint32)r_s16(state + 0x760);
    /* TODO: Original signed count and low16 record index are unchecked */
    while ((sint32)(sint16)index < (sint32)count) {
        record = state + (uint32)(sint32)(sint16)index * 12;
        timer = (uint32)(sint32)r_s16(record + 0x734); raw_timer = r_u16(record + 0x734);
        if ((sint32)timer < 0) { w_u16(record + 0x734, 0 - raw_timer); changed = 1; }
        index += 1; count = (uint32)(sint32)r_s16(state + 0x760);
    }
    if (changed) level2p_sub_80081A70(owner);
}

void level2p_sub_80082394(uint32 owner, uint32 force)
{
    uint32 state, index = 0, count, record, timer, raw_timer, changed = 0, item, last, first, second, third;
    FUNCTION_MARKER(0x80082394, "LEVEL2P.BIN");
    state = r_u32(owner + 0x388); count = (uint32)(sint32)r_s16(state + 0x760);
    /* TODO: Original count, signed low16 cursor and compacted record spans are unchecked */
    while ((sint32)(sint16)index < (sint32)count) {
        record = state + (uint32)(sint32)(sint16)index * 12;
        timer = (uint32)(sint32)r_s16(record + 0x734); raw_timer = r_u16(record + 0x734);
        if ((sint32)timer >= 0) {
            timer = raw_timer - 1; w_u16(record + 0x734, timer);
            if (force || !(timer & 0xFFFF)) {
                changed = 1; item = r_u32(record + 0x730); level2p_sub_80082138(owner, state, item);
                count = r_u16(state + 0x760); count -= 1; w_u16(state + 0x760, count);
                last = state + (uint32)(sint32)(sint16)count * 12;
                first = r_u32(last + 0x730); second = r_u32(last + 0x734); third = r_u32(last + 0x738);
                w_u32(record + 0x730, first); w_u32(record + 0x734, second); w_u32(record + 0x738, third);
                index -= 1;
            } else if ((sint16)timer == 200) level2p_sub_80081A70(owner);
        }
        index += 1; count = (uint32)(sint32)r_s16(state + 0x760);
    }
    if (changed) level2p_sub_80081A70(owner);
}

void level2p_sub_80082138(uint32 owner, uint32 state, uint32 kind)
{
    uint32 mask, flags, player, other_state, first, second, port;
    FUNCTION_MARKER(0x80082138, "LEVEL2P.BIN");
    switch (kind) {
    case 3: mask = 0xFFFFFFF7; break;
    case 4: mask = 0xFFFFFFEF; break;
    case 5: mask = 0xFFFFFFDF; break;
    case 6: mask = 0xFFFFFFBF; break;
    case 7: mask = 0xFFFFFF7F; break;
    case 8: mask = 0xFFFFFEFF; break;
    case 10: mask = 0xFFFFFBFF; break;
    case 11: mask = 0xFFFFF7FF; break;
    case 12: case 13:
        port = r_u8(state + 0x726); first = (uint32)(sint32)r_s16(state + 0x76C); second = (uint32)(sint32)r_s16(state + 0x76E);
        level2p_sub_800717E4(port, first, second, 0); return;
    case 14:
        flags = r_u32(state + 0x72C); player = r_u32(0x8009943C); w_u32(state + 0x72C, flags & 0xFFFFBFFF);
        if (owner == player) player = r_u32(0x80099440);
        /* TODO: Original other-player pointer is unchecked */
        other_state = r_u32(player + 0x388); flags = r_u32(other_state + 0x72C);
        if (!(flags & 0x4000)) level2p_sub_80068974(0);
        return;
    case 15: mask = 0xFFFF7FFF; break;
    default: return;
    }
    flags = r_u32(state + 0x72C); w_u32(state + 0x72C, flags & mask);
}

void level2p_sub_8006921C(uint32 output, uint32 parameter2, uint32 parameter3, uint32 parameter4, uint32 parameter5, uint32 parameter6, uint32 parameter7, uint32 parameter8, uint32 parameter9, uint32 parameter10)
{
    FUNCTION_MARKER(0x8006921C, "LEVEL2P.BIN");
    w_u16(output, 0); w_u16(output + 0x10, parameter4); w_u16(output + 2, parameter4);
    w_u16(output + 6, parameter2); w_u16(output + 8, parameter3);
    w_u8(output + 0x16, 0); w_u8(output + 0x17, 0);
    w_u16(output + 0x12, parameter5); w_u16(output + 4, parameter5);
    w_u16(output + 0xA, parameter6); w_u16(output + 0xC, parameter7); w_u16(output + 0xE, parameter8);
    w_u8(output + 0x14, parameter9); w_u8(output + 0x15, parameter10);
}

void level2p_sub_80068E64(uint32 effect, uint32 owner, uint32 mode, uint32 kind, uint32 parameters)
{
    uint32 first, second;
    FUNCTION_MARKER(0x80068E64, "LEVEL2P.BIN");
    w_u16(effect, kind); w_u16(effect + 2, 0);
    first = moh_contact_word_load_native(parameters); second = moh_contact_word_load_native(parameters + 4);
    moh_contact_word_store_native(effect + 4, first); moh_contact_word_store_native(effect + 8, second);
    level2p_sub_80068A04(effect + 0x18, owner); w_u32(effect + 0x1C, mode);
    level2p_sub_8006921C(effect + 0x70, 0x3F0, 0x80, 0x1F, 0x1F, 8, 0x3E0, 0xF9, 2, 1);
}

void level2p_sub_80078060(uint32 owner, uint32 state, uint32 target_state)
{
    uint32 first, second, angle, minimum, measure, sine, cosine, numerator, quotient, offset;
    FUNCTION_MARKER(0x80078060, "LEVEL2P.BIN");
    (void)owner;
    first = (uint32)(sint32)r_s16(target_state + 0x26); second = (uint32)(sint32)r_s16(target_state + 0x2E);
    angle = (uint32)(sint32)r_s16(state + 0x7F6);
    if ((sint32)first < 0) first = 0 - first;
    if ((sint32)second < 0) second = 0 - second;
    minimum = (sint32)first < (sint32)second ? first : second;
    measure = first + second - (uint32)((sint32)minimum >> 1);
    sine = moh_draft_missing_call(0x8001C9A8, angle); cosine = moh_draft_missing_call(0x8001C908, angle);
    numerator = 0 - measure * sine;
    if (!cosine) { moh_draft_fail_fast("MIPS BREAK division by zero", 0x800780E8); return; }
    if (cosine == 0xFFFFFFFF && numerator == 0x80000000) { moh_draft_fail_fast("MIPS BREAK signed division overflow", 0x80078100); return; }
    quotient = (uint32)((sint32)numerator / (sint32)cosine); offset = r_u32(target_state + 0x98);
    w_u32(0x800A94B0, 250); w_u32(target_state + 0x28, (quotient + offset) << 16); w_u32(0x800A94AC, 1);
}

void level2p_sub_80078148(uint32 owner, uint32 bind_player)
{
    uint32 state, flags, value, player, live_state, handle, target, target_state;
    FUNCTION_MARKER(0x80078148, "LEVEL2P.BIN");
    state = r_u32(owner + 0x388); w_u32(owner + 0xC, 0x29); w_u32(owner + 0x10, 0x20000);
    player = r_u8(state + 0x726); w_u32(owner + 0x10, player ? 0x22000 : 0x21000);
    w_u32(state + 0x8C, 1); flags = r_u32(state); w_u8(state + 0x54, 0);
    flags &= 0x480000; w_u32(state, flags); w_u32(state + 4, flags);
    value = (uint32)(sint32)r_s16(0x8008F924); w_u32(state + 0x40, value);
    player = 0xFFFFFFFF;
    if (bind_player) player = r_u8(state + 0x726);
    level2p_sub_800742C8(owner, player);
    w_u32(state + 0x710, 0); w_u32(state + 0x700, 0); w_u32(state + 0x50, 0); w_u32(state + 0x4C, 0);
    w_u32(state + 0x840, 0); w_u32(state + 0x83C, 0xFFFFFFFF);
    live_state = r_u32(owner + 0x388);
    w_u16(owner + 0x2DA, 0xFF9C); w_u16(owner + 0x2E8, 200); w_u16(owner + 0x2EC, 100); w_u16(owner + 0x3CC, 0x80);
    handle = r_u32(live_state + 0xA0); target = 0;
    /* TODO: Original nonzero handle uses an unchecked low16 table index */
    if (handle) target = r_u32(0x80099460 + (handle & 0xFFFF) * 4);
    if (target) {
        target_state = r_u32(target + 0x388);
        w_u32(owner + 0x228, 0); w_u32(state + 0x7F4, 0); w_u32(state + 0x7F8, 0); w_u32(state + 0x7FC, 0);
        level2p_sub_80078060(owner, state, target_state);
    }
    level2p_sub_80082394(owner, 1); level2p_sub_80082300(owner);
    player = r_u32(0x8009943C);
    if (owner == player) player = r_u32(0x80099440);
    if (player) level2p_sub_80082300(player);
    level2p_sub_8007D0A4(owner, state); w_u32(state + 0x674, 0);
}

void level2p_sub_8007A898(uint32 owner)
{
    uint32 state, flags, value, index, weapon, row, model, resource, unavailable, sound, bank, sound_flags, parameters, kind, shape;
    FUNCTION_MARKER(0x8007A898, "LEVEL2P.BIN");
    state = r_u32(owner + 0x388); flags = r_u32(state);
    if (flags & 0x200040) {
        value = r_u32(state + 0x678); value >>= 2; w_u32(state + 0x678, value);
        if (value < 400) w_u32(state + 0x678, 400);
        w_u32(0x800A94B0, 250); w_u32(0x800A94AC, 1);
        flags = r_u32(state); w_u32(state, flags & 0xFFDFFFBF);
    }
    flags = r_u32(state); w_u32(state, flags & 0xFFFFFFDF);
    /* TODO: Original signed slot index, weapon tables and model pointers are unchecked */
    index = (uint32)(sint32)r_s8(state + 0x54); weapon = r_u8(0x8009AB8C + index); w_u8(state + 0x55, weapon);
    model = r_u32(owner + 0x100); level2p_sub_800665BC(model, owner, 0);
    weapon = r_u8(state + 0x55); row = 0x8008F920 + weapon * 0x44;
    value = r_u16(row + 0x28); w_u16(state + 0x72A, value);
    value = (uint32)(sint32)r_s16(row + 4); w_u32(state + 0x40, value);
    model = r_u32(owner + 0x100); resource = r_u32(row);
    level2p_sub_8006654C(model, resource, 0x147, owner);
    level2p_sub_8003F764(owner, 2, 1, 0); level2p_sub_8003FC00(owner); level2p_sub_8004696C(owner);
    level2p_sub_80078320(owner, state, 1);
    index = (uint32)(sint32)r_s8(state + 0x54); value = (uint32)(sint32)r_s16(state + index * 2 + 0x66);
    unavailable = 0;
    if (!value) {
        weapon = r_u8(0x8009AB8C + index); flags = r_u16(0x8008F94A + weapon * 0x44);
        if (!(flags & 1)) unavailable = 1;
    }
    level2p_sub_8005A1B8(owner, 0x10, unavailable); level2p_sub_800425EC(owner, 10);
    weapon = r_u8(state + 0x55); row = 0x8008F920 + weapon * 0x44;
    flags = r_u16(row + 0x2A); sound = (uint32)(sint32)r_s16(row + 0x18); sound_flags = (flags >> 3) & 2;
    if ((sint32)sound >= 0) {
        bank = (uint32)(sint32)r_s16(row + 0x1A);
        if ((sint32)bank < 0) { model = r_u32(owner + 0x100); resource = r_u32(model + 8); bank = r_u16(resource + 0xC8); }
        level2p_sub_800420C4((uint32)(sint32)(sint16)bank, sound, owner, 0x10000, 0, 0x8000, 0x3334, 0, 0, 0, sound_flags);
    }
    level2p_sub_8005A1B8(owner, 0x11, 0);
    flags = r_u32(state); w_u32(state + 0x710, 0);
    if (flags & 2) w_u32(owner + 0x2D4, 0x960000);
    weapon = r_u8(state + 0x55); row = 0x8008F920 + weapon * 0x44;
    kind = r_u16(row + 0x34); parameters = row + 0x36;
    level2p_sub_80068E64(state + 0x850, owner, 0, kind, parameters);
    weapon = r_u8(state + 0x55);
    if (weapon == 3 || weapon == 10) { w_u16(state + 0x664, 0); w_u16(state + 0x666, 0); }
    shape = r_u32(owner + 0x304); w_u32(shape + 0x10, 0x80000);
    shape = r_u32(owner + 0x304); w_u32(shape + 0x14, 0x80000);
}

void level2p_sub_8005D0DC(uint32 owner, uint32 state)
{
    uint32 first = 0xFFFFFFFF, first_special = 0xFFFFFFFF, value, count, index, cursor, weapon, row, flags, amount;
    FUNCTION_MARKER(0x8005D0DC, "LEVEL2P.BIN");
    level2p_sub_8005CFDC();
    value = r_u32(owner + 0xF0);
    w_u32(owner + 0x38, 0); w_u32(owner + 0x3C, 0); w_u32(owner + 0x40, 0); w_u32(owner + 0x284, 0);
    w_u32(owner + 0xF4, value); level2p_sub_80078148(owner, 1);
    count = r_u32(0x8009A664); cursor = state;
    /* TODO: Original signed count, weapon indices and state span are unchecked */
    for (index = 0; (sint32)index < (sint32)count; ++index, cursor += 2) {
        weapon = r_u8(0x8009AB8C + index); row = 0x8008F920 + weapon * 0x44; flags = r_u16(row + 0x2A);
        if (flags & 1) {
            w_u16(row + 6, 0);
            weapon = r_u8(0x8009AB8C + index); w_u16(0x8008F928 + weapon * 0x44, 0);
        }
        flags = r_u32(0x800386A4);
        weapon = r_u8(0x8009AB8C + index); amount = r_u16(0x8008F928 + weapon * 0x44);
        w_u16(cursor + 0x5E, amount);
        if (!(flags & 0x40)) { weapon = r_u8(0x8009AB8C + index); amount = r_u16(0x8008F926 + weapon * 0x44); }
        w_u16(cursor + 0x56, amount); value = (uint32)(sint32)r_s16(cursor + 0x56); w_u16(cursor + 0x66, 0);
        if ((sint32)value != -1) {
            if ((sint16)first == -1) first = index;
            if ((sint16)first_special == -1) {
                weapon = r_u8(0x8009AB8C + index); flags = r_u16(0x8008F94A + weapon * 0x44);
                if (flags & 8) first_special = index;
            }
        }
    }
    if ((sint16)first_special == -1) {
        count = r_u32(0x8009A664);
        for (index = 0; (sint32)index < (sint32)count; ++index) {
            weapon = r_u8(0x8009AB8C + index); flags = r_u16(0x8008F94A + weapon * 0x44);
            if (flags & 8) { w_u16(state + index * 2 + 0x56, 0); break; }
        }
    }
    if ((sint16)first == -1) { w_u16(state + 0x56, 0); w_u8(state + 0x54, 0); }
    else w_u8(state + 0x54, first);
    level2p_sub_800782D4(owner);
    flags = r_u32(owner + 0x38); w_u32(owner + 0x38, flags | 0x20000);
    level2p_sub_8003FF0C(owner); level2p_sub_8003FC00(owner);
    w_u32(owner + 0x288, 1); w_u32(owner + 0x28C, 1); w_u32(owner + 0x21C, 0);
    level2p_sub_8007A898(owner); level2p_sub_8007D310(owner, 0);
}

void level2p_sub_8005CFDC(void)
{
    uint32 index;
    FUNCTION_MARKER(0x8005CFDC, "LEVEL2P.BIN");
    for (index = 0; index < 27; ++index) w_u32(0x80039CA8 + index * 4, 0);
    for (index = 0; index < 13; ++index) w_u16(0x8008FC80 - index * 0x44, 0);
}

uint32 level2p_sub_8004D268(uint32 point, uint32 direction, uint32 squared_length, uint32 target, uint32 shape_output)
{
    uint8 delta[8];
    uint32 delta_view, flags, state, resource, shapes, count, index, found = 0;
    uint32 px, py, pz, x, y, z, dx, dy, dz, first, second, third, metric, radius, value;
    FUNCTION_MARKER(0x8004D268, "LEVEL2P.BIN");
    flags = r_u32(target + 0xC);
    if (flags & 1) {
        state = r_u32(target + 0x388); resource = r_u32(state + 0xAC);
        shapes = r_u32(resource + 0xC0); count = r_u16(resource + 0xBE);
    } else {
        shapes = r_u32(target + 0x304); count = r_u16(target + 0x36);
    }
    px = (uint32)(sint32)r_s16(point + 2);
    py = (uint32)(sint32)r_s16(point + 6);
    pz = (uint32)(sint32)r_s16(point + 10);
    /* TODO: Native local mapping and original opaque high halfword remain unresolved */
    delta_view = moh_draft_local_address(delta, sizeof(delta));
    /* TODO: Original shape spans and output pointer are unchecked */
    for (index = 0; (sint32)index < (sint32)count; ++index, shapes += 0x48) {
        value = r_u32(shapes + 8);
        if (value == 7) continue;
        x = r_u32(shapes + 0x20); dx = x - px; w_u16(delta_view, dx);
        y = r_u32(shapes + 0x24); dy = y - py; w_u16(delta_view + 2, dy);
        z = r_u32(shapes + 0x28); dz = z - pz; w_u16(delta_view + 4, dz);
        xport_gte_write_data(9, dx); xport_gte_write_data(10, dy); xport_gte_write_data(11, dz);
        xport_gte_execute(0xA00428);
        radius = r_u32(shapes + 0x38);
        first = xport_gte_read_data(25); second = xport_gte_read_data(26); third = xport_gte_read_data(27);
        metric = first + second + third - radius;
        if ((sint32)metric > 0) {
            first = r_u32(delta_view); second = r_u32(delta_view + 4);
            xport_gte_write_control(0, first); xport_gte_write_control(1, second);
            x = r_u16(direction); y = r_u16(direction + 2); z = r_u16(direction + 4);
            xport_gte_write_data(9, x); xport_gte_write_data(10, y); xport_gte_write_data(11, z);
            xport_gte_execute(0x41E012);
            value = xport_gte_read_data(25);
            if ((sint32)value <= 0 || (sint32)squared_length < (sint32)metric) continue;
            x = (uint32)(sint32)r_s16(delta_view); y = (uint32)(sint32)r_s16(delta_view + 2);
            xport_gte_write_control(0, x); z = (uint32)(sint32)r_s16(delta_view + 4);
            xport_gte_write_control(2, y); xport_gte_write_control(4, z);
            x = (uint32)(sint32)r_s16(direction); y = (uint32)(sint32)r_s16(direction + 2); z = (uint32)(sint32)r_s16(direction + 4);
            xport_gte_write_data(9, x); xport_gte_write_data(10, y); xport_gte_write_data(11, z);
            xport_gte_execute(0x178000C);
            first = xport_gte_read_data(9); second = xport_gte_read_data(10); third = xport_gte_read_data(11);
            w_u16(delta_view, first); w_u16(delta_view + 2, second); w_u16(delta_view + 4, third);
            x = r_u16(delta_view); y = r_u16(delta_view + 2); z = r_u16(delta_view + 4);
            xport_gte_write_data(9, x); xport_gte_write_data(10, y); xport_gte_write_data(11, z);
            xport_gte_execute(0xA00428);
            metric = r_u32(shapes + 0x38);
            first = xport_gte_read_data(25); second = xport_gte_read_data(26); third = xport_gte_read_data(27);
            value = first + second + third;
            if ((sint32)value > (sint32)metric) continue;
        }
        w_u32(shapes, 1); w_u32(target + 0x314, metric);
        if (!found) { found = 1; w_u32(shape_output, index); }
    }
    return found;
}

uint32 level2p_sub_8004D48C(uint32 owner, uint32 direction, uint32 squared_length, uint32 target, uint32 shape_output)
{
    FUNCTION_MARKER(0x8004D48C, "LEVEL2P.BIN");
    return level2p_sub_8004D268(owner + 0x258, direction, squared_length, target, shape_output);
}

uint32 level2p_sub_8004D4B4(uint32 owner, uint32 owner_shape_output, uint32 target, uint32 target_shape_output)
{
    uint32 owner_index = 0, owner_offset = 0, found = 0, target_index, target_offset;
    uint32 count, base, shape, other, radius, value, first, second, third, x, y, z, coordinate, metric;
    FUNCTION_MARKER(0x8004D4B4, "LEVEL2P.BIN");
    /* TODO: Original live counts, shape spans and output pointers are unchecked */
    count = r_u16(owner + 0x36);
    if (!count) return 0;
    do {
        base = r_u32(owner + 0x304); shape = base + owner_offset; value = r_u32(shape + 8);
        if (value != 7) {
            target_index = 0; target_offset = 0;
            count = r_u16(target + 0x36); radius = r_u32(shape + 0x34);
            if (count) do {
                base = r_u32(target + 0x304); other = base + target_offset; value = r_u32(other + 8);
                if (value != 7) {
                    x = r_u32(other + 0x20); coordinate = r_u32(shape + 0x20); x -= coordinate;
                    y = r_u32(other + 0x24); coordinate = r_u32(shape + 0x24);
                    z = r_u32(other + 0x28); value = r_u32(shape + 0x28); y -= coordinate; z -= value;
                    xport_gte_write_data(9, x); xport_gte_write_data(10, y); xport_gte_write_data(11, z);
                    xport_gte_execute(0xA00428);
                    value = r_u32(other + 0x34); value += radius; metric = value * value;
                    first = xport_gte_read_data(25); second = xport_gte_read_data(26); third = xport_gte_read_data(27);
                    metric -= first + second + third;
                    if ((sint32)metric > 0) {
                        base = r_u32(owner + 0x304); w_u32(base + owner_offset, 1);
                        base = r_u32(target + 0x304); w_u32(base + target_offset, 1);
                        w_u32(owner + 0x314, metric); w_u32(target + 0x314, metric);
                        if (!found) { found = 1; w_u32(owner_shape_output, owner_index); w_u32(target_shape_output, target_index); }
                    }
                }
                count = r_u16(target + 0x36); target_index += 1; target_offset += 0x48;
            } while ((sint32)target_index < (sint32)count);
        }
        count = r_u16(owner + 0x36); owner_index += 1; owner_offset += 0x48;
    } while ((sint32)owner_index < (sint32)count);
    return found;
}

uint32 level2p_sub_8004D63C(uint32 owner, uint32 target)
{
    uint32 value, x, y, z, coordinate, radius, first, second, third;
    FUNCTION_MARKER(0x8004D63C, "LEVEL2P.BIN");
    /* TODO: Original owner and target spans are unchecked */
    value = r_u16(target + 0x2E8);
    if (!value) return 0;
    value = r_u16(owner + 0x2E8);
    if (!value) return 0;
    x = (uint32)(sint32)r_s16(target + 0x2E0); coordinate = (uint32)(sint32)r_s16(owner + 0x2E0);
    y = (uint32)(sint32)r_s16(target + 0x2E2); z = (uint32)(sint32)r_s16(target + 0x2E4); x -= coordinate;
    coordinate = (uint32)(sint32)r_s16(owner + 0x2E2); value = (uint32)(sint32)r_s16(owner + 0x2E4); y -= coordinate; z -= value;
    xport_gte_write_data(9, x); xport_gte_write_data(10, y); xport_gte_write_data(11, z);
    xport_gte_execute(0xA00428);
    radius = r_u16(target + 0x2E8); value = r_u16(owner + 0x2E8); radius += value; radius *= radius;
    first = xport_gte_read_data(25); second = xport_gte_read_data(26); third = xport_gte_read_data(27);
    value = radius - (first + second + third);
    return (sint32)value > 0;
}

uint32 level2p_sub_8004D6EC(uint32 point, uint32 target, uint32 direction, uint32 distance_output)
{
    uint32 radius, px, py, pz, x, y, z, dx, dy, dz, first, second, third, sum, largest, temporary;
    FUNCTION_MARKER(0x8004D6EC, "LEVEL2P.BIN");
    /* TODO: Original point, target and direction spans are unchecked */
    radius = r_u16(target + 0x2E8);
    if (!radius) return 0;
    px = (uint32)(sint32)r_s16(point + 2);
    py = (uint32)(sint32)r_s16(point + 6);
    pz = (uint32)(sint32)r_s16(point + 10);
    x = r_u16(target + 0x2E0); dx = (uint32)(sint32)(sint16)x - px;
    y = r_u16(target + 0x2E2); dy = (uint32)(sint32)(sint16)y - py;
    z = r_u16(target + 0x2E4); dz = (uint32)(sint32)(sint16)z - pz;
    xport_gte_write_control(0, (uint32)(sint32)(sint16)dx);
    xport_gte_write_control(2, (uint32)(sint32)(sint16)dy);
    xport_gte_write_control(4, (uint32)(sint32)(sint16)dz);
    x = (uint32)(sint32)r_s16(direction);
    y = (uint32)(sint32)r_s16(direction + 2);
    z = (uint32)(sint32)r_s16(direction + 4);
    xport_gte_write_data(9, x); xport_gte_write_data(10, y); xport_gte_write_data(11, z);
    xport_gte_execute(0x178000C);
    first = xport_gte_read_data(9); second = xport_gte_read_data(10); third = xport_gte_read_data(11);
    xport_gte_write_data(9, first & 0xFFFF); xport_gte_write_data(10, second & 0xFFFF); xport_gte_write_data(11, third & 0xFFFF);
    xport_gte_execute(0xA00428);
    radius = r_u16(target + 0x2E8); radius *= radius;
    first = xport_gte_read_data(25); second = xport_gte_read_data(26); third = xport_gte_read_data(27);
    sum = first + second + third;
    if ((sint32)sum > (sint32)radius) return 0;
    if (distance_output) {
        if ((sint32)dx < 0) dx = 0 - dx;
        if ((sint32)dz < 0) dz = 0 - dz;
        if ((sint32)dy < 0) dy = 0 - dy;
        largest = dx;
        if ((sint32)largest < (sint32)dy) { temporary = largest; largest = dy; dy = temporary; }
        if ((sint32)largest < (sint32)dz) { temporary = largest; largest = dz; dz = temporary; }
        sum = dy + dz; sum = (uint32)((sint32)sum >> 2);
        w_u32(distance_output, largest + sum);
    }
    return 1;
}

uint32 level2p_sub_8004D8B4(uint32 owner, uint32 target, uint32 direction, uint32 distance_output)
{
    FUNCTION_MARKER(0x8004D8B4, "LEVEL2P.BIN");
    return level2p_sub_8004D6EC(owner + 0x258, target, direction, distance_output);
}

void level2p_sub_8004E5D4(uint32 player, uint32 small_motor, uint32 strength, uint32 ticks, uint32 shift)
{
    uint32 level;
    FUNCTION_MARKER(0x8004E5D4, "LEVEL2P.BIN");
    level = r_u8(0x8008FCE4);
    switch (level) {
    case 1: strength = (uint32)((sint32)strength >> 1); break;
    case 2: strength -= (uint32)((sint32)strength >> 2); break;
    case 3: break;
    default: return;
    }
    /* TODO: Original player index is unchecked */
    w_u8(0x8008FEC8 + player * 2, small_motor);
    w_u8(0x8008FEC9 + player * 2, strength);
    w_u32(0x8008FEDC + player * 4, ticks);
    /* Unsigned shift retains the original bit31 result */
    w_u32(0x8008FED4 + player * 4, (1u << (shift & 31)) - 1);
}

uint32 level2p_sub_80087920(uint32 owner, uint32 point, uint32 parameter)
{
    uint32 count, object, value, state;
    FUNCTION_MARKER(0x80087920, "LEVEL2P.BIN");
    count = r_u32(0x8009DFA0);
    if (count >= 5) return 0;
    object = level2p_sub_80066934(1, 12, 0xFFFFFFFF, 0, 0, 0, 0, 0);
    if (!object) return 0;
    w_u32(object + 8, 0x66);
    w_u32(object + 0xC, 0x07020000);
    w_u32(object + 0x10, 0x213);
    w_u32(object + 0x36C, 0x80087880);
    w_u32(object + 0x374, 0);
    w_u32(object + 0x370, 0);
    w_u32(object + 0x378, 0);
    w_u32(object + 0x37C, 0);
    w_u32(object + 0x384, 0x800653D8);
    w_u32(object + 0x14, 0);
    /* TODO: Original point and allocated state pointers are unchecked */
    value = r_u32(point); w_u32(object + 0x20C, value);
    value = r_u32(point + 4); w_u32(object + 0x210, value);
    value = r_u32(point + 8); state = r_u32(object + 0x388); w_u32(object + 0x214, value);
    value = sub_8001552C();
    w_u32(state, (value & 15) + 8);
    level2p_sub_80068A04(state + 4, owner);
    count = r_u32(0x8009DFA0);
    w_u32(state + 8, parameter);
    w_u32(0x8009DFA0, count + 1);
    level2p_sub_80066BA8(object);
    return object;
}

void level2p_sub_80079AC0(uint32 owner, uint32 first_bone, uint32 second_bone, uint32 trigger, uint32 parameter5)
{
    uint8 impact[8], normal[8], hit[40], direction[8];
    uint32 end[3], distance;
    uint32 impact_view, normal_view, hit_view, direction_view, end_view, distance_view;
    uint32 state, player, value, counter, limit, weapon, flags, original, magnitude, model, asset, projectile;
    uint32 first_parameter = 0, second_parameter = 0, resource, floor, height;
    uint32 axis_x, axis_y, axis_z, x, y, z, dx, dz, largest, smaller, other, product, result;
    uint32 current, candidate, nearest, row, sound, bank;
    FUNCTION_MARKER(0x80079AC0, "LEVEL2P.BIN");
    state = r_u32(owner + 0x388);
    /* TODO: Original player, state, model and resource indices are unchecked */
    if (trigger & 0xFF) {
        player = (uint32)(sint32)r_s8(state + 0x54);
        value = (uint32)(sint32)r_s16(state + player * 2 + 0x66);
        if (value) {
            counter = r_u32(state + 0x6F8);
            if (counter) goto advance_throw;
            level2p_sub_8005A1B8(owner, 0x10, 1);
            if ((sint32)parameter5 < 12) {
                level2p_sub_8003F764(owner, 12, 1, 0);
                weapon = r_u8(state + 0x55);
                w_u32(state + 0x6F4, weapon == 10 ? 18 : 8);
                w_u16(state + 0x664, 30); w_u16(state + 0x666, 48);
            } else {
                level2p_sub_8003F764(owner, 14, 1, 0);
                weapon = r_u8(state + 0x55);
                w_u32(state + 0x6F4, weapon == 10 ? 15 : 8);
                value = r_u16(state + 0x666);
                w_u16(state + 0x666, 48); w_u16(state + 0x664, value);
            }
            original = r_u32(state + 0x40); w_u32(state + 0x6F8, 1);
            magnitude = (sint32)original < 0 ? 0 - original : original;
            w_u32(state + 0x44, original); w_u32(state + 0x48, magnitude);
            level2p_sub_80046BE4(owner);
        }
    }
    counter = r_u32(state + 0x6F8);
    if (!counter) return;
advance_throw:
    limit = r_u32(state + 0x6F4);
    w_u32(state + 0x6F8, counter + 1);
    if ((sint32)counter < (sint32)limit) return;
    weapon = r_u8(state + 0x55); resource = weapon == 10 ? 0x61282777 : 0x55A51DAB;
    model = r_u32(owner + 0x100); value = r_u32(model + 0x84);
    if (value) {
        asset = r_u32(model + 8); first_parameter = asset + 0xD8; second_parameter = asset + 0xE0;
        first_bone = (uint32)(sint32)r_s16(asset + 0xD4);
        second_bone = (uint32)(sint32)r_s16(asset + 0xD6);
    }
    projectile = level2p_sub_80045BF8(owner, resource, first_bone, second_bone, 90, parameter5 * 2 + 55, 0, first_parameter, second_parameter);
    if (projectile) {
        weapon = r_u8(state + 0x55); w_u32(projectile + 8, 0x13F3);
        flags = r_u32(projectile + 0xC);
        w_u32(projectile + 0x14, 0x08000451); w_u16(projectile + 0x3CE, weapon);
        w_u32(projectile + 0xC, flags | 0x80000000);
        floor = r_u32(owner + 0x208); height = r_u32(projectile + 0x210);
        if ((sint32)floor < (sint32)height) {
            w_u32(projectile + 0x210, floor - 0x200000);
            value = r_u32(projectile + 0x21C);
            if ((sint32)value > 0x370000) w_u32(projectile + 0x21C, 0x370000);
        }
        axis_x = r_u16(projectile + 0x344); axis_y = r_u16(projectile + 0x34A); axis_z = r_u16(projectile + 0x350);
        /* TODO: Native local mapping and original opaque vector padding remain unresolved */
        impact_view = moh_draft_local_address(impact, sizeof(impact)); normal_view = moh_draft_local_address(normal, sizeof(normal));
        hit_view = moh_draft_local_address(hit, sizeof(hit)); direction_view = moh_draft_local_address(direction, sizeof(direction));
        end_view = moh_draft_local_address(end, sizeof(end)); distance_view = moh_draft_local_address(&distance, sizeof(distance));
        x = (uint32)(sint32)r_s16(projectile + 0x20E); value = (uint32)(sint32)r_s16(owner + 0x20E);
        w_u16(impact_view + 2, 0); dx = x - value; w_u16(impact_view, dx);
        dx = (uint32)(sint32)(sint16)dx;
        z = (uint32)(sint32)r_s16(projectile + 0x216); value = (uint32)(sint32)r_s16(owner + 0x216);
        if ((sint32)dx < 0) dx = 0 - dx;
        dz = z - value; value = (uint32)(sint32)(sint16)dz;
        if ((sint32)value < 0) value = 0 - value;
        largest = dx; other = 0; w_u16(impact_view + 4, dz);
        if ((sint32)dx < 0) { largest = 0; other = dx; }
        smaller = value;
        if ((sint32)largest < (sint32)value) { smaller = largest; largest = value; }
        value = other + smaller; largest += (uint32)((sint32)value >> 2);
        w_u32(distance_view, largest);
        product = (0 - (uint32)(sint32)(sint16)axis_x) * largest;
        value = (uint32)((sint32)product >> 12); x = r_u32(projectile + 0x20C);
        w_u32(end_view, x + (value << 16));
        product = (0 - (uint32)(sint32)(sint16)axis_y) * largest;
        value = (uint32)((sint32)product >> 12); y = r_u32(projectile + 0x210);
        w_u32(end_view + 4, y + (value << 16));
        z = r_u32(projectile + 0x214);
        product = (0 - (uint32)(sint32)(sint16)axis_z) * largest;
        value = (uint32)((sint32)product >> 12); w_u32(end_view + 8, z + (value << 16));
        result = level2p_sub_8003F26C(projectile + 0x20C, end_view, normal_view, impact_view, distance_view, hit_view, 0);
        if ((sint32)result >= 0) {
            product = (0 - (uint32)(sint32)(sint16)axis_x) * 5;
            x = (uint32)(sint32)r_s16(impact_view);
            w_u32(projectile + 0x20C, (x + (uint32)((sint32)product >> 10)) << 16);
            product = (0 - (uint32)(sint32)(sint16)axis_y) * 5;
            y = (uint32)(sint32)r_s16(impact_view + 2);
            w_u32(projectile + 0x210, (y + (uint32)((sint32)product >> 10)) << 16);
            product = (0 - (uint32)(sint32)(sint16)axis_z) * 5;
            z = (uint32)(sint32)r_s16(impact_view + 4);
            w_u32(projectile + 0x214, (z + (uint32)((sint32)product >> 10)) << 16);
            floor = r_u32(owner + 0x208); height = r_u32(projectile + 0x210);
            if ((sint32)floor < (sint32)height) w_u32(projectile + 0x210, floor - 0x200000);
        }
        x = r_u32(end_view); y = r_u32(end_view + 4); z = r_u32(end_view + 8);
        w_u32(projectile + 0x258, x); w_u32(projectile + 0x25C, y); w_u32(projectile + 0x260, z);
        x = r_u32(projectile + 0x20C); value = r_u32(projectile + 0x258); x -= value;
        w_u16(direction_view, (uint32)((sint32)x >> 16));
        y = r_u32(projectile + 0x210); value = r_u32(projectile + 0x25C); y -= value;
        w_u16(direction_view + 2, (uint32)((sint32)y >> 16));
        z = r_u32(projectile + 0x214); value = r_u32(projectile + 0x260); z -= value;
        w_u16(direction_view + 4, (uint32)((sint32)z >> 16));
        x = r_u16(direction_view); y = r_u16(direction_view + 2); z = r_u16(direction_view + 4);
        xport_gte_write_data(9, x); xport_gte_write_data(10, y); xport_gte_write_data(11, z);
        xport_gte_execute(0xA00428);
        (void)xport_gte_read_data(25); (void)xport_gte_read_data(26); (void)xport_gte_read_data(27);
        moh_draft_missing_call(0x80016A40, direction_view, direction_view);
        nearest = 999999999; candidate = 0; current = r_u32(0x80099434);
        /* TODO: Original actor list assumes no cycles */
        while (current) {
            flags = r_u32(current + 0xC);
            if (!(flags & 0x20080000)) {
                value = r_u32(current + 0x10);
                if (!(value & 6)) {
                    value = r_u32(projectile + 0x14);
                    if ((value & flags) && current != projectile && current != owner && !(flags & 0x08000000)) {
                        result = level2p_sub_8004D8B4(projectile, current, direction_view, distance_view);
                        if (result) {
                            value = r_u32(distance_view);
                            if ((sint32)value < (sint32)nearest) { nearest = value; candidate = current; }
                        }
                    }
                }
            }
            current = r_u32(current + 4);
        }
        if (candidate) {
            value = r_u32(end_view); w_u32(projectile + 0x20C, value);
            value = r_u32(end_view + 4); w_u32(projectile + 0x210, value);
            value = r_u32(end_view + 8); w_u32(projectile + 0x214, value);
            level2p_sub_8006591C(projectile); level2p_sub_8004DFBC(projectile);
        }
    }
    flags = r_u32(0x800386A4);
    if (!(flags & 0x40)) {
        player = (uint32)(sint32)r_s8(state + 0x54);
        value = r_u16(state + player * 2 + 0x66);
        w_u16(state + player * 2 + 0x66, value - 1);
    }
    weapon = r_u8(state + 0x55); row = 0x8008F920 + weapon * 0x44;
    sound = (uint32)(sint32)r_s16(row + 0x14);
    if ((sint32)sound >= 0) {
        bank = (uint32)(sint32)r_s16(row + 0x16);
        if ((sint32)bank < 0) { model = r_u32(owner + 0x100); asset = r_u32(model + 8); bank = r_u16(asset + 0xC8); }
        level2p_sub_800420C4((uint32)(sint32)(sint16)bank, sound, owner, 0x20000, 0, 0x10000, 0x1800, 0, 0, 0, 0);
    }
    w_u32(state + 0x6F8, 0);
}

void level2p_sub_8007A150(uint32 owner, uint32 first_bone, uint32 second_bone, uint32 trigger)
{
    uint32 state, model, asset, mode, player, sound_parameter, flags, weapon, damage;
    uint32 first_parameter = 0, second_parameter = 0, timed = 0, projectile = 0;
    uint32 value, port, row, sound, bank, sound_flags, ammunition, original, magnitude, shift, threshold;
    FUNCTION_MARKER(0x8007A150, "LEVEL2P.BIN");
    state = r_u32(owner + 0x388);
    if (!(trigger & 0xFF)) return;
    /* TODO: Original model, player and weapon table indices are unchecked */
    model = r_u32(owner + 0x100); mode = r_u32(model + 0x84);
    if (mode) {
        asset = r_u32(model + 8);
        first_parameter = asset + 0xD8; second_parameter = asset + 0xE0;
        first_bone = (uint32)(sint32)r_s16(asset + 0xD4);
        second_bone = (uint32)(sint32)r_s16(asset + 0xD6);
    }
    player = (uint32)(sint32)r_s8(state + 0x54);
    value = (uint32)(sint32)r_s16(state + player * 2 + 0x66);
    if ((sint32)value <= 0) return;
    mode = r_u32(state + 0x844); sound_parameter = 0x17FFF;
    switch (mode) {
    case 0: case 1: case 7: case 8: timed = 1; break;
    default: break;
    }
    flags = r_u32(state); weapon = r_u8(state + 0x55);
    w_u32(state, flags | 0x40000);
    damage = r_u32(0x8008F94C + weapon * 0x44);
    switch (weapon) {
    case 0: case 5: case 8: case 9:
        projectile = level2p_sub_80045E68(owner, first_bone, second_bone, 0, 0, first_parameter, second_parameter, damage, 0, timed, 0x13F6, 0x2000, 0x20, 0x08000451);
        port = r_u8(state + 0x726);
        level2p_sub_8004E5D4(port, 0, 100, 3, 0);
        break;
    case 12:
        projectile = level2p_sub_80045E68(owner, first_bone, second_bone, 0, 0, first_parameter, second_parameter, 0x28000, 0, timed, 0x13F6, 0x2000, 0x20, 0x08000451);
        port = r_u8(state + 0x726);
        level2p_sub_8004E5D4(port, 0, 100, 3, 0);
        break;
    case 1: case 6: case 11:
        projectile = level2p_sub_80045E68(owner, first_bone, second_bone, 0, 0, first_parameter, second_parameter, damage, 11, timed, 0x13F7, 0x2000, 0x20, 0x08000451);
        value = r_u32(state + 0x50); value += 0x10000; w_u32(state + 0x50, value);
        if ((sint32)value > 0x10000) w_u32(state + 0x50, 0x10000);
        port = r_u8(state + 0x726);
        level2p_sub_8004E5D4(port, 0, 100, 3, 0);
        break;
    case 2:
        projectile = level2p_sub_8004609C(owner, first_bone, second_bone, 0, first_parameter, second_parameter, timed, 0x13F1, 0x2000, 0, 0x08000451);
        port = r_u8(state + 0x726);
        level2p_sub_8004E5D4(port, 0, 100, 7, 0);
        break;
    case 4:
        /* TODO: Original first five pellet results are stored through without null checks */
        projectile = level2p_sub_80046318(owner, first_bone, second_bone, 0, 0, first_parameter, second_parameter, timed, 0x13F7, 0x2000, 0, 0x08000451);
        value = r_u8(state + 0x55); sound_parameter = 0x20000; w_u16(projectile + 0x3CE, value);
        projectile = level2p_sub_80046318(owner, first_bone, second_bone, 0, 0, first_parameter, second_parameter, timed, 0x13F7, 0x2000, 0, 0x08000451);
        value = r_u8(state + 0x55); w_u16(projectile + 0x3CE, value);
        projectile = level2p_sub_80046318(owner, first_bone, second_bone, 0, 0, first_parameter, second_parameter, timed, 0x13F7, 0x2000, 0, 0x08000451);
        value = r_u8(state + 0x55); w_u16(projectile + 0x3CE, value);
        projectile = level2p_sub_80046318(owner, first_bone, second_bone, 0, 0, first_parameter, second_parameter, timed, 0x13F7, 0x2000, 0, 0x08000451);
        value = r_u8(state + 0x55); w_u16(projectile + 0x3CE, value);
        projectile = level2p_sub_80046318(owner, first_bone, second_bone, 0, 0, first_parameter, second_parameter, timed, 0x13F7, 0x2000, 0, 0x08000451);
        value = r_u8(state + 0x55); w_u16(projectile + 0x3CE, value);
        projectile = level2p_sub_80046318(owner, first_bone, second_bone, 0, 0, first_parameter, second_parameter, timed, 0x13F7, 0x2000, 0, 0x08000451);
        port = r_u8(state + 0x726);
        level2p_sub_8004E5D4(port, 0, 100, 7, 0);
        break;
    default: break;
    }
    if (projectile) { value = r_u8(state + 0x55); w_u16(projectile + 0x3CE, value); }
    flags = r_u32(state);
    if (!(flags & 0x200040)) level2p_sub_80068E58(state + 0x850);
    weapon = r_u8(state + 0x55); row = 0x8008F920 + weapon * 0x44;
    sound = (uint32)(sint32)r_s16(row + 0x14);
    if ((sint32)sound >= 0) {
        flags = r_u16(row + 0x2A); bank = (uint32)(sint32)r_s16(row + 0x16);
        sound_flags = (flags >> 3) & 2;
        if ((sint32)bank < 0) {
            model = r_u32(owner + 0x100); asset = r_u32(model + 8); bank = r_u16(asset + 0xC8);
        }
        if (weapon == 5 || weapon == 11) sound_parameter = 0x20000;
        level2p_sub_800420C4((uint32)(sint32)(sint16)bank, sound, owner, sound_parameter, 0, 0x10000, 0x1800, 0, 0, 0, sound_flags);
    }
    weapon = r_u8(state + 0x55);
    if (weapon != 7) {
        weapon = r_u8(state + 0x55); flags = r_u16(0x8008F94A + weapon * 0x44);
        if (!(flags & 0x10)) {
            value = sub_8001552C(); value *= 100;
            if (((sint32)value >> 15) >= 51) level2p_sub_80087920(owner, owner + 0x20C, 0x14000);
        }
    }
    player = (uint32)(sint32)r_s8(state + 0x54); ammunition = state + player * 2 + 0x66;
    value = r_u16(ammunition); value -= 1; w_u16(ammunition, value);
    if (!(value & 0xFFFF)) level2p_sub_8005A1B8(owner, 0x10, 1);
    flags = r_u32(state + 0x72C);
    if (flags & 8) {
        original = r_u32(state + 0x40); shift = (uint32)(sint32)r_s16(state + 0x762);
        magnitude = (sint32)original < 0 ? 0 - original : original;
        /* Unsigned shift permits bit31 before the original signed comparison */
        threshold = 1u << (shift & 31);
        if ((sint32)magnitude > (sint32)threshold) {
            w_u32(state + 0x44, (uint32)((sint32)original >> (shift & 31)));
            magnitude = (sint32)original < 0 ? 0 - original : original;
            w_u32(state + 0x48, (uint32)((sint32)magnitude >> (shift & 31)));
        } else {
            w_u32(state + 0x44, (sint32)original > 0 ? 1 : 0xFFFFFFFF);
            w_u32(state + 0x48, 1);
        }
    } else {
        original = r_u32(state + 0x40);
        magnitude = (sint32)original < 0 ? 0 - original : original;
        w_u32(state + 0x44, original); w_u32(state + 0x48, magnitude);
    }
    level2p_sub_80046BE4(owner);
}
