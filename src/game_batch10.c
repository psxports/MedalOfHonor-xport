#include "game_level2p_signatures.h"
#include "xport_trace.h"
#include "psx.h"

uint32 level2p_sub_8006B4F8(uint32 x, uint32 y, uint32 z, uint32 direction, uint32 red, uint32 green, uint32 blue, uint32 parameter8, uint32 parameter9, uint32 parameter10, uint32 count, uint32 mode)
{
    sint16 vector[3] = { 0 };
    uint8 matrix[32] = { 0 };
    uint32 owner, data, index, record, random, scale, vector_view, matrix_view;
    uint32 first, second, third, end_z;
    uint8 first_red = (uint8)red, first_green = (uint8)green, first_blue = (uint8)blue;
    FUNCTION_MARKER(0x8006B4F8, "LEVEL2P.BIN");
    owner = level2p_sub_80066934(0, 0x130, 0xFFFFFFFF, 0, 0, 0, 0, 0);
    if (!owner) return 0;
    data = r_u32(owner + 0x388);
    w_u8(data + 1, mode);
    w_u8(data, count);
    w_u32(owner + 8, 0x23);
    w_u32(owner + 0x10, 0x203);
    w_u32(owner + 0x36C, 0x8006B3F4);
    w_u32(owner + 0x370, 0x8006A9D4);
    w_u32(owner + 0x14, 0);
    w_u32(owner + 0xC, 0);
    w_u32(owner + 0x374, 0);
    w_u32(owner + 0x378, 0);
    w_u32(owner + 0x384, 0x800653D8);
    w_u32(owner + 0x20C, x);
    w_u32(owner + 0x210, y);
    w_u32(owner + 0x270, 0x10000);
    w_u32(owner + 0x274, 0x10000);
    w_u32(owner + 0x278, 0x10000);
    w_u32(owner + 0x228, 0);
    w_u32(owner + 0x22C, 0);
    w_u32(owner + 0x230, 0);
    w_u32(owner + 0x284, 0);
    w_u32(owner + 0x214, z);
    w_u16(data + 6, 0x3FF);
    w_u16(data + 2, 1);
    w_u16(data + 4, 1);
    w_u16(data + 0xA, 0x3E0);
    w_u16(data + 8, 0);
    w_u16(data + 0xC, 0xFD);
    w_u16(data + 0xE, 4);
    for (index = 0; (sint32)index < (sint32)count; ++index)
    {
        record = data + 0x10 + index * 36;
        random = sub_8001552C();
        w_u32(record + 4, x + (((random & 15) - 8) << 16));
        w_u32(record + 8, y + ((((random >> 4) & 15) - 8) << 16));
        w_u32(record + 0xC, z + ((((random >> 8) & 15) - 8) << 16));
        random = sub_8001552C();
        scale = (random & 0x4B) + 0x10;
        random = sub_8001552C();
        vector[0] = (sint16)(((random & 31) - 16) << 5);
        vector[1] = (sint16)((((random >> 5) & 31) - 16) << 5);
        vector[2] = (sint16)((((random >> 10) & 31) - 16) << 5);
        /* TODO: Addressable vector and matrix require permitted local adapters */
        vector_view = moh_draft_local_address(vector, sizeof(vector));
        matrix_view = moh_draft_local_address(matrix, sizeof(matrix));
        moh_draft_missing_call(0x8001676C, vector_view, matrix_view);
        moh_draft_missing_call(0x8001DAE0, matrix_view, direction, vector_view);
        xport_gte_write_data(8, scale);
        xport_gte_write_data(9, (uint16)vector[0]);
        xport_gte_write_data(10, (uint16)vector[1]);
        xport_gte_write_data(11, (uint16)vector[2]);
        xport_gte_execute(0x198003D);
        first = xport_gte_read_data(9);
        second = xport_gte_read_data(10);
        third = xport_gte_read_data(11);
        vector[0] = (sint16)first;
        vector[1] = (sint16)second;
        vector[2] = (sint16)third;
        w_u32(record + 0x10, r_u32(record + 4) + ((uint32)(sint32)vector[0] << 16));
        w_u32(record + 0x14, r_u32(record + 8) + ((uint32)(sint32)vector[1] << 16));
        end_z = r_u32(record + 0xC);
        w_u8(record + 0x1C, first_red);
        w_u8(record + 0x1E, first_blue);
        w_u8(record + 0x1D, first_green);
        w_u8(record + 0x20, first_red);
        w_u8(record + 0x22, first_blue);
        w_u32(record + 0x18, end_z + ((uint32)(sint32)vector[2] << 16));
        w_u8(record + 0x21, first_green);
    }
    return owner;
}

uint32 level2p_sub_8006B804(uint32 position, uint32 rotation, uint32 scale, uint32 first_color, uint32 second_color)
{
    sint16 vector[3] = { 0 };
    uint32 vector_view, first, second, third, red, green, blue, other_red, other_green, other_blue;
    uint32 x, y, z, owner;
    FUNCTION_MARKER(0x8006B804, "LEVEL2P.BIN");
    /* TODO: Addressable SDK output requires the permitted local adapter */
    vector_view = moh_draft_local_address(vector, sizeof(vector));
    moh_draft_missing_call(0x80016A40, rotation, vector_view);
    xport_gte_write_data(8, scale);
    xport_gte_write_data(9, (uint16)vector[0]);
    xport_gte_write_data(10, (uint16)vector[1]);
    xport_gte_write_data(11, (uint16)vector[2]);
    xport_gte_execute(0x198003D);
    first = xport_gte_read_data(9);
    second = xport_gte_read_data(10);
    third = xport_gte_read_data(11);
    vector[0] = (sint16)first;
    vector[1] = (sint16)second;
    vector[2] = (sint16)third;
    red = (uint32)(sint32)(sint8)r_u8(first_color);
    green = (uint32)(sint32)(sint8)r_u8(first_color + 1);
    blue = (uint32)(sint32)(sint8)r_u8(first_color + 2);
    other_red = (uint32)(sint32)(sint8)r_u8(second_color);
    other_green = (uint32)(sint32)(sint8)r_u8(second_color + 1);
    other_blue = (uint32)(sint32)(sint8)r_u8(second_color + 2);
    x = r_u32(position);
    y = r_u32(position + 4);
    z = r_u32(position + 8);
    /* Original callee ignores the three second-color carriers and extra thirteenth word */
    owner = level2p_sub_8006B4F8(x, y, z, vector_view, red, green, blue,
        other_red, other_green, other_blue, 8, 4);
    if (owner) level2p_sub_80066BA8(owner);
    return owner;
}


uint32 level2p_sub_800441E0(uint32 owner, uint32 parameter2, uint32 target, uint32 index)
{
    uint32 data, target_view, kind, random, record, value, position, midpoint;
    FUNCTION_MARKER(0x800441E0, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388);
    /* TODO: Addressable target word requires the permitted local adapter */
    target_view = moh_draft_local_address(&target, sizeof(target));
    if (level2p_sub_80068988(data + 0x54, target_view)) return 0;
    kind = r_u32(target + 8);
    if (kind - 0x13F0 < 10 || kind == 0x25) return 0;
    level2p_sub_8004CC0C(owner, parameter2, target, index);
    if (r_u16(data + 0x38) & 0x100)
    {
        w_u16(owner + 0x368, 0);
        w_u16(owner + 0x366, 0);
        w_u16(owner + 0x364, 0);
        level2p_sub_8006CD4C(owner, 6, 6, 0x4B, 0x10000, 0x1000, 0x40000, 5, 0);
        level2p_sub_800431A0(owner);
        level2p_sub_80066C64(owner);
        return 1;
    }
    if (r_u32(target + 8) == 0x1402)
    {
        random = sub_8001552C();
        level2p_sub_800420C4(0, (uint32)((sint32)random >> 15) + 12,
            owner, 0x20000, 0, 0x10000, 0x4CCD, 0, 0, 0, 0);
    }
    level2p_sub_80066C64(owner);
    /* MIPS retains these reads and computes three discarded native local midpoints */
    record = r_u32(target + 0x304) + index * 72;
    position = r_u32(owner + 0x20C);
    value = r_u32(record + 0x20);
    midpoint = (uint32)((sint32)((value << 16) + position) >> 1);
    value = r_u32(record + 0x24);
    position = r_u32(owner + 0x210);
    midpoint = (uint32)((sint32)((value << 16) + position) >> 1);
    value = r_u32(record + 0x28);
    position = r_u32(owner + 0x214);
    midpoint = (uint32)((sint32)((value << 16) + position) >> 1);
    return 1;
}

uint32 level2p_sub_800443BC(uint32 owner, uint32 parameter2, uint32 target, uint32 parameter4)
{
    uint32 data, target_view, kind;
    FUNCTION_MARKER(0x800443BC, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388);
    /* TODO: Addressable target word requires the permitted local adapter */
    target_view = moh_draft_local_address(&target, sizeof(target));
    if (level2p_sub_80068988(data + 0x54, target_view)) return 0;
    kind = r_u32(target + 8);
    if (kind - 0x13F0 < 10 || kind == 0x1777) return 0;
    level2p_sub_8004CC0C(owner, parameter2, target, parameter4);
    if (r_u16(data + 0x38) & 0x100)
    {
        w_u16(owner + 0x368, 0);
        w_u16(owner + 0x366, 0);
        w_u16(owner + 0x364, 0);
        level2p_sub_8006CD4C(owner, 6, 6, 0x4B, 0x10000, 0x1000, 0x40000, 5, 0);
        level2p_sub_800431A0(owner);
        level2p_sub_80066C64(owner);
    }
    return 1;
}


uint32 level2p_sub_8004CC0C(uint32 owner, uint32 index, uint32 target, uint32 target_index)
{
    sint16 origin[3], other[3];
    uint32 base, record, target_record = 0, damage = 0, kind, value, original, special = 0;
    uint32 side, origin_view, other_view, first_kind, second_kind, bits;
    sint32 height;
    FUNCTION_MARKER(0x8004CC0C, "LEVEL2P.BIN");
    if ((sint32)index < 0) return 0;
    if ((sint32)target_index < 0 && (r_u32(owner + 0xC) & 1))
        base = r_u32(r_u32(r_u32(owner + 0x388) + 0xAC) + 0xC0);
    else base = r_u32(owner + 0x304);
    record = base + index * 72;
    if ((sint32)target_index < 0) damage = r_u32(target + 0xF8);
    else
    {
        target_record = r_u32(target + 0x304) + target_index * 72;
        kind = r_u32(target_record + 8);
        if (kind == 9 || kind == 5 || kind == 6) damage = r_u32(target_record + 0x10);
    }
    if (damage)
    {
        original = damage;
        if (r_u32(owner + 0xC) & 1)
            special = r_u32(r_u32(owner + 0x388) + 0x72C) & 0x10;
        if (special)
        {
            damage = 1;
            if ((r_u32(record + 4) & 0x200) && r_u32(target + 8) != 0x31)
                damage = original << 1;
        }
        else
        {
            /* Jump table8003A2E0 reviewed from immutable LEVEL2P bytes */
            kind = r_u32(record + 8);
            switch (kind)
            {
            case 2: damage = (uint32)((sint32)damage >> 1) + (uint32)((sint32)damage >> 2); break;
            case 3: damage = (uint32)((sint32)damage >> 1); break;
            case 4: damage = (uint32)((sint32)damage >> 2); break;
            case 10: damage <<= 1; break;
            case 11: damage <<= 2; break;
            case 12: damage = 0x7FFFFFFF; break;
            }
        }
    }
    if ((sint32)target_index < 0)
    {
        other[0] = (sint16)r_u16(target + 0x25A);
        other[1] = (sint16)r_u16(target + 0x25E);
        other[2] = (sint16)r_u16(target + 0x262);
        origin[0] = (sint16)r_u16(record + 0x20);
        origin[1] = (sint16)r_u16(record + 0x24);
        origin[2] = (sint16)r_u16(record + 0x28);
        /* TODO: Addressable vectors require permitted local adapters */
        origin_view = moh_draft_local_address(origin, sizeof(origin));
        other_view = moh_draft_local_address(other, sizeof(other));
        side = level2p_sub_8004CB60(owner, origin_view, other_view);
        w_u32(owner + 0x38, r_u32(owner + 0x38) | (side ? 0x40 : 0x20));
        level2p_sub_8004B60C(owner, damage, target);
        return 1;
    }
    if (r_u32(target_record + 8) == 8 || r_u32(record + 8) == 8)
    {
        first_kind = r_u32(owner + 8);
        second_kind = r_u32(target + 8);
        if (first_kind != 0x1B && second_kind != 0x1B) return 0;
    }
    other[0] = (sint16)r_u16(target_record + 0x20);
    first_kind = r_u32(owner + 0x210);
    second_kind = r_u32(target + 0x210);
    other[1] = (sint16)r_u16(target_record + 0x24);
    other[2] = (sint16)r_u16(target_record + 0x28);
    origin[0] = (sint16)r_u16(record + 0x20);
    origin[1] = (sint16)r_u16(record + 0x24);
    origin[2] = (sint16)r_u16(record + 0x28);
    height = (sint32)(first_kind - second_kind) >> 16;
    origin_view = moh_draft_local_address(origin, sizeof(origin));
    other_view = moh_draft_local_address(other, sizeof(other));
    side = level2p_sub_8004CB60(owner, origin_view, other_view);
    kind = r_u32(target_record + 8);
    if (kind == 9 || kind == 5 || kind == 6)
    {
        w_u32(target + 0x38, r_u32(target + 0x38) | 0x200000);
        kind = r_u32(target_record + 8);
        bits = side ? 0x40 : 0x20;
        if (kind == 6) bits |= 0x100000;
        else if (kind == 9) bits |= 0x80000;
        if (kind == 5 || kind == 6 || kind == 9)
            w_u32(owner + 0x38, r_u32(owner + 0x38) | bits);
        level2p_sub_8004B21C(target);
    }
    else
    {
        kind = r_u32(record + 8);
        if (kind != 9 && kind != 5 && kind != 6)
            w_u32(owner + 0x38, r_u32(owner + 0x38) | (side ? 0x400 : 0x200));
    }
    if (height > 0)
    {
        kind = r_u32(target_record + 8);
        if (kind == 9 || kind == 5 || kind == 6)
            w_u32(owner + 0x38, r_u32(owner + 0x38) | 0x80);
    }
    if (height < 0)
    {
        kind = r_u32(target_record + 8);
        if (kind == 9 || kind == 5 || kind == 6)
            w_u32(owner + 0x38, r_u32(owner + 0x38) | 0x100);
    }
    if (damage)
        level2p_sub_8004B60C(owner, damage, target);
    return 1;
}


