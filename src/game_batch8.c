#include "game_level2p_signatures.h"
#include "xport_trace.h"
#include "psx.h"

static void moh_bone_save_controls(void)
{
    PsxGteSnapshot snapshot;
    uint32 stack = r_u32(0x80099100), index, element, value;
    psx_gte_snapshot(&snapshot);
    for (index = 0; index < 8; index++)
    {
        element = index * 2;
        if (index < 4) value = (uint32)(uint16)snapshot.rotation.m[element / 3][element % 3]
            | ((uint32)(uint16)snapshot.rotation.m[(element + 1) / 3][(element + 1) % 3] << 16);
        else if (index == 4) value = (uint32)(sint32)snapshot.rotation.m[2][2];
        else value = (uint32)snapshot.translation[index - 5];
        w_u32(0x80099104 + stack + index * 4, value);
    }
    w_u32(0x80099100, stack + 32);
}

static void moh_bone_restore_controls(void)
{
    uint32 stack = r_u32(0x80099100) - 32, index, first, second;
    second = r_u32(0x80099108 + stack);
    w_u32(0x80099100, stack);
    first = r_u32(0x80099104 + stack);
    xport_gte_write_control(0, first);
    xport_gte_write_control(1, second);
    for (index = 2; index < 8; index += 2)
    {
        second = r_u32(0x80099104 + stack + (index + 1) * 4);
        first = r_u32(0x80099104 + stack + index * 4);
        xport_gte_write_control(index, first);
        xport_gte_write_control(index + 1, second);
    }
}

static void moh_bone_rotation(uint32 first, uint32 second, uint32 matrix[5])
{
    uint32 a = (uint32)(sint32)(sint16)first, b = (uint32)((sint32)first >> 16);
    uint32 c = (uint32)(sint32)(sint16)second, d = (uint32)((sint32)second >> 16);
    uint32 packed, axis, pass1[3], pass2[3], pass3[2], diagonal;
    xport_gte_write_control(0, (d << 16) | (a & 0xFFFF));
    packed = (b << 16) | (c & 0xFFFF);
    xport_gte_write_control(1, packed);
    xport_gte_write_control(3, packed);
    xport_gte_write_control(2, ((0 - c) & 0xFFFF) | (d << 16));
    xport_gte_write_control(4, 0 - a);
    xport_gte_write_data(0, (d << 17) | ((a << 1) & 0xFFFF));
    xport_gte_write_data(1, 0);
    xport_gte_execute(0x486012);
    for (axis = 0; axis < 3; axis++) pass1[axis] = xport_gte_read_data(25 + axis);
    xport_gte_write_data(0, (b << 1) & 0xFFFF);
    xport_gte_write_data(1, d << 1);
    xport_gte_execute(0x486012);
    for (axis = 0; axis < 3; axis++) pass2[axis] = xport_gte_read_data(25 + axis);
    xport_gte_write_data(0, (0 - (b << 1)) << 16);
    xport_gte_write_data(1, a << 1);
    xport_gte_execute(0x486012);
    pass3[0] = xport_gte_read_data(25);
    pass3[1] = xport_gte_read_data(26);
    diagonal = ((uint32)((sint32)(c * c) >> 12) + (uint32)((sint32)(d * d) >> 12)) * 2 - 4096;
    matrix[0] = ((pass2[0] & 0xFFFF) << 16) | ((pass1[0] - 4096) & 0xFFFF);
    matrix[1] = ((pass1[1] & 0xFFFF) << 16) | (pass3[0] & 0xFFFF);
    matrix[2] = ((pass3[1] & 0xFFFF) << 16) | ((pass2[1] - 4096) & 0xFFFF);
    matrix[3] = ((pass2[2] & 0xFFFF) << 16) | (pass1[2] & 0xFFFF);
    matrix[4] = (uint32)(sint32)(sint16)diagonal;
}

uint32 level2p_sub_8008BCF0(uint32 owner, uint32 model, uint32 bone, uint32 output, uint32 parameter5, uint32 parameter6)
{
    uint32 offset, axis, base, handle, related, data, transform, first, second, third, rotation[5], translation[3];
    uint32 rotated[3], flags[2], rotated_view, flags_view;
    FUNCTION_MARKER(0x8008BCF0, "LEVEL2P.BIN");
    if (!model) return output;
    offset = bone << 3;
    if (!r_u32(model + 0x84))
    {
        for (axis = 0; axis < 3; axis++)
            w_u16(output + axis * 2, r_u16(r_u32(model + 0x2C) + offset + axis * 2));
        if (r_u32(owner + 8) == 1)
        {
            handle = r_u32(r_u32(owner + 0x388) + 0xA0);
            related = handle ? r_u32(0x80099460 + ((handle & 0xFFFF) << 2)) : 0;
            data = r_u32(related + 0x388);
            w_u16(output + 2, r_u16(output + 2) + r_u16(data + 0x94));
        }
        moh_bone_save_controls();
        transform = parameter6 ? parameter6 : owner + 0x340;
        for (axis = 0; axis < 8; axis++) xport_gte_write_control(axis, r_u32(transform + axis * 4));
        /* TODO Addressable SDK outputs need the permitted native local-view adapter */
        rotated_view = moh_draft_local_address(rotated, sizeof(rotated));
        flags_view = moh_draft_local_address(flags, sizeof(flags));
        moh_draft_missing_call(0x80016548, output, rotated_view, flags_view);
        moh_bone_restore_controls();
        w_u16(output, rotated[0]);
        w_u16(output + 2, rotated[1]);
        w_u16(output + 4, rotated[2]);
        return output;
    }
    moh_bone_save_controls();
    if (!parameter5)
    {
        for (axis = 0; axis < 3; axis++)
            w_u16(output + axis * 2, r_u16(r_u32(model + 0x94) + offset + axis * 2));
    }
    else
    {
        base = r_u32(model + 0x88);
        if (!base)
        {
            base = r_u32(model + 0x8C);
            if (!base) base = r_u32(model + 0x90);
        }
        first = r_u32(base + offset);
        second = r_u32(base + offset + 4);
        moh_bone_rotation(first, second, rotation);
        for (axis = 0; axis < 3; axis++)
            translation[axis] = (uint32)(sint32)r_s16(r_u32(model + 0x94) + offset + axis * 2);
        for (axis = 0; axis < 5; axis++) xport_gte_write_control(axis, rotation[axis]);
        for (axis = 0; axis < 3; axis++) xport_gte_write_control(5 + axis, translation[axis]);
        xport_gte_write_data(0, r_u32(parameter5));
        xport_gte_write_data(1, r_u32(parameter5 + 4));
        xport_gte_execute(0x480012);
        first = xport_gte_read_data(25);
        second = xport_gte_read_data(26);
        third = xport_gte_read_data(27);
        w_u16(output, first);
        w_u16(output + 2, second);
        w_u16(output + 4, third);
    }
    transform = parameter6 ? parameter6 : owner + 0x340;
    for (axis = 0; axis < 8; axis++) xport_gte_write_control(axis, r_u32(transform + axis * 4));
    xport_gte_write_data(0, r_u32(output));
    xport_gte_write_data(1, r_u32(output + 4));
    xport_gte_execute(0x480012);
    first = xport_gte_read_data(25);
    second = xport_gte_read_data(26);
    third = xport_gte_read_data(27);
    w_u16(output, first);
    w_u16(output + 2, second);
    w_u16(output + 4, third);
    if (r_u32(owner + 8) == 1)
    {
        handle = r_u32(r_u32(owner + 0x388) + 0xA0);
        related = handle ? r_u32(0x80099460 + ((handle & 0xFFFF) << 2)) : 0;
        data = r_u32(related + 0x388);
        w_u16(output + 2, r_u16(output + 2) + r_u16(data + 0x94));
    }
    moh_bone_restore_controls();
    return output;
}

uint32 level2p_sub_8005BBFC(uint32 position, uint32 surface_out, uint32 data_out)
{
    uint32 index = 0, surface[2], info[2], surface_view, info_view, selected, current, difference, distance, first, second;
    sint32 best = -32767, candidate;
    FUNCTION_MARKER(0x8005BBFC, "LEVEL2P.BIN");
    level2p_sub_8005B490();
    while ((sint32)index < r_s32(0x800AADC8))
    {
        selected = level2p_sub_8005B504((uint32)(sint32)r_s16(position + 2),
            (uint32)(sint32)r_s16(position + 10), index);
        w_u32(0x800AADE8, selected);
        if (selected)
        {
            /* TODO Query outputs need the permitted local adapter; no-hit locals remain unspecified */
            surface_view = moh_draft_local_address(surface, sizeof(surface));
            info_view = moh_draft_local_address(info, sizeof(info));
            candidate = (sint16)level2p_sub_8005BAA8(position, surface_view, info_view);
            selected = best == -32767;
            if (!selected && candidate != -32767)
            {
                current = r_u32(position + 4);
                if ((sint32)current < candidate && (sint32)current < best) selected = candidate < best;
                else
                {
                    difference = (uint32)candidate - current;
                    distance = (uint32)best - current;
                    if ((sint32)distance < 0) distance = 0 - distance;
                    /* Original compares signed candidate delta against absolute previous delta */
                    selected = (sint32)difference < (sint32)distance;
                }
            }
            if (selected)
            {
                if (surface_out) w_u32(surface_out, surface[0]);
                best = candidate;
                if (data_out)
                {
                    first = info[0];
                    second = info[1];
                    w_u32(data_out, first);
                    w_u32(data_out + 4, second);
                }
            }
        }
        index++;
    }
    return (uint32)best;
}


void level2p_sub_8005B490(void)
{
    uint32 index, active, count, entry = 0x8008D0B8;
    sint32 groups;
    FUNCTION_MARKER(0x8005B490, "LEVEL2P.BIN");
    groups = r_s32(0x80094C64);
    w_u32(0x800AADC8, 0);
    if (groups <= 0) return;
    active = r_u32(0x800A94C8);
    for (index = 0; (sint32)index < groups; index++, entry += 40)
    {
        if (index != active)
        {
            count = r_u32(0x800AADC8);
            w_u32(0x800AADD8 + (count << 2), entry);
            w_u32(0x800AADC8, count + 1);
        }
    }
}

