#include "game_level2p_signatures.h"
#include "xport_trace.h"
#include "psx.h"

static void moh_contact_load_matrix(uint32 address, uint32 controls)
{
    uint32 first, second, third;
    first = r_u32(address); second = r_u32(address + 4);
    xport_gte_write_control(0, first); xport_gte_write_control(1, second);
    first = r_u32(address + 8); second = r_u32(address + 12); third = r_u32(address + 16);
    xport_gte_write_control(2, first); xport_gte_write_control(3, second); xport_gte_write_control(4, third);
    if (controls == 5) return;
    first = r_u32(address + 20); second = r_u32(address + 24);
    xport_gte_write_control(5, first);
    third = r_u32(address + 28);
    xport_gte_write_control(6, second); xport_gte_write_control(7, third);
}

static void moh_contact_vector_execute(uint32 vector, uint32 opcode)
{
    uint32 value;
    value = r_u32(vector); xport_gte_write_data(0, value);
    value = r_u32(vector + 4); xport_gte_write_data(1, value);
    xport_gte_execute(opcode);
}

static uint32 moh_contact_plane_dot(uint32 plane, uint32 vector)
{
    uint32 first, second, x, y, z;
    first = r_u32(plane + 4); second = r_u32(plane + 8);
    xport_gte_write_control(0, first); xport_gte_write_control(1, second);
    x = r_u16(vector); y = r_u16(vector + 2); z = r_u16(vector + 4);
    xport_gte_write_data(9, x); xport_gte_write_data(10, y); xport_gte_write_data(11, z);
    xport_gte_execute(0x41E012);
    return xport_gte_read_data(25);
}

uint32 level2p_sub_8004C134(uint32 moving, uint32 owner, uint32 normal_output, uint32 force)
{
    uint8 first_buffer[8], second_buffer[8], third_buffer[8], motion_buffer[8], correction_buffer[8];
    uint32 first_distances[16], second_distances[16], rotation[3], matrix[8];
    uint32 first_view, second_view, third_view, motion_view, correction_view, first_dist_view, second_dist_view;
    uint32 rotation_view, matrix_view, planes, plane, index, bit, mask = 0, negative_mask = 0, height_mask = 0, result = 0;
    uint32 x, y, z, value, a, b, c, offset, distance, other_distance, maximum, temporary, flags, compare, gap;
    uint32 nx, nz, numerator, divisor, quotient, mac_x, mac_y, mac_z, old_flags, grounded, dead_position[3];
    FUNCTION_MARKER(0x8004C134, "LEVEL2P.BIN");
    if (!r_u16(owner + 0x2EE)) return 0;
    /* TODO: Original vector padding remains unspecified through fail-fast local adapters */
    first_view = moh_draft_local_address(first_buffer, sizeof(first_buffer));
    second_view = moh_draft_local_address(second_buffer, sizeof(second_buffer));
    third_view = moh_draft_local_address(third_buffer, sizeof(third_buffer));
    motion_view = moh_draft_local_address(motion_buffer, sizeof(motion_buffer));
    x = r_u16(moving + 0x2E0); planes = r_u32(owner + 0x300);
    w_u16(first_view, x); w_u16(third_view, x); w_u16(second_view, x);
    y = (uint32)(sint32)r_s16(moving + 0x212); w_u16(second_view + 2, y);
    value = r_u16(moving + 0x2E2); w_u16(first_view + 2, value);
    value = (uint32)(sint32)r_s16(moving + 0x2D6); w_u16(third_view + 2, y - value);
    z = r_u16(moving + 0x2E4); w_u16(first_view + 4, z); w_u16(third_view + 4, z); w_u16(second_view + 4, z);
    a = (uint32)(sint32)r_s16(moving + 0x21A); b = r_u16(moving + 0x27C); w_u16(motion_view, a + b);
    a = (uint32)(sint32)r_s16(moving + 0x21E); b = r_u16(moving + 0x27E); w_u16(motion_view + 2, a + b);
    a = r_u32(moving + 0x200); b = r_u32(moving + 0x220); c = r_u16(moving + 0x280);
    w_u16(motion_view + 4, c + (uint32)((sint32)(a + b) >> 16));
    level2p_sub_8004B8F8(owner, first_view, first_view, second_view, second_view, third_view, third_view);
    if (!r_s16(motion_view) && !r_s16(motion_view + 2) && !r_s16(motion_view + 4))
    {
        a = (uint32)(sint32)r_s16(moving + 0x20E); b = (uint32)(sint32)r_s16(moving + 0x25A); w_u16(motion_view, a - b);
        a = (uint32)(sint32)r_s16(moving + 0x212); b = (uint32)(sint32)r_s16(moving + 0x25E); w_u16(motion_view + 2, a - b);
        a = (uint32)(sint32)r_s16(moving + 0x216); b = (uint32)(sint32)r_s16(moving + 0x262); w_u16(motion_view + 4, a - b);
    }
    else
    {
        if (r_u32(moving + 12) & 1)
        {
            rotation[0] = 0; rotation[1] = r_u32(moving + 0x22C); rotation[2] = r_u32(moving + 0x230);
            rotation_view = moh_draft_local_address(rotation, sizeof(rotation));
            matrix_view = moh_draft_local_address(matrix, sizeof(matrix));
            level2p_sub_8008BB44(moving + 0x20C, rotation_view, moving + 0x270, matrix_view);
            moh_contact_load_matrix(matrix_view, 8);
        }
        else moh_contact_load_matrix(moving + 0x340, 8);
        moh_contact_vector_execute(motion_view, 0x486012);
        x = xport_gte_read_data(9); y = xport_gte_read_data(10); z = xport_gte_read_data(11);
        w_u16(motion_view, x); w_u16(motion_view + 2, y); w_u16(motion_view + 4, z);
    }
    level2p_sub_8004B820(owner, motion_view, motion_view);
    if (r_u32(moving + 0x18) & 1) return 0;
    first_dist_view = moh_draft_local_address(first_distances, sizeof(first_distances));
    second_dist_view = moh_draft_local_address(second_distances, sizeof(second_distances));
    for (index = 0; index < r_u16(owner + 0x2EE); ++index)
    {
        plane = planes + index * 28;
        value = moh_contact_plane_dot(plane, first_view); offset = (uint32)(sint32)r_s16(plane + 10);
        distance = (uint32)((sint32)value >> 15) - offset;
        /* TODO: Original does not bound plane count to either16-word local array */
        w_u32(first_dist_view + index * 4, distance);
        if (r_s16(plane + 6) < 0) value = moh_contact_plane_dot(plane, second_view);
        else value = moh_contact_plane_dot(plane, third_view);
        offset = (uint32)(sint32)r_s16(plane + 10);
        other_distance = (uint32)((sint32)value >> 15) - offset;
        w_u32(second_dist_view + index * 4, other_distance);
        distance = r_u32(first_dist_view + index * 4);
        bit = (uint32)1 << (index & 31);
        if ((sint32)distance < 0) { mask |= bit; negative_mask |= bit; }
        else
        {
            value = r_u16(moving + 0x2E8);
            if ((sint32)value < (sint32)distance)
            {
                a = (uint32)(sint32)r_s16(motion_view); c = (uint32)(sint32)r_s16(motion_view + 4);
                if ((sint32)a < 0) a = 0 - a;
                if ((sint32)c < 0) c = 0 - c;
                b = (uint32)(sint32)r_s16(motion_view + 2);
                if ((sint32)b < 0) b = 0 - b;
                if (a < b) { temporary = a; a = b; b = temporary; }
                if (a < c) { temporary = a; a = c; c = temporary; }
                maximum = a + ((b + c) >> 2);
                if ((sint32)(distance - maximum) <= 0) mask |= bit;
            }
            else mask |= bit;
        }
        value = (uint32)(sint32)r_s16(moving + 0x3CC); other_distance = r_u32(second_dist_view + index * 4);
        if ((sint32)other_distance <= (sint32)value) height_mask |= bit;
    }
    correction_view = moh_draft_local_address(correction_buffer, sizeof(correction_buffer));
    for (index = 0; index < r_u16(owner + 0x2EE); ++index)
    {
        plane = planes + index * 28; bit = (uint32)1 << (index & 31);
        if ((sint32)r_u32(moving + 0x21C) >= 0 && (r_u32(plane) & 1) && (mask & bit))
        {
            a = r_u32(plane + 12); b = r_u32(plane + 16);
            if ((negative_mask & a) == b)
            {
                a = r_u32(plane + 20); b = r_u32(plane + 24);
                if ((mask & a) == b)
                {
                    nx = (uint32)(sint32)r_s16(plane + 4); x = (uint32)(sint32)r_s16(first_view); a = nx * x;
                    nz = (uint32)(sint32)r_s16(plane + 8); z = (uint32)(sint32)r_s16(first_view + 4); b = nz * z;
                    offset = (uint32)(sint32)r_s16(plane + 10); divisor = (uint32)(sint32)r_s16(plane + 6);
                    numerator = (offset << 15) - (a + b);
                    if (!divisor) { moh_draft_fail_fast("Original contact division BREAK 0x1C00", 0x8004C6B4); return result; }
                    if (divisor == 0xFFFFFFFF && numerator == 0x80000000) { moh_draft_fail_fast("Original contact division BREAK 0x1800", 0x8004C6CC); return result; }
                    quotient = (uint32)((sint32)numerator / (sint32)divisor);
                    value = r_u32(owner + 0x210); w_u32(moving + 0x210, value + (quotient << 16));
                    value = r_u32(owner + 0x18); w_u32(owner + 0x18, value | 8);
                    old_flags = r_u32(moving + 0x18);
                    w_u32(moving + 0x21C, 0); w_u32(moving + 0x18, old_flags | 1);
                    level2p_sub_80068A04(moving + 0x2C, owner);
                    old_flags = r_u32(moving + 0x18); grounded = r_u32(moving + 0x284);
                    w_u32(moving + 0x18, old_flags | 4);
                    if (!grounded) result |= 1;
                    w_u32(moving + 0x284, 1);
                }
            }
        }
        if (!force)
        {
            a = r_u16(motion_view); b = r_u16(motion_view + 2); c = r_u16(motion_view + 4);
            if (!(a | b | c)) continue;
        }
        if (!(r_u32(plane) & 2) || !(mask & bit)) continue;
        if ((sint32)r_u32(first_dist_view + index * 4) < 0) continue;
        a = r_u32(plane + 12); b = r_u32(plane + 16);
        if ((negative_mask & a) != b) continue;
        a = r_u32(plane + 20); b = r_u32(plane + 24); compare = mask & a;
        if (compare != b || (height_mask & a) != compare) continue;
        a = (uint32)(sint32)r_s16(moving + 0x3CC); b = r_u32(second_dist_view + index * 4); gap = a - b;
        if ((sint32)gap <= 0 && !force) continue;
        a = (uint32)(sint32)r_s16(first_view + 2); b = (uint32)(sint32)r_s16(owner + 0x2F2);
        if ((sint32)a < (sint32)b) continue;
        nx = (uint32)(sint32)r_s16(plane + 4);
        w_u16(correction_view + 2, 0);
        value = (nx * gap << 1) + 0x8000; w_u16(correction_view, (uint32)((sint32)value >> 16));
        nz = (uint32)(sint32)r_s16(plane + 8);
        value = (nz * gap << 1) + 0x8000; w_u16(correction_view + 4, (uint32)((sint32)value >> 16));
        moh_contact_load_matrix(owner + 0x340, 5);
        moh_contact_vector_execute(correction_view, 0x486012);
        nx = (uint32)(sint32)r_s16(plane + 4); value = (uint32)(sint32)r_s16(moving + 0x3CC);
        value = ((0 - nx) * value << 1) + 0x8000; w_u16(correction_view, (uint32)((sint32)value >> 16));
        nz = (uint32)(sint32)r_s16(plane + 8); value = (uint32)(sint32)r_s16(moving + 0x3CC);
        value = ((0 - nz) * value << 1) + 0x8000; w_u16(correction_view + 4, (uint32)((sint32)value >> 16));
        mac_x = xport_gte_read_data(25); mac_y = xport_gte_read_data(26); mac_z = xport_gte_read_data(27);
        moh_contact_vector_execute(correction_view, 0x486012);
        if (!force)
        {
            value = r_u32(moving + 0x20C); w_u32(moving + 0x20C, value + (mac_x << 16));
            value = r_u32(moving + 0x214); w_u32(moving + 0x214, value + (mac_z << 16));
        }
        mac_x = xport_gte_read_data(25); mac_y = xport_gte_read_data(26); mac_z = xport_gte_read_data(27);
        value = r_u32(moving + 0x20C); dead_position[0] = value + (mac_x << 16);
        a = r_u32(moving + 0x2D4); b = r_u32(moving + 0x210); dead_position[1] = b - (uint32)((sint32)a >> 1);
        value = r_u32(moving + 0x214); dead_position[2] = value + (mac_z << 16);
        (void)dead_position;
        if (!force)
        {
            value = r_u32(motion_view); xport_gte_write_control(0, value);
            value = r_u32(motion_view + 4); xport_gte_write_control(1, value);
            x = r_u16(plane + 4); y = r_u16(plane + 6); z = r_u16(plane + 8);
            xport_gte_write_data(9, x); xport_gte_write_data(10, y); xport_gte_write_data(11, z);
            xport_gte_execute(0x41E012);
            if ((sint32)xport_gte_read_data(25) >= 0) continue;
        }
        if (!(r_u32(plane) & 8))
        {
            value = r_u32(moving + 0x38); w_u32(moving + 0x38, value | 0x8000);
        }
        else
        {
            value = r_u32(moving + 0x38); a = r_u32(moving + 0x48);
            w_u32(moving + 0x38, value | 0x40);
            if ((sint32)a <= 0 && (r_u32(moving + 12) & 5)) level2p_sub_8004B60C(moving, r_u32(owner + 0xF8), owner);
        }
        y = (uint32)(sint32)r_s16(plane + 6);
        if ((sint32)y > 0 && (sint32)r_u32(moving + 0x21C) <= 0)
        {
            if (!r_u32(moving + 0x284))
            {
                value = r_u32(moving + 0x1F4);
                w_u32(moving + 0x1F4, value + (((y * gap << 1) + 0x8000) & 0xFFFF0000));
            }
            w_u32(moving + 0x21C, 0);
            value = r_u32(owner + 0x18); w_u32(owner + 0x18, value | 0x10);
        }
        result |= 2;
        w_u32(moving + 0x200, 0); w_u32(moving + 0x218, 0); w_u32(moving + 0x220, 0);
        moh_contact_load_matrix(owner + 0x340, 5);
        moh_contact_vector_execute(plane + 4, 0x486012);
        x = xport_gte_read_data(9); y = xport_gte_read_data(10); z = xport_gte_read_data(11);
        w_u16(normal_output, x); w_u16(normal_output + 2, y); w_u16(normal_output + 4, z);
    }
    return result;
}