void level2p_sub_8004B21C(uint32 target)
{
    uint32 index = 0, offset = 0, kind, count;
    FUNCTION_MARKER(0x8004B21C, "LEVEL2P.BIN");
    if (!(r_u32(target + 0x10) & 0x20000)) return;
    if (!r_u16(target + 0x36)) return;
    do
    {
        kind = r_u32(r_u32(target + 0x304) + offset + 8);
        if (kind == 9 || kind == 5 || kind == 6)
            w_u32(r_u32(target + 0x304) + offset + 8, 8);
        count = r_u16(target + 0x36);
        ++index;
        offset += 72;
    } while ((sint32)index < (sint32)count);
}


void level2p_sub_8004B60C(uint32 owner, uint32 damage, uint32 target)
{
    uint32 player = 0, data, flags, health, score_player, change, award;
    FUNCTION_MARKER(0x8004B60C, "LEVEL2P.BIN");
    if (r_s32(owner + 0x48) > 0) return;
    /* TODO: Original reads target flags before its later null-target guards */
    if (r_u32(target + 0xC) & 1) player = target;
    else
    {
        flags = r_u32(target + 0x10);
        if (flags & 0x1000) player = r_u32(0x8009943C);
        else if (flags & 0x2000) player = r_u32(0x80099440);
    }
    if (player)
    {
        data = r_u32(player + 0x388);
        flags = r_u32(data + 0x72C);
        if (flags & 0x40) damage <<= r_u16(data + 0x764) & 31;
        if (flags & 0x80) damage = (uint32)((sint32)damage >> (r_u16(data + 0x766) & 31));
    }
    health = r_u32(owner + 0xF4) - damage;
    w_u32(owner + 0xF4, health);
    if ((sint32)health <= 0)
    {
        if (!(r_u32(owner + 0xC) & 0x20000000))
        {
            if (target)
            {
                flags = r_u32(target + 0x10);
                score_player = 0;
                change = 1;
                award = 0;
                if (flags & 0x1000)
                {
                    score_player = r_u32(0x8009943C);
                    if (owner == score_player) { score_player = owner; change = 0xFFFFFFFF; award = 1; }
                    else if (owner == r_u32(0x80099440)) award = 1;
                }
                else if (flags & 0x2000)
                {
                    if (owner == r_u32(0x8009943C)) { score_player = r_u32(0x80099440); award = 1; }
                    else if (owner == r_u32(0x80099440)) { score_player = owner; change = 0xFFFFFFFF; award = 1; }
                }
                if (award) level2p_sub_8007D310(score_player, change);
            }
            w_u32(owner + 0x38, r_u32(owner + 0x38) | 0x4000);
            w_u32(owner + 0xC, r_u32(owner + 0xC) | 0x20000000);
        }
        w_u32(owner + 0xF4, 0);
    }
    if (!target || !(r_u32(target + 0xC) & 0x10000000)) return;
    if (r_u32(target + 0x10) & 0x1000)
    {
        player = r_u32(0x8009943C);
        if (owner != player) { level2p_sub_8007D3EC(player); return; }
    }
    if (r_u32(target + 0x10) & 0x2000)
    {
        player = r_u32(0x80099440);
        if (owner != player) level2p_sub_8007D3EC(player);
    }
}

void level2p_sub_8007D310(uint32 player, uint32 change)
{
    uint32 data, clock, deadline, points, limit;
    FUNCTION_MARKER(0x8007D310, "LEVEL2P.BIN");
    if (r_u32(0x800968CC)) return;
    data = r_u32(player + 0x388);
    if (!r_s16(data + 0x716))
    {
        clock = r_u32(0x800968E4);
        deadline = r_u32(data + 0x8E0);
        if ((sint32)(clock - deadline) < 0 && (sint32)change > 0)
        {
            w_u32(data + 0x8E0, clock);
            goto score_limit;
        }
    }
    points = r_u16(data + 0x716) + change;
    w_u16(data + 0x716, points);
    if ((sint32)change > 0) w_u32(data, r_u32(data) | 0x8000000);
    else if ((sint16)points < 0)
    {
        clock = r_u32(0x800968E4);
        w_u16(data + 0x716, 0);
        w_u32(data + 0x8E0, clock + 150);
    }
score_limit:
    limit = r_u32(0x800968C8);
    if (limit != 0xFFFFFFFF && (uint32)(sint32)r_s16(data + 0x716) >= limit)
        level2p_sub_8005F760();
}

void level2p_sub_8007D3EC(uint32 player)
{
    uint32 data, value;
    FUNCTION_MARKER(0x8007D3EC, "LEVEL2P.BIN");
    data = r_u32(player + 0x388);
    value = r_u32(data + 0x71C);
    w_u32(data + 0x71C, value + 1);
}

void level2p_sub_8005F760(void)
{
    uint32 first, second, first_data, second_data, name, mask;
    FUNCTION_MARKER(0x8005F760, "LEVEL2P.BIN");
    if (r_u32(0x800968CC)) return;
    first = r_u32(0x8009943C);
    second = r_u32(0x80099440);
    first_data = r_u32(first + 0x388);
    second_data = r_u32(second + 0x388);
    level2p_sub_8007D0A4(first, first_data);
    level2p_sub_8007D0A4(r_u32(0x80099440), second_data);
    name = level2p_sub_80087824(r_u8(first_data + 0x726));
    /* TODO: Missing SDK formatting remains a permitted fail-fast boundary */
    moh_draft_missing_call(0x8001BE18, 0x80098FB4, 0x8003C49C, name, (uint32)(sint32)r_s16(first_data + 0x716));
    mask = (uint32)1 << (r_u8(first_data + 0x726) & 31);
    level2p_sub_8005ABFC(0x80098FB4, 0xAD, 0x28, 0x80, 0x80, 0x20, 0, 0xFFFFFFFF, 0xFF, mask, 0);
    moh_draft_missing_call(0x8001BE18, 0x80099004, 0x8003C4A8);
    mask = (uint32)1 << (r_u8(first_data + 0x726) & 31);
    level2p_sub_8005ABFC(0x80099004, 0x87, 0x50, 0x80, 0x80, 0x20, 0, 0xFFFFFFFF, 0xFF, mask, 0);
    name = level2p_sub_80087824(r_u8(second_data + 0x726));
    moh_draft_missing_call(0x8001BE18, 0x80098FDC, 0x8003C49C, name, (uint32)(sint32)r_s16(second_data + 0x716));
    mask = (uint32)1 << (r_u8(second_data + 0x726) & 31);
    level2p_sub_8005ABFC(0x80098FDC, 0xAD, 0x28, 0x80, 0x80, 0x20, 0, 0xFFFFFFFF, 0xFF, mask, 0);
    moh_draft_missing_call(0x8001BE18, 0x8009902C, 0x8003C4A8);
    mask = (uint32)1 << (r_u8(second_data + 0x726) & 31);
    level2p_sub_8005ABFC(0x8009902C, 0x87, 0x50, 0x80, 0x80, 0x20, 0, 0xFFFFFFFF, 0xFF, mask, 0);
    level2p_sub_8007D5E4();
    w_u32(0x800968CC, 1);
}


void level2p_sub_8007D0A4(uint32 player, uint32 data)
{
    FUNCTION_MARKER(0x8007D0A4, "LEVEL2P.BIN");
    level2p_sub_8005AF7C((r_u8(data + 0x726) + 3) & 0xFF);
    w_u32(data + 0x7D0, 0);
}

uint32 level2p_sub_80087824(uint32 index)
{
    uint32 selected;
    FUNCTION_MARKER(0x80087824, "LEVEL2P.BIN");
    selected = r_u32((index & 0xFF) ? 0x800B60EC : 0x800B60E8);
    return r_u32(0x8009B708 + selected * 4);
}

void level2p_sub_8007D5E4(void)
{
    uint32 first, second, first_data, second_data, winner, other;
    sint32 first_score, second_score;
    FUNCTION_MARKER(0x8007D5E4, "LEVEL2P.BIN");
    first = r_u32(0x8009943C);
    second = r_u32(0x80099440);
    first_data = r_u32(first + 0x388);
    second_data = r_u32(second + 0x388);
    first_score = r_s16(first_data + 0x716);
    second_score = r_s16(second_data + 0x716);
    if (second_score < first_score) { winner = 4; other = 5; }
    else if (first_score < second_score) { winner = 5; other = 4; }
    else return;
    level2p_sub_8004285C(0x19, winner, 5, 0, 0x10000, 0, 0x10000, 0, 0, 0, 0, 0);
    level2p_sub_8004285C(0, 0, 0x37, 0, 0x10000, 0, 0x10000, 0, 0, 0, 0, 0);
    level2p_sub_8004285C(0, other, 5, 0, 0x10000, 0, 0x10000, 0, 0, 0, 0, 0);
}

void level2p_sub_8005AF7C(uint32 tag)
{
    uint32 index = 0, record = 0x80096214;
    FUNCTION_MARKER(0x8005AF7C, "LEVEL2P.BIN");
    if (r_s32(0x80096198) <= 0) return;
    tag &= 0xFF;
    do
    {
        if (r_u8(record + 0x1F) == tag) level2p_sub_8005AF10(index);
        else { record += 32; ++index; }
    } while ((sint32)index < r_s32(0x80096198));
}

void level2p_sub_8005AF10(uint32 index)
{
    uint32 count, source, destination, first, second, third, fourth;
    FUNCTION_MARKER(0x8005AF10, "LEVEL2P.BIN");
    count = r_u32(0x80096198) - 1;
    w_u32(0x80096198, count);
    destination = 0x80096214 + index * 32;
    source = 0x80096214 + count * 32;
    first = r_u32(source);
    second = r_u32(source + 4);
    third = r_u32(source + 8);
    fourth = r_u32(source + 12);
    w_u32(destination, first);
    w_u32(destination + 4, second);
    w_u32(destination + 8, third);
    w_u32(destination + 12, fourth);
    first = r_u32(source + 16);
    second = r_u32(source + 20);
    third = r_u32(source + 24);
    fourth = r_u32(source + 28);
    w_u32(destination + 16, first);
    w_u32(destination + 20, second);
    w_u32(destination + 24, third);
    w_u32(destination + 28, fourth);
}