uint32 level2p_sub_8005B504(uint32 x, uint32 z, uint32 group)
{
    uint32 entry;
    sint32 minimum, maximum;
    FUNCTION_MARKER(0x8005B504, "LEVEL2P.BIN");
    entry = r_u32(0x800AADD8 + (group << 2));
    minimum = r_s16(entry);
    maximum = r_s16(entry + 4);
    if ((sint32)x < minimum || maximum < (sint32)x) return 0;
    minimum = r_s16(entry + 2);
    maximum = r_s16(entry + 6);
    if ((sint32)z < minimum || maximum < (sint32)z) return 0;
    return entry;
}

uint32 level2p_sub_8005BAA8(uint32 position, uint32 surface_out, uint32 data_out)
{
    sint16 point[4];
    uint32 point_view, result, face, group, index, descriptor, first, second, numerator;
    sint32 x, y, z, denominator;
    FUNCTION_MARKER(0x8005BAA8, "LEVEL2P.BIN");
    point[0] = r_s16(position + 2);
    point[1] = r_s16(position + 6);
    point[2] = r_s16(position + 10);
    w_u32(0x800AADF0, position);
    /* TODO Addressable local vector needs the permitted native adapter */
    point_view = moh_draft_local_address(point, sizeof(point));
    result = level2p_sub_8005B9A4(point_view, surface_out, 0x800AADEC);
    if (!data_out || result == 0xFFFF8001) return result;
    index = r_u32(0x800AADEC);
    group = r_u32(0x800AADE8);
    face = r_u32(group + 0x24) + index * 10;
    descriptor = r_u32(group + 0x20) + ((uint32)(sint32)r_s16(face + 6) << 3);
    first = r_u32(descriptor);
    second = r_u32(descriptor + 4);
    w_u32(data_out, first);
    w_u32(data_out + 4, second);
    y = r_s16(data_out + 2);
    w_u16(data_out + 6, 0);
    if (y < 0)
    {
        z = r_s16(data_out + 4);
        x = r_s16(data_out);
        if (z < 0) z = -z;
        if (x < 0) x = -x;
        denominator = (sint16)(x < z ? z : x);
        if (denominator > 0)
        {
            numerator = (0 - (uint32)y) << 4;
            if ((sint32)numerator / denominator < 28) w_u16(data_out + 6, 1);
        }
    }
    return result;
}


uint32 level2p_sub_8005B9A4(uint32 position, uint32 surface_out, uint32 index_out)
{
    uint32 group, nodes, faces, node, origin_x, origin_z, boundary, list, count;
    sint32 x, z, left, right;
    FUNCTION_MARKER(0x8005B9A4, "LEVEL2P.BIN");
    x = r_s16(position);
    group = r_u32(0x800AADE8);
    z = r_s16(position + 4);
    origin_x = (uint32)(sint32)r_s16(group);
    origin_z = (uint32)(sint32)r_s16(group + 2);
    nodes = r_u32(group + 0x14);
    faces = r_u32(group + 0x24);
    node = nodes;
    for (;;)
    {
        left = r_s16(node);
        if (left < 0)
        {
            w_u32(surface_out, r_u16(node + 6));
            group = r_u32(0x800AADE8);
            right = r_s16(node + 2);
            count = (uint32)(sint32)(sint16)~r_u16(node);
            list = r_u32(group + 0x18) + ((uint32)right << 1);
            return level2p_sub_8005B810(position, faces, list, count, index_out);
        }
        right = r_s16(node + 2);
        if (right >= 0)
        {
            boundary = origin_x + r_u16(node + 4);
            if (x < (sint32)boundary) node = nodes + ((uint32)left << 3);
            else
            {
                origin_x = boundary;
                node = nodes + ((uint32)right << 3);
            }
        }
        else
        {
            boundary = origin_z + r_u16(node + 4);
            if (z < (sint32)boundary) node = nodes + ((uint32)left << 3);
            else
            {
                origin_z = boundary;
                node = nodes + ((~(uint32)right) << 3);
            }
        }
    }
}

uint32 level2p_sub_8005B810(uint32 position, uint32 faces, uint32 indices, uint32 count, uint32 index_out)
{
    uint32 index, face, choose, above, near, previous_above = 0, previous_near = 0;
    sint32 best = -32767, best_index = -1, best_distance = 32767, candidate, y, distance;
    FUNCTION_MARKER(0x8005B810, "LEVEL2P.BIN");
    count = (uint32)(sint32)(sint16)count;
    for (index = 0; (sint32)index < (sint32)count; index++, indices += 2)
    {
        face = faces + (uint32)(sint32)r_s16(indices) * 10;
        if (!level2p_sub_8005B570(position, face)) continue;
        face = faces + (uint32)(sint32)r_s16(indices) * 10;
        candidate = (sint16)level2p_sub_8005B73C(face, position);
        y = r_s16(position + 2);
        distance = candidate - y;
        if (distance < 0) distance = -distance;
        above = y < candidate;
        near = 0;
        if (!above) near = y <= candidate + 32;
        choose = 0;
        if (above || near)
        {
            if (previous_above) choose = above ? candidate < best : 1;
            else choose = previous_near ? distance < best_distance : 1;
        }
        if (choose)
        {
            previous_above = above;
            previous_near = near;
            best_distance = distance;
            best = candidate;
            best_index = r_s16(indices);
        }
    }
    w_u32(index_out, (uint32)best_index);
    return (uint32)best;
}


uint32 level2p_sub_8005B570(uint32 position, uint32 face)
{
    uint32 group, vertices, first, second, third, normal, point_xy, first_xy, second_xy, third_xy;
    sint32 x, z, maximum_x, first_z, second_z, third_z, minimum_z, maximum_z;
    FUNCTION_MARKER(0x8005B570, "LEVEL2P.BIN");
    group = r_u32(0x800AADE8);
    x = r_s16(position);
    vertices = r_u32(group + 0x1C);
    first = vertices + ((uint32)(sint32)r_s16(face) << 3);
    second = vertices + ((uint32)(sint32)r_s16(face + 2) << 3);
    third = vertices + ((uint32)(sint32)r_s16(face + 4) << 3);
    normal = r_u32(group + 0x20) + ((uint32)(sint32)r_s16(face + 6) << 3);
    z = r_s16(position + 4);
    if (r_s16(normal + 2) > 0) return 0;
    if (x < r_s16(first)) return 0;
    maximum_x = r_s16(third);
    if (maximum_x <= r_s16(second)) maximum_x = r_s16(second);
    if (maximum_x < x) return 0;
    second_z = r_s16(second + 4);
    first_z = r_s16(first + 4);
    third_z = r_s16(third + 4);
    minimum_z = first_z < second_z ? first_z : second_z;
    if (third_z < minimum_z) minimum_z = third_z;
    if (z < minimum_z) return 0;
    maximum_z = first_z < second_z ? second_z : first_z;
    if (maximum_z < third_z) maximum_z = third_z;
    if (maximum_z < z) return 0;
    point_xy = ((uint32)x & 0xFFFF) | ((uint32)z << 16);
    first_xy = (uint32)r_u16(first) | ((uint32)r_u16(first + 4) << 16);
    second_xy = (uint32)r_u16(second) | ((uint32)r_u16(second + 4) << 16);
    xport_gte_write_data(12, first_xy);
    xport_gte_write_data(13, second_xy);
    xport_gte_write_data(14, point_xy);
    xport_gte_execute(0x1400006);
    third_xy = (uint32)r_u16(third) | ((uint32)r_u16(third + 4) << 16);
    if ((sint32)xport_gte_read_data(24) > 0) return 0;
    xport_gte_write_data(12, second_xy);
    xport_gte_write_data(13, third_xy);
    xport_gte_write_data(14, point_xy);
    xport_gte_execute(0x1400006);
    if ((sint32)xport_gte_read_data(24) > 0) return 0;
    xport_gte_write_data(12, third_xy);
    xport_gte_write_data(13, first_xy);
    xport_gte_write_data(14, point_xy);
    xport_gte_execute(0x1400006);
    return (sint32)xport_gte_read_data(24) <= 0;
}

uint32 level2p_sub_8005B73C(uint32 face, uint32 position)
{
    uint32 group, normal, vertex, numerator, first_product, second_product;
    sint32 divisor, magnitude;
    FUNCTION_MARKER(0x8005B73C, "LEVEL2P.BIN");
    group = r_u32(0x800AADE8);
    normal = r_u32(group + 0x20) + ((uint32)(sint32)r_s16(face + 6) << 3);
    vertex = r_u32(group + 0x1C) + ((uint32)(sint32)r_s16(face) << 3);
    divisor = r_s16(normal + 2);
    magnitude = divisor < 0 ? -divisor : divisor;
    if (magnitude <= 256) return (uint32)(sint32)r_s16(vertex + 2);
    first_product = (uint32)(sint32)r_s16(normal) * (uint32)(sint32)r_s16(position);
    second_product = (uint32)(sint32)r_s16(normal + 4) * (uint32)(sint32)r_s16(position + 4);
    numerator = 0 - (first_product + second_product + ((uint32)(sint32)r_s16(face + 8) << 15));
    /* Original division guards cannot trigger after the magnitude admission */
    return (uint32)(sint32)(sint16)((sint32)numerator / divisor);
}

uint32 level2p_sub_8006654C(uint32 destination, uint32 id, uint32 kind, uint32 mode)
{
    uint32 index, data;
    FUNCTION_MARKER(0x8006654C, "LEVEL2P.BIN");
    index = level2p_sub_80065BF8(id);
    if ((sint32)index < 0)
    {
        w_u32(destination, 0);
        return 0;
    }
    data = level2p_sub_80065BB8(index);
    return level2p_sub_80065C9C(destination, data, kind, mode);
}


void level2p_sub_8008BCCC(void)
{
    FUNCTION_MARKER(0x8008BCCC, "LEVEL2P.BIN");
    /* Original body is JR RA followed by NOP */
}

void level2p_sub_8008BCC4(void)
{
    FUNCTION_MARKER(0x8008BCC4, "LEVEL2P.BIN");
    /* Original body is JR RA followed by NOP */
}

void level2p_sub_800855FC(uint32 owner, uint32 model)
{
    FUNCTION_MARKER(0x800855FC, "LEVEL2P.BIN");
    level2p_sub_80084610(owner, model);
}

void level2p_sub_8008561C(uint32 owner)
{
    uint32 previous, current, model;
    FUNCTION_MARKER(0x8008561C, "LEVEL2P.BIN");
    previous = r_u32(owner + 0x3A0);
    current = r_u32(0x800968E0);
    if (previous != current)
    {
        model = r_u32(owner + 0x100);
        w_u32(owner + 0x3A0, current);
        level2p_sub_80084610(owner, model);
    }
}

