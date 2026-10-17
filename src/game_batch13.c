#include "game_level2p_signatures.h"
#include "xport_trace.h"
#include "psx.h"

static void moh_attachment_load_matrix(uint32 matrix)
{
    uint32 first, second, third;
    first = r_u32(matrix); second = r_u32(matrix + 4);
    xport_gte_write_control(0, first); xport_gte_write_control(1, second);
    first = r_u32(matrix + 8); second = r_u32(matrix + 12); third = r_u32(matrix + 16);
    xport_gte_write_control(2, first); xport_gte_write_control(3, second); xport_gte_write_control(4, third);
    first = r_u32(matrix + 20); second = r_u32(matrix + 24);
    xport_gte_write_control(5, first); third = r_u32(matrix + 28);
    xport_gte_write_control(6, second); xport_gte_write_control(7, third);
}

static void moh_attachment_difference(uint32 destination, uint32 first_point, uint32 second_point)
{
    uint32 x, first, y, z, second, third;
    x = r_u16(second_point); first = r_u16(first_point); y = r_u16(first_point + 2); z = r_u16(first_point + 4);
    w_u16(destination, x - first);
    second = r_u16(second_point + 2); third = r_u16(second_point + 4);
    w_u16(destination + 2, second - y); w_u16(destination + 4, third - z);
}

static void moh_attachment_cross(uint32 axis, uint32 vector, uint32 output)
{
    uint32 x, y, z;
    x = (uint32)(sint32)r_s16(axis); y = (uint32)(sint32)r_s16(axis + 2);
    xport_gte_write_control(0, x); z = (uint32)(sint32)r_s16(axis + 4);
    xport_gte_write_control(2, y); xport_gte_write_control(4, z);
    x = (uint32)(sint32)r_s16(vector); y = (uint32)(sint32)r_s16(vector + 2); z = (uint32)(sint32)r_s16(vector + 4);
    xport_gte_write_data(9, x); xport_gte_write_data(10, y); xport_gte_write_data(11, z);
    xport_gte_execute(0x178000C);
    x = xport_gte_read_data(9); y = xport_gte_read_data(10); z = xport_gte_read_data(11);
    w_u16(output, x); w_u16(output + 2, y); w_u16(output + 4, z);
}

void level2p_sub_80051840(uint32 owner, uint32 model, uint32 attachment, uint32 bone)
{
    uint8 matrix[32], first_point[8], second_point[8], primary[8], secondary[8], cross[8], position[12], projected[12], rotated[12];
    uint32 matrix_view, first_view, second_view, primary_view, secondary_view, cross_view, position_view, projected_view, rotated_view;
    uint32 first, second, third, value, pointer, offset, a, b, c, d, twice_a, twice_b, twice_d, d_square, c_square;
    uint32 result_x, result_y, result_z, values[9], index;
    FUNCTION_MARKER(0x80051840, "LEVEL2P.BIN");
    if (!attachment) return;
    first = r_u32(attachment + 0x3C); second = r_u32(attachment + 0x40); third = r_u32(attachment + 0x44);
    w_u32(attachment + 0x48, first); w_u32(attachment + 0x4C, second); w_u32(attachment + 0x50, third);
    if (r_u32(model + 0x84))
    {
        value = r_u32(owner + 0x228); w_u32(attachment + 0x60, value);
        value = r_u32(owner + 0x22C); w_u32(attachment + 0x64, value);
        value = r_u32(owner + 0x230); w_u32(attachment + 0x68, value);
        pointer = r_u32(model + 0x88); offset = bone << 3;
        if (!pointer) return;
        first = r_u32(pointer + offset); second = r_u32(pointer + offset + 4);
        a = (uint32)(sint32)(sint16)first; b = (uint32)((sint32)first >> 16);
        c = (uint32)(sint32)(sint16)second; d = (uint32)((sint32)second >> 16);
        xport_gte_write_control(0, (d << 16) | (a & 0xFFFF));
        value = (b << 16) | (c & 0xFFFF);
        xport_gte_write_control(1, value); xport_gte_write_control(3, value);
        xport_gte_write_control(2, ((0 - c) & 0xFFFF) | (d << 16)); xport_gte_write_control(4, 0 - a);
        twice_a = a << 1; twice_b = b << 1; twice_d = d << 1;
        xport_gte_write_data(0, (d << 17) | (twice_a & 0xFFFF)); xport_gte_write_data(1, 0);
        xport_gte_execute(0x486012); d_square = d * d;
        result_x = xport_gte_read_data(25); result_y = xport_gte_read_data(26); result_z = xport_gte_read_data(27);
        xport_gte_write_data(0, twice_b & 0xFFFF); xport_gte_write_data(1, twice_d);
        xport_gte_execute(0x486012);
        /* TODO: Matrix padding remains opaque through the fail-fast local adapter */
        matrix_view = moh_draft_local_address(matrix, sizeof(matrix));
        w_u16(matrix_view, result_x - 0x1000); w_u16(matrix_view + 6, result_y); w_u16(matrix_view + 12, result_z);
        result_x = xport_gte_read_data(25); result_y = xport_gte_read_data(26); result_z = xport_gte_read_data(27);
        c_square = c * c;
        xport_gte_write_data(0, (0 - twice_b) << 16); xport_gte_write_data(1, twice_a);
        xport_gte_execute(0x486012);
        value = (((uint32)((sint32)c_square >> 12) + (uint32)((sint32)d_square >> 12)) << 1) - 0x1000;
        w_u16(matrix_view + 2, result_x); w_u16(matrix_view + 8, result_y - 0x1000); w_u16(matrix_view + 14, result_z);
        first = xport_gte_read_data(25); second = xport_gte_read_data(26);
        w_u16(matrix_view + 4, first); w_u16(matrix_view + 10, second); w_u16(matrix_view + 16, value);
        pointer = r_u32(model + 0x94); value = (uint32)(sint32)r_s16(pointer + offset); w_u32(matrix_view + 20, value);
        pointer = r_u32(model + 0x94); value = (uint32)(sint32)r_s16(pointer + offset + 2); w_u32(matrix_view + 24, value);
        pointer = r_u32(model + 0x94); value = (uint32)(sint32)r_s16(pointer + offset + 4); w_u32(matrix_view + 28, value);
        moh_attachment_load_matrix(matrix_view);
        first = r_u32(attachment + 0x14); xport_gte_write_data(0, first);
        second = r_u32(attachment + 0x18); xport_gte_write_data(1, second); xport_gte_execute(0x480012);
        projected_view = moh_draft_local_address(projected, sizeof(projected)); rotated_view = moh_draft_local_address(rotated, sizeof(rotated));
        first = xport_gte_read_data(25); w_u32(projected_view, first);
        second = xport_gte_read_data(26); w_u32(projected_view + 4, second);
        third = xport_gte_read_data(27); w_u32(projected_view + 8, third);
        moh_draft_missing_call(0x800163E8, owner + 0x340, projected_view, rotated_view);
        moh_draft_missing_call(0x8001BD08, owner + 0x340, matrix_view);
        first = r_u32(rotated_view); second = r_u32(owner + 0x354); first += second;
        value = r_u32(rotated_view + 4); w_u32(projected_view, first);
        second = r_u32(owner + 0x358); value += second; w_u32(projected_view + 4, value);
        third = r_u32(rotated_view + 8); second = r_u32(owner + 0x35C); third += second; w_u32(projected_view + 8, third);
        w_u32(attachment + 0x3C, first << 16);
        second = r_u32(projected_view + 4); w_u32(attachment + 0x40, second << 16);
        third = r_u32(projected_view + 8); w_u32(attachment + 0x44, third << 16);
    }
    else
    {
        first_view = moh_draft_local_address(first_point, sizeof(first_point));
        level2p_sub_8008BCF0(owner, model, bone, first_view, 0, 0);
        value = (uint32)(sint32)r_s16(first_view); w_u32(attachment + 0x3C, value << 16);
        value = (uint32)(sint32)r_s16(first_view + 2); w_u32(attachment + 0x40, value << 16);
        value = (uint32)(sint32)r_s16(first_view + 4); w_u32(attachment + 0x44, value << 16);
        value = r_u32(owner + 0x228); w_u32(attachment + 0x60, value);
        value = r_u32(owner + 0x22C); w_u32(attachment + 0x64, value);
        value = r_u32(owner + 0x230); w_u32(attachment + 0x68, value);
        if (!bone) return;
        second_view = moh_draft_local_address(second_point, sizeof(second_point));
        primary_view = moh_draft_local_address(primary, sizeof(primary)); secondary_view = moh_draft_local_address(secondary, sizeof(secondary));
        cross_view = moh_draft_local_address(cross, sizeof(cross)); matrix_view = moh_draft_local_address(matrix, sizeof(matrix));
        value = (uint32)(sint32)r_s16(attachment + 0x70); level2p_sub_8008BCF0(owner, model, value, first_view, 0, 0);
        value = (uint32)(sint32)r_s16(attachment + 0x72); level2p_sub_8008BCF0(owner, model, value, second_view, 0, 0);
        moh_attachment_difference(primary_view, first_view, second_view); moh_draft_missing_call(0x80016A40, primary_view, primary_view);
        value = (uint32)(sint32)r_s16(attachment + 0x6C); level2p_sub_8008BCF0(owner, model, value, first_view, 0, 0);
        value = (uint32)(sint32)r_s16(attachment + 0x6E); level2p_sub_8008BCF0(owner, model, value, second_view, 0, 0);
        moh_attachment_difference(secondary_view, first_view, second_view); moh_draft_missing_call(0x80016A40, secondary_view, secondary_view);
        moh_attachment_cross(primary_view, secondary_view, cross_view); moh_attachment_cross(primary_view, cross_view, secondary_view);
        for (index = 0; index < 3; ++index) values[index] = r_u16(secondary_view + index * 2);
        for (index = 0; index < 3; ++index) values[index + 3] = r_u16(cross_view + index * 2);
        for (index = 0; index < 3; ++index) values[index + 6] = r_u16(primary_view + index * 2);
        w_u16(matrix_view, values[0]); w_u16(matrix_view + 6, values[1]); w_u16(matrix_view + 12, values[2]);
        w_u16(matrix_view + 2, values[3]); w_u16(matrix_view + 8, values[4]); w_u16(matrix_view + 14, values[5]);
        w_u16(matrix_view + 4, values[6]); w_u16(matrix_view + 10, values[7]); w_u16(matrix_view + 16, values[8]);
        position_view = moh_draft_local_address(position, sizeof(position));
        value = (uint32)(sint32)r_s16(attachment + 0x3E); w_u32(position_view, value);
        value = (uint32)(sint32)r_s16(attachment + 0x42); w_u32(position_view + 4, value);
        value = (uint32)(sint32)r_s16(attachment + 0x46); w_u32(position_view + 8, value);
        moh_draft_missing_call(0x80016218, matrix_view, position_view);
        value = r_u32(attachment + 0x24); w_u32(position_view, (uint32)((sint32)value >> 4));
        value = r_u32(attachment + 0x28); w_u32(position_view + 4, (uint32)((sint32)value >> 4));
        value = r_u32(attachment + 0x2C); w_u32(position_view + 8, (uint32)((sint32)value >> 4));
        moh_draft_missing_call(0x800165D8, matrix_view, position_view);
    }
    moh_draft_missing_call(0x800154EC, attachment + 0x74, matrix_view, 32);
}

static uint32 moh_firing_timer(uint32 table, uint32 range_offset, uint32 byte_offset)
{
    uint32 range, base, random;
    range = r_u32(table + range_offset); base = r_u8(table + byte_offset);
    if (range)
    {
        random = sub_8001552C(); range = r_u32(table + range_offset);
        base += (uint32)((sint32)(random * range) >> 15);
    }
    return base;
}

static uint32 moh_firing_angle(uint32 angle, uint32 spread, uint32 narrow)
{
    uint32 range = (uint32)(sint32)(sint16)spread, delta, random;
    if (narrow)
    {
        delta = (uint32)((sint32)range >> 1);
        if (range)
        {
            random = sub_8001552C(); delta = (uint32)((sint32)(0 - range) >> 1) + (uint32)((sint32)(random * range) >> 15);
        }
    }
    else
    {
        delta = 0 - range;
        if (range << 1)
        {
            random = sub_8001552C(); delta = (uint32)((sint32)(random * (range << 1)) >> 15) - range;
        }
    }
    return angle + (delta << 16);
}

static void moh_firing_point(uint32 target, uint32 output)
{
    uint32 first, second, value;
    value = (uint32)(sint32)r_s16(target + 0x20E); w_u16(output, value);
    first = r_u32(target + 0x2D4); second = r_u32(target + 0x210);
    value = second - (uint32)((sint32)first >> 1); w_u16(output + 2, (uint32)((sint32)value >> 16));
    value = (uint32)(sint32)r_s16(target + 0x216); w_u16(output + 4, value);
}

static uint32 moh_firing_spawn(uint32 owner, uint32 table, uint32 point, uint32 model, uint32 resource, uint32 special)
{
    uint32 first, second, damage;
    if (special)
    {
        first = r_u32(table + 8); second = r_u32(table + 12);
        return level2p_sub_8004609C(owner, first, second, point, 0, 0, 0, 0x13F1, 0x800, 0x10000, 0x08000451);
    }
    damage = r_u8(resource + 100) << 16; first = r_u32(table + 8); second = r_u32(table + 12);
    return level2p_sub_80045E68(owner, first, second, point, model, 0, 0, damage, 0, 0, 0x13F4, 0x800, 0, 0x08000401);
}

static void moh_firing_sound(uint32 owner, uint32 table, uint32 force_special)
{
    uint32 choice, bank, pointer, flags;
    choice = r_u32(table + 0x28);
    if ((sint32)choice < 0) return;
    bank = r_u32(table + 0x2C);
    if ((sint32)bank < 0)
    {
        pointer = r_u32(owner + 0x100); pointer = r_u32(pointer + 8); bank = r_u16(pointer + 200);
    }
    flags = force_special ? 3 : (r_u32(table + 4) == 0x1773 ? 3 : 0);
    level2p_sub_800420C4((uint32)(sint32)(sint16)bank, choice, owner, 0x10000, 0, 0x10000, 0x1800, 0, 0, flags, 0);
}