void level2p_sub_800444C8(uint32 owner)
{
    uint32 flags, data, selected, first, second, third, value, random, timer, byte_timer, player;
    sint32 distance, speed;
    FUNCTION_MARKER(0x800444C8, "LEVEL2P.BIN");
    flags = r_u32(owner + 0x3C);
    data = r_u32(owner + 0x388);
    if (flags & 1)
    {
        selected = level2p_sub_800552C8(owner, 0x50, 0xFFFFFFFF);
        if (!selected) goto failed_selection;
        first = r_u32(selected + 0x228);
        second = r_u32(selected + 0x22C);
        third = r_u32(selected + 0x230);
        w_u32(owner + 0x228, first);
        w_u32(owner + 0x22C, second);
        w_u32(owner + 0x230, third);
        value = r_u32(owner + 0xC);
        w_u32(owner + 0x200, 0x190000);
        w_u32(owner + 0x21C, 0xFFDF0000);
        w_u32(owner + 0x284, 0);
        w_u32(owner + 0xC, value | 0x40000A0);
        random = sub_8001552C();
        w_u32(data + 0x68, (uint32)((sint32)(random << 12) >> 15) << 16);
        w_u32(data + 0x6C, 0);
        w_u32(data + 0x70, 0);
        random = sub_8001552C();
        w_u32(data + 0x74, ((uint32)((sint32)(random * 650) >> 15) + 200) << 16);
        random = sub_8001552C();
        value = ((uint32)((sint32)(random * 30) >> 15) + 10) << 16;
        w_u16(data + 0x38, 0x348);
        timer = r_u16(data + 0x3A);
        w_u32(data + 0x78, value);
        byte_timer = r_u8(data + 0x41);
        w_u32(data + 0x7C, 0);
        w_u16(data + 0x3A, timer + 8);
        w_u8(data + 0x41, byte_timer + 8);
        return;
    }
    if (flags & 8)
    {
        level2p_sub_80068A04(owner + 0x30, 0);
        value = r_u32(owner + 0xC);
        w_u32(owner + 0x284, 0);
        w_u32(owner + 0xC, value | 0x40000A0);
        w_u16(data + 0x38, 0x348);
        return;
    }
    if (flags & 2)
    {
        selected = level2p_sub_800552C8(owner, 0x50, 0xFFFFFFFF);
        if (!selected) goto failed_selection;
        level2p_sub_80068A04(owner + 0x30, selected);
        w_u8(owner + 0x34, 0x11);
        value = r_u16(data + 0x38);
        w_u16(data + 0x3A, 120);
        w_u8(data + 0x41, 90);
        w_u32(data + 0x68, 0);
        w_u32(data + 0x6C, 0);
        w_u32(data + 0x70, 0);
        w_u32(data + 0x74, 0);
        w_u32(data + 0x78, 0);
        w_u32(data + 0x7C, 0);
        w_u16(data + 0x38, value & 0xFDF6);
        return;
    }
    if (!(flags & 4)) return;
    selected = level2p_sub_800552C8(owner, 0x50, 0xFFFFFFFF);
    if (!selected)
    {
        w_u32(owner + 0xC, r_u32(owner + 0xC) | 0x40000A0);
        w_u16(data + 0x38, 0x348);
        level2p_sub_80068A04(owner + 0x30, 0);
        return;
    }
    level2p_sub_80068A04(owner + 0x30, 0);
    player = r_u32(0x8009943C);
    first = r_u32(selected + 0x228);
    second = r_u32(selected + 0x22C);
    third = r_u32(selected + 0x230);
    w_u32(owner + 0x228, first);
    w_u32(owner + 0x22C, second);
    w_u32(owner + 0x230, third);
    value = level2p_sub_800517E8(owner, player) - 700;
    distance = (sint32)value < 0 ? 0 : (sint32)value;
    w_u32(owner + 0x21C, 0xFFE70000);
    speed = (distance >> 6) + 30;
    if (distance >= 2001)
    {
        speed = (distance >> 6) + 35;
        w_u32(owner + 0x21C, 0xFFE20000);
    }
    else if (distance >= 1301) speed = (distance >> 6) + 35;
    if (speed >= 66) speed = 65;
    value = r_u32(owner + 0xC);
    w_u32(owner + 0x200, (uint32)speed << 16);
    w_u32(owner + 0x284, 0);
    w_u32(owner + 0xC, value | 0x40000A0);
    random = sub_8001552C();
    w_u32(data + 0x74, ((uint32)((sint32)(random * 650) >> 15) + 200) << 16);
    random = sub_8001552C();
    value = ((uint32)((sint32)(random * 30) >> 15) + 10) << 16;
    w_u16(data + 0x3A, 120);
    w_u8(data + 0x41, 90);
    w_u32(data + 0x78, value);
    w_u16(data + 0x38, 0x348);
    return;
failed_selection:
    w_u32(owner + 0xC, r_u32(owner + 0xC) | 0x40000A0);
    w_u16(data + 0x38, 0x348);
}

uint32 level2p_sub_800517E8(uint32 owner, uint32 target)
{
    uint32 x, owner_x, z, owner_z, smaller;
    FUNCTION_MARKER(0x800517E8, "LEVEL2P.BIN");
    x = r_u32(target + 0x20C);
    owner_x = r_u32(owner + 0x20C);
    z = r_u32(target + 0x214);
    x -= owner_x;
    owner_z = r_u32(owner + 0x214);
    if ((sint32)x < 0) x = 0 - x;
    z -= owner_z;
    if ((sint32)z < 0) z = 0 - z;
    smaller = (sint32)z < (sint32)x ? z : x;
    return (uint32)((sint32)x >> 16) + (uint32)((sint32)z >> 16) - (uint32)((sint32)smaller >> 17);
}

uint32 level2p_sub_800552C8(uint32 owner, uint32 first_mask, uint32 second_mask)
{
    sint16 vector[3];
    sint32 height;
    uint32 current, selected = 0, best = 0x7FFF, first, second, vector_view, distance;
    FUNCTION_MARKER(0x800552C8, "LEVEL2P.BIN");
    current = r_u32(0x80099434);
    while (current)
    {
        first = first_mask == 0xFFFFFFFF ? 1 : r_u32(current + 0xC) & first_mask;
        second = second_mask == 0xFFFFFFFF ? 1 : r_u32(current + 0x10) & second_mask;
        if (first && second && current != owner && !(r_u32(current + 0xC) & 0x20080000))
        {
            first = r_u32(owner + 0x20C); second = r_u32(current + 0x20C);
            vector[0] = (sint16)((first - second) >> 16);
            first = r_u32(owner + 0x210); second = r_u32(current + 0x210);
            vector[1] = (sint16)((first - second) >> 16);
            first = r_u32(owner + 0x214); second = r_u32(current + 0x214);
            vector[2] = (sint16)((first - second) >> 16);
            /* TODO: Addressable vector requires the permitted local adapter */
            vector_view = moh_draft_local_address(vector, sizeof(vector));
            distance = level2p_sub_800548C4(vector_view);
            if ((distance & 0xFFFF) < (best & 0xFFFF))
            {
                height = vector[1];
                if (height < 0) height = -height;
                if (height < 1000) { selected = current; best = distance; }
            }
        }
        current = r_u32(current + 4);
    }
    return selected;
}

uint32 level2p_sub_800548C4(uint32 vector)
{
    uint32 first, second, third, temporary;
    FUNCTION_MARKER(0x800548C4, "LEVEL2P.BIN");
    first = level2p_sub_800548B0((uint32)(sint32)r_s16(vector));
    second = level2p_sub_800548B0((uint32)(sint32)r_s16(vector + 2));
    third = level2p_sub_800548B0((uint32)(sint32)r_s16(vector + 4));
    if ((sint32)first < (sint32)second) { temporary = first; first = second; second = temporary; }
    if ((sint32)first < (sint32)third) { temporary = first; first = third; third = temporary; }
    return first + (uint32)((sint32)(second + third) >> 2);
}

uint32 level2p_sub_800548B0(uint32 value)
{
    FUNCTION_MARKER(0x800548B0, "LEVEL2P.BIN");
    return (sint32)value < 0 ? 0 - value : value;
}


void level2p_sub_80044818(uint32 owner)
{
    uint8 output[8] = { 0 };
    uint32 data, timer, value, callback, handle, related, output_view, first, second;
    uint32 third, fourth, flags, random, speed, channel, x, y, z;
    FUNCTION_MARKER(0x80044818, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388);
    timer = (r_u16(data + 0x3A) - 1) & 0xFFFF;
    w_u16(data + 0x3A, timer);
    if (!timer && r_u32(owner + 8)) { level2p_sub_80066C64(owner); return; }
    timer = r_u8(data + 0x40);
    if (timer)
    {
        timer = (timer - 1) & 0xFF;
        w_u8(data + 0x40, timer);
        if (!timer) w_u32(owner + 0x14, r_u32(data + 0x3C));
    }
    level2p_sub_8004D8D4(owner);
    if (r_u32(owner + 8) == 0x13F3) level2p_sub_800444C8(owner);
    if (r_u16(data + 0x38) & 0x40)
    {
        timer = r_u8(data + 0x41);
        if (timer)
        {
            timer = (timer - 1) & 0xFF;
            w_u8(data + 0x41, timer);
            if (!timer)
            {
                callback = r_u32(data + 0x44);
                if (!callback)
                {
                    if (r_u16(data + 0x38) & 0x100)
                    {
                        w_u16(owner + 0x368, 0);
                        w_u16(owner + 0x366, 0);
                        w_u16(owner + 0x364, 0);
                        level2p_sub_8006CD4C(owner, 8, 10, 0x55, 0x10000, 0x1000, 0x40000, 5, 0);
                        random = sub_8001552C();
                        value = (uint32)((sint32)((random << 2) + random) >> 15) + 15;
                        level2p_sub_8006DF10(owner + 0x20C, 5, value, 40, 25, 80, 30,
                            0x3FF, 0x3F, 0x3E0, 0xFF, 1, 0x10000, 0, 0);
                        level2p_sub_800431A0(owner);
                    }
                    level2p_sub_80066C64(owner);
                    return;
                }
                moh_draft_indirect_call(callback, owner);
                if (r_u32(owner + 0xC) & 0x80000) return;
            }
        }
    }
    handle = r_u32(owner + 0x30);
    related = handle ? r_u32(0x80099460 + (handle & 0xFFFF) * 4) : 0;
    if (related && (sint8)r_u8(owner + 0x34) >= 0)
    {
        handle = r_u32(owner + 0x30);
        related = handle ? r_u32(0x80099460 + (handle & 0xFFFF) * 4) : 0;
        value = (uint32)(sint32)(sint8)r_u8(owner + 0x34);
        /* TODO: Addressable output requires the permitted local adapter */
        output_view = moh_draft_local_address(output, sizeof(output));
        level2p_sub_8008C5E4(related, value, output_view, 0, 0);
        w_u32(owner + 0x23C, 0);
        w_u32(owner + 0x238, 0);
        w_u32(owner + 0x234, 0);
        w_u32(owner + 0x220, 0);
        w_u32(owner + 0x21C, 0);
        w_u32(owner + 0x218, 0);
        w_u32(owner + 0x200, 0);
        w_u16(data + 0x38, r_u16(data + 0x38) & 0xFDF6);
        return;
    }
    first = r_u32(data + 0x68); second = r_u32(data + 0x74);
    third = r_u32(data + 0x6C); fourth = r_u32(data + 0x78);
    w_u32(data + 0x68, first + second);
    first = r_u32(data + 0x70);
    w_u32(data + 0x6C, third + fourth);
    second = r_u32(data + 0x7C);
    flags = r_u16(data + 0x38);
    w_u32(data + 0x70, first + second);
    if (flags & 0x400)
    {
        if (r_u32(owner + 0x284))
        {
            if (flags & 2)
            {
                random = sub_8001552C();
                level2p_sub_800420C4(1, (uint32)((sint32)(random << 1) >> 15) + 7,
                    owner, 0x40000, 0, 0x10000, 0x40AC, 0, 0, 0, 0);
            }
            value = r_u32(owner + 0x228);
            if (value) w_u32(owner + 0x228, (uint32)((sint32)value >> 1));
            value = r_u32(data + 0x78);
            if (value) w_u32(data + 0x78, (uint32)((sint32)value >> 1));
            w_u32(data + 0x70, 0x4000000);
            speed = r_u32(owner + 0x200);
            speed -= (sint32)speed >= 0x190000 ? 0x30000 : 0x10000;
            w_u32(owner + 0x200, speed);
            if ((sint32)speed >= 0) goto apply_acceleration;
            w_u32(data + 0x70, 0x4000000);
            w_u32(owner + 0x220, 0);
            w_u32(owner + 0x218, 0);
            w_u32(owner + 0x23C, 0);
            w_u32(owner + 0x238, 0);
            w_u32(owner + 0x234, 0);
            w_u32(owner + 0x200, 0);
            flags = r_u16(data + 0x38);
            channel = r_u32(data + 0x30);
            w_u32(data + 0x7C, 0);
            w_u32(data + 0x78, 0);
            w_u32(data + 0x74, 0);
            w_u32(data + 0x68, 0);
            w_u16(data + 0x38, flags & 0xFBFF);
        }
        else channel = r_u32(data + 0x30);
        if ((sint32)channel > 0)
        {
            level2p_sub_80042714(channel);
            w_u32(data + 0x30, 0);
        }
    }
apply_acceleration:
    flags = r_u16(data + 0x38);
    if ((flags & 1) && !(flags & 2))
    {
        first = r_u32(owner + 0x21C);
        second = r_u32(data + 0x34);
        w_u32(owner + 0x21C, first + second);
    }
    if (((flags & 1) && (flags & 2)) || (r_u16(data + 0x38) & 2))
        w_u16(data + 0x38, r_u16(data + 0x38) & 0xFFFD);
    if (r_u16(data + 0x38) & 0x10)
    {
        first = r_u32(data);
        x = r_u32(owner + 0x20C);
        y = r_u32(owner + 0x210);
        z = r_u32(owner + 0x214);
        level2p_sub_8006A9C4(first, x, y, z);
        if (!level2p_sub_80069CD8(r_u32(data))) level2p_sub_80066C64(owner);
    }
    w_u32(data + 0x80, r_u32(owner + 0x200));
}


