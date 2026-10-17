#include "game_level2p_signatures.h"
#include "xport_trace.h"
#include "psx.h"

static void moh_rtpt_vertices(uint32 vertices, uint32 packed)
{
    uint32 first = vertices + ((packed & 0x3FF) << 3);
    uint32 second = vertices + ((packed >> 7) & 0x1FF8);
    uint32 third = vertices + ((packed >> 17) & 0x1FF8);
    xport_gte_write_data(0, r_u32(first));
    xport_gte_write_data(1, r_u32(first + 4));
    xport_gte_write_data(2, r_u32(second));
    xport_gte_write_data(3, r_u32(second + 4));
    xport_gte_write_data(4, r_u32(third));
    xport_gte_write_data(5, r_u32(third + 4));
}

static void moh_rtpt_scale_colors(uint32 packet, uint32 factor, uint32 model)
{
    uint32 products[9], blend;
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
    w_u8(packet + 17, (uint8)(products[4] >> 7));
    w_u8(packet + 18, (uint8)(products[5] >> 7));
    w_u8(packet + 28, (uint8)(products[6] >> 7));
    w_u8(packet + 6, (uint8)(products[2] >> 7));
    w_u8(packet + 29, (uint8)(products[7] >> 7));
    blend = r_u16(packet + 26);
    w_u8(packet + 30, (uint8)(products[8] >> 7));
    w_u16(packet + 26, (uint16)((blend & 0xFF9F) | ((r_u16(model + 0x68) & 3) << 5)));
}

static void moh_rtpt_link(uint32 packet)
{
    uint32 tag, packet_word, table_word;
    tag = r_u32(0x1F8001D0) + r_u32(0x1F800198) * 4;
    packet_word = r_u32(packet);
    table_word = r_u32(tag);
    w_u32(packet, (packet_word & 0xFF000000) | (table_word & 0xFFFFFF));
    tag = r_u32(0x1F8001D0) + r_u32(0x1F800198) * 4;
    w_u32(tag, (r_u32(tag) & 0xFF000000) | (packet & 0xFFFFFF));
}

uint32 level2p_sub_8007F31C(uint32 model, uint32 ordering, uint32 index, uint32 pass, uint32 depth)
{
    uint32 vertices, colors, linked, handle, factor, code, packet, limit, limit_view, list, cursor, current;
    uint32 flags, xy1, xy2, xy3, packed, color1, color2, color3, slot, value, mask, color_index, iteration;
    sint32 fog, minimum, count, projected, shift, bucket, emitted = 0;
    FUNCTION_MARKER(0x8007F31C, "LEVEL2P.BIN");
    vertices = r_u32(model + 0x2C);
    fog = r_s16(r_u32(0x80098FB0) + 0xCC);
    linked = r_u32(model + 0x58);
    colors = r_u32(model + 0x48);
    if (!linked)
    {
        handle = r_u32(model + 0x54);
        linked = handle ? r_u32(0x80099460 + ((handle & 0xFFFF) << 2)) : 0;
        linked = r_u32(linked + 0x100);
    }
    minimum = r_s16(linked + 0xA8);
    code = r_u32(model + 0x68) ? 0x36 : 0x34;
    factor = r_u32(model + 0x6C);
    /* TODO: Addressable buffer-limit local requires the permitted native adapter */
    limit_view = moh_draft_local_address(&limit, sizeof(limit));
    packet = level2p_sub_8005365C(index, limit_view);
    limit -= 40;
    value = r_u32(0x80098FB0);
    flags = r_u32(value + 0xD8);
    w_u32(0x1F8001B0, value + 0xDC);
    w_u32(0x1F8001C0, 0);
    w_u32(0x1F8001C8, 0xFFFFFF);
    w_u32(0x1F8001D0, ordering);
    w_u32(0x1F8001AC, flags != 0);
    list = r_u32(model + (pass ? 0x28 : 0x20));
    count = r_s32(list);
    cursor = list + 4;
    moh_rtpt_vertices(vertices, r_u32(cursor + 12));
    for (;;)
    {
        if (count < 1 || packet >= limit) break;
        xport_gte_execute(0x280030);
        w_u8(packet + 3, 9);
        current = cursor;
        value = r_u32(current);
        packed = r_u32(current + 4);
        flags = r_u32(current + 8);
        cursor += 16;
        count--;
        w_u32(packet + 12, value);
        w_u32(packet + 24, packed);
        w_u32(packet + 36, flags);
        w_u32(0x1F800190, xport_gte_read_flag());
        xport_gte_execute(0x1400006);
        if (count > 0) moh_rtpt_vertices(vertices, r_u32(cursor + 12));
        if ((r_u32(0x1F800190) & 0x80020000) == 0x80000000) continue;
        if ((sint32)xport_gte_read_data(24) <= 0) continue;
        value = r_u16(linked + 0xAA) & 0x7FFF;
        xy1 = xport_gte_read_data(12);
        xy2 = xport_gte_read_data(13);
        xy3 = xport_gte_read_data(14);
        xport_gte_execute(0x158002D);
        projected = (sint32)(xport_gte_read_data(7) << 2);
        if (projected < minimum) projected = minimum;
        shift = (sint32)(value - (uint32)minimum) >> 9;
        bucket = (sint32)((uint32)projected - (uint32)minimum) >> ((uint32)shift & 31);
        flags = r_u32(0x1F8001C0);
        w_u32(0x1F800198, (uint32)bucket);
        if (!flags)
        {
            packed = r_u32(current + 12);
            color1 = r_u32(colors + ((packed & 0x3FF) << 2));
            color2 = r_u32(colors + ((packed >> 8) & 0xFFC));
            color3 = r_u32(colors + ((packed >> 18) & 0xFFC));
            w_u32(0x1F8001B4, color1);
            w_u32(0x1F8001B8, color2);
            w_u32(0x1F8001BC, color3);
        }
        else
        {
            value = r_u32(0x1F8001C8);
            w_u32(0x1F8001B4, value);
            w_u32(0x1F8001B8, value);
            w_u32(0x1F8001BC, value);
        }
        bucket = (sint32)(r_u32(0x1F800198) - 15);
        w_u32(0x1F800198, (uint32)bucket);
        if (bucket <= 0) w_u32(0x1F800198, 1);
        if (r_s32(0x1F800198) >= 512) w_u32(0x1F800198, 511);
        if (!pass && r_u32(0x1F8001AC))
        {
            mask = r_u32(0x1F8001C8);
            for (iteration = 0; iteration < 3; iteration++)
            {
                slot = 0x1F8001B4 + iteration * 4;
                value = r_u32(slot);
                color_index = value & mask;
                if (color_index < 40)
                {
                    packed = r_u32(r_u32(0x1F8001B0) + color_index * 20 + 12);
                    w_u32(slot, (value & 0xFF000000) | (packed & mask));
                }
            }
            if (!fog)
            {
                w_u32(packet + 4, r_u32(0x1F8001B4));
                w_u32(packet + 16, r_u32(0x1F8001B8));
                value = r_u32(0x1F8001BC);
                w_u8(packet + 7, (uint8)code);
                w_u32(packet + 28, value);
            }
        }
        /* TODO: Original no-fog path without remapping retains existing packet colors and code */
        if (fog)
        {
            xport_gte_write_data(20, r_u32(0x1F8001B4));
            xport_gte_write_data(21, r_u32(0x1F8001B8));
            value = r_u32(0x1F8001BC);
            xport_gte_write_data(22, value);
            xport_gte_write_data(6, value);
            xport_gte_execute(0xF8002A);
            w_u32(packet + 4, xport_gte_read_data(20));
            w_u32(packet + 16, xport_gte_read_data(21));
            w_u32(packet + 28, xport_gte_read_data(22));
            w_u8(packet + 7, (uint8)code);
            if (factor != 128) moh_rtpt_scale_colors(packet, factor, model);
            emitted++;
            moh_rtpt_link(packet);
        }
        else
        {
            moh_rtpt_link(packet);
            if (factor != 128) moh_rtpt_scale_colors(packet, factor, model);
            emitted++;
        }
        packet += 40;
        w_u32(packet - 32, xy1);
        w_u32(packet - 20, xy2);
        w_u32(packet - 8, xy3);
    }
    if (depth) w_u32(depth, (uint32)minimum);
    level2p_sub_8005367C(packet);
    return emitted > 0;
}

static void moh_render_save_controls(void)
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

static void moh_render_restore_controls(void)
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



static uint32 moh_render_depth_level(uint32 data)
{
    sint32 maximum = r_s16(data + 0x3E), value = r_s16(data + 0x40);
    if (maximum < value) maximum = value;
    value = r_s16(data + 0x3C);
    if (maximum <= value) maximum = value;
    maximum -= 2;
    return maximum < 0 ? 0 : (uint32)maximum;
}

uint32 level2p_sub_8007DAC0(uint32 model, uint32 minimum_output, uint32 temporary)
{
    uint32 zero[2], zero_view, handle, related, linked, data, level, projected, value;
    FUNCTION_MARKER(0x8007DAC0, "LEVEL2P.BIN");
    /* TODO: Addressable SDK zero vector requires the permitted native adapter */
    zero_view = moh_draft_local_address(zero, sizeof(zero));
    moh_draft_missing_call(0x800158AC, zero_view, 0, 8);
    if (temporary)
    {
        w_u32(minimum_output, 0);
        return 0;
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
        if (r_u32(linked + 0x74))
        {
            data = r_u32(model + 8);
            w_u32(0x8009ABF0, linked);
            level = moh_render_depth_level(data);
            xport_gte_write_data(0, zero[0]);
            xport_gte_write_data(1, zero[1]);
            xport_gte_execute(0x180001);
            projected = xport_gte_read_data(19);
        }
        else
        {
            level = moh_render_depth_level(r_u32(linked + 8));
            moh_render_save_controls();
            handle = r_u32(model + 0x54);
            related = handle ? r_u32(0x80099460 + ((handle & 0xFFFF) << 2)) : 0;
            level2p_sub_8008BA7C(related);
            xport_gte_write_data(0, zero[0]);
            xport_gte_write_data(1, zero[1]);
            xport_gte_execute(0x180001);
            projected = xport_gte_read_data(19);
            moh_render_restore_controls();
        }
    }
    else
    {
        level = moh_render_depth_level(r_u32(model + 8));
        xport_gte_write_data(0, zero[0]);
        xport_gte_write_data(1, zero[1]);
        xport_gte_execute(0x180001);
        projected = xport_gte_read_data(19);
    }
    value = projected - ((uint32)1 << ((level + 9) & 31));
    w_u32(minimum_output, value);
    if ((sint32)value < 0) w_u32(minimum_output, 0);
    return level;
}

