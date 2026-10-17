#include "game_level2p_signatures.h"
#include "xport_trace.h"
#include "psx.h"

static uint32 moh_level2p_rotation_word(const PsxGteSnapshot *snapshot, uint32 index)
{
    uint32 element = index * 2;
    if (index < 4) return (uint32)(uint16)snapshot->rotation.m[element / 3][element % 3]
        | ((uint32)(uint16)snapshot->rotation.m[(element + 1) / 3][(element + 1) % 3] << 16);
    if (index == 4) return (uint32)(sint32)snapshot->rotation.m[2][2];
    return (uint32)snapshot->translation[index - 5];
}

void level2p_sub_80041EE0(uint32 bank, uint32 choice, uint32 channel, uint32 left, uint32 right, uint32 parameter6, uint32 parameter7)
{
    uint32 table, pitch, product, divisor, random, offset = 0, index, cursor;
    FUNCTION_MARKER(0x80041EE0, "LEVEL2P.BIN");
    table = r_u32(0x8008D3EC + (bank << 2));
    if (r_u16(table + (choice << 1) + 2) <= 12) return;
    moh_draft_missing_call(0x80015C08, channel, (uint32)(sint32)(sint16)left, (uint32)(sint32)(sint16)right);
    pitch = (uint32)(sint32)r_s16(0x8008D920 + (bank << 1) + (choice << 4));
    pitch = (uint32)((sint32)(pitch * parameter6) >> 16);
    product = pitch * parameter7;
    divisor = (uint32)((sint32)product >> 15);
    if (divisor)
    {
        random = sub_8001552C();
        if (divisor == 0xFFFFFFFF && random == 0x80000000)
        {
            moh_draft_fail_fast("Original pitch division BREAK 0x1800", 0x80041FD4);
            return;
        }
        pitch += (uint32)((sint32)product >> 16) - (uint32)((sint32)random % (sint32)divisor);
    }
    moh_draft_missing_call(0x800157BC, channel, pitch & 0xFFFF);
    if ((sint32)choice >= 0)
    {
        cursor = r_u32(0x8008D3EC + (bank << 2));
        index = 0;
        do
        {
            offset += (uint32)r_u16(cursor) << 3;
            index++;
            cursor += 2;
        } while ((sint32)index <= (sint32)choice);
    }
    moh_draft_missing_call(0x80015D38, channel, r_u32(0x8008D3CC + (bank << 2)) + offset);
    moh_draft_missing_call(0x8001559C, channel, 0, 0, 0, 3, 15, 1, 1, 3);
    w_u32(0x8008D2AC, r_u32(0x8008D2AC) | (1u << (channel & 31)));
    w_u8(0x8008D3B4 + channel, 1);
}

void level2p_sub_80042BEC(uint32 owner, uint32 position, uint32 output)
{
    uint32 data, handle, related, origin, x, y, z, index, first, second, third;
    FUNCTION_MARKER(0x80042BEC, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388);
    handle = r_u32(data + 0xC);
    related = handle ? r_u32(0x80099460 + ((handle & 0xFFFF) << 2)) : 0;
    origin = owner;
    if (related)
    {
        handle = r_u32(data + 0xC);
        origin = handle ? r_u32(0x80099460 + ((handle & 0xFFFF) << 2)) : 0;
    }
    x = (r_u16(position) - (uint32)(sint32)r_s16(origin + 0x20E)) & 0xFFFF;
    y = (r_u16(position + 2) - (uint32)(sint32)r_s16(origin + 0x212)) & 0xFFFF;
    z = (r_u16(position + 4) - (uint32)(sint32)r_s16(origin + 0x216)) & 0xFFFF;
    for (index = 0; index < 5; index++) xport_gte_write_control(index, r_u32(data + 0x100 + index * 4));
    xport_gte_write_control(5, 0);
    xport_gte_write_control(6, 0);
    xport_gte_write_control(7, 0);
    xport_gte_write_data(0, x | (y << 16));
    /* Original VZ0 load has an unspecified upper half, discarded by the GTE register */
    xport_gte_write_data(1, (uint32)(sint32)(sint16)z);
    xport_gte_execute(0x486012);
    first = xport_gte_read_data(9);
    second = xport_gte_read_data(10);
    third = xport_gte_read_data(11);
    w_u16(output, first);
    w_u16(output + 2, second);
    w_u16(output + 4, third);
}

uint32 level2p_sub_80042D90(uint32 distance, uint32 mode)
{
    uint32 value, numerator, magic, high, quotient, shift, add, volume;
    sint64 product;
    FUNCTION_MARKER(0x80042D90, "LEVEL2P.BIN");
    value = (sint32)distance < 256 ? 256 : distance;
    numerator = (value - 256) << 12;
    add = 1;
    if (mode == 1) { magic = 0x84210843; shift = 12; }
    else if (mode == 2) { magic = 0xEA0EA0EB; shift = 13; }
    else if (mode == 3) { magic = 0xD20D20D3; shift = 13; }
    else { magic = 0x4BDA12F7; shift = 11; add = 0; }
    product = (sint64)(sint32)numerator * (sint64)(sint32)magic;
    high = (uint32)((uint64)product >> 32);
    if (add) high += numerator;
    quotient = (uint32)((sint32)high >> shift) - (uint32)((sint32)numerator >> 31);
    volume = 4096 - quotient;
    if ((sint32)volume <= 0) volume = 0;
    return (uint32)((sint32)(volume * r_u8(0x8008FCDE)) >> 7);
}

void level2p_sub_80042E9C(uint32 channel, uint32 left, uint32 right, uint32 mode)
{
    PsxGteSnapshot snapshot;
    sint16 first[3], second[3];
    uint32 stack, record, index, first_view, second_view, saved[2], stream, distance[2], product;
    sint32 x, y, z, temporary, first_volume, second_volume, multiplier;
    FUNCTION_MARKER(0x80042E9C, "LEVEL2P.BIN");
    stack = r_u32(0x80099100);
    psx_gte_snapshot(&snapshot);
    for (index = 0; index < 8; index++)
        w_u32(0x80099104 + stack + index * 4, moh_level2p_rotation_word(&snapshot, index));
    record = 0x8008D46C + channel * 52;
    w_u32(0x80099100, stack + 32);
    /* TODO Addressable vector outputs need the permitted local-view adapter */
    first_view = moh_draft_local_address(first, sizeof(first));
    second_view = moh_draft_local_address(second, sizeof(second));
    level2p_sub_80042BEC(r_u32(0x80099418), record, first_view);
    level2p_sub_80042BEC(r_u32(0x8009941C), record, second_view);
    stack = r_u32(0x80099100) - 32;
    saved[1] = r_u32(0x80099108 + stack);
    w_u32(0x80099100, stack);
    saved[0] = r_u32(0x80099104 + stack);
    xport_gte_write_control(0, saved[0]);
    xport_gte_write_control(1, saved[1]);
    for (index = 2; index < 8; index += 2)
    {
        saved[1] = r_u32(0x80099104 + stack + (index + 1) * 4);
        saved[0] = r_u32(0x80099104 + stack + index * 4);
        xport_gte_write_control(index, saved[0]);
        xport_gte_write_control(index + 1, saved[1]);
    }
    for (stream = 0; stream < 2; stream++)
    {
        x = stream ? second[0] : first[0];
        y = stream ? second[1] : first[1];
        z = stream ? second[2] : first[2];
        if (x < 0) x = -x;
        if (y < 0) y = -y;
        if (z < 0) z = -z;
        if (x < y) { temporary = x; x = y; y = temporary; }
        if (x < z) { temporary = x; x = z; z = temporary; }
        distance[stream] = moh_draft_missing_call(0x8001573C, (uint32)(x + ((y + z) >> 2)));
    }
    first_volume = (sint32)level2p_sub_80042D90(distance[0], mode) >> 1;
    second_volume = (sint32)level2p_sub_80042D90(distance[1], mode) >> 1;
    if (!r_u8(0x8008FCE3))
    {
        if (second_volume < first_volume) second_volume = first_volume;
        else first_volume = second_volume;
    }
    multiplier = r_s16(0x8008D456 + channel * 52);
    product = (uint32)second_volume * (uint32)multiplier;
    if ((sint32)product < 0) product += 0xFFF;
    second_volume = (sint32)product >> 12;
    product = (uint32)first_volume * (uint32)multiplier;
    if ((sint32)product < 0) product += 0xFFF;
    first_volume = (sint32)product >> 12;
    w_u16(left, (uint32)first_volume << 6);
    w_u16(right, (uint32)second_volume << 6);
}

void level2p_sub_80053EF0(uint32 entry, uint32 group)
{
    uint32 descriptor, frame, offset, index = 0, source, indices, bank, destination, value;
    FUNCTION_MARKER(0x80053EF0, "LEVEL2P.BIN");
    descriptor = r_u32(entry);
    frame = r_u32(entry + 0xC);
    offset = frame * r_u16(descriptor + 0x12) * 10;
    while ((sint32)index < (sint32)r_u16(descriptor + 0x12))
    {
        indices = r_u32(entry + 4);
        source = r_u32(entry + 8) + offset;
        bank = r_u32(0x80094C7C + (group << 8));
        value = r_u16(indices + (index << 1));
        destination = bank + r_u32(bank + 0x10) + (value << 4);
        w_u16(destination + 6, r_u16(source));
        w_u16(destination + 0xA, r_u16(source + 4));
        w_u16(destination + 0xE, r_u16(source + 8));
        w_u16(destination + 0xC, r_u16(source + 6));
        index++;
        value = r_u16(source + 2);
        offset += 10;
        w_u16(destination + 8, value);
    }
}