uint32 level2p_sub_80043E7C(uint32 moving, uint32 other)
{
    uint8 point[8], normal[8];
    uint32 related = other, best, hit_flags, related_view, point_view, normal_view, best_view, hit_flags_view;
    uint32 data, value, flags, hit, result = 0, first, second, effect, callback, count, duration;
    FUNCTION_MARKER(0x80043E7C, "LEVEL2P.BIN");
    data = r_u32(moving + 0x388);
    related_view = moh_draft_local_address(&related, sizeof(related));
    if (level2p_sub_80068988(data + 0x54, related_view)) return 0;
    value = r_u32(related_view);
    if (r_u32(value + 8) - 0x13F0 < 10) return 0;
    flags = r_u32(moving + 0x10);
    best = 0x7FFF;
    /* TODO: C134 writes six normal bytes; preserve the original opaque8-byte copy */
    normal_view = moh_draft_local_address(normal, sizeof(normal));
    if (flags & 4)
    {
        point_view = moh_draft_local_address(point, sizeof(point));
        best_view = moh_draft_local_address(&best, sizeof(best));
        hit_flags_view = moh_draft_local_address(&hit_flags, sizeof(hit_flags));
        hit = level2p_sub_8004C07C(moving, value, best_view, point_view, normal_view, hit_flags_view);
    }
    else hit = level2p_sub_8004C134(moving, value, normal_view, 0);
    if (!hit) return 0;
    value = r_u32(related_view);
    if (r_u32(value + 8) != 0x1777)
    {
        result = 1;
        if (r_u32(moving + 0x10) & 4)
        {
            first = (uint32)(sint32)r_s16(point_view); w_u32(moving + 0x20C, first << 16);
            first = (uint32)(sint32)r_s16(point_view + 2); w_u32(moving + 0x210, first << 16);
            first = (uint32)(sint32)r_s16(point_view + 4); w_u32(moving + 0x214, first << 16);
            first = r_u16(normal_view); w_u16(moving + 0x364, 0 - first);
            first = r_u16(normal_view + 2); w_u16(moving + 0x366, 0 - first);
            first = r_u16(normal_view + 4); w_u16(moving + 0x368, 0 - first);
        }
        else
        {
            first = r_u32(normal_view); second = r_u32(normal_view + 4);
            w_u32(moving + 0x364, first); w_u32(moving + 0x368, second);
        }
        if (r_u16(data + 0x38) & 0x100)
        {
            w_u16(moving + 0x368, 0); w_u16(moving + 0x366, 0); w_u16(moving + 0x364, 0);
            effect = level2p_sub_8006CD4C(moving, 6, 6, 0x4B, 0x10000, 0x1000, 0x40000, 5, 0);
            if (effect)
            {
                value = r_u32(related_view);
                if (r_u32(value + 12) & 0x10000000)
                {
                    flags = r_u32(effect + 0x10); w_u32(effect + 0x10, flags | 0x100000);
                }
            }
            level2p_sub_800431A0(moving);
            level2p_sub_80066C64(moving);
            return result;
        }
        value = r_u32(related_view);
        if (r_u32(value + 0x10) & 0x4000)
        {
            level2p_sub_800434E8(moving);
            first = sub_8001552C();
            count = (uint32)((sint32)(first * 3) >> 15) + 2;
            second = sub_8001552C();
            duration = (uint32)((sint32)(second << 2) >> 15) + 6;
            level2p_sub_8006DF10(moving + 0x20C, 1, count, 0x1E, 0x19, 15, 100, 0x3F4, 0xE0, 0x3E0, 0xFE, 15, 0x10000, 1, duration);
        }
        value = r_u32(related_view);
        if (r_u32(value + 8) == 0x1402)
        {
            first = sub_8001552C();
            level2p_sub_800420C4(0, (uint32)((sint32)first >> 15) + 12, moving, 0x20000, 0, 0x10000, 0x4CCD, 0, 0, 0, 0);
        }
        level2p_sub_80066C64(moving);
    }
    if (r_u32(moving + 0x10) & 4)
    {
        value = r_u32(related_view);
        callback = r_u32(value + 0x37C);
        if (callback) moh_draft_indirect_call(callback, value, moving);
    }
    return result;
}

uint32 level2p_sub_8004CB60(uint32 owner, uint32 origin, uint32 other)
{
    uint8 axis[8];
    uint32 axis_view, x, y, z, value, first, second;
    FUNCTION_MARKER(0x8004CB60, "LEVEL2P.BIN");
    axis_view = moh_draft_local_address(axis, sizeof(axis));
    value = r_u16(owner + 0x344); w_u16(axis_view, value);
    value = r_u16(owner + 0x34A); w_u16(axis_view + 2, value);
    value = r_u16(owner + 0x350); w_u16(axis_view + 4, value);
    value = r_u16(other); first = r_u16(origin); x = (uint16)(value - first);
    value = r_u16(other + 2); first = r_u16(origin + 2); y = (uint16)(value - first);
    value = r_u16(other + 4); first = r_u16(origin + 4); z = (uint16)(value - first);
    /* TODO: Original axis high half remains opaque at the fail-fast local adapter */
    first = r_u32(axis_view); second = r_u32(axis_view + 4);
    xport_gte_write_control(0, first); xport_gte_write_control(1, second);
    xport_gte_write_data(9, x); xport_gte_write_data(10, y); xport_gte_write_data(11, z);
    xport_gte_execute(0x41E012);
    return (sint32)xport_gte_read_data(25) > 0;
}

uint32 level2p_sub_8004F37C(uint32 vector)
{
    uint32 slot, index, flags, first, second;
    sint32 center, coordinate;
    FUNCTION_MARKER(0x8004F37C, "LEVEL2P.BIN");
    index = r_u32(0x8008FF38);
    center = r_s16(0x800AAD78); coordinate = r_s16(vector);
    if (center - 16 < coordinate && coordinate < center + 16)
    {
        center = r_s16(0x800AAD7A); coordinate = r_s16(vector + 2);
        if (center - 16 < coordinate && coordinate < center + 16)
        {
            center = r_s16(0x800AAD7C); coordinate = r_s16(vector + 4);
            if (center - 16 < coordinate && coordinate < center + 16) return 0;
        }
    }
    if (!r_s16(vector + 8) && !r_s16(vector + 10) && !r_s16(vector + 12)) return 0;
    index += 1;
    if ((sint32)index >= 100) index = 0;
    slot = 0x8008FF3C + (index << 7);
    level2p_sub_8004F0BC(vector, slot);
    w_u16(slot + 0x26, 0xC0); w_u16(slot + 0x28, 7);
    w_u16(slot + 0x2A, 0x1F); w_u16(slot + 0x2E, 0xFC);
    flags = r_u32(slot);
    w_u16(slot + 0x24, 0x3E0); w_u16(slot + 0x2C, 0x3E0);
    w_u32(0x8008FF38, index); w_u32(slot, flags | 7);
    first = r_u32(vector); second = r_u32(vector + 4);
    w_u32(0x800AAD78, first); w_u32(0x800AAD7C, second);
    return 1;
}