void level2p_sub_800807DC(uint32 owner, uint32 data)
{
    FUNCTION_MARKER(0x800807DC, "LEVEL2P.BIN");
    if (r_u32(data + 0x14) == 0xFFFFFFFF) level2p_sub_80066C64(owner);
    else
    {
        w_u32(owner + 0x36C, 0x800805C4);
        w_u32(owner + 0x370, 0);
        w_u32(data + 0xC, r_u32(data + 0x14));
        w_u32(owner + 0x14, 0);
    }
}

void level2p_sub_800804C8(uint32 owner)
{
    uint32 data, count;
    FUNCTION_MARKER(0x800804C8, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388);
    count = r_u32(data + 0xC);
    if (count != 0xFFFFFFFF)
    {
        count--;
        w_u32(data + 0xC, count);
        if (!count) level2p_sub_800807DC(owner, data);
    }
}

void level2p_sub_80080508(uint32 owner)
{
    uint32 value;
    FUNCTION_MARKER(0x80080508, "LEVEL2P.BIN");
    if (!(r_u32(0x800968E4) & 31))
    {
        value = level2p_sub_8003F140(owner + 0x20C, 0, 1, 0, 0, 1);
        if ((sint16)value == -32767) value = level2p_sub_8005BDE4(owner, 0);
        if ((sint16)value != -32767)
        {
            w_u32(owner + 0x210, (value << 16) - 0x60000);
            w_u32(owner + 0x36C, 0x800804C8);
            level2p_sub_8006591C(owner);
            level2p_sub_8004DFBC(owner);
        }
    }
    level2p_sub_800804C8(owner);
}

void level2p_sub_80065698(uint32 owner, uint32 draw)
{
    uint32 animate = 1, visible = 1, data, model, handle, related, mode;
    FUNCTION_MARKER(0x80065698, "LEVEL2P.BIN");
    if (owner != r_u32(0x8009942C) && (r_u32(owner + 0xC) & 1)) visible = level2p_sub_8008CC6C(owner);
    else if ((r_u32(owner + 0xC) & 0x3000000) && !level2p_sub_8008CC6C(owner))
    {
        if ((r_u32(owner + 0xC) & 0x2000000) && r_u32(owner + 0x298)) animate = 0;
        if (r_u32(owner + 0xC) & 0x1000000) visible = 0;
    }
    if (animate)
    {
        model = r_u32(owner + 0x100);
        if (!r_u32(model + 0x74)) level2p_sub_8008BCCC();
        else level2p_sub_8008561C(owner);
        if (owner != r_u32(0x8009942C) && (r_u32(owner + 0xC) & 1))
        {
            data = r_u32(owner + 0x388);
            if (!r_u32(data + 0x118)) level2p_sub_8008BCC4();
            else
            {
                model = data + 0xA4;
                level2p_sub_800855FC(owner, model);
                level2p_sub_8007CED8(owner, model);
            }
        }
    }
    else level2p_sub_8004DFBC(owner);
    if (!visible)
    {
        level2p_sub_80065630(owner);
        return;
    }
    data = r_u32(r_u32(0x80099428) + 0x388);
    handle = r_u32(data + 0xA0);
    related = handle ? r_u32(0x80099460 + ((handle & 0xFFFF) << 2)) : 0;
    if (related == owner) return;
    if (!(r_u32(owner + 0xC) & 1))
    {
        mode = (r_u32(owner + 0x10) & 0x80) ? 0xFFFFFFC0 : 128;
        level2p_sub_800684E4(owner, draw, mode);
    }
    else if (owner == r_u32(0x8009942C))
    {
        moh_draft_missing_call(0x8001BA68, 0x85);
        level2p_sub_800684E4(owner, draw, 192);
        moh_draft_missing_call(0x8001BA68, r_u32(0x80096974));
    }
    else level2p_sub_800684E4(owner, draw, 192);
}


void level2p_sub_80065630(uint32 owner)
{
    uint32 count;
    FUNCTION_MARKER(0x80065630, "LEVEL2P.BIN");
    if (!level2p_sub_8006547C(owner, r_u32(0x80099418)))
    {
        if (!level2p_sub_8006547C(owner, r_u32(0x8009941C)))
        {
            count = r_u32(owner + 0x290);
            w_u32(owner + 0x29C, 0);
            w_u32(owner + 0x290, count - 1);
        }
    }
}

uint32 level2p_sub_8008CC6C(uint32 owner)
{
    sint16 minimum[3], maximum[3];
    uint32 minimum_view, maximum_view;
    FUNCTION_MARKER(0x8008CC6C, "LEVEL2P.BIN");
    minimum[0] = (sint16)(r_u16(owner + 0x2E0) - r_u16(owner + 0x2E8));
    minimum[1] = (sint16)(r_u16(owner + 0x2E2) - r_u16(owner + 0x2E8));
    minimum[2] = (sint16)(r_u16(owner + 0x2E4) - r_u16(owner + 0x2E8));
    maximum[0] = (sint16)(r_u16(owner + 0x2E0) + r_u16(owner + 0x2E8));
    maximum[1] = (sint16)(r_u16(owner + 0x2E2) + r_u16(owner + 0x2E8));
    maximum[2] = (sint16)(r_u16(owner + 0x2E4) + r_u16(owner + 0x2E8));
    /* TODO: Addressable local bounds require the permitted native adapter */
    minimum_view = moh_draft_local_address(minimum, sizeof(minimum));
    maximum_view = moh_draft_local_address(maximum, sizeof(maximum));
    return level2p_sub_8008C840(minimum_view, maximum_view) != 0;
}

static void moh_screen_corner(uint32 minimum, uint32 maximum, uint32 plane, uint32 *xy, uint32 *z)
{
    uint32 x_sign, y_sign, z_sign, x, y;
    y_sign = r_u32(plane + 4);
    x_sign = r_u32(plane);
    z_sign = r_u32(plane + 8);
    x = r_u16((sint32)x_sign < 0 ? minimum : maximum);
    y = r_u16(((sint32)y_sign < 0 ? minimum : maximum) + 2);
    *xy = x | (y << 16);
    *z = r_u16(((sint32)z_sign < 0 ? minimum : maximum) + 4);
}

uint32 level2p_sub_8008C840(uint32 minimum, uint32 maximum)
{
    uint32 view, xy, z, index;
    FUNCTION_MARKER(0x8008C840, "LEVEL2P.BIN");
    view = r_u32(r_u32(r_u32(0x80099428) + 0x388) + 0xC8);
    for (index = 0; index < 8; index++) xport_gte_write_control(index, r_u32(0x80098B50 + index * 4));
    moh_screen_corner(minimum, maximum, view + 0x50, &xy, &z);
    xport_gte_write_data(0, xy);
    xport_gte_write_data(1, z);
    xport_gte_execute(0x180001);
    moh_screen_corner(minimum, maximum, view + 0x6C, &xy, &z);
    if (xport_gte_read_flag() & 0x40000) return 0;
    xport_gte_write_data(0, xy);
    xport_gte_write_data(1, z);
    xport_gte_execute(0x180001);
    moh_screen_corner(minimum, maximum, view + 0x88, &xy, &z);
    if ((sint16)xport_gte_read_data(14) < 0) return 0;
    xport_gte_write_data(0, xy);
    xport_gte_write_data(1, z);
    xport_gte_execute(0x180001);
    moh_screen_corner(minimum, maximum, view + 0xA4, &xy, &z);
    if ((sint32)(sint16)xport_gte_read_data(14) > r_s32(0x80098B74)) return 0;
    xport_gte_write_data(0, xy);
    xport_gte_write_data(1, z);
    xport_gte_execute(0x180001);
    moh_screen_corner(minimum, maximum, view + 0xC0, &xy, &z);
    if ((sint16)(xport_gte_read_data(14) >> 16) < 0) return 0;
    xport_gte_write_data(0, xy);
    xport_gte_write_data(1, z);
    xport_gte_execute(0x180001);
    moh_screen_corner(minimum, maximum, view + 0xDC, &xy, &z);
    if ((sint32)(sint16)(xport_gte_read_data(14) >> 16) > r_s32(0x80098B78)) return 0;
    xport_gte_write_data(0, xy);
    xport_gte_write_data(1, z);
    xport_gte_execute(0x180001);
    return (sint32)(xport_gte_read_data(19) << 16) <= r_s32(view + 0x40);
}

void level2p_sub_8007CED8(uint32 owner, uint32 model)
{
    uint32 data, output, current, frames, scaled, pose, extra, state;
    sint32 frame;
    FUNCTION_MARKER(0x8007CED8, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388);
    if (r_u32(data) & 0x1000) return;
    current = r_u32(model + 0x88);
    output = data + 0x348;
    if (current != output) w_u32(data + 0x340, current);
    frames = r_u32(model + 0x78);
    scaled = r_u32(owner + 0x228);
    frames = r_u8(r_u32(frames + 0x60));
    scaled = (uint32)((sint32)(scaled + 0x2000000) >> 8);
    frame = ((sint32)(frames * scaled) >> 18) + 1;
    if (frame < 0) frame = 0;
    if (frame >= (sint32)frames) frame = (sint32)frames - 1;
    pose = level2p_sub_800838F4(model, 0x18, (uint32)frame, output);
    moh_draft_missing_call(0x800154EC, output, r_u32(data + 0x340), 0x48);
    state = r_u8(data + 0x55);
    if (state == 3 || state == 10)
    {
        if (r_s16(data + 0x664) < r_s16(data + 0x666))
            w_u16(data + 0x664, (uint16)(r_u16(data + 0x664) + 2));
        extra = level2p_sub_800838F4(model, 0x17, (uint32)(sint32)r_s16(data + 0x664), data + 0x3F8);
        level2p_sub_80083774(r_u32(data + 0x340) + 0x48, pose + 0x48, data + 0x390, 5);
        moh_draft_missing_call(0x800154EC, data + 0x3B8, extra + 0x70, 0x40);
    }
    else level2p_sub_80083774(r_u32(data + 0x340) + 0x48, pose + 0x48, data + 0x390, 5);
    w_u32(model + 0x88, data + 0x348);
}

void level2p_sub_80083774(uint32 first, uint32 second, uint32 output, uint32 count)
{
    FUNCTION_MARKER(0x80083774, "LEVEL2P.BIN");
    level2p_sub_80085658(first, second, output, count & 0xFF);
}