void level2p_sub_80051F80(uint32 owner, uint32 target, uint32 kind, uint32 model)
{
    uint8 point[8];
    uint32 data, pointer, resource, spread, mode, table, value, cached, first, second, special, projectile, shot_data, point_view;
    sint32 angle;
    FUNCTION_MARKER(0x80051F80, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388); pointer = r_u32(owner + 0x1C); first = r_u32(owner + 0x38C);
    mode = r_u8(data + 0x11); resource = r_u32(pointer + 4); spread = r_u16(first + 200);
    table = 0x800942D0 + (kind & 0xFF) * 48;
    if (mode == 0)
    {
        value = r_u8(data + 0x14); value = value ? value - 1 : 0;
        first = r_u8(data + 0x13); w_u8(data + 0x14, value);
        if (!first)
        {
            value = r_u8(data + 0x12); if (value) w_u8(data + 0x12, value - 1);
        }
        goto finish;
    }
    if (mode == 2)
    {
        /* TODO: Original target-point padding stays opaque at the fail-fast local adapter */
        point_view = moh_draft_local_address(point, sizeof(point)); moh_firing_point(target, point_view);
        special = r_u32(table + 4) == 0x1773;
        projectile = moh_firing_spawn(owner, table, point_view, model, resource, special);
        if (special)
        {
            if (projectile)
            {
                value = r_u32(projectile + 0x228); w_u32(projectile + 0x228, moh_firing_angle(value, spread, 0));
                value = r_u32(projectile + 0x22C); w_u32(projectile + 0x22C, moh_firing_angle(value, spread, 0));
                shot_data = r_u32(projectile + 0x388); w_u32(projectile + 0x200, 0x820000);
                value = r_u16(shot_data + 0x3A); w_u16(shot_data + 0x3A, value << 1);
                moh_firing_sound(owner, table, 1);
            }
        }
        else if (projectile)
        {
            level2p_sub_80051E2C(projectile, owner);
            pointer = r_u32(0x8009943C); pointer = r_u32(pointer + 0x388); first = r_u32(pointer);
            value = r_u32(projectile + 0x228); w_u32(projectile + 0x228, moh_firing_angle(value, spread, (first & 2) != 0));
            value = r_u32(projectile + 0x22C); w_u32(projectile + 0x22C, moh_firing_angle(value, spread, 0));
        }
        level2p_sub_80068E58(data + 0xB68); w_u8(data + 0x11, 0);
        goto finish;
    }
    if (mode != 1 && mode != 3 && mode != 4) goto finish;
    cached = r_u8(data + 0x12);
    if (!cached)
    {
        value = moh_firing_timer(table, 0x24, 0x20); w_u8(data + 0x12, value);
        value = moh_firing_timer(table, 0x1C, 0x18); w_u8(data + 0x13, value); w_u8(data + 0x14, 0);
        goto finish;
    }
    value = r_u8(data + 0x14);
    if (value) { w_u8(data + 0x14, value - 1); goto finish; }
    if (!r_u8(data + 0x13)) { w_u8(data + 0x12, cached - 1); goto finish; }
    value = moh_firing_timer(table, 0x14, 0x10); w_u8(data + 0x14, value);
    first = level2p_sub_8003FB68(owner, target); second = level2p_sub_8003FB68(owner, target);
    angle = (sint16)second; if ((sint16)first < 0) angle = -angle;
    if (angle < 350) mode = r_u8(data + 0x11);
    else
    {
        mode = r_u8(data + 0x11);
        if (mode != 4)
        {
            if (mode != 3) goto finish;
            mode = r_u8(data + 0x11);
        }
    }
    point_view = 0;
    if (mode != 3)
    {
        point_view = moh_draft_local_address(point, sizeof(point)); moh_firing_point(target, point_view);
    }
    special = r_u32(table + 4) == 0x1773;
    projectile = moh_firing_spawn(owner, table, point_view, model, resource, special);
    if (special)
    {
        if (mode == 3)
        {
            if (projectile)
            {
                shot_data = r_u32(projectile + 0x388); w_u32(projectile + 0x200, 0x960000);
                value = r_u16(shot_data + 0x3A); w_u16(shot_data + 0x3A, value << 1);
            }
        }
        else
        {
            /* TODO: Original aimed special branch dereferences a failed creation result */
            value = r_u32(projectile + 0x228); w_u32(projectile + 0x228, moh_firing_angle(value, spread, 0));
            value = r_u32(projectile + 0x22C); value = moh_firing_angle(value, spread, 0);
            shot_data = r_u32(projectile + 0x388); w_u32(projectile + 0x22C, value);
            w_u32(projectile + 0x200, 0x960000);
            value = r_u16(shot_data + 0x3A); w_u16(shot_data + 0x3A, value << 1);
        }
    }
    else if (projectile)
    {
        level2p_sub_80051E2C(projectile, owner);
        if (mode != 3)
        {
            pointer = r_u32(0x8009943C); pointer = r_u32(pointer + 0x388); first = r_u32(pointer);
            if (first & 2)
            {
                value = sub_8001552C(); value = (uint32)((sint32)(value * 40) >> 15) - 20;
                first = r_u32(projectile + 0x228); w_u32(projectile + 0x228, first + (value << 16));
                value = sub_8001552C(); value = (uint32)((sint32)(value * 180) >> 15) - 90;
                first = r_u32(projectile + 0x22C); w_u32(projectile + 0x22C, first + (value << 16));
            }
            else
            {
                value = r_u32(projectile + 0x228); w_u32(projectile + 0x228, moh_firing_angle(value, spread, 0));
                value = r_u32(projectile + 0x22C); w_u32(projectile + 0x22C, moh_firing_angle(value, spread, 0));
            }
        }
    }
    level2p_sub_80068E58(data + 0xB68); moh_firing_sound(owner, table, 0);
    value = r_u8(data + 0x13) - 1; w_u8(data + 0x13, value);
    if (!(value & 0xFF))
    {
        value = r_u32(owner + 0x3C); w_u32(owner + 0x3C, value | 1); w_u8(data + 0x13, 0);
    }
finish:
    value = r_u8(data + 0x11); w_u8(data + 0x10, value);
}

void level2p_sub_80053548(void)
{
    uint32 resource, count, row = 0, entry, value, total, frame, table, current;
    FUNCTION_MARKER(0x80053548, "LEVEL2P.BIN");
    resource = r_u32(0x80098FB0); count = r_u32(resource + 0xD8);
    if (!count) return;
    do
    {
        entry = resource + 0xDC + row * 20;
        if (r_u32(entry))
        {
            value = r_u32(entry + 16) - 1; w_u32(entry + 16, value);
            if ((sint32)value <= 0)
            {
                frame = r_u32(entry + 8); total = r_u32(entry); ++frame; w_u32(entry + 8, frame);
                if (frame >= total) w_u32(entry + 8, 0);
                frame = r_u32(entry + 8); table = r_u32(entry + 4);
                value = r_u8(table + (frame << 2) + 3); w_u32(entry + 16, value);
                frame = r_u32(entry + 8); value = r_u32(table + (frame << 2)); w_u32(entry + 12, value);
            }
        }
        current = r_u32(0x80098FB0); count = r_u32(current + 0xD8); ++row;
    } while (row < count);
}

void level2p_sub_80053688(void)
{
    FUNCTION_MARKER(0x80053688, "LEVEL2P.BIN");
    level2p_sub_80053548();
}

void level2p_sub_80053748(uint32 index)
{
    uint32 slot;
    FUNCTION_MARKER(0x80053748, "LEVEL2P.BIN");
    slot = 0x80094C50 + (index << 2);
    if (r_u32(slot) != 0xFFFFFFFF) level2p_sub_8008AE58(index);
    w_u32(slot, 0xFFFFFFFF);
}

void level2p_sub_8004A0BC(void)
{
    uint32 current, previous = 0, value;
    FUNCTION_MARKER(0x8004A0BC, "LEVEL2P.BIN");
    current = r_u32(0x8008FDF0);
    if (!current) return;
    if (r_u32(current))
    {
        do
        {
            do { previous = current; current = r_u32(current); value = r_u32(current); } while (value);
            sub_80036E44(current);
            if (previous) w_u32(previous, 0);
            current = r_u32(0x8008FDF0); value = r_u32(current); previous = current;
        } while (value);
    }
    current = r_u32(0x8008FDF0); sub_80036E44(current); w_u32(0x8008FDF0, 0);
}

void level2p_sub_80053798(uint32 owner)
{
    uint32 index = 0, count;
    FUNCTION_MARKER(0x80053798, "LEVEL2P.BIN");
    count = r_u32(0x80094C64);
    if ((sint32)count > 0)
    {
        do
        {
            level2p_sub_80053748(index); count = r_u32(0x80094C64); ++index;
        } while ((sint32)index < (sint32)count);
    }
    level2p_sub_8004A0BC(); level2p_sub_800653D8(owner);
}

void level2p_sub_800536A8(uint32 owner, uint32 view)
{
    uint32 index = 0, count, table, buffer;
    FUNCTION_MARKER(0x800536A8, "LEVEL2P.BIN");
    level2p_sub_8008BA7C(owner); count = r_u32(0x80094C64);
    if ((sint32)count > 0)
    {
        do
        {
            if (index != r_u32(0x800A94C8))
            {
                table = r_u32(view + 0x2218); buffer = r_u32(view + 0x221C);
                level2p_sub_8008A440(0x8009506C + index * 0xB0, table, buffer, index);
            }
            count = r_u32(0x80094C64); ++index;
        } while ((sint32)index < (sint32)count);
    }
    w_u32(0x800A94A8, 0); level2p_sub_8008A62C();
}

void level2p_sub_8008A560(uint32 packet, uint32 u, uint32 v, uint32 width, uint32 height, uint32 clut_x, uint32 clut_y)
{
    uint32 left, right, value;
    FUNCTION_MARKER(0x8008A560, "LEVEL2P.BIN");
    height = (uint16)height; clut_x = (uint16)clut_x; clut_y = (uint16)clut_y;
    w_u8(packet + 3, 9); w_u8(packet + 7, 0x2C);
    w_u8(packet + 4, 0x50); w_u8(packet + 5, 0x50); w_u8(packet + 6, 0x50);
    left = (u & 0x3F) << 2; right = ((u + width) & 0x3F) << 2;
    w_u8(packet + 12, left); w_u8(packet + 28, left); w_u8(packet + 13, v);
    w_u8(packet + 20, right); w_u8(packet + 21, v); w_u8(packet + 36, right);
    w_u8(packet + 29, v + height); w_u8(packet + 37, v + height);
    value = moh_draft_missing_call(0x8001B968, 0, 0, u & 0xFFC0, v & 0xFF00); w_u16(packet + 22, value);
    value = moh_draft_missing_call(0x8001BA18, clut_x, clut_y); w_u16(packet + 14, value);
}

static uint32 moh_effect_clip_code(sint32 x, sint32 y, sint32 width, sint32 height)
{
    uint32 code = x < 0 ? 1 : (x >= width ? 2 : 0);
    if (y < 0) code += 4; else if (y >= height) code += 8;
    return code;
}

void level2p_sub_8008A62C(void)
{
    uint32 buffer, table, width, height, index, record, flags, packet, bit, i, value, first, second, third, depth, slot;
    uint32 u, v, texture_width, texture_height, clut_x, clut_y, code0, code1, code2;
    sint32 a, b, c, minimum, maximum, x0, y0, x1, y1, x2, y2;
    FUNCTION_MARKER(0x8008A62C, "LEVEL2P.BIN");
    buffer = r_u32(0x80098B88); table = r_u32(0x80098B84); width = r_u32(0x80098B74); height = r_u32(0x80098B78);
    w_u32(0x1F8003CC, 0x7FF98000); w_u32(0x1F8003BC, table); w_u32(0x1F800398, width); w_u32(0x1F80039C, height);
    for (index = 0; index < 100; ++index)
    {
        record = 0x8008FF3C + (index << 7); flags = r_u32(record);
        if (!(flags & 1)) continue;
        for (i = 0; i < 6; ++i) { value = r_u32(record + 4 + i * 4); xport_gte_write_data(i, value); }
        xport_gte_execute(0x280030); packet = record + buffer * 40 + 0x30; bit = buffer ? 4 : 2;
        if (flags & bit)
        {
            texture_height = r_u16(record + 0x2A); u = r_u16(record + 0x24); v = r_u16(record + 0x26); texture_width = r_u16(record + 0x28);
            clut_x = r_u16(record + 0x2C); clut_y = r_u16(record + 0x2E);
            level2p_sub_8008A560(packet, u, v, texture_width, texture_height, clut_x, clut_y);
            w_u32(record, r_u32(record) & ~bit);
        }
        flags = xport_gte_read_control(31); xport_gte_execute(0x1400006);
        if ((sint32)flags < 0 && (flags & r_u32(0x1F8003CC))) continue;
        if ((sint32)xport_gte_read_data(24) <= 0) continue;
        value = xport_gte_read_data(12); w_u32(0x1F8003A0, value);
        value = xport_gte_read_data(13); w_u32(0x1F8003A4, value);
        value = xport_gte_read_data(14); w_u32(0x1F8003A8, value);
        a = (sint32)xport_gte_read_data(17); b = (sint32)xport_gte_read_data(18); c = (sint32)xport_gte_read_data(19);
        minimum = a < b ? a : b; if (c < minimum) minimum = c;
        maximum = a < b ? b : a; if (maximum < c) maximum = c;
        depth = (uint32)((sint32)((uint32)maximum * 3 + (uint32)minimum) >> 4);
        if ((sint32)depth < 16)
        {
            a = (sint32)xport_gte_read_data(17); b = (sint32)xport_gte_read_data(18); c = (sint32)xport_gte_read_data(19);
            maximum = a < b ? b : a; if (maximum < c) maximum = c;
            if (maximum < 10) continue;
        }
        if ((depth >> 2) >= 0x200) continue;
        x0 = r_s16(0x1F8003A0); y0 = r_s16(0x1F8003A2); x1 = r_s16(0x1F8003A4); y1 = r_s16(0x1F8003A6);
        x2 = r_s16(0x1F8003A8); y2 = r_s16(0x1F8003AA); width = r_u32(0x1F800398); height = r_u32(0x1F80039C);
        w_u32(0x1F80037C, depth);
        code0 = moh_effect_clip_code(x0, y0, (sint32)width, (sint32)height);
        if (code0)
        {
            code1 = moh_effect_clip_code(x1, y1, (sint32)width, (sint32)height);
            if (code1)
            {
                code2 = moh_effect_clip_code(x2, y2, (sint32)width, (sint32)height);
                if (code2 && (code0 & code1 & code2)) continue;
            }
        }
        value = r_u32(record + 0x1C); xport_gte_write_data(0, value);
        value = r_u32(record + 0x20); xport_gte_write_data(1, value); xport_gte_execute(0x180001);
        first = r_u32(0x1F8003A0); second = r_u32(0x1F8003A4); third = r_u32(0x1F8003A8);
        w_u32(packet + 8, first); w_u32(packet + 16, second); w_u32(packet + 24, third);
        value = r_u32(0x1F80037C) - 35; w_u32(0x1F80037C, value);
        if ((sint32)value < 0) w_u32(0x1F80037C, 35);
        value = xport_gte_read_data(14); w_u32(packet + 32, value);
        depth = r_u32(0x1F80037C); table = r_u32(0x1F8003BC); slot = table + ((uint32)((sint32)depth >> 2) << 2);
        first = r_u32(packet); second = r_u32(slot); w_u32(packet, (first & 0xFF000000) | (second & 0xFFFFFF));
        depth = r_u32(0x1F80037C); table = r_u32(0x1F8003BC); slot = table + ((uint32)((sint32)depth >> 2) << 2);
        first = r_u32(slot); w_u32(slot, (first & 0xFF000000) | (packet & 0xFFFFFF));
    }
}

