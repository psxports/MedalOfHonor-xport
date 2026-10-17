#include "game_level2p_signatures.h"
#include "xport_trace.h"
#include "psx.h"

void level2p_sub_800470FC(uint32 destination, uint32 owner)
{
    uint32 data, output, index, table, pointer, position[2], input[4], quaternion[2];
    uint32 a, b, c, d, doubled_a, doubled_b, doubled_d, square_c, square_d;
    uint32 first[3], second[3], third[2], matrix[8], transformed[6], component;
    FUNCTION_MARKER(0x800470FC, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388);
    output = r_u32(destination + 0x388);
    index = r_u32(data + 0x634) << 3;
    pointer = r_u32(data + 0x138) + index;
    position[0] = r_u32(pointer); position[1] = r_u32(pointer + 4);
    input[0] = r_u32(data + 0x638); input[1] = r_u32(data + 0x63C);
    input[2] = r_u32(data + 0x640); input[3] = r_u32(data + 0x644);
    w_u32(output + 0x54, position[0] << 16);
    w_u32(output + 0x58, position[0] & 0xFFFF0000);
    w_u32(output + 0x5C, position[1] << 16);
    table = r_u32(data + 0x12C);
    if (!table)
    {
        table = r_u32(data + 0x130);
        if (!table) table = r_u32(data + 0x134);
    }
    quaternion[0] = r_u32(table + index);
    quaternion[1] = r_u32(table + index + 4);
    a = (uint32)(sint32)(sint16)quaternion[0];
    b = (uint32)((sint32)quaternion[0] >> 16);
    c = (uint32)(sint32)(sint16)quaternion[1];
    d = (uint32)((sint32)quaternion[1] >> 16);
    xport_gte_write_control(0, (d << 16) | (a & 0xFFFF));
    component = (b << 16) | (c & 0xFFFF);
    xport_gte_write_control(1, component);
    xport_gte_write_control(3, component);
    xport_gte_write_control(2, ((0 - c) & 0xFFFF) | (d << 16));
    xport_gte_write_control(4, 0 - a);
    doubled_a = a << 1; doubled_b = b << 1; doubled_d = d << 1;
    xport_gte_write_data(0, (d << 17) | (doubled_a & 0xFFFF));
    xport_gte_write_data(1, 0);
    xport_gte_execute(0x486012);
    square_d = d * d;
    first[0] = xport_gte_read_data(25);
    first[1] = xport_gte_read_data(26);
    first[2] = xport_gte_read_data(27);
    xport_gte_write_data(0, doubled_b & 0xFFFF);
    xport_gte_write_data(1, doubled_d);
    first[0] -= 0x1000;
    xport_gte_execute(0x486012);
    second[0] = xport_gte_read_data(25);
    second[1] = xport_gte_read_data(26);
    second[2] = xport_gte_read_data(27);
    square_c = c * c;
    xport_gte_write_data(0, (0 - doubled_b) << 16);
    xport_gte_write_data(1, doubled_a);
    second[1] -= 0x1000;
    square_d = (uint32)((sint32)square_d >> 12);
    xport_gte_execute(0x486012);
    square_c = (uint32)((sint32)square_c >> 12);
    third[0] = xport_gte_read_data(25);
    third[1] = xport_gte_read_data(26);
    matrix[0] = (first[0] & 0xFFFF) | (second[0] << 16);
    matrix[1] = (third[0] & 0xFFFF) | (first[1] << 16);
    matrix[2] = (second[1] & 0xFFFF) | (third[1] << 16);
    matrix[3] = (first[2] & 0xFFFF) | (second[2] << 16);
    /* Original undefined padding is discarded by the signed16 RT33 control register */
    matrix[4] = (uint32)(sint32)(sint16)(((square_c + square_d) << 1) - 0x1000);
    matrix[5] = (uint32)(sint32)r_s16(r_u32(data + 0x138) + index);
    matrix[6] = (uint32)(sint32)r_s16(r_u32(data + 0x138) + index + 2);
    matrix[7] = (uint32)(sint32)r_s16(r_u32(data + 0x138) + index + 4);
    for (component = 0; component < 8; ++component) xport_gte_write_control(component, matrix[component]);
    xport_gte_write_data(0, input[0]); xport_gte_write_data(1, input[1]);
    xport_gte_execute(0x480012);
    transformed[0] = xport_gte_read_data(25);
    transformed[1] = xport_gte_read_data(26);
    transformed[2] = xport_gte_read_data(27);
    xport_gte_write_data(0, input[2]); xport_gte_write_data(1, input[3]);
    xport_gte_execute(0x480012);
    transformed[3] = xport_gte_read_data(25);
    transformed[4] = xport_gte_read_data(26);
    transformed[5] = xport_gte_read_data(27);
    w_u32(output + 0x24, transformed[0] << 16);
    w_u32(output + 0x28, transformed[1] << 16);
    w_u32(output + 0x2C, transformed[2] << 16);
    w_u32(output + 0x84, transformed[3] << 16);
    w_u32(output + 0x88, transformed[4] << 16);
    w_u32(output + 0x8C, transformed[5] << 16);
}


void level2p_sub_80047678(uint32 owner)
{
    uint32 model, index, pointer, output, position[2], table, quaternion[2];
    uint32 a, b, c, d, doubled_a, doubled_b, doubled_d, square_c, square_d;
    uint32 first[3], second[3], third[2], matrix[8], transformed[3], component, vector_xy, vector_z;
    FUNCTION_MARKER(0x80047678, "LEVEL2P.BIN");
    model = r_u32(owner + 0x100);
    index = (uint32)(sint32)r_s16(r_u32(model + 8) + 0xD4) << 3;
    pointer = r_u32(model + 0x94);
    output = r_u32(owner + 0x388);
    position[0] = r_u32(pointer + index); position[1] = r_u32(pointer + index + 4);
    w_u32(output + 0x54, position[0] << 16);
    w_u32(output + 0x58, position[0] & 0xFFFF0000);
    w_u32(output + 0x5C, position[1] << 16);
    table = r_u32(model + 0x88);
    if (!table)
    {
        table = r_u32(model + 0x8C);
        if (!table) table = r_u32(model + 0x90);
    }
    quaternion[0] = r_u32(table + index); quaternion[1] = r_u32(table + index + 4);
    a = (uint32)(sint32)(sint16)quaternion[0];
    b = (uint32)((sint32)quaternion[0] >> 16);
    c = (uint32)(sint32)(sint16)quaternion[1];
    d = (uint32)((sint32)quaternion[1] >> 16);
    xport_gte_write_control(0, (d << 16) | (a & 0xFFFF));
    component = (b << 16) | (c & 0xFFFF);
    xport_gte_write_control(1, component);
    xport_gte_write_control(3, component);
    xport_gte_write_control(2, ((0 - c) & 0xFFFF) | (d << 16));
    xport_gte_write_control(4, 0 - a);
    doubled_a = a << 1; doubled_b = b << 1; doubled_d = d << 1;
    xport_gte_write_data(0, (d << 17) | (doubled_a & 0xFFFF));
    xport_gte_write_data(1, 0);
    xport_gte_execute(0x486012);
    square_d = d * d;
    first[0] = xport_gte_read_data(25);
    first[1] = xport_gte_read_data(26);
    first[2] = xport_gte_read_data(27);
    xport_gte_write_data(0, doubled_b & 0xFFFF);
    xport_gte_write_data(1, doubled_d);
    first[0] -= 0x1000;
    xport_gte_execute(0x486012);
    second[0] = xport_gte_read_data(25);
    second[1] = xport_gte_read_data(26);
    second[2] = xport_gte_read_data(27);
    square_c = c * c;
    xport_gte_write_data(0, (0 - doubled_b) << 16);
    xport_gte_write_data(1, doubled_a);
    second[1] -= 0x1000;
    square_d = (uint32)((sint32)square_d >> 12);
    xport_gte_execute(0x486012);
    square_c = (uint32)((sint32)square_c >> 12);
    third[0] = xport_gte_read_data(25);
    third[1] = xport_gte_read_data(26);
    matrix[0] = (first[0] & 0xFFFF) | (second[0] << 16);
    matrix[1] = (third[0] & 0xFFFF) | (first[1] << 16);
    matrix[2] = (second[1] & 0xFFFF) | (third[1] << 16);
    matrix[3] = (first[2] & 0xFFFF) | (second[2] << 16);
    /* Original undefined padding is discarded by the signed16 RT33 control register */
    matrix[4] = (uint32)(sint32)(sint16)(((square_c + square_d) << 1) - 0x1000);
    matrix[5] = (uint32)(sint32)(sint16)position[0];
    matrix[6] = (uint32)((sint32)position[0] >> 16);
    matrix[7] = (uint32)(sint32)(sint16)position[1];
    for (component = 0; component < 8; ++component) xport_gte_write_control(component, matrix[component]);
    pointer = r_u32(model + 8) + 0xD8;
    vector_xy = r_u32(pointer); xport_gte_write_data(0, vector_xy);
    vector_z = r_u32(pointer + 4); xport_gte_write_data(1, vector_z);
    xport_gte_execute(0x480012);
    transformed[0] = xport_gte_read_data(25);
    transformed[1] = xport_gte_read_data(26);
    transformed[2] = xport_gte_read_data(27);
    w_u32(output + 0x24, transformed[0] << 16);
    w_u32(output + 0x28, transformed[1] << 16);
    w_u32(output + 0x2C, transformed[2] << 16);
    pointer = r_u32(model + 8) + 0xE0;
    vector_xy = r_u32(pointer); xport_gte_write_data(0, vector_xy);
    vector_z = r_u32(pointer + 4); xport_gte_write_data(1, vector_z);
    xport_gte_execute(0x480012);
    transformed[0] = xport_gte_read_data(25);
    transformed[1] = xport_gte_read_data(26);
    transformed[2] = xport_gte_read_data(27);
    w_u32(output + 0x84, transformed[0] << 16);
    w_u32(output + 0x88, transformed[1] << 16);
    w_u32(output + 0x8C, transformed[2] << 16);
}

void level2p_sub_8008B000(uint32 allocation, uint32 mode)
{
    FUNCTION_MARKER(0x8008B000, "LEVEL2P.BIN");
    if (mode & 1) sub_8001DFB0(allocation);
}

void level2p_sub_8008B250(uint32 allocation, uint32 mode)
{
    uint32 begin, cursor;
    FUNCTION_MARKER(0x8008B250, "LEVEL2P.BIN");
    begin = allocation + 0x44;
    cursor = allocation + 0xEC;
    if (begin && begin != cursor)
    {
        cursor -= 0x1C;
        do
        {
            level2p_sub_8008B000(cursor, 2);
            if (cursor == begin) break;
            cursor -= 0x1C;
        } while (1);
    }
    if (mode & 1) sub_8001DFB0(allocation);
}

void level2p_sub_80047DC8(uint32 owner)
{
    uint32 resource;
    FUNCTION_MARKER(0x80047DC8, "LEVEL2P.BIN");
    resource = r_u32(r_u32(owner + 0x388) + 0xC8);
    if (resource) level2p_sub_8008B250(resource, 3);
    level2p_sub_8006543C(owner);
}