void level2p_sub_80044CD4(uint32 owner, uint32 context)
{
    uint32 data, ordering, matrix, first, second, third, x, y, z, index, packet, flags, bucket, tag, head;
    FUNCTION_MARKER(0x80044CD4, "LEVEL2P.BIN");
    matrix = context + 0x21E4;
    data = r_u32(owner + 0x388);
    ordering = r_u32(context + 0x2218);
    first = r_u32(matrix); second = r_u32(matrix + 4);
    xport_gte_write_control(0, first); xport_gte_write_control(1, second);
    first = r_u32(matrix + 8); second = r_u32(matrix + 12); third = r_u32(matrix + 16);
    xport_gte_write_control(2, first); xport_gte_write_control(3, second); xport_gte_write_control(4, third);
    first = r_u32(matrix + 20); second = r_u32(matrix + 24);
    xport_gte_write_control(5, first);
    third = r_u32(matrix + 28);
    xport_gte_write_control(6, second); xport_gte_write_control(7, third);
    x = r_u16(owner + 0x20E);
    index = r_u32(context + 0x221C);
    y = r_u16(owner + 0x212);
    z = r_u16(owner + 0x216);
    packet = data + index * 20 + 4;
    xport_gte_write_data(0, x | (y << 16));
    /* Original unspecified upper half of VZ0 is discarded by the data register */
    xport_gte_write_data(1, (uint32)(sint32)(sint16)z);
    xport_gte_execute(0x180001);
    w_u32(0x1F800190, xport_gte_read_control(31));
    if (r_s32(0x1F800190) < 0) return;
    w_u32(packet + 8, xport_gte_read_data(14));
    w_u32(0x1F800198, xport_gte_read_data(19));
    first = r_u32(0x1F800198);
    if ((sint32)first < 0 || ((sint32)first >> 4) >= 512) return;
    flags = r_u16(data + 0x38);
    if (flags & 0x2000)
    {
        x = r_u16(data + 0x86);
        w_u16(data + 0x38, flags & 0xDFFF);
        y = r_u16(data + 0x8A);
        z = r_u16(data + 0x8E);
    }
    else
    {
        x = r_u16(owner + 0x25A);
        y = r_u16(owner + 0x25E);
        z = r_u16(owner + 0x262);
    }
    xport_gte_write_data(0, x | (y << 16));
    xport_gte_write_data(1, (uint32)(sint32)(sint16)z);
    xport_gte_execute(0x180001);
    w_u32(0x1F800190, xport_gte_read_control(31));
    if (r_s32(0x1F800190) < 0) return;
    w_u32(packet + 16, xport_gte_read_data(14));
    first = r_u32(0x1F800198);
    tag = r_u32(packet);
    bucket = ordering + (uint32)((sint32)first >> 4) * 4;
    head = r_u32(bucket);
    w_u32(packet, (tag & 0xFF000000) | (head & 0xFFFFFF));
    bucket = ordering + (uint32)(r_s32(0x1F800198) >> 4) * 4;
    head = r_u32(bucket);
    w_u32(bucket, (head & 0xFF000000) | (packet & 0xFFFFFF));
}


void level2p_sub_80044EB4(uint32 owner, uint32 context)
{
    uint8 saved[32] = { 0 };
    uint32 data, kind, saved_view, matrix;
    FUNCTION_MARKER(0x80044EB4, "LEVEL2P.BIN");
    kind = r_u32(owner + 8);
    data = r_u32(owner + 0x388);
    if (kind == 0x13F1) level2p_sub_800466FC(owner, context);
    if (r_u16(data + 0x38) & 0x80)
    {
        if (r_u16(data + 0x3A) & 1) level2p_sub_80044CD4(owner, context);
        return;
    }
    if (r_u32(owner + 8) != 0x13F3) { level2p_sub_80065698(owner, context); return; }
    /* TODO: Addressable matrix backup and SDK copy retain permitted boundaries */
    saved_view = moh_draft_local_address(saved, sizeof(saved));
    matrix = owner + 0x340;
    moh_draft_missing_call(0x800154EC, saved_view, matrix, 32);
    level2p_sub_8008BB44(owner + 0x20C, data + 0x68, owner + 0x270, matrix);
    level2p_sub_80065698(owner, context);
    moh_draft_missing_call(0x800154EC, matrix, saved_view, 32);
}

void level2p_sub_80044FA4(uint32 owner)
{
    uint32 data, handle, related, current, owner_view, channel;
    FUNCTION_MARKER(0x80044FA4, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388);
    if (r_u32(data + 0x2C)) return;
    handle = r_u32(owner + 0x30);
    related = handle ? r_u32(0x80099460 + (handle & 0xFFFF) * 4) : 0;
    handle = r_u32(owner + 0x30);
    current = handle ? r_u32(0x80099460 + (handle & 0xFFFF) * 4) : 0;
    if (current)
    {
        /* TODO: Addressable owner word requires the permitted local adapter */
        owner_view = moh_draft_local_address(&owner, sizeof(owner));
        if (level2p_sub_80068988(related + 0x30, owner_view)) level2p_sub_80068A04(related + 0x30, 0);
    }
    channel = r_u32(data + 0x30);
    w_u32(data + 0x2C, 1);
    if ((sint32)channel > 0) { level2p_sub_80042714(channel); w_u32(data + 0x30, 0); }
    if (r_u16(data + 0x38) & 0x80) level2p_sub_800653D8(owner);
    else level2p_sub_8006543C(owner);
    if (r_u32(0x80099438) == owner) w_u32(0x80099438, 0);
}

void level2p_sub_800450D8(uint32 origin, uint32 target, uint32 output, uint32 height)
{
    uint32 first, second, x, z, y, angle, third, length;
    FUNCTION_MARKER(0x800450D8, "LEVEL2P.BIN");
    first = r_u32(target + 8); second = r_u32(origin + 8);
    z = (uint32)((sint32)(first - second) >> 16);
    first = r_u32(target); second = r_u32(origin);
    x = (uint32)((sint32)(first - second) >> 16);
    angle = moh_draft_missing_call(0x8001536C, x, z);
    w_u32(output + 4, angle << 16);
    first = r_u32(target); second = r_u32(origin);
    x = (uint32)((sint32)(first - second) >> 16);
    xport_gte_write_data(9, x);
    xport_gte_write_data(10, x);
    xport_gte_write_data(11, z);
    xport_gte_execute(0xA00428);
    first = r_u32(target + 4); second = r_u32(origin + 4);
    y = (uint32)((sint32)(first - second - (uint32)((sint32)height >> 1)) >> 16);
    first = xport_gte_read_data(25);
    xport_gte_read_data(26);
    third = xport_gte_read_data(27);
    length = moh_draft_missing_call(0x80016188, first + third);
    angle = moh_draft_missing_call(0x8001536C, y, length);
    w_u32(output, 0 - (angle << 16));
}

void level2p_sub_800451CC(uint32 owner, uint32 output, uint32 first_bone, uint32 second_bone, uint32 model, uint32 first_parameter, uint32 second_parameter, uint32 first_output, uint32 second_output)
{
    FUNCTION_MARKER(0x800451CC, "LEVEL2P.BIN");
    if (model)
    {
        level2p_sub_8008C61C(model, second_bone, second_output);
        level2p_sub_8008C61C(model, first_bone, first_output);
    }
    else
    {
        level2p_sub_8008C5E4(owner, second_bone, second_output, second_parameter, 0);
        level2p_sub_8008C5E4(owner, first_bone, first_output, first_parameter, 0);
    }
    w_u32(output, (uint32)(sint32)r_s16(second_output) << 16);
    w_u32(output + 4, (uint32)(sint32)r_s16(second_output + 2) << 16);
    w_u32(output + 8, (uint32)(sint32)r_s16(second_output + 4) << 16);
}

void level2p_sub_800452B0(uint32 owner, uint32 output)
{
    uint32 first, second, third;
    FUNCTION_MARKER(0x800452B0, "LEVEL2P.BIN");
    first = r_u32(owner + 0x228);
    second = r_u32(owner + 0x22C);
    third = r_u32(owner + 0x230);
    w_u32(output, first); w_u32(output + 4, second); w_u32(output + 8, third);
}

void level2p_sub_800466FC(uint32 owner, uint32 context)
{
    uint32 matrix, data, ordering, first, second, third, index, record, packet;
    uint32 x, y, z, screen, depth, numerator, divisor, quotient, factor, size, color, bucket, tag, head;
    FUNCTION_MARKER(0x800466FC, "LEVEL2P.BIN");
    matrix = context + 0x21E4;
    data = r_u32(owner + 0x388);
    ordering = r_u32(context + 0x2218);
    data = r_u32(data);
    first = r_u32(matrix); second = r_u32(matrix + 4);
    xport_gte_write_control(0, first); xport_gte_write_control(1, second);
    first = r_u32(matrix + 8); second = r_u32(matrix + 12); third = r_u32(matrix + 16);
    xport_gte_write_control(2, first); xport_gte_write_control(3, second); xport_gte_write_control(4, third);
    first = r_u32(matrix + 20); second = r_u32(matrix + 24);
    xport_gte_write_control(5, first);
    third = r_u32(matrix + 28);
    xport_gte_write_control(6, second); xport_gte_write_control(7, third);
    for (index = 0; index < 20; ++index)
    {
        record = data + 0x3C + index * 56;
        if (!(r_u32(record) & 1)) continue;
        x = r_u16(record + 0x12);
        first = r_u32(context + 0x221C);
        y = r_u16(record + 0x16); z = r_u16(record + 0x1A);
        packet = data + 0x508 + index * 80 + first * 40;
        xport_gte_write_data(0, x | (y << 16));
        /* Original unspecified VZ0 upper half is discarded by the data register */
        xport_gte_write_data(1, (uint32)(sint32)(sint16)z);
        xport_gte_execute(0x180001);
        w_u32(0x1F800190, xport_gte_read_control(31));
        if (r_s32(0x1F800190) < 0) continue;
        screen = xport_gte_read_data(14);
        w_u32(0x1F800198, xport_gte_read_data(19));
        depth = r_u32(0x1F800198);
        if ((sint32)depth < 0 || ((sint32)depth >> 4) >= 512) continue;
        numerator = depth << 2;
        divisor = r_u32(context + 8);
        if (!divisor)
        {
            moh_draft_fail_fast("Original particle size division BREAK 0x1C00", 0x80046828);
            return;
        }
        if (divisor == 0xFFFFFFFF && numerator == 0x80000000)
        {
            moh_draft_fail_fast("Original particle size division BREAK 0x1800", 0x80046840);
            return;
        }
        quotient = (uint32)((sint32)numerator / (sint32)divisor);
        first = r_u32(record + 0x28);
        factor = (uint32)((sint32)((64 - quotient) << 16) >> 5);
        size = (uint32)((sint32)((10 - first) * factor) >> 16);
        x = screen & 0xFFFF; y = screen >> 16;
        w_u16(packet + 8, x - size); w_u16(packet + 10, y - size);
        w_u16(packet + 16, x + size); w_u16(packet + 18, y - size);
        w_u16(packet + 24, x - size); w_u16(packet + 26, y);
        w_u16(packet + 32, x + size); w_u16(packet + 34, y);
        color = (r_u32(record + 0x28) << 1) + 10;
        w_u8(packet + 4, color); w_u8(packet + 5, color); w_u8(packet + 6, color);
        first = r_u32(0x1F800198);
        tag = r_u32(packet);
        bucket = ordering + (uint32)((sint32)first >> 4) * 4;
        head = r_u32(bucket);
        w_u32(packet, (tag & 0xFF000000) | (head & 0xFFFFFF));
        bucket = ordering + (uint32)(r_s32(0x1F800198) >> 4) * 4;
        head = r_u32(bucket);
        w_u32(bucket, (head & 0xFF000000) | (packet & 0xFFFFFF));
    }
}