void level2p_sub_80069AF0(uint32 data, uint32 parameter2, uint32 parameter3, uint32 parameter4, uint32 parameter5, uint32 parameter6, uint32 parameter7, uint32 parameter8, uint32 parameter9, uint32 parameter10, uint32 parameter11, uint32 parameter12, uint32 parameter13, uint32 parameter14, uint32 parameter15, uint32 parameter16, uint32 parameter17, uint32 parameter18, uint32 parameter19, uint32 parameter20, uint32 parameter21, uint32 parameter22, uint32 parameter23, uint32 parameter24, uint32 parameter25, uint32 parameter26, uint32 parameter27, uint32 parameter28, uint32 parameter29, uint32 parameter30)
{
    uint32 index, cursor, high;
    FUNCTION_MARKER(0x80069AF0, "LEVEL2P.BIN");
    if (!(parameter2 & 7)) parameter2 = 2;
    w_u32(data, parameter2);
    w_u16(data + 0x49C, parameter4);
    w_u16(data + 0x49E, parameter5);
    w_u16(data + 0x4A0, parameter6);
    w_u16(data + 0x4A4, parameter8);
    w_u16(data + 0x4A6, parameter9);
    w_u16(data + 0x4A8, parameter10);
    w_u16(data + 0x4AA, parameter8);
    w_u16(data + 0x4AC, parameter9);
    w_u16(data + 0x4AE, parameter10);
    w_u16(data + 0x4B0, parameter11);
    w_u16(data + 0x4B2, parameter12);
    w_u16(data + 0x4B4, parameter13);
    w_u16(data + 0x4B6, parameter11);
    w_u16(data + 0x4B8, parameter12);
    w_u16(data + 0x4BA, parameter13);
    w_u16(data + 0x4BC, parameter14);
    w_u16(data + 0x4BE, parameter15);
    w_u16(data + 0x4C0, parameter16);
    w_u16(data + 0x4C2, parameter14);
    w_u16(data + 0x4C4, parameter15);
    w_u16(data + 0x4C6, parameter16);
    w_u16(data + 0x4A2, parameter7);
    w_u32(data + 0x4C8, parameter17);
    w_u16(data + 0x4D0, parameter19);
    w_u16(data + 0x4D2, parameter20);
    w_u32(data + 0x4E0, parameter21);
    w_u32(data + 0x4CC, parameter18);
    w_u32(data + 0x4E4, parameter22);
    w_u32(data + 0x4E8, parameter23);
    w_u32(data + 0x4EC, parameter21);
    w_u32(data + 0x4F0, parameter22);
    w_u32(data + 0x4F4, parameter23);
    if (parameter21 || parameter23) w_u32(data, r_u32(data) | 0x10);
    high = (uint32)((sint32)parameter25 >> 16);
    w_u32(data + 0x4FC, parameter25);
    w_u32(data + 0x4F8, parameter24);
    w_u32(data + 0x4D4, parameter26);
    w_u16(data + 0x4DC, parameter28);
    w_u16(data + 0x4DE, parameter29);
    w_u32(data + 0x4D8, parameter27);
    w_u16(data + 0x500, high * high);
    cursor = data + 0x464;
    for (index = 0; index < 20; index++, cursor -= 0x38) w_u32(cursor, 0);
    w_u16(data + 0x504, parameter3);
    w_u16(data + 0x502, 0);
    w_u32(data + 4, 0);
    w_u32(data + 8, 0);
    w_u32(data + 0xC, 0);
    w_u32(data + 0x10, 0);
    w_u32(data + 0x14, 0);
    w_u32(data + 0x18, 0);
    w_u8(data + 0x506, parameter30);
}


uint32 level2p_sub_80066934(uint32 kind, uint32 size, uint32 id, uint32 parameter4, uint32 parameter5, uint32 parameter6, uint32 parameter7, uint32 parameter8)
{
    uint32 index, allocation_size, owner, cursor, field, type;
    FUNCTION_MARKER(0x80066934, "LEVEL2P.BIN");
    if (id != 0xFFFFFFFF && !parameter8)
    {
        index = level2p_sub_80065BF8(id);
        if ((sint32)index <= 0) return 0;
        parameter8 = level2p_sub_80065BB8(index);
    }
    allocation_size = size + ((kind & 5) ? 0x480 : 0x3D0);
    if ((kind & 2) && id != 0xFFFFFFFF) parameter5 = r_u16(parameter8 + 0xBE);
    allocation_size += parameter5 * 72 + parameter7 * 28 + parameter6 * 120;
    /* TODO Original reads this field even when the resource pointer is zero */
    type = r_u32(parameter8 + 0xFC);
    if (type == 0x1402 || type == 0x1405) return 0;
    owner = sub_80036DA0(allocation_size, 1);
    if (!owner) return 0;
    level2p_sub_80068AE8(owner, owner);
    cursor = owner + 0x3D0;
    w_u32(owner + 0x390, 0xFFFF);
    w_u32(owner + 0x394, 0);
    w_u32(owner + 0x398, 0xFFFF);
    w_u32(owner + 0x39C, 0);
    if (size)
    {
        w_u32(owner + 0x388, cursor);
        cursor += size;
    }
    else w_u32(owner + 0x388, 0);
    if (kind & 5)
    {
        w_u32(owner + 0x100, cursor);
        cursor += 0xB0;
    }
    else w_u32(owner + 0x100, 0);
    if (parameter5)
    {
        w_u32(owner + 0x304, cursor);
        cursor += parameter5 * 72;
    }
    else w_u32(owner + 0x304, 0);
    if (parameter7)
    {
        w_u32(owner + 0x300, cursor);
        cursor += parameter7 * 28;
        w_u16(owner + 0x2EE, parameter7);
    }
    else
    {
        w_u32(owner + 0x300, 0);
        w_u16(owner + 0x2EE, 0);
    }
    field = owner;
    for (index = 0; index < 2; index++, field += 0x74)
    {
        if (parameter6)
        {
            w_u32(field + 0x124, cursor);
            cursor += parameter6 * 60;
            w_u32(owner + 0x1EC, parameter6);
        }
        else
        {
            w_u32(field + 0x124, 0);
            w_u32(owner + 0x1EC, 0);
        }
    }
    level2p_sub_800666C4(owner);
    if (id == 0xFFFFFFFF || ((kind & 5) && level2p_sub_80065C9C(r_u32(owner + 0x100), parameter8, parameter4, owner)))
    {
        w_u32(owner + 0x38C, 0);
        return owner;
    }
    sub_80036E44(owner);
    return 0;
}

uint32 level2p_sub_800688DC(uint32 data, uint32 instance)
{
    uint32 owner, model;
    sint32 animation;
    FUNCTION_MARKER(0x800688DC, "LEVEL2P.BIN");
    owner = level2p_sub_8005CA84(instance, data);
    if (!owner) return 0;
    model = r_u32(owner + 0x100);
    if (model && r_u32(model + 8))
    {
        level2p_sub_8006591C(owner);
        model = r_u32(r_u32(owner + 0x100) + 8);
        animation = r_s16(model + 0x94);
        if (animation >= 0)
        {
            level2p_sub_8003F764(owner, (uint32)animation, 1, 0);
            level2p_sub_8003FC00(owner);
        }
    }
    return owner;
}

uint32 level2p_sub_800828C0(uint32 instance, uint32 data, uint32 id)
{
    uint32 current, object, selector;
    FUNCTION_MARKER(0x800828C0, "LEVEL2P.BIN");
    current = level2p_sub_80087870();
    object = r_u32(instance + 4);
    selector = (uint32)(sint32)r_s16(object + 0x34);
    if (selector == 100 || current + 1 == selector) return level2p_sub_80082518(object, data, id);
    return 0;
}


uint32 level2p_sub_80068A40(uint32 unused, uint32 index, uint32 generation)
{
    FUNCTION_MARKER(0x80068A40, "LEVEL2P.BIN");
    return (generation << 16) | (index & 0xFFFF);
}

void level2p_sub_80068AE8(uint32 owner, uint32 value)
{
    uint32 index, generation, handle, current;
    FUNCTION_MARKER(0x80068AE8, "LEVEL2P.BIN");
    index = r_u32(0x8009945C);
    if (index < 256)
    {
        w_u32(0x80099460 + (index << 2), value);
        generation = r_u32(0x80099458) + 1;
        w_u32(0x80099458, generation);
        w_u32(0x80099860 + (index << 2), generation);
        handle = level2p_sub_80068A40(owner, index, generation);
        current = r_u32(0x8009945C);
        w_u32(value, handle);
        w_u32(0x80099C60 + (current << 2), 0);
        w_u32(0x8009945C, current + 1);
    }
    else
    {
        for (index = 0; index < 256; index++)
        {
            if (!r_u32(0x80099460 + (index << 2)))
            {
                w_u32(0x80099460 + (index << 2), value);
                generation = r_u32(0x80099458) + 1;
                w_u32(0x80099458, generation);
                w_u32(0x80099860 + (index << 2), generation);
                handle = level2p_sub_80068A40(owner, index, generation);
                w_u32(value, handle);
                w_u32(0x80099C60 + (index << 2), 0);
                break;
            }
        }
    }
    /* TODO Original still copies the value word when all handle slots are occupied */
    w_u32(owner, r_u32(value));
}

void level2p_sub_8005A940(uint32 owner)
{
    uint32 index, cursor = owner + 0xE8;
    FUNCTION_MARKER(0x8005A940, "LEVEL2P.BIN");
    for (index = 0; index < 40; index++, cursor -= 4) w_u32(cursor, 0);
    w_u32(owner + 0xEC, 0);
}