uint32 level2p_sub_8008A440(uint32 resource, uint32 ordering_table, uint32 buffer, uint32 index)
{
    uint32 limit_output, limit_view, cursor, limit, data, node;
    FUNCTION_MARKER(0x8008A440, "LEVEL2P.BIN");
    /* TODO: Addressable local output requires the permitted fail-fast adapter */
    limit_view = moh_draft_local_address(&limit_output, sizeof(limit_output));
    cursor = level2p_sub_8005365C(buffer, limit_view);
    w_u32(0x800A94D8, 0); w_u32(0x800A94DC, 0); w_u32(0x800A94E0, 0);
    limit = r_u32(limit_view); w_u32(0x800AA484, 0); limit -= 40;
    /* Original8001535C only swaps SP; native branch storage needs no guest stack */
    data = r_u32(resource + 8); node = r_u32(data + 0x14);
    if (r_u32(0x800A94A4)) sub_80013698(node);
    else level2p_sub_80089E98(node);
    if (r_u32(0x800A94A0)) cursor = level2p_sub_80088818(cursor, limit, resource, ordering_table, index);
    else cursor = sub_800126A8(cursor, limit, resource, ordering_table);
    level2p_sub_8005367C(cursor);
    return r_u32(0x80094C38);
}

static void moh_debug_triangle_input(uint32 vertices)
{
    uint32 first, second, third, value;
    first = vertices + r_u16(0x1F8003EE) * 8; second = vertices + r_u16(0x1F8003F0) * 8;
    third = vertices + r_u16(0x1F8003F2) * 8;
    value = r_u32(first); xport_gte_write_data(0, value); value = r_u32(first + 4); xport_gte_write_data(1, value);
    value = r_u32(second); xport_gte_write_data(2, value); value = r_u32(second + 4); xport_gte_write_data(3, value);
    value = r_u32(third); xport_gte_write_data(4, value); value = r_u32(third + 4); xport_gte_write_data(5, value);
}

static uint32 moh_debug_triangle_visible(uint32 flags)
{
    uint32 value, depth, near_depth, mask = 0, width, height, code0, code1, code2;
    sint32 a, b, c, minimum, maximum, x0, y0, x1, y1, x2, y2;
    if ((sint32)flags < 0 && (flags & r_u32(0x1F8003CC))) return 0;
    if ((sint32)xport_gte_read_data(24) <= 0) return 0;
    value = xport_gte_read_data(12); w_u32(0x1F8003A0, value);
    value = xport_gte_read_data(13); w_u32(0x1F8003A4, value);
    value = xport_gte_read_data(14); w_u32(0x1F8003A8, value);
    a = (sint32)xport_gte_read_data(17); b = (sint32)xport_gte_read_data(18); c = (sint32)xport_gte_read_data(19);
    minimum = a < b ? a : b; if (c < minimum) minimum = c;
    maximum = a < b ? b : a; if (maximum < c) maximum = c;
    depth = (uint32)((sint32)((uint32)maximum * 3 + (uint32)minimum) >> 4); near_depth = r_u32(0x1F80038C) << 2;
    w_u32(0x1F800380, (uint32)a); w_u32(0x1F800384, (uint32)b); w_u32(0x1F800388, (uint32)c);
    if (a < (sint32)near_depth) { mask |= 1; if (a < 200) mask |= 8; }
    if (b < (sint32)near_depth) { mask |= 2; if (b < 200) mask |= 16; }
    if (c < (sint32)near_depth) { mask |= 4; if (c < 200) mask |= 32; }
    if ((sint32)depth < 16)
    {
        a = (sint32)xport_gte_read_data(17); b = (sint32)xport_gte_read_data(18); c = (sint32)xport_gte_read_data(19);
        maximum = a < b ? b : a; if (maximum < c) maximum = c;
        if (maximum < 10) return 0;
    }
    if ((depth >> 2) >= 512) return 0;
    x0 = r_s16(0x1F8003A0); y0 = r_s16(0x1F8003A2); x1 = r_s16(0x1F8003A4);
    w_u32(0x1F80037C, depth);
    y1 = r_s16(0x1F8003A6); x2 = r_s16(0x1F8003A8); y2 = r_s16(0x1F8003AA);
    width = r_u32(0x1F800398); height = r_u32(0x1F80039C);
    code0 = moh_effect_clip_code(x0, y0, (sint32)width, (sint32)height);
    if (code0)
    {
        code1 = moh_effect_clip_code(x1, y1, (sint32)width, (sint32)height);
        if (code1)
        {
            code2 = moh_effect_clip_code(x2, y2, (sint32)width, (sint32)height);
            if (code2 && (code0 & code1 & code2)) return 0;
        }
    }
    return mask ? mask << 1 : 1;
}

static void moh_debug_triangle_color(uint32 cursor, uint32 node_slot, uint32 mode, uint32 index)
{
    uint32 i, node, bits, wanted, found = 0, bit = 0, selected = 0, quotient, remainder, color;
    if (mode == 1)
    {
        w_u32(cursor + 4, 0); w_u32(cursor + 12, 0); w_u32(cursor + 20, 0); w_u32(cursor + 28, 0);
        if (index == 0 || index == 3) for (i = 0; i < 4; ++i) w_u8(cursor + 4 + i * 8, 255);
        if (index == 1 || index == 3) for (i = 0; i < 4; ++i) w_u8(cursor + 5 + i * 8, 255);
        if (index == 2) for (i = 0; i < 4; ++i) w_u8(cursor + 6 + i * 8, 255);
        return;
    }
    node = r_u32(node_slot); bits = r_u32(node + (mode == 2 ? 16 : 20)); color = 0xFFFFFF00;
    if (bits)
    {
        wanted = ((r_u32(0x800968E4) >> 5) & 31) + 1;
        do
        {
            if (bits & (1u << bit)) { ++found; selected = bit; }
            bit = (bit + 1) & 31;
        } while (found < wanted);
        w_u8(cursor + 4, selected); w_u8(cursor + 7, 0);
        quotient = (selected + 1) / 6; remainder = (selected + 1) % 6;
        w_u8(cursor + 6, quotient * 51); w_u8(cursor + 5, remainder * 51); color = r_u32(cursor + 4);
        if (color != selected + (remainder * 51 << 8) + (quotient * 51 << 16))
            moh_draft_missing_call(0x80015B28, 0x8003D830);
    }
    w_u32(cursor + 4, color); w_u32(cursor + 12, color); w_u32(cursor + 20, color); w_u32(cursor + 28, color);
}

uint32 level2p_sub_80088818(uint32 cursor, uint32 limit, uint32 resource, uint32 ordering_table, uint32 index)
{
    uint32 total, row = 0, level, focus, near_depth, vertices, data, fog, colors, width, height, node_slot, node, triangle, remaining;
    uint32 value, pair, z, flags, visible, first, second, third, color0, color1, color2, mode, depth, table, slot;
    FUNCTION_MARKER(0x80088818, "LEVEL2P.BIN");
    total = r_u32(0x800A94E0); level = r_u32(0x80098FB0); focus = r_u32(0x80099438); near_depth = r_u32(0x800A94B0);
    vertices = r_u32(resource + 0x2C); data = r_u32(resource + 8); fog = (uint32)(sint32)r_s16(level + 0xCC); colors = r_u32(data + 0x18);
    w_u32(0x1F8003BC, ordering_table); width = r_u32(0x80098B74); height = r_u32(0x80098B78);
    w_u32(0x1F8003C8, level + 0xDC); w_u32(0x1F8003CC, 0x7FF98000);
    w_u32(0x1F8003F4, 0); w_u32(0x1F8003F8, 0); w_u32(0x1F8003FC, focus);
    w_u32(0x1F800398, width); w_u32(0x1F80039C, height); w_u32(0x1F80038C, near_depth); w_u32(0x1F8003C4, fog);
    /* TODO: Original packet admission only compares the cursor, without reserving 40 bytes */
    while (row < total && cursor < limit)
    {
        node_slot = 0x800A94E4 + (row << 2); node = r_u32(node_slot); triangle = r_u32(node + 24);
        pair = r_u32(triangle); z = (uint32)(sint32)r_s16(triangle + 4);
        w_u32(0x1F8003EE, pair); w_u16(0x1F8003F2, z); xport_gte_read_control(31);
        moh_debug_triangle_input(vertices); node = r_u32(node_slot); remaining = r_u32(node + 12);
        value = r_u32(0x1F8003F8); w_u32(0x1F8003F8, value + remaining); xport_gte_execute(0x280030);
        while ((sint32)remaining > 0 && cursor < limit)
        {
            --remaining; pair = r_u32(0x1F8003EE); z = (uint32)(sint32)r_s16(0x1F8003F2);
            w_u32(0x1F8003E8, pair); w_u16(0x1F8003EC, z);
            if ((sint32)remaining > 0) triangle += 16;
            data = r_u32(resource + 0x2C); flags = xport_gte_read_control(31); xport_gte_execute(0x1400006);
            pair = r_u32(triangle); z = (uint32)(sint32)r_s16(triangle + 4);
            w_u32(0x1F8003EE, pair); w_u16(0x1F8003F2, z); moh_debug_triangle_input(data);
            visible = moh_debug_triangle_visible(flags);
            if (!visible) { xport_gte_execute(0x280030); continue; }
            value = r_u32(0x1F8003F4); first = r_u16(0x1F8003E8); second = r_u16(0x1F8003EA);
            value += 1; second = colors + second * 4;
            first = colors + first * 4; pair = r_u32(0x1F8003A4); z = r_u32(0x1F8003A8);
            w_u32(0x1F8003F4, value); third = r_u16(0x1F8003EC);
            color0 = r_u32(first); color1 = r_u32(second); color2 = r_u32(colors + third * 4); value = r_u32(0x1F8003A0);
            w_u32(0x1F8003B0, color0); w_u32(0x1F8003B4, color1); w_u32(0x1F8003B8, color2);
            w_u32(cursor + 8, value); w_u32(cursor + 32, value); w_u32(cursor + 16, pair); w_u32(cursor + 24, z);
            mode = r_u32(0x800A94A0); moh_debug_triangle_color(cursor, node_slot, mode, index);
            depth = r_u32(0x1F80037C); table = r_u32(0x1F8003BC); slot = table + ((uint32)((sint32)depth >> 2) << 2);
            first = r_u32(cursor); second = r_u32(slot); w_u32(cursor, (first & 0xFF000000) | second); w_u32(slot, cursor & 0xFFFFFF);
            xport_gte_execute(0x280030); w_u8(cursor + 3, 9); w_u8(cursor + 7, 0x5C); w_u32(cursor + 36, 0x55555555);
            w_u8(cursor + 23, 0); w_u8(cursor + 31, 0); cursor += 40;
        }
        ++row;
    }
    return cursor;
}

static void moh_core_renderer_ids(uint32 ids[3])
{
    ids[0] = r_u16(0x1F8003C0); ids[1] = r_u16(0x1F8003C2); ids[2] = r_u16(0x1F8003C4);
}

static void moh_core_renderer_initial_input(uint32 vertices, const uint32 ids[3])
{
    uint32 first = vertices + ids[0] * 8, second = vertices + ids[1] * 8, third = vertices + ids[2] * 8;
    uint32 xy, z, value;
    z = r_u32(first + 4); xy = r_u32(first); xport_gte_write_data(1, z);
    value = r_u32(second + 4); xport_gte_write_data(0, xy); xy = r_u32(second); xport_gte_write_data(3, value);
    value = r_u32(third + 4); xport_gte_write_data(2, xy); xy = r_u32(third);
    xport_gte_write_data(5, value); xport_gte_write_data(4, xy);
}

static uint32 moh_core_renderer_fog_amount(uint32 value)
{
    if ((sint32)value < 0) return 0;
    return value < 0x1000 ? value : 0x1000;
}