uint32 level2p_sub_8007DE4C(uint32 model, uint32 ordering, uint32 index, uint32 pass, uint32 depth, uint32 temporary)
{
    uint32 vertices, colors, code, value, limit, limit_view, packet, minimum, minimum_view, shift;
    uint32 list, cursor, handle, related, count, packed, flags, linked, projected, bucket, slot, mask, color_index, iteration, tag;
    sint32 fog, emitted = 0;
    FUNCTION_MARKER(0x8007DE4C, "LEVEL2P.BIN");
    vertices = r_u32(model + 0x2C);
    fog = r_s16(r_u32(0x80098FB0) + 0xCC);
    value = r_u32(model + 0x68);
    colors = r_u32(model + 0x48);
    code = value ? 0x36 : 0x34;
    xport_gte_write_data(6, code << 24);
    value = r_u32(0x80098FB0);
    flags = r_u32(value + 0xD8);
    w_u32(0x1F8001B0, value + 0xDC);
    w_u32(0x1F8001C0, 0);
    w_u32(0x1F8001C4, 0);
    w_u32(0x1F8001C8, 0xFFFFFF);
    w_u32(0x1F8001AC, flags != 0);
    /* TODO: Addressable limit and minimum locals require the permitted native adapter */
    limit_view = moh_draft_local_address(&limit, sizeof(limit));
    packet = level2p_sub_8005365C(index, limit_view);
    minimum_view = moh_draft_local_address(&minimum, sizeof(minimum));
    limit -= 40;
    value = level2p_sub_8007DAC0(model, minimum_view, temporary);
    shift = temporary ? 4 : value + 1;
    w_u32(0x1F8001A8, 0xFFFF);
    w_u32(0x1F8001D0, ordering);
    list = r_u32(model + (pass ? 0x28 : 0x20));
    count = r_u32(list);
    cursor = list + 4;
    moh_rtpt_vertices(vertices, r_u32(cursor + 12));
    handle = r_u32(model + 0x54);
    related = handle ? r_u32(0x80099460 + ((handle & 0xFFFF) << 2)) : 0;
    while ((sint32)count > 0 && packet < limit)
    {
        xport_gte_execute(0x280030);
        value = r_u32(cursor);
        packed = r_u32(cursor + 4);
        flags = r_u32(cursor + 8);
        w_u32(packet + 12, value);
        w_u32(packet + 24, packed);
        w_u32(packet + 36, flags);
        w_u32(0x1F800190, xport_gte_read_flag());
        xport_gte_execute(0x1400006);
        /* Original preloads the next triangle even on the final iteration */
        moh_rtpt_vertices(vertices, r_u32(cursor + 28));
        if ((r_u32(0x1F800190) & 0x7FFD8000) || (sint32)xport_gte_read_data(24) <= 0) goto next_triangle;
        xport_gte_execute(0x158002D);
        projected = xport_gte_read_data(7) << 2;
        linked = 0;
        if (related)
        {
            linked = r_u32(model + 0x58);
            if (!linked)
            {
                handle = r_u32(model + 0x54);
                value = handle ? r_u32(0x80099460 + ((handle & 0xFFFF) << 2)) : 0;
                linked = r_u32(value + 0x100);
            }
        }
        if (related && r_u32(linked + 0x74))
        {
            value = (uint32)(sint32)r_s16(linked + 0xA8);
            flags = r_u32(linked + 0xAC);
            bucket = projected - value;
            w_u32(0x1F800198, bucket);
            if ((sint32)bucket < 0) bucket = 0;
            else bucket = (uint32)((sint32)bucket >> (flags & 31));
        }
        else bucket = (projected - minimum + r_u32(0x1F8001C4)) >> (shift & 31);
        w_u32(0x1F800198, bucket);
        if ((sint32)bucket < 0) goto next_triangle;
        if ((sint32)bucket >= 512) w_u32(0x1F800198, 509);
        w_u32(packet + 8, xport_gte_read_data(12));
        w_u32(packet + 20, xport_gte_read_data(13));
        w_u32(packet + 32, xport_gte_read_data(14));
        packed = r_u32(cursor + 12);
        value = r_u32(colors + ((packed & 0x3FF) << 2));
        flags = r_u32(colors + ((packed >> 8) & 0xFFC));
        bucket = r_u32(colors + ((packed >> 18) & 0xFFC));
        w_u32(0x1F8001B4, value);
        w_u32(0x1F8001B8, flags);
        w_u32(0x1F8001BC, bucket);
        if (!pass && r_u32(0x1F8001AC))
        {
            mask = r_u32(0x1F8001C8);
            for (iteration = 0; iteration < 3; iteration++)
            {
                slot = 0x1F8001B4 + iteration * 4;
                value = r_u32(slot);
                color_index = value & mask;
                if (color_index < 40)
                {
                    packed = r_u32(r_u32(0x1F8001B0) + color_index * 20 + 12);
                    w_u32(slot, (value & 0xFF000000) | (packed & mask));
                }
            }
        }
        if (related)
        {
            bucket = r_u32(0x1F800198) - 15;
            w_u32(0x1F800198, bucket);
            if ((sint32)bucket <= 0) w_u32(0x1F800198, 1);
        }
        if (fog)
        {
            xport_gte_write_data(20, r_u32(0x1F8001B4));
            xport_gte_write_data(21, r_u32(0x1F8001B8));
            value = r_u32(0x1F8001BC);
            xport_gte_write_data(22, value);
            xport_gte_write_data(6, value);
            xport_gte_execute(0xF8002A);
            emitted++;
            tag = r_u32(0x1F8001D0) + r_u32(0x1F800198) * 4;
            w_u32(packet, r_u32(tag) | 0x9000000);
            w_u32(tag, packet & 0xFFFFFF);
            w_u32(packet + 4, xport_gte_read_data(20));
            w_u32(packet + 16, xport_gte_read_data(21));
            w_u32(packet + 28, xport_gte_read_data(22));
            w_u8(packet + 7, (uint8)code);
        }
        else
        {
            emitted++;
            value = r_u32(0x1F8001B4);
            flags = r_u32(0x1F8001B8);
            bucket = r_u32(0x1F8001BC);
            w_u32(packet + 4, value);
            w_u32(packet + 16, flags);
            w_u32(packet + 28, bucket);
            w_u8(packet + 7, (uint8)code);
            tag = r_u32(0x1F8001D0) + r_u32(0x1F800198) * 4;
            w_u32(packet, r_u32(tag) | 0x9000000);
            w_u32(tag, packet & 0xFFFFFF);
        }
        packet += 40;
next_triangle:
        count--;
        cursor += 16;
    }
    if (depth) w_u32(depth, minimum);
    level2p_sub_8005367C(packet);
    return emitted > 0;
}


static void moh_segment_copy8(uint32 output, uint32 input)
{
    uint32 first = r_u32(input), second = r_u32(input + 4);
    w_u32(output, first);
    w_u32(output + 4, second);
}

static uint32 moh_segment_edge(uint32 context, uint32 from_field, uint32 to_field)
{
    uint32 first, second, x, y, z, first_x, first_y, first_z, second_x, second_y, second_z, normal;
    first = r_u32(context + from_field);
    x = r_u16(context + 0x10);
    y = r_u16(context + 0x12);
    first_x = r_u16(first);
    z = r_u16(context + 0x14);
    first_y = r_u16(first + 2);
    first_z = r_u16(first + 4);
    w_u16(context + 0x20, (uint16)(x - first_x));
    w_u16(context + 0x24, (uint16)(z - first_z));
    w_u16(context + 0x22, (uint16)(y - first_y));
    first = r_u32(context + from_field);
    second = r_u32(context + to_field);
    first_x = r_u16(first);
    second_x = r_u16(second);
    second_y = r_u16(second + 2);
    second_z = r_u16(second + 4);
    first_y = r_u16(first + 2);
    first_z = r_u16(first + 4);
    w_u16(context + 0x28, (uint16)(second_x - first_x));
    w_u16(context + 0x2C, (uint16)(second_z - first_z));
    w_u16(context + 0x2A, (uint16)(second_y - first_y));
    xport_gte_write_control(0, (uint32)(sint32)r_s16(context + 0x20));
    xport_gte_write_control(2, (uint32)(sint32)r_s16(context + 0x22));
    xport_gte_write_control(4, (uint32)(sint32)r_s16(context + 0x24));
    xport_gte_write_data(9, (uint32)(sint32)r_s16(context + 0x28));
    xport_gte_write_data(10, (uint32)(sint32)r_s16(context + 0x2A));
    xport_gte_write_data(11, (uint32)(sint32)r_s16(context + 0x2C));
    xport_gte_execute(0x178000C);
    x = xport_gte_read_data(9);
    y = xport_gte_read_data(10);
    z = xport_gte_read_data(11);
    w_u16(context + 0x30, (uint16)x);
    w_u16(context + 0x32, (uint16)y);
    w_u16(context + 0x34, (uint16)z);
    normal = r_u32(context + 0x60);
    xport_gte_write_control(0, r_u32(normal));
    xport_gte_write_control(1, r_u32(normal + 4));
    xport_gte_write_data(9, r_u16(context + 0x30));
    xport_gte_write_data(10, r_u16(context + 0x32));
    xport_gte_write_data(11, r_u16(context + 0x34));
    xport_gte_execute(0x41E012);
    return (sint32)xport_gte_read_data(25) >= 0;
}