void level2p_sub_800666C4(uint32 owner)
{
    uint32 model, index, field;
    FUNCTION_MARKER(0x800666C4, "LEVEL2P.BIN");
    w_u32(owner + 8, 0);
    w_u32(owner + 0xC, 0);
    w_u32(owner + 0x10, 0);
    w_u32(owner + 0x18, 0);
    w_u32(owner + 0x28, 0);
    w_u32(owner + 0x3BC, 0xFFFFFFFF);
    level2p_sub_80068A04(owner + 0x2C, 0);
    level2p_sub_80068A04(owner + 0x30, 0);
    model = r_u32(owner + 0x100);
    w_u32(owner + 0xF4, 0x640000);
    w_u32(owner + 0x294, 1);
    w_u32(owner + 0x290, 1);
    w_u32(owner + 0xF8, 0);
    w_u32(owner + 0x298, 0);
    w_u32(owner + 0x36C, 0);
    w_u32(owner + 0x370, 0);
    w_u32(owner + 0x374, 0);
    w_u32(owner + 0x378, 0);
    w_u32(owner + 0x37C, 0);
    w_u32(owner + 0x380, 0);
    w_u32(owner + 0x384, 0);
    w_u32(owner + 0x20C, 0);
    w_u32(owner + 0x210, 0);
    w_u32(owner + 0x214, 0);
    w_u32(owner + 0x228, 0);
    w_u32(owner + 0x22C, 0);
    w_u32(owner + 0x230, 0);
    w_u32(owner + 0x270, 0x10000);
    w_u32(owner + 0x274, 0x10000);
    w_u32(owner + 0x278, 0x10000);
    w_u32(owner + 0x218, 0);
    w_u32(owner + 0x21C, 0);
    w_u32(owner + 0x220, 0);
    w_u32(owner + 0x200, 0);
    w_u32(owner + 0x204, 0);
    w_u32(owner + 0x1FC, 0);
    if (model)
    {
        w_u16(r_u32(owner + 0x100) + 0x5C, 0xFFFF);
        w_u16(r_u32(owner + 0x100) + 0x5E, 0);
        w_u16(r_u32(owner + 0x100) + 0x62, 0xFFFF);
        w_u16(r_u32(owner + 0x100) + 0x64, 0);
        w_u16(r_u32(owner + 0x100) + 0x3E, 0);
        w_u32(r_u32(owner + 0x100) + 8, 0);
    }
    field = owner;
    for (index = 0; index < 2; index++, field += 0x74)
    {
        w_u32(field + 0x114, 0);
        w_u16(field + 0x11E, 0xFFFF);
        w_u32(field + 0x104, 0);
        w_u32(field + 0x108, 0);
        w_u32(field + 0x10C, 0);
        w_u32(field + 0x110, 0);
        w_u16(field + 0x122, 0);
        w_u32(field + 0x114, 0);
        w_u16(field + 0x120, 0);
        w_u16(field + 0x11C, 0xFFFF);
    }
    w_u32(owner + 0x288, 1);
    w_u32(owner + 0x28C, 1);
    w_u32(owner + 0x284, 1);
    w_u32(owner + 0x24C, 0);
    w_u32(owner + 0x250, 0);
    w_u32(owner + 0x254, 0);
    w_u32(owner + 0x1F4, 0);
    w_u16(owner + 0x36, 0);
    w_u16(owner + 0x2DC, 0);
    w_u16(owner + 0x2DA, 0);
    w_u16(owner + 0x2D8, 0);
    w_u16(owner + 0x2E4, 0);
    w_u16(owner + 0x2E2, 0);
    w_u16(owner + 0x2E0, 0);
    w_u16(owner + 0x2E8, 100);
    w_u16(owner + 0x2EC, 0);
    w_u32(owner + 0x2D4, 0);
    level2p_sub_8005A940(owner);
    w_u8(owner + 0x2A0, 64);
    w_u8(owner + 0x2A1, 64);
    w_u8(owner + 0x2A2, 64);
    w_u16(owner + 0x2A4, 128);
    w_u16(owner + 0x2A6, 128);
    w_u16(owner + 0x2A8, 128);
    w_u16(owner + 0x2AC, 4096);
    w_u16(owner + 0x2AE, 4096);
    w_u16(owner + 0x2B0, 4096);
    w_u32(owner + 0x224, 0x10000);
    w_u16(owner + 0x2B4, 0);
    w_u16(owner + 0x2B6, 0);
    w_u16(owner + 0x2B8, 0);
    w_u16(owner + 0x2BC, 0);
    w_u16(owner + 0x2BE, 0);
    w_u16(owner + 0x2C0, 0);
    w_u16(owner + 0x2C4, 0);
    w_u16(owner + 0x2C6, 0);
    w_u16(owner + 0x2C8, 0);
    w_u16(owner + 0x2CC, 0);
    w_u16(owner + 0x2CE, 0);
    w_u16(owner + 0x2D0, 0);
    w_u32(owner + 0x44, 0);
    w_u32(owner + 0x48, 0);
    w_u32(owner + 0x3C, 0);
    w_u32(owner + 0x40, 0);
    w_u32(owner + 0x38, 0);
    w_u32(owner + 0x3A0, 0);
    w_u16(owner + 0x2EA, 0xFFFF);
    w_u16(owner + 0x3CC, 100);
    w_u16(owner + 0x3CE, 0xFFFF);
}

uint32 level2p_sub_8005CA84(uint32 instance, uint32 data)
{
    FUNCTION_MARKER(0x8005CA84, "LEVEL2P.BIN");
    if (r_u32(data + 0xFC) == 0x1776) return level2p_sub_8005AAA4(instance, data);
    return 0;
}

uint32 level2p_sub_8005AAA4(uint32 instance, uint32 data)
{
    uint32 object, owner, kind, health, counter, intensity, model;
    FUNCTION_MARKER(0x8005AAA4, "LEVEL2P.BIN");
    object = r_u32(instance + 4);
    owner = level2p_sub_80066934(3, 0, r_u32(data), 0x157, 0, 10, 6, data);
    if (!owner) return 0;
    kind = r_u32(data + 0xFC);
    w_u32(owner + 0x14, 0x88214051);
    w_u32(owner + 0xC, 0x3008400);
    w_u32(owner + 0x10, 0x44030);
    w_u32(owner + 8, kind);
    health = (uint32)(sint32)r_s16(object + 0x5E);
    counter = r_u8(0x80096191);
    w_u32(owner + 0x294, health);
    w_u32(owner + 0x290, health);
    intensity = r_u8(object + 0x5C);
    w_u8(0x80096191, counter + 1);
    w_u32(owner + 0x36C, 0x8005AA78);
    w_u32(owner + 0x370, 0x80065698);
    w_u32(owner + 0x384, 0x8006543C);
    model = r_u32(owner + 0x100);
    w_u16(owner + 0x2EC, 0);
    w_u8(owner + 0x35, counter);
    w_u32(owner + 0x374, 0);
    w_u32(owner + 0x378, 0);
    w_u32(owner + 0xF0, intensity << 16);
    w_u32(owner + 0xF4, intensity << 16);
    model = r_u32(model + 8);
    level2p_sub_8004B2AC(owner, model + 0x44, model + 0x7C, 0xFFFFFFFF);
    w_u16(owner + 0x36, 0);
    level2p_sub_8006591C(owner);
    level2p_sub_8004DFBC(owner);
    return owner;
}

void level2p_sub_8005AA78(uint32 owner)
{
    FUNCTION_MARKER(0x8005AA78, "LEVEL2P.BIN");
    level2p_sub_8003FF0C(owner);
    level2p_sub_8003FC00(owner);
}

void level2p_sub_8006543C(uint32 owner)
{
    uint32 model;
    FUNCTION_MARKER(0x8006543C, "LEVEL2P.BIN");
    model = r_u32(owner + 0x100);
    if (model) level2p_sub_800665BC(model, owner, 2);
    level2p_sub_800653D8(owner);
}

void level2p_sub_800653D8(uint32 owner)
{
    uint32 data;
    FUNCTION_MARKER(0x800653D8, "LEVEL2P.BIN");
    level2p_sub_80061740(owner, 0);
    data = r_u32(owner + 0x38C);
    if (data) sub_80036E44(data);
    level2p_sub_80088FA8(owner, 2);
}

static sint32 moh_level2p_bound_component(uint32 address, const uint32 local[2], uint32 copied, uint32 axis)
{
    if (copied) return (sint16)(local[axis >> 1] >> ((axis & 1) << 4));
    return r_s16(address + axis * 2);
}

void level2p_sub_8004B2AC(uint32 owner, uint32 lower, uint32 upper, uint32 padding)
{
    uint32 template[42], low[2], high[2], copied = padding != 0xFFFFFFFF;
    uint32 index, face, destination, words[4], axis, side, value, first, second;
    sint16 width[3];
    sint32 difference, maximum, half;
    FUNCTION_MARKER(0x8004B2AC, "LEVEL2P.BIN");
    for (index = 0; index < 42; index++) template[index] = r_u32(0x8003A238 + index * 4);
    w_u16(owner + 0x2EE, 6);
    for (axis = 0; axis < 3; axis++)
    {
        difference = r_s16(upper + axis * 2) - r_s16(lower + axis * 2);
        if (difference < 0) difference = -difference;
        width[axis] = (sint16)difference;
    }
    if (copied)
    {
        low[0] = r_u32(lower);
        low[1] = r_u32(lower + 4);
        high[0] = r_u32(upper);
        high[1] = r_u32(upper + 4);
        half = (sint32)padding >> 1;
        if (width[0] < width[2])
        {
            width[0] = (sint16)((uint32)(sint32)width[0] + padding);
            low[0] = (low[0] & 0xFFFF0000) | ((low[0] - (uint32)half) & 0xFFFF);
            high[0] = (high[0] & 0xFFFF0000) | ((high[0] + (uint32)half) & 0xFFFF);
        }
        else if (width[0] != width[2])
        {
            width[2] = (sint16)((uint32)(sint32)width[2] + padding);
            low[1] = (low[1] & 0xFFFF0000) | ((low[1] - (uint32)half) & 0xFFFF);
            high[1] = (high[1] & 0xFFFF0000) | ((high[1] + (uint32)half) & 0xFFFF);
        }
    }
    maximum = width[1];
    if (maximum <= width[0]) maximum = width[0];
    if (maximum <= width[2]) maximum = width[2];
    w_u16(owner + 0x2E8, (uint32)((maximum + 30) >> 1));
    for (axis = 0; axis < 3; axis++)
        w_u16(owner + 0x2D8 + axis * 2, (uint32)(moh_level2p_bound_component(lower, low, copied, axis) + (width[axis] >> 1)));
    for (face = 0; face < 6; face++)
    {
        destination = r_u32(owner + 0x300) + face * 28;
        for (index = 0; index < 4; index++) words[index] = template[face * 7 + index];
        for (index = 0; index < 4; index++) w_u32(destination + index * 4, words[index]);
        for (index = 0; index < 3; index++) words[index] = template[face * 7 + 4 + index];
        for (index = 0; index < 3; index++) w_u32(destination + 16 + index * 4, words[index]);
    }
    for (face = 0; face < 6; face++)
    {
        axis = face < 2 ? 1 : (face < 4 ? 0 : 2);
        side = face & 1;
        difference = side ? moh_level2p_bound_component(upper, high, copied, axis)
            : moh_level2p_bound_component(lower, low, copied, axis);
        destination = r_u32(owner + 0x300);
        if (difference < 0) difference = -difference;
        w_u16(destination + face * 28 + 10, (uint32)difference - 1);
    }
    first = copied ? low[0] : r_u32(lower);
    second = copied ? low[1] : r_u32(lower + 4);
    w_u32(owner + 0x2F0, first);
    w_u32(owner + 0x2F4, second);
    first = copied ? high[0] : r_u32(upper);
    second = copied ? high[1] : r_u32(upper + 4);
    w_u32(owner + 0x2F8, first);
    w_u32(owner + 0x2FC, second);
}