uint32 sub_800126A8(uint32 cursor, uint32 limit, uint32 resource, uint32 ordering_table)
{
    uint32 color_local[3], ids[3], xy[3], vz[3];
    uint32 remaining_nodes, radius, focus, vertices, value, first, second, third, level, node, triangle, remaining, next, visible;
    uint32 colors, color0, color1, color2, uv, tex, fog, a, b, c, apply_fog, depth, slot, parameter, color_view;
    FUNCTION_MARKER(0x800126A8, "SLUS_009.74");
    /* Original scratch register saves and SP exchange use native local state */
    remaining_nodes = r_u32(0x800A94E0); radius = r_u16(0x800A94D0); focus = r_u32(0x80099438);
    w_u16(0x1F8003B8, 0); w_u16(0x1F8003BA, 0); vertices = r_u32(resource + 0x2C);
    if (focus)
    {
        first = r_u32(focus + 0x20C); second = r_u32(focus + 0x210); third = r_u32(focus + 0x214);
        first = (uint32)((sint32)first >> 16); second = (uint32)((sint32)second >> 16); third = (uint32)((sint32)third >> 16);
        w_u16(0x1F800390, first); w_u16(0x1F800392, second); w_u16(0x1F800394, third);
        w_u16(0x1F800396, first - radius); w_u16(0x1F800398, second - radius); w_u16(0x1F80039A, third - radius);
        w_u16(0x1F80039C, first + radius); w_u16(0x1F80039E, second + radius); w_u16(0x1F8003A0, third + radius);
    }
    value = r_u32(0x800A94B0); w_u32(0x1F8003A4, value);
    first = r_u32(0x80098B74); second = r_u32(0x80098B78); w_u32(0x1F800000, 0x1F800050);
    w_u16(0x1F8003CC, first); w_u16(0x1F8003CE, second); level = r_u32(0x80098FB0);
    /* TODO: Original admission trusts wrapping cursor plus640 and all node/triangle counts */
    for (;;)
    {
        --remaining_nodes;
        if ((sint32)remaining_nodes < 0 || cursor + 640 >= limit) return cursor;
        node = r_u32(0x800A94E4 + (remaining_nodes << 2));
        if (focus) { value = sub_800135F8(node, 0x1F800000); w_u32(0x8001366C, value); }
        triangle = r_u32(node + 24); remaining = r_u32(node + 12);
        if (!remaining) continue;
        value = r_u32(triangle); parameter = r_u16(0x1F8003BA);
        ids[0] = (uint16)value; ids[1] = value >> 16; ids[2] = r_u16(triangle + 4);
        moh_core_renderer_initial_input(vertices, ids); w_u16(0x1F8003BA, parameter + remaining); xport_gte_execute(0x280030);
        while (remaining)
        {
            if (cursor + 640 >= limit) return cursor;
            --remaining; next = remaining ? triangle + 16 : triangle;
            visible = sub_80012D5C(next, vertices, 0x1F800000);
            if (!visible)
            {
                moh_core_renderer_ids(ids); xport_gte_execute(0x280030); triangle = next; continue;
            }
            value = r_u32(resource + 8); w_u16(0x1F8003C6, ids[0]); colors = r_u32(value + 24);
            w_u16(0x1F8003C8, ids[1]); w_u16(0x1F8003CA, ids[2]); color0 = r_u32(colors + ids[0] * 4);
            value = r_u16(0x1F8003B8); color1 = r_u32(colors + ids[1] * 4); w_u16(0x1F8003B8, value + 1);
            color2 = r_u32(colors + ids[2] * 4);
            if (r_u32(level + 0xD8))
            {
                if ((color0 & 0xFF00FF) < 40) { value = r_u32(level + 0xDC + (color0 & 255) * 20 + 12); color0 = (color0 & 0xFF000000) | (value & 0xFFFFFF); }
                if ((color1 & 0xFF00FF) < 40) { value = r_u32(level + 0xDC + (color1 & 255) * 20 + 12); color1 = (color1 & 0xFF000000) | (value & 0xFFFFFF); }
                if ((color2 & 0xFF00FF) < 40) { value = r_u32(level + 0xDC + (color2 & 255) * 20 + 12); color2 = (color2 & 0xFF000000) | (value & 0xFFFFFF); }
            }
            if (focus && (sint32)(r_u32(0x8001366C) - 1) >= 0)
            {
                /* TODO: Register-carried lighting colors require the permitted local address adapter */
                color_view = moh_draft_local_address(color_local, sizeof(color_local));
                w_u32(color_view, color0); w_u32(color_view + 4, color1); w_u32(color_view + 8, color2);
                sub_80012FF8(resource, vertices, color_view, 0x1F800000);
                color0 = r_u32(color_view); color1 = r_u32(color_view + 4); color2 = r_u32(color_view + 8);
            }
            first = r_u32(0x1F80002C); second = r_u32(0x1F800030); third = r_u32(0x1F800034); fog = r_u16(level + 0xCC);
            w_u32(cursor + 8, first); w_u32(cursor + 20, second); w_u32(cursor + 32, third); uv = r_u32(triangle + 8);
            apply_fog = 0;
            if (visible < 2 && fog < (r_u32(0x1F8003A8) << 2))
            {
                a = r_u16(0x1F80001A); b = r_u16(0x1F800022); c = r_u16(0x1F80002A);
                xport_gte_write_data(6, color0); xport_gte_write_data(8, moh_core_renderer_fog_amount((a - fog) << 1)); xport_gte_execute(0x780010);
                color0 = xport_gte_read_data(22); xport_gte_write_data(6, color1);
                xport_gte_write_data(8, moh_core_renderer_fog_amount((b - fog) << 1)); xport_gte_execute(0x780010);
                color1 = xport_gte_read_data(22); xport_gte_write_data(6, color2);
                xport_gte_write_data(8, moh_core_renderer_fog_amount((c - fog) << 1)); apply_fog = 1; xport_gte_execute(0x780010);
            }
            w_u32(cursor + 4, color0); w_u32(cursor + 16, color1); w_u16(cursor + 14, uv); w_u32(0x1F800044, color0);
            if (apply_fog) color2 = xport_gte_read_data(22);
            w_u32(cursor + 28, color2); w_u32(0x1F800048, color1);
            if (visible < 2)
            {
                xport_gte_execute(0x280030); depth = r_u32(0x1F8003A8); slot = ordering_table + ((depth >> 2) << 2);
                value = r_u32(slot); w_u32(cursor, value | 0x09000000); w_u32(slot, cursor & 0xFFFFFF); w_u16(cursor + 24, uv >> 16);
                tex = r_u32(triangle + 12); moh_core_renderer_ids(ids); value = r_u16(triangle + 6);
                w_u16(cursor + 26, tex & 0xFFF); w_u16(cursor + 36, tex >> 16); w_u16(cursor + 12, value);
                w_u8(cursor + 7, (tex & 0x4000) ? 0x36 : 0x34); cursor += 40; triangle = next; continue;
            }
            parameter = r_u32(0x800A94AC); w_u32(0x1F80004C, color2); w_u16(0x1F80003A, uv);
            w_u32(0x1F8003AC, limit); w_u32(0x1F8003B0, resource); w_u32(0x1F8003B4, ordering_table);
            first = vertices + r_u16(0x1F8003C6) * 8; second = vertices + r_u16(0x1F8003C8) * 8;
            third = vertices + r_u16(0x1F8003CA) * 8; value = r_u32(0x1F8003A4);
            a = r_u32(first); w_u32(0x1F800014, a); a = r_u16(first + 4); tex = r_u32(triangle + 12); w_u16(0x1F800018, a);
            a = r_u32(second); w_u16(0x1F80003E, tex & 0xFFF); w_u32(0x1F80001C, a); w_u16(0x1F800040, tex >> 16);
            a = r_u16(second + 4); w_u16(0x1F80003C, uv >> 16); w_u16(0x1F800020, a); a = r_u32(third);
            w_u32(0x1F800024, a); a = r_u16(third + 4); w_u32(0x1F800004, ordering_table); w_u32(0x1F80000C, 0);
            w_u8(0x1F800047, (tex & 0x4000) ? 0x36 : 0x34); w_u16(0x1F800028, a);
            a = r_u16(triangle + 6); w_u32(0x1F800010, value); w_u16(0x1F800038, a);
            cursor = sub_80011B48(0x1F800014, cursor, visible >> 1, parameter - 1);
            limit = r_u32(0x1F8003AC); moh_core_renderer_ids(ids);
            for (a = 0; a < 3; ++a) { first = vertices + ids[a] * 8; xy[a] = r_u32(first); vz[a] = r_u16(first + 4); }
            for (a = 0; a < 3; ++a) { xport_gte_write_data(a * 2, xy[a]); xport_gte_write_data(a * 2 + 1, vz[a]); }
            resource = r_u32(0x1F8003B0); xport_gte_execute(0x280030); ordering_table = r_u32(0x1F8003B4); triangle = next;
        }
    }
}

uint32 level2p_sub_800538C0(void)
{
    uint8 filename[40];
    uint32 owner, flags, resource, value, filename_view = 0, next_id = 1, selected = 0, row = 0, found, identifier, count, size;
    FUNCTION_MARKER(0x800538C0, "LEVEL2P.BIN");
    w_u32(0x80094C40, 0); owner = level2p_sub_80066934(0, 28, 0xFFFFFFFF, 0, 0, 0, 0, 0);
    if (!owner) return 0;
    w_u32(owner + 8, 3); w_u32(owner + 16, 0x203);
    w_u32(owner + 0x36C, 0x80053688); w_u32(owner + 0x370, 0x800536A8); w_u32(owner + 0x384, 0x80053798);
    w_u32(owner + 0x270, 0x10000); w_u32(owner + 0x274, 0x10000); w_u32(owner + 0x278, 0x10000);
    flags = r_u32(owner + 12); resource = r_u32(0x80098FB0);
    w_u32(owner + 0x14, 0); w_u32(owner + 0x374, 0); w_u32(owner + 0x378, 0);
    w_u32(owner + 0x20C, 0); w_u32(owner + 0x210, 0); w_u32(owner + 0x214, 0);
    w_u32(owner + 0x228, 0); w_u32(owner + 0x22C, 0); w_u32(owner + 0x230, 0);
    w_u32(owner + 0x284, 1); w_u32(owner + 12, flags | 0x08020000);
    value = r_u32(resource + 0x98); value = sub_80036BF0(value << 1);
    resource = r_u32(0x80098FB0); w_u32(0x80094C60, value);
    value = r_u32(resource + 0x80); w_u32(0x8009533C, value);
    value = r_u32(resource + 0x84); w_u32(0x80094C64, value);
    /* TODO: Original file search is unbounded and sprintf trusts its40-byte local buffer */
    while ((sint32)row < (sint32)r_u32(0x8009533C))
    {
        if (!filename_view) filename_view = moh_draft_local_address(filename, sizeof(filename));
        do
        {
            identifier = next_id++; resource = r_u32(0x80098FB0);
            moh_draft_missing_call(0x8001BE18, filename_view, 0x8003C090, resource, identifier);
            found = level2p_sub_80049F28(filename_view);
        } while (!found);
        resource = r_u32(0x80098FB0); value = r_u32(resource + 0x88);
        if ((sint32)value < (sint32)next_id)
        {
            count = r_u32(resource + 0x84);
            if ((sint32)selected < (sint32)count) { w_u32(0x80095340 + (selected << 2), next_id - 1); ++selected; }
        }
        size = r_u32(found + 8); value = r_u32(0x80094C40); if (value < size) w_u32(0x80094C40, size);
        ++row;
    }
    count = r_u32(0x80094C64); row = 0;
    if ((sint32)count > 0)
    {
        do
        {
            value = r_u32(0x80094C40); ++row; value = sub_80036BF0(value + 0x7FF);
            w_u32(0x8009532C + ((row - 1) << 2), value); count = r_u32(0x80094C64);
        } while ((sint32)row < (sint32)count);
    }
    return owner;
}

void level2p_sub_80053B04(void)
{
    FUNCTION_MARKER(0x80053B04, "LEVEL2P.BIN");
    w_u32(0x80095350, 0); w_u32(0x80094C44, 0); w_u32(0x80094C48, 0);
}

void level2p_sub_80053DD0(uint32 value)
{
    FUNCTION_MARKER(0x80053DD0, "LEVEL2P.BIN");
    w_u32(0x80095350, value); level2p_sub_80053DF4();
}

void level2p_sub_80053FD8(void)
{
    uint32 index;
    FUNCTION_MARKER(0x80053FD8, "LEVEL2P.BIN");
    for (index = 0; index < 4; ++index) level2p_sub_80053FAC(index);
    index = 98;
    do { w_u8(0x80095D98 + index, 255); --index; } while ((sint32)index >= 0);
}

uint32 level2p_sub_80054510(uint32 owner, uint32 point, uint32 angle_output)
{
    uint8 direction[8], orientation[8];
    uint32 orientation_view, direction_view, first, second, value, heading, angle, difference, chosen, magnitude, cross;
    FUNCTION_MARKER(0x80054510, "LEVEL2P.BIN");
    /* TODO: Addressable semantic vectors require the permitted fail-fast local adapter */
    orientation_view = moh_draft_local_address(orientation, sizeof(orientation));
    direction_view = moh_draft_local_address(direction, sizeof(direction));
    moh_draft_missing_call(0x800158AC, orientation_view, 0, 8);
    value = r_u16(owner + 0x344); w_u16(orientation_view, value);
    value = r_u16(owner + 0x350); w_u16(orientation_view + 4, value);
    first = r_u32(orientation_view); second = r_u32(orientation_view + 4);
    moh_draft_missing_call(0x800158AC, direction_view, 0, 8);
    value = (uint32)(sint32)r_s16(owner + 0x20E); value = r_u16(point) - value; w_u16(direction_view, value);
    value = (uint32)(sint32)r_s16(owner + 0x216); value = r_u16(point + 4) - value; w_u16(direction_view + 4, value);
    value = r_u32(direction_view); cross = r_u32(direction_view + 4); w_u32(orientation_view, value); w_u32(orientation_view + 4, cross);
    value = (uint32)(sint32)r_s16(orientation_view); cross = (uint32)(sint32)r_s16(orientation_view + 4);
    angle = moh_draft_missing_call(0x8001536C, value, cross); heading = r_u32(owner + 0x22C);
    while ((sint32)heading < -(sint32)0x08000000) heading += 0x10000000;
    while ((sint32)heading > (sint32)0x08000000) heading -= 0x10000000;
    difference = (uint32)((sint32)heading >> 16) - angle; chosen = 0x1000 - difference;
    if ((sint32)difference < (sint32)chosen) chosen = difference;
    w_u32(angle_output, chosen << 16); magnitude = (sint32)chosen < 0 ? 0 - chosen : chosen;
    xport_gte_write_control(0, (uint32)(sint32)(sint16)first);
    xport_gte_write_control(2, (uint32)(sint32)(sint16)(first >> 16));
    xport_gte_write_control(4, (uint32)(sint32)(sint16)second);
    value = (uint32)(sint32)r_s16(orientation_view); cross = (uint32)(sint32)r_s16(orientation_view + 2); heading = (uint32)(sint32)r_s16(orientation_view + 4);
    xport_gte_write_data(9, value); xport_gte_write_data(10, cross); xport_gte_write_data(11, heading); xport_gte_execute(0x178000C);
    xport_gte_read_data(9); cross = xport_gte_read_data(10); xport_gte_read_data(11);
    if (magnitude - 0x641 < 0x2BB) return 3;
    if (magnitude - 0x259 >= 0x383) return 0;
    return (sint16)cross < 0 ? 1 : 2;
}

uint32 level2p_sub_80054718(uint32 owner, uint32 target, uint32 limit)
{
    uint8 normal[8], impact[8], fraction[8], bone[8], difference[8];
    uint32 start[3], end[3], data, normal_view, impact_view, fraction_view, bone_view, difference_view, start_view, end_view;
    uint32 first, second, third, value, result;
    FUNCTION_MARKER(0x80054718, "LEVEL2P.BIN");
    data = r_u32(owner + 0x38C);
    /* TODO: Semantic local vectors use fail-fast mapping; original unused padding remains opaque */
    normal_view = moh_draft_local_address(normal, sizeof(normal)); impact_view = moh_draft_local_address(impact, sizeof(impact));
    fraction_view = moh_draft_local_address(fraction, sizeof(fraction)); bone_view = moh_draft_local_address(bone, sizeof(bone));
    difference_view = moh_draft_local_address(difference, sizeof(difference));
    start_view = moh_draft_local_address(start, sizeof(start)); end_view = moh_draft_local_address(end, sizeof(end));
    moh_draft_missing_call(0x800158AC, normal_view, 0, 8);
    first = r_u32(owner + 0x20C); second = r_u32(target + 0x20C); w_u16(difference_view, (uint32)((sint32)(first - second) >> 16));
    first = r_u32(owner + 0x210); second = r_u32(target + 0x210); w_u16(difference_view + 2, (uint32)((sint32)(first - second) >> 16));
    first = r_u32(owner + 0x214); second = r_u32(target + 0x214); w_u16(difference_view + 4, (uint32)((sint32)(first - second) >> 16));
    value = (uint32)(sint32)r_s16(data + 0xAA); level2p_sub_8008C5E4(owner, value, bone_view, 0, 0);
    first = (uint32)(sint32)r_s16(bone_view); third = (uint32)(sint32)r_s16(bone_view + 4); w_u32(start_view, first << 16);
    second = (uint32)(sint32)r_s16(bone_view + 2); w_u32(start_view + 8, third << 16); w_u32(start_view + 4, second << 16);
    value = r_u32(target + 0x20C); w_u32(end_view, value);
    first = r_u32(target + 0x210); second = r_u32(target + 0x2D4); w_u32(end_view + 4, first - second);
    value = r_u32(target + 0x214); w_u32(end_view + 8, value);
    value = level2p_sub_800548C4(difference_view);
    if ((sint32)value >= (sint32)limit) return 0;
    result = level2p_sub_8003F26C(start_view, end_view, normal_view, impact_view, fraction_view, owner + 0x318, 0);
    return result >> 31;
}