uint32 level2p_sub_8003DF20(uint32 context)
{
    uint32 iteration, face, group, normal, first, second, third, x, y, z, numerator, divisor, fraction, output, hit;
    uint32 distance1, distance2;
    sint32 start_y, end_y, minimum_y, maximum_y, value;
    FUNCTION_MARKER(0x8003DF20, "LEVEL2P.BIN");
    start_y = r_s16(context + 2);
    end_y = r_s16(context + 10);
    if (start_y < end_y)
    {
        w_u32(context + 0x58, (uint32)start_y);
        value = end_y;
    }
    else
    {
        value = r_s16(context + 2);
        w_u32(context + 0x58, (uint32)end_y);
    }
    w_u32(context + 0x5C, (uint32)value);
    x = r_u16(context + 8) - r_u16(context);
    y = r_u16(context + 10);
    z = r_u16(context + 12);
    w_u16(context + 0x18, (uint16)x);
    z -= r_u16(context + 4);
    y -= r_u16(context + 2);
    w_u16(context + 0x1C, (uint16)z);
    w_u16(context + 0x1A, (uint16)y);
    value = r_s32(context + 0x4C);
    w_u32(context + 0x54, 0xFFFFFFFF);
    w_u32(context + 0x50, (uint32)value);
    for (iteration = 0; (sint32)iteration < r_s32(context + 0x40); iteration++)
    {
        face = r_u32(context + 0x38) + (uint32)(sint32)r_s16(r_u32(context + 0x3C) + iteration * 2) * 10;
        group = r_u32(0x800AAC38);
        normal = r_u32(group + 0x20) + ((uint32)(sint32)r_s16(face + 6) << 3);
        w_u32(context + 0x60, normal);
        first = r_u32(group + 0x1C) + ((uint32)(sint32)r_s16(face) << 3);
        w_u32(context + 0x64, first);
        second = r_u32(group + 0x1C) + ((uint32)(sint32)r_s16(face + 2) << 3);
        w_u32(context + 0x68, second);
        third = r_u32(group + 0x1C) + ((uint32)(sint32)r_s16(face + 4) << 3);
        w_u32(context + 0x6C, third);
        minimum_y = r_s16(first + 2);
        maximum_y = r_s16(second + 2);
        if (minimum_y > maximum_y) { value = minimum_y; minimum_y = maximum_y; maximum_y = value; }
        value = r_s16(third + 2);
        if (value < minimum_y) minimum_y = value;
        else if (maximum_y < value) maximum_y = value;
        if (maximum_y < r_s32(context + 0x58) || r_s32(context + 0x5C) < minimum_y) continue;
        xport_gte_write_control(0, r_u32(context));
        xport_gte_write_control(1, r_u32(context + 4));
        xport_gte_write_control(3, r_u32(context + 8));
        xport_gte_write_control(4, r_u32(context + 12));
        xport_gte_write_data(9, r_u16(normal));
        xport_gte_write_data(10, r_u16(normal + 2));
        xport_gte_write_data(11, r_u16(normal + 4));
        xport_gte_execute(0x41E012);
        x = xport_gte_read_data(25);
        z = xport_gte_read_data(27);
        distance1 = (uint32)((sint32)x >> 15) + (uint32)(sint32)r_s16(face + 8);
        distance2 = (uint32)((sint32)z >> 15) + (uint32)(sint32)r_s16(face + 8);
        if ((sint32)distance1 < 0 || (sint32)(distance1 ^ distance2) >= 0) continue;
        if ((sint32)distance1 < 0) distance1 = 0 - distance1;
        if ((sint32)distance2 < 0) distance2 = 0 - distance2;
        numerator = distance1 << 12;
        divisor = distance1 + distance2;
        if (!divisor) moh_draft_fail_fast("Original segment division BREAK 0x1C00", 0x8003DF20);
        if (divisor == 0xFFFFFFFF && numerator == 0x80000000)
            moh_draft_fail_fast("Original segment division BREAK 0x1800", 0x8003DF20);
        fraction = (uint32)((sint32)numerator / (sint32)divisor);
        if ((sint32)fraction >= r_s32(context + 0x50)) continue;
        xport_gte_write_data(8, fraction);
        xport_gte_write_data(9, r_u16(context + 0x18));
        xport_gte_write_data(10, r_u16(context + 0x1A));
        xport_gte_write_data(11, r_u16(context + 0x1C));
        xport_gte_execute(0x198003D);
        x = xport_gte_read_data(9);
        y = xport_gte_read_data(10);
        z = xport_gte_read_data(11);
        w_u16(context + 0x10, (uint16)x);
        w_u16(context + 0x12, (uint16)y);
        w_u16(context + 0x14, (uint16)z);
        w_u16(context + 0x10, (uint16)(r_u16(context + 0x10) + r_u16(context)));
        w_u16(context + 0x12, (uint16)(r_u16(context + 0x12) + r_u16(context + 2)));
        w_u16(context + 0x14, (uint16)(r_u16(context + 0x14) + r_u16(context + 4)));
        if (!moh_segment_edge(context, 0x64, 0x68)) continue;
        if (!moh_segment_edge(context, 0x68, 0x6C)) continue;
        if (!moh_segment_edge(context, 0x6C, 0x64)) continue;
        hit = r_u32(context + 0x70);
        w_u32(context + 0x50, fraction);
        w_u32(context + 0x4C, fraction);
        value = r_s16(r_u32(context + 0x3C) + iteration * 2);
        output = r_u32(context + 0x48);
        w_u32(context + 0x54, (uint32)value);
        moh_segment_copy8(output, context + 0x10);
        moh_segment_copy8(hit, output);
        output = r_u32(context + 0x44);
        normal = r_u32(context + 0x60);
        hit = r_u32(context + 0x70);
        moh_segment_copy8(output, normal);
        moh_segment_copy8(hit + 8, output);
        w_u16(r_u32(context + 0x44) + 6, r_u16(face + 8));
        hit = r_u32(context + 0x70);
        moh_segment_copy8(hit + 16, r_u32(context + 0x64));
        hit = r_u32(context + 0x70);
        moh_segment_copy8(hit + 24, r_u32(context + 0x68));
        hit = r_u32(context + 0x70);
        moh_segment_copy8(hit + 32, r_u32(context + 0x6C));
    }
    return r_u32(context + 0x54);
}


typedef struct MOH_SEGMENT_BRANCH
{
    sint16 child, minimum_x, maximum_x, minimum_z, maximum_z;
    sint32 start_x, end_x, start_z, end_z;
} MOH_SEGMENT_BRANCH;

static sint32 moh_segment_split_point(sint32 boundary, sint32 start, sint32 end, sint32 other_start, sint32 other_end)
{
    uint32 numerator, divisor, fraction = 0, product;
    divisor = (uint32)end - (uint32)start;
    if (divisor)
    {
        numerator = ((uint32)boundary - (uint32)start) << 16;
        if (divisor == 0xFFFFFFFF && numerator == 0x80000000)
            moh_draft_fail_fast("Original tree division BREAK 0x1800", 0x8003E700);
        fraction = (uint32)((sint32)numerator / (sint32)divisor);
    }
    product = ((uint32)other_end - (uint32)other_start) * fraction;
    return (sint32)((uint32)other_start + (uint32)((sint32)product >> 16));
}

uint32 level2p_sub_8003E700(uint32 start, uint32 end, uint32 normal, uint32 point, uint32 fraction_output, uint32 hit, uint32 flags)
{
    MOH_SEGMENT_BRANCH pending[8], *saved;
    uint32 group, nodes, node, depth = 0, value, list;
    sint32 minimum_x, maximum_x, minimum_z, maximum_z, start_x, end_x, start_z, end_z;
    sint32 first, second, boundary, crossing, child, selected = -1;
    FUNCTION_MARKER(0x8003E700, "LEVEL2P.BIN");
    moh_segment_copy8(0x1F800000, start);
    moh_segment_copy8(0x1F800008, end);
    end_x = r_s16(0x1F800008);
    start_x = r_s16(0x1F800000);
    start_z = r_s16(0x1F800004);
    end_z = r_s16(0x1F80000C);
    group = r_u32(0x800AAC38);
    value = r_u32(group + 0x24);
    w_u32(0x1F800044, normal);
    w_u32(0x1F800048, point);
    w_u32(0x1F800070, hit);
    w_u32(0x1F800074, flags);
    w_u32(0x1F800038, value);
    minimum_x = r_s16(group);
    maximum_x = r_s16(group + 4);
    minimum_z = r_s16(group + 2);
    maximum_z = r_s16(group + 6);
    nodes = r_u32(group + 0x14);
    w_u32(0x1F80004C, 32767);
    node = nodes;
    for (;;)
    {
        first = r_s16(node);
        if (first < 0)
        {
            group = r_u32(0x800AAC38);
            list = r_u32(group + 0x18) + ((uint32)(sint32)r_s16(node + 2) << 1);
            w_u32(0x1F80003C, list);
            value = (uint32)(sint32)r_s16(node);
            w_u32(0x8008D158, r_u32(0x8008D158) - 1 - (uint32)first);
            w_u32(0x1F800040, ~value);
            /* Original8001535C only swaps SP; semantic local branch storage stays native */
            value = level2p_sub_8003DF20(0x1F800000);
            if ((sint32)value >= 0) selected = (sint32)value;
            if (selected >= 0 || !depth)
            {
                w_u32(fraction_output, r_u32(0x1F80004C));
                return (uint32)selected;
            }
            saved = &pending[--depth];
            minimum_x = saved->minimum_x;
            maximum_x = saved->maximum_x;
            minimum_z = saved->minimum_z;
            maximum_z = saved->maximum_z;
            start_x = saved->start_x;
            end_x = saved->end_x;
            start_z = saved->start_z;
            end_z = saved->end_z;
            node = nodes + ((uint32)(sint32)saved->child << 3);
            continue;
        }
        second = r_s16(node + 2);
        if (second >= 0)
        {
            boundary = (sint32)((uint32)minimum_x + r_u16(node + 4));
            if (start_x < boundary)
            {
                if (boundary < end_x && depth < 8)
                {
                    crossing = moh_segment_split_point(boundary, start_x, end_x, start_z, end_z);
                    saved = &pending[depth++];
                    saved->child = (sint16)r_u16(node + 2);
                    saved->minimum_x = (sint16)boundary;
                    saved->maximum_x = (sint16)maximum_x;
                    saved->minimum_z = (sint16)minimum_z;
                    saved->maximum_z = (sint16)maximum_z;
                    saved->start_x = boundary;
                    saved->end_x = end_x;
                    saved->start_z = crossing;
                    saved->end_z = end_z;
                    end_x = boundary;
                    end_z = crossing;
                }
                maximum_x = (sint32)((uint32)boundary - 1);
                child = r_s16(node);
            }
            else
            {
                if (end_x < boundary && depth < 8)
                {
                    crossing = moh_segment_split_point(boundary, start_x, end_x, start_z, end_z);
                    saved = &pending[depth++];
                    saved->child = (sint16)r_u16(node);
                    saved->minimum_x = (sint16)minimum_x;
                    saved->maximum_x = (sint16)((uint32)boundary - 1);
                    saved->minimum_z = (sint16)minimum_z;
                    saved->maximum_z = (sint16)maximum_z;
                    saved->start_x = boundary;
                    saved->end_x = end_x;
                    saved->start_z = crossing;
                    saved->end_z = end_z;
                    end_x = boundary;
                    end_z = crossing;
                }
                minimum_x = boundary;
                child = r_s16(node + 2);
            }
        }
        else
        {
            boundary = (sint32)((uint32)minimum_z + r_u16(node + 4));
            if (start_z < boundary)
            {
                if (boundary < end_z && depth < 8)
                {
                    crossing = moh_segment_split_point(boundary, start_z, end_z, start_x, end_x);
                    saved = &pending[depth++];
                    saved->child = (sint16)~r_u16(node + 2);
                    saved->minimum_x = (sint16)minimum_x;
                    saved->maximum_x = (sint16)maximum_x;
                    saved->minimum_z = (sint16)boundary;
                    saved->maximum_z = (sint16)maximum_z;
                    saved->start_x = crossing;
                    saved->end_x = end_x;
                    saved->start_z = boundary;
                    saved->end_z = end_z;
                    end_x = crossing;
                    end_z = boundary;
                }
                maximum_z = (sint32)((uint32)boundary - 1);
                child = r_s16(node);
            }
            else
            {
                if (end_z < boundary && depth < 8)
                {
                    crossing = moh_segment_split_point(boundary, start_z, end_z, start_x, end_x);
                    saved = &pending[depth++];
                    saved->child = (sint16)r_u16(node);
                    saved->minimum_x = (sint16)minimum_x;
                    saved->maximum_x = (sint16)maximum_x;
                    saved->minimum_z = (sint16)minimum_z;
                    saved->maximum_z = (sint16)((uint32)boundary - 1);
                    saved->start_x = crossing;
                    saved->end_x = end_x;
                    saved->start_z = boundary;
                    saved->end_z = end_z;
                    end_x = crossing;
                    end_z = boundary;
                }
                minimum_z = boundary;
                child = ~r_s16(node + 2);
            }
        }
        node = nodes + ((uint32)child << 3);
    }
}