static uint32 moh_quaternion_divide(uint32 numerator, uint32 divisor)
{
    if (!divisor) moh_draft_fail_fast("Original quaternion division BREAK 0x1C00", 0x80085658);
    if (divisor == 0xFFFFFFFF && numerator == 0x80000000)
        moh_draft_fail_fast("Original quaternion division BREAK 0x1800", 0x80085658);
    return (uint32)((sint32)numerator / (sint32)divisor);
}

void level2p_sub_80085658(uint32 first, uint32 second, uint32 output, uint32 count)
{
    uint32 end, index, dot, negative, magnitude, divisor, angle, quarter, first_weight, second_weight, value;
    sint32 left[4], right[4], cosine;
    FUNCTION_MARKER(0x80085658, "LEVEL2P.BIN");
    end = output + ((count & 0xFF) << 3);
    while (output != end)
    {
        dot = 0;
        for (index = 0; index < 4; index++)
        {
            left[index] = r_s16(first + index * 2);
            right[index] = r_s16(second + index * 2);
            dot += (uint32)left[index] * (uint32)right[index];
        }
        cosine = (sint32)(dot + 0x800) >> 12;
        negative = (uint32)cosine >> 31;
        if (cosine >= (sint32)(0x1000 - r_u32(0x8009B4F0))
            && cosine <= (sint32)(r_u32(0x8009B4F0) + 0x1000))
        {
            for (index = 0; index < 4; index++) w_u16(output + index * 2, (uint16)right[index]);
        }
        else
        {
            magnitude = negative ? 0 - (uint32)cosine : (uint32)cosine;
            if ((sint32)(0x1000 - magnitude) < r_s32(0x8009B4EC))
            {
                for (index = 0; index < 4; index++)
                {
                    if (negative)
                        value = (uint32)(((left[index] + right[index] + 2) >> 2) - right[index]);
                    else value = (uint32)(right[index] + ((left[index] - right[index] + 2) >> 2));
                    w_u16(output + index * 2, (uint16)value);
                }
            }
            else
            {
                value = (uint32)((sint32)(0x1000800 - magnitude * magnitude) >> 12);
                divisor = moh_draft_missing_call(0x8001BC68, value);
                angle = moh_draft_missing_call(0x8001536C, divisor, magnitude);
                quarter = (uint32)((sint32)(angle + 2) >> 2);
                value = moh_draft_missing_call(0x8001C9A8, angle - quarter) << 17;
                second_weight = moh_quaternion_divide(value, divisor);
                value = moh_draft_missing_call(0x8001C9A8, quarter) << 17;
                first_weight = moh_quaternion_divide(value, divisor);
                if (negative) first_weight = 0 - first_weight;
                for (index = 0; index < 4; index++)
                {
                    value = first_weight * (uint32)left[index] + second_weight * (uint32)right[index] + 0x10000;
                    w_u16(output + index * 2, (uint16)((sint32)value >> 17));
                }
            }
        }
        first += 8;
        second += 8;
        output += 8;
    }
}


uint32 level2p_sub_80083794(uint32 source, uint32 output, uint32 count)
{
    sint32 pairs;
    uint32 first, second, third, packed;
    FUNCTION_MARKER(0x80083794, "LEVEL2P.BIN");
    pairs = (sint32)count >> 1;
    while (pairs > 0)
    {
        count -= 2;
        second = r_u32(source + 4);
        third = r_u32(source + 8);
        first = r_u32(source);
        source += 12;
        pairs--;
        w_u16(output + 6, (uint16)((uint32)((sint32)first >> 20) << 1));
        w_u16(output, (uint16)((uint32)((sint32)(first << 16) >> 20) << 1));
        w_u16(output + 2, (uint16)((sint32)(second << 20) >> 19));
        packed = (uint32)((sint32)((uint32)((sint32)second >> 12) << 28) >> 20)
            | ((first >> 12) & 0xF0) | (first & 15);
        w_u16(output + 4, (uint16)(packed << 1));
        w_u16(output + 14, (uint16)((uint32)((sint32)(third << 16) >> 20) << 1));
        w_u16(output + 8, (uint16)((uint32)((sint32)second >> 20) << 1));
        w_u16(output + 10, (uint16)((uint32)((sint32)(third << 4) >> 20) << 1));
        packed = ((uint32)((sint32)third >> 28) << 8) | ((third & 15) << 4) | ((second >> 16) & 15);
        w_u16(output + 12, (uint16)(packed << 1));
        output += 16;
    }
    if (count & 1)
    {
        first = r_u32(source);
        second = r_u32(source + 4);
        w_u16(output + 6, (uint16)((uint32)((sint32)first >> 20) << 1));
        w_u16(output, (uint16)((uint32)((sint32)(first << 16) >> 20) << 1));
        w_u16(output + 2, (uint16)((sint32)(second << 20) >> 19));
        packed = (uint32)((sint32)((uint32)((sint32)second >> 12) << 28) >> 20)
            | ((first >> 12) & 0xF0) | (first & 15);
        w_u16(output + 4, (uint16)(packed << 1));
        output += 8;
    }
    return output;
}

static uint32 moh_frame_divide(uint32 numerator, uint32 divisor, uint32 origin)
{
    if (!divisor) moh_draft_fail_fast("Original frame division BREAK 0x1C00", origin);
    if (divisor == 0xFFFFFFFF && numerator == 0x80000000)
        moh_draft_fail_fast("Original frame division BREAK 0x1800", origin);
    return (uint32)((sint32)numerator / (sint32)divisor);
}

static void moh_frame_blend(uint32 first, uint32 second, uint32 output, uint32 count, uint32 mode, uint32 origin)
{
    uint32 end = output + count * 8, index, dot, negative, magnitude, width;
    uint32 angle, fraction, divisor, first_weight, second_weight, value;
    sint32 left[4], right[4], cosine;
    while (output != end)
    {
        dot = 0;
        for (index = 0; index < 4; index++)
        {
            left[index] = r_s16(first + index * 2);
            right[index] = r_s16(second + index * 2);
            dot += (uint32)left[index] * (uint32)right[index];
        }
        cosine = (sint32)(dot + 0x800) >> 12;
        negative = (uint32)cosine >> 31;
        width = r_u32(0x8009B4F0);
        if (cosine >= (sint32)(0x1000 - width) && cosine <= (sint32)(width + 0x1000))
        {
            for (index = 0; index < 4; index++)
                w_u16(output + index * 2, (uint16)(mode == 1 ? left[index] : right[index]));
        }
        else
        {
            magnitude = negative ? 0 - (uint32)cosine : (uint32)cosine;
            if ((sint32)(0x1000 - magnitude) < r_s32(0x8009B4EC))
            {
                for (index = 0; index < 4; index++)
                {
                    if (mode == 1)
                        value = (uint32)(left[index] + (((negative ? -left[index] - right[index] : right[index] - left[index]) + 2) >> 2));
                    else if (mode == 2)
                        value = (uint32)((left[index] + (negative ? -right[index] : right[index]) + 1) >> 1);
                    else if (negative)
                        value = (uint32)(((left[index] + right[index] + 2) >> 2) - right[index]);
                    else value = (uint32)(right[index] + ((left[index] - right[index] + 2) >> 2));
                    w_u16(output + index * 2, (uint16)value);
                }
            }
            else
            {
                value = (uint32)((sint32)(0x1000800 - magnitude * magnitude) >> 12);
                divisor = moh_draft_missing_call(0x8001BC68, value);
                angle = moh_draft_missing_call(0x8001536C, divisor, magnitude);
                fraction = mode == 2 ? (uint32)((sint32)(angle + 1) >> 1) : (uint32)((sint32)(angle + 2) >> 2);
                value = moh_draft_missing_call(0x8001C9A8, angle - fraction) << 17;
                first_weight = moh_frame_divide(value, divisor, origin);
                value = moh_draft_missing_call(0x8001C9A8, fraction) << 17;
                second_weight = moh_frame_divide(value, divisor, origin);
                if (negative) second_weight = 0 - second_weight;
                if (mode == 3)
                {
                    value = first_weight;
                    first_weight = second_weight;
                    second_weight = value;
                }
                for (index = 0; index < 4; index++)
                {
                    value = first_weight * (uint32)left[index] + second_weight * (uint32)right[index] + 0x10000;
                    w_u16(output + index * 2, (uint16)((sint32)value >> 17));
                }
            }
        }
        first += 8;
        second += 8;
        output += 8;
    }
}

uint32 level2p_sub_800838F4(uint32 model, uint32 animation, uint32 frame, uint32 output)
{
    uint32 data, record, packed, count, first, last, distance, mode, frames, second, last_record;
    FUNCTION_MARKER(0x800838F4, "LEVEL2P.BIN");
    data = r_u32(r_u32(model + 0x78) + animation * 4);
    record = r_u32(data + 4) + frame * 20;
    packed = r_u8(record + 14);
    count = r_u8(record + 15);
    last = frame + (packed >> 4);
    first = frame - (packed & 15);
    if (r_u32(record + 16)) mode = 0;
    else
    {
        distance = last - first;
        if ((sint32)distance < 0 ? last == first - 2 : distance == 2) mode = 2;
        else
        {
            mode = frame - first;
            if ((sint32)mode < 0) mode = first - frame;
        }
    }
    if (!mode) level2p_sub_80083794(r_u32(record + 16), output, count);
    else
    {
        frames = r_u32(data + 4);
        last_record = frames + last * 20;
        second = level2p_sub_80083794(r_u32(frames + first * 20 + 16), 0x1F800000, count);
        level2p_sub_80083794(r_u32(last_record + 16), second, count);
        if (mode == 1 || mode == 2 || mode == 3) moh_frame_blend(0x1F800000, second, output, count, mode, 0x800838F4);
    }
    return output;
}

void level2p_sub_800684E4(uint32 owner, uint32 draw, uint32 mode)
{
    uint32 model, result, flags, count;
    FUNCTION_MARKER(0x800684E4, "LEVEL2P.BIN");
    if (owner == r_u32(0x8009942C)) level2p_sub_8008B860(owner);
    else level2p_sub_8008BA7C(owner);
    if (owner == r_u32(0x8009942C) || !(r_u32(owner + 0xC) & 1)) model = r_u32(owner + 0x100);
    else model = r_u32(owner + 0x388) + 0xA4;
    result = level2p_sub_8007FB10(model, r_u32(draw + 0x2218), r_u32(draw + 0x221C), mode);
    flags = r_u32(owner + 0xC);
    w_u32(owner + 0x29C, result);
    if (flags & 0x8000)
    {
        if (result)
        {
            count = r_u32(owner + 0x294);
            w_u32(owner + 0x298, 1);
            w_u32(owner + 0x290, count);
        }
        else if (r_u32(owner + 0x298)) level2p_sub_80065630(owner);
    }
}