static void moh_bone_install_controls(uint32 matrix)
{
    uint32 first, second, third;
    first = r_u32(matrix); second = r_u32(matrix + 4);
    xport_gte_write_control(0, first); xport_gte_write_control(1, second);
    first = r_u32(matrix + 8); second = r_u32(matrix + 12); third = r_u32(matrix + 16);
    xport_gte_write_control(2, first); xport_gte_write_control(3, second); xport_gte_write_control(4, third);
    first = r_u32(matrix + 20); second = r_u32(matrix + 24);
    xport_gte_write_control(5, first);
    third = r_u32(matrix + 28);
    xport_gte_write_control(6, second); xport_gte_write_control(7, third);
}

uint32 level2p_sub_8008C61C(uint32 model, uint32 bone, uint32 output)
{
    uint32 matrix[8] = { 0 }, transformed[3] = { 0 };
    uint8 flags[8] = { 0 };
    uint32 base, stack, value, save, index, first, second, matrix_view, result_view, flags_view;
    FUNCTION_MARKER(0x8008C61C, "LEVEL2P.BIN");
    if (!model) return output;
    bone <<= 3;
    base = r_u32(model + 0xC0);
    w_u16(output, r_u16(base + bone));
    base = r_u32(model + 0xC0);
    stack = r_u32(0x80099100);
    value = r_u16(base + bone + 2);
    w_u16(output + 2, value);
    base = r_u32(model + 0xC0);
    value = r_u16(base + bone + 4);
    save = 0x80099104 + stack;
    w_u16(output + 4, value);
    for (index = 0; index < 8; index += 2)
    {
        first = xport_gte_read_control(index);
        second = xport_gte_read_control(index + 1);
        w_u32(save + index * 4, first);
        w_u32(save + (index + 1) * 4, second);
    }
    value = r_u32(model + 8);
    w_u32(0x80099100, stack + 32);
    if (value && (r_u32(model) & 1)) moh_bone_install_controls(model + 0x74);
    else
    {
        /* TODO: Addressable matrix requires the permitted local adapter */
        matrix_view = moh_draft_local_address(matrix, sizeof(matrix));
        level2p_sub_8008BB44(model + 0x3C, model + 0x60, model + 0x24, matrix_view);
        moh_bone_install_controls(matrix_view);
    }
    /* TODO: SDK transform outputs retain permitted local and call boundaries */
    result_view = moh_draft_local_address(transformed, sizeof(transformed));
    flags_view = moh_draft_local_address(flags, sizeof(flags));
    moh_draft_missing_call(0x80016548, output, result_view, flags_view);
    stack = r_u32(0x80099100) - 32;
    save = 0x80099104 + stack;
    second = r_u32(save + 4);
    w_u32(0x80099100, stack);
    first = r_u32(save);
    xport_gte_write_control(0, first); xport_gte_write_control(1, second);
    for (index = 2; index < 8; index += 2)
    {
        second = r_u32(save + (index + 1) * 4);
        first = r_u32(save + index * 4);
        xport_gte_write_control(index, first); xport_gte_write_control(index + 1, second);
    }
    w_u16(output, (uint16)transformed[0]);
    w_u16(output + 2, (uint16)transformed[1]);
    w_u16(output + 4, (uint16)transformed[2]);
    return output;
}


void level2p_sub_80045444(uint32 owner, uint32 moving)
{
    uint32 data, moving_data, target, first, second, third, distance, speed, timer;
    FUNCTION_MARKER(0x80045444, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388);
    moving_data = r_u32(moving + 0x388);
    target = data + 0x820;
    first = r_u32(data + 0x814); second = r_u32(data + 0x818); third = r_u32(data + 0x81C);
    w_u32(moving + 0x20C, first); w_u32(moving + 0x210, second); w_u32(moving + 0x214, third);
    level2p_sub_800450D8(moving + 0x20C, target, moving + 0x228, 0);
    distance = level2p_sub_8006883C(moving + 0x20C, target);
    speed = r_u32(moving + 0x200);
    if (!speed)
    {
        moh_draft_fail_fast("Original motion division BREAK 0x1C00", 0x800454BC);
        return;
    }
    if (speed == 0xFFFFFFFF && distance == 0x80000000)
    {
        moh_draft_fail_fast("Original motion division BREAK 0x1800", 0x800454D4);
        return;
    }
    timer = (uint32)((sint32)distance / (sint32)speed) + 1;
    w_u8(moving_data + 0x40, timer);
    first = r_u32(moving + 0x14);
    w_u32(moving_data + 0x3C, first);
    w_u32(moving + 0x14, r_u32(moving + 0x14) & 0xF7FFF9FF);
}

void level2p_sub_80045520(uint32 owner, uint32 output)
{
    FUNCTION_MARKER(0x80045520, "LEVEL2P.BIN");
    w_u32(output, r_u32(owner + 0x20C));
    w_u32(output + 4, r_u32(owner + 0x210));
    w_u32(output + 8, r_u32(owner + 0x214));
}

void level2p_sub_80045544(uint32 owner, uint32 origin, uint32 output)
{
    uint32 target;
    FUNCTION_MARKER(0x80045544, "LEVEL2P.BIN");
    target = r_u32(owner + 0x388);
    level2p_sub_800450D8(origin, target, output, 0);
}

uint32 level2p_sub_8006883C(uint32 origin, uint32 target)
{
    uint32 x, z, first, second, smaller;
    FUNCTION_MARKER(0x8006883C, "LEVEL2P.BIN");
    x = r_u32(target); first = r_u32(origin);
    z = r_u32(target + 8);
    x -= first;
    second = r_u32(origin + 8);
    if ((sint32)x < 0) x = 0 - x;
    z -= second;
    if ((sint32)z < 0) z = 0 - z;
    smaller = (sint32)z < (sint32)x ? z : x;
    return x + z - (uint32)((sint32)smaller >> 1);
}


void level2p_sub_800452D0(uint32 owner, uint32 origin, uint32 output, uint32 explicit_point, uint32 model, uint32 first_point, uint32 second_point)
{
    uint32 start[3] = { 0 }, end[3] = { 0 }, cleared[2][3] = { { 0 } };
    uint32 start_view, end_view, clear_view;
    FUNCTION_MARKER(0x800452D0, "LEVEL2P.BIN");
    /* TODO: Addressable vectors and SDK clear retain permitted boundaries */
    if (explicit_point)
    {
        start_view = moh_draft_local_address(start, sizeof(start));
        moh_draft_missing_call(0x800158AC, start_view, 0, 12);
        start[0] = (uint32)(sint32)r_s16(explicit_point) << 16;
        start[1] = (uint32)(sint32)r_s16(explicit_point + 2) << 16;
        start[2] = (uint32)(sint32)r_s16(explicit_point + 4) << 16;
        end[0] = start[0]; end[1] = start[1]; end[2] = start[2];
        end_view = moh_draft_local_address(end, sizeof(end));
        level2p_sub_800450D8(origin, end_view, output, 0);
        w_u32(output + 8, 0);
    }
    else
    {
        clear_view = moh_draft_local_address(cleared[0], sizeof(cleared[0]));
        moh_draft_missing_call(0x800158AC, clear_view, 0, 12);
        cleared[0][0] = (uint32)(sint32)r_s16(second_point) << 16;
        cleared[0][1] = (uint32)(sint32)r_s16(second_point + 2) << 16;
        cleared[0][2] = (uint32)(sint32)r_s16(second_point + 4) << 16;
        end[0] = cleared[0][0]; end[1] = cleared[0][1]; end[2] = cleared[0][2];
        clear_view = moh_draft_local_address(cleared[1], sizeof(cleared[1]));
        moh_draft_missing_call(0x800158AC, clear_view, 0, 12);
        cleared[1][0] = (uint32)(sint32)r_s16(first_point) << 16;
        cleared[1][1] = (uint32)(sint32)r_s16(first_point + 2) << 16;
        cleared[1][2] = (uint32)(sint32)r_s16(first_point + 4) << 16;
        start[0] = cleared[1][0]; start[1] = cleared[1][1]; start[2] = cleared[1][2];
        start_view = moh_draft_local_address(start, sizeof(start));
        end_view = moh_draft_local_address(end, sizeof(end));
        level2p_sub_800450D8(start_view, end_view, output, 0);
    }
}

void level2p_sub_80045570(uint32 owner, uint32 moving, uint32 first_bone, uint32 second_bone, uint32 explicit_point, uint32 model, uint32 first_parameter, uint32 second_parameter, uint32 timed)
{
    sint16 first[4] = { 0 }, second[4] = { 0 };
    uint32 data, handle, related, kind, direct, first_view, second_view;
    FUNCTION_MARKER(0x80045570, "LEVEL2P.BIN");
    if (timed) { level2p_sub_80045444(owner, moving); return; }
    if (r_u32(owner + 0xC) & 1)
    {
        data = r_u32(owner + 0x388);
        if (r_u32(data) & 0x100)
        {
            handle = r_u32(data + 0xA0);
            related = handle ? r_u32(0x80099460 + (handle & 0xFFFF) * 4) : 0;
            /* TODO: Original forwards a zero resolved owner without a null guard */
            level2p_sub_80045520(related, moving + 0x20C);
            level2p_sub_80045544(related, moving + 0x20C, moving + 0x228);
            return;
        }
    }
    kind = r_u32(moving + 8);
    direct = kind == 0x13F3;
    if (kind == 0x13F1) direct = r_u32(owner + 0xC) & 1;
    /* TODO: Addressable bone outputs require permitted local adapters */
    first_view = moh_draft_local_address(first, sizeof(first));
    second_view = moh_draft_local_address(second, sizeof(second));
    level2p_sub_800451CC(owner, moving + 0x20C, first_bone, second_bone, model,
        first_parameter, second_parameter, first_view, second_view);
    if (direct) level2p_sub_800452B0(owner, moving + 0x228);
    else level2p_sub_800452D0(owner, moving + 0x20C, moving + 0x228, explicit_point,
        model, first_view, second_view);
}