uint32 level2p_sub_8003EDDC(uint32 start, uint32 end, uint32 normal, uint32 point, uint32 fraction_output, uint32 hit, uint32 flags)
{
    sint16 first[4] = { 0 }, second[4] = { 0 };
    uint32 first_view, second_view;
    FUNCTION_MARKER(0x8003EDDC, "LEVEL2P.BIN");
    first[0] = r_s16(end + 2);
    first[1] = r_s16(end + 6);
    first[2] = r_s16(end + 10);
    second[0] = r_s16(start + 2);
    second[1] = r_s16(start + 6);
    second[2] = r_s16(start + 10);
    /* Unused vector padding stays native instead of reproducing guest stack bytes */
    /* TODO: Addressable vectors require the permitted native adapter */
    first_view = moh_draft_local_address(first, sizeof(first));
    second_view = moh_draft_local_address(second, sizeof(second));
    return level2p_sub_8003E700(first_view, second_view, normal, point, fraction_output, hit, flags);
}

uint32 level2p_sub_8003EE54(uint32 source)
{
    uint32 group, value, nodes, indices, vertices;
    sint32 node_count, index_count, vertex_count, normal_count;
    FUNCTION_MARKER(0x8003EE54, "LEVEL2P.BIN");
    group = r_u32(0x800AAC38);
    w_u16(group, r_u16(source));
    w_u16(group + 2, r_u16(source + 2));
    w_u16(group + 4, r_u16(source + 4));
    w_u16(group + 6, r_u16(source + 6));
    w_u16(group + 8, r_u16(source + 8));
    value = r_u16(source + 10);
    node_count = r_s16(group + 8);
    w_u16(group + 10, (uint16)value);
    w_u16(group + 14, r_u16(source + 12));
    w_u16(group + 16, r_u16(source + 14));
    index_count = r_s16(group + 10);
    value = r_u16(source + 16);
    nodes = source + 18;
    indices = nodes + ((uint32)node_count << 3);
    w_u32(group + 0x14, nodes);
    w_u32(group + 0x18, indices);
    vertices = indices + ((uint32)index_count << 1);
    w_u16(group + 12, (uint16)value);
    if (vertices & 3) vertices += 2;
    vertex_count = r_s16(group + 14);
    normal_count = r_s16(group + 16);
    w_u32(group + 0x1C, vertices);
    vertices += (uint32)vertex_count << 3;
    w_u32(group + 0x20, vertices);
    w_u32(group + 0x24, vertices + ((uint32)normal_count << 3));
    return 1;
}

uint32 level2p_sub_8003EFF4(uint32 start, uint32 end, uint32 index)
{
    uint32 group, dx, dz, right_product, top_right, bottom_right, top_left, bottom_left;
    sint32 x1, x2, z1, z2, boundary;
    FUNCTION_MARKER(0x8003EFF4, "LEVEL2P.BIN");
    group = r_u32(0x800AAC48 + index * 4);
    x1 = r_s16(start + 2);
    x2 = r_s16(end + 2);
    z1 = r_s16(start + 10);
    boundary = r_s16(group + 6);
    dx = (uint32)x2 - (uint32)x1;
    top_right = ((uint32)boundary - (uint32)z1) * dx;
    z2 = r_s16(end + 10);
    boundary = r_s16(group + 4);
    dz = (uint32)z2 - (uint32)z1;
    right_product = ((uint32)boundary - (uint32)x1) * dz;
    top_right -= right_product;
    bottom_right = ((uint32)(sint32)r_s16(group + 2) - (uint32)z1) * dx - right_product;
    if ((sint32)(top_right ^ bottom_right) <= 0 && boundary < x1 && x2 < boundary) return group;
    boundary = r_s16(group + 6);
    top_left = ((uint32)boundary - (uint32)z1) * dx - ((uint32)(sint32)r_s16(group) - (uint32)x1) * dz;
    if ((sint32)(top_right ^ top_left) <= 0 && boundary < z1 && z2 < boundary) return group;
    boundary = r_s16(group + 2);
    bottom_left = ((uint32)boundary - (uint32)z1) * dx - ((uint32)(sint32)r_s16(group) - (uint32)x1) * dz;
    if ((sint32)(bottom_left ^ top_left) <= 0 && r_s16(group) < x1 && x2 < r_s16(group)) return group;
    if ((sint32)(bottom_left ^ bottom_right) <= 0 && boundary < z1 && z2 < boundary) return group;
    return 0;
}

uint32 level2p_sub_8003F26C(uint32 start, uint32 end, uint32 normal, uint32 point, uint32 fraction_output, uint32 hit, uint32 flags)
{
    uint32 start_data[3], normal_data[2], point_data[2], hit_data[10], fraction[2];
    uint32 start_view, normal_view, point_view, hit_view, fraction_view, index = 0, group, selected = 0xFFFFFFFF, candidate, block;
    sint32 best = 32767;
    FUNCTION_MARKER(0x8003F26C, "LEVEL2P.BIN");
    level2p_sub_8003EF14();
    start_data[0] = r_u32(start);
    start_data[1] = r_u32(start + 4);
    start_data[2] = r_u32(start + 8);
    /* TODO: Addressable query locals require the permitted native adapter */
    while ((sint32)index < r_s32(0x800AAC3C))
    {
        group = level2p_sub_8003EF88((uint32)((sint32)start_data[0] >> 16), (uint32)((sint32)start_data[2] >> 16), index);
        w_u32(0x800AAC38, group);
        if (!group)
        {
            group = level2p_sub_8003EF88((uint32)(sint32)r_s16(end + 2), (uint32)(sint32)r_s16(end + 10), index);
            w_u32(0x800AAC38, group);
            if (!group)
            {
                start_view = moh_draft_local_address(start_data, sizeof(start_data));
                group = level2p_sub_8003EFF4(start_view, end, index);
                w_u32(0x800AAC38, group);
            }
        }
        if (group)
        {
            start_view = moh_draft_local_address(start_data, sizeof(start_data));
            normal_view = moh_draft_local_address(normal_data, sizeof(normal_data));
            point_view = moh_draft_local_address(point_data, sizeof(point_data));
            fraction_view = moh_draft_local_address(fraction, sizeof(fraction));
            hit_view = moh_draft_local_address(hit_data, sizeof(hit_data));
            candidate = level2p_sub_8003EDDC(start_view, end, normal_view, point_view, fraction_view, hit_view, flags);
            if ((sint32)fraction[0] < best)
            {
                best = (sint32)fraction[0];
                w_u32(normal, normal_data[0]);
                w_u32(normal + 4, normal_data[1]);
                w_u32(point, point_data[0]);
                w_u32(point + 4, point_data[1]);
                w_u32(fraction_output, fraction[0]);
                for (block = 0; block < 8; block += 4)
                {
                    w_u32(hit + block * 4, hit_data[block]);
                    w_u32(hit + block * 4 + 4, hit_data[block + 1]);
                    w_u32(hit + block * 4 + 8, hit_data[block + 2]);
                    w_u32(hit + block * 4 + 12, hit_data[block + 3]);
                }
                w_u32(hit + 32, hit_data[8]);
                w_u32(hit + 36, hit_data[9]);
                selected = candidate;
            }
        }
        index++;
    }
    return selected;
}

uint32 level2p_sub_8003F4C8(uint32 source, uint32 index)
{
    FUNCTION_MARKER(0x8003F4C8, "LEVEL2P.BIN");
    w_u32(0x800AAC38, 0x8008D0B8 + index * 40);
    return level2p_sub_8003EE54(source);
}

uint32 level2p_sub_8003FA98(uint32 owner)
{
    uint32 data, index, entry, count;
    FUNCTION_MARKER(0x8003FA98, "LEVEL2P.BIN");
    data = r_u32(owner + 12);
    if (!data)
    {
        w_u32(owner + 12, 0);
        return 0xFFFFFFFF;
    }
    for (index = 0; (sint32)index < r_s32(owner + 0xB0); index++)
        w_u32(r_u32(owner + 0xB4) + index * 60, 0);
    count = r_u16(data);
    w_u16(owner + 0xBC, (uint16)count);
    count = r_u16(data + 4);
    w_u32(owner + 0xB8, data + 8);
    w_u16(owner + 0x94, (uint16)count);
    for (index = 0; index < r_u16(owner + 0xBC); index++)
    {
        entry = r_u32(owner + 0xB8) + index * 16;
        w_u32(entry + 12, data + r_u32(entry + 12));
    }
    return 0;
}