uint32 level2p_sub_8003F140(uint32 position, uint32 parameter2, uint32 parameter3, uint32 parameter4, uint32 parameter5, uint32 parameter6)
{
    uint32 index = 0, group, surface[2], info[2], surface_view, info_view;
    sint32 best = -32767, candidate;
    FUNCTION_MARKER(0x8003F140, "LEVEL2P.BIN");
    level2p_sub_8003EF14();
    while ((sint32)index < r_s32(0x800AAC3C))
    {
        group = level2p_sub_8003EF88((uint32)(sint32)r_s16(position + 2), (uint32)(sint32)r_s16(position + 10), index);
        w_u32(0x800AAC38, group);
        if (group)
        {
            /* TODO: Addressable output locals require the permitted native adapter */
            surface_view = moh_draft_local_address(surface, sizeof(surface));
            info_view = moh_draft_local_address(info, sizeof(info));
            candidate = (sint16)level2p_sub_8003EC18(position, surface_view, parameter3, info_view, parameter5, parameter6);
            if (best < candidate)
            {
                if (parameter2) w_u32(parameter2, surface[0]);
                best = candidate;
                if (parameter4)
                {
                    w_u32(parameter4, info[0]);
                    w_u32(parameter4 + 4, info[1]);
                }
            }
        }
        index++;
    }
    return (uint32)best;
}

void level2p_sub_8008BA7C(uint32 owner)
{
    uint32 matrix[8], translated[3], matrix_view, translated_view, index;
    FUNCTION_MARKER(0x8008BA7C, "LEVEL2P.BIN");
    /* TODO: SDK matrix locals require the permitted addressable-local adapter */
    matrix_view = moh_draft_local_address(matrix, sizeof(matrix));
    translated_view = moh_draft_local_address(translated, sizeof(translated));
    moh_draft_missing_call(0x800154EC, matrix_view, owner + 0x340, 32);
    moh_draft_missing_call(0x800163E8, 0x80098B50, matrix_view + 20, translated_view);
    moh_draft_missing_call(0x8001BD08, 0x80098B50, matrix_view);
    matrix[5] = translated[0] + r_u32(0x80098B64);
    matrix[6] = translated[1] + r_u32(0x80098B68);
    matrix[7] = translated[2] + r_u32(0x80098B6C);
    for (index = 0; index < 8; index++) xport_gte_write_control(index, matrix[index]);
}


void level2p_sub_80084610(uint32 owner, uint32 model)
{
    sint32 frame, animation, previous_frame, transition, override_frame, mode, base;
    sint16 target[3];
    uint32 count, pose, record, packed, first, output, index, value;
    FUNCTION_MARKER(0x80084610, "LEVEL2P.BIN");
    frame = r_s16(model + 0x5E);
    animation = r_s16(model + 0x5C);
    previous_frame = r_s16(model + 0x60);
    transition = r_s16(model + 0x3E);
    if (animation < 0)
    {
        w_u16(model + 0x62, 0xFFFF);
        w_u16(model + 0x64, 0xFFFF);
        return;
    }
    if (!transition && animation == r_s16(model + 0x62) && frame == r_s16(model + 0x64)) return;
    if (transition > 0)
    {
        if (r_s16(model + 0x62) != -1)
        {
            record = r_u32(r_u32(r_u32(model + 0x78) + (uint32)animation * 4) + 4) + (uint32)frame * 20;
            count = r_u8(record + 15);
            if (!r_u32(model + 0x88))
            {
                pose = level2p_sub_800838F4(model, (uint32)animation, (uint32)frame, r_u32(model + 0x90));
                w_u32(model + 0x88, pose);
                return;
            }
            frame = previous_frame;
            override_frame = r_s16(model + 0x40);
            if (override_frame >= 0) frame = override_frame;
            w_u32(model + 0x34, (uint32)animation);
            w_u32(model + 0x38, (uint32)frame);
            pose = level2p_sub_800838F4(model, (uint32)animation, (uint32)frame, 0x8009B500);
            record = r_u32(r_u32(r_u32(model + 0x78) + (uint32)animation * 4) + 4) + (uint32)frame * 20;
            packed = r_u32(record + 4);
            target[0] = (sint16)((sint32)(packed << 22) >> 22);
            record = r_u32(r_u32(r_u32(model + 0x78) + (uint32)animation * 4) + 4) + (uint32)frame * 20;
            packed = r_u32(record + 4);
            target[1] = (sint16)((sint32)(packed << 11) >> 21);
            record = r_u32(r_u32(r_u32(model + 0x78) + (uint32)animation * 4) + 4) + (uint32)frame * 20;
            packed = r_u32(record + 4);
            target[2] = (sint16)((sint32)(packed << 1) >> 22);
            mode = 4 - transition;
            if (mode == 1 || mode == 2 || mode == 3)
            {
                first = r_u32(model + 0x8C);
                output = r_u32(model + 0x90);
                moh_frame_blend(first, pose, output, count, (uint32)mode, 0x80084610);
                w_u32(model + 0x88, r_u32(model + 0x90));
                for (index = 0; index < 3; index++)
                {
                    base = r_s16(model + 0xA0 + index * 2);
                    if (mode == 3) value = (uint32)(target[index] - ((target[index] - base) >> 2));
                    else
                    {
                        value = r_u16(model + 0xA0 + index * 2);
                        value += (uint32)((target[index] - base) >> (mode == 2 ? 1 : 2));
                    }
                    w_u16(model + 0x98 + index * 2, (uint16)value);
                }
            }
            else if (!mode)
            {
                pose = r_u32(model + 0x8C);
                value = r_u16(model + 0xA0);
                w_u32(model + 0x88, pose);
                w_u16(model + 0x98, (uint16)value);
                w_u16(model + 0x9A, r_u16(model + 0xA2));
                w_u16(model + 0x9C, r_u16(model + 0xA4));
            }
            goto save_frame;
        }
        w_u16(model + 0x3E, 0);
    }
    pose = level2p_sub_800838F4(model, (uint32)animation, (uint32)frame, r_u32(model + 0x90));
    w_u32(model + 0x88, pose);
save_frame:
    w_u16(model + 0x62, (uint16)animation);
    w_u16(model + 0x64, (uint16)frame);
}


void level2p_sub_8003EF14(void)
{
    sint32 count;
    uint32 index = 0, active, destination;
    FUNCTION_MARKER(0x8003EF14, "LEVEL2P.BIN");
    count = r_s32(0x80094C64);
    w_u32(0x800AAC3C, 0);
    if (count <= 0) return;
    active = r_u32(0x800A94C8);
    while ((sint32)index < count)
    {
        if (index != active)
        {
            destination = r_u32(0x800AAC3C);
            w_u32(0x800AAC48 + destination * 4, 0x8008D0B8 + index * 40);
            w_u32(0x800AAC3C, destination + 1);
        }
        index++;
    }
}

uint32 level2p_sub_8003EF88(uint32 x, uint32 z, uint32 index)
{
    uint32 group;
    sint32 minimum, maximum;
    FUNCTION_MARKER(0x8003EF88, "LEVEL2P.BIN");
    group = r_u32(0x800AAC48 + index * 4);
    minimum = r_s16(group);
    maximum = r_s16(group + 4);
    if ((sint32)x < minimum || maximum < (sint32)x) return 0;
    minimum = r_s16(group + 2);
    maximum = r_s16(group + 6);
    if ((sint32)z < minimum || maximum < (sint32)z) return 0;
    return group;
}

uint32 level2p_sub_8003EC18(uint32 position, uint32 surface, uint32 mode, uint32 info, uint32 parameter5, uint32 parameter6)
{
    uint32 flags, result, group, face, normal, first, second;
    sint32 y, x, z, divisor;
    FUNCTION_MARKER(0x8003EC18, "LEVEL2P.BIN");
    w_u32(0x8008D168, position);
    w_u16(0x8008D15C, (uint16)r_s16(position + 2));
    w_u16(0x8008D15E, (uint16)r_s16(position + 6));
    w_u16(0x8008D160, (uint16)r_s16(position + 10));
    flags = parameter5 != 0;
    if (parameter6) flags |= 2;
    /* Original mode only exchanges SP through the reviewed8001535C helper */
    result = level2p_sub_8003DEE0(0x8008D15C, surface, 0x8008D164, flags);
    if (info && result != 0xFFFF8001)
    {
        face = r_u32(0x8008D164);
        group = r_u32(0x800AAC38);
        face = r_u32(group + 0x24) + face * 10;
        normal = r_u32(group + 0x20) + ((uint32)(sint32)r_s16(face + 6) << 3);
        first = r_u32(normal);
        second = r_u32(normal + 4);
        w_u32(info, first);
        w_u32(info + 4, second);
        y = r_s16(info + 2);
        w_u16(info + 6, 0);
        if (y < 0)
        {
            z = r_s16(info + 4);
            x = r_s16(info);
            if (z < 0) z = -z;
            if (x < 0) x = -x;
            divisor = (sint16)(z <= x ? x : z);
            /* Positive truncated divisor excludes the original division BREAK cases */
            if (divisor > 0 && (-y * 16) / divisor < 28) w_u16(info + 6, 1);
        }
    }
    return result;
}


uint32 level2p_sub_8003DEE0(uint32 position, uint32 surface, uint32 index_output, uint32 flags)
{
    sint16 local[3];
    uint32 local_view;
    FUNCTION_MARKER(0x8003DEE0, "LEVEL2P.BIN");
    local[0] = (sint16)r_u16(position);
    local[1] = (sint16)r_u16(position + 2);
    local[2] = (sint16)r_u16(position + 4);
    /* TODO: Addressable position copy requires the permitted native adapter */
    local_view = moh_draft_local_address(local, sizeof(local));
    return level2p_sub_8003DDCC(local_view, surface, index_output, flags);
}