uint32 level2p_sub_80069CD8(uint32 owner)
{
    sint16 vector[3] = { 0 }, normalized[3] = { 0 };
    uint32 direction[3] = { 0 }, transformed[3] = { 0 };
    uint8 transform_flags[8] = { 0 };
    uint32 active = 0, index, record, flags, life, x, y, z, vx, vy, vz, value, other;
    uint32 metric, first, second, third, limit, damping, magnitude, axis, field, stack, save;
    uint32 count, cursor, emitted, random, spread, base, centered, factor, input_view, output_view, flags_view;
    sint32 threshold, budget;
    FUNCTION_MARKER(0x80069CD8, "LEVEL2P.BIN");
    for (index = 0; index < 20; ++index)
    {
        record = owner + 0x3C + index * 56;
        flags = r_u32(record);
        if (!(flags & 1)) continue;
        active = 1;
        life = r_u32(record + 0x28);
        metric = 0xFFFFFFFF;
        if ((sint32)life <= 0) { w_u32(record, flags & 0xFFFFFFFE); continue; }
        if ((sint32)life < 32000) w_u32(record + 0x28, life - 1);
        x = r_u32(record + 0x10); y = r_u32(record + 0x14); z = r_u32(record + 0x18);
        vx = r_u32(record + 4);
        w_u32(record + 0x1C, x);
        vy = r_u32(record + 8);
        w_u32(record + 0x10, x + vx);
        w_u32(record + 0x20, y);
        vz = r_u32(record + 12);
        w_u32(record + 0x24, z);
        w_u32(record + 0x14, y + vy);
        w_u32(record + 0x18, z + vz);
        if (r_u32(owner) & 0x10)
        {
            vx = r_u32(record + 4);
            y = (uint32)(sint32)r_s16(record + 10); z = (uint32)(sint32)r_s16(record + 14);
            xport_gte_write_data(9, (uint32)((sint32)vx >> 16));
            xport_gte_write_data(10, y); xport_gte_write_data(11, z);
            xport_gte_execute(0xA00428);
            first = xport_gte_read_data(25); second = xport_gte_read_data(26); third = xport_gte_read_data(27);
            limit = r_u16(owner + 0x500);
            metric = first + second + third;
            if ((sint32)metric < (sint32)limit)
            {
                value = r_u32(owner + 0x4E0);
                if (value) w_u32(record + 4, vx + value);
                value = r_u32(owner + 0x4E4);
                if (value) w_u32(record + 8, r_u32(record + 8) + value);
                value = r_u32(owner + 0x4E8);
                if (value) w_u32(record + 12, r_u32(record + 12) + value);
            }
        }
        else
        {
            value = r_u32(record + 8);
            limit = r_u32(owner + 0x4FC);
            magnitude = (sint32)value < 0 ? 0 - value : value;
            if ((sint32)magnitude < (sint32)limit || (sint32)(value ^ r_u32(owner + 0x4E4)) < 0)
            {
                other = r_u32(owner + 0x4E4);
                if (other) w_u32(record + 8, value + other);
            }
        }
        if (r_u32(owner) & 8)
        {
            if ((sint32)metric < 0)
            {
                x = (uint32)(sint32)r_s16(record + 6);
                y = (uint32)(sint32)r_s16(record + 10); z = (uint32)(sint32)r_s16(record + 14);
                xport_gte_write_data(9, x); xport_gte_write_data(10, y); xport_gte_write_data(11, z);
                xport_gte_execute(0xA00428);
                first = xport_gte_read_data(25); second = xport_gte_read_data(26); third = xport_gte_read_data(27);
                metric = first + second + third;
            }
            threshold = r_s16(owner + 0x4FA);
            limit = threshold ? (uint32)threshold * (uint32)threshold : 0;
            if ((sint32)limit < (sint32)metric)
            {
                vector[0] = r_s16(record + 6); vector[1] = r_s16(record + 10); vector[2] = r_s16(record + 14);
                /* TODO: SDK normalization and addressable locals retain permitted boundaries */
                input_view = moh_draft_local_address(vector, sizeof(vector));
                output_view = moh_draft_local_address(normalized, sizeof(normalized));
                moh_draft_missing_call(0x80016A40, input_view, output_view);
                for (axis = 0; axis < 3; ++axis)
                {
                    factor = (uint32)(r_s32(owner + 0x4F8) >> 12);
                    value = r_u32(record + 4 + axis * 4);
                    w_u32(record + 4 + axis * 4, value - (uint32)(sint32)normalized[axis] * factor);
                }
            }
            else { w_u32(record + 4, 0); w_u32(record + 8, 0); w_u32(record + 12, 0); }
        }
        else
        {
            for (axis = 0; axis < 2; ++axis)
            {
                field = record + (axis ? 12 : 4);
                value = r_u32(field); damping = r_u32(owner + 0x4F8);
                magnitude = (sint32)value < 0 ? 0 - value : value;
                if ((sint32)damping < (sint32)magnitude)
                    w_u32(field, (sint32)value > 0 ? value - damping : value + damping);
                else w_u32(field, 0);
            }
        }
    }
    if (r_u32(owner) & 0x20)
    {
        stack = r_u32(0x80099100); save = 0x80099104 + stack;
        for (index = 0; index < 8; index += 2)
        {
            first = xport_gte_read_control(index); second = xport_gte_read_control(index + 1);
            w_u32(save + index * 4, first); w_u32(save + (index + 1) * 4, second);
        }
        w_u32(0x80099100, stack + 32);
        moh_bone_install_controls(owner + 0x1C);
    }
    budget = r_s16(owner + 0x504);
    value = r_u16(owner + 0x504);
    if (budget > 0)
    {
        active = 1;
        if (r_u32(owner) & 1) w_u16(owner + 0x504, value - 1);
        if (r_u16(owner + 0x502)) w_u16(owner + 0x502, r_u16(owner + 0x502) - 1);
        else
        {
            if (r_u32(owner) & 4) w_u16(owner + 0x504, r_u16(owner + 0x504) - 1);
            spread = r_u16(owner + 0x4A2); count = r_u16(owner + 0x4A0);
            cursor = 0;
            if (spread)
            {
                random = sub_8001552C(); spread = r_u16(owner + 0x4A2);
                count += (uint32)((sint32)(random * spread) >> 15);
            }
            for (emitted = 0; (sint32)emitted < (sint32)count; ++emitted)
            {
                while (cursor < 20 && (r_u32(owner + 0x3C + cursor * 56) & 1)) ++cursor;
                if (cursor >= 20) break;
                record = owner + 0x3C + cursor * 56;
                w_u32(record, r_u32(record) | 1);
                flags = r_u32(owner); ++cursor;
                centered = flags & 0x40;
                if (flags & 0x20)
                {
                    for (axis = 0; axis < 3; ++axis)
                    {
                        field = owner + 0x4BC + axis * 2;
                        spread = r_u16(field);
                        value = centered ? 0 - (uint32)((sint32)(sint16)spread >> 1) : 0;
                        if ((sint16)spread)
                        {
                            random = sub_8001552C(); spread = (uint32)(sint32)r_s16(field);
                            if (centered) value += (uint32)((sint32)(random * spread) >> 15);
                            else value = (random * spread) >> 15;
                        }
                        vector[axis] = (sint16)value;
                    }
                    input_view = moh_draft_local_address(vector, sizeof(vector));
                    output_view = moh_draft_local_address(transformed, sizeof(transformed));
                    flags_view = moh_draft_local_address(transform_flags, sizeof(transform_flags));
                    moh_draft_missing_call(0x80016548, input_view, output_view, flags_view);
                    for (axis = 0; axis < 3; ++axis)
                    {
                        value = transformed[axis]; base = r_u32(owner + 4 + axis * 4);
                        w_u32(record + 0x10 + axis * 4, base + (value << 16));
                    }
                    first = r_u32(record + 0x10); second = r_u32(record + 0x14); third = r_u32(record + 0x18);
                    w_u32(record + 0x1C, first); w_u32(record + 0x20, second); w_u32(record + 0x24, third);
                    for (axis = 0; axis < 3; ++axis)
                    {
                        field = owner + 0x4B0 + axis * 2;
                        spread = (uint32)(sint32)r_s16(field); base = r_u16(owner + 0x4A4 + axis * 2);
                        if (spread)
                        {
                            random = sub_8001552C(); spread = (uint32)(sint32)r_s16(field);
                            base += (uint32)((sint32)(random * spread) >> 15);
                        }
                        vector[axis] = (sint16)base;
                    }
                    input_view = moh_draft_local_address(vector, sizeof(vector));
                    output_view = moh_draft_local_address(direction, sizeof(direction));
                    moh_draft_missing_call(0x80016548, input_view, output_view, flags_view);
                }
                else
                {
                    for (axis = 0; axis < 3; ++axis)
                    {
                        field = owner + 0x4BC + axis * 2;
                        spread = (uint32)(sint32)r_s16(field);
                        if (spread)
                        {
                            random = sub_8001552C(); spread = (uint32)(sint32)r_s16(field);
                            base = (uint32)(sint32)r_s16(owner + 6 + axis * 4);
                            value = (uint32)((sint32)(random * spread) >> 15);
                            if (centered) base -= (uint32)((sint32)spread >> 1);
                            value += base;
                        }
                        else value = (uint32)(sint32)r_s16(owner + 6 + axis * 4);
                        w_u32(record + 0x10 + axis * 4, value << 16);
                    }
                    first = r_u32(record + 0x10); second = r_u32(record + 0x14); third = r_u32(record + 0x18);
                    w_u32(record + 0x1C, first); w_u32(record + 0x20, second); w_u32(record + 0x24, third);
                    for (axis = 0; axis < 3; ++axis)
                    {
                        field = owner + 0x4B0 + axis * 2;
                        spread = (uint32)(sint32)r_s16(field);
                        if (spread)
                        {
                            random = sub_8001552C(); spread = (uint32)(sint32)r_s16(field);
                            base = (uint32)(sint32)r_s16(owner + 0x4A4 + axis * 2);
                            direction[axis] = base + (uint32)((sint32)(random * spread) >> 15);
                        }
                        else direction[axis] = (uint32)(sint32)r_s16(owner + 0x4A4 + axis * 2);
                    }
                }
                input_view = moh_draft_local_address(direction, sizeof(direction));
                output_view = moh_draft_local_address(normalized, sizeof(normalized));
                moh_draft_missing_call(0x800169FC, input_view, output_view);
                spread = (uint32)(sint32)r_s16(owner + 0x4CE);
                if (spread)
                {
                    random = sub_8001552C(); spread = (uint32)(sint32)r_s16(owner + 0x4CE);
                    base = (uint32)(sint32)r_s16(owner + 0x4CA);
                    value = base + (uint32)((sint32)(random * spread) >> 15);
                }
                else value = (uint32)(sint32)r_s16(owner + 0x4CA);
                factor = (uint32)((sint32)(value << 16) >> 12);
                for (axis = 0; axis < 3; ++axis)
                    w_u32(record + 4 + axis * 4, (uint32)(sint32)normalized[axis] * factor);
                spread = r_u16(owner + 0x4D2);
                if (spread)
                {
                    random = sub_8001552C(); spread = r_u16(owner + 0x4D2); base = r_u16(owner + 0x4D0);
                    value = base + (uint32)((sint32)(random * spread) >> 15);
                }
                else value = r_u16(owner + 0x4D0);
                w_u32(record + 0x28, value);
                if (r_s32(owner + 0x4D8) > 0)
                {
                    base = r_u32(owner + 0x4D4); random = sub_8001552C(); spread = r_u32(owner + 0x4D8);
                    w_u32(record + 0x30, base + (uint32)((sint32)(random * spread) >> 15));
                }
                if (r_u32(owner) & 2) w_u16(owner + 0x504, r_u16(owner + 0x504) - 1);
                spread = r_u16(owner + 0x49E); base = r_u16(owner + 0x49C);
                if (spread)
                {
                    random = sub_8001552C(); spread = r_u16(owner + 0x49E);
                    base += (uint32)((sint32)(random * spread) >> 15);
                }
                w_u16(owner + 0x502, base);
            }
        }
    }
    if (r_u32(owner) & 0x20)
    {
        stack = r_u32(0x80099100) - 32; save = 0x80099104 + stack;
        second = r_u32(save + 4); w_u32(0x80099100, stack); first = r_u32(save);
        xport_gte_write_control(0, first); xport_gte_write_control(1, second);
        for (index = 2; index < 8; index += 2)
        {
            second = r_u32(save + (index + 1) * 4); first = r_u32(save + index * 4);
            xport_gte_write_control(index, first); xport_gte_write_control(index + 1, second);
        }
    }
    return active;
}