uint32 level2p_sub_8003FB68(uint32 owner, uint32 target)
{
    uint32 x, z, angle;
    sint32 heading;
    FUNCTION_MARKER(0x8003FB68, "LEVEL2P.BIN");
    x = r_u32(target + 0x20C) - r_u32(owner + 0x20C);
    heading = r_s16(owner + 0x22E);
    x = (uint32)((sint32)x >> 16);
    z = r_u32(target + 0x214) - r_u32(owner + 0x214);
    z = (uint32)((sint32)z >> 16);
    angle = moh_draft_missing_call(0x8001536C, x, z) - (uint32)heading;
    while ((sint16)angle < -2048) angle += 4096;
    while ((sint16)angle > 2048) angle -= 4096;
    return (uint32)(sint32)(sint16)angle;
}


uint32 level2p_sub_800402B4(uint32 owner)
{
    uint32 data, flags, mask, angle;
    FUNCTION_MARKER(0x800402B4, "LEVEL2P.BIN");
    data = r_u32(owner + 0x38C);
    flags = r_u32(data + 8);
    if ((flags & 0x18) == 0x18)
    {
        mask = r_s32(data + 0x11C) < r_s32(data + 0x10C) ? 0xFFFFFFF7 : 0xFFFFFFEF;
        flags &= mask;
    }
    if (flags & 4)
    {
        w_u32(owner + 0x238, 0);
        if (flags & 0x10)
        {
            if (!(flags & 2))
            {
                angle = moh_draft_missing_call(0x8001536C, (uint32)(sint32)r_s16(data + 0xF4), (uint32)(sint32)r_s16(data + 0xF8));
                w_u32(owner + 0x22C, (angle + 0x400) << 16);
            }
            w_u32(data + 8, r_u32(data + 8) & 0xFFFFFFFE);
        }
        else if (flags & 8)
        {
            if (!(flags & 1))
            {
                angle = moh_draft_missing_call(0x8001536C, (uint32)(sint32)r_s16(data + 0xF4), (uint32)(sint32)r_s16(data + 0xF8));
                w_u32(owner + 0x22C, (angle - 0x400) << 16);
            }
            w_u32(data + 8, r_u32(data + 8) & 0xFFFFFFFD);
        }
        else
        {
            angle = moh_draft_missing_call(0x8001536C, (uint32)(sint32)r_s16(data + 0xF4), (uint32)(sint32)r_s16(data + 0xF8));
            if (r_s32(data + 0x100) > 0) { angle -= 0x400; mask = 0xFFFFFFFD; }
            else { angle += 0x400; mask = 0xFFFFFFFE; }
            w_u32(data + 8, r_u32(data + 8) & mask);
            w_u32(owner + 0x22C, angle << 16);
        }
        return 0;
    }
    if (flags & 8)
    {
        if (!(flags & 1))
        {
            angle = moh_draft_missing_call(0x8001536C, (uint32)(sint32)r_s16(data + 0x104), (uint32)(sint32)r_s16(data + 0x108));
            w_u32(owner + 0x22C, (angle - 0x400) << 16);
        }
        w_u32(data + 8, r_u32(data + 8) & 0xFFFFFFFD);
        return 0;
    }
    if (flags & 0x10)
    {
        if (!(flags & 2))
        {
            angle = moh_draft_missing_call(0x8001536C, (uint32)(sint32)r_s16(data + 0x114), (uint32)(sint32)r_s16(data + 0x118));
            w_u32(owner + 0x22C, (angle + 0x400) << 16);
        }
        w_u32(data + 8, r_u32(data + 8) & 0xFFFFFFFE);
        return 0;
    }
    return 1;
}

void level2p_sub_80040490(uint32 owner, uint32 forward, uint32 direction, uint32 step, uint32 distance)
{
    uint32 data, product, numerator, ratio, element, angle, cross[3], index, amount = step;
    sint32 divisor, turn;
    FUNCTION_MARKER(0x80040490, "LEVEL2P.BIN");
    data = r_u32(owner + 0x38C);
    xport_gte_write_control(0, (uint32)(sint32)r_s16(forward));
    xport_gte_write_control(2, (uint32)(sint32)r_s16(forward + 2));
    xport_gte_write_control(4, (uint32)(sint32)r_s16(forward + 4));
    xport_gte_write_data(9, (uint32)(sint32)r_s16(direction));
    xport_gte_write_data(10, (uint32)(sint32)r_s16(direction + 2));
    xport_gte_write_data(11, (uint32)(sint32)r_s16(direction + 4));
    xport_gte_execute(0x178000C);
    for (index = 0; index < 3; index++) cross[index] = xport_gte_read_data(25 + index);
    divisor = r_s16(data + 0xC2);
    if (divisor)
    {
        if (divisor < (sint32)distance)
        {
            w_u32(owner + 0x224, 0x10000);
            ratio = r_u32(data + 0xDC);
            product = r_u16(data + 0xC4);
            w_u32(data + 0x18, ratio);
            w_u16(owner + 0x2EC, (uint16)product);
            w_u16(owner + 0x2E8, r_u16(data + 0xC6));
            element = data + ((uint32)(sint32)r_s16(data + 0xC0) << 1);
            product = r_u16(element + 0xD0);
        }
        else
        {
            numerator = distance << 16;
            if (divisor == -1 && numerator == 0x80000000)
                moh_draft_fail_fast("Original movement division BREAK 0x1800", 0x80040490);
            ratio = (uint32)((sint32)numerator / divisor);
            w_u32(owner + 0x224, ratio);
            product = (uint32)(sint32)r_s16(data + 0xC4);
            w_u32(data + 0x18, ratio);
            product *= r_u32(owner + 0x224);
            w_u16(owner + 0x2EC, (uint16)(product >> 16));
            product = (uint32)(sint32)r_s16(data + 0xCA) * r_u32(owner + 0x224);
            w_u16(owner + 0x3CC, (uint16)(product >> 16));
            element = data + ((uint32)(sint32)r_s16(data + 0xC0) << 1);
            ratio = r_u32(owner + 0x224);
            product = (uint32)(sint32)r_s16(element + 0xD0) * ratio;
            product = (uint32)((sint32)product >> 16);
        }
        w_u16(element + 0xCC, (uint16)product);
    }
    for (index = 0; index < 3; index++) cross[index] = (uint32)((sint32)cross[index] >> ((sint32)distance < 2001 ? 1 : 2));
    turn = (sint32)cross[1];
    if (turn < -32)
    {
        w_u32(data + 8, r_u32(data + 8) | 2);
        amount = 0 - amount;
        if ((sint32)((uint32)turn + (uint32)(sint32)(sint16)step) >= 0) amount = (uint16)turn;
    }
    else if (turn > 32)
    {
        w_u32(data + 8, r_u32(data + 8) | 1);
        if ((sint32)((uint32)turn - (uint32)(sint32)(sint16)step) <= 0) amount = (uint16)turn;
    }
    else
    {
        xport_gte_write_control(0, r_u32(direction));
        xport_gte_write_control(1, r_u32(direction + 4));
        xport_gte_write_data(9, r_u16(forward));
        xport_gte_write_data(10, r_u16(forward + 2));
        xport_gte_write_data(11, r_u16(forward + 4));
        xport_gte_execute(0x41E012);
        if ((sint32)xport_gte_read_data(25) < 0)
        {
            if (turn < 0) { amount = 0 - amount; w_u32(data + 8, r_u32(data + 8) | 2); }
            else w_u32(data + 8, r_u32(data + 8) | 1);
        }
        else amount = 0;
    }
    if (r_u32(data + 4) & 4) level2p_sub_800402B4(owner);
    if (r_u32(data + 8) & 3)
    {
        angle = r_u32(owner + 0x22C) + (amount << 16);
        w_u32(owner + 0x22C, angle);
        while ((sint32)angle < -0x8000000) angle += 0x10000000;
        while ((sint32)angle > 0x8000000) angle -= 0x10000000;
        w_u32(owner + 0x22C, angle);
    }
}

void level2p_sub_800407F8(uint32 owner, uint32 target, uint32 parameter3, uint32 reverse)
{
    sint16 forward[4] = { 0 }, direction[4] = { 0 };
    uint32 step, forward_view, direction_view;
    sint32 x, z, distance;
    FUNCTION_MARKER(0x800407F8, "LEVEL2P.BIN");
    step = r_u16(r_u32(owner + 0x38C) + 0xD4);
    if (!target || (r_u32(target + 12) & 0x180000))
    {
        w_u32(owner + 0x38, r_u32(owner + 0x38) | 0x10);
        return;
    }
    forward[0] = (sint16)r_u16(owner + 0x344);
    forward[2] = (sint16)r_u16(owner + 0x350);
    if (reverse) { forward[0] = (sint16)-forward[0]; forward[2] = (sint16)-forward[2]; }
    direction[0] = (sint16)(r_s16(target + 0x20E) - r_s16(owner + 0x20E));
    direction[2] = (sint16)(r_s16(target + 0x216) - r_s16(owner + 0x216));
    x = direction[0] < 0 ? -direction[0] : direction[0];
    z = direction[2] < 0 ? -direction[2] : direction[2];
    distance = x < z ? z + (x >> 2) : x + (z >> 2);
    /* TODO: Addressable direction vectors require the permitted native adapter */
    forward_view = moh_draft_local_address(forward, sizeof(forward));
    direction_view = moh_draft_local_address(direction, sizeof(direction));
    level2p_sub_80040490(owner, forward_view, direction_view, (uint32)(sint32)(sint16)step, (uint32)distance);
}