uint32 level2p_sub_8003DDCC(uint32 position, uint32 surface, uint32 index_output, uint32 flags)
{
    uint32 group, nodes, faces, node, list, split;
    sint32 x, z, origin_x, origin_z, left, right, boundary, count;
    FUNCTION_MARKER(0x8003DDCC, "LEVEL2P.BIN");
    x = r_s16(position);
    group = r_u32(0x800AAC38);
    z = r_s16(position + 4);
    origin_x = r_s16(group);
    origin_z = r_s16(group + 2);
    nodes = r_u32(group + 0x14);
    faces = r_u32(group + 0x24);
    node = nodes;
    for (;;)
    {
        left = r_s16(node);
        if (left < 0)
        {
            w_u32(surface, r_u16(node + 6));
            group = r_u32(0x800AAC38);
            list = (uint32)(sint32)r_s16(node + 2) << 1;
            count = (sint16)~r_u16(node);
            list += r_u32(group + 0x18);
            return level2p_sub_8003DC48(position, faces, list, (uint32)count, index_output, flags & 1, flags & 2);
        }
        right = r_s16(node + 2);
        split = r_u16(node + 4);
        if (right >= 0)
        {
            boundary = (sint32)((uint32)origin_x + split);
            if (x < boundary) node = nodes + ((uint32)left << 3);
            else
            {
                origin_x = boundary;
                node = nodes + ((uint32)right << 3);
            }
        }
        else
        {
            boundary = (sint32)((uint32)origin_z + split);
            if (z < boundary) node = nodes + ((uint32)left << 3);
            else
            {
                origin_z = boundary;
                node = nodes + (~(uint32)right << 3);
            }
        }
    }
}


uint32 level2p_sub_8003DC48(uint32 position, uint32 faces, uint32 indices, uint32 count, uint32 index_output, uint32 flag1, uint32 flag2)
{
    uint32 iteration, face, group, normal;
    sint32 best = -32767, distance = 32767, selected = -1, candidate, delta, y;
    FUNCTION_MARKER(0x8003DC48, "LEVEL2P.BIN");
    for (iteration = 0; (sint32)iteration < (sint16)count; iteration++, indices += 2)
    {
        if (flag1)
        {
            face = faces + (uint32)(sint32)r_s16(indices) * 10;
            group = r_u32(0x800AAC38);
            normal = r_u32(group + 0x20) + ((uint32)(sint32)r_s16(face + 6) << 3);
            if (r_s16(normal + 2) >= -23168) continue;
        }
        face = faces + (uint32)(sint32)r_s16(indices) * 10;
        if (!level2p_sub_8003D9A8(position, face)) continue;
        face = faces + (uint32)(sint32)r_s16(indices) * 10;
        candidate = (sint16)level2p_sub_8003DB74(face, position);
        y = r_s16(position + 2);
        delta = candidate + 16 - y;
        if (delta < 0) delta = -delta;
        if (delta < distance && (!flag2 || y <= candidate))
        {
            distance = delta;
            best = candidate;
            selected = r_s16(indices);
        }
    }
    w_u32(index_output, (uint32)selected);
    return (uint32)best;
}

uint32 level2p_sub_8003D9A8(uint32 position, uint32 face)
{
    uint32 group, vertices, first, second, third, normal, point_xy, first_xy, second_xy, third_xy;
    sint32 x, z, maximum_x, first_z, second_z, third_z, minimum_z, maximum_z;
    FUNCTION_MARKER(0x8003D9A8, "LEVEL2P.BIN");
    group = r_u32(0x800AAC38);
    x = r_s16(position);
    vertices = r_u32(group + 0x1C);
    first = vertices + ((uint32)(sint32)r_s16(face) << 3);
    second = vertices + ((uint32)(sint32)r_s16(face + 2) << 3);
    third = vertices + ((uint32)(sint32)r_s16(face + 4) << 3);
    normal = r_u32(group + 0x20) + ((uint32)(sint32)r_s16(face + 6) << 3);
    z = r_s16(position + 4);
    if (r_s16(normal + 2) > 0) return 0;
    if (x < r_s16(first)) return 0;
    maximum_x = r_s16(third);
    if (maximum_x <= r_s16(second)) maximum_x = r_s16(second);
    if (maximum_x < x) return 0;
    second_z = r_s16(second + 4);
    first_z = r_s16(first + 4);
    third_z = r_s16(third + 4);
    minimum_z = first_z < second_z ? first_z : second_z;
    if (third_z < minimum_z) minimum_z = third_z;
    if (z < minimum_z) return 0;
    maximum_z = first_z < second_z ? second_z : first_z;
    if (maximum_z < third_z) maximum_z = third_z;
    if (maximum_z < z) return 0;
    point_xy = ((uint32)x & 0xFFFF) | ((uint32)z << 16);
    first_xy = (uint32)r_u16(first) | ((uint32)r_u16(first + 4) << 16);
    second_xy = (uint32)r_u16(second) | ((uint32)r_u16(second + 4) << 16);
    xport_gte_write_data(12, first_xy);
    xport_gte_write_data(13, second_xy);
    xport_gte_write_data(14, point_xy);
    xport_gte_execute(0x1400006);
    third_xy = (uint32)r_u16(third) | ((uint32)r_u16(third + 4) << 16);
    if ((sint32)xport_gte_read_data(24) > 0) return 0;
    xport_gte_write_data(12, second_xy);
    xport_gte_write_data(13, third_xy);
    xport_gte_write_data(14, point_xy);
    xport_gte_execute(0x1400006);
    if ((sint32)xport_gte_read_data(24) > 0) return 0;
    xport_gte_write_data(12, third_xy);
    xport_gte_write_data(13, first_xy);
    xport_gte_write_data(14, point_xy);
    xport_gte_execute(0x1400006);
    return (sint32)xport_gte_read_data(24) <= 0;
}

uint32 level2p_sub_8003DB74(uint32 face, uint32 position)
{
    uint32 group, normal, vertex, numerator, first_product, second_product;
    sint32 divisor, magnitude;
    FUNCTION_MARKER(0x8003DB74, "LEVEL2P.BIN");
    group = r_u32(0x800AAC38);
    vertex = (uint32)(sint32)r_s16(face) << 3;
    normal = r_u32(group + 0x20) + ((uint32)(sint32)r_s16(face + 6) << 3);
    vertex += r_u32(group + 0x1C);
    divisor = r_s16(normal + 2);
    magnitude = divisor < 0 ? -divisor : divisor;
    if (magnitude <= 256) return (uint32)(sint32)r_s16(vertex + 2);
    first_product = (uint32)(sint32)r_s16(normal) * (uint32)(sint32)r_s16(position);
    second_product = (uint32)(sint32)r_s16(normal + 4) * (uint32)(sint32)r_s16(position + 4);
    numerator = 0 - (first_product + second_product + ((uint32)(sint32)r_s16(face + 8) << 15));
    /* Magnitude admission excludes the original division BREAK cases */
    return (uint32)(sint32)(sint16)((sint32)numerator / divisor);
}

uint32 level2p_sub_8007FB10(uint32 model, uint32 draw1, uint32 draw2, uint32 mode)
{
    uint32 handle, related, linked, compressed = 0, temporary = 0, ordering, result = 0;
    uint32 depth[2], depth_view, submitted, data, level, offset, step, shift, bucket, previous;
    sint32 nearest = 65535, base, maximum, value;
    FUNCTION_MARKER(0x8007FB10, "LEVEL2P.BIN");
    if (r_u32(model + 0x74))
    {
        if (r_u32(model + 4) & 0x80) return level2p_sub_800802E8(model, draw1, draw2);
        return level2p_sub_8007FF88(model, draw1, draw2, mode);
    }
    handle = r_u32(model + 0x54);
    related = handle ? r_u32(0x80099460 + ((handle & 0xFFFF) << 2)) : 0;
    if (related)
    {
        linked = r_u32(model + 0x58);
        if (!linked)
        {
            handle = r_u32(model + 0x54);
            related = handle ? r_u32(0x80099460 + ((handle & 0xFFFF) << 2)) : 0;
            linked = r_u32(related + 0x100);
        }
        compressed = r_u32(linked + 0x74) != 0;
        ordering = r_u32(linked + draw2 * 4 + 0x4C);
        if (!(r_u32(linked + 4) & 0x20))
        {
            moh_draft_missing_call(0x8001722C, ordering, 512);
            w_u32(linked + 4, r_u32(linked + 4) | 0x20);
        }
    }
    else
    {
        ordering = r_u32(model + draw2 * 4 + 0x4C);
        if (!ordering)
        {
            temporary = 1;
            ordering = draw1;
        }
        else if (!(r_u32(model + 4) & 0x20))
        {
            moh_draft_missing_call(0x8001722C, ordering, 512);
            w_u32(model + 4, r_u32(model + 4) | 0x20);
        }
    }
    if (r_u32(model + 0x28))
    {
        /* TODO: Addressable depth output requires the permitted native adapter */
        depth_view = moh_draft_local_address(depth, sizeof(depth));
        submitted = compressed ? level2p_sub_8007F31C(model, ordering, draw2, 1, depth_view)
            : level2p_sub_8007DE4C(model, ordering, draw2, 1, depth_view, temporary);
        result = submitted != 0;
        if ((sint32)depth[0] < nearest) nearest = (sint32)depth[0];
    }
    if (r_u32(model + 0x20))
    {
        depth_view = moh_draft_local_address(depth, sizeof(depth));
        submitted = compressed ? level2p_sub_8007F31C(model, ordering, draw2, 0, depth_view)
            : level2p_sub_8007DE4C(model, ordering, draw2, 0, depth_view, temporary);
        result = submitted != 0 || result != 0;
        if ((sint32)depth[0] < nearest) nearest = (sint32)depth[0];
    }
    if (temporary || !result) return result;
    handle = r_u32(model + 0x54);
    related = handle ? r_u32(0x80099460 + ((handle & 0xFFFF) << 2)) : 0;
    if (related) return result;
    base = (sint32)((uint32)nearest - mode) >> 4;
    if (base < 0) base = 0;
    else if (base > 511) base = 511;
    data = r_u32(model + 8);
    maximum = r_s16(data + 0x3E);
    value = r_s16(data + 0x40);
    if (maximum < value) maximum = value;
    value = r_s16(data + 0x3C);
    if (maximum <= value) maximum = value;
    value = maximum - 4;
    if (value < 0) value = 0;
    level = (uint32)value;
    shift = (3 - level) & 31;
    step = (uint32)(8 >> (level & 31));
    offset = 0;
    /* TODO: Original metadata can produce zero step; retain the original loop */
    while ((sint32)offset < 512)
    {
        if (model == r_u32(r_u32(0x8009943C) + 0x100)) bucket = 1;
        else
        {
            value = (sint32)((uint32)base + (uint32)((sint32)offset >> shift));
            bucket = value < 511 ? (uint32)value : 511;
        }
        previous = offset ? offset - 1 : 0;
        offset += step;
        sub_800145DC(draw1 + bucket * 4, ordering + offset * 4 - 4, ordering + previous * 4);
    }
    return result;
}