uint32 level2p_sub_8004362C(uint32 owner, uint32 events)
{
    uint8 axis[8];
    uint32 flags, data, value, first, second, third, x, y, z, count, duration;
    uint32 speed, candidate, limit, kind, nearest, best, distance, matches, axis_view;
    FUNCTION_MARKER(0x8004362C, "LEVEL2P.BIN");
    flags = r_u32(owner + 0x10); data = r_u32(owner + 0x388);
    if (flags & 4)
    {
        if (!(flags & 8))
        {
            w_u32(owner + 0x10, flags | 8); w_u32(data + 0x5C, events);
            return 1;
        }
        w_u32(owner + 0x10, flags & 0xFFFFFFF7); events = r_u32(data + 0x5C);
        if (flags & 0x20)
        {
            if (level2p_sub_8004F37C(owner + 0x318))
            {
                x = (uint32)(sint32)r_s16(owner + 0x318); z = (uint32)(sint32)r_s16(owner + 0x31C);
                w_u32(owner + 0x20C, x << 16); y = (uint32)(sint32)r_s16(owner + 0x31A);
                w_u32(owner + 0x214, z << 16); w_u32(owner + 0x210, y << 16);
            }
            if (!(r_u16(data + 0x38) & 0x200))
            {
                value = sub_8001552C(); count = (uint32)((sint32)(value * 3) >> 15) + 2;
                value = sub_8001552C(); duration = (uint32)((sint32)(value << 2) >> 15) + 6;
                level2p_sub_8006DF10(owner + 0x20C, 1, count, 30, 25, 15, 100, 0x3F4, 0xE0, 0x3E0, 0xFE, 15, 0x10000, 1, duration);
            }
        }
    }
    if (!(events & 3) && !(r_u16(data + 0x38) & 0x200)) goto finish;
    flags = r_u16(data + 0x38); w_u16(data + 0x38, flags | 2);
    if ((flags & 4) && (events & 1))
        w_u32(owner + 0xC, r_u32(owner + 0xC) & 0xFFFFFF7F);
    else if ((r_u32(data + 0x38) & 0x208) && !((flags = r_u16(data + 0x38)) & 0x400))
    {
        if (!(flags & 0x800))
        {
            speed = r_u32(owner + 0x200); speed -= (uint32)((sint32)speed / 3);
            w_u32(owner + 0x200, speed);
            if ((sint32)speed < 0) w_u32(owner + 0x200, 0);
            if (events & 1)
            {
                first = r_u32(owner + 0x210); second = r_u32(owner + 0x21C); third = r_u32(data + 0x34);
                candidate = first + second + third;
                first = r_u32(owner + 0x1F4); limit = r_u32(owner + 0x208); candidate += first;
                if ((sint32)limit < (sint32)candidate)
                {
                    w_u16(data + 0x38, (r_u16(data + 0x38) & 0xFFF6) | 0x400);
                    w_u32(owner + 0x284, 1); flags = r_u32(owner + 0xC);
                    w_u32(owner + 0x21C, 0); w_u32(owner + 0x228, 0);
                    w_u32(owner + 0xC, (flags & 0xFFFFFF7F) | 0x28); w_u32(data + 0x74, 0);
                    if (r_s32(owner + 0x200) <= 0x1DFFFF)
                    {
                        w_u32(data + 0x80, 0x1E0000); w_u32(owner + 0x200, 0x1E0000);
                    }
                }
            }
        }
        kind = r_u32(owner + 8);
        value = sub_8001552C();
        if (kind == 0x13F3)
            level2p_sub_800420C4(1, (uint32)((sint32)(value << 1) >> 15) + 7, owner, 0x40000, 0, 0x10000, 0x40AC, 0, 0, 0, 0);
        else
            level2p_sub_800420C4(0, (uint32)((sint32)(value * 7) >> 15), owner, 0x30000, 0, 0x10000, 0x7FFF, 0, 0, 3, 0);
        if (!(r_u16(data + 0x38) & 0x800))
        {
            w_u32(data + 0x34, 0xA0000); value = r_u32(owner + 0x21C);
            w_u32(owner + 0x21C, (uint32)((sint32)value >> 2));
        }
    }
    if ((r_u16(data + 0x38) & 0x10) && (events & 1))
    {
        value = r_u32(data); w_u16(value + 0x4A4, 0);
        value = r_u32(data); w_u16(value + 0x4A6, 0xFF9C);
        value = r_u32(data); w_u16(value + 0x4A8, 0);
    }
    if (!(r_u16(data + 0x38) & 0x200)) goto finish;
    if (!(r_u32(owner + 0xC) & 0x80)) w_u32(owner + 0x200, r_u32(data + 0x80));
    flags = r_u16(data + 0x38);
    if (!(flags & 0x800))
    {
        if (flags & 0x400)
        {
            speed = r_u32(owner + 0x200); speed -= (uint32)((sint32)speed >> 1);
            w_u32(owner + 0x200, speed);
            if ((sint32)speed < 0) w_u32(owner + 0x200, 0);
            if (r_u16(data + 0x38) & 0x4000)
            {
                candidate = r_u32(0x80099434); nearest = 0; best = 0x7FFFFFFF; matches = 0;
                while (candidate)
                {
                    kind = r_u32(candidate + 8);
                    if ((kind == 0x1402 || kind == 0x1405) && !(r_u32(candidate + 0xC) & 0x20080000))
                    {
                        distance = level2p_sub_800517E8(owner, candidate);
                        if (distance < 0x4B0)
                        {
                            flags = r_u32(candidate + 0x3C); ++matches; w_u32(candidate + 0x3C, flags | 0x40);
                            if (!nearest || distance < best) { nearest = candidate; best = distance; }
                        }
                    }
                    candidate = r_u32(candidate + 4);
                }
                if (nearest)
                {
                    kind = r_u32(nearest + 8);
                    if (kind == 0x1405 || (matches & 0xFF) >= 3 || best < 1000)
                    {
                        value = (kind != 0x1405 && (matches & 0xFF) >= 3) ? 0x20 : 0x80;
                        w_u32(nearest + 0x3C, r_u32(nearest + 0x3C) | value);
                        level2p_sub_8004D148(owner, 0xFFFF, 7, 0xFFFFFFFF, 0xFFFFFFFF);
                    }
                }
                w_u16(data + 0x38, r_u16(data + 0x38) & 0xBFFF);
            }
        }
        else if (events & 2)
        {
            speed = r_u32(owner + 0x200); speed -= (uint32)((sint32)speed / 3);
            w_u32(owner + 0x200, speed);
            if ((sint32)speed < 0) w_u32(owner + 0x200, 0);
        }
        first = r_u32(data + 0x74); second = r_u32(data + 0x78);
        w_u32(data + 0x74, (uint32)((sint32)first >> 1)); w_u32(data + 0x78, (uint32)((sint32)second >> 1));
    }
    /* TODO: Original axis padding remains opaque through the fail-fast local adapter */
    axis_view = moh_draft_local_address(axis, sizeof(axis));
    x = r_u16(owner + 0x344); w_u16(axis_view + 2, 0); w_u16(axis_view, x);
    z = r_u16(owner + 0x350); w_u16(axis_view + 4, z);
    first = r_u32(axis_view); second = r_u32(axis_view + 4);
    xport_gte_write_control(0, first); xport_gte_write_control(1, second);
    x = r_u16(owner + 0x364); y = r_u16(owner + 0x366); z = r_u16(owner + 0x368);
    xport_gte_write_data(9, x); xport_gte_write_data(10, y); xport_gte_write_data(11, z);
    xport_gte_execute(0x41E012); value = xport_gte_read_data(25);
    xport_gte_write_data(8, 0 - (uint32)((sint32)value >> 17));
    x = r_u16(owner + 0x364); y = r_u16(owner + 0x366); z = r_u16(owner + 0x368);
    xport_gte_write_data(9, x); xport_gte_write_data(10, y); xport_gte_write_data(11, z);
    xport_gte_execute(0x198003D);
    x = (uint16)xport_gte_read_data(9); y = xport_gte_read_data(10); z = (uint16)xport_gte_read_data(11);
    first = r_u16(axis_view); second = r_u16(axis_view + 4);
    w_u16(axis_view, first + x); w_u16(axis_view + 4, second + z);
    level2p_sub_80043594(axis_view, owner + 0x228, owner + 0x22C);
    first = r_u32(owner + 0x228); w_u32(owner + 0x230, 0); second = r_u32(data + 0x68);
    if ((sint32)(first ^ second) >= 0) w_u32(data + 0x74, 0 - r_u32(data + 0x74));
    first = r_u32(owner + 0x22C); second = r_u32(data + 0x6C);
    if ((sint32)(first ^ second) >= 0) w_u32(data + 0x78, 0 - r_u32(data + 0x78));
    if (!r_u32(data + 0x74) && !r_u32(data + 0x78))
    {
        first = r_u32(owner + 0x228); second = r_u32(owner + 0x22C); third = r_u32(owner + 0x230);
        w_u32(data + 0x68, first); w_u32(data + 0x6C, second); w_u32(data + 0x70, third);
    }
    if (r_s32(owner + 0x228) > 0)
    {
        value = r_u32(owner + 0x21C); if ((sint32)value < 0) value = 0 - value;
        w_u32(owner + 0x21C, value);
    }
finish:
    flags = r_u16(data + 0x38);
    if (flags & 0x40) return 1;
    if (flags & 0x100)
    {
        level2p_sub_8006CD4C(owner, 10, 12, 130, 0x10000, 0x1000, 0x40000, 5, 0);
        value = sub_8001552C(); count = (uint32)((sint32)(value * 5) >> 15) + 15;
        level2p_sub_8006DF10(owner + 0x20C, 5, count, 40, 25, 80, 30, 0x3FF, 0x3F, 0x3E0, 0xFF, 1, 0x17FFF, 0, 0);
        level2p_sub_800431A0(owner);
    }
    else level2p_sub_800434E8(owner);
    value = sub_8001552C(); value = (((value & 0x3FF) - 0x200) << 16) + 0x8000000;
    first = r_u32(owner + 0x22C); first += value; w_u32(owner + 0x22C, first);
    if ((sint32)first > 0x10000000) w_u32(owner + 0x22C, first - 0x10000000);
    level2p_sub_80066C64(owner);
    return 1;
}

void level2p_sub_8004EBE8(uint32 first_radius, uint32 second_radius, uint32 vector)
{
    sint32 a, b, c, min_a, max_a, min_b, max_b, span_a, span_b;
    uint32 first_center, second_center, first_high, first_low, second_high, second_low, value;
    FUNCTION_MARKER(0x8004EBE8, "LEVEL2P.BIN");
    a = r_s16(vector + 0x18); b = r_s16(vector + 0x10); c = r_s16(vector + 0x20);
    min_a = b < a ? b : a; if (c < min_a) min_a = c;
    max_a = b < a ? a : b; if (max_a < c) max_a = c;
    a = r_s16(vector + 0x1C); b = r_s16(vector + 0x14); c = r_s16(vector + 0x24);
    min_b = b < a ? b : a; if (c < min_b) min_b = c;
    max_b = b < a ? a : b; if (max_b < c) max_b = c;
    span_a = max_a - min_a; if (span_a < 0) span_a = -span_a;
    span_b = max_b - min_b; if (span_b < 0) span_b = -span_b;
    second_center = (uint32)(sint32)r_s16(vector + 4);
    first_center = (uint32)(sint32)r_s16(vector + 0);
    second_high = second_center + second_radius; first_high = first_center + first_radius;
    first_low = first_center - first_radius; second_low = second_center - second_radius;
    if (span_a < (sint32)(first_radius << 1)) first_radius = (uint32)(span_a >> 1);
    if (span_b < (sint32)(second_radius << 1)) second_radius = (uint32)(span_b >> 1);
    if ((sint32)second_radius < (sint32)first_radius) first_radius = second_radius;
    else second_radius = first_radius;
    if (max_b < (sint32)second_high)
    {
        if ((sint32)second_low < min_b) value = (uint32)min_b + second_radius;
        else value = r_u16(vector + 4) - (second_high - (uint32)max_b);
        w_u16(vector + 4, value);
    }
    else if ((sint32)second_low < min_b)
    {
        value = r_u16(vector + 4); value -= second_low - (uint32)min_b;
        w_u16(vector + 4, value);
    }
    if (max_a < (sint32)first_high)
    {
        if ((sint32)first_low < min_a) value = (uint32)min_a + first_radius;
        else value = r_u16(vector + 0) - (first_high - (uint32)max_a);
        w_u16(vector + 0, value);
    }
    else if ((sint32)first_low < min_a)
    {
        value = r_u16(vector + 0); value -= first_low - (uint32)min_a;
        w_u16(vector + 0, value);
    }
}

void level2p_sub_8004ED84(uint32 first_radius, uint32 second_radius, uint32 vector)
{
    sint32 a, b, c, min_a, max_a, min_b, max_b, span_a, span_b;
    uint32 first_center, second_center, first_high, first_low, second_high, second_low, value;
    FUNCTION_MARKER(0x8004ED84, "LEVEL2P.BIN");
    a = r_s16(vector + 0x1A); b = r_s16(vector + 0x12); c = r_s16(vector + 0x22);
    min_a = b < a ? b : a; if (c < min_a) min_a = c;
    max_a = b < a ? a : b; if (max_a < c) max_a = c;
    a = r_s16(vector + 0x1C); b = r_s16(vector + 0x14); c = r_s16(vector + 0x24);
    min_b = b < a ? b : a; if (c < min_b) min_b = c;
    max_b = b < a ? a : b; if (max_b < c) max_b = c;
    span_a = max_a - min_a; if (span_a < 0) span_a = -span_a;
    span_b = max_b - min_b; if (span_b < 0) span_b = -span_b;
    second_center = (uint32)(sint32)r_s16(vector + 2);
    first_center = (uint32)(sint32)r_s16(vector + 4);
    second_high = second_center + second_radius; first_high = first_center + first_radius;
    first_low = first_center - first_radius; second_low = second_center - second_radius;
    if (span_b < (sint32)(first_radius << 1)) first_radius = (uint32)(span_b >> 1);
    if (span_a < (sint32)(second_radius << 1)) second_radius = (uint32)(span_a >> 1);
    if ((sint32)second_radius < (sint32)first_radius) first_radius = second_radius;
    else second_radius = first_radius;
    if (max_a < (sint32)second_high)
    {
        if ((sint32)second_low < min_a) value = (uint32)min_a + second_radius;
        else value = r_u16(vector + 2) - (second_high - (uint32)max_a);
        w_u16(vector + 2, value);
    }
    else if ((sint32)second_low < min_a)
    {
        value = r_u16(vector + 2); value -= second_low - (uint32)min_a;
        w_u16(vector + 2, value);
    }
    if (max_b < (sint32)first_high)
    {
        if ((sint32)first_low < min_b) value = (uint32)min_b + first_radius;
        else value = r_u16(vector + 4) - (first_high - (uint32)max_b);
        w_u16(vector + 4, value);
    }
    else if ((sint32)first_low < min_b)
    {
        value = r_u16(vector + 4); value -= first_low - (uint32)min_b;
        w_u16(vector + 4, value);
    }
}