void level2p_sub_80040928(uint32 owner, uint32 index, uint32 property, uint32 threshold)
{
    uint16 target[3];
    sint16 direction[4] = { 0 }, forward[4] = { 0 };
    uint32 data, table, record, plane1, plane2, forward_view, direction_view;
    sint32 x, z, distance;
    FUNCTION_MARKER(0x80040928, "LEVEL2P.BIN");
    data = r_u32(owner + 0x38C);
    table = r_u32(r_u32(r_u32(0x80099430) + 0x388) + 8);
    if (r_u32(data + 0x8C))
    {
        target[0] = r_u16(data + 0xAC);
        target[1] = r_u16(table + index * 52 + 22);
        target[2] = r_u16(data + 0xAE);
    }
    else
    {
        target[0] = r_u16(data + 0xA0);
        target[1] = r_u16(data + 0xA2);
        target[2] = r_u16(data + 0xA4);
    }
    direction[0] = (sint16)(target[0] - r_s16(owner + 0x20E));
    direction[2] = (sint16)(target[2] - r_s16(owner + 0x216));
    forward[0] = (sint16)r_u16(owner + 0x344);
    forward[2] = (sint16)r_u16(owner + 0x350);
    x = direction[0] < 0 ? -direction[0] : direction[0];
    z = direction[2] < 0 ? -direction[2] : direction[2];
    distance = x < z ? z + (x >> 2) : x + (z >> 2);
    level2p_sub_8005A1B8(owner, property, (uint32)distance);
    record = table + index * 52;
    plane1 = r_u32(record + 0x24) & 0xFFFF;
    plane2 = r_u32(record + 0x28);
    if (r_u32(data + 0x8C))
    {
        if (distance >= (sint16)threshold)
        {
            xport_gte_write_control(0, plane1);
            xport_gte_write_control(1, plane2);
            xport_gte_write_data(9, (uint16)-direction[0]);
            xport_gte_write_data(10, (uint16)-direction[1]);
            xport_gte_write_data(11, (uint16)-direction[2]);
            xport_gte_execute(0x41E012);
            if ((sint32)xport_gte_read_data(25) < 0) w_u32(data + 0x8C, 0);
        }
        else w_u32(data + 0x8C, 0);
    }
    else if (distance < (sint16)threshold)
    {
        level2p_sub_8005A1B8(owner, property, 0);
        w_u32(owner + 0x38, r_u32(owner + 0x38) | 0x400000);
    }
    /* TODO: Addressable direction vectors require the permitted native adapter */
    forward_view = moh_draft_local_address(forward, sizeof(forward));
    direction_view = moh_draft_local_address(direction, sizeof(direction));
    level2p_sub_80040490(owner, forward_view, direction_view, 256, (uint32)distance);
}

void level2p_sub_8005A1B8(uint32 owner, uint32 property, uint32 value)
{
    uint32 data, first, second, third, field, bit, word;
    FUNCTION_MARKER(0x8005A1B8, "LEVEL2P.BIN");
    if (property - 0x60 < 32)
    {
        if (property != 0x6A) return;
        data = r_u32(owner + 0x38C);
        if (!data) return;
        if (value)
        {
            first = r_u32(data + 0xE0); second = r_u32(data + 0xE4); third = r_u32(data + 0xE8);
            w_u32(owner + 0x20C, first); w_u32(owner + 0x210, second); w_u32(owner + 0x214, third);
        }
        else
        {
            first = r_u32(owner + 0x20C); second = r_u32(owner + 0x210); third = r_u32(owner + 0x214);
            w_u32(data + 0xE0, first); w_u32(data + 0xE4, second); w_u32(data + 0xE8, third);
        }
        return;
    }
    if (!(property & 0x1C0)) { w_u32(owner + property * 4 + 0x4C, value); return; }
    if (property & 0x100)
    {
        property &= 0xFFFFFEFF;
        field = owner + (property >> 5) * 4 + 0xEC;
        bit = (uint32)1 << (property & 31);
    }
    else if (property & 0x40) { field = owner + 12; bit = (uint32)1 << ((property - 0x40) & 31); }
    else if (property & 0x80) { field = owner + 16; bit = (uint32)1 << ((property - 0x80) & 31); }
    else return;
    word = r_u32(field) | bit;
    w_u32(field, word);
    if (!value) word ^= bit;
    w_u32(field, word);
}


void level2p_sub_80040BC4(uint32 owner, uint32 target, uint32 property, uint32 threshold)
{
    sint16 forward[4] = { 0 }, direction[4] = { 0 };
    sint32 x, z, distance;
    uint32 forward_view, direction_view;
    FUNCTION_MARKER(0x80040BC4, "LEVEL2P.BIN");
    direction[0] = (sint16)(r_u16(target) - r_s16(owner + 0x20E));
    direction[2] = (sint16)(r_u16(target + 4) - r_s16(owner + 0x216));
    forward[0] = (sint16)r_u16(owner + 0x344);
    forward[2] = (sint16)r_u16(owner + 0x350);
    x = direction[0] < 0 ? -direction[0] : direction[0];
    z = direction[2] < 0 ? -direction[2] : direction[2];
    distance = x < z ? z + (x >> 2) : x + (z >> 2);
    level2p_sub_8005A1B8(owner, property, (uint32)distance);
    if (distance < (sint16)threshold)
    {
        level2p_sub_8005A1B8(owner, property, 0);
        w_u32(owner + 0x38, r_u32(owner + 0x38) | 0x400000);
    }
    /* TODO: Addressable direction vectors require the permitted native adapter */
    forward_view = moh_draft_local_address(forward, sizeof(forward));
    direction_view = moh_draft_local_address(direction, sizeof(direction));
    level2p_sub_80040490(owner, forward_view, direction_view, 256, (uint32)distance);
}

void level2p_sub_80041104(uint32 owner)
{
    uint32 data, flags, old_flags, value, movement, current;
    FUNCTION_MARKER(0x80041104, "LEVEL2P.BIN");
    data = r_u32(owner + 0x38C);
    w_u32(data + 0x94, r_u32(owner + 0x20C));
    value = r_u32(owner + 0x210);
    flags = r_u32(data + 4);
    w_u32(data + 0x98, value);
    old_flags = r_u32(data + 8);
    value = r_u32(owner + 0x214);
    movement = r_u32(data + 4) & 4;
    w_u32(data + 8, old_flags & 0xFFFFFFFC);
    w_u32(data + 0x9C, value);
    if (!movement) w_u32(data + 8, old_flags & 0xFFFFFFE0);
    if (flags & 1) level2p_sub_800407F8(owner, r_u32(data + 0x10), r_u32(data + 0x50), 0);
    if (flags & 0x4000) level2p_sub_800407F8(owner, r_u32(data + 0x10), r_u32(data + 0x50), 1);
    if (flags & 0x1000)
    {
        current = r_u32(owner + 0x38C);
        value = r_u32(data + 0x80);
        level2p_sub_80040BC4(owner, r_u32(current + 0x40), value, (uint32)(sint32)r_s16(current + 0xA8));
    }
    if (flags & 0x800)
        level2p_sub_80040928(owner, r_u32(data + 0x3C), r_u32(data + 0x7C), (uint32)(sint32)r_s16(data + 0xA8));
    if ((flags & 4) && !(flags & 0x5C03)) level2p_sub_800402B4(owner);
    if (flags & 2) level2p_sub_80040D1C(owner);
    current = r_u32(owner + 0x38C);
    w_u32(current + 8, r_u32(current + 8) & 0xFFFFFFDF);
}


void level2p_sub_80041628(void)
{
    sint32 index;
    FUNCTION_MARKER(0x80041628, "LEVEL2P.BIN");
    /* TODO: Missing SDK calls retain permitted fail-fast boundaries */
    moh_draft_missing_call(0x8001582C);
    moh_draft_missing_call(0x80015B68, 0x3FFF, 0x3FFF);
    moh_draft_missing_call(0x8001586C, 1);
    moh_draft_missing_call(0x80015BE8, 0x7FFF, 0x7FFF);
    level2p_sub_80042B88();
    for (index = 7; index >= 0; --index)
        w_u32(0x8008D40C + (uint32)index * 4, 0);
    w_u32(0x8009F470, 0x1010);
    w_u32(0x8008D2AC, 0);
    level2p_sub_80089158();
}

void level2p_sub_800416A8(void)
{
    uint32 index;
    FUNCTION_MARKER(0x800416A8, "LEVEL2P.BIN");
    for (index = 1; index < 24; ++index) level2p_sub_80042714(index);
    w_u32(0x8008D2AC, 0);
}

void level2p_sub_800416E4(void)
{
    uint32 index;
    FUNCTION_MARKER(0x800416E4, "LEVEL2P.BIN");
    for (index = 1; index < 24; ++index) level2p_sub_80042714(index);
    w_u32(0x8008D2AC, 0);
    for (index = 0; index < 8; ++index) level2p_sub_80041C94(index);
    w_u32(0x8009F470, 0xB010);
}

void level2p_sub_80041C94(uint32 index)
{
    uint32 field;
    FUNCTION_MARKER(0x80041C94, "LEVEL2P.BIN");
    field = 0x8008D40C + index * 4;
    if (r_u32(field)) w_u32(field, 0);
}

void level2p_sub_80042B88(void)
{
    uint32 index;
    FUNCTION_MARKER(0x80042B88, "LEVEL2P.BIN");
    for (index = 1; index < 8; ++index)
    {
        w_u32(0x8008D40C + index * 4, 0);
        w_u16(0x8008D42C + index * 2, 0xFFFF);
    }
}

void level2p_sub_80089158(void)
{
    uint32 initial;
    FUNCTION_MARKER(0x80089158, "LEVEL2P.BIN");
    initial = r_u32(0x8009F470);
    w_u32(0x8009F484, 0);
    w_u32(0x8009F474, initial);
    w_u32(0x8009F478, initial + 0x5000);
    w_u32(0x8009F470, initial + 0xA000);
}


uint32 level2p_sub_80041CC0(uint32 bank, uint32 slot)
{
    uint32 offset, size, field, data, destination, index, output, value;
    uint32 signed_slot = (uint32)(sint32)(sint16)slot;
    FUNCTION_MARKER(0x80041CC0, "LEVEL2P.BIN");
    offset = (uint32)r_u16(bank + 0x12) * 0x200 + 0xA20;
    field = 0x8008D3CC + signed_slot * 4;
    size = r_u32(bank + 12);
    value = r_u32(field + 0x40);
    size -= offset;
    if (!value)
    {
        if ((uint16)slot >= 8) return 0xFFFFFFFF;
        if (r_u32(bank) != 0x56414270) return 0xFFFFFFFF;
        if ((sint32)(r_u32(0x8009F470) + size) > 0x7FFFF)
        {
            level2p_sub_800885B8(5, 0x10);
            return 0xFFFFFFFF;
        }
        data = bank + offset;
        /* TODO: Missing SDK transfer and copy calls retain fail-fast boundaries */
        moh_draft_missing_call(0x800156DC);
        moh_draft_missing_call(0x8001575C, data, size);
        moh_draft_missing_call(0x80015C88, 1);
        destination = 0x8008E920 + signed_slot * 0x200;
        w_u32(field + 0x20, destination);
        moh_draft_missing_call(0x800154EC, destination, data - 0x200, 0x200);
        w_u32(field, r_u32(0x8009F470));
        output = 0x8008D920 + signed_slot * 2;
        for (index = 0; index < 256; ++index)
        {
            value = level2p_sub_80042B04(index, r_u16(bank + 0x12), bank);
            w_u16(output, (uint16)value);
            output += 0x10;
        }
        value = r_u32(0x8009F470);
        w_u32(field + 0x40, 1);
        w_u32(0x8009F470, value + size);
    }
    return (uint32)(sint32)r_s16(0x8008D42C + signed_slot * 2);
}