void level2p_sub_80047E10(uint32 state)
{
    uint32 first, second;
    FUNCTION_MARKER(0x80047E10, "LEVEL2P.BIN");
    w_u32(state + 0x94, 0xFFFFFF10);
    w_u32(state + 0x98, 0xFFFFFF12);
    w_u32(state + 0x20, 0xC80000); w_u32(state + 0x2C, 0xC80000);
    w_u32(state + 0x38, 0xC80000); w_u32(state + 0x78, 0xC80000);
    w_u32(state + 0x84, 0xC80000);
    first = r_u32(state + 0x98);
    w_u32(state + 0x18, 0); w_u32(state + 0x24, 0);
    w_u32(state + 0x30, 0); w_u32(state + 0x48, 0);
    w_u32(state + 0x50, 0); w_u32(state + 0x54, 0);
    w_u32(state + 0x5C, 0); w_u32(state + 0x60, 0);
    w_u32(state + 0x68, 0); w_u32(state + 0x80, 0);
    w_u32(state + 0x1C, first << 16);
    second = r_u32(state + 0x98);
    w_u32(state + 0x28, first << 16);
    first = r_u32(state + 0x94);
    w_u32(state + 0x34, second << 16);
    second = r_u32(state + 0x94);
    w_u32(state + 0x4C, first << 16);
    w_u32(state + 0x58, second << 16);
    first = r_u32(state + 0x94);
    w_u32(state + 0x64, second << 16);
    w_u32(state + 0x7C, first << 16);
    w_u32(state + 0x88, first << 16);
    w_u32(state + 0x8C, 0);
}


uint32 level2p_sub_8008B1E8(uint32 allocation)
{
    uint32 element, count;
    FUNCTION_MARKER(0x8008B1E8, "LEVEL2P.BIN");
    element = allocation + 0x44;
    w_u32(allocation + 0x3C, 0);
    w_u32(allocation + 0x40, 0);
    for (count = 0; count < 6; ++count)
    {
        level2p_sub_8008AFF4(element);
        element += 0x1C;
    }
    w_u32(allocation + 0xEC, 0);
    return allocation;
}

void level2p_sub_80047EC8(uint32 owner)
{
    uint32 flags, data, x, y, z, settings, resource, value;
    FUNCTION_MARKER(0x80047EC8, "LEVEL2P.BIN");
    w_u32(owner + 0xC, 0x20002);
    w_u32(owner + 0x36C, 0x80047BA0);
    w_u32(owner + 0x384, 0x80047DC8);
    flags = r_u32(owner + 0x10);
    data = r_u32(owner + 0x388);
    w_u32(owner + 8, 2);
    w_u32(owner + 0x14, 0);
    w_u32(owner + 0x374, 0); w_u32(owner + 0x378, 0);
    w_u32(owner + 0x37C, 0); w_u32(owner + 0x284, 0);
    w_u32(owner + 0x10, flags | 0x213);
    x = r_u32(owner + 0x20C); y = r_u32(owner + 0x210); z = r_u32(owner + 0x214);
    w_u32(owner + 0x3C0, x); w_u32(owner + 0x3C4, y); w_u32(owner + 0x3C8, z);
    w_u32(data + 0x9C, 0x400); w_u32(data + 0x14, 0);
    w_u16(data + 0xD8, 3); w_u16(data + 0xDA, 3); w_u16(data + 0xE0, 2);
    level2p_sub_80046E80(owner, 0, 0, 0);
    level2p_sub_80047E10(data);
    w_u32(data + 0x10, 0);
    level2p_sub_80068A04(data + 0xA0, 0);
    settings = r_u32(0x80098FB0);
    w_u16(data + 0xCE, 0);
    value = r_u32(settings + 0x590);
    if ((sint32)value <= 0) value = 0x1194;
    w_u32(data + 0xC4, value);
    w_u32(data + 0x90, 0);
    resource = r_u32(data + 0xC8);
    w_u32(resource + 0x18, 0); w_u32(resource + 0x1C, 0xFFFF0000); w_u32(resource + 0x20, 0);
    resource = r_u32(data + 0xC8);
    w_u32(resource + 0x30, 0x2000000); w_u32(resource + 0x34, 0xC00000); w_u32(resource + 0x38, 0);
    value = r_u32(0x80096974);
    resource = r_u32(data + 0xC8);
    w_u32(resource + 0x3C, value << 16);
    value = r_u32(data + 0xC4);
    resource = r_u32(data + 0xC8);
    w_u32(resource + 0x40, value << 16);
}

uint32 level2p_sub_80048038(void)
{
    uint32 index, descriptor, owner, flags, model, animation, data, allocation;
    FUNCTION_MARKER(0x80048038, "LEVEL2P.BIN");
    index = level2p_sub_80065BF8(0xCBE42983);
    if ((sint32)index > 0)
    {
        descriptor = level2p_sub_80065BB8(index);
        owner = level2p_sub_80066934(1, 0x120, 0xCBE42983, 0x157, 0, 5, 0, descriptor);
        if (!owner) return 0;
        flags = r_u32(owner + 0x10);
        model = r_u32(owner + 0x100);
        w_u32(owner + 0x10, flags | 0x200);
        if (!model || !r_u32(model + 8)) w_u32(owner + 0x370, 0);
        else
        {
            level2p_sub_8006591C(owner);
            animation = (uint32)(sint32)r_s16(r_u32(r_u32(owner + 0x100) + 8) + 0x94);
            if ((sint32)animation >= 0)
            {
                level2p_sub_8003F764(owner, animation, 1, 0);
                level2p_sub_8003FC00(owner);
            }
            w_u32(owner + 0x370, 0x8004701C);
        }
    }
    else
    {
        owner = level2p_sub_80066934(0, 0x120, 0xFFFFFFFF, 0, 0, 0, 0, 0);
        if (!owner) return 0;
        flags = r_u32(owner + 0x10);
        w_u32(owner + 0x370, 0);
        w_u32(owner + 0x10, flags | 0x200);
    }
    data = r_u32(owner + 0x388);
    allocation = sub_80016718(0xF0);
    /* TODO: Original forwards allocation failure without a null check */
    allocation = level2p_sub_8008B1E8(allocation);
    w_u32(data + 0xC8, allocation);
    level2p_sub_80047EC8(owner);
    level2p_sub_8004DFBC(owner);
    return owner;
}


static void moh_camera_load_matrix(uint32 address)
{
    uint32 first, second, third;
    first = r_u32(address); second = r_u32(address + 4);
    xport_gte_write_control(0, first); xport_gte_write_control(1, second);
    first = r_u32(address + 8); second = r_u32(address + 12); third = r_u32(address + 16);
    xport_gte_write_control(2, first); xport_gte_write_control(3, second); xport_gte_write_control(4, third);
    first = r_u32(address + 20); second = r_u32(address + 24);
    xport_gte_write_control(5, first);
    third = r_u32(address + 28);
    xport_gte_write_control(6, second); xport_gte_write_control(7, third);
}

static void moh_camera_push_matrix(const uint32 *matrix)
{
    uint32 stack, save, index, first, second;
    stack = r_u32(0x80099100);
    save = 0x80099104 + stack;
    for (index = 0; index < 8; index += 2)
    {
        first = xport_gte_read_control(index); second = xport_gte_read_control(index + 1);
        w_u32(save + index * 4, first); w_u32(save + (index + 1) * 4, second);
    }
    w_u32(0x80099100, stack + 32);
    for (index = 0; index < 8; ++index) xport_gte_write_control(index, matrix[index]);
}

static void moh_camera_pop_matrix(void)
{
    uint32 stack, save, index, first, second;
    stack = r_u32(0x80099100) - 32;
    save = 0x80099104 + stack;
    second = r_u32(save + 4);
    w_u32(0x80099100, stack);
    first = r_u32(save);
    xport_gte_write_control(0, first); xport_gte_write_control(1, second);
    for (index = 2; index < 8; index += 2)
    {
        second = r_u32(save + (index + 1) * 4); first = r_u32(save + index * 4);
        xport_gte_write_control(index, first); xport_gte_write_control(index + 1, second);
    }
}