static void moh_model_merge_buckets(uint32 draw, uint32 ordering, sint32 base, sint32 step)
{
    uint32 offset = 0, previous, next, last, bucket;
    if (base < 2) base = 2;
    /* TODO: Original depth range can produce zero step; no invented clamp */
    while ((sint32)offset < 512)
    {
        bucket = base < 511 ? (uint32)base : 511;
        previous = offset ? offset - 1 : 0;
        next = offset + (uint32)step;
        last = (sint32)(next - 1) < 511 ? ordering + next * 4 - 4 : ordering + 0x7FC;
        sub_800145DC(draw + bucket * 4, last, ordering + previous * 4);
        base = (sint32)(bucket + 1);
        offset = next;
    }
}

uint32 level2p_sub_800802E8(uint32 model, uint32 draw, uint32 index)
{
    uint32 ordering, result;
    sint32 range, step, minimum;
    FUNCTION_MARKER(0x800802E8, "LEVEL2P.BIN");
    ordering = r_u32(model + index * 4 + 0x4C);
    if (r_s16(model + 0xAA) & 0x8000)
    {
        if (!(r_u32(model + 4) & 0x20))
        {
            moh_draft_missing_call(0x8001722C, ordering, 512);
            w_u32(model + 4, r_u32(model + 4) | 0x20);
        }
        w_u16(model + 0xAA, r_u16(model + 0xAA) & 0x7FFF);
        result = level2p_sub_8007E414(model, ordering, index, 0) != 0;
        w_u32(0x8009ABF4, r_u32(0x1F8003FC));
        return result;
    }
    range = r_s16(0x8009ABF6);
    range -= r_s16(0x8009ABF4);
    minimum = r_s16(0x8009ABF4);
    range >>= 4;
    /* Range at least two excludes both original division BREAK cases */
    step = range < 2 ? 511 : (sint16)(511 / range);
    moh_model_merge_buckets(draw, ordering, minimum >> 4, step);
    return 0;
}

uint32 level2p_sub_8007FF88(uint32 model, uint32 draw, uint32 index, uint32 mode)
{
    uint32 handle, related, linked, ordering, temporary = 0, result;
    sint32 range, minimum, step;
    FUNCTION_MARKER(0x8007FF88, "LEVEL2P.BIN");
    handle = r_u32(model + 0x54);
    related = handle ? r_u32(0x80099460 + ((handle & 0xFFFF) << 2)) : 0;
    if (related)
    {
        linked = r_u32(model + 0x58);
        if (!linked)
        {
            handle = r_u32(model + 0x54);
            related = handle ? r_u32(0x80099460 + ((handle & 0xFFFF) << 2)) : 0;
            linked = r_u32(related + 0x100);
        }
        ordering = r_u32(linked + index * 4 + 0x4C);
        if (!(r_u32(linked + 4) & 0x20))
        {
            moh_draft_missing_call(0x8001722C, ordering, 512);
            w_u32(linked + 4, r_u32(linked + 4) | 0x20);
        }
    }
    else
    {
        ordering = r_u32(model + index * 4 + 0x4C);
        if (!ordering)
        {
            temporary = 1;
            ordering = draw;
        }
        else if (!(r_u32(model + 4) & 0x20))
        {
            moh_draft_missing_call(0x8001722C, ordering, 512);
            w_u32(model + 4, r_u32(model + 4) | 0x20);
        }
    }
    if (r_u32(model + 0x6C) == 128) result = level2p_sub_8007E414(model, ordering, index, temporary) != 0;
    else result = level2p_sub_8007EB04(model, ordering, index, temporary) != 0;
    if (temporary || !result) return result;
    handle = r_u32(model + 0x54);
    related = handle ? r_u32(0x80099460 + ((handle & 0xFFFF) << 2)) : 0;
    if (related) return result;
    range = r_s16(0x1F8003FE);
    range -= r_s16(0x1F8003FC);
    minimum = r_s16(0x1F8003FC);
    range >>= 4;
    step = range < 2 ? 511 : (sint16)(511 / range);
    if (model == r_u32(r_u32(0x8009942C) + 0x100))
        sub_800145DC(draw + 4, ordering + 0x7FC, ordering);
    else moh_model_merge_buckets(draw, ordering, minimum >> 4, step);
    return result;
}


uint32 level2p_sub_8005365C(uint32 kind, uint32 limit_output)
{
    FUNCTION_MARKER(0x8005365C, "LEVEL2P.BIN");
    w_u32(limit_output, r_u32(0x80094C3C));
    return r_u32(0x80094C38);
}

void level2p_sub_8005367C(uint32 cursor)
{
    FUNCTION_MARKER(0x8005367C, "LEVEL2P.BIN");
    w_u32(0x80094C38, cursor);
}

static void moh_model_project_store(uint32 vertex)
{
    if ((xport_gte_read_flag() & 0x80020000) == 0x80000000) w_u16(vertex + 4, 0xFFFF);
    else
    {
        w_u32(vertex, xport_gte_read_data(14));
        w_u32(vertex + 4, xport_gte_read_data(19));
    }
}

static void moh_model_depth_extrema(uint32 vertex, uint32 *minimum, uint32 *maximum)
{
    sint32 value = r_s16(vertex + 4);
    uint32 raw = r_u16(vertex + 4);
    if (value != -1)
    {
        if (value < (sint16)*minimum) *minimum = raw;
        if ((sint16)*maximum < value) *maximum = raw;
    }
}

uint32 level2p_sub_8007E414(uint32 model, uint32 ordering, uint32 index, uint32 temporary)
{
    uint32 format, polygons, source, vertex_data, limit, limit_view, packet, table, count, code;
    uint32 previous, vertex, iteration, minimum = 32767, maximum = 0xFFFF8000, shift, bucket_min = 1;
    uint32 handle, related, records, first, second, third, bucket, color1, color2, color3, emitted = 0, tag;
    sint32 depth;
    FUNCTION_MARKER(0x8007E414, "LEVEL2P.BIN");
    if (!r_u32(model + 0x88)) return 0;
    format = r_u32(model + 0x84);
    polygons = r_u32(model + 0x7C);
    source = r_u32(r_u32(r_u32(model + 0x74) + 4) + 4);
    vertex_data = r_u32(model + 0x80);
    /* TODO: Addressable buffer-limit local requires the permitted native adapter */
    limit_view = moh_draft_local_address(&limit, sizeof(limit));
    packet = level2p_sub_8005365C(r_u32(0x80098B88), limit_view);
    count = r_u32(polygons + 4);
    limit -= 40;
    table = packet + count * 40;
    if (table > limit) return 0;
    code = r_u32(model + 0x68) ? 0x36 : 0x34;
    if (table + r_u32(vertex_data + 4) * 16 > limit) return 0;
    moh_bone_save_controls();
    level2p_sub_800836C0(model, format, source, table);
    moh_bone_restore_controls();
    previous = r_u32(table);
    xport_gte_write_data(0, r_u32(previous));
    xport_gte_write_data(1, r_u32(previous + 4));
    xport_gte_execute(0x180001);
    moh_model_project_store(previous);
    vertex = previous + 8;
    xport_gte_write_data(0, r_u32(vertex));
    xport_gte_write_data(1, r_u32(vertex + 4));
    xport_gte_execute(0x180001);
    iteration = 1;
    while ((sint32)iteration < (sint32)(r_u32(0x1F8003FC) - 1))
    {
        moh_model_depth_extrema(previous, &minimum, &maximum);
        moh_model_project_store(vertex);
        previous = vertex;
        vertex += 8;
        xport_gte_write_data(0, r_u32(vertex));
        xport_gte_write_data(1, r_u32(vertex + 4));
        xport_gte_execute(0x180001);
        iteration++;
    }
    moh_model_depth_extrema(previous, &minimum, &maximum);
    moh_model_project_store(vertex);
    moh_model_depth_extrema(vertex, &minimum, &maximum);
    shift = 4;
    if (!temporary)
    {
        bucket_min = 2;
        shift = (uint32)(((sint32)(sint16)maximum - (sint32)(sint16)minimum) >> 9);
    }
    handle = r_u32(model + 0x54);
    records = r_u32(polygons);
    related = handle ? r_u32(0x80099460 + ((handle & 0xFFFF) << 2)) : 0;
    for (iteration = 0; (sint32)iteration < (sint32)count; iteration++, records += 28)
    {
        first = r_u32(table + (uint32)r_u8(records + 23) * 4) + (uint32)r_u8(records + 22) * 8;
        second = r_u32(table + (uint32)r_u8(records + 25) * 4) + (uint32)r_u8(records + 24) * 8;
        third = r_u32(table + (uint32)r_u8(records + 27) * 4) + (uint32)r_u8(records + 26) * 8;
        if (r_s16(first + 4) < 0 || r_s16(second + 4) < 0 || r_s16(third + 4) < 0) continue;
        xport_gte_write_data(12, r_u32(first));
        xport_gte_write_data(14, r_u32(third));
        xport_gte_write_data(13, r_u32(second));
        xport_gte_execute(0x1400006);
        if ((sint32)xport_gte_read_data(24) <= 0) continue;
        xport_gte_write_data(17, r_u16(first + 4));
        xport_gte_write_data(18, r_u16(second + 4));
        xport_gte_write_data(19, r_u16(third + 4));
        xport_gte_execute(0x158002D);
        depth = (sint32)(xport_gte_read_data(7) << 2);
        if (!temporary)
        {
            if (depth < (sint16)minimum) depth = (sint16)minimum;
            depth = (sint32)((uint32)depth - (uint32)(sint32)(sint16)minimum);
        }
        bucket = (uint32)(depth >> (shift & 31));
        color1 = r_u32(records);
        color2 = r_u32(records + 4);
        color3 = r_u32(records + 8);
        depth = r_s16(r_u32(0x80098FB0) + 0xCC);
        w_u32(packet + 8, r_u32(first));
        w_u32(packet + 20, r_u32(second));
        w_u32(packet + 32, r_u32(third));
        if (!depth)
        {
            w_u32(packet + 4, r_u32(records));
            w_u32(packet + 16, r_u32(records + 4));
            w_u32(packet + 28, r_u32(records + 8));
        }
        else
        {
            xport_gte_write_data(20, color1);
            xport_gte_write_data(21, color2);
            xport_gte_write_data(22, color3);
            xport_gte_write_data(6, color3);
            xport_gte_execute(0xF8002A);
            w_u32(packet + 4, xport_gte_read_data(20));
            w_u32(packet + 16, xport_gte_read_data(21));
            w_u32(packet + 28, xport_gte_read_data(22));
        }
        w_u8(packet + 3, 9);
        w_u8(packet + 7, (uint8)code);
        w_u32(packet + 12, r_u32(records + 12));
        w_u32(packet + 24, r_u32(records + 16));
        w_u16(packet + 36, r_u16(records + 20));
        if (related) bucket -= 15;
        if ((sint32)bucket < (sint32)bucket_min) bucket = bucket_min;
        if ((sint32)bucket >= 512) bucket = 511;
        emitted++;
        tag = ordering + bucket * 4;
        w_u32(packet, (r_u32(packet) & 0xFF000000) | (r_u32(tag) & 0xFFFFFF));
        w_u32(tag, (r_u32(tag) & 0xFF000000) | (packet & 0xFFFFFF));
        packet += 40;
    }
    tag = r_u16(model + 0xAA);
    w_u32(0x1F8003FC, minimum | (maximum << 16));
    w_u16(model + 0xA8, (uint16)minimum);
    w_u32(model + 0xAC, shift);
    w_u16(model + 0xAA, (uint16)((tag & 0x8000) | maximum));
    level2p_sub_8005367C(packet);
    return (sint32)emitted > 0;
}