uint32 level2p_sub_80054850(uint32 angle)
{
    FUNCTION_MARKER(0x80054850, "LEVEL2P.BIN");
    while ((sint16)angle < -0x800) angle += 0x1000;
    while ((sint16)angle > 0x800) angle -= 0x1000;
    return (uint32)(sint32)(sint16)angle;
}

uint32 level2p_sub_8007D038(uint32 owner)
{
    uint32 target, first, second;
    FUNCTION_MARKER(0x8007D038, "LEVEL2P.BIN");
    target = r_u32(0x8009943C); first = level2p_sub_800517E8(owner, target);
    target = r_u32(0x80099440); second = level2p_sub_800517E8(owner, target);
    if ((sint32)second < (sint32)first) return r_u32(0x80099440);
    return r_u32(0x8009943C);
}

uint32 level2p_sub_80054948(uint32 owner)
{
    uint32 target;
    FUNCTION_MARKER(0x80054948, "LEVEL2P.BIN");
    target = level2p_sub_8007D038(owner); return level2p_sub_80054978(owner, target);
}

static void moh_collision_transform_point(uint32 point)
{
    uint32 x, y, z;
    x = r_u32(point); xport_gte_write_data(0, x); x = r_u32(point + 4); xport_gte_write_data(1, x);
    xport_gte_execute(0x480012);
    x = xport_gte_read_data(25); y = xport_gte_read_data(26); z = xport_gte_read_data(27);
    w_u16(point, x); w_u16(point + 2, y); w_u16(point + 4, z);
}

uint32 level2p_sub_80054978(uint32 owner, uint32 target)
{
    uint8 first[8], second[8], direction[8], delta[8], matrix[32];
    uint32 radius, data, model, value, reference, reference_data, offset, pointer, word0, word1, q0, q1, q2, q3;
    uint32 first_view, second_view, direction_view, delta_view, matrix_view, x, y, z, square2, square3, diagonal, index, a, b, c;
    FUNCTION_MARKER(0x80054978, "LEVEL2P.BIN");
    radius = r_u16(owner + 0x2E8); data = r_u32(target + 0x388);
    if (!radius) { pointer = r_u32(owner + 0x38C); if (pointer) radius = r_u16(pointer + 0xC6); }
    model = r_u32(target + 0x100); value = r_u32(model + 0x84);
    /* TODO: Original vector and matrix padding stays opaque through fail-fast local adapters */
    first_view = moh_draft_local_address(first, sizeof(first)); second_view = moh_draft_local_address(second, sizeof(second));
    direction_view = moh_draft_local_address(direction, sizeof(direction)); delta_view = moh_draft_local_address(delta, sizeof(delta));
    if (value)
    {
        pointer = r_u32(owner + 0x388); data = r_u32(model + 8); value = r_u32(pointer + 0xA0);
        offset = (uint32)(sint32)r_s16(data + 0xD6); reference = 0;
        if (value) reference = r_u32(0x80099460 + (value & 0xFFFF) * 4);
        offset <<= 3; pointer = r_u32(model + 0x88);
        /* TODO: Original quaternion branch dereferences an unresolved or null owner reference */
        reference_data = r_u32(reference + 0x388); pointer += offset;
        word0 = r_u32(pointer); word1 = r_u32(pointer + 4);
        q0 = (uint32)(sint32)(sint16)word0; q1 = (uint32)((sint32)word0 >> 16);
        q2 = (uint32)(sint32)(sint16)word1; q3 = (uint32)((sint32)word1 >> 16);
        xport_gte_write_control(0, (q3 << 16) | (q0 & 0xFFFF));
        value = (q1 << 16) | (q2 & 0xFFFF); xport_gte_write_control(1, value); xport_gte_write_control(3, value);
        xport_gte_write_control(2, ((0 - q2) & 0xFFFF) | (q3 << 16)); xport_gte_write_control(4, 0 - q0);
        xport_gte_write_data(0, (q3 << 17) | ((q0 << 1) & 0xFFFF)); xport_gte_write_data(1, 0); xport_gte_execute(0x486012);
        square3 = q3 * q3; a = xport_gte_read_data(25); b = xport_gte_read_data(26); c = xport_gte_read_data(27);
        xport_gte_write_data(0, (q1 << 1) & 0xFFFF); xport_gte_write_data(1, q3 << 1); xport_gte_execute(0x486012);
        matrix_view = moh_draft_local_address(matrix, sizeof(matrix));
        w_u16(matrix_view, a - 0x1000); w_u16(matrix_view + 6, b); w_u16(matrix_view + 12, c);
        a = xport_gte_read_data(25); b = xport_gte_read_data(26); c = xport_gte_read_data(27); square2 = q2 * q2;
        xport_gte_write_data(0, (0 - (q1 << 1)) << 16); xport_gte_write_data(1, q0 << 1); xport_gte_execute(0x486012);
        diagonal = (((uint32)((sint32)square2 >> 12) + (uint32)((sint32)square3 >> 12)) << 1) - 0x1000;
        w_u16(matrix_view + 2, a); w_u16(matrix_view + 8, b - 0x1000); w_u16(matrix_view + 14, c);
        a = xport_gte_read_data(25); b = xport_gte_read_data(26);
        w_u16(matrix_view + 4, a); w_u16(matrix_view + 10, b); w_u16(matrix_view + 16, diagonal);
        pointer = r_u32(model + 0x94); value = (uint32)(sint32)r_s16(pointer + offset); w_u32(matrix_view + 20, value);
        pointer = r_u32(model + 0x94); value = (uint32)(sint32)r_s16(pointer + offset + 2); w_u32(matrix_view + 24, value);
        pointer = r_u32(model + 0x94); value = (uint32)(sint32)r_s16(pointer + offset + 4); w_u32(matrix_view + 28, value);
        moh_attachment_load_matrix(matrix_view); pointer = r_u32(model + 8);
        value = r_u32(pointer + 0xE0); xport_gte_write_data(0, value); value = r_u32(pointer + 0xE4); xport_gte_write_data(1, value);
        xport_gte_execute(0x480012); x = xport_gte_read_data(25); y = xport_gte_read_data(26); z = xport_gte_read_data(27);
        w_u16(first_view, x); w_u16(first_view + 2, y); w_u16(first_view + 4, z);
        x = r_u16(first_view); z = r_u16(first_view + 4); y = r_u16(first_view + 2);
        w_u16(second_view + 4, z + 100); w_u16(second_view, x); w_u16(second_view + 2, y);
        moh_attachment_load_matrix(target + 0x340); moh_collision_transform_point(first_view);
        value = r_u16(first_view + 2); a = r_u16(reference_data + 0x94); w_u16(first_view + 2, value + a);
        moh_collision_transform_point(second_view);
        value = r_u16(second_view + 2); a = r_u16(reference_data + 0x94); w_u16(second_view + 2, value + a);
    }
    else
    {
        index = r_u8(data + 0x55); value = (uint32)(sint32)r_s16(0x8008F92C + index * 68);
        level2p_sub_8008C5E4(target, value, first_view, 0, 0);
        index = r_u8(data + 0x55); value = (uint32)(sint32)r_s16(0x8008F92E + index * 68);
        level2p_sub_8008C5E4(target, value, second_view, 0, 0);
    }
    moh_attachment_difference(direction_view, first_view, second_view); moh_draft_missing_call(0x80016A40, direction_view, direction_view);
    x = r_u16(target + 0x2E0); w_u16(first_view, x); y = r_u16(target + 0x2E2); w_u16(first_view + 2, y);
    z = r_u16(target + 0x2E4); w_u16(first_view + 4, z);
    a = r_u16(owner + 0x2E0); w_u16(second_view, a); b = r_u16(owner + 0x2E2); w_u16(second_view + 2, b);
    c = r_u16(owner + 0x2E4); w_u16(delta_view, a - x); w_u16(delta_view + 2, b - y);
    w_u16(second_view + 4, c); w_u16(delta_view + 4, c - z);
    moh_attachment_cross(delta_view, direction_view, delta_view);
    x = r_u16(delta_view); y = r_u16(delta_view + 2); z = r_u16(delta_view + 4);
    xport_gte_write_data(9, x); xport_gte_write_data(10, y); xport_gte_write_data(11, z); xport_gte_execute(0xA00428);
    x = xport_gte_read_data(25); y = xport_gte_read_data(26); z = xport_gte_read_data(27);
    return (sint32)(x + y + z) <= (sint32)(radius * radius);
}

uint32 level2p_sub_80054EB0(uint32 owner, uint32 state, uint32 index)
{
    uint8 vector[8], saved[8];
    uint32 resource, entry, x, z, mode, kind, point0, point1, vector_view, saved_view, value, first, second;
    uint32 abs_x, abs_z, shift_x = 0, shift_z = 0, result;
    FUNCTION_MARKER(0x80054EB0, "LEVEL2P.BIN");
    resource = r_u32(0x80099430); resource = r_u32(resource + 0x388); resource = r_u32(resource + 8); entry = resource + index * 52;
    x = (uint32)(sint32)r_s16(entry + 0x24); z = (uint32)(sint32)r_s16(entry + 0x28);
    mode = r_u32(entry + 0x10); kind = r_u32(entry + 12); point0 = r_u32(entry + 0x14); point1 = r_u32(entry + 0x18);
    /* TODO: Addressable difference vectors require the permitted fail-fast local adapter */
    vector_view = moh_draft_local_address(vector, sizeof(vector)); saved_view = moh_draft_local_address(saved, sizeof(saved));
    moh_draft_missing_call(0x800158AC, vector_view, 0, 8);
    value = (uint32)(sint32)r_s16(owner + 0x20E); w_u16(vector_view, (point0 & 0xFFFF) - value);
    value = (uint32)(sint32)r_s16(owner + 0x212); w_u16(vector_view + 2, (point0 >> 16) - value);
    value = (uint32)(sint32)r_s16(owner + 0x216); w_u16(vector_view + 4, (point1 & 0xFFFF) - value);
    first = r_u32(vector_view); second = r_u32(vector_view + 4); w_u32(saved_view, first); w_u32(saved_view + 4, second);
    w_u32(state + 0x8C, 1); w_u16(state + 0xAC, 0); w_u16(state + 0xAE, 0);
    abs_x = (sint32)x < 0 ? 0 - x : x; abs_z = (sint32)z < 0 ? 0 - z : z;
    if (mode == 4) w_u32(state + 0x8C, 0);
    else if (abs_z < abs_x)
    {
        if (x && (mode == 1 || mode == 2))
        {
            if (((sint32)x < 0 && mode == 1) || ((sint32)x > 0 && mode == 2))
            { w_u16(state + 0xAE, 100); shift_z = 0 - 40; }
            else { w_u16(state + 0xAE, 0 - 100); shift_z = 40; }
        }
        else w_u32(state + 0x8C, 0);
    }
    else
    {
        if (z && (mode == 1 || mode == 2))
        {
            if (((sint32)z < 0 && mode == 2) || ((sint32)z > 0 && mode == 1))
            { w_u16(state + 0xAC, 100); shift_x = 0 - 40; }
            else { w_u16(state + 0xAC, 0 - 100); shift_x = 40; }
        }
        else w_u32(state + 0x8C, 0);
    }
    value = level2p_sub_800548C4(saved_view); if ((sint32)value < 250) w_u32(state + 0x8C, 0);
    if (kind == 8) { w_u32(state + 0xA0, point0); w_u32(state + 0xA4, point1); }
    else
    {
        first = (point0 & 0xFFFF) + shift_x - (uint32)((sint32)(x * 5) >> 10); w_u16(state + 0xA0, first);
        w_u16(state + 0xA2, point0 >> 16);
        first = (point1 & 0xFFFF) + shift_z - (uint32)((sint32)(z * 5) >> 10); w_u16(state + 0xA4, first);
        first = (point0 & 0xFFFF) + shift_x; value = r_u16(state + 0xAC);
        w_u16(state + 0xAC, value + first - (uint32)((sint32)(x * 25) >> 12));
        first = (point1 & 0xFFFF) + shift_z; value = r_u16(state + 0xAE);
        w_u16(state + 0xAE, value + first - (uint32)((sint32)(z * 25) >> 12));
    }
    result = r_u32(state + 0x8C); w_u16(state + 0xB6, index);
    return result;
}

uint32 level2p_sub_800551A4(uint32 owner, uint32 kind)
{
    uint8 difference[8];
    uint32 current, best = 0x7FFF, selected = 0, difference_view = 0, first, second, value, distance;
    sint32 vertical;
    FUNCTION_MARKER(0x800551A4, "LEVEL2P.BIN");
    current = r_u32(0x80099434);
    /* TODO: Original linked-list walk has no cycle bound; local padding remains opaque */
    while (current)
    {
        if (r_u32(current + 8) == kind && current != owner && !(r_u32(current + 12) & 0x20080000))
        {
            if (!difference_view) difference_view = moh_draft_local_address(difference, sizeof(difference));
            first = r_u32(owner + 0x20C); second = r_u32(current + 0x20C); value = first - second; w_u16(difference_view, (uint32)((sint32)value >> 16));
            first = r_u32(owner + 0x210); second = r_u32(current + 0x210); value = first - second; w_u16(difference_view + 2, (uint32)((sint32)value >> 16));
            first = r_u32(owner + 0x214); second = r_u32(current + 0x214); value = first - second; w_u16(difference_view + 4, (uint32)((sint32)value >> 16));
            distance = level2p_sub_800548C4(difference_view);
            if ((uint16)distance < (uint16)best)
            {
                vertical = r_s16(difference_view + 2); if (vertical < 0) vertical = -vertical;
                if (vertical < 1000) { selected = current; best = distance; }
            }
        }
        current = r_u32(current + 4);
    }
    return selected;
}