void level2p_sub_8004819C(uint32 owner, uint32 target)
{
    uint32 data, flags, matrix_address, first, second, third, x, y, z, next_x;
    uint32 shift, current, offset, destination, delta, cached_x, cached_y, cached_z;
    uint32 direction, angle, translation_view, vector_view, second_view, matrix_view, flags_view, result_view;
    uint32 matrix[8], scale[4], translation[4], result[3], sdk_flags;
    sint16 vector[4] = { 0 }, second_vector[4] = { 0 };
    FUNCTION_MARKER(0x8004819C, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388);
    flags = r_u32(data + 0x9C);
    if (flags & 0x1800)
    {
        if (flags & 0x800) matrix_address = owner + 0x340;
        else matrix_address = r_u32(target + 0x388) + 0x154;
        moh_camera_load_matrix(matrix_address);
        x = (uint32)(sint32)r_s16(data + 0x1A);
        y = (uint32)(sint32)r_s16(data + 0x1E);
        z = (uint32)(sint32)r_s16(data + 0x22);
        xport_gte_write_data(0, (x & 0xFFFF) | (y << 16));
        /* VZ0 discards the original undefined high half */
        xport_gte_write_data(1, z);
        xport_gte_execute(0x480012);
        first = xport_gte_read_data(25); second = xport_gte_read_data(26); third = xport_gte_read_data(27);
        w_u32(data, first << 16);
        w_u32(data + 4, second << 16);
        next_x = (uint32)(sint32)r_s16(data + 0x7A);
        w_u32(data + 8, third << 16);
        w_u32(data + 0x98, (uint32)(sint32)(sint16)second);
        y = (uint32)(sint32)r_s16(data + 0x7E);
        z = (uint32)(sint32)r_s16(data + 0x82);
        xport_gte_write_data(0, (next_x & 0xFFFF) | (y << 16));
        xport_gte_write_data(1, z);
        xport_gte_execute(0x480012);
        first = xport_gte_read_data(25); second = xport_gte_read_data(26); third = xport_gte_read_data(27);
        w_u32(data + 0x6C, first << 16);
        w_u32(data + 0x70, second << 16);
        w_u32(data + 0x74, third << 16);
        return;
    }
    if (flags & 0x480)
    {
        translation_view = moh_draft_local_address(translation, sizeof(translation));
        moh_draft_missing_call(0x800158AC, translation_view, 0, 16);
        vector[0] = r_s16(data + 0x1A); vector[1] = 0; vector[2] = r_s16(data + 0x22);
        second_vector[0] = r_s16(data + 0x7A); second_vector[1] = 0; second_vector[2] = r_s16(data + 0x82);
        first = r_u32(r_u32(target + 0x388) + 0x800);
        w_u32(data + 0xBC, first);
        matrix[0] = 0x1000; matrix[1] = 0; matrix[2] = 0x1000; matrix[3] = 0;
        /* Only RT33 consumes the final matrix word's low half */
        matrix[4] = 0x1000;
        angle = (uint32)(sint32)r_s16(data + 0xBE);
        matrix_view = moh_draft_local_address(matrix, sizeof(matrix));
        moh_draft_missing_call(0x80016248, angle, matrix_view);
        moh_draft_missing_call(0x80016218, matrix_view, translation_view);
        moh_camera_push_matrix(matrix);
        vector_view = moh_draft_local_address(vector, sizeof(vector));
        second_view = moh_draft_local_address(second_vector, sizeof(second_vector));
        flags_view = moh_draft_local_address(&sdk_flags, sizeof(sdk_flags));
        moh_draft_missing_call(0x80016B30, vector_view, vector_view, flags_view);
        moh_draft_missing_call(0x80016B30, second_view, second_view, flags_view);
        moh_camera_pop_matrix();
        first = r_u32(target + 0x20C);
        w_u32(data, first + ((uint32)(uint16)vector[0] << 16));
        first = r_u32(target + 0x214);
        w_u32(data + 8, first + ((uint32)(uint16)vector[2] << 16));
        first = r_u32(target + 0x210); second = r_u32(data + 0x1C);
        w_u32(data + 4, first + second);
        first = r_u32(target + 0x20C);
        w_u32(data + 0x6C, first + ((uint32)(uint16)second_vector[0] << 16));
        first = r_u32(target + 0x214) + ((uint32)(uint16)second_vector[2] << 16);
        second = r_u32(data + 0x94);
        w_u32(data + 0x74, first);
        first = r_u32(target + 0x210);
        w_u32(data + 0x70, first + (second << 16));
        return;
    }
    if (flags & 0x10)
    {
        first = r_u32(target + 0x20C); second = r_u32(data + 0x18);
        current = r_u32(data); shift = (uint32)(sint32)r_s16(data + 0xDA) & 31;
        w_u32(data, current + (uint32)((sint32)(first + second - current) >> shift));
        first = r_u32(target + 0x210); second = r_u32(data + 0x1C); current = r_u32(data + 4);
        w_u32(data + 4, current + (uint32)((sint32)(first + second - current) >> shift));
        first = r_u32(target + 0x214); second = r_u32(data + 0x20); current = r_u32(data + 8);
        w_u32(data + 8, current + (uint32)((sint32)(first + second - current) >> shift));
        for (offset = 0; offset < 12; offset += 4)
        {
            first = r_u32(target + 0x20C + offset); second = r_u32(data + 0x78 + offset);
            current = r_u32(data + offset);
            shift = (uint32)(sint32)r_s16(data + 0xDA) & 31;
            destination = r_u32(data + 0x6C + offset);
            w_u32(data + 0x6C + offset, destination + (uint32)((sint32)(first + second - current) >> shift));
        }
        return;
    }
    if (flags & 8)
    {
        first = r_u32(data + 0x24); second = r_u32(data + 0x18);
        shift = (uint32)(sint32)r_s16(data + 0xDA) & 31; cached_y = r_u32(data + 0x1C);
        delta = (uint32)((sint32)(first - second) >> shift);
        current = r_u32(data); first = r_u32(data + 0x28);
        x = current + delta; shift = (uint32)(sint32)r_s16(data + 0xDA) & 31;
        delta = first - cached_y;
        w_u32(data, x);
        current = r_u32(data + 4); cached_x = r_u32(data + 0x18);
        delta = (uint32)((sint32)delta >> shift);
        first = r_u32(data + 0x2C); y = current + delta;
        w_u32(data + 4, y);
        second = r_u32(data + 0x20); shift = (uint32)(sint32)r_s16(data + 0xDA) & 31;
        delta = (uint32)((sint32)(first - second) >> shift);
        current = r_u32(data + 8); first = r_u32(data + 0x84);
        z = current + delta; shift = (uint32)(sint32)r_s16(data + 0xDA) & 31;
        delta = first - cached_x;
        w_u32(data + 8, z);
        current = r_u32(data + 0x6C); cached_z = r_u32(data + 0x20);
        delta = (uint32)((sint32)delta >> shift);
        first = r_u32(data + 0x88); x = current + delta;
        w_u32(data + 0x6C, x);
        second = r_u32(data + 0x1C); shift = (uint32)(sint32)r_s16(data + 0xDA) & 31;
        delta = (uint32)((sint32)(first - second) >> shift);
        current = r_u32(data + 0x70); first = r_u32(data + 0x8C);
        y = current + delta; shift = (uint32)(sint32)r_s16(data + 0xDA) & 31;
        delta = first - cached_z;
        w_u32(data + 0x70, y);
        current = r_u32(data + 0x74);
        w_u32(data + 0x74, current + (uint32)((sint32)delta >> shift));
        return;
    }
    if (!(flags & 0x120)) return;
    vector_view = moh_draft_local_address(vector, sizeof(vector));
    moh_draft_missing_call(0x800158AC, vector_view, 0, 8);
    translation_view = moh_draft_local_address(translation, sizeof(translation));
    moh_draft_missing_call(0x800158AC, translation_view, 0, 16);
    scale[0] = r_u32(0x80039DB0); scale[1] = r_u32(0x80039DB4); scale[2] = r_u32(0x80039DB8);
    scale[3] = r_u32(0x80039DBC);
    second_vector[0] = r_s16(data + 0x1A); second_vector[1] = r_s16(data + 0x1E); second_vector[2] = r_s16(data + 0x22);
    if (r_u32(data + 0x9C) & 0x100)
    {
        first = r_u32(data + 0x30); direction = r_u32(target + 0x288);
        first *= direction;
        angle = r_u32(data + 0xBC);
        second = r_u32(data + 0x34); third = r_u32(data + 0x38);
        w_u32(data + 0x28, second); w_u32(data + 0x2C, third); w_u32(data + 0x24, first);
        direction = r_u32(target + 0x288);
    }
    else
    {
        direction = r_u32(target + 0x288); angle = r_u32(data + 0xBC);
    }
    first = r_u32(target + 0x22C);
    delta = first - (angle + (direction == 1 ? 0 : 0x8000000));
    while ((sint32)delta > 0x8000000) delta -= 0x10000000;
    while ((sint32)delta < -0x8000000) delta += 0x10000000;
    shift = r_u32(data + 0x14) & 31; current = r_u32(data + 0xBC);
    w_u32(data + 0xBC, current + (uint32)((sint32)delta >> shift));
    vector[0] = 0; angle = (uint32)(sint32)r_s16(data + 0xBE); vector[2] = 0; vector[1] = (sint16)angle;
    matrix_view = moh_draft_local_address(matrix, sizeof(matrix));
    moh_draft_missing_call(0x8001676C, vector_view, matrix_view);
    moh_draft_missing_call(0x80016218, matrix_view, translation_view);
    result_view = moh_draft_local_address(scale, sizeof(scale));
    moh_draft_missing_call(0x800165D8, matrix_view, result_view);
    moh_camera_push_matrix(matrix);
    second_view = moh_draft_local_address(second_vector, sizeof(second_vector));
    result_view = moh_draft_local_address(result, sizeof(result));
    flags_view = moh_draft_local_address(&sdk_flags, sizeof(sdk_flags));
    moh_draft_missing_call(0x80016548, second_view, result_view, flags_view);
    moh_camera_pop_matrix();
    first = result[0] << 16; second = r_u32(target + 0x20C);
    current = r_u32(data); shift = (uint32)(sint32)r_s16(data + 0xDA) & 31;
    w_u32(data, current + (uint32)((sint32)(second + first - current) >> shift));
    first = result[2] << 16; second = r_u32(target + 0x214); current = r_u32(data + 8);
    w_u32(data + 8, current + (uint32)((sint32)(second + first - current) >> shift));
    first = result[1] << 16; second = r_u32(target + 0x210); current = r_u32(data + 4);
    w_u32(data + 4, current + (uint32)((sint32)(second + first - current) >> 1));
}


static void moh_camera_update_yaw(uint32 data, uint32 target)
{
    uint32 first, second, third, direction, angle, delta, shift, current;
    if (r_u32(data + 0x9C) & 0x200)
    {
        first = r_u32(data + 0x60); direction = r_u32(target + 0x288);
        first *= direction;
        angle = r_u32(data + 0xB0);
        second = r_u32(data + 0x64); third = r_u32(data + 0x68);
        w_u32(data + 0x58, second); w_u32(data + 0x5C, third); w_u32(data + 0x54, first);
        direction = r_u32(target + 0x288);
    }
    else
    {
        direction = r_u32(target + 0x288); angle = r_u32(data + 0xB0);
    }
    first = r_u32(target + 0x22C);
    delta = first - (angle + (direction == 1 ? 0 : 0x8000000));
    while ((sint32)delta > 0x8000000) delta -= 0x10000000;
    while ((sint32)delta < -0x8000000) delta += 0x10000000;
    shift = r_u32(data + 0x14) & 31; current = r_u32(data + 0xB0);
    w_u32(data + 0xB0, current + (uint32)((sint32)delta >> shift));
}