uint32 level2p_sub_80042B04(uint32 sample, uint32 count, uint32 bank)
{
    uint32 index = 0, record = bank + 0x824, high, number, volume;
    FUNCTION_MARKER(0x80042B04, "LEVEL2P.BIN");
    count <<= 4;
    while ((sint32)index < (sint32)count)
    {
        high = r_u8(record + 0x13);
        number = (high << 8) | r_u8(record + 0x12);
        if (sample == number - 1)
        {
            volume = r_u16(0x8008D2B4 + ((uint32)r_u8(record) + 4) * 2);
            return volume < 0x1000 ? volume : 0x1000;
        }
        record += 0x20;
        ++index;
    }
    return 0x800;
}


void level2p_sub_80041744(void)
{
    uint32 mask = 0, index, bits, channel, record, handle, owner, age, value, shift;
    uint32 left_view, right_view, mask_view, quotient, next, product, bank, choice;
    uint32 volume, spread, pitch, variation;
    sint16 left = 0, right = 0, divisor;
    FUNCTION_MARKER(0x80041744, "LEVEL2P.BIN");
    moh_draft_missing_call(0x80015A9C, 0x8008D3B4);
    if (r_u32(0x8008D2AC))
    {
        moh_draft_missing_call(0x800158DC, 1);
        index = 0;
        while (r_u32(0x8008D2AC))
        {
            if (r_u32(0x8008D2AC) & 1) w_u8(0x8008D3B4 + index, 1);
            bits = r_u32(0x8008D2AC) >> 1;
            ++index;
            w_u32(0x8008D2AC, bits);
        }
        w_u32(0x8008D2AC, 0);
    }
    for (channel = 1; channel < 24; ++channel)
    {
        record = 0x8008D440 + channel * 52;
        if (!(r_u8(0x8008D3B4 + channel) & 1))
        {
            if (channel == r_u32(0x8008D2A0)) w_u32(0x8008D2A0, 0xFFFFFFFF);
            if (r_u8(record + 0xE))
            {
                w_u8(record + 0xE, 0);
                handle = r_u32(record + 4);
                owner = handle ? r_u32(0x80099460 + (handle & 0xFFFF) * 4) : 0;
                spread = r_u32(record + 0x1C);
                bank = (uint32)(sint32)r_s16(record + 0xC);
                pitch = r_u32(record + 0x20);
                variation = r_u32(record + 0x24);
                choice = r_u32(record + 8);
                volume = r_u32(record + 0x18);
                level2p_sub_800423A0(bank, choice, owner, volume, spread, pitch, variation, 0, 1, channel, 0);
            }
            else
            {
                level2p_sub_80068A04(record + 4, 0);
                w_u8(record + 0x12, 0);
                w_u16(record + 0x28, 0);
            }
            continue;
        }
        age = (r_u16(record + 0x28) + 1) & 0xFFFF;
        w_u16(record + 0x28, age);
        if (age >= 0x15F && !r_u8(record + 0xE))
        {
            level2p_sub_80042714(channel);
            continue;
        }
        if (!level2p_sub_80068A64(record + 4))
        {
            handle = r_u32(record + 4);
            owner = handle ? r_u32(0x80099460 + (handle & 0xFFFF) * 4) : 0;
            /* TODO: Original permits a zero resolved owner before this memory read */
            if (!(r_u32(owner + 0xC) & 0x80000))
            {
                handle = r_u32(record + 4);
                owner = handle ? r_u32(0x80099460 + (handle & 0xFFFF) * 4) : 0;
                w_u16(record + 0x2C, r_u16(owner + 0x20E));
                handle = r_u32(record + 4);
                owner = handle ? r_u32(0x80099460 + (handle & 0xFFFF) * 4) : 0;
                w_u16(record + 0x2E, r_u16(owner + 0x212));
                handle = r_u32(record + 4);
                owner = handle ? r_u32(0x80099460 + (handle & 0xFFFF) * 4) : 0;
                w_u16(record + 0x30, r_u16(owner + 0x216));
            }
            else level2p_sub_80068A04(record + 4, 0);
        }
        if (r_u8(0x8008D3B4 + channel) == 3 && !r_u8(record + 0x12))
        {
            mask |= (uint32)1 << (channel & 31);
            continue;
        }
        if (r_u8(record + 0x12))
        {
            /* TODO: Addressable output words require the permitted local adapter */
            left_view = moh_draft_local_address(&left, sizeof(left));
            right_view = moh_draft_local_address(&right, sizeof(right));
            moh_draft_missing_call(0x80015B88, channel, left_view, right_view);
            value = r_u8(record + 0x12);
            quotient = (uint32)(((uint64)value * 0x24924925) >> 32);
            shift = (((quotient + ((value - quotient) >> 1)) >> 2) & 0xFF) + 3;
            value = (uint16)left - (uint32)((sint32)left >> (shift & 31));
            next = (uint16)right - (uint32)((sint32)right >> (shift & 31));
            left = (sint16)value;
            right = (sint16)next;
            if (left < 0) left = 0;
            if (right < 0) right = 0;
            moh_draft_missing_call(0x80015C08, channel, (uint32)(sint32)left, (uint32)(sint32)right);
            value = (r_u8(record + 0x12) - 1) & 0xFF;
            w_u8(record + 0x12, value);
            if (!value)
            {
                mask_view = moh_draft_local_address(&mask, sizeof(mask));
                level2p_sub_800427B4(channel, mask_view);
            }
        }
        else if (!(r_u32(record) & 1) && r_u32(0x8008D2B0) == 3)
        {
            left_view = moh_draft_local_address(&left, sizeof(left));
            right_view = moh_draft_local_address(&right, sizeof(right));
            level2p_sub_80042E9C(channel, left_view, right_view, r_u8(record + 0x13));
            value = r_u8(record + 0x10);
            next = value + 1;
            if (value < r_u8(record + 0xF))
            {
                value = r_u8(record + 0xF);
                w_u8(record + 0x10, next);
                product = value * (next & 0xFF);
                if (!product) product = 1;
                divisor = (sint16)product;
                if (!divisor)
                {
                    moh_draft_fail_fast("Original volume division BREAK 0x1C00", 0x80041AF4);
                    return;
                }
                /* Signed16 numerators cannot trigger the original INT32_MIN overflow guards */
                left = (sint16)((sint32)left / divisor);
                right = (sint16)((sint32)right / divisor);
            }
            moh_draft_missing_call(0x80015C08, channel, (uint32)(sint32)left, (uint32)(sint32)right);
        }
    }
    value = r_u32(0x8008D2B0);
    w_u32(0x8008D2B0, value + 1);
    if ((sint32)value >= 3) w_u32(0x8008D2B0, 0);
    if (mask) moh_draft_missing_call(0x800158DC, 0);
}

uint32 level2p_sub_800423A0(uint32 bank, uint32 choice, uint32 owner, uint32 volume, uint32 spread, uint32 pitch, uint32 variation, uint32 parameter8, uint32 repeat, uint32 channel, uint32 mode)
{
    uint32 record, scaled, random, product, divisor, value, left_view, right_view;
    sint16 left = 0, right = 0;
    FUNCTION_MARKER(0x800423A0, "LEVEL2P.BIN");
    if ((uint16)bank >= 8 || (sint32)choice < 0) return 0xFFFFFFFF;
    if (!r_u32(0x8008D40C + (uint32)(sint32)(sint16)bank * 4)) return 0xFFFFFFFF;
    record = 0x8008D440 + channel * 52;
    scaled = volume << 7;
    w_u32(record + 0x14, scaled);
    if ((sint32)scaled > 0xFF0000) w_u32(record + 0x14, 0xFF0000);
    w_u8(record + 0xE, repeat);
    w_u16(record + 0x28, 0);
    w_u8(record + 0x13, mode);
    if (spread)
    {
        random = sub_8001552C();
        scaled = r_u32(record + 0x14);
        product = scaled * spread;
        divisor = product << 1;
        if (!divisor)
        {
            moh_draft_fail_fast("Original sound division BREAK 0x1C00", 0x8004248C);
            return channel;
        }
        if (divisor == 0xFFFFFFFF && random == 0x80000000)
        {
            moh_draft_fail_fast("Original sound division BREAK 0x1800", 0x800424A4);
            return channel;
        }
        w_u32(record + 0x14, scaled + product - (uint32)((sint32)random % (sint32)divisor));
    }
    w_u8(record + 0x12, 0);
    w_u8(record + 0x10, 0);
    if (owner && !(r_u32(owner + 0xC) & 0x40000000))
    {
        w_u16(record + 0x2C, r_u16(owner + 0x20E));
        w_u16(record + 0x2E, r_u16(owner + 0x212));
        w_u16(record + 0x30, r_u16(owner + 0x216));
        /* TODO: Addressable output words require the permitted local adapter */
        left_view = moh_draft_local_address(&left, sizeof(left));
        right_view = moh_draft_local_address(&right, sizeof(right));
        level2p_sub_80042E9C(channel, left_view, right_view, mode);
        if (!left && !right) left = right = 1;
    }
    else
    {
        scaled = r_u32(record + 0x14);
        w_u16(record + 0x2C, 0);
        w_u16(record + 0x2E, 0);
        w_u16(record + 0x30, 0);
        value = r_u8(0x8008FCDE);
        product = (uint32)((sint32)scaled >> 10) * value;
        w_u32(record, r_u32(record) | 1);
        left = right = (sint16)((sint32)product >> 8);
    }
    level2p_sub_80041EE0((uint32)(sint32)(sint16)bank, choice, channel,
        (uint32)(sint32)left, (uint32)(sint32)right, pitch, variation);
    return channel;
}