void level2p_sub_800836C0(uint32 model, uint32 format, uint32 source, uint32 output)
{
    uint32 matrix[8] = { 0x1000, 0, 0x1000, 0, 0x1000, 0, 0, 0 };
    uint32 vertex_data, x, y, z, count, kind, matrix_view;
    FUNCTION_MARKER(0x800836C0, "LEVEL2P.BIN");
    vertex_data = r_u32(model + 0x80);
    x = (uint32)(sint32)r_s16(model + 0x98) << 19;
    y = (uint32)(sint32)r_s16(model + 0x9A) << 19;
    z = (uint32)(sint32)r_s16(model + 0x9C) << 19;
    kind = r_u8(source + 15);
    w_u32(0x8009B4FC, 0x800AA490);
    count = r_u32(vertex_data + 4);
    matrix[5] = x;
    matrix[6] = y;
    matrix[7] = z;
    /* Matrix padding is unused by the original GTE control4 input */
    /* TODO: Addressable native matrix requires the permitted local adapter */
    matrix_view = moh_draft_local_address(matrix, sizeof(matrix));
    w_u32(0x1F8003FC, 0);
    w_u32(0x8009B4F4, kind);
    w_u32(0x8009B4F8, count);
    sub_80014128(format, matrix_view, output, model);
}


uint32 level2p_sub_8007EB04(uint32 model, uint32 ordering, uint32 index, uint32 temporary)
{
    uint32 format, polygons, source, vertex_data, limit, limit_view, packet, table, count, code;
    uint32 previous, vertex, iteration, minimum = 32767, maximum = 0xFFFF8000, shift, bucket_min = 1;
    uint32 handle, related, records, first, second, third, bucket, color1, color2, color3, emitted = 0, tag;
    uint32 factor, products[9];
    sint32 depth;
    FUNCTION_MARKER(0x8007EB04, "LEVEL2P.BIN");
    if (!r_u32(model + 0x88)) return 0;
    format = r_u32(model + 0x84);
    polygons = r_u32(model + 0x7C);
    source = r_u32(r_u32(r_u32(model + 0x74) + 4) + 4);
    vertex_data = r_u32(model + 0x80);
    /* TODO: Addressable buffer-limit local requires the permitted native adapter */
    limit_view = moh_draft_local_address(&limit, sizeof(limit));
    packet = level2p_sub_8005365C(r_u32(0x80098B88), limit_view);
    count = r_u32(polygons + 4);
    limit -= 40;
    table = packet + count * 40;
    if (table > limit) return 0;
    code = r_u32(model + 0x68) ? 0x36 : 0x34;
    if (table + r_u32(vertex_data + 4) * 16 > limit) return 0;
    moh_bone_save_controls();
    level2p_sub_800836C0(model, format, source, table);
    moh_bone_restore_controls();
    previous = r_u32(table);
    xport_gte_write_data(0, r_u32(previous));
    xport_gte_write_data(1, r_u32(previous + 4));
    xport_gte_execute(0x180001);
    moh_model_project_store(previous);
    vertex = previous + 8;
    xport_gte_write_data(0, r_u32(vertex));
    xport_gte_write_data(1, r_u32(vertex + 4));
    xport_gte_execute(0x180001);
    iteration = 1;
    while ((sint32)iteration < (sint32)(r_u32(0x1F8003FC) - 1))
    {
        moh_model_depth_extrema(previous, &minimum, &maximum);
        moh_model_project_store(vertex);
        previous = vertex;
        vertex += 8;
        xport_gte_write_data(0, r_u32(vertex));
        xport_gte_write_data(1, r_u32(vertex + 4));
        xport_gte_execute(0x180001);
        iteration++;
    }
    moh_model_depth_extrema(previous, &minimum, &maximum);
    moh_model_project_store(vertex);
    moh_model_depth_extrema(vertex, &minimum, &maximum);
    shift = 4;
    if (!temporary)
    {
        bucket_min = 2;
        shift = (uint32)(((sint32)(sint16)maximum - (sint32)(sint16)minimum) >> 9);
    }
    handle = r_u32(model + 0x54);
    records = r_u32(polygons);
    related = handle ? r_u32(0x80099460 + ((handle & 0xFFFF) << 2)) : 0;
    for (iteration = 0; (sint32)iteration < (sint32)count; iteration++, records += 28)
    {
        first = r_u32(table + (uint32)r_u8(records + 23) * 4) + (uint32)r_u8(records + 22) * 8;
        second = r_u32(table + (uint32)r_u8(records + 25) * 4) + (uint32)r_u8(records + 24) * 8;
        third = r_u32(table + (uint32)r_u8(records + 27) * 4) + (uint32)r_u8(records + 26) * 8;
        if (r_s16(first + 4) < 0 || r_s16(second + 4) < 0 || r_s16(third + 4) < 0) continue;
        xport_gte_write_data(12, r_u32(first));
        xport_gte_write_data(14, r_u32(third));
        xport_gte_write_data(13, r_u32(second));
        xport_gte_execute(0x1400006);
        if ((sint32)xport_gte_read_data(24) <= 0) continue;
        xport_gte_write_data(17, r_u16(first + 4));
        xport_gte_write_data(18, r_u16(second + 4));
        xport_gte_write_data(19, r_u16(third + 4));
        xport_gte_execute(0x158002D);
        depth = (sint32)(xport_gte_read_data(7) << 2);
        if (!temporary)
        {
            if (depth < (sint16)minimum) depth = (sint16)minimum;
            depth = (sint32)((uint32)depth - (uint32)(sint32)(sint16)minimum);
        }
        bucket = (uint32)(depth >> (shift & 31));
        color1 = r_u32(records);
        color2 = r_u32(records + 4);
        color3 = r_u32(records + 8);
        factor = r_u32(model + 0x6C);
        depth = r_s16(r_u32(0x80098FB0) + 0xCC);
        w_u32(packet + 8, r_u32(first));
        w_u32(packet + 20, r_u32(second));
        w_u32(packet + 32, r_u32(third));
        if (!depth)
        {
            w_u32(packet + 4, r_u32(records));
            w_u32(packet + 16, r_u32(records + 4));
            w_u32(packet + 28, r_u32(records + 8));
        }
        else
        {
            xport_gte_write_data(20, color1);
            xport_gte_write_data(21, color2);
            xport_gte_write_data(22, color3);
            xport_gte_write_data(6, color3);
            xport_gte_execute(0xF8002A);
            w_u32(packet + 4, xport_gte_read_data(20));
            w_u32(packet + 16, xport_gte_read_data(21));
            w_u32(packet + 28, xport_gte_read_data(22));
        }
        w_u8(packet + 3, 9);
        w_u8(packet + 7, (uint8)code);
        w_u32(packet + 12, r_u32(records + 12));
        w_u32(packet + 24, r_u32(records + 16));
        w_u16(packet + 36, r_u16(records + 20));
        if (factor != 128)
        {
            products[0] = (uint32)r_u8(packet + 4) * factor;
            products[1] = (uint32)r_u8(packet + 5) * factor;
            products[2] = (uint32)r_u8(packet + 6) * factor;
            products[3] = (uint32)r_u8(packet + 16) * factor;
            products[4] = (uint32)r_u8(packet + 17) * factor;
            products[5] = (uint32)r_u8(packet + 18) * factor;
            products[6] = (uint32)r_u8(packet + 28) * factor;
            products[7] = (uint32)r_u8(packet + 29) * factor;
            w_u8(packet + 4, (uint8)(products[0] >> 7));
            w_u8(packet + 5, (uint8)(products[1] >> 7));
            products[8] = (uint32)r_u8(packet + 30) * factor;
            w_u8(packet + 16, (uint8)(products[3] >> 7));
            w_u8(packet + 6, (uint8)(products[2] >> 7));
            w_u8(packet + 17, (uint8)(products[4] >> 7));
            w_u8(packet + 18, (uint8)(products[5] >> 7));
            w_u8(packet + 28, (uint8)(products[6] >> 7));
            w_u8(packet + 29, (uint8)(products[7] >> 7));
            w_u8(packet + 30, (uint8)(products[8] >> 7));
        }
        if (related) bucket -= 15;
        if ((sint32)bucket < (sint32)bucket_min) bucket = bucket_min;
        if ((sint32)bucket >= 512) bucket = 511;
        emitted++;
        tag = ordering + bucket * 4;
        w_u32(packet, (r_u32(packet) & 0xFF000000) | (r_u32(tag) & 0xFFFFFF));
        w_u32(tag, (r_u32(tag) & 0xFF000000) | (packet & 0xFFFFFF));
        packet += 40;
    }
    tag = r_u16(model + 0xAA);
    w_u32(0x1F8003FC, minimum | (maximum << 16));
    w_u16(model + 0xA8, (uint16)minimum);
    w_u32(model + 0xAC, shift);
    w_u16(model + 0xAA, (uint16)((tag & 0x8000) | maximum));
    level2p_sub_8005367C(packet);
    return (sint32)emitted > 0;
}