void level2p_sub_80048B98(uint32 owner, uint32 target)
{
    static const uint32 reset_offsets[3] = { 0x54, 0x5C, 0x58 };
    uint32 data, flags, x, y, z, first, second, third, current, shift, index;
    uint32 matrix_address, angle, matrix[8], translation[4], result[3], other_result[3], query[3], sdk_flags;
    uint32 translation_view, vector_view, second_view, matrix_view, result_view, flags_view, query_view;
    uint32 height, previous, delta, vertical, old_flags, velocity, position;
    sint16 vector[4] = { 0 }, second_vector[4] = { 0 }, axis[4] = { 0 };
    FUNCTION_MARKER(0x80048B98, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388);
    flags = r_u32(data + 0x9C);
    if (flags & 0x800)
    {
        x = (uint32)(sint32)r_s16(data + 0x4A);
        y = (uint32)(sint32)r_s16(data + 0x4E);
        z = (uint32)(sint32)r_s16(data + 0x52);
        w_u32(data + 0x94, y);
        moh_camera_load_matrix(owner + 0x340);
        xport_gte_write_data(0, (x & 0xFFFF) | (y << 16));
        xport_gte_write_data(1, z);
        xport_gte_execute(0x480012);
        first = xport_gte_read_data(25); second = xport_gte_read_data(26); third = xport_gte_read_data(27);
        w_u32(data + 0x3C, first << 16); w_u32(data + 0x44, third << 16);
        height = r_u32(owner + 0x208);
        delta = (uint32)((sint32)height >> 16) - (uint32)(sint32)(sint16)second;
        if ((sint32)delta < 0x55) w_u32(data + 0x40, height - 0x550000);
        else w_u32(data + 0x40, second << 16);
        return;
    }
    if (flags & 0x80) goto copy_target_with_height;
    if (flags & 0x1000)
    {
        x = (uint32)(sint32)r_s16(data + 0x4A);
        matrix_address = r_u32(target + 0x388) + 0x154;
        y = (uint32)(sint32)r_s16(data + 0x4E);
        z = (uint32)(sint32)r_s16(data + 0x52);
        w_u32(data + 0x94, y);
        moh_camera_load_matrix(matrix_address);
        xport_gte_write_data(0, (x & 0xFFFF) | (y << 16));
        xport_gte_write_data(1, z);
        xport_gte_execute(0x480012);
        first = xport_gte_read_data(25); second = xport_gte_read_data(26); third = xport_gte_read_data(27);
        w_u32(data + 0x3C, first << 16); w_u32(data + 0x40, second << 16); w_u32(data + 0x44, third << 16);
        first = r_u32(target + 0x20C); w_u32(owner + 0x20C, first);
        first = r_u32(target + 0x210); w_u32(owner + 0x210, first);
        goto copy_target_z;
    }
    if (flags & 0x400) goto copy_target_with_height;
    if (flags & 2)
    {
        first = r_u32(target + 0x20C); second = r_u32(data + 0x48);
        current = r_u32(owner + 0x20C); shift = (uint32)(sint32)r_s16(data + 0xD8) & 31;
        w_u32(owner + 0x1F0, (uint32)((sint32)(first + second - current) >> shift));
        first = r_u32(target + 0x210); second = r_u32(data + 0x4C);
        current = r_u32(owner + 0x210); shift = (uint32)(sint32)r_s16(data + 0xD8) & 31;
        w_u32(owner + 0x1F4, (uint32)((sint32)(first + second - current) >> shift));
        first = r_u32(target + 0x214); second = r_u32(data + 0x50);
        first += second;
        goto write_velocity_z;
    }
    if (flags & 1)
    {
        first = r_u32(data + 0x54); current = r_u32(owner + 0x20C);
        shift = (uint32)(sint32)r_s16(data + 0xD8) & 31;
        w_u32(owner + 0x1F0, (uint32)((sint32)(first - current) >> shift));
        first = r_u32(data + 0x58); current = r_u32(owner + 0x210);
        shift = (uint32)(sint32)r_s16(data + 0xD8) & 31;
        w_u32(owner + 0x1F4, (uint32)((sint32)(first - current) >> shift));
        first = r_u32(data + 0x5C);
        goto write_velocity_z;
    }
    if (flags & 0x204)
    {
        translation_view = moh_draft_local_address(translation, sizeof(translation));
        moh_draft_missing_call(0x800158AC, translation_view, 0, 16);
        second_view = moh_draft_local_address(axis, sizeof(axis));
        moh_draft_missing_call(0x800158AC, second_view, 0, 8);
        axis[1] = 0x1000;
        first = (uint32)(sint32)r_s16(data + 0x1A); second = (uint32)(sint32)r_s16(data + 0x4A); vector[0] = (sint16)(first - second);
        first = (uint32)(sint32)r_s16(data + 0x1E); second = (uint32)(sint32)r_s16(data + 0x4E); vector[1] = (sint16)(first - second);
        first = (uint32)(sint32)r_s16(data + 0x22); second = (uint32)(sint32)r_s16(data + 0x52); vector[2] = (sint16)(first - second);
        xport_gte_write_control(0, (uint32)(sint32)axis[0]);
        xport_gte_write_control(2, (uint32)(sint32)axis[1]);
        xport_gte_write_control(4, (uint32)(sint32)axis[2]);
        xport_gte_write_data(9, (uint32)(sint32)vector[0]);
        xport_gte_write_data(10, (uint32)(sint32)vector[1]);
        xport_gte_write_data(11, (uint32)(sint32)vector[2]);
        xport_gte_execute(0x178000C);
        second_vector[0] = (sint16)xport_gte_read_data(9);
        second_vector[1] = (sint16)xport_gte_read_data(10);
        second_vector[2] = (sint16)xport_gte_read_data(11);
        second_vector[1] = r_s16(data + 0x4E);
        vector[0] = r_s16(data + 0x4A); vector[1] = r_s16(data + 0x4E); vector[2] = r_s16(data + 0x52);
        moh_camera_update_yaw(data, target);
        matrix[0] = 0x1000; matrix[1] = 0; matrix[2] = 0x1000; matrix[3] = 0; matrix[4] = 0x1000;
        angle = (uint32)(sint32)r_s16(data + 0xB2);
        matrix_view = moh_draft_local_address(matrix, sizeof(matrix));
        moh_draft_missing_call(0x80016248, angle, matrix_view);
        moh_draft_missing_call(0x80016218, matrix_view, translation_view);
        moh_camera_push_matrix(matrix);
        vector_view = moh_draft_local_address(vector, sizeof(vector));
        result_view = moh_draft_local_address(result, sizeof(result));
        flags_view = moh_draft_local_address(&sdk_flags, sizeof(sdk_flags));
        moh_draft_missing_call(0x80016548, vector_view, result_view, flags_view);
        second_view = moh_draft_local_address(second_vector, sizeof(second_vector));
        result_view = moh_draft_local_address(other_result, sizeof(other_result));
        moh_draft_missing_call(0x80016548, second_view, result_view, flags_view);
        moh_camera_pop_matrix();
        x = r_u32(target + 0x20C); y = r_u32(target + 0x210); second = r_u32(data + 0x4C);
        z = r_u32(target + 0x214);
        level2p_sub_800684D4(owner, x + (result[0] << 16), y + second, z + (result[2] << 16));
        first = r_u32(target + 0x20C);
        w_u32(data + 0x6C, first + (other_result[0] << 16));
        first = r_u32(owner + 0x210); w_u32(data + 0x70, first);
        first = r_u32(target + 0x214);
        w_u32(data + 0x74, first + (other_result[2] << 16));
        return;
    }
    if (!(flags & 0x40)) return;
    translation_view = moh_draft_local_address(translation, sizeof(translation));
    moh_draft_missing_call(0x800158AC, translation_view, 0, 16);
    first = r_u32(target + 0x20C); second = r_u32(target + 0x258); third = r_u32(target + 0x260);
    x = (uint32)((sint32)(first - second) >> 16);
    first = r_u32(target + 0x214);
    if ((sint32)x < 0) x = 0 - x;
    z = (uint32)((sint32)(first - third) >> 16);
    if ((sint32)z < 0) z = 0 - z;
    if ((sint32)(x + z) >= 0x15)
    {
        for (index = 0; index < 3; ++index)
        {
            first = r_u32(data + reset_offsets[index]); second = r_u32(data + reset_offsets[index] + 12);
            if (first != second) w_u32(data + reset_offsets[index], second);
        }
    }
    previous = r_u32(target + 0x25C);
    first = (uint32)(sint32)r_s16(data + 0x4A); second = r_u16(data + 0xD0);
    current = r_u32(target + 0x210);
    vector[0] = (sint16)(first + second);
    first = (uint32)(sint32)r_s16(data + 0x4E); second = r_u16(data + 0xD2);
    vertical = previous - current;
    vector[1] = (sint16)(first + second);
    first = (uint32)(sint32)r_s16(data + 0x52); second = r_u16(data + 0xD4);
    vertical = (uint32)((sint32)vertical >> 16);
    vector[2] = (sint16)(first + second);
    if ((sint32)vertical < 0) vertical = 0 - vertical;
    moh_camera_update_yaw(data, target);
    matrix[0] = 0x1000; matrix[1] = 0; matrix[2] = 0x1000; matrix[3] = 0; matrix[4] = 0x1000;
    angle = (uint32)(sint32)r_s16(data + 0xB2);
    matrix_view = moh_draft_local_address(matrix, sizeof(matrix));
    moh_draft_missing_call(0x80016248, angle, matrix_view);
    moh_draft_missing_call(0x80016218, matrix_view, translation_view);
    moh_camera_push_matrix(matrix);
    vector_view = moh_draft_local_address(vector, sizeof(vector));
    result_view = moh_draft_local_address(result, sizeof(result));
    flags_view = moh_draft_local_address(&sdk_flags, sizeof(sdk_flags));
    moh_draft_missing_call(0x80016548, vector_view, result_view, flags_view);
    moh_camera_pop_matrix();
    first = r_u32(target + 0x20C); query[0] = first + (result[0] << 16);
    first = r_u32(target + 0x210); second = r_u32(data + 0x58); query[1] = first + second;
    first = r_u32(target + 0x214); query[2] = first + (result[2] << 16);
    query_view = moh_draft_local_address(query, sizeof(query));
    height = level2p_sub_8003F140(query_view, 0, 1, 0, 0, 0) - 0x100;
    w_u16(data + 0xDC, height);
    first = (uint32)(sint32)r_s16(owner + 0x212);
    if ((sint32)(height << 16) < (sint32)((first - 0x100) << 16))
    {
        first = r_u16(data + 0xDE); w_u16(data + 0xDC, first);
    }
    first = (uint32)(sint32)r_s16(owner + 0x212); second = (uint32)(sint32)r_s16(data + 0xDC);
    delta = first - second;
    third = r_u16(data + 0xDC);
    if ((sint32)delta < 0x40) w_u16(data + 0xDC, third - 0x40);
    first = (uint32)(sint32)r_s16(target + 0x212); second = (uint32)(sint32)r_s16(data + 0xDC);
    if ((sint16)(first + 0x200) < (sint32)second) w_u16(data + 0xDC, first);
    first = r_u16(data + 0xDC); w_u16(data + 0xDE, first);
    if (r_u32(owner + 0x38) & 0x10000)
    {
        first = r_u32(target + 0x20C); shift = (uint32)(sint32)r_s16(data + 0xD8) & 31;
        current = r_u32(owner + 0x20C);
        w_u32(owner + 0x1F4, 0);
        w_u32(owner + 0x1F0, (uint32)((sint32)(first + (result[0] << 16) - current) >> shift));
        first = r_u32(target + 0x214); current = r_u32(owner + 0x214);
        shift = (uint32)(sint32)r_s16(data + 0xD8) & 31;
        w_u32(owner + 0x1F8, (uint32)((sint32)(first + (result[2] << 16) - current) >> shift));
        shift = (uint32)(sint32)r_s16(data + 0xE0) & 31;
        velocity = r_u32(owner + 0x1F0); position = r_u32(owner + 0x20C);
        position += (uint32)((sint32)velocity >> shift);
        old_flags = r_u32(owner + 0x38);
        w_u32(owner + 0x20C, position);
        shift = (uint32)(sint32)r_s16(data + 0xE0) & 31;
        w_u32(owner + 0x38, old_flags & 0xFFFEFFFF);
        velocity = r_u32(owner + 0x1F8); position = r_u32(owner + 0x214);
        w_u32(owner + 0x214, position + (uint32)((sint32)velocity >> shift));
        return;
    }
    first = r_u32(target + 0x20C); current = r_u32(owner + 0x20C);
    shift = (uint32)(sint32)r_s16(data + 0xD8) & 31;
    w_u32(owner + 0x1F0, (uint32)((sint32)(first + (result[0] << 16) - current) >> shift));
    first = (uint32)(sint32)r_s16(data + 0xDC); second = r_u32(data + 0x58);
    current = r_u32(owner + 0x210);
    delta = ((first + result[1]) << 16) - current + second;
    shift = (uint32)(sint32)r_s16(data + 0xD8) & 31; third = (uint32)(sint32)r_s16(data + 0xD2);
    w_u32(owner + 0x1F4, (uint32)((sint32)delta >> shift) + (third << 16) - (vertical << 16));
    first = r_u32(target + 0x214) + (result[2] << 16);
write_velocity_z:
    current = r_u32(owner + 0x214); shift = (uint32)(sint32)r_s16(data + 0xD8) & 31;
    w_u32(owner + 0x1F8, (uint32)((sint32)(first - current) >> shift));
    return;
copy_target_with_height:
    first = r_u32(target + 0x20C); w_u32(owner + 0x20C, first);
    first = r_u32(data + 0x94); second = r_u32(target + 0x210);
    w_u32(owner + 0x210, second + (first << 16));
copy_target_z:
    first = r_u32(target + 0x214); w_u32(owner + 0x214, first);
}

void level2p_sub_80047BA0(uint32 owner)
{
    uint32 data, handle, related, flags, model, first, second, third, fourth, timer;
    FUNCTION_MARKER(0x80047BA0, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388);
    handle = r_u32(data + 0xC);
    related = handle ? r_u32(0x80099460 + (handle & 0xFFFF) * 4) : 0;
    if (related)
    {
        flags = r_u32(data + 0x9C);
        if (flags & 0x800)
        {
            model = r_u32(owner + 0x100);
            if (!model || !r_u32(model + 8)) w_u32(data + 0x9C, (r_u32(data + 0x9C) & 0xFFFFF7FF) | 0x400);
            else
            {
                if (!r_u32(0x800968E8))
                {
                    level2p_sub_8003FF0C(owner);
                    level2p_sub_8003FC00(owner);
                }
                handle = r_u32(data + 0xC);
                related = handle ? r_u32(0x80099460 + (handle & 0xFFFF) * 4) : 0;
                /* Original resolves A1 even though the pose callee ignores it */
                (void)related;
                level2p_sub_80047678(owner);
            }
        }
        else if (flags & 0x1000)
        {
            handle = r_u32(data + 0xC);
            related = handle ? r_u32(0x80099460 + (handle & 0xFFFF) * 4) : 0;
            level2p_sub_800470FC(owner, related);
        }
        level2p_sub_80046EAC(data);
        handle = r_u32(data + 0xC);
        related = handle ? r_u32(0x80099460 + (handle & 0xFFFF) * 4) : 0;
        level2p_sub_8004819C(owner, related);
        handle = r_u32(data + 0xC);
        related = handle ? r_u32(0x80099460 + (handle & 0xFFFF) * 4) : 0;
        level2p_sub_80048B98(owner, related);
    }
    if (r_s16(data + 0xCE) <= 0) return;
    first = (uint32)(sint32)r_s16(data + 0xCC); second = r_u32(owner + 0x210);
    w_u32(owner + 0x210, second + (first << 16));
    first = (uint32)(sint32)r_s16(data + 0xCC); second = r_u32(data + 4);
    third = (uint32)(sint32)r_s16(data + 0xCC);
    fourth = r_u32(data + 0x70);
    w_u32(data + 4, second + (first << 16));
    first = r_u16(data + 0xCC);
    w_u32(data + 0x70, fourth + (third << 16));
    timer = r_u16(data + 0xCE);
    w_u16(data + 0xCC, 0 - (uint32)((sint32)(sint16)first >> 1));
    w_u16(data + 0xCE, timer - 1);
}