uint32 level2p_sub_80045718(uint32 owner, uint32 kind, uint32 model, uint32 speed, uint32 first_bone, uint32 second_bone, uint32 explicit_point, uint32 parameter8, uint32 damage, uint32 duration, uint32 extra_bytes, uint32 draw_model, uint32 first_parameter, uint32 second_parameter, uint32 timed, uint32 object_flags, uint32 role_flags, uint32 state)
{
    uint32 object, data, value, old_flags, flags, first, second, third, source_data, index, weapon;
    uint32 distance, product, position;
    sint16 rotation[3];
    sint32 x, z, smaller, larger;
    FUNCTION_MARKER(0x80045718, "LEVEL2P.BIN");
    object = level2p_sub_80066934(model != 0xFFFFFFFF, extra_bytes + 0x90, model,
        model != 0xFFFFFFFF ? 0x147 : 0, 2, 0, 0, 0);
    if (!object) return 0;
    w_u32(object + 0x36C, 0x80044818);
    w_u32(object + 0x370, 0x80044EB4);
    w_u32(object + 0x374, 0x800441E0);
    w_u32(object + 0x378, 0x8004362C);
    w_u32(object + 0x384, 0x80044FA4);
    w_u32(object + 8, kind);
    old_flags = r_u32(object + 0x10);
    w_u32(object + 0xC, object_flags | 0x108);
    w_u32(object + 0x14, state);
    w_u32(object + 0x10, old_flags | 0x200 | role_flags);
    value = r_u32(owner + 0xC);
    w_u32(object + 0xF8, damage);
    w_u32(object + 0xC, r_u32(object + 0xC) | (value & 4));
    level2p_sub_80068A04(object + 0x30, 0);
    data = r_u32(object + 0x388);
    w_u8(object + 0x34, 0xFF);
    w_u16(data + 0x38, 0);
    level2p_sub_80068A04(data + 0x54, owner);
    w_u16(data + 0x3A, duration);
    w_u32(data + 0x2C, 0);
    w_u32(data + 0x30, 0);
    w_u8(data + 0x42, 0);
    w_u32(data + 0x34, 0x10000);
    flags = r_u32(owner + 0x10);
    if (flags & 0x1000) w_u32(object + 0x10, r_u32(object + 0x10) | 0x1000);
    else if (flags & 0x2000) w_u32(object + 0x10, r_u32(object + 0x10) | 0x2000);
    w_u32(object + 0x218, 0); w_u32(object + 0x21C, 0); w_u32(object + 0x220, 0);
    w_u32(object + 0x200, speed);
    level2p_sub_80045570(owner, object, first_bone, second_bone, explicit_point, draw_model,
        first_parameter, second_parameter, timed);
    w_u32(object + 0x284, 0);
    level2p_sub_80068A04(data + 0x58, 0);
    w_u32(data + 0x44, 0);
    level2p_sub_8006591C(object);
    first = r_u32(object + 0x20C); second = r_u32(object + 0x210); third = r_u32(object + 0x214);
    w_u32(object + 0x258, first); w_u32(object + 0x25C, second); w_u32(object + 0x260, third);
    level2p_sub_8004DFBC(object);
    if (r_u32(owner + 0xC) & 1)
    {
        source_data = r_u32(owner + 0x388);
        index = (uint32)(sint32)(sint8)r_u8(source_data + 0x54);
        weapon = r_u8(0x8009AB8C + index);
        if (r_u16(0x8008F94A + weapon * 68) & 2)
        {
            rotation[0] = (sint16)r_u16(object + 0x344);
            rotation[1] = (sint16)r_u16(object + 0x34A);
            rotation[2] = (sint16)r_u16(object + 0x350);
            first = (uint32)(sint32)r_s16(object + 0x20E); second = (uint32)(sint32)r_s16(owner + 0x20E);
            x = (sint16)(first - second);
            first = (uint32)(sint32)r_s16(object + 0x216); second = (uint32)(sint32)r_s16(owner + 0x216);
            z = (sint16)(first - second);
            if (x < 0) x = -x;
            if (z < 0) z = -z;
            larger = x; smaller = 0;
            if (x < 0) { larger = 0; smaller = x; }
            if (larger < z) { value = (uint32)larger; larger = z; z = (sint32)value; }
            distance = (uint32)larger + (uint32)((smaller + z) >> 2);
            distance -= (uint32)((sint32)distance >> 2);
            product = (0 - (uint32)(sint32)rotation[0]) * distance;
            position = r_u32(object + 0x20C);
            w_u32(object + 0x20C, position + ((uint32)((sint32)product >> 12) << 16));
            product = (0 - (uint32)(sint32)rotation[1]) * distance;
            position = r_u32(object + 0x210);
            w_u32(object + 0x210, position + ((uint32)((sint32)product >> 12) << 16));
            product = (0 - (uint32)(sint32)rotation[2]) * distance;
            position = r_u32(object + 0x214);
            w_u32(object + 0x214, position + ((uint32)((sint32)product >> 12) << 16));
        }
    }
    if (!model || model == 0xFFFFFFFF)
    {
        w_u32(r_u32(object + 0x304) + 0x3C, 0);
        w_u32(r_u32(object + 0x304) + 0x84, 0);
    }
    level2p_sub_8004D8D4(object);
    level2p_sub_80066BC8(object);
    first = r_u32(object + 0x20C); second = r_u32(object + 0x210); third = r_u32(object + 0x214);
    w_u32(data + 0x84, first); w_u32(data + 0x88, second); w_u32(data + 0x8C, third);
    return object;
}

void level2p_sub_80045B1C(uint32 owner, uint32 red, uint32 green, uint32 blue, uint32 end_red, uint32 end_green, uint32 end_blue)
{
    uint32 data, first, second, third;
    FUNCTION_MARKER(0x80045B1C, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388);
    w_u16(data + 0x38, r_u16(data + 0x38) | 0xA0);
    /* TODO: Missing SDK packet initialization remains a permitted fail-fast boundary */
    moh_draft_missing_call(0x80016168, data + 4);
    moh_draft_missing_call(0x80016168, data + 0x18);
    w_u8(data + 8, red); w_u8(data + 9, green); w_u8(data + 10, blue);
    w_u8(data + 0x1C, red); w_u8(data + 0x1D, green); w_u8(data + 0x1E, blue);
    w_u8(data + 0x10, end_red); w_u8(data + 0x11, end_green); w_u8(data + 0x12, end_blue);
    w_u8(data + 0x24, end_red); w_u8(data + 0x25, end_green); w_u8(data + 0x26, end_blue);
    first = r_u32(owner + 0x20C); second = r_u32(owner + 0x210); third = r_u32(owner + 0x214);
    w_u32(owner + 0x258, first); w_u32(owner + 0x25C, second); w_u32(owner + 0x260, third);
}

void level2p_sub_80066BC8(uint32 owner)
{
    uint32 current, next;
    FUNCTION_MARKER(0x80066BC8, "LEVEL2P.BIN");
    current = r_u32(0x80099434);
    /* TODO: Original assumes an existing list head and has no null guard */
    next = r_u32(current + 4);
    while (next) { current = r_u32(current + 4); next = r_u32(current + 4); }
    w_u32(owner + 4, 0);
    w_u32(current + 4, owner);
}


uint32 level2p_sub_80045BF8(uint32 owner, uint32 model, uint32 first_bone, uint32 second_bone, uint32 timer, uint32 speed, uint32 explicit_point, uint32 first_parameter, uint32 second_parameter)
{
    uint32 zero_vector[4] = { 0 };
    uint16 matrix[16] = { 0 };
    sint16 input[4] = { 0 };
    uint32 object, data, first, second, random, value, matrix_view, zero_view;
    FUNCTION_MARKER(0x80045BF8, "LEVEL2P.BIN");
    object = level2p_sub_80045718(owner, 0x13F3, model, speed << 16, first_bone, second_bone,
        explicit_point, 0x20, 0x4000, 150, 0, 0, first_parameter, second_parameter, 0, 0, 0, 0);
    if (!object) return 0;
    w_u16(object + 0x2EC, 40);
    w_u32(object + 0x2D4, 0x280000);
    w_u32(object + 0x37C, 0x80043E7C);
    w_u32(object + 0x374, 0x800443BC);
    first = r_u32(object + 0xC); second = r_u32(object + 0x10);
    data = r_u32(object + 0x388);
    w_u32(object + 0xC, first | 0x40000A0);
    w_u32(object + 0x10, second | 0x40);
    w_u16(data + 0x38, 0x4349);
    w_u8(data + 0x41, timer);
    w_u32(data + 0x34, 0x30000);
    random = sub_8001552C();
    w_u32(data + 0x68, (uint32)((sint32)(random << 12) >> 15) << 16);
    w_u32(data + 0x6C, 0); w_u32(data + 0x70, 0);
    random = sub_8001552C();
    w_u32(data + 0x74, ((uint32)((sint32)(random * 650) >> 15) + 200) << 16);
    random = sub_8001552C();
    w_u32(data + 0x78, ((uint32)((sint32)(random * 30) >> 15) + 10) << 16);
    w_u32(data + 0x7C, 0);
    w_u8(data + 0x42, 120);
    /* TODO: SDK clear/rotation/translation and addressable locals retain permitted boundaries */
    zero_view = moh_draft_local_address(zero_vector, sizeof(zero_vector));
    moh_draft_missing_call(0x800158AC, zero_view, 0, 16);
    w_u32(object + 0x230, 0);
    input[0] = 0; input[1] = 0;
    input[2] = r_s16(object + 0x202);
    matrix[0] = 0x1000; matrix[4] = 0x1000; matrix[8] = 0x1000;
    matrix_view = moh_draft_local_address(matrix, sizeof(matrix));
    moh_draft_missing_call(0x80015FC8, (uint32)(sint32)r_s16(object + 0x22A), matrix_view);
    moh_draft_missing_call(0x80016218, matrix_view, zero_view);
    moh_bone_install_controls(matrix_view);
    xport_gte_write_data(0, 0);
    /* Original unspecified upper half of VZ0 is discarded by the data register */
    xport_gte_write_data(1, (uint32)(sint32)input[2]);
    xport_gte_execute(0x486012);
    zero_vector[0] = xport_gte_read_data(25);
    zero_vector[1] = xport_gte_read_data(26);
    zero_vector[2] = xport_gte_read_data(27);
    value = zero_vector[1];
    w_u32(object + 0x21C, value << 16);
    value = zero_vector[2];
    w_u32(object + 0x228, 0);
    w_u16(object + 0x2E8, 76);
    w_u32(object + 0x200, value << 16);
    return object;
}

uint32 level2p_sub_80045E68(uint32 owner, uint32 first_bone, uint32 second_bone, uint32 explicit_point, uint32 model, uint32 first_parameter, uint32 second_parameter, uint32 damage, uint32 spread, uint32 timed, uint32 kind, uint32 object_flags, uint32 role_flags, uint32 state)
{
    uint32 random, high, quotient, speed, object, data, flags, mask, center, first, second, third;
    sint64 product;
    FUNCTION_MARKER(0x80045E68, "LEVEL2P.BIN");
    random = sub_8001552C();
    product = (sint64)(sint32)random * (sint64)(sint32)0xEA0EA0EB;
    high = (uint32)((uint64)product >> 32);
    quotient = (uint32)((sint32)(high + random) >> 6) - (uint32)((sint32)random >> 31);
    speed = (random - quotient * 70 + 600) << 16;
    object = level2p_sub_80045718(owner, kind, 0xFFFFFFFF, speed, first_bone, second_bone,
        explicit_point, 0x20, damage, 15, 0, model, first_parameter, second_parameter, timed,
        object_flags, role_flags, state);
    if (!object) return 0;
    data = r_u32(object + 0x388);
    w_u32(object + 0x37C, 0x80043E7C);
    w_u16(data + 0x38, 0);
    if ((r_u32(owner + 0xC) & 1) && (r_u32(r_u32(owner + 0x388) + 0x72C) & 0x20))
        w_u16(data + 0x38, 0xA40);
    level2p_sub_80045B1C(object, 230, 180, 70, 40, 20, 10);
    flags = r_u32(object + 0x10);
    w_u32(object + 0x10, flags | 0x90004);
    if (spread)
    {
        random = sub_8001552C();
        mask = ((uint32)2 << (spread & 31)) - 1;
        center = (uint32)1 << (spread & 31);
        first = (random & mask) - center;
        random = sub_8001552C(); second = (random & mask) - center;
        random = sub_8001552C(); third = (random & mask) - center;
        w_u32(object + 0x228, r_u32(object + 0x228) + (first << 9));
        w_u32(object + 0x22C, r_u32(object + 0x22C) + (second << 9));
        w_u32(object + 0x230, r_u32(object + 0x230) + (third << 9));
    }
    return object;
}