uint32 level2p_sub_80055430(uint32 owner, uint32 flags, uint32 step)
{
    uint8 vector[8], normal[8], impact[8];
    uint32 start[3], end[3], fraction, vector_view, normal_view, impact_view, start_view, end_view, fraction_view;
    uint32 mode, value, height, y, z, result;
    FUNCTION_MARKER(0x80055430, "LEVEL2P.BIN");
    /* TODO: Semantic vectors and ray outputs require the permitted fail-fast local adapter */
    vector_view = moh_draft_local_address(vector, sizeof(vector)); fraction_view = moh_draft_local_address(&fraction, sizeof(fraction));
    moh_draft_missing_call(0x800158AC, vector_view, 0, 8); mode = flags & 15; w_u32(fraction_view, 0);
    switch (mode)
    {
    case 1: w_u16(vector_view, 0 - step); break;
    case 2: w_u16(vector_view, step); break;
    case 4: w_u16(vector_view + 4, step); break;
    case 8: w_u16(vector_view + 4, 0 - step); break;
    default: return 0;
    }
    moh_attachment_load_matrix(owner + 0x340); moh_collision_transform_point(vector_view); xport_gte_read_control(31);
    height = r_u32(owner + 0x2D4); if (!(flags & 16)) height = (uint32)((sint32)height >> 1);
    start_view = moh_draft_local_address(start, sizeof(start)); end_view = moh_draft_local_address(end, sizeof(end));
    normal_view = moh_draft_local_address(normal, sizeof(normal)); impact_view = moh_draft_local_address(impact, sizeof(impact));
    value = r_u32(owner + 0x20C); w_u32(end_view, value);
    y = r_u32(owner + 0x210); value = (uint32)(sint32)r_s16(vector_view); y -= height; w_u32(end_view + 4, y);
    z = r_u32(owner + 0x214); height = (uint32)(sint32)r_s16(vector_view + 4);
    w_u32(start_view, value << 16); w_u32(start_view + 4, y); w_u32(start_view + 8, height << 16); w_u32(end_view + 8, z);
    result = level2p_sub_8003F26C(start_view, end_view, normal_view, impact_view, fraction_view, owner + 0x318, 0);
    if ((sint32)result >= 0) return 0;
    if (r_u32(owner + 16) & 0x200000)
    {
        value = level2p_sub_8003F140(start_view, 0, 1, 0, 0, 1); w_u16(0x800AADC0, value);
        y = (uint32)(sint32)r_s16(owner + 0x212);
        if ((sint32)(y - (uint32)(sint32)(sint16)value) < -400) return 0;
    }
    return step + 1;
}

uint32 level2p_sub_80055660(uint32 identifier)
{
    uint32 count, pointer, index = 0;
    FUNCTION_MARKER(0x80055660, "LEVEL2P.BIN");
    count = r_u32(0x8009A07C); pointer = r_u32(0x8009A080);
    if ((sint32)count > 0)
    {
        do
        {
            if (r_s16(pointer) == (sint16)identifier) return index;
            ++index; pointer += 8;
        } while ((sint32)index < (sint32)count);
    }
    level2p_sub_800885B8(4, 0x40);
    return 0xFFFFFFFF;
}

uint32 level2p_sub_800556C8(uint32 owner, uint32 index)
{
    uint8 difference[8];
    uint32 count, pointer, selected = 0xFFFFFFFF, best = 0x7FFF, row = 0, difference_view, first, second, distance;
    FUNCTION_MARKER(0x800556C8, "LEVEL2P.BIN");
    count = r_u32(0x8009A07C);
    /* TODO: Original index admission permits index equal to table count */
    if ((sint32)count < (sint32)index || (sint32)index < 0) level2p_sub_800885B8(4, 0x40);
    pointer = r_u32(0x8009A080) + (index << 3); count = (uint32)(sint32)r_s16(pointer + 2); pointer = r_u32(pointer + 4);
    if ((sint32)count <= 0) return selected;
    difference_view = moh_draft_local_address(difference, sizeof(difference));
    do
    {
        first = (uint32)(sint32)r_s16(owner + 0x20E); second = r_u16(pointer); w_u16(difference_view, first - second);
        first = (uint32)(sint32)r_s16(owner + 0x212); second = r_u16(pointer + 2); w_u16(difference_view + 2, first - second);
        first = (uint32)(sint32)r_s16(owner + 0x216); second = r_u16(pointer + 4); w_u16(difference_view + 4, first - second);
        distance = level2p_sub_800548C4(difference_view);
        if ((uint16)distance < (uint16)best) { selected = row; best = distance; }
        ++row; pointer += 20;
    } while ((sint32)row < (sint32)count);
    return selected;
}

uint32 level2p_sub_800557D8(uint32 owner, uint32 command)
{
    uint32 packed, model, animation, pointer, header, value, state, end, data, first, second, source, destination, length, index;
    sint32 mode, countdown;
    FUNCTION_MARKER(0x800557D8, "LEVEL2P.BIN");
    packed = r_u16(command); model = r_u32(owner + 0x100); mode = (sint16)packed >> 12; animation = packed & 0xFFF;
    pointer = r_u32(model + 0x78); value = r_u32(pointer + animation * 4);
    if (!value) level2p_sub_800885B8(1, 0x100);
    header = r_u32(0x8009617C); w_u16(header, animation); value = r_u16(command + 4); w_u16(header + 2, value);
    if ((sint16)value < 3) value = 3;
    state = r_u32(0x8009618C); w_u16(header + 4, value); end = (uint32)(sint32)r_s16(command + 6); w_u16(state + 0x18, end);
    if (end == 0xFFFFFFFF)
    {
        if (r_u32(model + 0x74))
        {
            pointer = r_u32(model + 0x78); pointer = r_u32(pointer + animation * 4); value = r_u8(pointer);
        }
        else
        {
            pointer = r_u32(model + 8); pointer = r_u32(pointer + 0x98); value = r_u16(pointer + animation * 16 + 4);
        }
        w_u16(state + 0x18, value - 1);
    }
    else if (end == (uint32)(sint32)r_s16(header + 2)) w_u16(state + 0x18, 0xFFFF);
    if (!r_u32(0x80096178))
    {
        if (mode == 1)
        {
            if (r_u32(model + 0x74))
            {
                pointer = r_u32(owner + 0x100); first = r_u32(pointer + 0x98); second = r_u32(pointer + 0x9C);
                w_u32(pointer + 0xA0, first); w_u32(pointer + 0xA4, second);
                pointer = r_u32(owner + 0x100); source = r_u32(pointer + 0x88);
                if (source)
                {
                    length = r_u16(pointer + 0x72); destination = r_u32(pointer + 0x8C);
                    moh_draft_missing_call(0x800154EC, destination, source, length << 3);
                }
            }
            else
            {
                destination = r_u32(model + 0x30); length = r_u16(model + 0x3C); source = r_u32(model + 0x2C);
                moh_draft_missing_call(0x800154EC, destination, source, length << 3);
            }
            w_u16(model + 0x3E, 4); w_u16(model + 0x40, 0xFFFF);
        }
        else if (mode == 2) w_u16(model + 0x3E, 0);
        else
        {
            countdown = r_s16(model + 0x3E); value = r_u16(model + 0x3E);
            if (countdown > 0) w_u16(model + 0x3E, value - 1);
        }
    }
    /* TODO: Original command trusts animation table entries and dependent resource identifiers */
    if (r_u32(model + 0x74))
    {
        pointer = r_u32(model + 0x78); pointer = r_u32(pointer + animation * 4); value = r_u8(pointer + 2);
        if (value)
        {
            data = r_u32(model + 8); value = r_u32(data + (value - 1) * 4 + 0xEC); index = level2p_sub_80065BF8(value);
            if ((sint32)index < 0) level2p_sub_800885B8(2, 0x18);
            data = level2p_sub_80065BB8(index); value = r_u32(data + 0x38); w_u32(model + 0x7C, value);
            value = r_u32(data + 0x84); w_u32(model + 0x80, value);
        }
        else
        {
            first = r_u32(model + 8); second = r_u32(model + 8); value = r_u32(first + 0x38); w_u32(model + 0x7C, value);
            value = r_u32(second + 0x84); w_u32(model + 0x80, value);
        }
    }
    else
    {
        pointer = r_u32(model + 8); pointer = r_u32(pointer + 0x98); value = r_u16(pointer + animation * 16 + 4);
        if (!value) level2p_sub_800885B8(1, 0x100);
    }
    level2p_sub_8003F508(owner); state = r_u32(0x8009618C); w_u16(state + 0x1C, 0);
    w_u32(owner + 0x248, 0); w_u32(owner + 0x244, 0); w_u32(owner + 0x240, 0); w_u32(0x80096188, 1);
    return command + 8;
}

static void moh_command_mask_update(uint32 owner, uint32 command, uint32 flags, uint32 offset, uint32 set)
{
    uint32 operand, mask, state, value;
    operand = r_u32(command + 4); mask = level2p_sub_8005A984(1, flags, operand, owner);
    state = r_u32(0x8009618C); value = r_u32(state + offset);
    w_u32(state + offset, set ? value | mask : value & ~mask);
}

uint32 level2p_sub_80055B0C(uint32 owner, uint32 command, uint32 flags)
{
    uint32 mode, first, offset;
    FUNCTION_MARKER(0x80055B0C, "LEVEL2P.BIN");
    mode = r_u8(command);
    if (mode >= 5) return command + 8;
    first = mode ? (mode - 1) * 4 : 0;
    moh_command_mask_update(owner, command, flags, first, mode != 0);
    for (offset = 0; offset < 16; offset += 4)
        if (offset != first) moh_command_mask_update(owner, command, flags, offset, 0);
    return command + 8;
}

uint32 level2p_sub_80055E5C(uint32 owner, uint32 command, uint32 flags)
{
    uint32 value, sound, pitch, range, volume, parameter, bank, mode, special, voice, quarter, left, right, gain;
    FUNCTION_MARKER(0x80055E5C, "LEVEL2P.BIN");
    value = r_u8(command); sound = level2p_sub_8005A984(1, flags, value, owner);
    value = r_u32(command + 4); pitch = level2p_sub_8005A984(2, flags, value, owner);
    value = r_u32(command + 8); range = level2p_sub_8005A984(3, flags, value, owner);
    value = r_u32(command + 12); volume = level2p_sub_8005A984(4, flags, value, owner);
    value = r_u32(command + 16); parameter = level2p_sub_8005A984(5, flags, value, owner);
    value = r_u32(command + 20); bank = level2p_sub_8005A984(6, flags, value, owner);
    if (r_u32(0x80096178) || (sint32)sound < 0) return command + 24;
    mode = r_u8(command + 1); special = (mode & 32) != 0;
    if (!(mode & 3))
    {
        if (bank == 0xFFFFFFFF)
        {
            value = r_u32(owner + 0x100); value = r_u32(value + 8); bank = value ? r_u16(value + 200) : 0;
        }
        voice = level2p_sub_800420C4((uint32)(sint32)(sint16)bank, sound, owner, volume, parameter, pitch, range, 0, special, 0, 0);
        w_u32(owner + 0xFC, voice); return command + 24;
    }
    voice = level2p_sub_800420C4(0, sound, owner, volume, parameter, pitch, range, 0, special, 0, 0);
    if ((sint32)voice < 0) return command + 24;
    mode = r_u8(command + 1); quarter = (uint32)((sint32)volume >> 2);
    if (mode & 2)
    {
        left = 0; if (quarter) { value = sub_8001552C(); left = (value * quarter) >> 15; }
        right = 0; if (quarter) { value = sub_8001552C(); right = (value * quarter) >> 15; }
    }
    else { left = quarter; right = quarter; }
    gain = r_u8(0x8008FCDE);
    if (!gain) { left = 0; right = 0; }
    else if (gain == 1)
    {
        left = (uint32)((sint32)(sint16)left >> 1); right = (uint32)((sint32)(sint16)right >> 1);
    }
    else if (gain == 3) { left <<= 1; right <<= 1; }
    if ((sint16)left > 0x3FFF) left = 0x3FFF;
    if ((sint16)right > 0x3FFF) right = 0x3FFF;
    moh_draft_missing_call(0x80015C08, voice, (uint32)(sint32)(sint16)left, (uint32)(sint32)(sint16)right);
    return command + 24;
}

uint32 level2p_sub_80056128(uint32 owner, uint32 command, uint32 flags)
{
    uint32 value;
    FUNCTION_MARKER(0x80056128, "LEVEL2P.BIN");
    value = r_u32(command + 4); value = level2p_sub_8005A984(1, flags, value, owner); w_u32(owner + 0x1FC, value);
    return command + 8;
}

uint32 level2p_sub_80056170(uint32 owner, uint32 command, uint32 flags)
{
    uint32 value, mode;
    FUNCTION_MARKER(0x80056170, "LEVEL2P.BIN");
    if (!(r_u32(command) & 4))
    {
        value = r_u32(command + 4); value = level2p_sub_8005A984(1, flags, value, owner); w_u32(owner + 0x200, value);
    }
    if (!(r_u32(command) & 8))
    {
        value = r_u32(command + 8); value = level2p_sub_8005A984(2, flags, value, owner); w_u32(owner + 0x21C, value);
    }
    mode = r_u32(command); if (mode & 2) return command + 12;
    if (!(mode & 16))
    {
        value = r_u32(command + 12); value = level2p_sub_8005A984(3, flags, value, owner); w_u32(owner + 0x218, value);
    }
    return command + 16;
}

uint32 level2p_sub_80056238(uint32 owner, uint32 command, uint32 flags)
{
    uint32 value, current, maximum;
    FUNCTION_MARKER(0x80056238, "LEVEL2P.BIN");
    if (!(r_u32(command) & 2)) return command + 16;
    value = r_u32(command + 8); value = level2p_sub_8005A984(2, flags, value, owner);
    current = r_u32(owner + 0x21C); w_u32(owner + 0x21C, current + value);
    value = r_u32(command + 4); value = level2p_sub_8005A984(1, flags, value, owner);
    current = r_u32(owner + 0x200); maximum = r_u32(owner + 0x204); current += value;
    if (maximum && (sint32)current >= (sint32)maximum) current = maximum;
    w_u32(owner + 0x200, current); return command + 12;
}

uint32 level2p_sub_800562F0(uint32 owner, uint32 command, uint32 flags)
{
    uint32 mode, subtract = 0, value, current;
    FUNCTION_MARKER(0x800562F0, "LEVEL2P.BIN");
    mode = r_u32(command); if (!(mode & 2)) return command + 16;
    if (mode & 1) subtract = (sint32)r_u32(owner + 0x288) < 0;
    value = r_u32(command + 4); value = level2p_sub_8005A984(1, flags, value, owner);
    current = r_u32(owner + 0x20C); w_u32(owner + 0x20C, subtract ? current - value : current + value);
    value = r_u32(command + 8); value = level2p_sub_8005A984(2, flags, value, owner);
    current = r_u32(owner + 0x210); w_u32(owner + 0x210, current + value); return command + 12;
}