void level2p_sub_800497AC(uint32 owner, uint32 target, uint32 offset)
{
    uint32 data, handle, previous, resolved, value;
    FUNCTION_MARKER(0x800497AC, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388);
    handle = r_u32(data + 0xC);
    previous = handle ? r_u32(0x80099460 + (handle & 0xFFFF) * 4) : 0;
    handle = r_u32(data + 0xC);
    resolved = handle ? r_u32(0x80099460 + (handle & 0xFFFF) * 4) : 0;
    if (resolved && (r_u32(previous + 0xC) & 1)) level2p_sub_80068A04(r_u32(previous + 0x388) + 0xA0, 0);
    level2p_sub_80068A04(data + 0xC, target);
    if (offset)
    {
        value = r_u32(offset); w_u32(data + 0x18, value);
        value = r_u32(offset + 4); w_u32(data + 0x1C, value);
        value = r_u32(offset + 8); w_u32(data + 0x20, value);
    }
    /* TODO: Original requires a nonnull target for this flag read */
    if (r_u32(target + 0xC) & 1) level2p_sub_80068A04(r_u32(target + 0x388) + 0xA0, owner);
}

void level2p_sub_800498D0(uint32 view, uint32 owner)
{
    uint32 data, first, second, index, product;
    uint32 difference[3], normalized[3], right[3], up[4], relative[3], reference[3], negative[3], translated[3];
    uint32 difference_view, normalized_view, right_view, up_view, relative_view, reference_view, negative_view, translated_view;
    FUNCTION_MARKER(0x800498D0, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388);
    up[0] = r_u32(0x80039DC0); up[1] = r_u32(0x80039DC4); up[2] = r_u32(0x80039DC8);
    up[3] = r_u32(0x80039DCC);
    first = r_u32(view); second = r_u32(view + 12); difference[0] = first - second;
    first = r_u32(view + 4); second = r_u32(view + 16); difference[1] = first - second;
    first = r_u32(view + 8); second = r_u32(view + 20); difference[2] = first - second;
    first = (uint32)(sint32)r_s16(data + 0x6E); second = r_u32(view + 12); relative[0] = first - second;
    first = (uint32)(sint32)r_s16(data + 0x72); second = r_u32(view + 16); relative[1] = first - second;
    first = (uint32)(sint32)r_s16(data + 0x76); second = r_u32(view + 20); relative[2] = first - second;
    relative_view = moh_draft_local_address(relative, sizeof(relative));
    reference_view = moh_draft_local_address(reference, sizeof(reference));
    moh_draft_missing_call(0x80016A10, relative_view, reference_view);
    difference_view = moh_draft_local_address(difference, sizeof(difference));
    normalized_view = moh_draft_local_address(normalized, sizeof(normalized));
    moh_draft_missing_call(0x80016A10, difference_view, normalized_view);
    up_view = moh_draft_local_address(up, sizeof(up));
    moh_draft_missing_call(0x80016578, normalized_view, reference_view, up_view);
    right_view = moh_draft_local_address(right, sizeof(right));
    moh_draft_missing_call(0x80016578, up_view, normalized_view, right_view);
    w_u16(view + 0x1C, right[0] << 1);
    w_u16(view + 0x1E, right[1] << 1);
    w_u16(view + 0x20, right[2] << 1);
    for (index = 0; index < 3; ++index)
    {
        product = (up[index] << 2) + up[index];
        if ((sint32)product < 0) product += 7;
        product = (uint32)((sint32)product >> 3) << 1;
        w_u16(view + 0x22 + index * 2, product);
    }
    w_u16(view + 0x28, normalized[0] << 1);
    w_u16(view + 0x2A, normalized[1] << 1);
    w_u32(view + 0x30, 0); w_u32(view + 0x34, 0); w_u32(view + 0x38, 0);
    w_u16(view + 0x2C, normalized[2] << 1);
    /* RT33 discards the original unwritten high half at view2E */
    negative[0] = 0 - r_u32(view + 12);
    negative[1] = 0 - r_u32(view + 16);
    negative[2] = 0 - r_u32(view + 20);
    negative_view = moh_draft_local_address(negative, sizeof(negative));
    translated_view = moh_draft_local_address(translated, sizeof(translated));
    moh_draft_missing_call(0x800163E8, view + 0x1C, negative_view, translated_view);
    w_u32(view + 0x30, translated[0]);
    w_u32(view + 0x34, translated[1]);
    w_u32(view + 0x38, translated[2]);
}


static void moh_camera_plane_distance(uint32 plane)
{
    uint32 index, normal, point, sum = 0;
    for (index = 0; index < 3; ++index)
    {
        normal = r_u32(plane + 12 + index * 4); point = r_u32(plane + index * 4);
        sum += (uint32)((sint32)normal >> 8) * (uint32)((sint32)point >> 8);
    }
    w_u32(plane + 24, 0 - sum);
}

static void moh_camera_cross_local(const uint32 *first, const uint32 *second, uint32 *output)
{
    uint32 ax, ay, az, bx, by, bz;
    ax = (uint32)((sint32)first[0] >> 8); ay = (uint32)((sint32)first[1] >> 8); az = (uint32)((sint32)first[2] >> 8);
    bx = (uint32)((sint32)second[0] >> 8); by = (uint32)((sint32)second[1] >> 8); bz = (uint32)((sint32)second[2] >> 8);
    output[0] = ay * bz - az * by;
    output[1] = az * bx - ax * bz;
    output[2] = ax * by - ay * bx;
}

void level2p_sub_8008AEC8(uint32 input, uint32 output)
{
    uint32 first, value, mask = 0x40000000, index, shift, local[3], normalized[3], input_view, output_view;
    sint32 bit = 30;
    FUNCTION_MARKER(0x8008AEC8, "LEVEL2P.BIN");
    first = r_u32(input);
    if ((sint32)first < 0) first = 0 - first;
    for (;;)
    {
        if (first & mask) break;
        value = r_u32(input + 4);
        if ((sint32)value < 0) value = 0 - value;
        if (value & mask) break;
        value = r_u32(input + 8);
        if ((sint32)value < 0) value = 0 - value;
        if (value & mask) break;
        --bit;
        mask >>= 1;
        if (bit < 14) break;
    }
    shift = (uint32)(bit - 13);
    for (index = 0; index < 3; ++index)
    {
        value = r_u32(input + index * 4);
        local[index] = (sint32)shift > 0 ? (uint32)((sint32)value >> (shift & 31)) : value;
    }
    input_view = moh_draft_local_address(local, sizeof(local));
    output_view = moh_draft_local_address(normalized, sizeof(normalized));
    moh_draft_missing_call(0x80016A10, input_view, output_view);
    w_u32(output, normalized[0] << 4);
    w_u32(output + 4, normalized[1] << 4);
    w_u32(output + 8, normalized[2] << 4);
}

static void moh_camera_normalize_local(uint32 *input, uint32 *output)
{
    uint32 input_view, output_view;
    input_view = moh_draft_local_address(input, 12);
    output_view = moh_draft_local_address(output, 12);
    level2p_sub_8008AEC8(input_view, output_view);
}

void level2p_sub_8008B804(uint32 plane)
{
    FUNCTION_MARKER(0x8008B804, "LEVEL2P.BIN");
    moh_camera_plane_distance(plane);
}

void level2p_sub_8008B028(uint32 output, uint32 first_point, uint32 origin, uint32 second_point)
{
    uint32 x, y, z, index, first, second, delta[3], first_normal[3], second_normal[3], delta_view;
    FUNCTION_MARKER(0x8008B028, "LEVEL2P.BIN");
    x = r_u32(origin); y = r_u32(origin + 4); z = r_u32(origin + 8);
    w_u32(output, x); w_u32(output + 4, y); w_u32(output + 8, z);
    for (index = 0; index < 3; ++index)
    {
        first = r_u32(origin + index * 4); second = r_u32(first_point + index * 4);
        delta[index] = first - second;
    }
    moh_camera_normalize_local(delta, first_normal);
    for (index = 0; index < 3; ++index)
    {
        first = r_u32(second_point + index * 4); second = r_u32(origin + index * 4);
        delta[index] = first - second;
    }
    moh_camera_normalize_local(delta, second_normal);
    moh_camera_cross_local(first_normal, second_normal, delta);
    delta_view = moh_draft_local_address(delta, sizeof(delta));
    level2p_sub_8008AEC8(delta_view, output + 12);
    moh_camera_plane_distance(output);
}

void level2p_sub_8008B2CC(uint32 view)
{
    uint32 delta[3], forward[3], up[3], right[3], scaled_up[3], scaled_right[3], center[3], corners[4][3], far_point[3];
    uint32 index, first, second, scale, neg_right[3], point_views[4], count, x, y, z;
    FUNCTION_MARKER(0x8008B2CC, "LEVEL2P.BIN");
    for (index = 0; index < 3; ++index)
    {
        first = r_u32(view + 12 + index * 4); second = r_u32(view + index * 4); delta[index] = first - second;
    }
    moh_camera_normalize_local(delta, forward);
    for (index = 0; index < 3; ++index)
    {
        first = r_u32(view + 36 + index * 4); second = r_u32(view + index * 4); delta[index] = first - second;
    }
    moh_camera_normalize_local(delta, up);
    moh_camera_cross_local(forward, up, delta);
    moh_camera_normalize_local(delta, right);
    moh_camera_cross_local(right, forward, delta);
    moh_camera_normalize_local(delta, up);
    for (index = 0; index < 3; ++index) neg_right[index] = 0 - right[index];
    scale = (uint32)((sint32)r_u32(view + 0x30) >> 9);
    for (index = 0; index < 3; ++index) scaled_up[index] = (uint32)((sint32)up[index] >> 8) * scale;
    scale = (uint32)((sint32)r_u32(view + 0x34) >> 9);
    for (index = 0; index < 3; ++index) scaled_right[index] = (uint32)((sint32)neg_right[index] >> 8) * scale;
    for (index = 0; index < 3; ++index)
    {
        scale = (uint32)((sint32)r_u32(view + 0x3C) >> 8);
        first = r_u32(view + index * 4);
        center[index] = first + scale * (uint32)((sint32)forward[index] >> 8);
        corners[0][index] = center[index] + scaled_right[index] - scaled_up[index];
        corners[1][index] = center[index] + scaled_right[index] + scaled_up[index];
        corners[2][index] = center[index] - scaled_right[index] - scaled_up[index];
        corners[3][index] = center[index] - scaled_right[index] + scaled_up[index];
    }
    for (index = 0; index < 4; ++index) point_views[index] = moh_draft_local_address(corners[index], sizeof(corners[index]));
    level2p_sub_8008B028(view + 0x60, point_views[0], view, point_views[2]);
    level2p_sub_8008B028(view + 0x7C, point_views[3], view, point_views[1]);
    level2p_sub_8008B028(view + 0x98, point_views[1], view, point_views[0]);
    level2p_sub_8008B028(view + 0xB4, point_views[2], view, point_views[3]);
    x = r_u32(view); y = r_u32(view + 4); z = r_u32(view + 8);
    w_u32(view + 0x44, x); w_u32(view + 0x48, y); w_u32(view + 0x4C, z);
    w_u32(view + 0x50, forward[0]); w_u32(view + 0x54, forward[1]); w_u32(view + 0x58, forward[2]);
    level2p_sub_8008B804(view + 0x44);
    first = r_u32(view + 0x40);
    scale = (uint32)((sint32)first >> 8);
    if ((sint32)first > 0)
    {
        first = r_u32(view); far_point[0] = first + scale * (uint32)((sint32)forward[0] >> 8);
        scale = (uint32)((sint32)r_u32(view + 0x40) >> 8);
        first = r_u32(view + 4); far_point[1] = first + scale * (uint32)((sint32)forward[1] >> 8);
        scale = (uint32)((sint32)r_u32(view + 0x40) >> 8);
        first = r_u32(view + 8); far_point[2] = first + scale * (uint32)((sint32)forward[2] >> 8);
        w_u32(view + 0xD0, far_point[0]); w_u32(view + 0xD4, far_point[1]); w_u32(view + 0xD8, far_point[2]);
        w_u32(view + 0xDC, 0 - forward[0]); w_u32(view + 0xE0, 0 - forward[1]); w_u32(view + 0xE4, 0 - forward[2]);
        level2p_sub_8008B804(view + 0xD0);
        count = 6;
    }
    else count = 5;
    w_u32(view + 0xEC, count);
}