void level2p_sub_8004EF20(uint32 first_radius, uint32 second_radius, uint32 vector)
{
    sint32 a, b, c, min_a, max_a, min_b, max_b, span_a, span_b;
    uint32 first_center, second_center, first_high, first_low, second_high, second_low, value;
    FUNCTION_MARKER(0x8004EF20, "LEVEL2P.BIN");
    a = r_s16(vector + 0x18); b = r_s16(vector + 0x10); c = r_s16(vector + 0x20);
    min_a = b < a ? b : a; if (c < min_a) min_a = c;
    max_a = b < a ? a : b; if (max_a < c) max_a = c;
    a = r_s16(vector + 0x1A); b = r_s16(vector + 0x12); c = r_s16(vector + 0x22);
    min_b = b < a ? b : a; if (c < min_b) min_b = c;
    max_b = b < a ? a : b; if (max_b < c) max_b = c;
    span_a = max_a - min_a; if (span_a < 0) span_a = -span_a;
    span_b = max_b - min_b; if (span_b < 0) span_b = -span_b;
    second_center = (uint32)(sint32)r_s16(vector + 2);
    first_center = (uint32)(sint32)r_s16(vector + 0);
    second_high = second_center + second_radius; first_high = first_center + first_radius;
    first_low = first_center - first_radius; second_low = second_center - second_radius;
    if (span_a < (sint32)(first_radius << 1)) first_radius = (uint32)(span_a >> 1);
    if (span_b < (sint32)(second_radius << 1)) second_radius = (uint32)(span_b >> 1);
    if ((sint32)second_radius < (sint32)first_radius) first_radius = second_radius;
    else second_radius = first_radius;
    if (max_b < (sint32)second_high)
    {
        if ((sint32)second_low < min_b) value = (uint32)min_b + second_radius;
        else value = r_u16(vector + 2) - (second_high - (uint32)max_b);
        w_u16(vector + 2, value);
    }
    else if ((sint32)second_low < min_b)
    {
        value = r_u16(vector + 2); value -= second_low - (uint32)min_b;
        w_u16(vector + 2, value);
    }
    if (max_a < (sint32)first_high)
    {
        if ((sint32)first_low < min_a) value = (uint32)min_a + first_radius;
        else value = r_u16(vector + 0) - (first_high - (uint32)max_a);
        w_u16(vector + 0, value);
    }
    else if ((sint32)first_low < min_a)
    {
        value = r_u16(vector + 0); value -= first_low - (uint32)min_a;
        w_u16(vector + 0, value);
    }
}

uint32 level2p_sub_8004F0BC(uint32 vector, uint32 output)
{
    uint8 angles[8], matrix[32];
    uint32 position[3], angles_view, matrix_view, position_view, radius, value, first, second, third, offset;
    sint32 x, y, z, maximum;
    FUNCTION_MARKER(0x8004F0BC, "LEVEL2P.BIN");
    value = sub_8001552C(); radius = (uint32)((sint32)(value << 2) >> 15) + 5;
    z = r_s16(vector + 12); y = r_s16(vector + 10);
    if (z < 0) z = -z;
    x = r_s16(vector + 8);
    if (y < 0) y = -y;
    if (x < 0) x = -x;
    maximum = x < z ? z : x;
    if (maximum < y) level2p_sub_8004EBE8(radius, radius, vector);
    else if (z < x) level2p_sub_8004ED84(radius, radius, vector);
    else level2p_sub_8004EF20(radius, radius, vector);
    value = 0 - radius;
    w_u16(output + 0x14, value); w_u16(output + 4, value);
    w_u16(output + 0xE, value); w_u16(output + 6, value);
    w_u16(output + 0x1C, radius); w_u16(output + 0xC, radius);
    w_u16(output + 0x1E, radius); w_u16(output + 0x16, radius);
    w_u16(output + 0x20, 0); w_u16(output + 0x18, 0);
    w_u16(output + 0x10, 0); w_u16(output + 8, 0);
    /* TODO: Original local padding stays opaque at the fail-fast local adapter */
    angles_view = moh_draft_local_address(angles, sizeof(angles));
    matrix_view = moh_draft_local_address(matrix, sizeof(matrix));
    position_view = moh_draft_local_address(position, sizeof(position));
    first = (uint32)(sint32)r_s16(vector + 8); second = (uint32)(sint32)r_s16(vector + 12);
    value = moh_draft_missing_call(0x8001536C, first, second);
    w_u16(angles_view + 2, value + 0x800); w_u16(angles_view + 4, 0);
    second = (uint32)(sint32)r_s16(vector + 12); first = (uint32)(sint32)r_s16(vector + 8);
    xport_gte_write_data(9, first); xport_gte_write_data(10, first); xport_gte_write_data(11, second);
    xport_gte_execute(0xA00428);
    y = r_s16(vector + 10);
    first = xport_gte_read_data(25); second = xport_gte_read_data(26); third = xport_gte_read_data(27);
    value = moh_draft_missing_call(0x80016188, first + third);
    value = moh_draft_missing_call(0x8001536C, (uint32)y, value);
    w_u16(angles_view, value + 0x1000);
    position[0] = (uint32)(sint32)r_s16(vector);
    position[1] = (uint32)(sint32)r_s16(vector + 2);
    position[2] = (uint32)(sint32)r_s16(vector + 4);
    sub_8001AF98(angles_view, matrix_view);
    moh_draft_missing_call(0x80016218, matrix_view, position_view);
    moh_contact_load_matrix(matrix_view, 8);
    for (offset = 4; offset < 0x24; offset += 8)
    {
        moh_contact_vector_execute(output + offset, 0x480012);
        first = xport_gte_read_data(9); second = xport_gte_read_data(10); third = xport_gte_read_data(11);
        w_u16(output + offset, first); w_u16(output + offset + 2, second); w_u16(output + offset + 4, third);
    }
    return 1;
}

uint32 level2p_sub_8004F500(uint32 x, uint32 y, uint32 width, uint32 height, uint32 border, uint32 red, uint32 green, uint32 blue, uint32 count, uint32 blend, uint32 mask)
{
    uint32 index = 0, base = 0x80093140, row, buffer, packet;
    FUNCTION_MARKER(0x8004F500, "LEVEL2P.BIN");
    /* TODO: Original slot search has no exhaustion bound */
    while (r_u8(base + 0x1E3)) { base += 0x1EC; ++index; }
    blend = (uint8)blend;
    w_u16(base + 0x1E0, count); w_u8(base + 0x1E2, border);
    for (row = 0; row < 10; ++row)
        for (buffer = 0; buffer < 2; ++buffer)
        {
            packet = base + row * 0x30 + buffer * 0x18;
            w_u8(packet + 3, 5); w_u8(packet + 7, 0x2A);
        }
    level2p_sub_8004F928(index, x, y);
    level2p_sub_8004FB74(index, width, height);
    level2p_sub_8004FF84(index, red, green, blue);
    w_u8(base + 0x1E4, blend); w_u32(base + 0x1E8, mask); w_u8(base + 0x1E3, 1);
    return index;
}

void level2p_sub_8004F664(uint32 owner, uint32 channel)
{
    uint32 limit_buffer, limit_view, cursor, kind, buffer_offset, base, packet, index, multiplier, count;
    uint32 pointer, first, second, value;
    FUNCTION_MARKER(0x8004F664, "LEVEL2P.BIN");
    kind = r_u32(owner + 0x221C);
    limit_view = moh_draft_local_address(&limit_buffer, sizeof(limit_buffer));
    cursor = level2p_sub_8005365C(kind, limit_view); buffer_offset = kind * 24;
    limit_buffer -= 12;
    for (base = 0x80093140; (sint32)base < (sint32)0x8009428C; base += 0x1EC)
    {
        if (!r_u8(base + 0x1E3)) continue;
        if (!(r_u32(base + 0x1E8) & (1u << (channel & 31)))) continue;
        if (limit_buffer < cursor)
        {
            level2p_sub_8005367C(limit_buffer);
            break;
        }
        multiplier = r_u8(base + 0x1E2) ? 5 : 1;
        count = r_u16(base + 0x1E0) * multiplier;
        index = 0;
        if (count)
        {
            do
            {
                packet = base + buffer_offset + index * 0x30;
                pointer = r_u32(owner + 0x2218); first = r_u32(packet); second = r_u32(pointer);
                w_u32(packet, (first & 0xFF000000) | (second & 0xFFFFFF));
                pointer = r_u32(owner + 0x2218); first = r_u32(pointer);
                w_u32(pointer, (first & 0xFF000000) | (packet & 0xFFFFFF));
                count = r_u16(base + 0x1E0) * multiplier; ++index;
            } while ((sint32)index < (sint32)count);
        }
        value = moh_draft_missing_call(0x8001B968, 0, r_u8(base + 0x1E4), 0, 0);
        moh_draft_missing_call(0x8001B8C8, cursor, 1, 0, (uint16)value, 0);
        pointer = r_u32(owner + 0x2218); first = r_u32(cursor); second = r_u32(pointer);
        w_u32(cursor, (first & 0xFF000000) | (second & 0xFFFFFF));
        pointer = r_u32(owner + 0x2218); value = cursor & 0xFFFFFF; first = r_u32(pointer);
        cursor += 12;
        w_u32(pointer, (first & 0xFF000000) | value);
    }
    level2p_sub_8005367C(cursor);
}

void level2p_sub_8004F900(uint32 index)
{
    FUNCTION_MARKER(0x8004F900, "LEVEL2P.BIN");
    w_u8(0x80093323 + index * 0x1EC, 0);
}

void level2p_sub_8004F928(uint32 index, uint32 x, uint32 y)
{
    uint32 base, buffer, packet, count, item, expanded, delta_x, delta_y, value, last_source;
    FUNCTION_MARKER(0x8004F928, "LEVEL2P.BIN");
    base = 0x80093140 + index * 0x1EC;
    for (buffer = 0; buffer < 2; ++buffer)
    {
        packet = base + buffer * 0x18;
        delta_x = x - (uint32)(sint32)r_s16(packet + 8);
        delta_y = y - (uint32)(sint32)r_s16(packet + 10);
        expanded = r_u8(base + 0x1E2) != 0;
        count = r_u16(base + 0x1E0) * (expanded ? 5 : 1);
        item = 0;
        if (!count) continue;
        do
        {
            packet = base + buffer * 0x18 + item * 0x30;
            value = r_u16(packet + 8); w_u16(packet + 8, value + delta_x);
            value = r_u16(packet + 10); w_u16(packet + 10, value + delta_y);
            value = r_u16(packet + 12); w_u16(packet + 12, value + delta_x);
            value = r_u16(packet + 14); w_u16(packet + 14, value + delta_y);
            value = r_u16(packet + 16); w_u16(packet + 16, value + delta_x);
            value = r_u16(packet + 18); w_u16(packet + 18, value + delta_y);
            /* TODO: Expanded path reads the next primitive for its last coordinate pair */
            last_source = packet + (expanded ? 0x30 : 0);
            value = r_u16(last_source + 20); w_u16(packet + 20, value + delta_x);
            value = r_u16(last_source + 22); w_u16(packet + 22, value + delta_y);
            count = r_u16(base + 0x1E0) * (expanded ? 5 : 1); ++item;
        } while ((sint32)item < (sint32)count);
    }
}

void level2p_sub_8004FB74(uint32 index, uint32 width, uint32 height)
{
    uint32 base, border, buffer, item, count, packet, origin, value, stage;
    uint32 x, y, right, bottom, left_outer, top_outer, right_outer, bottom_outer;
    FUNCTION_MARKER(0x8004FB74, "LEVEL2P.BIN");
    base = 0x80093140 + index * 0x1EC; border = r_u8(base + 0x1E2);
    if (!border)
    {
        for (buffer = 0; buffer < 2; ++buffer)
        {
            origin = base + buffer * 0x18; item = 0; count = r_u16(base + 0x1E0);
            if (!count) continue;
            do
            {
                packet = origin + item * 0x30;
                value = r_u16(origin + 8); w_u16(packet + 8, value);
                value = r_u16(origin + 10); w_u16(packet + 10, value);
                value = r_u16(origin + 8); w_u16(packet + 12, value + width);
                value = r_u16(origin + 10); w_u16(packet + 14, value);
                value = r_u16(origin + 8); w_u16(packet + 16, value);
                value = r_u16(origin + 10); w_u16(packet + 18, value + height);
                value = r_u16(origin + 8); w_u16(packet + 20, value + width);
                value = r_u16(origin + 10); w_u16(packet + 22, value + height);
                count = r_u16(base + 0x1E0); ++item;
            } while ((sint32)item < (sint32)count);
        }
        return;
    }
    x = (uint32)(sint32)r_s16(base + 8); y = (uint32)(sint32)r_s16(base + 10);
    left_outer = x - border * 2; top_outer = y - border;
    right = x + width; right_outer = right + border * 2;
    bottom = y + height; bottom_outer = bottom + border;
    for (buffer = 0; buffer < 2; ++buffer)
    {
        item = 0; count = r_u16(base + 0x1E0);
        for (stage = 0; stage < 5; ++stage)
        {
            /* Stage1 reuses the preceding count; later stages reload it */
            if (stage >= 2) count = r_u16(base + 0x1E0);
            if ((sint32)item >= (sint32)(count * (stage + 1))) continue;
            do
            {
                packet = base + buffer * 0x18 + item * 0x30;
                if (stage == 0)
                {
                    w_u16(packet + 8, x); w_u16(packet + 10, y); w_u16(packet + 12, right); w_u16(packet + 14, y);
                    w_u16(packet + 16, x); w_u16(packet + 18, bottom); w_u16(packet + 20, right); w_u16(packet + 22, bottom);
                }
                else if (stage == 1)
                {
                    w_u16(packet + 8, left_outer); w_u16(packet + 10, top_outer); w_u16(packet + 12, right_outer); w_u16(packet + 14, top_outer);
                    w_u16(packet + 16, x); w_u16(packet + 18, y); w_u16(packet + 20, right); w_u16(packet + 22, y);
                }
                else if (stage == 2)
                {
                    w_u16(packet + 8, left_outer); w_u16(packet + 10, top_outer); w_u16(packet + 12, x); w_u16(packet + 14, y);
                    w_u16(packet + 16, left_outer); w_u16(packet + 18, bottom_outer); w_u16(packet + 20, x); w_u16(packet + 22, bottom);
                }
                else if (stage == 3)
                {
                    w_u16(packet + 8, right); w_u16(packet + 10, y); w_u16(packet + 12, right_outer); w_u16(packet + 14, top_outer);
                    w_u16(packet + 16, right); w_u16(packet + 18, bottom); w_u16(packet + 20, right_outer); w_u16(packet + 22, bottom_outer);
                }
                else
                {
                    w_u16(packet + 8, x); w_u16(packet + 10, bottom); w_u16(packet + 12, right); w_u16(packet + 14, bottom);
                    w_u16(packet + 16, left_outer); w_u16(packet + 18, bottom_outer); w_u16(packet + 20, right_outer); w_u16(packet + 22, bottom_outer);
                }
                count = r_u16(base + 0x1E0); ++item;
            } while ((sint32)item < (sint32)(count * (stage + 1)));
        }
    }
}