uint32 level2p_sub_80056424(uint32 owner, uint32 command, uint32 flags)
{
    uint32 value, mode;
    FUNCTION_MARKER(0x80056424, "LEVEL2P.BIN");
    value = r_u16(command); value = level2p_sub_8005A984(1, flags, value, owner);
    mode = r_u32(0x80096180); value = level2p_sub_8003F764(owner, (uint32)(sint32)(sint16)value, 0, mode);
    if (!value) return command + 4;
    mode = r_u32(0x80096180); return r_u32(owner + mode * 116 + 0x118);
}

uint32 level2p_sub_8007D408(uint32 owner)
{
    uint32 flags, data, value;
    FUNCTION_MARKER(0x8007D408, "LEVEL2P.BIN");
    flags = r_u32(owner + 12); data = r_u32(owner + 0x388); w_u32(owner + 12, flags | 0x10000000);
    value = r_u32(data + 0x720) + 1; w_u32(data + 0x720, value); return value;
}

uint32 level2p_sub_800564F0(uint32 owner, uint32 command, uint32 flags)
{
    uint32 mode, first = 0, second = 0, third = 0, value;
    FUNCTION_MARKER(0x800564F0, "LEVEL2P.BIN");
    mode = r_u32(command);
    if (mode & 2) first = 0xFFFFFFFF;
    else
    {
        if (mode & 1) { value = r_u32(command + 8); first = level2p_sub_8005A984(2, flags, value, owner); }
        mode = r_u32(command);
    }
    if (mode & 8) second = 0xFFFFFFFF;
    else if (mode & 4) { value = r_u32(command + 12); second = level2p_sub_8005A984(3, flags, value, owner); }
    mode = r_u32(command);
    if (mode & 32) third = 0xFFFFFFFF;
    else if (mode & 16) { value = r_u32(command + 16); third = level2p_sub_8005A984(4, flags, value, owner); }
    value = r_u32(command + 4); value = level2p_sub_8005A984(1, flags, value, owner);
    level2p_sub_8004D148(owner, value, first, second, third << 16);
    if (r_u32(owner + 8) == 1)
    {
        if (first == 9 || first == 5 || first == 6) level2p_sub_8007D408(owner);
        else { value = r_u32(owner + 12); w_u32(owner + 12, value & 0xEFFFFFFF); }
    }
    return command + 20;
}

uint32 level2p_sub_80056674(uint32 owner, uint32 command)
{
    FUNCTION_MARKER(0x80056674, "LEVEL2P.BIN");
    return level2p_sub_800566B4(owner, command);
}

uint32 level2p_sub_80056694(uint32 owner, uint32 command)
{
    FUNCTION_MARKER(0x80056694, "LEVEL2P.BIN");
    return level2p_sub_800566B4(owner, command);
}

uint32 level2p_sub_800566B4(uint32 owner, uint32 command)
{
    static const uint32 match_order[8] = {0, 1, 2, 4, 3, 5, 7, 6};
    uint32 arguments[8] = {0}, words = 1, selected = 0xFFFFFFFF, header, count, index, kind, model, data, state, table, entry, row, type;
    uint32 value, field, payload, payload_type, payload_words;
    FUNCTION_MARKER(0x800566B4, "LEVEL2P.BIN");
    header = r_u32(command); count = r_u32(owner + 0x1EC); kind = header & 255;
    for (index = 0; index < 8; ++index)
        if (header & (1 << index)) { arguments[index] = r_u32(command + words * 4); ++words; }
    model = r_u32(owner + 0x100); data = r_u32(model + 8); value = 0;
    if (data)
    {
        state = r_u32(0x8009618C); field = (uint32)(sint32)r_s16(state + 0x1A); type = (uint32)(sint32)r_s16(data + 0x94);
        value = field == type;
    }
    if (value) { table = r_u32(data + 0xB4); count = r_u32(data + 0xB0); }
    else if (r_u8(command + 3) == 14) table = r_u32(owner + 0x124);
    else { state = r_u32(0x8009618C); table = r_u32(state + 0x20); }
    /* TODO: Original table counts, payload type and copy length are unchecked */
    entry = table;
    for (row = 0; row < count; ++row, entry += 60)
    {
        type = r_u32(entry);
        if (!type && selected == 0xFFFFFFFF) { selected = row; continue; }
        if (type != kind) continue;
        for (index = 0; index < 8; ++index)
        {
            field = match_order[index]; value = r_u32(entry + 8 + field * 4);
            if (value != arguments[field]) break;
        }
        if (index == 8) { selected = row; break; }
    }
    payload = command + words * 4; payload_type = r_u8(payload + 3); payload_words = r_u32(0x8008D170 + payload_type * 4);
    if ((sint32)selected >= 0)
    {
        entry = table + selected * 60; w_u32(entry, payload_type == 15 ? 0 : kind);
        w_u32(entry + 8, arguments[0]); w_u32(entry + 12, arguments[1]); w_u32(entry + 16, arguments[2]); w_u32(entry + 20, arguments[3]);
        w_u32(entry + 28, arguments[5]); w_u32(entry + 36, arguments[7]); w_u32(entry + 24, arguments[4]);
        value = r_u8(command + 1); w_u32(entry + 32, arguments[6]); w_u32(entry + 4, value);
        moh_draft_missing_call(0x800154EC, entry + 40, payload, payload_words << 2);
    }
    return command + ((words + payload_words) << 2);
}

uint32 level2p_sub_800569C0(uint32 owner, uint32 command, uint32 flags)
{
    uint32 mode, value, current, offset;
    FUNCTION_MARKER(0x800569C0, "LEVEL2P.BIN");
    mode = r_u8(command); offset = mode ? 16 : 12;
    value = r_u32(command + 4); value = level2p_sub_8005A984(1, flags, value, owner);
    current = r_u32(owner + offset); w_u32(owner + offset, current | value);
    return command + 8;
}

uint32 level2p_sub_80056A44(uint32 owner, uint32 command, uint32 flags)
{
    uint32 mode, value, offset, current;
    FUNCTION_MARKER(0x80056A44, "LEVEL2P.BIN");
    mode = r_u8(command); offset = mode ? 16 : 12; value = r_u32(command + 4);
    value = level2p_sub_8005A984(1, flags, value, owner); current = r_u32(owner + offset); w_u32(owner + offset, current & ~value);
    return command + 8;
}

uint32 level2p_sub_80056AC8(uint32 owner, uint32 command)
{
    FUNCTION_MARKER(0x80056AC8, "LEVEL2P.BIN");
    level2p_sub_8006F44C(owner); return command + 4;
}

uint32 level2p_sub_80056AF0(uint32 owner, uint32 command, uint32 flags)
{
    uint32 value, field;
    FUNCTION_MARKER(0x80056AF0, "LEVEL2P.BIN");
    value = r_u16(command); field = level2p_sub_8005A984(1, flags, value, owner);
    value = r_u32(command + 4); value = level2p_sub_8005A984(2, flags, value, owner); level2p_sub_8005A1B8(owner, field, value);
    return command + 8;
}

uint32 level2p_sub_80056B68(uint32 owner, uint32 command, uint32 flags)
{
    uint32 pointer, index, value;
    FUNCTION_MARKER(0x80056B68, "LEVEL2P.BIN");
    pointer = r_u32(owner + 0x100); pointer = r_u32(pointer + 8);
    if (pointer)
    {
        value = r_u32(command + 4); index = level2p_sub_8005A984(1, flags, value, owner);
        value = r_u16(command); value = level2p_sub_8005A984(2, flags, value, owner);
        pointer = r_u32(owner + 0x100); pointer = r_u32(pointer + 8); pointer = r_u32(pointer + 0xB8);
        /* TODO: Original command trusts the evaluated resource index */
        w_u16(pointer + (index << 4) + 4, value);
    }
    return command + 8;
}

uint32 level2p_sub_80068740(uint32 kind, uint32 parameter)
{
    uint32 object, pointer, animation;
    FUNCTION_MARKER(0x80068740, "LEVEL2P.BIN");
    object = level2p_sub_8005C838(kind, parameter);
    if (object)
    {
        pointer = r_u32(object + 0x100);
        if (pointer && r_u32(pointer + 8))
        {
            level2p_sub_8006591C(object); pointer = r_u32(object + 0x100); pointer = r_u32(pointer + 8);
            animation = (uint32)(sint32)r_s16(pointer + 0x94);
            if ((sint32)animation >= 0) { level2p_sub_8003F764(object, animation, 1, 0); level2p_sub_8003FC00(object); }
        }
    }
    return object;
}

uint32 level2p_sub_80056C00(uint32 owner, uint32 command, uint32 flags)
{
    uint32 value, object;
    FUNCTION_MARKER(0x80056C00, "LEVEL2P.BIN");
    value = r_u8(command); value = level2p_sub_8005A984(1, flags, value, owner); object = level2p_sub_80068740(value, 0);
    if (object) level2p_sub_80066BA8(object);
    return command + 8;
}

uint32 level2p_sub_80056C54(uint32 owner, uint32 command, uint32 flags)
{
    uint32 value, state;
    FUNCTION_MARKER(0x80056C54, "LEVEL2P.BIN");
    value = r_u16(command); value = level2p_sub_8005A984(1, flags, value, owner); state = r_u32(0x8009618C);
    w_u16(state + 0x1C, value - 1); w_u32(0x80096188, 1); return command + 4;
}

uint32 level2p_sub_80056CA8(uint32 owner, uint32 command)
{
    FUNCTION_MARKER(0x80056CA8, "LEVEL2P.BIN");
    level2p_sub_80066C64(owner); w_u32(0x80096188, 1); return command + 4;
}

static uint32 moh_command_halfword_triplet(uint32 owner, uint32 command, uint32 flags, uint32 offset)
{
    uint32 value, index, destination;
    value = r_u16(command); index = level2p_sub_8005A984(1, flags, value, owner);
    value = r_u32(command + 4); value = level2p_sub_8005A984(2, flags, value, owner);
    destination = owner + (index << 4) + offset; w_u16(destination, value);
    value = r_u32(command + 8); value = level2p_sub_8005A984(3, flags, value, owner); w_u16(destination + 2, value);
    value = r_u32(command + 12); value = level2p_sub_8005A984(4, flags, value, owner); w_u16(destination + 4, value);
    return command + 16;
}

uint32 level2p_sub_80056CDC(uint32 owner, uint32 command, uint32 flags)
{
    FUNCTION_MARKER(0x80056CDC, "LEVEL2P.BIN");
    /* TODO: Original evaluated owner slot index is unchecked */
    return moh_command_halfword_triplet(owner, command, flags, 0x2A4);
}

uint32 level2p_sub_80056D8C(uint32 owner, uint32 command, uint32 flags)
{
    FUNCTION_MARKER(0x80056D8C, "LEVEL2P.BIN");
    /* TODO: Original evaluated owner slot index is unchecked */
    return moh_command_halfword_triplet(owner, command, flags, 0x2AC);
}

uint32 level2p_sub_80056E3C(uint32 owner, uint32 command, uint32 flags)
{
    uint32 value, index;
    FUNCTION_MARKER(0x80056E3C, "LEVEL2P.BIN");
    for (index = 0; index < 3; ++index)
    {
        value = r_u32(command + 4 + index * 4); value = level2p_sub_8005A984(index + 1, flags, value, owner);
        w_u8(owner + 0x2A0 + index, value);
    }
    return command + 16;
}

void level2p_sub_80068710(uint32 owner)
{
    FUNCTION_MARKER(0x80068710, "LEVEL2P.BIN");
    /* Original8BCD4 ignores all six input carriers and immediately returns */
    (void)owner; level2p_sub_8008BCD4();
}

uint32 level2p_sub_80056EC0(uint32 owner, uint32 command)
{
    FUNCTION_MARKER(0x80056EC0, "LEVEL2P.BIN");
    level2p_sub_80068710(owner); return command + 4;
}

void level2p_sub_8008BCDC(uint32 model, uint32 value)
{
    FUNCTION_MARKER(0x8008BCDC, "LEVEL2P.BIN");
    if (model) w_u32(model + 0x68, value);
}

uint32 level2p_sub_80056EE8(uint32 owner, uint32 command, uint32 flags)
{
    uint32 value, model;
    FUNCTION_MARKER(0x80056EE8, "LEVEL2P.BIN");
    if (owner)
    {
        value = r_u8(command); value = level2p_sub_8005A984(1, flags, value, owner); model = r_u32(owner + 0x100);
        level2p_sub_8008BCDC(model, value);
    }
    return command + 4;
}

uint32 level2p_sub_80056F3C(uint32 owner, uint32 command, uint32 flags)
{
    uint32 value, mode;
    FUNCTION_MARKER(0x80056F3C, "LEVEL2P.BIN");
    value = r_u16(command); value = level2p_sub_8005A984(1, flags, value, owner); mode = r_u32(0x80096180);
    value = level2p_sub_8003F764(owner, (uint32)(sint32)(sint16)value, 1, mode);
    if (!value) return command + 4;
    mode = r_u32(0x80096180); return r_u32(owner + mode * 116 + 0x118);
}

static uint32 moh_command_signed_remainder(uint32 value, uint32 divisor, uint32 address)
{
    /* TODO: Original BREAK exceptions terminate through the native fail-fast boundary */
    if (!divisor) moh_draft_fail_fast("Original division by zero BREAK", address);
    if (divisor == 0xFFFFFFFF && value == 0x80000000) moh_draft_fail_fast("Original signed division overflow BREAK", address);
    return (uint32)((sint32)value % (sint32)divisor);
}

uint32 level2p_sub_80056FD8(uint32 owner, uint32 command, uint32 flags)
{
    uint32 random, value, invalid = 0, field, low, high;
    FUNCTION_MARKER(0x80056FD8, "LEVEL2P.BIN");
    random = sub_8001552C(); random = (uint32)((sint32)random % 100);
    value = r_u8(command + 1); value = level2p_sub_8005A984(4, flags, value, owner);
    if ((sint32)value < (sint32)random)
    {
        value = r_u8(command + 1); invalid = level2p_sub_8005A984(4, flags, value, owner) != 0;
    }
    if (invalid) { value = r_u8(command); field = level2p_sub_8005A984(1, flags, value, owner); value = 0xFFFFFFFF; }
    else
    {
        value = (uint32)(sint32)r_s16(command + 4); low = level2p_sub_8005A984(2, flags, value, owner);
        value = (uint32)(sint32)r_s16(command + 6); high = level2p_sub_8005A984(3, flags, value, owner);
        value = r_u8(command); field = level2p_sub_8005A984(1, flags, value, owner); random = sub_8001552C();
        value = low + moh_command_signed_remainder(random, high - low + 1, 0x80056FD8);
    }
    level2p_sub_8005A1B8(owner, field, value); return command + 8;
}