void level2p_sub_80049B20(uint32 owner)
{
    uint32 data, view, origin, value, handle, related, eye[3], near_point[3], second_point[3], zero_view;
    FUNCTION_MARKER(0x80049B20, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388);
    view = data + 0xE4;
    origin = (r_u32(data + 0x9C) & 0x1800) ? data + 0x3C : owner + 0x20C;
    value = (uint32)(sint32)r_s16(origin + 2); w_u32(view + 12, value);
    value = (uint32)(sint32)r_s16(origin + 6); w_u32(view + 16, value);
    value = (uint32)(sint32)r_s16(origin + 10); w_u32(view + 20, value);
    value = (uint32)(sint32)r_s16(data + 2); w_u32(view, value);
    value = (uint32)(sint32)r_s16(data + 6); w_u32(view + 4, value);
    value = (uint32)(sint32)r_s16(data + 10); w_u32(view + 8, value);
    value = (uint32)(sint32)r_s16(data + 0x92); w_u32(view + 24, value);
    level2p_sub_800498D0(view, owner);
    origin = (r_u32(data + 0x9C) & 0x1800) ? data + 0x3C : owner + 0x20C;
    eye[0] = r_u32(origin); eye[1] = r_u32(origin + 4); eye[2] = r_u32(origin + 8);
    zero_view = moh_draft_local_address(near_point, sizeof(near_point));
    moh_draft_missing_call(0x800158AC, zero_view, 0, 12);
    near_point[0] = r_u32(data); near_point[1] = r_u32(data + 4); near_point[2] = r_u32(data + 8);
    zero_view = moh_draft_local_address(second_point, sizeof(second_point));
    moh_draft_missing_call(0x800158AC, zero_view, 0, 12);
    second_point[0] = r_u32(data + 0x6C); second_point[1] = r_u32(data + 0x70); second_point[2] = r_u32(data + 0x74);
    view = r_u32(data + 0xC8);
    w_u32(view + 12, near_point[0]); w_u32(view + 16, near_point[1]); w_u32(view + 20, near_point[2]);
    view = r_u32(data + 0xC8);
    w_u32(view, eye[0]); w_u32(view + 4, eye[1]); w_u32(view + 8, eye[2]);
    view = r_u32(data + 0xC8);
    w_u32(view + 0x24, second_point[0]); w_u32(view + 0x28, second_point[1]); w_u32(view + 0x2C, second_point[2]);
    value = r_u32(data + 0xC4); view = r_u32(data + 0xC8);
    w_u32(view + 0x40, value << 16);
    handle = r_u32(data + 0xC);
    related = handle ? r_u32(0x80099460 + (handle & 0xFFFF) * 4) : 0;
    /* TODO: Original dereferences an unresolved or null attachment */
    value = r_u32(r_u32(related + 0x388) + 0x678);
    view = r_u32(data + 0xC8);
    w_u32(view + 0x3C, value << 16);
    level2p_sub_8008B2CC(r_u32(data + 0xC8));
}


void level2p_sub_8004A164(uint32 position, uint32 count, uint32 slot)
{
    uint32 state, end, consumed, remaining, result, output;
    FUNCTION_MARKER(0x8004A164, "LEVEL2P.BIN");
    state = 0x8008FDF4 + (slot << 5);
    end = r_u32(state + 8); consumed = r_u32(state + 12);
    remaining = end - consumed;
    if ((sint32)remaining < (sint32)count)
    {
        end = r_u32(state + 8); consumed = r_u32(state + 12);
        count = end - consumed;
    }
    if (!count)
    {
        w_u8(state + 0x1C, 0);
        return;
    }
    output = r_u32(state + 0x14);
    result = moh_draft_missing_call(0x80019220, position, count, output, 0x80);
    if ((sint32)result <= 0) moh_draft_missing_call(0x80015B28, 0x80039E68, r_u32(0x800968E0));
    output = r_u32(state + 0x14);
    w_u32(state + 0x14, output + (count << 11));
    remaining = r_u32(state + 4); w_u32(state + 4, remaining + count);
    remaining = r_u32(state + 12); w_u32(state + 12, remaining + count);
    remaining = r_u32(state + 16); w_u32(state + 16, remaining - count);
}

void level2p_sub_80049D90(void)
{
    uint8 position[8];
    uint32 sector, position_view;
    FUNCTION_MARKER(0x80049D90, "LEVEL2P.BIN");
    if (!r_u8(0x8008FE10)) return;
    if (!r_u32(0x8008FCD0)) return;
    w_u32(0x8008FCD0, 0);
    sector = r_u32(0x8008FDF8);
    /* TODO: SDK writes three BCD bytes; its fourth copied padding byte remains unspecified */
    position_view = moh_draft_local_address(position, sizeof(position));
    sub_8001ADD8(sector, position_view);
    level2p_sub_8004A164(position_view, 0x32, 0);
}

void level2p_sub_80049DF8(void)
{
    FUNCTION_MARKER(0x80049DF8, "LEVEL2P.BIN");
    moh_draft_missing_call(0x80019E20);
    moh_draft_missing_call(0x80019640, 0x80049D90);
}

void level2p_sub_8004A23C(uint32 slot, uint32 count)
{
    uint8 position[8];
    uint32 state, output, sector, position_view;
    FUNCTION_MARKER(0x8004A23C, "LEVEL2P.BIN");
    state = 0x8008FDF4 + (slot << 5);
    w_u8(state + 0x1D, 0);
    w_u32(state + 0x10, count);
    output = r_u32(state + 0x18);
    w_u32(state + 0x14, output);
    sector = r_u32(state + 4);
    /* TODO: Preserve unspecified SDK padding without recreating the guest stack */
    position_view = moh_draft_local_address(position, sizeof(position));
    sub_8001ADD8(sector, position_view);
    level2p_sub_8004A164(position_view, 0x14, 1);
    w_u32(0x8008FCD0, 1);
}


uint32 level2p_sub_80049E2C(uint32 output, uint32 input)
{
    uint32 cursor, destination, value, length;
    FUNCTION_MARKER(0x80049E2C, "LEVEL2P.BIN");
    moh_draft_missing_call(0x8001AD48, output, 0x80039DD0);
    if (r_u8(input) != 0x5C) moh_draft_missing_call(0x8001ABC8, output, 0x80039DD4);
    length = moh_draft_missing_call(0x8001AC88, output);
    cursor = input; destination = output + length;
    value = r_u8(cursor);
    while (value)
    {
        w_u8(destination, value < 0x61 ? value : value - 0x20);
        ++cursor; ++destination;
        if ((sint32)(cursor - input) >= 0x7E) moh_draft_missing_call(0x80015B28, 0x80039DD8, input, 0x80);
        value = r_u8(cursor);
    }
    w_u8(destination, 0);
    moh_draft_missing_call(0x8001ABC8, output, 0x80039E14);
    return output;
}

uint32 level2p_sub_80049F28(uint32 filename)
{
    uint8 path[128];
    uint32 tail, cursor, path_view, record, result, attempts = 0, basename = 0, value;
    FUNCTION_MARKER(0x80049F28, "LEVEL2P.BIN");
    tail = r_u32(0x8008FDF0);
    path_view = moh_draft_local_address(path, sizeof(path));
    cursor = level2p_sub_80049E2C(path_view, filename);
    if (tail)
    {
        for (;;)
        {
            value = r_u32(tail);
            if (!value) break;
            tail = value;
        }
    }
    record = sub_80036BF0(0x1C);
    if (!record)
    {
        moh_draft_missing_call(0x80015B28, 0x80039E18);
        return 0;
    }
    w_u32(record, 0);
    for (;;)
    {
        result = sub_800374EC(record + 4, cursor);
        ++attempts;
        if (result + 1 > 1) break;
        moh_draft_missing_call(0x80015B28, 0x80039E34, path_view, attempts);
        moh_draft_missing_call(0x80015B28, 0x80039E5C, result);
        moh_draft_missing_call(0x8001AD98);
        moh_draft_missing_call(0x80016E38, 0);
        moh_draft_missing_call(0x80015DB8, 0);
        moh_draft_missing_call(0x80015DB8, 0);
        moh_draft_missing_call(0x80015DB8, 0);
    }
    if (r_u32(0x8008FDF0)) w_u32(tail, record);
    else w_u32(0x8008FDF0, record);
    value = r_u8(cursor);
    while (value)
    {
        if (value == 0x5C) basename = cursor + 1;
        ++cursor;
        value = r_u8(cursor);
    }
    /* TODO: Original forwards null basename if the path has no separator */
    moh_draft_missing_call(0x8001AD48, record + 12, basename);
    return record;
}

uint32 level2p_sub_8004A29C(uint32 filename, uint32 slot, uint32 count, uint32 buffer)
{
    uint8 path[128], position[8];
    uint32 record, found = 0, basename = 0, cursor, value, state, size, rounded, position_view, path_view;
    FUNCTION_MARKER(0x8004A29C, "LEVEL2P.BIN");
    record = r_u32(0x8008FDF0);
    path_view = moh_draft_local_address(path, sizeof(path));
    cursor = level2p_sub_80049E2C(path_view, filename);
    while (record && !found)
    {
        value = r_u8(cursor);
        while (value)
        {
            if (value == 0x5C) basename = cursor + 1;
            ++cursor;
            value = r_u8(cursor);
        }
        if (basename && !moh_draft_missing_call(0x8001ACC8, record + 12, basename)) found = 1;
        else record = r_u32(record);
    }
    if (!found)
    {
        record = level2p_sub_80049F28(filename);
        if (!record) return 0;
    }
    state = 0x8008FDF4 + (slot << 5);
    w_u32(state, record + 4);
    if (buffer) w_u32(state + 0x14, buffer);
    else
    {
        size = r_u32(r_u32(state) + 4);
        value = sub_80036BF0(size + 0x7FF);
        w_u32(state + 0x14, value);
    }
    value = r_u32(state + 0x14); w_u32(state + 0x18, value);
    state = 0x8008FDF4 + (slot << 5);
    w_u32(state + 12, 0);
    size = r_u32(r_u32(state) + 4);
    rounded = size + 0x7FF;
    if ((sint32)rounded < 0) rounded = size + 0xFFE;
    w_u32(state + 8, (uint32)((sint32)rounded >> 11));
    w_u32(state + 0x10, count);
    value = r_u32(r_u32(state)); w_u32(state + 4, value);
    value = r_u32(state + 4);
    /* TODO: Original fourth CD position byte remains unspecified at the SDK boundary */
    position_view = moh_draft_local_address(position, sizeof(position));
    sub_8001ADD8(value, position_view);
    w_u8(state + 0x1C, 1);
    w_u32(0x8008FCD0, 0);
    if (slot == 1) level2p_sub_8004A164(position_view, 0x14, 1);
    return r_u32(state + 0x18);
}

void level2p_sub_8004A484(void)
{
    uint8 result[8];
    uint32 result_view, status;
    FUNCTION_MARKER(0x8004A484, "LEVEL2P.BIN");
    result_view = moh_draft_local_address(result, sizeof(result));
    do { status = moh_draft_missing_call(0x800195D0, result_view); } while ((sint32)status > 0);
}