void level2p_sub_800665BC(uint32 model, uint32 owner, uint32 mode)
{
    uint32 index, offset, flags, data, kind, allocation;
    FUNCTION_MARKER(0x800665BC, "LEVEL2P.BIN");
    if (!model) return;
    if (owner && r_u16(owner + 0x36))
    {
        index = 0;
        offset = 0;
        do
        {
            data = r_u32(owner + 0x304);
            allocation = r_u32(data + offset + 0x3C);
            index++;
            level2p_sub_800665BC(allocation, 0, 2);
            offset += 72;
        } while ((sint32)index < (sint32)r_u16(owner + 0x36));
    }
    flags = r_u32(model + 4);
    if (flags & 8)
    {
        data = r_u32(model + 8);
        w_u32(model + 4, flags & 0xFFFFFFF7);
        w_u32(data + 0x1C, 0);
    }
    /* TODO Recursive zero-owner calls still read the original owner+8 address */
    kind = r_u32(owner + 8);
    if (kind == 0x1402 || kind == 0x1405) return;
    allocation = r_u32(model);
    if (allocation) level2p_sub_80088FA8(r_u32(model), mode);
    w_u32(model, 0);
}

void level2p_sub_80088FA8(uint32 allocation, uint32 kind)
{
    uint32 count, entry;
    FUNCTION_MARKER(0x80088FA8, "LEVEL2P.BIN");
    if (!kind)
    {
        sub_80036E44(allocation);
        return;
    }
    count = r_u32(0x8009EBC8);
    entry = 0x8009EBCC + (count << 3);
    w_u32(entry, allocation);
    w_u32(entry + 4, kind);
    w_u32(0x8009EBC8, count + 1);
}

uint32 level2p_sub_8004E288(uint32 player, uint32 mask, uint32 edge, uint32 inverted)
{
    uint32 held, previous;
    FUNCTION_MARKER(0x8004E288, "LEVEL2P.BIN");
    held = r_u32(0x8008FE5C + (player << 2)) & mask;
    if (inverted)
    {
        if (!edge) return held == 0;
        if (held) return 0;
        previous = r_u32(0x8008FE64 + (player << 2)) & mask;
        return previous != 0;
    }
    if (!edge) return held == mask;
    if (held != mask) return 0;
    previous = r_u32(0x8008FE64 + (player << 2)) & mask;
    return (previous ^ mask) != 0;
}

uint32 level2p_sub_8005A9C8(uint32 cursor, uint32 owner, uint32 state, uint32 model_state, uint32 parameter5, uint32 stream)
{
    uint32 descriptor, code, target, result;
    FUNCTION_MARKER(0x8005A9C8, "LEVEL2P.BIN");
    w_u32(0x8009617C, model_state);
    w_u32(0x8009618C, state);
    w_u32(0x80096188, 0);
    w_u32(0x80096178, parameter5);
    descriptor = r_u32(state + 0x10);
    w_u32(0x80096180, stream);
    w_u32(0x80096184, descriptor);
    descriptor = r_u32(cursor);
    code = r_u32(descriptor);
    target = r_u32(0x80095E00 + (code >> 24) * 12);
    result = moh_draft_indirect_call(target, owner, descriptor, (code >> 16) & 255);
    w_u32(cursor, result);
    return r_u32(0x80096188);
}

uint32 level2p_sub_8003FF0C(uint32 owner)
{
    uint32 matched = 0, player = 0, value, flags, stream, state, model_state, index, offset;
    uint32 entry, conditions, edge, mask, held, cursor, opcode, target, cursor_view;
    FUNCTION_MARKER(0x8003FF0C, "LEVEL2P.BIN");
    if (r_u32(owner + 0xC) & 1) player = r_u8(r_u32(owner + 0x388) + 0x726);
    if (r_s32(owner + 0x44) > 0)
    {
        flags = r_u32(owner + 0x38);
        value = r_u32(owner + 0x44);
        w_u32(owner + 0x38, flags & 0xFFFFF9FF);
        w_u32(owner + 0x44, value - 1);
    }
    if (r_s32(owner + 0x48) > 0)
    {
        flags = r_u32(owner + 0x38);
        value = r_u32(owner + 0x48);
        w_u32(owner + 0x38, flags & 0xFFE7FE1F);
        w_u32(owner + 0x48, value - 1);
    }
    for (stream = 0; stream < 2; stream++)
    {
        state = owner + 0x104 + stream * 0x74;
        if (!r_u32(state + 0x10)) continue;
        model_state = r_u32(owner + 0x100) + 0x5C;
        offset = 0;
        for (index = 0; index < r_u32(owner + 0x1EC); index++, offset += 60)
        {
            entry = r_u32(state + 0x20) + offset;
            conditions = r_u32(entry);
            if (!conditions)
            {
                if (matched) break;
                continue;
            }
            edge = (r_u32(entry + 4) & 0xC0) != 0;
            if ((conditions & 1) && !level2p_sub_8004E288(player, r_u32(entry + 8), edge, 0)) continue;
            if ((conditions & 2) && edge)
            {
                mask = r_u32(r_u32(state + 0x20) + offset + 0xC);
                held = r_u32(0x8008FE5C + (player << 2)) & mask;
                if (held) continue;
                if (!(r_u32(0x8008FE64 + (player << 2)) & mask)) continue;
            }
            if (conditions & 4)
            {
                mask = r_u32(r_u32(state + 0x20) + offset + 0x10);
                if ((mask & r_u32(owner + 0x38)) != mask) continue;
            }
            if ((conditions & 8) && (r_u32(r_u32(state + 0x20) + offset + 0x14) & r_u32(owner + 0x38))) continue;
            if (conditions & 0x10)
            {
                mask = r_u32(r_u32(state + 0x20) + offset + 0x18);
                if ((mask & r_u32(owner + 0x3C)) != mask) continue;
            }
            if ((conditions & 0x20) && (r_u32(r_u32(state + 0x20) + offset + 0x1C) & r_u32(owner + 0x3C))) continue;
            if (conditions & 0x40)
            {
                mask = r_u32(r_u32(state + 0x20) + offset + 0x20);
                if ((mask & r_u32(owner + 0x40)) != mask) continue;
            }
            if ((conditions & 0x80) && (r_u32(r_u32(state + 0x20) + offset + 0x24) & r_u32(owner + 0x40))) continue;
            entry = r_u32(state + 0x20) + offset;
            cursor = entry + 0x28;
            opcode = r_u8(entry + 0x2B);
            if (opcode == 0x26)
            {
                target = r_u32(r_u32(state + 0x10) + 0xC);
                value = r_u16(entry + 0x28);
                w_u16(state + 0x18, 0xFFFF);
                w_u16(state + 0x1C, 0);
                w_u16(state + 0x1E, 0);
                w_u32(state + 0x14, target + (value << 2));
            }
            else if (opcode != 0x38)
            {
                /* TODO Addressable command cursor needs the permitted local-view adapter */
                cursor_view = moh_draft_local_address(&cursor, sizeof(cursor));
                level2p_sub_8005A9C8(cursor_view, owner, state, model_state, 0, stream);
            }
            matched = 1;
            break;
        }
    }
    return matched;
}