void level2p_sub_800427B4(uint32 channel, uint32 mask)
{
    uint32 record, value;
    FUNCTION_MARKER(0x800427B4, "LEVEL2P.BIN");
    if (!channel) return;
    moh_draft_missing_call(0x80015C08, channel, 0, 0);
    record = 0x8008D440 + channel * 52;
    value = r_u32(mask) | ((uint32)1 << (channel & 31));
    w_u32(mask, value);
    w_u8(0x8008D3B4 + channel, 0);
    w_u8(record + 0x12, 0);
    w_u8(record + 0xE, 0);
    level2p_sub_80068A04(record + 4, 0);
    w_u8(record + 0x11, 0xFF);
}


void level2p_sub_800425EC(uint32 owner, uint32 fade)
{
    uint32 channel, record, owner_view;
    FUNCTION_MARKER(0x800425EC, "LEVEL2P.BIN");
    /* TODO: Addressable owner word requires the permitted local adapter */
    owner_view = moh_draft_local_address(&owner, sizeof(owner));
    for (channel = 1; channel < 24; ++channel)
    {
        record = 0x8008D440 + channel * 52;
        if (level2p_sub_80068988(record + 4, owner_view))
        {
            w_u8(record + 0xF, 0);
            w_u8(record + 0x11, fade);
            w_u8(record + 0x12, fade);
        }
    }
}

void level2p_sub_80042674(uint32 owner)
{
    uint32 channel, record, owner_view, current;
    FUNCTION_MARKER(0x80042674, "LEVEL2P.BIN");
    /* TODO: Addressable owner word requires the permitted local adapter */
    owner_view = moh_draft_local_address(&owner, sizeof(owner));
    for (channel = 1; channel < 24; ++channel)
    {
        record = 0x8008D440 + channel * 52;
        if (level2p_sub_80068988(record + 4, owner_view))
        {
            current = owner;
            w_u16(record + 0x2C, r_u16(current + 0x20E));
            w_u16(record + 0x2E, r_u16(current + 0x212));
            w_u16(record + 0x30, r_u16(current + 0x216));
            level2p_sub_80068A04(record + 4, 0);
        }
    }
}

void level2p_sub_8004285C(uint32 bank, uint32 choice, uint32 kind, uint32 owner, uint32 volume, uint32 spread, uint32 pitch, uint32 variation, uint32 repeat, uint32 mode, uint32 parameter11, uint32 parameter12)
{
    uint32 record, next;
    FUNCTION_MARKER(0x8004285C, "LEVEL2P.BIN");
    record = 0x800AAC58 + r_u32(0x8008D29C) * 36;
    w_u8(record + 3, mode);
    w_u8(record, choice);
    w_u8(record + 1, kind);
    w_u8(record + 2, repeat);
    level2p_sub_80068A04(record + 4, owner);
    next = r_u32(0x8008D29C);
    w_u32(record + 8, volume);
    w_u32(record + 0xC, spread);
    w_u32(record + 0x10, pitch);
    w_u32(record + 0x14, variation);
    w_u32(record + 0x18, parameter11);
    w_u32(record + 0x1C, parameter12);
    w_u16(record + 0x20, 1);
    w_u16(record + 0x22, bank);
    w_u32(0x8008D29C, (next + 1) & 7);
}


void level2p_sub_8004294C(void)
{
    uint32 index, record, handle, owner, bank, choice, volume, spread, pitch, variation;
    uint32 repeat, mode, parameter10, parameter11, result, next, active, delay, clock, due;
    FUNCTION_MARKER(0x8004294C, "LEVEL2P.BIN");
    index = r_u32(0x8008D298);
    if (index == r_u32(0x8008D29C)) return;
    if (r_s32(0x8008D2A0) >= 0) return;
    if (!r_u32(0x8008D2A4))
    {
        record = 0x800AAC58 + index * 36;
        w_u32(0x8008D2A4, 1);
        delay = (uint32)(sint32)r_s16(record + 0x22);
        clock = r_u32(0x800968E4);
        w_u32(0x8008D2A8, clock + delay);
        return;
    }
    clock = r_u32(0x800968E4);
    due = r_u32(0x8008D2A8);
    if (clock < due) return;
    record = 0x800AAC58 + index * 36;
    handle = r_u32(record + 4);
    owner = handle ? r_u32(0x80099460 + (handle & 0xFFFF) * 4) : 0;
    spread = r_u32(record + 0xC);
    bank = (uint32)(sint32)(sint8)r_u8(record);
    choice = (uint32)(sint32)(sint8)r_u8(record + 1);
    pitch = r_u32(record + 0x10);
    variation = r_u32(record + 0x14);
    repeat = r_u8(record + 2);
    mode = r_u8(record + 3);
    parameter10 = r_u32(record + 0x18);
    parameter11 = r_u32(record + 0x1C);
    volume = r_u32(record + 8);
    result = level2p_sub_800420C4(bank, choice, owner, volume, spread, pitch,
        variation, repeat, mode, parameter10, parameter11);
    w_u32(0x8008D2A0, result);
    next = r_u32(0x8008D298);
    w_u16(record + 0x20, 0);
    next = (next + 1) & 7;
    record = 0x800AAC58 + next * 36;
    active = (uint32)(sint32)r_s16(record + 0x20);
    w_u32(0x8008D2A4, 0);
    w_u32(0x8008D298, next);
    if (active && r_s16(record + 0x22) < 0)
    {
        delay = r_u16(record + 0x22);
        w_u32(0x8008D2A0, 0xFFFFFFFF);
        w_u16(record + 0x22, 0 - delay);
    }
}


void level2p_sub_80042BC4(void)
{
    FUNCTION_MARKER(0x80042BC4, "LEVEL2P.BIN");
    level2p_sub_800416E4();
    level2p_sub_80042B88();
}

void level2p_sub_800433C4(uint32 owner, uint32 index, uint32 amount)
{
    uint32 data, field, value, limit, record, sample, bank, flags;
    FUNCTION_MARKER(0x800433C4, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388);
    field = data + index * 2;
    value = (uint32)(sint32)r_s16(field + 0x56);
    if ((sint32)value < 0) w_u16(field + 0x56, amount);
    else
    {
        limit = (uint32)(sint32)r_s16(field + 0x5E);
        amount += value;
        if ((sint32)amount < (sint32)limit) limit = amount;
        w_u16(field + 0x56, limit);
    }
    level2p_sub_80046BE4(owner);
    record = 0x8008F920 + (uint32)(sint32)(sint8)r_u8(0x8009AB8C + index) * 68;
    sample = (uint32)(sint32)r_s16(record + 0x20);
    if ((sint32)sample < 0) return;
    flags = (r_u16(0x8008F94A + (uint32)r_u8(data + 0x55) * 68) >> 3) & 2;
    bank = (uint32)(sint32)r_s16(record + 0x22);
    if ((sint32)bank < 0) bank = (uint32)(sint32)r_s16(r_u32(record + 0x40) + 0xC8);
    level2p_sub_800420C4(bank, sample, owner, 0x10000, 0, 0x7FFF, 0, 0, 0, 0, flags);
}

void level2p_sub_800434E8(uint32 owner)
{
    uint8 first[3] = { 0xF0, 0x48, 0 }, second[3] = { 0x40, 0x10, 0 };
    uint32 random, sample, first_view, second_view;
    FUNCTION_MARKER(0x800434E8, "LEVEL2P.BIN");
    random = sub_8001552C();
    sample = (uint32)((sint32)((random << 3) - random) >> 15);
    level2p_sub_800420C4(0, sample, owner, 0x30000, 0, 0x10000, 0x7FFF, 0, 0, 3, 0);
    /* TODO: Addressable color bytes require the permitted local adapter */
    first_view = moh_draft_local_address(first, sizeof(first));
    second_view = moh_draft_local_address(second, sizeof(second));
    level2p_sub_8006B804(owner + 0x20C, owner + 0x364, 0xC00, first_view, second_view);
}

void level2p_sub_80043594(uint32 vector, uint32 pitch_output, uint32 heading_output)
{
    uint32 angle, x, z, first, third, length;
    FUNCTION_MARKER(0x80043594, "LEVEL2P.BIN");
    angle = moh_draft_missing_call(0x8001536C, (uint32)(sint32)r_s16(vector), (uint32)(sint32)r_s16(vector + 4));
    w_u32(heading_output, angle << 16);
    x = r_u16(vector);
    z = r_u16(vector + 4);
    xport_gte_write_data(9, x);
    xport_gte_write_data(10, angle);
    xport_gte_write_data(11, z);
    xport_gte_execute(0xA00428);
    first = xport_gte_read_data(25);
    xport_gte_read_data(26);
    third = xport_gte_read_data(27);
    length = moh_draft_missing_call(0x80016188, first + third);
    angle = moh_draft_missing_call(0x8001536C, (uint32)(sint32)r_s16(vector + 2), length);
    w_u32(pitch_output, 0 - (angle << 16));
}

void level2p_sub_80046AF8(uint32 output, uint32 number, uint32 offset, uint32 width)
{
    uint32 quotient, previous;
    FUNCTION_MARKER(0x80046AF8, "LEVEL2P.BIN");
    if (width == 3)
    {
        if ((sint32)number < 100) w_u8(output + offset, 0x20);
        else
        {
            quotient = (uint32)((sint32)number / 100);
            w_u8(output + offset, quotient + 0x30);
            number -= quotient * 100;
        }
        ++offset;
    }
    if ((sint32)number >= 10)
    {
        quotient = (uint32)((sint32)number / 10);
        w_u8(output + offset, quotient + 0x30);
        number -= quotient * 10;
    }
    else
    {
        previous = r_u8(output + offset - 1);
        w_u8(output + offset, previous == 0x20 || width == 2 ? 0x20 : 0x30);
    }
    w_u8(output + offset + 1, number + 0x30);
}

void level2p_sub_80046BE4(uint32 owner)
{
    uint32 data, index, field, total, kind, current;
    FUNCTION_MARKER(0x80046BE4, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388);
    index = (uint32)(sint32)(sint8)r_u8(data + 0x54);
    field = data + index * 2;
    total = (uint32)(sint32)r_s16(field + 0x66);
    kind = (uint32)(sint32)(sint8)r_u8(0x8009AB8C + index);
    current = (uint32)(sint32)r_s16(field + 0x56);
    if (kind == 7)
    {
        w_u32(data + 0x6DC, 0);
        w_u32(data + 0x6E0, 0);
    }
    else
    {
        level2p_sub_80046AF8(data + 0x6DC, total, 0, 2);
        level2p_sub_80046AF8(data + 0x6E0, current, 0, 3);
    }
}