uint32 level2p_sub_8004AB7C(uint32 filename)
{
    uint32 record, basename = 0, cursor, value, length, left, right, index, mismatch, first, second;
    FUNCTION_MARKER(0x8004AB7C, "LEVEL2P.BIN");
    value = r_u8(filename);
    record = r_u32(0x8008FDF0);
    cursor = filename;
    while (value)
    {
        value = r_u8(cursor);
        if (value == 0x5C) basename = cursor + 1;
        ++cursor;
        value = r_u8(cursor);
    }
    while (record)
    {
        left = record + 12; right = basename;
        length = moh_draft_missing_call(0x8001AC88, left);
        index = 0; mismatch = 0;
        /* TODO: Original permits null basename and unsigned length-minus4 underflow */
        if (!(right & 3))
        {
            while (index < length - 4)
            {
                first = r_u32(left); second = r_u32(right);
                if (first != second) { mismatch = 1; break; }
                left += 4; index += 4; right += 4;
            }
        }
        if (!mismatch)
        {
            while (index < length)
            {
                first = r_u8(left); second = r_u8(right);
                ++left;
                if (first != second) { mismatch = 1; break; }
                ++index; ++right;
            }
        }
        if (!mismatch)
        {
            moh_draft_missing_call(0x80015B28, 0x8003A10C, filename);
            return record + 4;
        }
        record = r_u32(record);
    }
    return 0;
}

static void moh_file_read_reset(uint32 diagnostic, uint32 reset_diagnostic, uint32 callback)
{
    moh_draft_missing_call(0x80015B28, diagnostic);
    moh_draft_missing_call(0x8001AD98);
    moh_draft_missing_call(0x80015DB8, 0);
    moh_draft_missing_call(0x80015DB8, 0);
    if (!moh_draft_missing_call(0x80019F64)) moh_draft_missing_call(0x80015B28, reset_diagnostic);
    moh_draft_missing_call(0x80019640, callback);
}

uint32 level2p_sub_8004A4AC(uint32 filename, uint32 size_output, uint32 supplied_buffer)
{
    uint8 path[128];
    uint32 record[2], path_view, record_view, found, attempts, result, size, buffer, sector, sectors, bytes, remaining, destination;
    FUNCTION_MARKER(0x8004A4AC, "LEVEL2P.BIN");
    path_view = moh_draft_local_address(path, sizeof(path));
    sub_800362E8(path_view, filename, 0);
    found = level2p_sub_8004AB7C(path_view);
    if (!found)
    {
        attempts = 1;
        record_view = moh_draft_local_address(record, sizeof(record));
        for (;;)
        {
            result = sub_800374EC(record_view, path_view);
            if (result + 1 > 1) break;
            moh_draft_missing_call(0x80015B28, 0x80039E34, path_view, attempts);
            moh_draft_missing_call(0x80015B28, 0x80039E5C, result);
            moh_draft_missing_call(0x8001AD98);
            moh_draft_missing_call(0x80016E38, 0);
            moh_draft_missing_call(0x80015DB8, 0);
            moh_draft_missing_call(0x80015DB8, 0);
            moh_draft_missing_call(0x80015DB8, 0);
            if (attempts++ >= 0x20) break;
        }
        if (!result) level2p_sub_80049D88();
        if (result == 0xFFFFFFFF && attempts >= 0x20) level2p_sub_80049D88();
        /* TODO: Original continues after exhausted search without guaranteed file-info initialization */
        sub_8001ADD8(record[0], 0x8003859C);
        size = record[1];
    }
    else
    {
        sub_8001ADD8(r_u32(found), 0x8003859C);
        size = r_u32(found + 4);
    }
    buffer = supplied_buffer;
    if (!buffer) buffer = sub_80036BF0(size + 1);
    if (!buffer) goto finish;
    w_u32(0x80038590, buffer);
    sector = moh_draft_missing_call(0x8001AEE8, 0x8003859C);
    /* S5 is initialized to zero and never changed on the normal entry path */
    w_u32(0x80038594, (uint32)((sint32)size >> 11));
    if (r_u32(0x80038594))
    {
        w_u32(0x80038598, 1);
        result = moh_draft_missing_call(0x80019640, 0x800369EC);
        w_u32(0x800385A0, result);
        for (;;)
        {
            sectors = r_u32(0x80038594); destination = r_u32(0x80038590);
            result = moh_draft_missing_call(0x80019220, 0x8003859C, sectors, destination, 0x80);
            if ((sint32)result > 0) break;
            moh_file_read_reset(0x80039E9C, 0x80039ECC, 0x800369EC);
        }
        while (r_u32(0x80038598)) { }
        sectors = r_u32(0x80038594); result = r_u32(0x80038594);
        bytes = sectors << 11;
        sub_8001ADD8(sector + result, 0x8003859C);
        remaining = size - bytes;
        if (remaining)
        {
            w_u32(0x80038598, 1);
            result = moh_draft_missing_call(0x80019640, 0x80036A70);
            w_u32(0x800385A0, result);
            for (;;)
            {
                result = moh_draft_missing_call(0x80019220, 0x8003859C, 1, 0x80037D90, 0x80);
                if ((sint32)result > 0) break;
                moh_file_read_reset(0x80039F04, 0x80039F34, 0x80036A70);
            }
            while (r_u32(0x80038598)) { }
            destination = r_u32(0x80038590) + bytes;
            w_u32(0x80038590, destination);
            moh_draft_missing_call(0x800154EC, destination, 0x80037D90, remaining);
        }
    }
    else
    {
        w_u32(0x80038598, 1);
        result = moh_draft_missing_call(0x80019640, 0x80036A70);
        w_u32(0x800385A0, result);
        do { result = moh_draft_missing_call(0x80019220, 0x8003859C, 1, 0x80037D90, 0x80); } while ((sint32)result <= 0);
        /* Original executes this diagnostic/reset sequence after successful small reads */
        moh_file_read_reset(0x80039F6C, 0x80039F9C, 0x80036A70);
        while (r_u32(0x80038598)) { }
        destination = r_u32(0x80038590);
        moh_draft_missing_call(0x800154EC, destination, 0x80037D90, size);
    }
    if (buffer) w_u8(buffer + size, 0);
finish:
    if (size_output) w_u32(size_output, size);
    return buffer;
}


uint32 level2p_sub_8004ACE8(uint32 sizes, uint32 selected)
{
    uint32 count, cursor, allocation;
    FUNCTION_MARKER(0x8004ACE8, "LEVEL2P.BIN");
    w_u32(selected, 0);
    count = r_u32(sizes);
    if ((sint32)count <= 0) return 0;
    cursor = sizes + (count << 2);
    do
    {
        allocation = sub_80036BF0(r_u32(cursor));
        if (allocation)
        {
            w_u32(selected, count);
            return allocation;
        }
        --count; cursor -= 4;
    } while ((sint32)count > 0);
    return 0;
}

static void moh_sized_read_retry(uint32 path, uint32 attempt, uint32 status)
{
    moh_draft_missing_call(0x80015B28, 0x80039E34, path, attempt);
    moh_draft_missing_call(0x80015B28, 0x80039E5C, status);
    moh_draft_missing_call(0x8001AD98);
    moh_draft_missing_call(0x80016E38, 0);
    moh_draft_missing_call(0x80015DB8, 0);
    moh_draft_missing_call(0x80015DB8, 0);
    moh_draft_missing_call(0x80015DB8, 0);
}

uint32 level2p_sub_8004AD5C(uint32 filename, uint32 selected)
{
    uint32 table[512], record[2], buffer = 0, path_view, record_view, table_view, position_view, status_view;
    uint32 attempt, result, status, index, sectors;
    uint8 path[128], position[8], status_bytes[8];
    FUNCTION_MARKER(0x8004AD5C, "LEVEL2P.BIN");
    w_u32(selected, 0);
    path_view = moh_draft_local_address(path, sizeof(path));
    sub_800362E8(path_view, filename, 0);
    record_view = moh_draft_local_address(record, sizeof(record));
    attempt = 1;
    for (;;)
    {
        result = sub_800374EC(record_view, path_view);
        if (result + 1 > 1) break;
        moh_sized_read_retry(path_view, attempt, result);
        if (attempt++ >= 0x20)
        {
            moh_draft_missing_call(0x80015B28, 0x8003A128, path_view, attempt);
            goto failure;
        }
    }
    attempt = 0;
    /* TODO: Original fourth CD-position byte is unspecified at the SDK boundary */
    position_view = moh_draft_local_address(position, sizeof(position));
    sub_8001ADD8(record[0], position_view);
    table_view = moh_draft_local_address(table, sizeof(table));
    status_view = moh_draft_local_address(status_bytes, sizeof(status_bytes));
    while (attempt++ < 0x20)
    {
        status = 1;
        result = moh_draft_missing_call(0x80019220, position_view, 1, table_view, 0x80);
        if (result)
        {
            do { status = moh_draft_missing_call(0x800195D0, status_view); } while ((sint32)status > 0);
            if (!status) break;
        }
        moh_sized_read_retry(path_view, attempt, status);
    }
    if (attempt >= 0x20) goto failure;
    buffer = level2p_sub_8004ACE8(table_view, selected);
    if (!buffer) goto failure;
    attempt = 0;
    sub_8001ADD8(record[0] + 1, position_view);
    while (attempt++ < 0x20)
    {
        status = 1;
        index = r_u32(selected);
        /* TODO: Original does not bound the selected index to the header sector */
        sectors = r_u32(table_view + (index << 2)) >> 11;
        result = moh_draft_missing_call(0x80019220, position_view, sectors, buffer, 0x80);
        if (result)
        {
            do { status = moh_draft_missing_call(0x800195D0, status_view); } while ((sint32)status > 0);
            if (!status) break;
        }
        moh_sized_read_retry(path_view, attempt, status);
    }
    if (attempt < 0x20) return buffer;
failure:
    moh_draft_missing_call(0x80015B28, 0x8003A150);
    if (buffer) sub_80036E44(buffer);
    return 0;
}


void level2p_sub_8004B01C(void)
{
    static const uint32 offsets[10] = { 8, 0, 4, 9, 10, 15, 11, 12, 14, 13 };
    uint32 mission, level, music, volume, difficulty, life, perfect, stereo, parameter, index, value;
    FUNCTION_MARKER(0x8004B01C, "LEVEL2P.BIN");
    sub_8001AF68();
    w_u8(0x8008FCDC, 0); w_u8(0x8008FCE2, 1);
    mission = r_u32(0x80038660);
    w_u8(0x8008FCE4, 3); w_u8(0x8008FCE5, 0);
    w_u32(0x8008FCD4, mission + 1);
    level = r_u32(0x80038664); music = r_u8(0x8003864C); volume = r_u32(0x80038654);
    w_u32(0x8008FCD8, level + 1);
    w_u8(0x8008FCDD, music);
    difficulty = r_u8(0x800386DC); life = r_u8(0x80038668); perfect = r_u8(0x800386DE);
    w_u8(0x8008FCDE, volume * 42); w_u8(0x8008FCDF, difficulty);
    w_u8(0x8008FCE0, life); w_u8(0x8008FCE1, perfect);
    stereo = r_u32(0x80038650); parameter = r_u8(0x800386AC);
    w_u8(0x8008FCE3, stereo == 0); w_u8(0x8008FCE6, parameter);
    moh_draft_missing_call(0x80015B28, 0x8003A18C);
    for (index = 0; index < 10; ++index)
    {
        if (index == 1 || index == 2) value = r_u32(0x8008FCD4 + offsets[index]);
        else value = r_u8(0x8008FCD4 + offsets[index]);
        moh_draft_missing_call(0x80015B28, 0x8003A1B8 + index * 12, value);
    }
}