void level2p_sub_8004FF84(uint32 index, uint32 red, uint32 green, uint32 blue)
{
    uint32 base, border, count, item = 0, buffer, stage, packet, r, g, b;
    FUNCTION_MARKER(0x8004FF84, "LEVEL2P.BIN");
    base = 0x80093140 + index * 0x1EC; border = r_u8(base + 0x1E2);
    w_u8(base + 0x1E5, red); w_u8(base + 0x1E6, green); w_u8(base + 0x1E7, blue);
    count = r_u16(base + 0x1E0);
    if (count)
    {
        do
        {
            for (buffer = 0; buffer < 2; ++buffer)
            {
                packet = base + item * 0x30 + buffer * 0x18;
                w_u8(packet + 4, red); w_u8(packet + 5, green); w_u8(packet + 6, blue);
            }
            count = r_u16(base + 0x1E0); ++item;
        } while ((sint32)item < (sint32)count);
    }
    if (!border) return;
    for (stage = 0; stage < 2; ++stage)
    {
        r = (uint32)((sint32)red >> 1); g = (uint32)((sint32)green >> 1); b = (uint32)((sint32)blue >> 1);
        if (!stage) { r += red; g += green; b += blue; }
        count = r_u16(base + 0x1E0) * (stage ? 5 : 3);
        if ((sint32)item >= (sint32)count) continue;
        do
        {
            for (buffer = 0; buffer < 2; ++buffer)
            {
                packet = base + item * 0x30 + buffer * 0x18;
                w_u8(packet + 4, r); w_u8(packet + 5, g); w_u8(packet + 6, b);
            }
            count = r_u16(base + 0x1E0) * (stage ? 5 : 3); ++item;
        } while ((sint32)item < (sint32)count);
    }
}

static void moh_fade_step_rgb(uint32 data)
{
    uint32 red, green, blue, red_step, green_step, blue_step;
    red = r_u8(data + 0x18); red_step = r_u8(data + 0x10);
    green_step = r_u8(data + 0x12); blue_step = r_u8(data + 0x14);
    w_u8(data + 0x18, red + red_step);
    green = r_u8(data + 0x19); blue = r_u8(data + 0x1A);
    w_u8(data + 0x19, green + green_step); w_u8(data + 0x1A, blue + blue_step);
}

static void moh_fade_copy_rgb(uint32 data, uint32 stride, uint32 start, uint32 buffers)
{
    uint32 buffer, channel, index, value;
    for (buffer = 0; buffer < buffers; ++buffer)
        for (channel = 0; channel < 3; ++channel)
        {
            index = r_u32(0x80098B88);
            if (buffer) index = 1 - index;
            value = r_u8(data + 0x18 + channel);
            w_u8(data + index * stride + start + channel, value);
        }
}

void level2p_sub_800501F0(uint32 owner)
{
    uint32 data, value, decrement;
    FUNCTION_MARKER(0x800501F0, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388); value = r_u32(data + 4); decrement = value - 1;
    if ((sint32)value > 0)
    {
        w_u32(data + 4, decrement);
        if (!decrement && (r_u32(data) & 1)) w_u32(owner + 0x370, 0x800503D8);
        return;
    }
    if (r_s32(data + 8) > 0)
    {
        if (r_u32(data) & 1) moh_fade_step_rgb(data);
        if (r_u32(data) & 4)
        {
            value = r_u32(data + 0x6C); decrement = r_u32(data + 0x70); value -= decrement;
            w_u32(data + 0x6C, value);
            if ((sint32)value < 0) value = 0;
            w_u32(data + 0x6C, value);
        }
        w_u32(data + 8, r_u32(data + 8) - 1);
        return;
    }
    moh_fade_copy_rgb(data, 40, 0x20, 2);
    value = r_u32(data + 12);
    if ((sint32)value > 0) w_u32(data + 12, value - 1);
    else level2p_sub_80066C64(owner);
}

uint32 level2p_sub_800504E0(void)
{
    uint32 candidate;
    FUNCTION_MARKER(0x800504E0, "LEVEL2P.BIN");
    candidate = r_u32(0x80099434);
    while (candidate)
    {
        if (r_u32(candidate + 8) == 0x32) return candidate;
        candidate = r_u32(candidate + 4);
    }
    return 0;
}

uint32 level2p_sub_80050528(uint32 flags, uint32 delay, uint32 duration, uint32 hold, uint32 red, uint32 green, uint32 blue, uint32 target_red, uint32 target_green, uint32 target_blue, uint32 rectangle)
{
    uint32 owner, data, first, second, value, offset;
    sint32 divisor, red_step, green_step, blue_step;
    FUNCTION_MARKER(0x80050528, "LEVEL2P.BIN");
    red = (uint8)red; green = (uint8)green; blue = (uint8)blue;
    target_red = (uint8)target_red; target_green = (uint8)target_green; target_blue = (uint8)target_blue;
    owner = level2p_sub_80066934(0, 0x74, 0xFFFFFFFF, 0, 0, 0, 0, 0);
    if (!owner) return 0;
    w_u32(owner + 8, 0x32); w_u32(owner + 12, 0x20000); w_u32(owner + 16, 0x203);
    w_u32(owner + 20, 0); w_u32(owner + 0x36C, 0x800501F0);
    data = r_u32(owner + 0x388);
    if (delay) w_u32(owner + 0x370, 0);
    else if (flags & 1) w_u32(owner + 0x370, 0x800503D8);
    w_u32(owner + 0x384, 0x800653D8); w_u32(owner + 0x374, 0); w_u32(owner + 0x378, 0);
    if (!(flags & 0x18)) flags |= 0x18;
    divisor = (sint16)duration;
    if (!divisor)
    {
        moh_draft_fail_fast("Original fade division BREAK 0x1C00", 0x80050648);
        return 0;
    }
    /* Byte differences cannot reach the original INT_MIN divided by -1 trap */
    red_step = ((sint32)target_red - (sint32)red) / divisor;
    green_step = ((sint32)target_green - (sint32)green) / divisor;
    blue_step = ((sint32)target_blue - (sint32)blue) / divisor;
    w_u32(data, flags); w_u32(data + 4, delay); w_u32(data + 8, duration); w_u32(data + 12, hold);
    w_u8(data + 0x18, red); w_u8(data + 0x19, green); w_u8(data + 0x1A, blue);
    w_u16(data + 0x10, red_step); w_u16(data + 0x12, green_step); w_u16(data + 0x14, blue_step);
    if (flags & 4)
    {
        w_u32(data + 0x6C, 0x300000); w_u32(data + 0x70, 0x300000 / duration);
    }
    else { w_u32(data + 0x6C, 0); w_u32(data + 0x70, 0); }
    moh_draft_missing_call(0x8001B9F8, data + 0x1C);
    moh_draft_missing_call(0x8001B9F8, data + 0x44);
    w_u8(data + 0x20, red); w_u8(data + 0x21, green); w_u8(data + 0x22, blue);
    w_u8(data + 0x48, red); w_u8(data + 0x49, green); w_u8(data + 0x4A, blue);
    w_u8(data + 0x28, 0xFC); w_u8(data + 0x29, 0); w_u8(data + 0x30, 0xFF); w_u8(data + 0x31, 0);
    w_u8(data + 0x38, 0xFC); w_u8(data + 0x39, 3); w_u8(data + 0x40, 0xFF); w_u8(data + 0x41, 3);
    w_u8(data + 0x50, 0xFC); w_u8(data + 0x51, 0); w_u8(data + 0x58, 0xFF); w_u8(data + 0x59, 0);
    w_u8(data + 0x60, 0xFC); w_u8(data + 0x61, 3); w_u8(data + 0x68, 0xFF); w_u8(data + 0x69, 3);
    value = moh_draft_missing_call(0x8001B968, 0, 1, 0x3C0, 0); w_u16(data + 0x32, value);
    value = moh_draft_missing_call(0x8001B968, 0, 1, 0x3C0, 0); w_u16(data + 0x5A, value);
    /* The alternate zero-blend block is unreachable after the original constant3 */
    moh_draft_missing_call(0x8001BA38, data + 0x1C, 1);
    moh_draft_missing_call(0x8001BA38, data + 0x44, 1);
    first = r_u16(data + 0x32); second = r_u16(data + 0x5A);
    w_u16(data + 0x32, (first & 0xFF9F) | 0x40); w_u16(data + 0x5A, (second & 0xFF9F) | 0x40);
    value = moh_draft_missing_call(0x8001BA18, 0x3E0, 0xFD); w_u16(data + 0x2A, value);
    value = moh_draft_missing_call(0x8001BA18, 0x3E0, 0xFD); w_u16(data + 0x52, value);
    for (offset = 0; offset < 0x50; offset += 0x28)
    {
        if (rectangle)
        {
            value = r_u16(rectangle); w_u16(data + offset + 0x24, value);
            value = r_u16(rectangle + 2); w_u16(data + offset + 0x26, value);
            first = r_u16(rectangle); second = r_u16(rectangle + 4); w_u16(data + offset + 0x2C, first + second);
            value = r_u16(rectangle + 2); w_u16(data + offset + 0x2E, value);
            value = r_u16(rectangle); w_u16(data + offset + 0x34, value);
            first = r_u16(rectangle + 2); second = r_u16(rectangle + 6); w_u16(data + offset + 0x36, first + second);
            first = r_u16(rectangle); second = r_u16(rectangle + 4); w_u16(data + offset + 0x3C, first + second);
            first = r_u16(rectangle + 2); second = r_u16(rectangle + 6); w_u16(data + offset + 0x3E, first + second);
        }
        else
        {
            w_u16(data + offset + 0x24, 0); w_u16(data + offset + 0x26, 0);
            w_u16(data + offset + 0x2C, 0); w_u16(data + offset + 0x2E, 0xF0);
            w_u16(data + offset + 0x34, 0x200); w_u16(data + offset + 0x36, 0);
            w_u16(data + offset + 0x3C, 0x200); w_u16(data + offset + 0x3E, 0xF0);
        }
    }
    level2p_sub_80066BA8(owner);
    return owner;
}

void level2p_sub_80050A08(uint32 owner)
{
    uint32 data, value;
    FUNCTION_MARKER(0x80050A08, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388); value = r_u32(data + 4);
    if ((sint32)value > 0) { w_u32(data + 4, value - 1); return; }
    if (r_s32(data + 8) > 0)
    {
        if (r_u32(data) & 1)
        {
            moh_fade_step_rgb(data);
            moh_fade_copy_rgb(data, 24, 0x30, 1);
        }
        w_u32(data + 8, r_u32(data + 8) - 1);
        return;
    }
    moh_fade_copy_rgb(data, 24, 0x30, 1);
    value = r_u32(data + 12);
    if ((sint32)value > 0) w_u32(data + 12, value - 1);
    else level2p_sub_80066C64(owner);
}