uint32 level2p_sub_8004609C(uint32 owner, uint32 first_bone, uint32 second_bone, uint32 explicit_point, uint32 first_parameter, uint32 second_parameter, uint32 timed, uint32 kind, uint32 object_flags, uint32 role_flags, uint32 state)
{
    uint32 object, data, flags, role, x, y, z;
    FUNCTION_MARKER(0x8004609C, "LEVEL2P.BIN");
    object = level2p_sub_80045718(owner, kind, 0x9EED43D5, 0xC80000, first_bone, second_bone,
        explicit_point, 0x20, 0x40000, 30, 0xB64, 0, first_parameter, second_parameter, timed,
        object_flags, role_flags, state);
    if (!object) return 0;
    data = r_u32(object + 0x388);
    w_u32(data, data + 0x90);
    w_u32(object + 0x37C, 0x80043E7C);
    flags = r_u32(object + 0xC); role = r_u32(object + 0x10);
    w_u32(object + 0xC, flags | 0x4000000);
    w_u32(object + 0x10, role | 4);
    w_u16(data + 0x38, 0x130);
    x = (uint32)(sint32)r_s16(object + 0x21A);
    y = (uint32)(sint32)(sint16)(r_u16(object + 0x21E) - 25);
    z = (uint32)(sint32)r_s16(object + 0x222);
    level2p_sub_80069AF0(r_u32(data), 2, 100, 1, 0, 3, 3, x, y, z, 12, 20, 12,
        5, 5, 5, 0x50000, 0x190000, 6, 8, 0, 0xFFFFE000, 0, 0x50000, 0x230000,
        0, 0, 1, 2, 40);
    level2p_sub_8006B91C(r_u32(data), 0x278, 0x1EC, 31, 19, 400, 0x1F5, 2);
    if ((r_u32(owner + 0xC) & 1) && (r_u32(r_u32(owner + 0x388) + 0x72C) & 0x20))
        w_u16(data + 0x38, r_u16(data + 0x38) | 0xA40);
    level2p_sub_8003F764(object, 0, 1, 0);
    w_u8(0x800A94CC, 180); w_u8(0x800A94CD, 160); w_u8(0x800A94CE, 16);
    w_u16(0x800A94D0, 500);
    w_u32(0x800A94D4, 250000);
    w_u32(0x80099438, object);
    return object;
}

uint32 level2p_sub_80046318(uint32 owner, uint32 first_bone, uint32 second_bone, uint32 explicit_point, uint32 model, uint32 first_parameter, uint32 second_parameter, uint32 timed, uint32 kind, uint32 object_flags, uint32 role_flags, uint32 state)
{
    uint32 random, speed, object, data, first, second, third;
    FUNCTION_MARKER(0x80046318, "LEVEL2P.BIN");
    random = sub_8001552C();
    speed = ((uint32)((sint32)(random * 400) >> 15) + 350) << 16;
    object = level2p_sub_80045718(owner, kind, 0xFFFFFFFF, speed, first_bone, second_bone,
        explicit_point, 0x20, 0x20000, 15, 0, model, first_parameter, second_parameter, timed,
        object_flags, role_flags, state);
    if (!object) return 0;
    data = r_u32(object + 0x388);
    w_u32(object + 0x37C, 0x80043E7C);
    w_u16(data + 0x38, 0);
    if ((r_u32(owner + 0xC) & 1) && (r_u32(r_u32(owner + 0x388) + 0x72C) & 0x20))
        w_u16(data + 0x38, 0xA40);
    level2p_sub_80045B1C(object, 230, 180, 70, 40, 20, 10);
    w_u32(object + 0x10, r_u32(object + 0x10) | 0x24);
    random = sub_8001552C(); first = (random & 0x3FFF) - 0x2000;
    random = sub_8001552C(); second = (random & 0x3FFF) - 0x2000;
    random = sub_8001552C(); third = (random & 0x3FFF) - 0x2000;
    w_u32(object + 0x228, r_u32(object + 0x228) + (first << 9));
    w_u32(object + 0x22C, r_u32(object + 0x22C) + (second << 9));
    w_u32(object + 0x230, r_u32(object + 0x230) + (third << 9));
    return object;
}


uint32 level2p_sub_80046500(uint32 owner)
{
    uint32 data, cheat, kind, index, capacity, field, loaded, difference, reserve, current;
    uint32 record, sample, flags, bank;
    sint32 needed;
    FUNCTION_MARKER(0x80046500, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388);
    cheat = r_u32(0x800386A4) & 0x40;
    kind = r_u8(data + 0x55);
    index = (uint32)(sint32)(sint8)r_u8(data + 0x54);
    capacity = r_u16(0x8008F92A + kind * 68);
    field = data + index * 2;
    loaded = r_u16(field + 0x66);
    difference = capacity - loaded;
    needed = (sint16)difference;
    if (needed <= 0) return 1;
    if (cheat) w_u16(field + 0x66, capacity);
    else
    {
        reserve = (uint32)(sint32)r_s16(field + 0x56);
        current = r_u16(field + 0x56);
        if (needed < (sint32)reserve)
        {
            w_u16(field + 0x66, capacity);
            index = (uint32)(sint32)(sint8)r_u8(data + 0x54);
            field = data + index * 2;
            w_u16(field + 0x56, r_u16(field + 0x56) - difference);
        }
        else
        {
            if ((sint32)reserve <= 0) return (sint16)loaded > 0;
            w_u16(field + 0x66, loaded + current);
            index = (uint32)(sint32)(sint8)r_u8(data + 0x54);
            w_u16(data + index * 2 + 0x56, 0);
        }
    }
    kind = r_u8(data + 0x55);
    if (kind == 3 || kind == 10)
    {
        index = (uint32)(sint32)(sint8)r_u8(data + 0x54);
        if (r_s16(data + index * 2 + 0x66)) level2p_sub_8005A1B8(owner, 0x10, 0);
    }
    else if (!r_u32(data + 0x710))
    {
        level2p_sub_8003F764(owner, 9, 1, 0);
        record = 0x8008F920 + (uint32)r_u8(data + 0x55) * 68;
        sample = (uint32)(sint32)r_s16(record + 0x20);
        if ((sint32)sample >= 0)
        {
            flags = (r_u16(record + 0x2A) >> 3) & 2;
            bank = (uint32)(sint32)r_s16(record + 0x22);
            if ((sint32)bank < 0) bank = (uint32)(sint32)r_s16(r_u32(r_u32(owner + 0x100) + 8) + 0xC8);
            level2p_sub_800420C4(bank, sample, owner, 0x20000, 0, 0x7FFF, 0, 0, 0, 0, flags);
        }
    }
    level2p_sub_80046BE4(owner);
    return 1;
}

void level2p_sub_8004696C(uint32 owner)
{
    uint32 data, palette, left, right, top, bottom, quad, packet;
    sint32 kind;
    FUNCTION_MARKER(0x8004696C, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388);
    kind = (sint8)r_u8(data + 0x54);
    if (kind == 1) { palette = 0x3DFF; left = 0xA0; right = 0xC0; top = 0xA0; bottom = 0xB4; }
    else if (kind == 2) { palette = 0x3FFF; left = 0xC0; right = 0xE0; top = 0xA0; bottom = 0xB4; }
    else if (kind == 3) { palette = 0x3E7F; left = 0xA0; right = 0xC0; top = 0xC0; bottom = 0xD4; }
    else { palette = 0x3DFE; left = 0x80; right = 0xA0; top = 0xA0; bottom = 0xB4; }
    w_u16(data + 0x6C2, palette);
    w_u16(data + 0x69A, palette);
    for (quad = 0; quad < 2; ++quad)
    {
        packet = data + 0x698 + quad * 40;
        w_u8(packet, left); w_u8(packet + 1, top);
        w_u8(packet + 8, right); w_u8(packet + 9, top);
        w_u8(packet + 16, left); w_u8(packet + 17, bottom);
        w_u8(packet + 24, right); w_u8(packet + 25, bottom);
    }
    level2p_sub_80046BE4(owner);
}


void level2p_sub_80046C6C(uint32 owner)
{
    uint32 data, mask;
    FUNCTION_MARKER(0x80046C6C, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388);
    w_u8(data + 0x693, 0x2C); w_u8(data + 0x6BB, 0x2C);
    w_u8(data + 0x68F, 9); w_u8(data + 0x6B7, 9);
    w_u8(data + 0x690, 0x80); w_u8(data + 0x691, 0x80);
    w_u8(data + 0x692, 0x80); w_u8(data + 0x6B8, 0x80);
    w_u8(data + 0x6B9, 0x80); w_u8(data + 0x6BA, 0x80);
    w_u16(data + 0x694, 0x19F); w_u16(data + 0x696, 0xF);
    w_u16(data + 0x69C, 0x1BF); w_u16(data + 0x69E, 0xF);
    w_u16(data + 0x6A4, 0x19F); w_u16(data + 0x6A6, 0x23);
    w_u16(data + 0x6AC, 0x1BF); w_u16(data + 0x6AE, 0x23);
    w_u16(data + 0x6BC, 0x19F); w_u16(data + 0x6BE, 0xF);
    w_u16(data + 0x6C4, 0x1BF); w_u16(data + 0x6C6, 0xF);
    w_u16(data + 0x6CC, 0x19F); w_u16(data + 0x6CE, 0x23);
    w_u16(data + 0x6D4, 0x1BF); w_u16(data + 0x6D6, 0x23);
    w_u16(data + 0x6CA, 0xF); w_u16(data + 0x6A2, 0xF);
    w_u8(data + 0x6DE, 0); w_u8(data + 0x6E3, 0);
    level2p_sub_80046BE4(owner);
    mask = (uint32)1 << (r_u8(data + 0x726) & 31);
    level2p_sub_8005ABFC(data + 0x6E0, 0x1C5, 0x12, 100, 100, 100, 0, 0xFFFFFFFF, 0xFF, mask, 0);
    mask = (uint32)1 << (r_u8(data + 0x726) & 31);
    level2p_sub_8005ABFC(data + 0x6DC, 0x17C, 0x12, 100, 100, 100, 0, 0xFFFFFFFF, 0xFF, mask, 0);
    level2p_sub_8004696C(owner);
}

void level2p_sub_80046DD8(void)
{
    uint32 names[13], index;
    FUNCTION_MARKER(0x80046DD8, "LEVEL2P.BIN");
    names[0] = r_u32(0x8009A624); names[1] = r_u32(0x8009A628);
    names[2] = r_u32(0x8009A62C); names[3] = r_u32(0x8009A630);
    names[4] = r_u32(0x8009A634); names[5] = r_u32(0x8009A638);
    names[6] = r_u32(0x8009A63C); names[7] = r_u32(0x8009A660);
    names[8] = r_u32(0x8009A640); names[9] = r_u32(0x8009A644);
    names[10] = r_u32(0x8009A648); names[11] = r_u32(0x8009A64C);
    names[12] = r_u32(0x8009A650);
    for (index = 0; index < 13; ++index) w_u32(0x8008F95C + index * 68, names[index]);
}


void level2p_sub_80046E80(uint32 owner, uint32 x, uint32 y, uint32 z)
{
    uint32 data;
    FUNCTION_MARKER(0x80046E80, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388);
    w_u16(data + 0xD0, x);
    w_u16(data + 0xD2, y);
    w_u16(data + 0xD4, z);
}

void level2p_sub_80046E98(uint32 owner, uint32 first, uint32 second)
{
    uint32 data;
    FUNCTION_MARKER(0x80046E98, "LEVEL2P.BIN");
    data = r_u32(owner + 0x388);
    w_u16(data + 0xCC, first);
    w_u16(data + 0xCE, second);
}

void level2p_sub_80046EAC(uint32 state)
{
    static const uint32 offsets[9] = { 0x48, 0x4C, 0x50, 0x18, 0x1C, 0x20, 0x78, 0x7C, 0x80 };
    uint32 index, address, current, target, difference, shift;
    FUNCTION_MARKER(0x80046EAC, "LEVEL2P.BIN");
    for (index = 0; index < 9; ++index)
    {
        address = state + offsets[index];
        current = r_u32(address);
        target = r_u32(address + 12);
        difference = target - current;
        if (current == target) continue;
        shift = r_u32(state + 0x10) & 31;
        w_u32(address, current + (uint32)((sint32)difference >> shift));
    }
}