void level2p_sub_8004B180(uint32 owner)
{
    uint32 index, offset, pointer, related;
    FUNCTION_MARKER(0x8004B180, "LEVEL2P.BIN");
    if (r_u16(owner + 0x36))
    {
        index = 0; offset = 0;
        do
        {
            pointer = r_u32(owner + 0x304);
            ++index;
            w_u32(pointer + offset, 0);
            offset += 0x48;
        } while (index < r_u16(owner + 0x36));
    }
    if (!(r_u32(owner + 12) & 1)) return;
    related = r_u32(r_u32(owner + 0x388) + 0xAC);
    if (r_u16(related + 0xBE))
    {
        index = 0; offset = 0;
        do
        {
            pointer = r_u32(related + 0xC0);
            ++index;
            w_u32(pointer + offset, 0);
            offset += 0x48;
        } while (index < r_u16(related + 0xBE));
    }
}

static void moh_inverse_rotation_controls(uint32 owner)
{
    sint16 angles[3];
    uint32 matrix[5] = { 0 }, angles_view, matrix_view, index;
    angles[0] = (sint16)(0 - (uint32)(sint32)r_s16(owner + 0x22A));
    angles[1] = (sint16)(0 - (uint32)(sint32)r_s16(owner + 0x22E));
    angles[2] = (sint16)(0 - (uint32)(sint32)r_s16(owner + 0x232));
    angles_view = moh_draft_local_address(angles, sizeof(angles));
    matrix_view = moh_draft_local_address(matrix, sizeof(matrix));
    sub_8001AF98(angles_view, matrix_view);
    /* SDK writes18 bytes; RT33 discards the final word's padding */
    for (index = 0; index < 5; ++index) xport_gte_write_control(index, matrix[index]);
}

static void moh_rotation_result_store(uint32 output)
{
    uint32 x, y, z;
    x = xport_gte_read_data(25); y = xport_gte_read_data(26); z = xport_gte_read_data(27);
    w_u16(output, x); w_u16(output + 2, y); w_u16(output + 4, z);
}

void level2p_sub_8004B820(uint32 owner, uint32 input, uint32 output)
{
    uint32 value;
    FUNCTION_MARKER(0x8004B820, "LEVEL2P.BIN");
    moh_inverse_rotation_controls(owner);
    value = r_u32(input); xport_gte_write_data(0, value);
    value = r_u32(input + 4); xport_gte_write_data(1, value);
    xport_gte_execute(0x486012);
    moh_rotation_result_store(output);
}

void level2p_sub_8004B8F8(uint32 owner, uint32 first, uint32 first_output, uint32 second, uint32 second_output, uint32 third, uint32 third_output)
{
    uint32 inputs[3], outputs[3], index, position, value, x, y, z;
    FUNCTION_MARKER(0x8004B8F8, "LEVEL2P.BIN");
    inputs[0] = first; inputs[1] = second; inputs[2] = third;
    outputs[0] = first_output; outputs[1] = second_output; outputs[2] = third_output;
    moh_inverse_rotation_controls(owner);
    for (index = 0; index < 3; ++index)
    {
        position = (uint32)(sint32)r_s16(owner + 0x20E); value = r_u16(inputs[index]); x = value - position;
        position = (uint32)(sint32)r_s16(owner + 0x212); value = r_u16(inputs[index] + 2); y = value - position;
        position = (uint32)(sint32)r_s16(owner + 0x216); value = r_u16(inputs[index] + 4); z = value - position;
        xport_gte_write_data(0, (x & 0xFFFF) | (y << 16));
        /* VZ0 discards the original undefined high half */
        xport_gte_write_data(1, (uint32)(sint32)(sint16)z);
        xport_gte_execute(0x486012);
        moh_rotation_result_store(outputs[index]);
    }
}


uint32 level2p_sub_8004BB48(uint32 start, uint32 end, uint32 owner, uint32 best, uint32 point_output, uint32 normal_output, uint32 flags_output)
{
    uint8 first[8], second[8], unused[8], point[8];
    uint32 distances[32], first_view, second_view, unused_view, point_view, distance_view, planes, plane, other;
    uint32 mask = 0, hit = 0, index, inner, first_word, second_word, third_word, x, y, z, delta[3];
    uint32 first_distance, second_distance, value, offset, divisor, numerator, quotient, reject, cx, cy, cz;
    FUNCTION_MARKER(0x8004BB48, "LEVEL2P.BIN");
    w_u32(best, 0x7FFFFFFF);
    planes = r_u32(owner + 0x300);
    /* TODO: Original vector padding and unused third input remain unspecified */
    first_view = moh_draft_local_address(first, sizeof(first));
    second_view = moh_draft_local_address(second, sizeof(second));
    unused_view = moh_draft_local_address(unused, sizeof(unused));
    level2p_sub_8004B8F8(owner, start, first_view, end, second_view, unused_view, unused_view);
    x = r_u16(first_view); value = r_u16(second_view); y = r_u16(second_view + 2);
    delta[0] = (uint16)(value - x);
    value = r_u16(first_view + 2); z = r_u16(second_view + 4); offset = r_u16(first_view + 4);
    delta[2] = (uint16)(z - offset); delta[1] = (uint16)(y - value);
    distance_view = moh_draft_local_address(distances, sizeof(distances));
    for (index = 0; index < r_u16(owner + 0x2EE); ++index)
    {
        plane = planes + index * 28;
        first_word = r_u32(first_view); second_word = r_u32(first_view + 4);
        xport_gte_write_control(0, first_word); xport_gte_write_control(1, second_word);
        first_word = r_u32(second_view); second_word = r_u32(second_view + 4);
        xport_gte_write_control(3, first_word); xport_gte_write_control(4, second_word);
        x = r_u16(plane + 4); y = r_u16(plane + 6); z = r_u16(plane + 8);
        xport_gte_write_data(9, x); xport_gte_write_data(10, y); xport_gte_write_data(11, z);
        xport_gte_execute(0x41E012);
        first_word = xport_gte_read_data(25); second_word = xport_gte_read_data(27);
        offset = (uint32)(sint32)r_s16(plane + 10);
        first_distance = (uint32)((sint32)first_word >> 15) - offset;
        /* TODO: Original plane count is not bounded to the32-entry distance buffer */
        w_u32(distance_view + index * 8, first_distance);
        offset = (uint32)(sint32)r_s16(plane + 10);
        second_distance = (uint32)((sint32)second_word >> 15) - offset;
        w_u32(distance_view + index * 8 + 4, second_distance);
        if ((sint32)(first_distance ^ second_distance) < 0) mask |= (uint32)1 << (index & 31);
    }
    point_view = moh_draft_local_address(point, sizeof(point));
    for (index = 0; index < r_u16(owner + 0x2EE); ++index)
    {
        plane = planes + index * 28;
        if (!(r_u32(plane) & 3)) continue;
        if (!(mask & ((uint32)1 << (index & 31)))) continue;
        first_distance = r_u32(distance_view + index * 8);
        if ((sint32)first_distance < 0) continue;
        if ((sint32)first_distance >= (sint32)r_u32(best)) continue;
        numerator = first_distance << 12;
        second_distance = r_u32(distance_view + index * 8 + 4);
        if ((sint32)second_distance < 0) second_distance = 0 - second_distance;
        divisor = first_distance + second_distance;
        if (!divisor) { moh_draft_fail_fast("Original intersection division BREAK 0x1C00", 0x8004BD88); return 0; }
        if (divisor == 0xFFFFFFFF && numerator == 0x80000000) { moh_draft_fail_fast("Original intersection division BREAK 0x1800", 0x8004BDA0); return 0; }
        quotient = (uint32)((sint32)numerator / (sint32)divisor);
        xport_gte_write_data(8, quotient);
        xport_gte_write_data(9, delta[0]); xport_gte_write_data(10, delta[1]); xport_gte_write_data(11, delta[2]);
        xport_gte_execute(0x198003D);
        x = xport_gte_read_data(9); y = xport_gte_read_data(10); z = xport_gte_read_data(11);
        w_u16(point_view, x); w_u16(point_view + 2, y); w_u16(point_view + 4, z);
        x = r_u16(point_view); cx = r_u16(first_view); cy = r_u16(first_view + 2); cz = r_u16(first_view + 4);
        cx = (uint16)(x + cx); w_u16(point_view, cx);
        y = r_u16(point_view + 2); z = r_u16(point_view + 4);
        cy = (uint16)(y + cy); cz = (uint16)(z + cz);
        w_u16(point_view + 2, cy); w_u16(point_view + 4, cz);
        reject = 0;
        for (inner = 0; inner < r_u16(owner + 0x2EE); ++inner)
        {
            if (inner == index) continue;
            value = (uint32)1 << (inner & 31);
            if (!(r_u32(plane + 12) & value) && !(r_u32(plane + 20) & value)) continue;
            other = planes + inner * 28;
            first_word = r_u32(other + 4); second_word = r_u32(other + 8);
            xport_gte_write_control(0, first_word); xport_gte_write_control(1, second_word);
            xport_gte_write_data(9, cx); xport_gte_write_data(10, cy); xport_gte_write_data(11, cz);
            xport_gte_execute(0x41E012);
            value = xport_gte_read_data(25); offset = (uint32)(sint32)r_s16(other + 10);
            if ((sint32)((uint32)((sint32)value >> 15) - offset) >= 0)
            {
                if (inner < r_u16(owner + 0x2EE)) reject = 1;
                break;
            }
        }
        if (reject) continue;
        value = r_u32(distance_view + index * 8);
        if ((sint32)value < 0) value = 0 - value;
        w_u32(best, value);
        first_word = r_u32(point_view); second_word = r_u32(point_view + 4);
        w_u32(point_output, first_word); w_u32(point_output + 4, second_word);
        first_word = r_u32(plane + 4); second_word = r_u32(plane + 8);
        w_u32(normal_output, first_word); w_u32(normal_output + 4, second_word);
        value = r_u32(plane); hit = 1; w_u32(flags_output, value);
    }
    if (hit)
    {
        moh_camera_load_matrix(owner + 0x340);
        value = r_u32(point_output); xport_gte_write_data(0, value);
        value = r_u32(point_output + 4); xport_gte_write_data(1, value);
        xport_gte_execute(0x480012);
        x = xport_gte_read_data(25); y = xport_gte_read_data(26); z = xport_gte_read_data(27);
        value = r_u32(normal_output); xport_gte_write_data(0, value);
        value = r_u32(normal_output + 4); xport_gte_write_data(1, value);
        xport_gte_execute(0x486012);
        w_u16(point_output, x); w_u16(point_output + 2, y); w_u16(point_output + 4, z);
        first_word = xport_gte_read_data(25); second_word = xport_gte_read_data(26); third_word = xport_gte_read_data(27);
        w_u16(normal_output, first_word); w_u16(normal_output + 2, second_word); w_u16(normal_output + 4, third_word);
    }
    return hit;
}

uint32 level2p_sub_8004C07C(uint32 moving, uint32 owner, uint32 best, uint32 point, uint32 normal, uint32 flags)
{
    uint8 first[8], second[8];
    uint32 first_view, second_view, x, y, z;
    FUNCTION_MARKER(0x8004C07C, "LEVEL2P.BIN");
    if (!r_u16(owner + 0x2EE)) return 0;
    if (r_u32(moving + 0x18) & 1) return 0;
    first_view = moh_draft_local_address(first, sizeof(first));
    second_view = moh_draft_local_address(second, sizeof(second));
    x = (uint32)(sint32)r_s16(moving + 0x25A); y = r_u32(moving + 0x25C); z = r_u32(moving + 0x260);
    w_u16(first_view, x); w_u16(first_view + 4, z >> 16); w_u16(first_view + 2, y >> 16);
    x = (uint32)(sint32)r_s16(moving + 0x20E); y = r_u32(moving + 0x210); z = r_u32(moving + 0x214);
    w_u16(second_view + 2, y >> 16); w_u16(second_view, x); w_u16(second_view + 4, z >> 16);
    /* TODO: Preserve original unspecified vector padding through the local adapter */
    return level2p_sub_8004BB48(first_view, second_view, owner, best, point, normal, flags);
}