static void moh_fade_link_packet(uint32 data, uint32 view, uint32 stride, uint32 offset)
{
    uint32 index, packet, pointer, first, second;
    index = r_u32(view + 0x221C); packet = data + index * stride;
    pointer = r_u32(view + 0x2218); first = r_u32(packet + offset); second = r_u32(pointer);
    w_u32(packet + offset, (first & 0xFF000000) | (second & 0xFFFFFF));
    index = r_u32(view + 0x221C); pointer = r_u32(view + 0x2218);
    packet = data + index * stride + offset; first = r_u32(pointer);
    w_u32(pointer, (first & 0xFF000000) | (packet & 0xFFFFFF));
}

void level2p_sub_800503D8(uint32 owner, uint32 view)
{
    uint32 player, player_data, data, channel, flags;
    FUNCTION_MARKER(0x800503D8, "LEVEL2P.BIN");
    player = r_u32(0x8009942C); player_data = r_u32(player + 0x388);
    data = r_u32(owner + 0x388); channel = r_u8(player_data + 0x726); flags = r_u32(data);
    if (!(flags & (8u << (channel & 31))) || !(flags & 1)) return;
    moh_fade_copy_rgb(data, 40, 0x20, 1);
    moh_fade_link_packet(data, view, 40, 0x1C);
}

void level2p_sub_80050B9C(uint32 owner, uint32 view)
{
    uint32 data;
    FUNCTION_MARKER(0x80050B9C, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388);
    if (r_u32(data + 4)) return;
    moh_fade_link_packet(data, view, 24, 0x2C);
    moh_fade_link_packet(data, view, 8, 0x1C);
}

void level2p_sub_80050FA8(uint32 display, uint32 unused, uint32 height_value)
{
    uint32 value;
    FUNCTION_MARKER(0x80050FA8, "LEVEL2P.BIN");
    (void)unused;
    w_u32(display + 0x2208, 0x21C); w_u32(display + 0x220C, 0x78);
    value = r_u32(height_value); w_u32(height_value, value << 1);
}

void level2p_sub_80050FCC(uint32 display, uint32 x, uint32 y)
{
    uint32 width, height, value;
    FUNCTION_MARKER(0x80050FCC, "LEVEL2P.BIN");
    height = r_u32(display + 4); width = r_u32(display);
    moh_draft_missing_call(0x8001BB28, display + 0x10, 0, 0, width, height);
    height = r_u32(display + 4); width = r_u32(display);
    moh_draft_missing_call(0x8001BB28, display + 0xC8, x, y, width, height);
    height = r_u32(display + 4); width = r_u32(display);
    moh_draft_missing_call(0x8001BB28, display + 0x6C, 0, 0x78, width, height);
    height = r_u32(display + 4); width = r_u32(display);
    moh_draft_missing_call(0x8001BB28, display + 0x124, x, y + 0x78, width, height);
    w_u8(display + 0x26, 0); w_u8(display + 0xDE, 0); w_u8(display + 0x82, 0); w_u8(display + 0x13A, 0);
    value = r_u32(0x800942B4) ? 0 : 1;
    w_u8(display + 0x27, value); w_u8(display + 0xDF, value); w_u8(display + 0x83, value); w_u8(display + 0x13B, value);
    height = r_u32(display + 4); width = r_u32(display);
    moh_draft_missing_call(0x8001BBE8, display + 0x180, x, y, width, height);
    height = r_u32(display + 4); width = r_u32(display);
    moh_draft_missing_call(0x8001BBE8, display + 0x194, 0, 0, width, height);
    moh_draft_missing_call(0x80016E38, 0);
    moh_draft_missing_call(0x80016DA0, 1);
    moh_draft_missing_call(0x8001AF88);
    moh_draft_missing_call(0x80016D40, 0x8005173C);
    moh_draft_missing_call(0x8001AD38);
}

void level2p_sub_80051120(uint32 display, uint32 width, uint32 height, uint32 distance)
{
    uint32 height_view, local_height = height, first, second, screen_x = 0, screen_y = 0;
    FUNCTION_MARKER(0x80051120, "LEVEL2P.BIN");
    moh_draft_missing_call(0x80015B28, 0x8003A390);
    moh_draft_missing_call(0x80015B28, 0x8003A3A8);
    w_u32(display + 8, distance); w_u32(display, width); w_u32(display + 4, height);
    height_view = moh_draft_local_address(&local_height, sizeof(local_height));
    /* Original second argument points at width but the full callee never reads it */
    level2p_sub_80050FA8(display, 0, height_view);
    moh_draft_missing_call(0x80015B28, 0x8003A3BC);
    moh_draft_missing_call(0x8001BAA0);
    first = r_u32(display); second = r_u32(display + 4);
    moh_draft_missing_call(0x8001BA78, (uint32)((sint32)first / 2), (uint32)((sint32)second / 2));
    moh_draft_missing_call(0x8001BA68, r_u32(display + 8));
    if (r_u32(display + 4) == 0x1E0)
    {
        first = r_u32(display);
        if ((sint32)first < 0x141 && !r_u32(0x800942B4)) screen_x = first;
    }
    else screen_y = 0xF0;
    level2p_sub_80050FCC(display, screen_x, screen_y);
    w_u32(0x80098B88, 0); w_u32(0x80098B70, 0);
    first = moh_draft_missing_call(0x80015DB8, 0xFFFFFFFF);
    w_u32(0x800942AC, first);
}

void level2p_sub_80051240(uint32 display, uint32 edge)
{
    uint32 packet, index, pointer, first, second, bottom;
    FUNCTION_MARKER(0x80051240, "LEVEL2P.BIN");
    if (r_u32(0x800968E8)) return;
    index = r_u32(display + 0x221C); packet = 0x800AAD80 + index * 24;
    w_u8(packet + 3, 5); w_u8(packet + 7, 0x28);
    w_u8(packet + 4, 0x1E); w_u8(packet + 5, 0x1E); w_u8(packet + 6, 0x22);
    if (!edge)
    {
        bottom = 0x78;
        w_u16(packet + 8, 0); w_u16(packet + 10, 0x76); w_u16(packet + 12, 0x200); w_u16(packet + 14, 0x76);
        w_u16(packet + 16, 0); w_u16(packet + 18, bottom); w_u16(packet + 20, 0x200);
    }
    else
    {
        if (edge != 1) return;
        bottom = 2;
        w_u16(packet + 8, 0); w_u16(packet + 10, 0); w_u16(packet + 12, 0x200); w_u16(packet + 14, 0);
        w_u16(packet + 16, 0); w_u16(packet + 18, bottom); w_u16(packet + 20, 0x200);
    }
    w_u16(packet + 22, bottom);
    pointer = r_u32(display + 0x2218); first = r_u32(packet); second = r_u32(pointer);
    w_u32(packet, (first & 0xFF000000) | (second & 0xFFFFFF));
    pointer = r_u32(display + 0x2218); first = r_u32(pointer);
    w_u32(pointer, (first & 0xFF000000) | (packet & 0xFFFFFF));
}

void level2p_sub_8005154C(void)
{
    uint32 value;
    FUNCTION_MARKER(0x8005154C, "LEVEL2P.BIN");
    if (r_u32(0x800942B0))
    {
        while (moh_draft_missing_call(0x80016E38, 1))
        {
            value = r_u32(0x800942BC); w_u32(0x800942BC, value + 1);
        }
    }
    level2p_sub_8008CE50();
}

void level2p_sub_800515A4(void)
{
    uint32 table;
    FUNCTION_MARKER(0x800515A4, "LEVEL2P.BIN");
    level2p_sub_8005154C();
    table = r_u32(0x80098B70) ? 0x80097310 : 0x80098310;
    moh_draft_missing_call(0x800172D8, table);
}

void level2p_sub_800515F0(uint32 owner, uint32 buffer)
{
    uint32 data, handle, player = 0, screen, offset, first, second, third, fourth, table, cursor, previous, maximum, used;
    FUNCTION_MARKER(0x800515F0, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388); handle = r_u32(data + 12);
    w_u32(0x80099428, owner);
    if (handle) player = r_u32(0x80099460 + ((handle & 0xFFFF) << 2));
    /* TODO: Original zero handle still dereferences player address0 without a fallback */
    data = r_u32(player + 0x388); screen = r_u32(data + 0x678);
    w_u32(0x8009942C, player); w_u32(0x80096974, screen);
    moh_draft_missing_call(0x8001BA68, screen);
    data = r_u32(owner + 0x388);
    for (offset = 0; offset < 0x30; offset += 16)
    {
        first = r_u32(data + 0xE4 + offset); second = r_u32(data + 0xE8 + offset);
        third = r_u32(data + 0xEC + offset); fourth = r_u32(data + 0xF0 + offset);
        w_u32(0x80098B34 + offset, first); w_u32(0x80098B38 + offset, second);
        w_u32(0x80098B3C + offset, third); w_u32(0x80098B40 + offset, fourth);
    }
    first = r_u32(data + 0x114); second = r_u32(data + 0x118); third = r_u32(data + 0x11C);
    w_u32(0x80098B64, first); w_u32(0x80098B68, second); w_u32(0x80098B6C, third);
    table = 0x80096B14 + (buffer << 11); first = r_u32(0x80098B70); table += first << 12;
    w_u32(0x80098B84, table); moh_draft_missing_call(0x8001722C, table, 0x200);
    cursor = r_u32(0x80094C38); previous = r_u32(0x800942A4); maximum = r_u32(0x800942B8);
    used = cursor - previous;
    if ((sint32)maximum < (sint32)used) w_u32(0x800942B8, used);
    w_u32(0x80098B88, buffer); level2p_sub_80053614(buffer);
    w_u32(0x800942A4, r_u32(0x80094C38));
}

void level2p_sub_8005173C(void)
{
    uint32 buffer, environment, table;
    FUNCTION_MARKER(0x8005173C, "LEVEL2P.BIN");
    if (!r_u32(0x80094294)) return;
    w_u32(0x80094294, 0);
    buffer = r_u32(0x80098B70); environment = 0x800969D8 + buffer * 0xB8;
    moh_draft_missing_call(0x80017348, environment);
    buffer = r_u32(0x80098B70); table = 0x80097B10 + (buffer << 12);
    moh_draft_missing_call(0x800172D8, table);
    w_u32(0x80094298, 1);
}

void level2p_sub_80053614(uint32 buffer)
{
    uint32 resource, size, base, cursor;
    FUNCTION_MARKER(0x80053614, "LEVEL2P.BIN");
    resource = r_u32(0x80098FB0); size = r_u32(resource + 0x98);
    base = r_u32(0x80094C60); cursor = base + size * buffer;
    w_u32(0x80094C38, cursor);
    size = r_u32(resource + 0x98); w_u32(0x80094C3C, cursor + size - 1);
}

void level2p_sub_8008CD00(void)
{
    uint32 phase;
    FUNCTION_MARKER(0x8008CD00, "LEVEL2P.BIN");
    phase = r_u32(0x800AAC28);
    if (phase == 0) level2p_sub_8006145C();
    else if (phase == 1)
    {
        level2p_sub_8006145C(); level2p_sub_8006F2D0();
    }
    else
    {
        if (phase == 2)
        {
            level2p_sub_8006145C(); level2p_sub_80068C40();
        }
        if (phase == 2 || phase == 3)
        {
            level2p_sub_8006145C(); level2p_sub_80053DF4();
        }
        w_u32(0x800AAC28, 0xFFFFFFFF);
    }
    phase = r_u32(0x800AAC28); w_u32(0x800AAC28, phase + 1);
}

void level2p_sub_8008CDBC(void)
{
    uint32 phase;
    FUNCTION_MARKER(0x8008CDBC, "LEVEL2P.BIN");
    phase = r_u32(0x800AAC2C);
    if (phase == 0) level2p_sub_800614FC();
    else if (phase == 1) level2p_sub_8006F368();
    else if (phase == 2) level2p_sub_8008CD00();
    else w_u32(0x800AAC2C, 0xFFFFFFFF);
    phase = r_u32(0x800AAC2C); w_u32(0x800AAC2C, phase + 1);
}