uint32 level2p_sub_8003F764(uint32 owner, uint32 animation, uint32 enabled, uint32 mode)
{
    uint32 model, asset, descriptor, state, mask, transition = 0, first, second;
    uint32 destination, source, bytes, count, index, offset;
    FUNCTION_MARKER(0x8003F764, "LEVEL2P.BIN");
    model = r_u32(owner + 0x100);
    asset = r_u32(model + 8);
    descriptor = r_u32(asset + 0xB8) + ((uint32)(sint32)(sint16)animation << 4);
    if ((sint16)animation < 0) return 0;
    state = owner + mode * 0x74 + 0x104;
    if (!enabled)
    {
        mask = r_u32(descriptor);
        if (mask & r_u32(state + 4)) transition = 1;
        else if (!(mask & r_u32(state)))
        {
            if (mask & r_u32(state + 8)) return 0;
            if (mask & r_u32(state + 0xC)) return 0;
            if (r_u32(state + 0x10)) return 0;
        }
    }
    w_u32(state + 0x10, descriptor);
    w_u16(state + 0x1A, animation);
    w_u32(state + 0x14, r_u32(descriptor + 0xC));
    model = r_u32(owner + 0x100);
    if (transition)
    {
        if (r_u32(model + 0x74))
        {
            first = r_u32(model + 0x98);
            second = r_u32(model + 0x9C);
            w_u32(model + 0xA0, first);
            w_u32(model + 0xA4, second);
            model = r_u32(owner + 0x100);
            source = r_u32(model + 0x88);
            if (source)
            {
                bytes = (uint32)r_u16(model + 0x72) << 3;
                destination = r_u32(model + 0x8C);
                moh_draft_missing_call(0x800154EC, destination, source, bytes);
            }
        }
        else
        {
            destination = r_u32(model + 0x30);
            bytes = (uint32)r_u16(model + 0x3C) << 3;
            source = r_u32(model + 0x2C);
            moh_draft_missing_call(0x800154EC, destination, source, bytes);
        }
        w_u16(r_u32(owner + 0x100) + 0x3E, 4);
        w_u16(r_u32(owner + 0x100) + 0x40, 0xFFFF);
    }
    else w_u16(model + 0x3E, 0);
    w_u32(owner + 0x1FC, 0);
    w_u32(state, 0);
    w_u32(state + 4, 0xFFFFFFFF);
    w_u32(state + 8, 0);
    w_u32(state + 0xC, 0);
    w_u32(owner + 0x28C, r_u32(owner + 0x288));
    count = 0;
    offset = 0;
    if (!mode)
    {
        level2p_sub_8004D148(owner, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
        asset = r_u32(r_u32(owner + 0x100) + 8);
        count = r_u32(asset + 0xB0);
        if (count)
        {
            index = r_u32(owner + 0x1EC);
            destination = r_u32(state + 0x20);
            source = r_u32(asset + 0xB4);
            if (index < count)
            {
                moh_draft_missing_call(0x800154EC, destination, source, index * 60);
                goto animation_reset_counters;
            }
            offset = count * 60;
            moh_draft_missing_call(0x800154EC, destination, source, offset);
        }
    }
    if (count < r_u32(owner + 0x1EC))
    {
        do
        {
            destination = r_u32(state + 0x20) + offset;
            count++;
            w_u32(destination, 0);
            offset += 60;
        } while (count < r_u32(owner + 0x1EC));
    }
animation_reset_counters:
    w_u16(state + 0x18, 0xFFFF);
    w_u16(state + 0x1C, 0);
    w_u16(state + 0x1E, 0);
    return 1;
}

void level2p_sub_8003FC00(uint32 owner)
{
    uint32 model, stream, state, cursor, model_state, done, base, length, depth, current_model;
    uint32 random, product, high, percentage, asset, choices, entry, index, selected, cursor_view;
    sint64 wide;
    FUNCTION_MARKER(0x8003FC00, "LEVEL2P.BIN");
    model = r_u32(owner + 0x100);
    for (stream = 0; stream < 2; stream++)
    {
        state = owner + 0x104 + stream * 0x74;
        if (!r_u32(state + 0x10))
        {
            if (!stream) level2p_sub_8003F764(owner, 0, 0, 0);
            continue;
        }
        cursor = r_u32(state + 0x14);
        model_state = model + 0x5C;
        if (r_s16(state + 0x1C) > 0)
        {
            w_u16(state + 0x1C, r_u16(state + 0x1C) - 1);
            continue;
        }
        if (r_s16(model_state + 2) < r_s16(state + 0x18))
        {
            w_u16(model_state + 2, r_u16(model_state + 2) + 1);
            current_model = r_u32(owner + 0x100);
            if (r_s16(current_model + 0x3E) > 0)
                w_u16(current_model + 0x3E, r_u16(current_model + 0x3E) - 1);
            w_u16(state + 0x1C, 0);
            level2p_sub_8003F508(owner);
            continue;
        }
        w_u16(state + 0x18, 0xFFFF);
        done = 0;
        while (!done)
        {
            depth = (uint32)(sint32)r_s16(state + 0x1E);
            if (!depth) base = r_u32(r_u32(state + 0x10) + 0xC);
            else base = r_u32(state + ((depth - 1) << 4) + 0x2C);
            depth = (uint32)(sint32)r_s16(state + 0x1E);
            if (!depth) length = r_u16(r_u32(state + 0x10) + 0xA);
            else length = r_u16(state + ((depth - 1) << 4) + 0x30);
            if (((sint32)(cursor - base) >> 2) >= (sint32)length)
            {
                w_u32(state + 0x10, 0);
                w_u16(state + 0x1A, 0xFFFF);
                w_u16(r_u32(owner + 0x100) + 0x5C, 0xFFFF);
                w_u16(r_u32(owner + 0x100) + 0x5E, 0);
                if (stream) break;
                random = sub_8001552C();
                product = random * 100;
                wide = (sint64)(sint32)product * (sint64)(sint32)0x80010003;
                high = (uint32)((uint64)wide >> 32);
                percentage = (uint32)((sint32)(high + product) >> 14) - (uint32)((sint32)product >> 31);
                asset = r_u32(model + 8);
                choices = r_u16(asset + 0xBC);
                selected = 0;
                if (choices)
                {
                    entry = r_u32(asset + 0xB8);
                    for (index = 0; (sint32)index < (sint32)choices; index++, entry += 16)
                    {
                        if ((sint32)percentage < (sint32)r_u16(entry + 4))
                        {
                            level2p_sub_8003F764(owner, (uint32)(sint32)(sint16)index, 0, 0);
                            selected = 1;
                            break;
                        }
                    }
                }
                if (!selected) level2p_sub_8003F764(owner, 0, 0, 0);
                state = owner + 0x104 + stream * 0x74;
                cursor = r_u32(state + 0x14);
                model_state = model + 0x5C;
            }
            depth = r_u16(state + 0x1A);
            /* TODO Addressable script cursor needs the permitted local-view adapter */
            cursor_view = moh_draft_local_address(&cursor, sizeof(cursor));
            done = level2p_sub_8005A9C8(cursor_view, owner, state, model_state, 0, stream);
            if (r_s16(state + 0x1A) != (sint16)depth)
            {
                state = owner + 0x104 + stream * 0x74;
                model_state = model + 0x5C;
            }
            w_u32(state + 0x14, cursor);
        }
    }
}

void level2p_sub_8003F508(uint32 owner)
{
    uint32 model, current, frame, animation, table, row, packed, value, index, scale, input_view, output_view;
    sint32 x, y, z;
    sint16 input[4] = {0, 0, 0, 0};
    uint32 output[3];
    FUNCTION_MARKER(0x8003F508, "LEVEL2P.BIN");
    model = r_u32(owner + 0x100);
    if (!(r_u32(owner + 0xC) & 0x1000)) return;
    frame = (uint32)(sint32)r_s16(model + 0x5E);
    if ((sint32)frame < 0) return;
    if (!r_u32(model + 0x74))
    {
        table = r_u32(r_u32(model + 8) + 0x98);
        animation = (uint32)(sint32)r_s16(model + 0x5C);
        row = r_u32(table + (animation << 4) + 8) + frame * 24;
        packed = r_u32(row + 4);
    }
    else
    {
        animation = (uint32)(sint32)r_s16(model + 0x5C);
        row = r_u32(r_u32(r_u32(model + 0x78) + (animation << 2)) + 4) + frame * 20;
        value = r_u32(row + 4);
        w_u16(model + 0x98, (uint32)((sint32)(value << 22) >> 22));
        current = r_u32(owner + 0x100);
        frame = (uint32)(sint32)r_s16(model + 0x5E);
        table = r_u32(r_u32(r_u32(current + 0x78) + (animation << 2)) + 4);
        value = r_u32(table + frame * 20 + 4);
        w_u16(current + 0x9A, (uint32)((sint32)(value << 11) >> 21));
        animation = (uint32)(sint32)r_s16(model + 0x5C);
        current = r_u32(owner + 0x100);
        frame = (uint32)(sint32)r_s16(model + 0x5E);
        table = r_u32(r_u32(r_u32(current + 0x78) + (animation << 2)) + 4);
        value = r_u32(table + frame * 20 + 4);
        w_u16(current + 0x9C, (uint32)((sint32)(value << 1) >> 22));
        packed = r_u32(row);
    }
    x = (sint32)(packed << 22) >> 22;
    y = (sint32)(packed << 11) >> 21;
    z = (sint32)(packed << 1) >> 22;
    if (y < 0) level2p_sub_8006F44C(owner);
    for (index = 0; index < 5; index++) xport_gte_write_control(index, r_u32(owner + 0x340 + index * 4));
    scale = r_u32(owner + 0x224);
    if (scale != 0x10000)
    {
        x = (sint32)((uint32)x * scale) >> 16;
        y = (sint32)((uint32)y * scale) >> 16;
        z = (sint32)((uint32)z * scale) >> 16;
    }
    input[0] = (sint16)x;
    w_u16(owner + 0x27C, (uint32)x);
    input[1] = (sint16)y;
    w_u16(owner + 0x27E, (uint32)y);
    input[2] = (sint16)z;
    w_u16(owner + 0x280, (uint32)z);
    /* TODO SDK vector buffers need the permitted local-view adapter */
    input_view = moh_draft_local_address(input, sizeof(input));
    output_view = moh_draft_local_address(output, sizeof(output));
    moh_draft_missing_call(0x8001556C, input_view, output_view);
    w_u32(owner + 0x1F0, r_u32(owner + 0x1F0) + (output[0] << 16));
    w_u32(owner + 0x1F4, r_u32(owner + 0x1F4) + (output[1] << 16));
    w_u32(owner + 0x1F8, r_u32(owner + 0x1F8) + (output[2] << 16));
}

void level2p_sub_8004D148(uint32 owner, uint32 mask, uint32 parameter3, uint32 parameter4, uint32 parameter5)
{
    uint32 index = 0, offset = 0, entry, value;
    FUNCTION_MARKER(0x8004D148, "LEVEL2P.BIN");
    if (!r_u16(owner + 0x36)) return;
    do
    {
        entry = r_u32(owner + 0x304) + offset;
        if (r_u32(entry + 4) & mask)
        {
            if ((sint32)parameter3 > 0) w_u32(entry + 8, parameter3);
            else if ((sint32)parameter3 < 0) w_u32(entry + 8, r_u32(entry + 0xC));
            if ((sint32)parameter5 > 0) w_u32(r_u32(owner + 0x304) + offset + 0x10, parameter5);
            else if ((sint32)parameter5 < 0)
            {
                entry = r_u32(owner + 0x304) + offset;
                w_u32(entry + 0x10, r_u32(entry + 0x14));
            }
            if ((sint32)parameter4 > 0)
            {
                w_u32(r_u32(owner + 0x304) + offset + 0x34, parameter4);
                w_u32(r_u32(owner + 0x304) + offset + 0x38, parameter4 * parameter4);
            }
            else if ((sint32)parameter4 < 0)
            {
                entry = r_u32(owner + 0x304) + offset;
                w_u32(entry + 0x34, r_u32(entry + 0x30));
                entry = r_u32(owner + 0x304) + offset;
                value = r_u32(entry + 0x34);
                w_u32(entry + 0x38, value * value);
            }
        }
        index++;
        offset += 72;
    } while ((sint32)index < (sint32)r_u16(owner + 0x36));
}


uint32 level2p_sub_80065C9C(uint32 destination, uint32 data, uint32 parameter, uint32 owner)
{
    uint32 size = 0, cursor = 0, pointer, value, count, bytes, source, packed, first, second, index, offset, entry;
    FUNCTION_MARKER(0x80065C9C, "LEVEL2P.BIN");
    w_u32(destination + 0x6C, 128);
    w_u32(destination, 0);
    w_u32(destination + 0x68, 0);
    w_u32(destination + 4, parameter);
    w_u32(destination + 8, data);
    if (!(parameter & 1) && r_u32(data + 0x1C))
    {
        pointer = r_u32(data + 0x2C);
        if (pointer) size = (r_u32(pointer) << 4) + 20;
        pointer = r_u32(data + 0x30);
        if (pointer) size += 8 + (r_u32(pointer) << 2);
        pointer = r_u32(data + 0x34);
        if (pointer) size += 20 + (r_u32(pointer) << 4);
    }
    if (!(parameter & 2)) size += (uint32)r_u16(data + 0x8C) << 3;
    if (!r_u32(data + 0x14))
    {
        if (!(parameter & 0x40)) size += (uint32)r_u16(data + 0x8C) << 3;
        if (!(parameter & 0x100)) size += (uint32)r_u16(data + 0x8C) << 2;
    }
    if (!(parameter & 4)) size += (uint32)r_u16(data + 0x8E) << 3;
    if (!(parameter & 0x10)) size += 4096;
    if ((sint32)level2p_sub_80065BF8(0x42A7A5EF) >= 0) size += r_u16(data + 0xBE) * 176;
    pointer = r_u32(data + 8);
    if (pointer) size += r_u8(r_u32(pointer + 4) + 1) * 24;
    else
    {
        w_u32(destination + 0x74, 0);
        w_u16(destination + 0x70, 0);
        w_u16(destination + 0x72, 0);
        w_u32(destination + 0x7C, 0);
        w_u32(destination + 0x84, 0);
        w_u32(destination + 0x88, 0);
        w_u32(destination + 0x90, 0);
        w_u32(destination + 0x78, 0);
        w_u32(destination + 0x80, 0);
        w_u32(destination + 0x8C, 0);
        w_u32(destination + 0x94, 0);
    }
    if ((sint32)size > 0)
    {
        value = r_u32(data + 0xFC);
        if (value != 0x1402 && value != 0x1405) cursor = sub_80036BF0(size);
        if (!cursor) return 0;
        w_u32(destination, cursor);
    }
    pointer = r_u32(data + 8);
    if (pointer)
    {
        w_u16(destination + 0x70, r_u16(pointer));
        pointer = r_u32(data + 8);
        w_u32(destination + 0x78, pointer + 4);
        w_u16(destination + 0x72, r_u8(r_u32(pointer + 4) + 1));
        pointer = r_u32(data + 8);
        first = r_u16(destination + 0x72);
        bytes = (uint32)r_u16(destination + 0x72) << 3;
        w_u32(destination + 0x74, pointer);
        w_u32(destination + 0x7C, r_u32(data + 0x38));
        w_u32(destination + 0x80, r_u32(data + 0x84));
        value = r_u32(data + 0x9C);
        count = r_u16(destination + 0x72);
        w_u32(destination + 0x90, cursor);
        cursor += count << 3;
        pointer = r_u32(destination + 0x78);
        w_u32(destination + 0x8C, cursor);
        cursor += first << 3;
        w_u32(destination + 0x94, cursor);
        w_u32(destination + 0x84, value);
        source = r_u32(r_u32(r_u32(pointer) + 4) + 0x10);
        cursor += bytes;
        moh_draft_missing_call(0x800154EC, r_u32(destination + 0x8C), source, bytes);
        pointer = r_u32(r_u32(destination + 0x78));
        source = r_u32(r_u32(pointer + 4) + 0x10);
        bytes = (uint32)r_u16(destination + 0x72) << 3;
        moh_draft_missing_call(0x800154EC, r_u32(destination + 0x90), source, bytes);
        pointer = r_u32(destination + 0x78);
        w_u32(destination + 0x88, 0);
        packed = r_u32(r_u32(r_u32(pointer) + 4) + 4);
        pointer = r_u32(destination + 0x78);
        w_u16(destination + 0x98, (uint32)((sint32)(packed << 22) >> 22));
        packed = r_u32(r_u32(r_u32(pointer) + 4) + 4);
        w_u16(destination + 0x9A, (uint32)((sint32)(packed << 11) >> 21));
        packed = r_u32(r_u32(r_u32(pointer) + 4) + 4);
        w_u16(destination + 0x9C, (uint32)((sint32)(packed << 1) >> 22));
        first = r_u32(destination + 0x98);
        second = r_u32(destination + 0x9C);
        w_u32(destination + 0xA0, first);
        w_u32(destination + 0xA4, second);
    }
    if (!(parameter & 1) && r_u32(data + 0x1C))
    {
        w_u32(destination + 0x14, 0);
        w_u32(destination + 0x18, 0);
        w_u32(destination + 0x1C, 0);
        w_u32(destination + 0x20, 0);
        w_u32(destination + 0x24, 0);
        w_u32(destination + 0x28, 0);
        pointer = r_u32(data + 0x2C);
        if (pointer) w_u32(destination + 0x20, pointer);
        if (r_u32(data + 0x30))
        {
            w_u32(destination + 0x24, cursor);
            source = r_u32(data + 0x30);
            bytes = (r_u32(source) << 2) + 8;
            pointer = cursor;
            cursor += bytes;
            moh_draft_missing_call(0x800154EC, pointer, source, bytes);
        }
        pointer = r_u32(data + 0x34);
        if (pointer) w_u32(destination + 0x28, pointer);
    }
    else
    {
        w_u32(destination + 0x14, r_u32(data + 0x20));
        w_u32(destination + 0x1C, r_u32(data + 0x28));
        w_u32(destination + 0x18, r_u32(data + 0x24));
        w_u32(destination + 0x20, r_u32(data + 0x2C));
        w_u32(destination + 0x24, r_u32(data + 0x30));
        w_u32(destination + 0x28, r_u32(data + 0x34));
        if (!r_u32(data + 0x1C))
        {
            w_u32(data + 0x1C, 1);
            w_u32(destination + 4, r_u32(destination + 4) | 8);
        }
    }
    w_u16(destination + 0x3C, r_u16(data + 0x8C));
    if (!(parameter & 2))
    {
        w_u32(destination + 0x2C, cursor);
        bytes = (uint32)r_u16(data + 0x8C) << 3;
        source = r_u32(data + 0x88);
        pointer = cursor;
        cursor += bytes;
        moh_draft_missing_call(0x800154EC, pointer, source, bytes);
    }
    else w_u32(destination + 0x2C, r_u32(data + 0x88));
    if (!r_u32(data + 0x14))
    {
        if (!(parameter & 0x40))
        {
            w_u32(destination + 0x30, cursor);
            cursor += (uint32)r_u16(data + 0x8C) << 3;
        }
        w_u32(destination + 0x34, 0xFFFFFFFF);
        w_u32(destination + 0x38, 0xFFFFFFFF);
        if (!(parameter & 0x100))
        {
            w_u32(destination + 0x48, cursor);
            bytes = (uint32)r_u16(data + 0x8C) << 2;
            moh_draft_missing_call(0x800154EC, cursor, r_u32(data + 0xC4), bytes);
            cursor += (uint32)r_u16(data + 0x8C) << 2;
        }
        else w_u32(destination + 0x48, r_u32(data + 0xC4));
    }
    w_u16(destination + 0x42, r_u16(data + 0x8E));
    if (!(parameter & 4))
    {
        /* TODO Original admits this copy by the existing destination+44 field */
        if (r_u32(destination + 0x44))
        {
            w_u32(destination + 0x44, cursor);
            bytes = (uint32)r_u16(data + 0x8E) << 3;
            source = r_u32(data + 0x90);
            pointer = cursor;
            cursor += bytes;
            moh_draft_missing_call(0x800154EC, pointer, source, bytes);
        }
        else w_u32(destination + 0x44, 0);
    }
    else w_u32(destination + 0x44, r_u32(data + 0x90));
    if (!(parameter & 0x10))
    {
        w_u32(destination + 0x4C, cursor);
        w_u32(destination + 0x50, cursor + 0x800);
    }
    else
    {
        w_u32(destination + 0x4C, 0);
        w_u32(destination + 0x50, 0);
    }
    w_u16(destination + 0x5C, 0xFFFF);
    w_u16(destination + 0x5E, 0);
    level2p_sub_80068A04(destination + 0x54, 0);
    w_u32(destination + 0x58, 0);
    if (owner)
    {
        w_u32(owner + 0x2D4, r_u32(data + 0xA0));
        w_u32(owner + 0x270, r_u32(data + 0xA4));
        w_u32(owner + 0x274, r_u32(data + 0xA8));
        w_u32(owner + 0x278, r_u32(data + 0xAC));
        w_u16(owner + 0x36, r_u16(data + 0xBE));
        count = r_u16(data + 0xBE);
        for (index = 0, offset = 0; (sint32)index < (sint32)count; index++, offset += 72)
        {
            source = r_u32(data + 0xC0) + offset;
            entry = r_u32(owner + 0x304) + offset;
            w_u32(entry + 4, r_u32(source + 4));
            w_u32(r_u32(owner + 0x304) + offset + 8, r_u32(r_u32(data + 0xC0) + offset + 0xC));
            w_u32(r_u32(owner + 0x304) + offset + 0xC, r_u32(r_u32(data + 0xC0) + offset + 0xC));
            w_u16(r_u32(owner + 0x304) + offset + 0x1C, r_u16(r_u32(data + 0xC0) + offset + 0x1C));
            w_u16(r_u32(owner + 0x304) + offset + 0x1E, r_u16(r_u32(data + 0xC0) + offset + 0x1E));
            w_u32(r_u32(owner + 0x304) + offset + 0x34, r_u32(r_u32(data + 0xC0) + offset + 0x34));
            w_u32(r_u32(owner + 0x304) + offset + 0x30, r_u32(r_u32(data + 0xC0) + offset + 0x30));
            w_u32(r_u32(owner + 0x304) + offset + 0x38, r_u32(r_u32(data + 0xC0) + offset + 0x38));
            w_u32(r_u32(owner + 0x304) + offset + 0x10, r_u32(r_u32(data + 0xC0) + offset + 0x10));
            w_u32(r_u32(owner + 0x304) + offset + 0x14, r_u32(r_u32(data + 0xC0) + offset + 0x14));
            w_u16(r_u32(owner + 0x304) + offset + 0x40, r_u16(r_u32(data + 0xC0) + offset + 0x40));
            w_u16(r_u32(owner + 0x304) + offset + 0x42, r_u16(r_u32(data + 0xC0) + offset + 0x42));
            w_u16(r_u32(owner + 0x304) + offset + 0x44, r_u16(r_u32(data + 0xC0) + offset + 0x44));
            w_u32(r_u32(owner + 0x304) + offset + 0x18, destination);
            if (parameter & 0x200)
            {
                entry = r_u32(owner + 0x304) + offset;
                w_u32(entry + 0x34, r_u32(entry + 0x34) << 1);
                entry = r_u32(owner + 0x304) + offset;
                w_u32(entry + 0x30, r_u32(entry + 0x30) << 1);
                entry = r_u32(owner + 0x304) + offset;
                w_u32(entry + 0x38, r_u32(entry + 0x38) << 1);
            }
            w_u32(r_u32(owner + 0x304) + offset + 0x3C, 0);
            count = r_u16(data + 0xBE);
        }
    }
    else
    {
        value = r_u32(data);
        if (value == 0xB94537A4 || value == 0x96FE167F)
        {
            count = r_u16(data + 0xBE);
            for (index = 0, offset = 0; (sint32)index < (sint32)count; index++, offset += 72)
            {
                w_u32(r_u32(data + 0xC0) + offset + 0x18, destination);
                count = r_u16(data + 0xBE);
            }
        }
    }
    return 1;
}


void level2p_sub_8006F44C(uint32 owner)
{
    FUNCTION_MARKER(0x8006F44C, "LEVEL2P.BIN");
    w_u32(owner + 0x284, 0);
}

void level2p_sub_8004D8D4(uint32 owner)
{
    uint32 index = 0, offset = 0, entry, axis, radius, position, value;
    sint32 limit;
    FUNCTION_MARKER(0x8004D8D4, "LEVEL2P.BIN");
    w_u16(owner + 0x308, 0x7FFF);
    w_u16(owner + 0x30A, 0x7FFF);
    w_u16(owner + 0x30C, 0x7FFF);
    w_u16(owner + 0x30E, 0x8001);
    w_u16(owner + 0x310, 0x8001);
    w_u16(owner + 0x312, 0x8001);
    while ((sint32)index < (sint32)r_u16(owner + 0x36))
    {
        entry = r_u32(owner + 0x304) + offset;
        for (axis = 0; axis < 3; axis++)
        {
            radius = (uint32)(sint32)r_s16(entry + 0x34);
            position = r_u32(entry + 0x20 + axis * 4);
            limit = r_s16(owner + 0x308 + axis * 2);
            value = position - radius;
            if (limit < (sint32)value) value = (uint32)limit;
            w_u16(owner + 0x308 + axis * 2, value);
        }
        for (axis = 0; axis < 3; axis++)
        {
            radius = (uint32)(sint32)r_s16(entry + 0x34);
            position = r_u32(entry + 0x20 + axis * 4);
            limit = r_s16(owner + 0x30E + axis * 2);
            value = position + radius;
            if ((sint32)value <= limit) value = (uint32)limit;
            w_u16(owner + 0x30E + axis * 2, value);
        }
        offset += 72;
        index++;
    }
}

void level2p_sub_8006591C(uint32 owner)
{
    sint16 rotation[4] = {0, 0, 0, 0};
    uint32 translation[4] = {0, 0, 0, 0}, scale[4];
    uint32 rotation_view, translation_view, scale_view, first, second, third;
    FUNCTION_MARKER(0x8006591C, "LEVEL2P.BIN");
    first = r_u32(0x8003CD48);
    second = r_u32(0x8003CD4C);
    third = r_u32(0x8003CD50);
    scale[0] = first;
    scale[1] = second;
    scale[2] = third;
    scale[3] = r_u32(0x8003CD54);
    rotation[0] = r_s16(owner + 0x22A);
    rotation[1] = r_s16(owner + 0x22E);
    rotation[2] = r_s16(owner + 0x232);
    /* TODO SDK local vectors need the permitted addressable-local adapter */
    rotation_view = moh_draft_local_address(rotation, sizeof(rotation));
    translation_view = moh_draft_local_address(translation, sizeof(translation));
    scale_view = moh_draft_local_address(scale, sizeof(scale));
    moh_draft_missing_call(0x8001AF98, rotation_view, owner + 0x340);
    translation[0] = (uint32)(sint32)r_s16(owner + 0x20E);
    translation[1] = (uint32)(sint32)r_s16(owner + 0x212);
    translation[2] = (uint32)(sint32)r_s16(owner + 0x216);
    moh_draft_missing_call(0x80016218, owner + 0x340, translation_view);
    scale[0] = (uint32)(r_s32(owner + 0x270) >> 4);
    scale[1] = (uint32)(r_s32(owner + 0x274) >> 4);
    scale[2] = (uint32)(r_s32(owner + 0x278) >> 4);
    moh_draft_missing_call(0x800165D8, owner + 0x340, scale_view);
}

void level2p_sub_8004DFBC(uint32 owner)
{
    uint32 matrix[8], rotation[3], matrix_view, rotation_view, transform, index, first, second, third;
    uint32 count, flags, model, records, data, resource;
    sint32 bone;
    FUNCTION_MARKER(0x8004DFBC, "LEVEL2P.BIN");
    if (r_u32(owner + 0x3BC) == r_u32(0x800993FC)) return;
    bone = r_s16(owner + 0x2EA);
    if (bone == -1)
    {
        if (r_u32(owner + 0xC) & 1)
        {
            rotation[0] = 0;
            rotation[1] = r_u32(owner + 0x22C);
            rotation[2] = r_u32(owner + 0x230);
            /* TODO Addressable rotation and matrix use the permitted local adapter */
            rotation_view = moh_draft_local_address(rotation, sizeof(rotation));
            matrix_view = moh_draft_local_address(matrix, sizeof(matrix));
            level2p_sub_8008BB44(owner + 0x20C, rotation_view, owner + 0x270, matrix_view);
            transform = matrix_view;
        }
        else transform = owner + 0x340;
        for (index = 0; index < 8; index++) xport_gte_write_control(index, r_u32(transform + index * 4));
        xport_gte_write_data(0, r_u32(owner + 0x2D8));
        xport_gte_write_data(1, r_u32(owner + 0x2DC));
        xport_gte_execute(0x480012);
        first = xport_gte_read_data(25);
        second = xport_gte_read_data(26);
        third = xport_gte_read_data(27);
        w_u16(owner + 0x2E0, first);
        w_u16(owner + 0x2E2, second);
        w_u16(owner + 0x2E4, third);
    }
    else level2p_sub_8008C5E4(owner, (uint32)bone, owner + 0x2E0, 0, 0);
    if (!r_u16(owner + 0x36)) return;
    count = r_u16(owner + 0x36);
    w_u16(owner + 0x308, 0x8001);
    w_u16(owner + 0x30A, 0x8001);
    w_u16(owner + 0x30C, 0x8001);
    w_u16(owner + 0x30E, 0x7FFF);
    w_u16(owner + 0x310, 0x7FFF);
    w_u16(owner + 0x312, 0x7FFF);
    flags = r_u32(owner + 0xC) & 1;
    model = r_u32(owner + 0x100);
    records = r_u32(owner + 0x304);
    level2p_sub_8004DA08(owner, model, owner + 0x340, records, count, flags);
    if (r_u32(owner + 0xC) & 1)
    {
        data = r_u32(owner + 0x388);
        resource = r_u32(data + 0xAC);
        count = r_u16(resource + 0xBE);
        records = r_u32(resource + 0xC0);
        level2p_sub_8004DA08(owner, data + 0xA4, data + 0x154, records, count, 0);
    }
    w_u32(owner + 0x3BC, r_u32(0x800993FC));
}

void level2p_sub_800824D4(uint32 kind, uint32 index_out, uint32 id_out)
{
    uint32 index, entry = 0x8009ABF8;
    FUNCTION_MARKER(0x800824D4, "LEVEL2P.BIN");
    for (index = 0; index < 43; index++, entry += 52)
    {
        if (r_u32(entry) == kind)
        {
            w_u32(index_out, index);
            w_u32(id_out, r_u32(entry + 4));
            return;
        }
    }
    /* TODO Original leaves both output words untouched when no type matches */
}

uint32 level2p_sub_80082518(uint32 object, uint32 data, uint32 id)
{
    uint32 selection[2], selection_view, owner, entry, model, axis, value, storage, target, maximum;
    sint16 width[3], height;
    sint32 difference, bound;
    FUNCTION_MARKER(0x80082518, "LEVEL2P.BIN");
    /* TODO No-match lookup retains unspecified original local output words */
    selection_view = moh_draft_local_address(selection, sizeof(selection));
    level2p_sub_800824D4(data, selection_view, selection_view + 4);
    if (id == 0xFFFFFFFF) id = selection[1];
    entry = 0x8009ABF8 + selection[0] * 52;
    owner = level2p_sub_80066934(3, r_u16(entry + 8), id, 0x147, 0, 0, 0, 0);
    if (!owner) return 0;
    w_u32(owner + 8, 0x65);
    w_u32(owner + 0x10, 0x211);
    w_u32(owner + 0x36C, 0x800804C8);
    w_u32(owner + 0xC, 0x47020200);
    w_u32(owner + 0x234, 0);
    w_u32(owner + 0x238, 0);
    w_u32(owner + 0x23C, 0);
    w_u32(owner + 0x374, 0);
    w_u32(owner + 0x370, 0x80065698);
    w_u32(owner + 0x378, 0);
    target = r_u32(entry + 0x14);
    w_u32(owner + 0x384, 0x8006543C);
    w_u32(owner + 0x14, 0x8000001);
    w_u32(owner + 0x37C, target);
    w_u32(owner + 0x20C, (uint32)(sint32)r_s16(object + 0x10) << 16);
    value = (uint32)(sint32)r_s16(object + 0x12);
    model = r_u32(owner + 0x100);
    w_u32(owner + 0x210, (value - 10) << 16);
    value = (uint32)(sint32)r_s16(object + 0x14);
    w_u16(owner + 0x2EC, 0);
    w_u16(owner + 0x36, 0);
    w_u32(owner + 0x214, value << 16);
    model = r_u32(model + 8);
    for (axis = 0; axis < 3; axis++)
    {
        difference = r_s16(model + 0x7C + axis * 2) - r_s16(model + 0x44 + axis * 2);
        if (difference < 0) difference = -difference;
        width[axis] = (sint16)difference;
    }
    bound = width[1];
    if (bound <= width[0]) bound = width[0];
    if (bound <= width[2]) bound = width[2];
    maximum = (uint32)((bound + 30) >> 1);
    w_u16(owner + 0x2E8, maximum);
    for (axis = 0; axis < 3; axis++)
        w_u16(owner + 0x2D8 + axis * 2, (uint32)(r_s16(model + 0x44 + axis * 2) + (width[axis] >> 1)));
    height = (sint16)level2p_sub_8005BDE4(owner, 0);
    if (height == -32767) w_u32(owner + 0x36C, 0x80080508);
    else w_u32(owner + 0x210, ((uint32)(sint32)height - 6) << 16);
    if (r_u32(object + 0x58) & 1)
    {
        w_u32(owner + 0xC, r_u32(owner + 0xC) | 0x8000);
        value = (uint32)(sint32)r_s16(object + 0x5E);
        w_u32(owner + 0x294, value);
        w_u32(owner + 0x290, value);
    }
    storage = r_u32(owner + 0x388);
    value = r_u16(entry + 0xA);
    w_u32(storage + 4, selection[0]);
    w_u32(storage + 8, id);
    w_u32(storage + 0x10, 900);
    w_u32(storage, value);
    if (r_u32(object + 0x30) & 8) w_u32(storage + 0x14, (uint32)(sint32)r_s16(object + 0x36));
    else w_u32(storage + 0x14, 0xFFFFFFFF);
    w_u32(storage + 0x18, 0x8009AC10 + selection[0] * 52);
    w_u32(storage + 0x1C, r_u32(entry + 0x2C));
    target = r_u32(entry + 0x10);
    moh_draft_indirect_call(target, object, storage);
    value = r_u32(storage + 0x10);
    w_u32(storage + 0xC, value);
    if (value)
    {
        level2p_sub_8006591C(owner);
        level2p_sub_8004DFBC(owner);
        return owner;
    }
    level2p_sub_80066C64(owner);
    return 0;
}


void level2p_sub_8008BB44(uint32 position, uint32 rotation, uint32 scale, uint32 output)
{
    sint16 angles[4];
    uint32 vector[3], angles_view, vector_view;
    FUNCTION_MARKER(0x8008BB44, "LEVEL2P.BIN");
    angles[0] = r_s16(rotation + 2);
    angles[1] = r_s16(rotation + 6);
    angles[2] = r_s16(rotation + 10);
    /* TODO SDK local vectors need the permitted addressable-local adapter */
    angles_view = moh_draft_local_address(angles, sizeof(angles));
    vector_view = moh_draft_local_address(vector, sizeof(vector));
    moh_draft_missing_call(0x8001AF98, angles_view, output);
    vector[0] = (uint32)(sint32)r_s16(position + 2);
    vector[1] = (uint32)(sint32)r_s16(position + 6);
    vector[2] = (uint32)(sint32)r_s16(position + 10);
    moh_draft_missing_call(0x80016218, output, vector_view);
    vector[0] = (uint32)(r_s32(scale) >> 4);
    vector[1] = (uint32)(r_s32(scale + 4) >> 4);
    vector[2] = (uint32)(r_s32(scale + 8) >> 4);
    moh_draft_missing_call(0x800165D8, output, vector_view);
}

void level2p_sub_8008C5E4(uint32 owner, uint32 bone, uint32 output, uint32 parameter4, uint32 parameter5)
{
    FUNCTION_MARKER(0x8008C5E4, "LEVEL2P.BIN");
    level2p_sub_8008BCF0(owner, r_u32(owner + 0x100), bone, output, parameter4, parameter5);
}

uint32 level2p_sub_8005BDE4(uint32 owner, uint32 mode)
{
    FUNCTION_MARKER(0x8005BDE4, "LEVEL2P.BIN");
    /* Original1535C calls exchange only guest SP; native C keeps its ordinary call stack */
    return (uint32)(sint32)(sint16)level2p_sub_8005BD4C(owner, mode);
}

uint32 level2p_sub_8005BD4C(uint32 owner, uint32 output)
{
    uint32 position[3], result_data[2], position_view, result_view, result, surface, first, second;
    FUNCTION_MARKER(0x8005BD4C, "LEVEL2P.BIN");
    position[0] = r_u32(owner + 0x20C);
    position[1] = r_u32(owner + 0x210);
    position[2] = r_u32(owner + 0x214);
    /* TODO Addressable ground-query buffers need the permitted local adapter */
    position_view = moh_draft_local_address(position, sizeof(position));
    result_view = moh_draft_local_address(result_data, sizeof(result_data));
    w_u32(0x800AADF4, 0xFFFF);
    result = level2p_sub_8005BBFC(position_view, 0x800AADF4, result_view);
    surface = r_u32(0x800AADF4);
    first = result_data[0];
    second = result_data[1];
    w_u32(output, first);
    w_u32(output + 4, second);
    w_u32(owner + 0x390, surface);
    return (uint32)(sint32)(sint16)result;
}

static void moh_level2p_collision_offset(uint32 first, uint32 second, uint32 offset, sint16 output[3])
{
    uint32 a = (uint32)(sint32)(sint16)first, b = (uint32)((sint32)first >> 16);
    uint32 c = (uint32)(sint32)(sint16)second, d = (uint32)((sint32)second >> 16);
    uint32 doubled_a, doubled_b, doubled_d, packed, axis, pass1[3], pass2[3], pass3[2], matrix[5], diagonal;
    xport_gte_write_control(0, (d << 16) | (a & 0xFFFF));
    packed = (b << 16) | (c & 0xFFFF);
    xport_gte_write_control(1, packed);
    xport_gte_write_control(3, packed);
    xport_gte_write_control(2, ((0 - c) & 0xFFFF) | (d << 16));
    xport_gte_write_control(4, 0 - a);
    doubled_a = a << 1;
    doubled_d = d << 1;
    doubled_b = b << 1;
    xport_gte_write_data(0, (d << 17) | (doubled_a & 0xFFFF));
    xport_gte_write_data(1, 0);
    xport_gte_execute(0x486012);
    for (axis = 0; axis < 3; axis++) pass1[axis] = xport_gte_read_data(25 + axis);
    xport_gte_write_data(0, doubled_b & 0xFFFF);
    xport_gte_write_data(1, doubled_d);
    xport_gte_execute(0x486012);
    for (axis = 0; axis < 3; axis++) pass2[axis] = xport_gte_read_data(25 + axis);
    xport_gte_write_data(0, (0 - doubled_b) << 16);
    xport_gte_write_data(1, doubled_a);
    xport_gte_execute(0x486012);
    pass3[0] = xport_gte_read_data(25);
    pass3[1] = xport_gte_read_data(26);
    diagonal = ((uint32)((sint32)(c * c) >> 12) + (uint32)((sint32)(d * d) >> 12)) * 2 - 4096;
    matrix[0] = ((pass2[0] & 0xFFFF) << 16) | ((pass1[0] - 4096) & 0xFFFF);
    matrix[1] = ((pass1[1] & 0xFFFF) << 16) | (pass3[0] & 0xFFFF);
    matrix[2] = ((pass3[1] & 0xFFFF) << 16) | ((pass2[1] - 4096) & 0xFFFF);
    matrix[3] = ((pass2[2] & 0xFFFF) << 16) | (pass1[2] & 0xFFFF);
    matrix[4] = (uint32)(sint32)(sint16)diagonal;
    for (axis = 0; axis < 5; axis++) xport_gte_write_control(axis, matrix[axis]);
    for (axis = 5; axis < 8; axis++) xport_gte_write_control(axis, 0);
    xport_gte_write_data(0, r_u32(offset));
    xport_gte_write_data(1, r_u32(offset + 4));
    xport_gte_execute(0x480012);
    for (axis = 0; axis < 3; axis++) output[axis] = (sint16)xport_gte_read_data(25 + axis);
}

void level2p_sub_8004DA08(uint32 owner, uint32 model, uint32 matrix, uint32 records, uint32 count, uint32 attached)
{
    uint32 index, axis, first_index, second_index, base, first_point, second_point, handle, related;
    uint32 point[3], data, quaternion, first, second, radius, position, value;
    sint16 offset[3];
    sint32 average, limit;
    FUNCTION_MARKER(0x8004DA08, "LEVEL2P.BIN");
    count &= 0xFFFF;
    for (index = 0; (sint32)index < (sint32)count; index++, records += 72)
    {
        if (!r_u32(records + 0x18)) continue;
        first_index = r_u16(records + 0x1C);
        second_index = r_u16(records + 0x1E);
        if (first_index == 0xFFFE)
        {
            handle = r_u32(r_u32(owner + 0x388) + 0xA0);
            related = handle ? r_u32(0x80099460 + ((handle & 0xFFFF) << 2)) : 0;
            point[0] = r_u16(records + 0x40);
            data = r_u32(related + 0x388);
            point[1] = (r_u16(records + 0x42) + r_u16(data + 0x94)) & 0xFFFF;
            point[2] = r_u16(records + 0x44);
        }
        else if (first_index == 0xFFFF)
        {
            point[0] = r_u16(records + 0x40);
            point[1] = r_u16(records + 0x42);
            point[2] = r_u16(records + 0x44);
        }
        else
        {
            if (r_u32(model + 0x84))
            {
                offset[0] = 0;
                offset[1] = 0;
                offset[2] = 0;
                base = r_u32(model + 0x94);
                first_point = base + first_index * 8;
                second_point = base + second_index * 8;
                if ((r_u32(records + 0x40) || r_s16(records + 0x44)) && r_u32(model + 0x88))
                {
                    quaternion = r_u32(model + 0x88) + first_index * 8;
                    first = r_u32(quaternion);
                    second = r_u32(quaternion + 4);
                    moh_level2p_collision_offset(first, second, records + 0x40, offset);
                }
                for (axis = 0; axis < 3; axis++)
                {
                    average = (r_s16(first_point + axis * 2) + r_s16(second_point + axis * 2)) >> 1;
                    point[axis] = ((uint32)(sint32)offset[axis] + (uint32)average) & 0xFFFF;
                }
            }
            else
            {
                base = r_u32(r_u32(records + 0x18) + 0x2C);
                first_point = base + first_index * 8;
                second_point = base + second_index * 8;
                for (axis = 0; axis < 3; axis++)
                {
                    average = (r_s16(first_point + axis * 2) + r_s16(second_point + axis * 2)) >> 1;
                    point[axis] = (uint32)average & 0xFFFF;
                }
            }
            if (attached)
            {
                handle = r_u32(r_u32(owner + 0x388) + 0xA0);
                related = handle ? r_u32(0x80099460 + ((handle & 0xFFFF) << 2)) : 0;
                data = r_u32(related + 0x388);
                point[1] = (point[1] + r_u16(data + 0x94)) & 0xFFFF;
            }
        }
        for (axis = 0; axis < 8; axis++) xport_gte_write_control(axis, r_u32(matrix + axis * 4));
        xport_gte_write_data(0, point[0] | (point[1] << 16));
        /* VZ0 discards the original unspecified upper half of the local word */
        xport_gte_write_data(1, (uint32)(sint32)(sint16)point[2]);
        xport_gte_execute(0x480012);
        for (axis = 0; axis < 3; axis++) w_u32(records + 0x20 + axis * 4, xport_gte_read_data(25 + axis));
        for (axis = 0; axis < 3; axis++)
        {
            radius = (uint32)(sint32)r_s16(records + 0x34);
            position = r_u32(records + 0x20 + axis * 4);
            limit = r_s16(owner + 0x308 + axis * 2);
            value = position - radius;
            if (limit < (sint32)value) value = (uint32)limit;
            w_u16(owner + 0x308 + axis * 2, value);
        }
        for (axis = 0; axis < 3; axis++)
        {
            radius = (uint32)(sint32)r_s16(records + 0x34);
            position = r_u32(records + 0x20 + axis * 4);
            limit = r_s16(owner + 0x30E + axis * 2);
            value = position + radius;
            if ((sint32)value <= limit) value = (uint32)limit;
            w_u16(owner + 0x30E + axis * 2, value);
        }
    }
}