uint32 level2p_sub_800572D8(uint32 owner, uint32 command, uint32 flags)
{
    uint32 value, divisor, random, low, high, field;
    FUNCTION_MARKER(0x800572D8, "LEVEL2P.BIN");
    value = r_u8(command + 1); value = level2p_sub_8005A984(4, flags, value, owner); divisor = value * 480; random = sub_8001552C();
    if (!divisor) moh_draft_fail_fast("Original unsigned division by zero BREAK", 0x800572D8);
    if (random % divisor < 16)
    {
        value = r_u32(command + 4); low = level2p_sub_8005A984(2, flags, value, owner);
        value = r_u32(command + 8); high = level2p_sub_8005A984(3, flags, value, owner);
        value = r_u8(command); field = level2p_sub_8005A984(1, flags, value, owner); random = sub_8001552C();
        value = low + moh_command_signed_remainder(random, high - low + 1, 0x800572D8);
    }
    else { value = r_u8(command); field = level2p_sub_8005A984(1, flags, value, owner); value = 0xFFFFFFFF; }
    level2p_sub_8005A1B8(owner, field, value); return command + 12;
}

uint32 level2p_sub_800687CC(uint32 kind, uint32 identifier)
{
    uint32 current;
    FUNCTION_MARKER(0x800687CC, "LEVEL2P.BIN");
    current = r_u32(0x80099434);
    /* TODO: Original linked-list walk has no cycle bound */
    while (current)
    {
        if (r_u32(current + 8) == kind && r_s8(current + 0x35) == (sint8)identifier && !(r_u32(current + 12) & 0x20080000)) return current;
        current = r_u32(current + 4);
    }
    return 0;
}

uint32 level2p_sub_80057154(uint32 owner, uint32 command, uint32 flags)
{
    uint32 value, target_kind, identifier, target, operand, mode, shifted;
    FUNCTION_MARKER(0x80057154, "LEVEL2P.BIN");
    operand = r_u32(command + 4); value = level2p_sub_8005A984(1, flags, operand, owner);
    operand = r_u32(command + 8); target_kind = level2p_sub_8005A984(2, flags, operand, owner);
    operand = r_u8(command + 1); identifier = level2p_sub_8005A984(3, flags, operand, owner);
    operand = r_u32(command + 12); target = level2p_sub_8005A984(4, flags, operand, owner);
    if (!target)
    {
        target = owner;
        if (target_kind != 0xFFFFFFFF && (sint8)identifier != -1) target = level2p_sub_800687CC(target_kind, (uint32)(sint32)(sint8)identifier);
    }
    /* TODO: Original setters use an unresolved or null target without a guard */
    mode = r_u8(command); shifted = (uint32)((sint32)value >> 16);
    switch (mode)
    {
    case 1: w_u32(target + 0xF4, value); break;
    case 2: w_u32(target + 0x270, value); w_u32(target + 0x274, value); w_u32(target + 0x278, value); break;
    case 3: w_u32(target + 0x2D4, value); break;
    case 4: w_u32(target + 0x294, shifted); w_u32(target + 0x290, shifted); break;
    case 5: w_u32(target + 0x44, shifted); break;
    case 6: w_u32(target + 0x48, shifted); break;
    case 19: w_u16(target + 0x2E8, shifted); break;
    case 20: w_u16(target + 0x2EC, shifted); break;
    }
    return command + 16;
}

static uint32 moh_command_motion_words(uint32 owner, uint32 command, uint32 flags, uint32 offset, uint32 add)
{
    uint32 index, value, current;
    for (index = 0; index < 3; ++index)
    {
        value = r_u32(command + 4 + index * 4); value = level2p_sub_8005A984(index + 1, flags, value, owner);
        if (add) { current = r_u32(owner + offset + index * 4); value += current; }
        w_u32(owner + offset + index * 4, value);
    }
    return command + 16;
}

uint32 level2p_sub_80057420(uint32 owner, uint32 command, uint32 flags)
{
    FUNCTION_MARKER(0x80057420, "LEVEL2P.BIN");
    return moh_command_motion_words(owner, command, flags, 0x234, 0);
}

uint32 level2p_sub_800574A4(uint32 owner, uint32 command, uint32 flags)
{
    FUNCTION_MARKER(0x800574A4, "LEVEL2P.BIN");
    return moh_command_motion_words(owner, command, flags, 0x234, 1);
}

uint32 level2p_sub_80057544(uint32 owner, uint32 command, uint32 flags)
{
    FUNCTION_MARKER(0x80057544, "LEVEL2P.BIN");
    return moh_command_motion_words(owner, command, flags, 0x228, 0);
}

uint32 level2p_sub_800575C8(uint32 owner, uint32 command, uint32 flags)
{
    FUNCTION_MARKER(0x800575C8, "LEVEL2P.BIN");
    return moh_command_motion_words(owner, command, flags, 0x228, 1);
}

static void moh_bounds_prepare(uint32 point, const uint32 *plane, const uint32 *minimum, const uint32 *maximum)
{
    uint32 axis;
    for (axis = 0; axis < 3; ++axis)
        w_u16(point + axis * 2, (sint32)plane[axis] < 0 ? minimum[axis] : maximum[axis]);
}

static void moh_bounds_project(uint32 point)
{
    uint32 value;
    value = r_u32(point); xport_gte_write_data(0, value);
    value = r_u32(point + 4); xport_gte_write_data(1, value);
    xport_gte_execute(0x180001);
}

void level2p_sub_80089E98(uint32 root)
{
    uint32 pending[32], planes[6][3], minimum[3], maximum[3];
    uint8 point[8];
    uint32 owner, height, width, data, depth_limit, top = 0, count = 0, node, value, axis, plane, point_view, flags, accepted;
    FUNCTION_MARKER(0x80089E98, "LEVEL2P.BIN");
    owner = r_u32(0x80099428); height = r_u32(0x80098B78);
    owner = r_u32(owner + 0x388); width = r_u32(0x80098B74); data = r_u32(owner + 200);
    for (plane = 0; plane < 6; ++plane)
        for (axis = 0; axis < 3; ++axis) planes[plane][axis] = r_u32(data + 0x50 + plane * 28 + axis * 4);
    depth_limit = r_u32(data + 0x40); pending[0] = root;
    /* TODO: Original point padding is opaque and traversal trusts its 32-entry local array */
    point_view = moh_draft_local_address(point, sizeof(point));
    while ((sint32)top >= 0)
    {
        node = pending[top]; --top;
        value = r_u32(node); minimum[0] = (uint16)value; minimum[1] = value >> 16;
        minimum[2] = (uint32)(sint32)r_s16(node + 4);
        value = r_u32(node + 6); maximum[0] = (uint16)value; maximum[1] = value >> 16;
        maximum[2] = (uint32)(sint32)r_s16(node + 10);
        accepted = 0;
        moh_bounds_prepare(point_view, planes[3], minimum, maximum); moh_bounds_project(point_view);
        moh_bounds_prepare(point_view, planes[2], minimum, maximum);
        value = xport_gte_read_data(14); if ((sint16)(value >> 16) < 0) goto classify;
        moh_bounds_project(point_view); moh_bounds_prepare(point_view, planes[1], minimum, maximum);
        value = xport_gte_read_data(14); if ((sint32)width < (sint16)value) goto classify;
        moh_bounds_project(point_view); moh_bounds_prepare(point_view, planes[5], minimum, maximum);
        value = xport_gte_read_data(14); if ((sint16)value < 0) goto classify;
        moh_bounds_project(point_view); moh_bounds_prepare(point_view, planes[4], minimum, maximum);
        flags = xport_gte_read_control(31); value = xport_gte_read_data(27);
        if ((sint32)depth_limit < (sint32)(value << 16) || (flags & 0x400000)) goto classify;
        moh_bounds_project(point_view); moh_bounds_prepare(point_view, planes[0], minimum, maximum);
        value = xport_gte_read_data(14); if ((sint32)height < (sint16)(value >> 16)) goto classify;
        moh_bounds_project(point_view); flags = xport_gte_read_control(31);
        accepted = (flags & 0x40000) == 0;
classify:
        if (!accepted) continue;
        if (r_s32(node + 12) > 0)
        {
            w_u32(0x800A94E4 + (count << 2), node); ++count;
            continue;
        }
        value = r_u32(node + 24); if (value) { ++top; pending[top] = value; }
        value = r_u32(node + 32); if (value) { ++top; pending[top] = value; }
        value = r_u32(node + 28); if (value) { ++top; pending[top] = value; }
    }
    w_u32(0x800AA484, 0); w_u32(0x800A94E0, count);
}

uint32 level2p_sub_8008AA78(uint32 state, uint32 index)
{
    uint32 data, value, data_offset, relocated, resource, count, bsp, base, cursor, field, maximum = 0, first;
    FUNCTION_MARKER(0x8008AA78, "LEVEL2P.BIN");
    data = r_u32(state + 0x10);
    w_u32(state + 4, 0); w_u32(state + 12, 0); w_u32(state + 0x98, 0); w_u32(state + 0x90, 0);
    w_u32(state + 0xC4, 0); w_u32(state + 0x88, 0); w_u32(state + 0x34, 0); w_u32(state + 0x30, 0);
    w_u32(state + 0x2C, 0); w_u32(state + 0x28, 0); w_u32(state + 0x24, 0); w_u32(state + 0x20, 0);
    w_u16(state + 0x8C, 0); w_u32(state + 0x88, 0); w_u16(state + 0x8E, 0);
    w_u32(state + 0x90, 0); w_u32(state + 0x18, 0);
    if (!data) return 0;
    value = r_u32(data + 8); w_u32(state + 0x14, data + value);
    value = r_u16(data + 0x14); data_offset = r_u32(data + 0x10); w_u16(state + 0x8C, value);
    value = r_u32(data + 0x18); w_u32(state + 0x88, data + value);
    value = r_u16(data + 0x1C); w_u16(state + 0x8E, value);
    value = r_u32(data + 0x20); w_u32(state + 0x90, data + value);
    value = r_u32(data + 0x28); relocated = data + data_offset; w_u32(state + 0x18, data + value);
    resource = r_u32(data + 0x38); count = r_u32(data + 0x34);
    level2p_sub_80054030(data + resource, count, index);
    value = r_u32(data + 0x3C); base = r_u32(state + 0x14); bsp = data + value;
    cursor = base; field = base + 0x18;
    /* TODO: Original relocation walk trusts offsets and has no independent count bound */
    do
    {
        if (!r_u32(field - 12))
        {
            first = r_u32(field); if ((sint32)maximum < (sint32)first) maximum = first;
            value = r_u32(field + 4); if ((sint32)maximum < (sint32)value) maximum = value;
            value = r_u32(field + 8); if ((sint32)maximum < (sint32)value) maximum = value;
            w_u32(field, (sint32)first < 0 ? 0 : base + first);
            value = r_u32(field + 8); w_u32(field + 8, (sint32)value < 0 ? 0 : base + value);
            value = r_u32(field + 4); w_u32(field + 4, (sint32)value < 0 ? 0 : base + value);
            field += 36; cursor += 36;
        }
        else
        {
            value = r_u32(field); cursor += 28; w_u32(field, relocated + value); field += 28;
        }
    } while ((sint32)(cursor - base) <= (sint32)maximum);
    if (level2p_sub_8003F4C8(bsp, index)) return 1;
    moh_draft_missing_call(0x80015B28, 0x8003D8F8);
    return 0;
}

void level2p_sub_8008ADB4(uint32 filename, uint32 index)
{
    uint8 size_output[8];
    uint32 buffer, size_view, data, state;
    FUNCTION_MARKER(0x8008ADB4, "LEVEL2P.BIN");
    buffer = r_u32(0x8009532C + (index << 2));
    /* TODO: Map original opaque local output through the fail-fast adapter */
    size_view = moh_draft_local_address(size_output, sizeof(size_output));
    data = level2p_sub_8004A4AC(filename, size_view, buffer);
    state = 0x80094C6C + (index << 8); w_u32(state + 0x10, data);
    if (!level2p_sub_8008AA78(state, index)) moh_draft_missing_call(0x80015B28, 0x8003D974);
    level2p_sub_80065C9C(0x8009506C + index * 176, state, 22, 0);
}

void level2p_sub_80053FAC(uint32 index)
{
    uint32 group;
    FUNCTION_MARKER(0x80053FAC, "LEVEL2P.BIN");
    group = 0x80095378 + index * 0x288;
    w_u32(group + 0x284, 0); w_u32(group + 0x280, 0);
}

void level2p_sub_80054030(uint32 resource, uint32 count, uint32 index)
{
    uint32 row = 0, group, entry, value, identifier, slot;
    FUNCTION_MARKER(0x80054030, "LEVEL2P.BIN");
    if (!count) { level2p_sub_80053FAC(index); return; }
    if (index >= 4) level2p_sub_800885B8(5, 0x20);
    group = 0x80095378 + index * 0x288;
    w_u32(group + 0x280, count); w_u32(group + 0x284, 1);
    /* TODO: Original resource count and relative entry lengths are unchecked */
    do
    {
        entry = group + row * 20; w_u32(entry, resource);
        value = r_u16(resource + 0x16); w_u32(entry + 8, resource + value);
        value = r_u16(resource + 0x14); w_u32(entry + 16, 1); w_u32(entry + 4, resource + value);
        identifier = r_u32(resource + 12);
        if (identifier < 99)
        {
            slot = 0x80095D98 + identifier; value = r_u8(slot);
            if (value == 0xFF) { w_u8(slot, 0); w_u32(entry + 12, 0); }
            else
            {
                value = r_u8(slot); w_u32(entry + 12, value); level2p_sub_80053EF0(entry, index);
            }
        }
        else w_u32(entry + 12, 0);
        value = r_u32(resource); ++row; resource += value;
    } while (row < count);
}

void level2p_sub_80053808(void)
{
    uint8 filename[40];
    uint32 resource, count, filename_view, index = 0, identifier;
    FUNCTION_MARKER(0x80053808, "LEVEL2P.BIN");
    resource = r_u32(0x80098FB0); count = r_u32(resource + 0x84); w_u32(0x80094C64, count);
    if ((sint32)count <= 0) return;
    /* TODO: Original sprintf trusts its 40-byte local buffer; adapter remains fail-fast */
    filename_view = moh_draft_local_address(filename, sizeof(filename));
    do
    {
        resource = r_u32(0x80098FB0); identifier = r_u32(0x80095340 + (index << 2));
        moh_draft_missing_call(0x8001BE18, filename_view, 0x8003C090, resource, identifier);
        level2p_sub_8008ADB4(filename_view, index);
        identifier = r_u32(0x80095340 + (index << 2)); w_u32(0x80094C50 + (index << 2), identifier);
        count = r_u32(0x80094C64); ++index;
    } while ((sint32)index < (sint32)count);
}