void level2p_sub_8008CE50(void)
{
    uint32 tick, previous, phase, elapsed, buffer;
    FUNCTION_MARKER(0x8008CE50, "LEVEL2P.BIN");
    w_u32(0x800AAC24, 0); w_u32(0x800AAC20, 1);
    level2p_sub_8008CD00();
    tick = moh_draft_missing_call(0x80015DB8, 0xFFFFFFFF); previous = r_u32(0x800942AC);
    if (previous == tick)
    {
        do
        {
            level2p_sub_8008CDBC(); tick = moh_draft_missing_call(0x80015DB8, 0xFFFFFFFF);
            previous = r_u32(0x800942AC);
        } while (previous == tick);
        previous = r_u32(0x800942AC);
    }
    if (previous == tick - 1)
    {
        do
        {
            level2p_sub_8008CD00(); tick = moh_draft_missing_call(0x80015DB8, 0xFFFFFFFF);
            previous = r_u32(0x800942AC);
        } while (previous == tick - 1);
    }
    phase = r_u32(0x800AAC30); previous = r_u32(0x800942AC); phase = 1 - phase;
    elapsed = (uint32)((sint32)(tick - previous + phase) >> 1);
    w_u32(0x800AAC30, phase);
    w_u32(0x800968D8, (sint32)elapsed > 0 ? elapsed - 1 : 0);
    w_u32(0x800AAC20, 0);
    if (!r_u32(0x800AAC24))
    {
        buffer = r_u32(0x80098B70); w_u32(0x800AAC24, 1); buffer = buffer == 0;
        w_u32(0x80098B70, buffer);
        moh_draft_missing_call(0x80017408, 0x80096AEC + buffer * 20);
    }
    buffer = r_u32(0x80098B70);
    moh_draft_missing_call(0x80017348, 0x8009697C + buffer * 0xB8);
    w_u32(0x800942AC, tick);
}

uint32 level2p_sub_8006F2D0(void)
{
    uint32 count, head, next, slot, instance;
    FUNCTION_MARKER(0x8006F2D0, "LEVEL2P.BIN");
    count = r_u32(0x8009A078);
    if (!count) return 0;
    head = r_u32(0x8009A070); slot = 0x8009A084 + (head << 2); instance = r_u32(slot);
    next = head + 1; w_u32(0x8009A070, next); w_u32(slot, 0);
    if (next >= 64) w_u32(0x8009A070, head - 63);
    w_u32(0x8009A078, count - 1);
    if (count == 1) { w_u32(0x8009A070, 0); w_u32(0x8009A074, 0); }
    level2p_sub_8006F390(instance);
    return r_u32(0x8009A078) < count;
}

void level2p_sub_8006F368(void)
{
    FUNCTION_MARKER(0x8006F368, "LEVEL2P.BIN");
    while (level2p_sub_8006F2D0()) { }
}

void level2p_sub_80068C40(void)
{
    uint32 count, index, offset, object, first, address, last;
    FUNCTION_MARKER(0x80068C40, "LEVEL2P.BIN");
    count = r_u32(0x8009945C);
    for (index = 0; index < count; ++index)
    {
        offset = index << 2;
        if (!r_u32(0x80099C60 + offset)) continue;
        object = r_u32(0x80099460 + offset);
        if (!object) continue;
        first = r_u32(0x80099408);
        w_u32(0x80099460 + offset, 0); w_u32(0x80099860 + offset, 0); w_u32(0x80099C60 + offset, 0);
        if (object == first) w_u32(0x80099408, 0);
        for (address = 0x8009940C; address <= 0x80099424; address += 4)
            if (object == r_u32(address)) w_u32(address, 0);
        for (address = 0x80099430; address <= 0x80099440; address += 4)
            if (object == r_u32(address)) w_u32(address, 0);
        for (address = 0x80099448; address < 0x80099458; address += 4)
            if (object == r_u32(address)) w_u32(address, 0);
    }
    if (!r_u32(0x8009945C)) return;
    do
    {
        last = r_u32(0x8009945C) - 1;
        if (r_u32(0x80099460 + (last << 2))) return;
        w_u32(0x8009945C, last);
    } while (last);
}

uint32 level2p_sub_80053B20(uint32 resource)
{
    uint32 index;
    FUNCTION_MARKER(0x80053B20, "LEVEL2P.BIN");
    for (index = 0; index < 4; ++index)
        if (r_u32(0x80094C50 + (index << 2)) == resource) return 1;
    return 0;
}

void level2p_sub_80053B58(void)
{
    uint32 index, count = 0, resource, value;
    FUNCTION_MARKER(0x80053B58, "LEVEL2P.BIN");
    for (index = 4; index; --index) w_u32(0x80095354 + ((index - 1) << 2), 0xFFFFFFFF);
    resource = r_u32(0x80095350);
    if (!resource) return;
    for (index = 0; index < 4; ++index)
    {
        if (index) resource = r_u32(0x80095350);
        value = (uint32)(sint32)r_s16(resource + 0x30 + index * 2);
        if (!level2p_sub_80053B20(value))
        {
            resource = r_u32(0x80095350);
            value = (uint32)(sint32)r_s16(resource + 0x30 + index * 2);
            w_u32(0x80095354 + (count << 2), value); ++count;
        }
    }
    w_u32(0x80094C44, count);
}

uint32 level2p_sub_80053C90(uint32 resource)
{
    uint32 current;
    FUNCTION_MARKER(0x80053C90, "LEVEL2P.BIN");
    current = r_u32(0x80095350);
    if ((uint32)(sint32)r_s16(current + 0x30) == resource) return 0;
    if ((uint32)(sint32)r_s16(current + 0x32) == resource) return 0;
    if ((uint32)(sint32)r_s16(current + 0x34) == resource) return 0;
    return (uint32)(sint32)r_s16(current + 0x36) != resource;
}

void level2p_sub_80053CF0(void)
{
    uint32 index, resource, count;
    FUNCTION_MARKER(0x80053CF0, "LEVEL2P.BIN");
    w_u32(0x80094C48, 0);
    for (index = 4; index; --index) w_u32(0x80095364 + ((index - 1) << 2), 0xFFFFFFFF);
    if (!r_u32(0x80095350)) return;
    for (index = 0; index < 4; ++index)
    {
        resource = r_u32(0x80094C50 + (index << 2));
        if (resource != 0xFFFFFFFF && level2p_sub_80053C90(resource))
        {
            count = r_u32(0x80094C48);
            w_u32(0x80095364 + (count << 2), index); w_u32(0x80094C48, count + 1);
        }
    }
}

void level2p_sub_8008AE58(uint32 index)
{
    uint32 slot;
    FUNCTION_MARKER(0x8008AE58, "LEVEL2P.BIN");
    slot = 0x8009506C + index * 0xB0;
    if (r_u32(slot)) level2p_sub_80088FA8(r_u32(slot), 0);
    w_u32(slot, 0);
}

void level2p_sub_8008AC7C(uint32 filename, uint32 index)
{
    uint32 buffer, result, status;
    FUNCTION_MARKER(0x8008AC7C, "LEVEL2P.BIN");
    buffer = r_u32(0x8009532C + (index << 2));
    result = level2p_sub_8004A29C(filename, 0, 10000, buffer);
    status = r_u32(0x800968E0);
    w_u32(0x80094C7C + (index << 8), result); w_u32(0x800A94C8, index);
    moh_draft_missing_call(0x80015B28, 0x8003D918, filename, status);
}

void level2p_sub_80053DF4(void)
{
    uint8 filename[64];
    uint32 index, resource, count, available, filename_view, name, selected;
    FUNCTION_MARKER(0x80053DF4, "LEVEL2P.BIN");
    index = r_u32(0x800A94C8);
    if (index != 0xFFFFFFFF)
    {
        resource = r_u32(0x80094C50 + (index << 2));
        if (!level2p_sub_80053C90(resource)) return;
        moh_draft_missing_call(0x80015B28, 0x8003C09C);
    }
    resource = r_u32(0x80095350); w_u32(0x80094C4C, resource);
    level2p_sub_80053B58(); level2p_sub_80053CF0();
    count = r_u32(0x80094C44);
    if (!count) return;
    available = r_u32(0x80094C48);
    if (count != available) level2p_sub_800885B8(5, 0x20);
    index = r_u32(0x80095364); level2p_sub_8008AE58(index);
    /* TODO: Original unbounded format uses a64-byte local buffer */
    filename_view = moh_draft_local_address(filename, sizeof(filename));
    name = r_u32(0x80098FB0); selected = r_u32(0x80095354);
    moh_draft_missing_call(0x8001BE18, filename_view, 0x8003C090, name, selected);
    level2p_sub_8008AC7C(filename_view, index);
    selected = r_u32(0x80095354); w_u32(0x80094C50 + (index << 2), selected);
}

void level2p_sub_8005134C(void)
{
    uint32 result, first, second, source_data, destination_data, value;
    FUNCTION_MARKER(0x8005134C, "LEVEL2P.BIN");
    if (!r_u32(0x80094298))
    {
        if (r_u32(0x80094294))
        {
            result = moh_draft_missing_call(0x80016E38, 1);
            if (result)
            {
                w_u32(0x80094294, 0); w_u32(0x80094298, 1);
                while (moh_draft_missing_call(0x80016E38, 1)) { }
                w_u32(0x80094294, 1); w_u32(0x80094298, 0);
            }
            level2p_sub_8005173C();
        }
        while (moh_draft_missing_call(0x80016E38, 1)) { }
    }
    level2p_sub_800515F0(r_u32(0x80099418), 0);
    level2p_sub_80051240(0x8009696C, 0);
    level2p_sub_8005C624(r_u32(0x8009943C)); level2p_sub_8005F45C();
    first = r_u32(0x80098B84); second = r_u32(0x80098B88); level2p_sub_8005B1DC(first, second, 0);
    level2p_sub_800677FC(0x8009696C, r_u32(0x80099434));
    level2p_sub_8007CE34(r_u32(0x8009943C), 0x8009696C); level2p_sub_800515A4();
    w_u32(0x80094290, 1);
    first = r_u32(0x80099418); second = r_u32(0x8009941C);
    source_data = r_u32(first + 0x388); destination_data = r_u32(second + 0x388);
    value = r_u32(source_data + 0xC4); w_u32(destination_data + 0xC4, value);
    level2p_sub_800515F0(second, 1); level2p_sub_80051240(0x8009696C, 1);
    level2p_sub_8005C624(r_u32(0x80099440)); level2p_sub_8005F45C();
    first = r_u32(0x80098B84); second = r_u32(0x80098B88); level2p_sub_8005B1DC(first, second, 1);
    level2p_sub_800677FC(0x8009696C, r_u32(0x80099434));
    level2p_sub_8007CE34(r_u32(0x80099440), 0x8009696C);
    moh_draft_missing_call(0x8001BA68, 400); result = moh_draft_missing_call(0x80016E38, 1);
    w_u32(0x80094298, 0); w_u32(0x80094294, 1);
    if (!result) level2p_sub_8005173C();
}

void level2p_sub_8005F45C(void)
{
    uint32 index, item, buffer, table, packet, slot, first, second, depth;
    FUNCTION_MARKER(0x8005F45C, "LEVEL2P.BIN");
    for (index = 0; index < 12; ++index)
    {
        item = 0x80098B90 + index * 0x58;
        if (!r_u8(item + 4) || (r_u32(item) & 8)) continue;
        buffer = r_u32(0x80098B88); table = r_u32(0x80098B84);
        depth = (uint32)(sint32)r_s8(item + 7);
        packet = item + 8 + buffer * 40; slot = table + (depth << 2);
        first = r_u32(packet); second = r_u32(slot);
        w_u32(packet, (first & 0xFF000000) | (second & 0xFFFFFF));
        depth = (uint32)(sint32)r_s8(item + 7); table = r_u32(0x80098B84); buffer = r_u32(0x80098B88);
        slot = table + (depth << 2); packet = item + 8 + buffer * 40; first = r_u32(slot);
        w_u32(slot, (first & 0xFF000000) | (packet & 0xFFFFFF));
    }
}

void level2p_sub_800677FC(uint32 view, uint32 object)
{
    uint32 callback, next, attachment, flags, instance, resource;
    FUNCTION_MARKER(0x800677FC, "LEVEL2P.BIN");
    while (object)
    {
        callback = r_u32(object + 0x370); next = r_u32(object + 4);
        if (callback) moh_draft_indirect_call(callback, object, view);
        attachment = r_u32(object + 0x100);
        if (attachment)
        {
            flags = r_u32(attachment + 4);
            if (!(flags & 0x10)) w_u32(attachment + 4, flags & 0xFFFFFFDF);
        }
        if ((r_u32(object + 12) & 0x8000) && r_s32(object + 0x290) <= 0 && r_u32(object + 0x298))
        {
            instance = r_u32(object + 0x1C);
            if (instance)
            {
                resource = r_u32(instance + 4);
                if (r_u32(resource + 12)) level2p_sub_800615E4(0, instance, 2);
            }
            level2p_sub_80066C64(object);
        }
        object = next;
    }
}

void level2p_sub_8007CE34(uint32 owner, uint32 view)
{
    uint32 data, channel;
    FUNCTION_MARKER(0x8007CE34, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388); channel = r_u8(data + 0x726);
    level2p_sub_8004F664(view, channel);
    moh_fade_link_packet(data, view, 40, 0x68C);
}

uint32 level2p_sub_8005B1DC(uint32 ordering_table, uint32 buffer, uint32 channel)
{
    uint32 limit, limit_view, clut, reserved, cursor, count, mask, row, item, text, offset, character, value, first, second;
    FUNCTION_MARKER(0x8005B1DC, "LEVEL2P.BIN");
    clut = (uint16)moh_draft_missing_call(0x8001BA18, 0x3F0, 0xFA);
    limit_view = moh_draft_local_address(&limit, sizeof(limit));
    reserved = level2p_sub_8005365C(buffer, limit_view); cursor = reserved + 12; count = 0;
    row = 0; first = r_u32(0x80096198); limit -= 20;
    if ((sint32)first > 0)
    {
        mask = 1u << (channel & 31);
        do
        {
            item = 0x80096214 + row * 32;
            if (r_u32(item + 24) & mask)
            {
                text = r_u32(item); offset = 0;
                if (r_u8(text))
                {
                    do
                    {
                        if (limit < cursor)
                        {
                            level2p_sub_8005367C(limit);
                            return 0xFFFFFFFF;
                        }
                        character = r_u8(text);
                        if (character != 0x20)
                        {
                            moh_draft_missing_call(0x8001C748, cursor);
                            if (r_u8(item + 11)) { value = r_u8(cursor + 7); w_u8(cursor + 7, value + 2); }
                            value = r_u16(item + 4); w_u16(cursor + 8, value + offset);
                            value = r_u16(item + 6);
                            w_u16(cursor + 16, 10); w_u16(cursor + 18, 7); w_u16(cursor + 10, value);
                            /* TODO: Original glyph table indices accept every non-space byte */
                            character = r_u8(text); value = r_u8(0x8009617B + character); w_u8(cursor + 12, value);
                            character = r_u8(text); value = r_u8(0x800961B7 + character); w_u8(cursor + 13, value);
                            value = r_u8(item + 8); w_u8(cursor + 4, value);
                            value = r_u8(item + 9); ++count; w_u8(cursor + 5, value);
                            value = r_u8(item + 10); w_u16(cursor + 14, clut); w_u8(cursor + 6, value);
                            first = r_u32(cursor); second = r_u32(ordering_table);
                            w_u32(cursor, (first & 0xFF000000) | (second & 0xFFFFFF));
                            first = r_u32(ordering_table); value = cursor & 0xFFFFFF; cursor += 20;
                            w_u32(ordering_table, (first & 0xFF000000) | value);
                        }
                        ++text; character = r_u8(text); offset += 12;
                    } while (character);
                }
            }
            first = r_u32(0x80096198); ++row;
        } while ((sint32)row < (sint32)first);
    }
    value = moh_draft_missing_call(0x8001B968, 0, 1, 0x3F8, 0);
    moh_draft_missing_call(0x8001B8C8, reserved, 0, 1, (uint16)value, 0);
    first = r_u32(ordering_table); second = r_u32(reserved);
    w_u32(reserved, (second & 0xFF000000) | (first & 0xFFFFFF));
    first = r_u32(ordering_table);
    w_u32(ordering_table, (first & 0xFF000000) | (reserved & 0xFFFFFF));
    level2p_sub_8005367C(cursor);
    return count;
}

uint32 level2p_sub_8006888C(uint32 owner, uint32 other)
{
    uint32 x, z, first, second, minimum;
    FUNCTION_MARKER(0x8006888C, "LEVEL2P.BIN");
    first = r_u32(other + 0x20C); second = r_u32(owner + 0x20C);
    z = r_u32(other + 0x214); x = first - second; second = r_u32(owner + 0x214);
    if ((sint32)x < 0) x = 0 - x;
    z -= second; if ((sint32)z < 0) z = 0 - z;
    minimum = (sint32)z < (sint32)x ? z : x;
    return x + z - (uint32)((sint32)minimum >> 1);
}

uint32 level2p_sub_8005C1A8(uint32 owner, uint32 channel, uint32 ordering_table, uint32 buffer)
{
    uint8 translation[16], matrix[32], point[8];
    uint32 translation_view, matrix_view, point_view, stack, saved, index, packet, other, value, first, second;
    uint32 distance, delta, x, z, mac_x, mac_y, mac_z, screen_x, screen_y, color;
    sint32 height;
    FUNCTION_MARKER(0x8005C1A8, "LEVEL2P.BIN");
    translation_view = moh_draft_local_address(translation, sizeof(translation));
    moh_draft_missing_call(0x800158AC, translation_view, 0, 16);
    stack = r_u32(0x80099100); saved = 0x80099104 + stack;
    for (index = 0; index < 8; index += 2)
    {
        first = xport_gte_read_control(index); second = xport_gte_read_control(index + 1);
        w_u32(saved + index * 4, first); w_u32(saved + index * 4 + 4, second);
    }
    packet = 0x800AB9F8 + (channel << 7) + (buffer << 6);
    w_u32(0x80099100, stack + 32);
    for (index = 0; index < 2; ++index)
    {
        first = r_u32(packet); second = r_u32(ordering_table);
        w_u32(packet, (first & 0xFF000000) | (second & 0xFFFFFF));
        first = r_u32(ordering_table); w_u32(ordering_table, (first & 0xFF000000) | (packet & 0xFFFFFF));
        packet += 16;
    }
    other = r_u32(0x8009943C); if (owner == other) other = r_u32(0x80099440);
    distance = (uint32)((sint32)level2p_sub_8006888C(owner, other) >> 16);
    /* TODO: Original point and matrix padding stays opaque at the fail-fast adapter */
    point_view = moh_draft_local_address(point, sizeof(point));
    first = r_u32(other + 0x20C); second = r_u32(owner + 0x20C); delta = first - second;
    if ((sint32)distance >= 0x1401)
    {
        x = (uint32)((sint32)((uint32)((sint32)delta >> 16) * 0x1400) / (sint32)distance);
        w_u16(point_view + 2, 0); w_u16(point_view, x);
        first = r_u32(other + 0x214); second = r_u32(owner + 0x214); delta = first - second;
        z = (uint32)((sint32)((uint32)((sint32)delta >> 16) * 0x1400) / (sint32)distance);
    }
    else
    {
        w_u16(point_view + 2, 0); x = (uint32)((sint32)delta >> 16); w_u16(point_view, x);
        first = r_u32(other + 0x214); second = r_u32(owner + 0x214); z = (uint32)((sint32)(first - second) >> 16);
    }
    w_u16(point_view + 4, z);
    matrix_view = moh_draft_local_address(matrix, sizeof(matrix));
    w_u16(matrix_view + 16, 0x1000); w_u16(matrix_view + 8, 0x1000); w_u16(matrix_view, 0x1000);
    w_u16(matrix_view + 14, 0); w_u16(matrix_view + 12, 0); w_u16(matrix_view + 10, 0);
    w_u16(matrix_view + 6, 0); w_u16(matrix_view + 4, 0); w_u16(matrix_view + 2, 0);
    value = 0 - r_u32(owner + 0x22C);
    moh_draft_missing_call(0x80016248, (uint32)((sint32)value >> 16), matrix_view);
    moh_draft_missing_call(0x80016218, matrix_view, translation_view);
    moh_contact_load_matrix(matrix_view, 8); moh_contact_vector_execute(point_view, 0x486012);
    mac_x = xport_gte_read_data(25); mac_y = xport_gte_read_data(26); mac_z = xport_gte_read_data(27);
    first = r_u32(other + 0x210); second = r_u32(owner + 0x210);
    height = (sint32)(second - first) >> 18;
    if (height > 100) height = 100; else if (height < -100) height = -100;
    screen_x = (mac_x >> 8) + 60; screen_y = ((0 - mac_z) >> 9) + 44; color = 100 - (uint32)height;
    for (index = 2; index < 4; ++index)
    {
        w_u16(packet + 8, screen_x); w_u16(packet + 10, screen_y);
        w_u16(packet + 12, screen_x); w_u16(packet + 14, screen_y);
        if (index & 1) { w_u16(packet + 10, screen_y - 1); w_u16(packet + 14, screen_y + 1); }
        else
        {
            first = r_u16(packet + 8); second = r_u16(packet + 12);
            w_u16(packet + 8, first - 1); w_u16(packet + 12, second + 1);
        }
        w_u8(packet + 4, ~color); w_u8(packet + 5, color + 32);
        first = r_u32(packet); second = r_u32(ordering_table);
        w_u32(packet, (first & 0xFF000000) | (second & 0xFFFFFF));
        first = r_u32(ordering_table); w_u32(ordering_table, (first & 0xFF000000) | (packet & 0xFFFFFF));
        packet += 16;
    }
    stack = r_u32(0x80099100) - 32; saved = 0x80099104 + stack;
    second = r_u32(saved + 4); w_u32(0x80099100, stack); first = r_u32(saved);
    xport_gte_write_control(0, first); xport_gte_write_control(1, second);
    for (index = 2; index < 8; index += 2)
    {
        second = r_u32(saved + index * 4 + 4); first = r_u32(saved + index * 4);
        xport_gte_write_control(index, first); xport_gte_write_control(index + 1, second);
    }
    return first;
}

void level2p_sub_8005C624(uint32 owner)
{
    uint32 data, disabled, channel, table, buffer, packet, index = 0, red = 0, green = 64, first, second;
    sint32 empty;
    FUNCTION_MARKER(0x8005C624, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388); disabled = r_u32(0x800968E8); channel = r_u8(data + 0x726);
    if (disabled) return;
    table = r_u32(0x80098B84); buffer = r_u32(0x80098B88); empty = 32 - r_s16(owner + 0xF6);
    level2p_sub_8005C1A8(owner, channel, table, buffer);
    packet = 0x800AADF8 + channel * 0x600 + buffer * 0x300;
    /* TODO: Original first loop can run beyond32 primitives for a negative input */
    while ((sint32)index < empty)
    {
        w_u8(packet + 4, 1); w_u8(packet + 5, 1); w_u8(packet + 6, 1);
        if (index < 16) { ++green; red += 5; } else { green -= 5; --red; }
        if (!(index & 1))
        {
            first = r_u32(packet); second = r_u32(table);
            w_u32(packet, (first & 0xFF000000) | (second & 0xFFFFFF));
            first = r_u32(table); w_u32(table, (first & 0xFF000000) | (packet & 0xFFFFFF));
        }
        ++index; packet += 24;
    }
    while (index < 32)
    {
        w_u8(packet + 4, red); w_u8(packet + 5, green); w_u8(packet + 6, 0);
        if (index < 16) { ++green; red += 5; } else { green -= 5; --red; }
        if (!(index & 1))
        {
            first = r_u32(packet); second = r_u32(table);
            w_u32(packet, (first & 0xFF000000) | (second & 0xFFFFFF));
            first = r_u32(table); w_u32(table, (first & 0xFF000000) | (packet & 0xFFFFFF));
        }
        ++index; packet += 24;
    }
}

void level2p_sub_80051E2C(uint32 owner, uint32 other)
{
    uint32 direction[3], data, first, second, x, z, minimum, maximum, distance, product, value, axis;
    FUNCTION_MARKER(0x80051E2C, "LEVEL2P.BIN");
    direction[0] = r_u16(owner + 0x344); data = r_u32(owner + 0x388);
    direction[1] = r_u16(owner + 0x34A); direction[2] = r_u16(owner + 0x350);
    first = (uint32)(sint32)r_s16(owner + 0x20E); second = (uint32)(sint32)r_s16(other + 0x20E);
    x = (uint32)(sint32)(sint16)(first - second);
    first = (uint32)(sint32)r_s16(owner + 0x216); second = (uint32)(sint32)r_s16(other + 0x216);
    if ((sint32)x < 0) x = 0 - x;
    z = (uint32)(sint32)(sint16)(first - second); if ((sint32)z < 0) z = 0 - z;
    maximum = (sint32)x < (sint32)z ? z : x; minimum = (sint32)x < (sint32)z ? x : z;
    distance = maximum + (uint32)((sint32)minimum >> 2);
    distance -= (uint32)((sint32)distance >> 2);
    for (axis = 0; axis < 3; ++axis)
    {
        product = (0 - (uint32)(sint32)(sint16)direction[axis]) * distance;
        value = r_u32(owner + 0x20C + axis * 4);
        w_u32(owner + 0x20C + axis * 4, value + ((uint32)((sint32)product >> 12) << 16));
    }
    w_u16(data + 0x38, r_u16(data + 0x38) | 0x2000);
}
