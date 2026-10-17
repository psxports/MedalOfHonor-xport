#include "psx.h"
#include "game_draft_signatures.h"

/* Unverified draft C, integration and adapters are deferred */

uint32 sub_8001AC88(uint32 a0)
{
    uint32 length = 0;
    if (a0) while (r_u8(a0++)) ++length;
    return length;
}

uint32 sub_8001D0CC(uint32 a0, uint32 a1)
{
    /* Continued from the existing word-clear draft */
    uint32 remaining = a1 - 1u;
    if (a1) do { w_u32(a0, 0u); a0 += 4u; --remaining; } while (remaining != 0xFFFFFFFFu);
    return remaining;
}

uint32 sub_80023CDC(uint32 a0) { return sub_800272BC(1u, a0); }
uint32 sub_80023CAC(uint32 a0) { return sub_8002703C(1u, a0); }
uint32 sub_80028A74(uint32 a0, uint32 a1) { return sub_800272BC(a0, a1); }
uint32 sub_80023C08(void) { return r_u32(0x800345D4u); }
uint32 sub_80023BF8(void) { w_u32(0x800345D4u, 1u); return 1u; }
void sub_80022E08(uint32 a0) { w_u32(0x800D0A78u, a0); }
uint32 sub_80022E48(void) { return 0x80034602u; }
uint32 sub_80022E20(uint32 a0) { return r_u32(0x800345F4u + (a0 << 2)); }

uint32 sub_8001E0EC(uint32 a0)
{
    uint32 previous = r_u32(0x8003504Cu);
    w_u32(0x8003504Cu, a0);
    return previous;
}

uint32 sub_8002AE48(uint32 a0)
{
    uint32 previous = r_u32(0x800D0A88u);
    w_u32(0x800D0A88u, a0);
    return previous;
}

uint32 sub_80025130(uint32 a0, uint32 a1)
{
    w_u8(a0 + 55u, 67u);
    w_u32(a0 + 44u, a0 + 36u);
    w_u8(a0 + 36u, (uint8)a1);
    w_u8(a0 + 54u, 1u);
    return 1u;
}

uint32 sub_80017D68(uint32 a0, uint32 a1, uint32 a2)
{
    uint32 command = 0xE1000000u;
    if (a1) command |= 0x200u;
    return command | (a2 & 0x9FFu) | (a0 ? 0x400u : 0u);
}

uint32 sub_80021888(void)
{
    uint32 target = r_u32(0x800302D0u);
    uint32 value = (r_u32(target) & 0xF0FFFFFFu) | 0x20000000u;
    w_u32(target, value);
    return value;
}

uint32 sub_8001CB08(uint32 a0)
{
    /* TODO Bind the callback table to native targets */
    return moh_draft_indirect_call(r_u32(r_u32(0x8002F258u) + 20u), 4u, a0);
}

uint32 sub_8001CAD8(uint32 argument0, uint32 argument1)
{
    /* TODO Bind the callback table and its forwarded argument ABI */
    return moh_draft_indirect_call(r_u32(r_u32(0x8002F258u) + 4u), argument0, argument1);
}

uint32 sub_8001AD98(void)
{
    sub_80023B80();
    moh_draft_missing_call(0x8001985Cu);
    moh_draft_missing_call(0x80022600u);
    return sub_80023BF8();
}

uint32 moh_native_cd_stop(void)
{
    /* Synchronous native reads have no CD interrupt pipeline to stop */
    w_u32(0x800345D4u, 0u);
    w_u32(0x80035044u, 0u);
    w_u32(0x80035048u, 0u);
    return moh_native_cd_reset();
}

uint32 sub_80022D34(void)
{
    w_u32(0x800345D4u, 0u);
    sub_80027A18();
    w_u32(0x80035044u, 0u);
    w_u32(0x80035048u, 0u);
    return sub_8001CB3C(0u, 0u);
}

uint32 sub_80018794(uint32 a0)
{
    uint32 target;
    w_u32(r_u32(0x8002E020u), 0x04000002u);
    w_u32(r_u32(0x8002E024u), a0);
    w_u32(r_u32(0x8002E028u), 0u);
    target = r_u32(0x8002E02Cu);
    w_u32(target, 0x01000401u);
    return target;
}

uint32 sub_8001AEE8(uint32 a0)
{
    uint32 minute = r_u8(a0), second = r_u8(a0 + 1u), frame = r_u8(a0 + 2u);
    return 75u * (60u * (10u * (minute >> 4) + (minute & 15u))
        + 10u * (second >> 4) + (second & 15u))
        + 10u * (frame >> 4) + (frame & 15u) - 150u;
}

uint32 sub_8001F740(uint32 first_high_product, uint32 first_dividend, uint32 second_high_product, uint32 second_dividend)
{
    /* TODO Callers must recover these nonstandard incoming carriers */
    uint32 left = (uint32)((sint32)(first_high_product + first_dividend) >> 6) - (uint32)((sint32)first_dividend >> 31);
    uint32 right = (uint32)((sint32)(second_high_product + second_dividend) >> 6) - (uint32)((sint32)second_dividend >> 31);
    w_u32(0x800D1590u, 6u);
    w_u16(0x800D1598u, (uint16)left);
    w_u16(0x800D159Au, (uint16)right);
    return moh_draft_missing_call(0x80028F40u, 0x800D1590u, second_high_product, (uint32)((sint32)second_dividend >> 31));
}

void sub_80020138(void) { sub_8001FF08(0u); }

void sub_8001F17C(void)
{
    sub_8001AF88();
    if (r_u32(0x80035064u) == 1u)
    {
        sub_8001D0F0(0u);
        sub_8001D1B0(0u);
    }
    else
    {
        moh_draft_missing_call(0x80028B60u, 0u);
        moh_draft_missing_call(0x80028B90u, 0u);
    }
    w_u8(r_u32(0x8002F37Cu), 0u);
    w_u8(r_u32(0x8002F388u), 0u);
    sub_8001AD38();
}

uint32 sub_80016CD4(uint32 a0)
{
    uint32 previous = r_u8(0x8002DF1Au);
    w_u8(0x8002DF1Au, (uint8)a0);
    if (a0 & 255u)
        moh_draft_indirect_call(r_u32(0x8002DF14u), 0x800147E8u,
            r_u8(0x8002DF1Au), r_u8(0x8002DF18u), r_u8(0x8002DF1Bu));
    return previous;
}

uint32 sub_80016DA0(uint32 a0)
{
    if (r_u8(0x8002DF1Au) >= 2u)
        moh_draft_indirect_call(r_u32(0x8002DF14u), 0x80014844u, a0);
    if (!a0) sub_800191F8(0x8002DF84u, 0xFFFFFFFFu, 20u);
    return moh_draft_indirect_call(r_u32(r_u32(0x8002DF10u) + 16u), a0 ? 0x03000000u : 0x03000001u);
}

uint32 sub_80016FBC(uint32 a0, uint32 a1, uint32 a2, uint32 a3)
{
    uint32 table, color;
    sub_80016EA0(0x80014890u, a0);
    color = ((a3 & 255u) << 16) | ((a2 & 255u) << 8) | (a1 & 255u);
    table = r_u32(0x8002DF10u);
    return moh_draft_indirect_call(r_u32(table + 8u), r_u32(table + 12u), a0, 8u, color);
}

uint32 sub_8001704C(uint32 a0, uint32 a1)
{
    uint32 table;
    sub_80016EA0(0x800148A8u, a0);
    table = r_u32(0x8002DF10u);
    return moh_draft_indirect_call(r_u32(table + 8u), r_u32(table + 32u), a0, 8u, a1);
}

uint32 sub_800172E0(uint32 ot, uint32 incoming_v0)
{
    uint32 table;
    /* TODO Recover the debug-level value carried in incoming v0 at callers */
    if (incoming_v0 >= 2u)
        moh_draft_indirect_call(r_u32(0x8002DF14u), 0x800148FCu, ot);
    table = r_u32(0x8002DF10u);
    return moh_draft_indirect_call(r_u32(table + 8u), r_u32(table + 24u), ot, 0u, 0u);
}

uint32 sub_8002178C(uint32 a0, uint32 a1)
{
    uint32 alignment, shifted;
    if (r_u32(0x800302E0u))
    {
        alignment = r_u32(0x800302E8u);
        if (!alignment)
        {
            /* TODO Map the original BREAK 7 exception */
            return moh_draft_missing_call(0x800217B8u);
        }
        if (a1 % alignment) a1 = (a1 + alignment) & ~r_u32(0x800302ECu);
    }
    shifted = a1 >> (r_u32(0x800302E4u) & 31u);
    if (a0 == 0xFFFFFFFEu) return a1;
    if (a0 == 0xFFFFFFFFu) return shifted & 0xFFFFu;
    w_u16(r_u32(0x800302BCu) + (a0 << 1), (uint16)shifted);
    return a1;
}

uint32 sub_80027A18(void)
{
    w_u8(r_u32(0x80035308u), 1u);
    while (r_u8(r_u32(0x8003530Cu)) & 7u)
    {
        w_u8(r_u32(0x80035308u), 1u);
        w_u8(r_u32(0x8003530Cu), 7u);
        w_u8(r_u32(0x80035318u), 7u);
    }
    w_u8(0x80035322u, 0u);
    w_u8(0x80035321u, r_u8(0x80035322u));
    w_u8(0x80035320u, 2u);
    w_u8(r_u32(0x80035308u), 0u);
    w_u8(r_u32(0x8003530Cu), 0u);
    w_u32(r_u32(0x80035310u), 4901u);
    return 4901u;
}

uint32 sub_80022BBC(void)
{
    sub_80027BDC();
    sub_80027AEC();
    w_u32(0x800D0A78u, 0u);
    w_u32(0x800D0A74u, r_u32(0x800D0A78u));
    w_u32(0x800D0A70u, 0u);
    sub_80022C54();
    moh_draft_missing_call(0x80019710u, 0u);
    w_u32(0x80035044u, 0x800233D4u);
    w_u32(0x80035048u, 0x80023A3Cu);
    sub_8001CB3C(0u, 0x8002307Cu);
    w_u32(0x80035064u, 1u);
    w_u32(0x800345D4u, 1u);
    return 1u;
}

uint32 sub_80022C54(void)
{
    uint32 i;
    w_u32(0x800345D4u, 0u);
    w_u8(0x800345D8u, 0u);
    for (i = 0; i < 4u; ++i) w_u8(0x800345DCu - i, 0u);
    w_u32(0x800345E0u, 0u);
    for (i = 0; i < 8u; ++i) w_u8(0x800345EBu - i, 0u);
    w_u32(0x800345F4u, 2u);
    w_u32(0x800345F8u, 14u);
    w_u8(0x800345ECu, 0u);
    w_u32(0x800345F0u, 0u);
    w_u32(0x800345FCu, 21u);
    w_u8(0x80034600u, 0u);
    w_u8(0x80034601u, 0u);
    sub_8001ADD8(0u, 0x80034602u);
    w_u8(0x80034606u, 0u);
    w_u8(0x80034607u, 0u);
    for (i = 0u; i < 4u; ++i) w_u8(0x80034608u + i, 0u);
    w_u32(0x8003460Cu, 0u);
    w_u32(0x80034610u, 1u);
    w_u32(0x80034614u, 0u);
    w_u32(0x8003461Cu, 0u);
    w_u32(0x80034618u, 0u);
    w_u32(0x80034620u, 0u);
    w_u8(0x80034624u, 0u);
    w_u8(0x80034625u, 0u);
    return 1u;
}

uint32 sub_80037BB4(uint32 a0, uint32 a1)
{
    uint32 left_size = sub_8001AC88(a0), size = sub_8001AC88(a1);
    if (left_size != size) return 0u;
    while (size >= 4u)
    {
        if (r_u32(a0) != r_u32(a1)) return 0u;
        a0 += 4u; a1 += 4u; size -= 4u;
    }
    while (size--)
    {
        if (r_u8(a0++) != r_u8(a1++)) return 0u;
    }
    return 1u;
}

uint32 sub_80023AD0(uint32 a0, uint32 a1)
{
    uint32 offset = 0u, byte;
    if ((a0 & 255u) != 5u)
    {
        offset = r_u32(0x800346A8u + (r_u8(0x800345D8u) << 2)) - 1u;
        if ((sint32)offset < 0) return a1 + offset;
    }
    byte = r_u8(a1 + offset);
    w_u8(0x80034608u, (uint8)(byte >> 7));
    w_u8(0x80034609u, (uint8)((byte >> 6) & 1u));
    w_u8(0x8003460Au, (uint8)((byte >> 5) & 1u));
    w_u8(0x8003460Bu, (uint8)((byte >> 1) & 1u));
    w_u8(0x800345ECu, (uint8)byte);
    sub_80023C60(0x800345E4u, a1);
    /* The copy loop returns zero; a null source leaves the status bit */
    return a1 ? 0u : ((byte >> 5) & 1u);
}

uint32 sub_80023A3C(uint32 a0, uint32 a1)
{
    uint32 result;
    sub_80023AD0(a0 & 255u, a1);
    if (r_u8(0x800345ECu) & 16u)
    {
        w_u32(0x800345F4u, 2u);
        w_u32(0x800345F8u, 12u);
    }
    result = r_u32(0x800D0A78u);
    if (result)
    {
        result = r_u32(0x800345D4u);
        if (result) return moh_draft_indirect_call(r_u32(0x800D0A78u), a0 & 255u, a1);
    }
    return result;
}

uint32 sub_800234E4(uint32 a0, uint32 a1)
{
    uint32 command = 0u, result, status;
    if ((a0 & 255u) != 2u)
    {
        status = r_u8(0x800345ECu);
        w_u32(0x800345F4u, (status & 16u) ? 2u : 1u);
        w_u32(0x800345F8u, (status & 16u) ? 12u : 11u);
    }
    else if ((command = r_u8(0x800345D8u)) == 14u)
    {
        if ((r_u8(0x80034601u) ^ r_u8(0x800345D9u)) & 128u)
        {
            w_u32(0x800345F8u, 15u); w_u32(0x800345F4u, 2u); w_u32(0x80034614u, 3u);
        }
        else { w_u32(0x800345F4u, 1u); w_u32(0x800345F8u, 11u); }
        w_u8(0x80034601u, r_u8(0x800345D9u));
    }
    else if (command == 3u || command == 6u || command == 27u)
    {
        w_u32(0x800345F8u, command == 3u ? 16u : 17u);
        w_u32(0x800345F4u, 2u); w_u8(0x80034607u, (uint8)command); w_u32(0x80034620u, 1200u);
    }
    else
    {
        command = r_u8(0x800345D8u);
        if (command == 2u)
        {
            uint32 bytes = r_u32(0x800345DCu) << 24;
            bytes |= r_u32(0x800345D8u) >> 8;
            w_masked_u32(0x80034604u, bytes >> 16, 3u);
            w_masked_u32(0x80034600u, bytes << 16, 12u);
        }
        if (command == 3u || command == 6u || command == 27u)
            w_u8(0x80034607u, r_u8(0x800345D8u));
        if (command == 21u || command == 22u) w_u8(0x80034606u, r_u8(0x800345D8u));
        w_u32(0x800345F4u, 1u); w_u32(0x800345F8u, 11u);
    }
    result = r_u32(0x800D0A74u);
    if (result)
    {
        result = r_u32(0x800345D4u);
        if (result) return moh_draft_indirect_call(r_u32(0x800D0A74u), a0 & 255u, a1);
    }
    return result;
}

uint32 sub_80022EA4(uint32 a0, uint32 a1)
{
    uint32 command, timeout;
    sub_80027A18();
    w_u8(0x800345D8u, (uint8)a0);
    if (a1)
    {
        sub_80023C18(0x800345D9u, a1);
        w_u32(0x800345E0u, 0x800345D9u);
    }
    else w_u32(0x800345E0u, 0u);
    command = r_u8(0x800345D8u);
    timeout = r_u32(0x80034628u + (command << 2));
    w_u32(0x80034618u, timeout ? 60u * timeout : 30u);
    w_u32(0x8003461Cu, 0u);
    if ((command == 8u && r_u8(0x8003460Bu) != 1u)
        || (command == 7u && r_u8(0x8003460Bu) == 1u)
        || (command == 9u && r_u8(0x80034624u) && r_u8(0x80034625u) == command))
        w_u8(0x800345D8u, 1u);
    if (r_u8(0x800345D8u) == 1u)
    {
        w_u32(0x800345E0u, 0u);
        timeout = r_u32(0x8003462Cu);
        w_u32(0x80034618u, timeout ? 60u * timeout : 30u);
    }
    if (sub_80027584(r_u8(0x800345D8u), r_u32(0x800345E0u), 0u, 1u))
    {
        w_u32(0x8003461Cu, 0u); w_u32(0x80034618u, 0u);
        return 0u;
    }
    command = r_u8(0x800345D8u);
    w_u8(0x80034600u, (uint8)command);
    if (command != 1u) { w_u8(0x80034625u, (uint8)command); w_u8(0x80034624u, 0u); }
    return 1u;
}

uint32 sub_8002AD00(uint32 a0)
{
    /* Continued from the existing character-output draft without saved-register bookkeeping */
    uint32 result, count;
    if ((a0 & 255u) == 9u)
    {
        do { sub_8002AD00(32u); result = r_u32(0x800358ECu) & 7u; } while (result);
        return result;
    }
    if ((a0 & 255u) == 10u)
    {
        sub_8002AD00(13u);
        w_u32(0x800358ECu, 0u);
    }
    else if (r_u8(0x8002F29Du + (a0 & 255u)) & 0x97u)
        w_u32(0x800358ECu, r_u32(0x800358ECu) + 1u);
    count = r_u32(0x800358F0u);
    if ((sint32)count >= 32)
    {
        /* TODO Bind write with the original fd, guest buffer and byte count */
        moh_draft_missing_call(0x80028264u, 1u, 0x800D1798u, count);
        w_u32(0x800358F0u, 0u);
    }
    result = r_u32(0x800358F0u);
    w_u8(0x800D1798u + result, (uint8)a0);
    w_u32(0x800358F0u, ++result);
    return result;
}

uint32 sub_80016B60(uint32 mode)
{
    uint32 subtype = mode & 7u, graph_type, width, height;
    if (subtype == 0u || subtype == 3u)
        moh_draft_missing_call(0x80015B28u, 0x800147B4u, 0x8002DED0u, 0x8002DF18u);
    if (subtype == 0u || subtype == 3u || subtype == 5u)
    {
        sub_800191F8(0x8002DF18u, 0u, 128u);
        sub_8001CA78();
        moh_draft_missing_call(0x80022550u, r_u32(0x8002DF10u) & 0xFFFFFFu);
        w_u8(0x8002DF18u, sub_80018D40(mode));
        graph_type = r_u8(0x8002DF18u);
        w_u8(0x8002DF19u, 1u);
        width = r_u16(0x8002DF98u + 4u * graph_type);
        graph_type = r_u8(0x8002DF18u);
        w_u16(0x8002DF1Cu, width);
        height = r_u16(0x8002DFA4u + 4u * graph_type);
        w_u16(0x8002DF1Eu, height);
        sub_800191F8(0x8002DF28u, 0xFFFFFFFFu, 92u);
        sub_800191F8(0x8002DF84u, 0xFFFFFFFFu, 20u);
        return r_u8(0x8002DF18u);
    }
    if (r_u8(0x8002DF1Au) >= 2u)
        moh_draft_indirect_call(r_u32(0x8002DF14u), 0x800147D4u, mode);
    return moh_draft_indirect_call(r_u32(r_u32(0x8002DF10u) + 52u), 1u);
}

uint32 sub_80018AE0(void)
{
    uint32 tail, entry, argument0, argument1, target, status_port, target_index;
    if (r_u32(r_u32(0x8002E02Cu)) & 0x01000000u) return 1u;
    w_u32(0x8002E04Cu, sub_8001CBAC(0u));
    while (r_u32(0x8002E040u) != r_u32(0x8002E044u)
        && !(r_u32(r_u32(0x8002E02Cu)) & 0x01000000u))
    {
        tail = r_u32(0x8002E044u);
        if (((tail + 1u) & 63u) == r_u32(0x8002E040u) && !r_u32(0x8002DF24u))
            sub_8001CAD8(2u, 0u);
        status_port = r_u32(0x8002E020u);
        while (!(r_u32(status_port) & 0x04000000u)) {}
        target_index = r_u32(0x8002E044u);
        entry = 0x800CA304u + 96u * r_u32(0x8002E044u);
        argument0 = r_u32(entry + 4u);
        argument1 = r_u32(0x800CA30Cu + 96u * r_u32(0x8002E044u));
        target = r_u32(0x800CA304u + 96u * target_index);
        /* TODO Resolve the queue callback targets */
        moh_draft_indirect_call(target, argument0, argument1);
        w_u32(0x8002E044u, (r_u32(0x8002E044u) + 1u) & 63u);
    }
    sub_8001CBAC(r_u32(0x8002E04Cu));
    if (r_u32(0x8002E040u) == r_u32(0x8002E044u)
        && !(r_u32(r_u32(0x8002E02Cu)) & 0x01000000u)
        && r_u32(0x8002DF20u) && r_u32(0x8002DF24u))
    {
        target = r_u32(0x8002DF24u);
        w_u32(0x8002DF20u, 0u);
        moh_draft_indirect_call(target);
    }
    return (r_u32(0x8002E040u) - r_u32(0x8002E044u)) & 63u;
}

uint32 sub_80018E7C(uint32 a0)
{
    uint32 pending;
    if (a0)
    {
        pending = (r_u32(0x8002E040u) - r_u32(0x8002E044u)) & 63u;
        if (pending) sub_80018AE0();
        if ((r_u32(r_u32(0x8002E02Cu)) & 0x01000000u)
            || !(r_u32(r_u32(0x8002E020u)) & 0x04000000u))
            return pending ? pending : 1u;
        return pending;
    }
    sub_80018FB8();
    while (r_u32(0x8002E040u) != r_u32(0x8002E044u))
    {
        sub_80018AE0();
        if (sub_80018FEC()) return 0xFFFFFFFFu;
    }
    for (;;)
    {
        if (!(r_u32(r_u32(0x8002E02Cu)) & 0x01000000u)
            && (r_u32(r_u32(0x8002E020u)) & 0x04000000u)) return 0u;
        if (sub_80018FEC()) return 0xFFFFFFFFu;
    }
}

uint32 sub_80017F6C(uint32 a0, uint32 a1)
{
    uint32 target = r_u32(0x8002E03Cu);
    w_u32(target, r_u32(target) | 0x08000000u);
    w_u32(r_u32(0x8002E038u), 0u);
    w_u32(r_u32(0x8002E030u), a0 + (a1 << 2) - 4u);
    w_u32(r_u32(0x8002E034u), a1);
    w_u32(r_u32(0x8002E038u), 0x11000002u);
    sub_80018FB8();
    while (r_u32(r_u32(0x8002E038u)) & 0x01000000u)
        if (sub_80018FEC()) return 0xFFFFFFFFu;
    return a1;
}

uint32 sub_80025464(uint32 a0)
{
    uint32 skip = 0u, output;
    if (r_u32(0x80034780u) && (r_u8(r_u32(a0 + 60u)) >> 4) == 8u)
        skip = r_u8(a0 + 55u) == 0u;
    w_u32(0x800347B8u, skip);
    if (!skip) moh_draft_indirect_call(r_u32(0x80034740u), a0);
    output = moh_draft_indirect_call(r_u32(0x8003473Cu), a0, r_u32(0x800347B8u));
    output = sub_8002454C(a0, output & 255u);
    if (output != 90u && output && (sint32)output >= 0) return 0xFFFFFFFCu;
    return output;
}

uint32 sub_80023D74(void)
{
    uint32 port = r_u32(0x800347ACu), current;
    if (r_u16(port + 10u) & 2u) { w_u16(port + 10u, 0u); return 0u; }
    w_u32(0x800347A4u, 1u);
    if (r_u32(0x80034784u) && (sint32)r_u32(0x800D0A9Cu) < 150)
        w_u32(0x800D0A9Cu, r_u32(0x800D0A9Cu) + 1u);
    if (!r_u32(0x80034788u) && (sint32)r_u32(0x800D0AA0u) < 150)
        w_u32(0x800D0AA0u, r_u32(0x800D0AA0u) + 1u);
    if (r_u32(0x8003476Cu) && (sint32)r_u32(0x80034788u) >= (sint32)r_u32(0x80034784u))
    {
        current = r_u32(0x80034784u);
        w_u32(0x80034778u, 0u); w_u32(0x80034774u, current);
        if (!sub_80023F04(r_u32(0x80034768u) + 240u * current))
            moh_draft_indirect_call(r_u32(0x80034734u), 0xFFFFu);
        w_u32(0x8003477Cu, 0u);
        while ((sint32)r_u32(0x80034788u) >= (sint32)r_u32(0x80034774u))
            sub_80024238(r_u32(0x80034768u) + 240u * r_u32(0x80034774u));
        w_u16(r_u32(0x800347ACu) + 14u, 136u);
    }
    return 0u;
}

uint32 sub_80036A70(uint32 status)
{
    uint32 result;
    if ((status & 255u) == 2u)
    {
        result = moh_draft_missing_call(0x80019640u, r_u32(0x800385A0u));
        w_u32(0x80038598u, 0u);
        /* TODO Original v0 is reloaded with the global's high address after the callback */
        return 0x80040000u;
    }
    result = moh_draft_missing_call(0x80019220u, 0x8003859Cu, r_u32(0x80038594u), 0x80037D90u, 0x80u);
    if ((sint32)result <= 0) moh_draft_missing_call(0x80015B28u, 0x80035F64u);
    return moh_draft_missing_call(0x80015B28u, 0x80035F44u);
}

uint32 sub_8002454C(uint32 a0, uint32 a1)
{
    uint32 control = 136u, received, timer, elapsed, index, port;
    uint32 status_port, timer_base, timer_limit;
    if ((r_u8(r_u32(a0 + 60u)) >> 4) == 8u && r_u8(a0 + 68u) >= 9u) control = 34u;
    port = r_u32(0x800347ACu);
    while (!(r_u16(port + 4u) & 2u)) {}
    sub_800251D8(400u);
    port = r_u32(0x800347ACu);
    received = r_u8(port);
    w_u16(port + 14u, (uint16)((!r_u8(a0 + 68u) && (received >> 4) == 8u) ? 34u : control));
    status_port = r_u32(0x800347A8u);
    if (!(r_u32(status_port) & 128u))
    {
        timer_base = r_u32(0x800D0A90u);
        timer_limit = r_u32(0x800D0A94u);
        do
        {
            r_u16(0x1F801124u);
            timer = r_u16(0x1F801120u);
            if ((sint32)timer < (sint32)timer_base)
            {
                if (r_u16(0x1F801128u)) timer += r_u16(0x1F801128u);
                else timer += 0x10000u;
            }
            index = r_u16(0x1F801124u);
            elapsed = timer - timer_base;
            if (!(index & 512u)) elapsed = (uint32)((sint32)elapsed >> 3);
            if ((sint32)elapsed >= (sint32)timer_limit) return 0xFFFFFFFEu;
        } while (!(r_u32(status_port) & 128u));
    }
    if (r_u8(a0 + 232u) != 8u && r_u32(0x80034778u) == 2u)
    {
        sub_800251D8(60u);
        while (!sub_800251F8()) {}
    }
    w_u8(r_u32(0x800347ACu), (uint8)a1);
    if (r_u32(0x80034778u) == 3u && received == 128u)
    {
        port = r_u32(0x800347ACu);
        w_u32(r_u32(0x800347A8u), 0xFFFFFF7Fu);
        w_u16(port + 10u, r_u16(port + 10u) | 16u);
    }
    timer = r_u8(a0 + 69u);
    index = r_u8(a0 + 68u);
    w_u8(a0 + 69u, (uint8)(timer + 1u));
    if (index != 255u) w_u8(r_u32(a0 + 60u) + r_u8(a0 + 68u), (uint8)received);
    w_u8(a0 + 68u, (uint8)(r_u8(a0 + 68u) + 1u));
    return received;
}

void sub_8001FF08(uint32 a0)
{
    uint32 mode, counter = 0xF2000002u, reload = 17640u, callback, use_vblank;
    sint32 delay = 998;
    while (delay-- >= 0) {}
    mode = r_u32(0x8002FF5Cu);
    w_u8(0x8002FF6Eu, 6u); w_u8(0x8002FF6Cu, 0u); w_u8(0x8002FF6Du, 0u);
    w_u32(0x8002FF68u, 0u);
    if (mode == 0u) { w_u8(0x8002FF6Eu, 127u); return; }
    if (mode == 3u) reload = 0x89D0u;
    else if (mode == 5u)
    {
        w_u8(0x8002FF6Eu, 0u);
        if (a0) { counter = 0xF2000003u; reload = 1u; }
        else w_u8(0x8002FF6Cu, 1u);
    }
    else if (mode != 2u)
    {
        if (r_u32(0x8002FF60u)) return;
        mode = r_u32(0x8002FF5Cu);
        if (!mode) moh_draft_fail_fast("Original sequencer division by zero", 0x80020008u);
        /* The full MIPS divides by the requested frequency, unlike the truncated IDA formula */
        if ((sint32)mode < 70)
        {
            reload = (uint32)(2116800 / (sint32)mode);
            w_u8(0x8002FF6Du, (uint8)(r_u8(0x8002FF6Du) + 1u));
        }
        else reload = (uint32)(4233600 / (sint32)mode);
    }
    use_vblank = r_u8(0x8002FF6Cu);
    sub_8001AF88();
    if (use_vblank) sub_8001CB08(r_u32(0x8002FF64u));
    else
    {
        moh_draft_missing_call(0x8001AB90u, counter);
        moh_draft_missing_call(0x8001AA90u, counter, reload & 0xFFFFu, 4096u);
        if (!r_u8(0x8002FF6Eu))
        {
            callback = sub_8001CAA8(0u, 0u);
            w_u32(0x8002FF68u, callback);
            callback = 0x80020158u;
        }
        else callback = r_u8(0x8002FF6Du) ? 0x800201A4u : r_u32(0x8002FF64u);
        sub_8001CAA8((uint32)(sint32)(sint8)r_u8(0x8002FF6Eu), callback);
    }
    sub_8001AD38();
}

uint32 sub_800362E8(uint32 a0, uint32 a1, uint32 a2)
{
    uint32 input = a1, output, byte;
    /* TODO Local buffer and string SDK adapters are deferred */
    moh_draft_missing_call(0x8001AD48u, a0, 0x80035D20u);
    if (a2) moh_draft_missing_call(0x8001ABC8u, a0, 0x80035D24u);
    if (r_u8(input) != 92u) moh_draft_missing_call(0x8001ABC8u, a0, 0x80035D2Cu);
    output = a0 + sub_8001AC88(a0);
    while ((byte = r_u8(input)) != 0u)
    {
        w_u8(output++, (uint8)(byte < 97u ? byte : byte - 32u));
        ++input;
        if ((sint32)(input - a1) >= 126)
            moh_draft_missing_call(0x80015B28u, 0x80035D30u, a1, 128u);
    }
    w_u8(output, 0u);
    moh_draft_missing_call(0x8001ABC8u, a0, 0x80035D6Cu);
    return a0;
}

uint32 sub_800363FC(uint32 filename, uint32 size_address, uint32 destination)
{
    uint8 name_storage[128];
    uint32 position_storage[2];
    uint32 name, position, attempts = 1u, result, file_size = 0u, allocation = 0u;
    uint32 first_sector, sectors, copied;
    /* TODO Bind native addressable locals for filename conversion and directory lookup */
    name = moh_draft_local_address(name_storage, sizeof(name_storage));
    position = moh_draft_local_address(position_storage, sizeof(position_storage));
    sub_800362E8(name, filename, 0u);
    for (;;)
    {
        result = sub_800374EC(position, name);
        if (result + 1u >= 2u) break;
        moh_draft_missing_call(0x80015B28u, 0x80035D70u, name, attempts);
        moh_draft_missing_call(0x80015B28u, 0x80035D98u, result);
        moh_native_cd_reset();
        moh_native_draw_sync(0u);
        moh_draft_missing_call(0x80015DB8u, 0u);
        moh_draft_missing_call(0x80015DB8u, 0u);
        moh_draft_missing_call(0x80015DB8u, 0u);
        if (attempts++ < 32u) continue;
        moh_draft_missing_call(0x80015B28u, 0x80035DA4u, name, attempts);
        goto finish;
    }
    sub_8001ADD8(r_u32(position), 0x8003859Cu);
    file_size = r_u32(position + 4u);
    allocation = destination ? destination : sub_80036BF0(file_size + 1u);
    if (!allocation) goto finish;
    first_sector = r_u32(position);
    w_u32(0x80038590u, allocation);
    /* The original alternate buffered branch is unreachable with its zero-initialized carried count */
    w_u32(0x80038594u, (uint32)((sint32)file_size >> 11));
    if (r_u32(0x80038594u))
    {
        w_u32(0x80038598u, 1u);
        w_u32(0x800385A0u, moh_draft_missing_call(0x80019640u, 0x800369ECu));
        while ((sint32)moh_draft_missing_call(0x80019220u, 0x8003859Cu,
            r_u32(0x80038594u), r_u32(0x80038590u), 128u) <= 0)
        {
            moh_native_cd_reset();
            moh_draft_missing_call(0x80015DB8u, 0u);
            moh_draft_missing_call(0x80015DB8u, 0u);
            moh_draft_missing_call(0x80019640u, 0x800369ECu);
            moh_draft_missing_call(0x80015B28u, 0x80035DCCu);
        }
        while (r_u32(0x80038598u)) {}
        sectors = r_u32(0x80038594u);
        copied = sectors << 11;
        sub_8001ADD8(first_sector + r_u32(0x80038594u), 0x8003859Cu);
        if (file_size != copied)
        {
            w_u32(0x80038598u, 1u);
            w_u32(0x800385A0u, moh_draft_missing_call(0x80019640u, 0x80036A70u));
            while ((sint32)moh_draft_missing_call(0x80019220u, 0x8003859Cu, 1u, 0x80037D90u, 128u) <= 0)
            {
                moh_draft_missing_call(0x80015B28u, 0x80035E00u);
                moh_native_cd_reset();
                moh_draft_missing_call(0x80015DB8u, 0u);
                moh_draft_missing_call(0x80015DB8u, 0u);
                moh_draft_missing_call(0x80019640u, 0x80036A70u);
            }
            while (r_u32(0x80038598u)) {}
            result = r_u32(0x80038590u) + copied;
            w_u32(0x80038590u, result);
            moh_draft_missing_call(0x800154ECu, result, 0x80037D90u, file_size - copied);
        }
    }
    else
    {
        w_u32(0x80038598u, 1u);
        w_u32(0x800385A0u, moh_draft_missing_call(0x80019640u, 0x80036A70u));
        while ((sint32)moh_draft_missing_call(0x80019220u, 0x8003859Cu, 1u, 0x80037D90u, 128u) <= 0)
        {
            moh_draft_missing_call(0x80015B28u, 0x80035E34u);
            moh_native_cd_reset();
            moh_draft_missing_call(0x80015DB8u, 0u);
            moh_draft_missing_call(0x80015DB8u, 0u);
            moh_draft_missing_call(0x80019640u, 0x80036A70u);
        }
        while (r_u32(0x80038598u)) {}
        moh_draft_missing_call(0x800154ECu, r_u32(0x80038590u), 0x80037D90u, file_size);
    }
finish:
    if (allocation) w_u8(allocation + file_size, 0u);
    if (size_address) w_u32(size_address, file_size);
    return allocation;
}

uint32 sub_800374EC(uint32 a0, uint32 a1)
{
    uint8 component_storage[128];
    uint32 position_storage[2];
    uint32 component, position, directory = 0x80038E38u, records = 0x80039638u;
    uint32 hashes = 0x80039A38u, checksum = 0u, fresh = 0u, cursor = a1, level;
    uint32 name, record, hash, found, result, length, remaining, word, shift, cache_search;
    /* TODO Guest-local adapters and oversized component aliasing are unresolved */
    component = moh_draft_local_address(component_storage, sizeof(component_storage));
    position = moh_draft_local_address(position_storage, sizeof(position_storage));
    if (r_u8(a1) != 92u)
    {
        moh_draft_missing_call(0x80015B28u, 0x80036200u, a1);
        return 0u;
    }
    if (r_u32(0x80039D30u))
    {
        w_u32(position, r_u32(0x80039D48u));
        w_u32(position + 4u, r_u32(0x80039D4Cu));
    }
    else
    {
        sub_8003740C(position, position + 4u);
        w_u32(0x80039D30u, 1u);
        w_u32(0x80039D48u, r_u32(position));
        w_u32(0x80039D4Cu, r_u32(position + 4u));
    }
    for (level = 0u; ; ++level)
    {
        ++cursor;
        if (level >= 8u)
        {
            moh_draft_missing_call(0x80015B28u, 0x800362B4u, a1);
            return 0u;
        }
        word = component;
        w_u8(component, 0u);
        while (r_u8(cursor) != 92u && r_u8(cursor))
        {
            w_u8(word++, r_u8(cursor++));
            /* TODO Original 16-byte component buffer has no explicit bound */
        }
        w_u8(word, 0u);
        if (!r_u8(cursor))
        {
            found = 0u; name = directory; record = records; hash = hashes;
            if (r_u8(0x80039D44u) && !fresh)
            {
                while (name < r_u32(0x80039D34u))
                {
                    if (r_u32(hash) == checksum && sub_80037BB4(component, name)) { found = 1u; break; }
                    name += 16u; record += 8u; hash += 4u;
                }
                if (found)
                {
                    w_u32(a0, r_u32(record)); w_u32(a0 + 4u, r_u32(record + 4u));
                    return a0;
                }
            }
            result = sub_80037A0C(position, position + 4u, component);
            if (!result)
            {
                moh_draft_missing_call(0x80015B28u, 0x8003624Cu, a1);
                return 0u;
            }
            if (!r_u8(0x80039D44u) || fresh) w_u8(0x80039D44u, 1u);
            moh_draft_missing_call(0x80015B28u, 0x80036230u, a1);
            moh_draft_missing_call(0x80015B28u, 0x8003623Cu, r_u32(position), result);
            w_u32(a0 + 4u, result); w_u32(a0, r_u32(position));
            sub_80037C70(component, a0, checksum);
            return a0;
        }
        name = directory; record = records; hash = hashes; found = 0u;
        cache_search = r_u8(0x80039D44u) && !fresh;
        if (cache_search)
        {
            while (name < r_u32(0x80039D34u))
            {
                if (r_u32(hash) == checksum && sub_80037BB4(component, name)) { found = 1u; break; }
                name += 16u; record += 8u; hash += 4u;
            }
        }
        if (found)
        {
            length = sub_8001AC88(component);
            records = record; directory = name; hashes = hash;
            w_u32(position, r_u32(records)); w_u32(position + 4u, r_u32(records + 4u));
        }
        else
        {
            if (!sub_80037A0C(position, position + 4u, component))
            {
                moh_draft_missing_call(0x80015B28u, 0x8003627Cu, a1);
                return 0xFFFFFFFFu;
            }
            length = sub_8001AC88(component);
            if (!cache_search)
            {
                record = r_u32(0x80039D3Cu);
                w_u8(0x80039D44u, 1u);
            }
            fresh = 1u;
            w_u32(record, r_u32(position)); w_u32(record + 4u, r_u32(position + 4u));
            sub_80037C70(component, record, checksum);
        }
        remaining = length; word = component; shift = 0u;
        while ((sint32)remaining >= 4)
        {
            checksum += r_u32(word); word += 4u; remaining -= 4u;
        }
        while ((sint32)remaining > 0)
        {
            checksum += (uint32)r_u8(word++) << (shift & 31u);
            --remaining; shift += 8u;
        }
    }
}

uint32 sub_80029530(uint32 a0, uint32 a1, uint32 a2, uint32 a3)
{
    uint32 sizes[256];
    uint32 index = 16u, i, count, program, table, sample_table, active = 0u;
    uint32 capacity = 64u, total = 0u, aligned, allocated, accumulated, signature;
    sint32 requested = (sint32)(sint16)(a1 & 0xFFFFu);
    if (sub_80029448() == 1u) return 0xFFFFFFFFu;
    sub_80029420(1u);
    if (requested >= 16) goto no_slot;
    if (requested == -1)
    {
        for (i = 0u; i < 16u && r_u8(0x800D15C0u + i); ++i) {}
        if (i >= 16u) goto no_slot;
        index = i;
    }
    else
    {
        if (r_u8(0x800D15C0u + (uint32)requested)) goto no_slot;
        index = a1;
    }
    index = (uint32)(sint32)(sint16)(index & 0xFFFFu);
    w_u8(0x800D15C0u + index, 1u);
    w_u16(0x800D15D4u, (uint16)(r_u16(0x800D15D4u) + 1u));
    w_u32(0x800D14CCu + (index << 2), a0);
    signature = r_u32(a0);
    w_u32(0x800D15D8u, 0u);
    if ((signature >> 8) != 0x564142u) goto invalid_bank;
    if ((signature & 255u) == 112u && (sint32)r_u32(a0 + 4u) >= 5) capacity = 128u;
    w_u16(0x800D166Cu, (uint16)capacity);
    if (capacity < r_u16(a0 + 18u)) goto invalid_bank;
    program = a0 + 32u;
    w_u32(0x800D154Cu + (index << 2), program);
    table = program + (capacity << 4);
    for (i = 0u; i < capacity; ++i)
    {
        uint32 entry = program + (i << 4);
        count = r_u8(entry);
        w_u32(entry + 8u, active);
        if (count) ++active;
    }
    w_u32(0x800D150Cu + (index << 2), table);
    count = r_u8(a0 + 22u);
    sample_table = table + ((uint32)r_u16(a0 + 18u) << 9);
    for (i = 0u; i < 256u; ++i)
    {
        if (count >= i)
        {
            sizes[i] = (uint32)r_u16(sample_table) << ((sint32)r_u32(a0 + 4u) >= 5 ? 3u : 2u);
            total += sizes[i];
        }
        sample_table += 2u;
    }
    aligned = (total + 63u) & 0xFFFFFFC0u;
    /* TODO Resolve the allocation callback; a3 also receives the final sample-table cursor */
    allocated = moh_draft_indirect_call(a2, aligned, a3, index, sample_table);
    if (allocated == 0xFFFFFFFFu) return 0xFFFFFFFFu;
    if (allocated + aligned > 0x80000u) goto invalid_bank;
    w_u32(0x800D161Cu + (index << 2), allocated);
    accumulated = 0u;
    for (i = 0u; i <= count; ++i)
    {
        accumulated += sizes[i];
        w_u16(program + ((i >> 1) << 4) + ((i & 1u) ? 14u : 12u),
            (uint16)((allocated + accumulated) >> 3));
    }
    w_u32(0x800D15DCu + (index << 2), accumulated);
    w_u8(0x800D15C0u + index, 2u);
    return index;
invalid_bank:
    w_u8(0x800D15C0u + index, 0u);
    sub_80029420(0u);
    w_u16(0x800D15D4u, (uint16)(r_u16(0x800D15D4u) - 1u));
    return 0xFFFFFFFFu;
no_slot:
    sub_80029420(0u);
    return 0xFFFFFFFFu;
}

uint32 moh_draft_cd_timeout(void)
{
    uint32 now = moh_draft_missing_call(0x80015DB8u, 0xFFFFFFFFu);
    uint32 iterations, sync, ready, label, ready_name, command, command_name, sync_name;
    if ((sint32)r_u32(0x800D0D3Cu) >= (sint32)now)
    {
        iterations = r_u32(0x800D0D40u);
        w_u32(0x800D0D40u, iterations + 1u);
        if ((sint32)iterations <= 3932160) return 0u;
    }
    moh_draft_missing_call(0x800224B0u, 0x80015178u);
    sync = r_u8(0x80035320u);
    ready = r_u8(0x80035321u);
    label = r_u32(0x800D0D44u);
    ready_name = r_u32(0x800350E8u + 4u * ready);
    command = r_u8(0x80035061u);
    command_name = r_u32(0x80035068u + 4u * command);
    sync_name = r_u32(0x800350E8u + 4u * sync);
    /* Keep the fifth printf argument originally passed on the stack */
    moh_draft_missing_call(0x80015B28u, 0x80015188u, label, command_name, sync_name, ready_name);
    sub_80027A18();
    return 0xFFFFFFFFu;
}

void moh_draft_cd_dispatch(uint32 site)
{
    uint32 port, saved_bank, flags, callback;
    if (!sub_8001CB9C()) return;
    port = r_u32(0x80035308u);
    saved_bank = r_u8(port) & 3u;
    for (;;)
    {
        flags = sub_80026AD8();
        if (!flags) break;
        if (flags & 4u)
        {
            callback = r_u32(0x80035048u);
            if (callback) moh_draft_indirect_call(callback, r_u8(0x80035321u), 0x800D0D2Cu);
        }
        if (flags & 2u)
        {
            callback = r_u32(0x80035044u);
            if (callback) moh_draft_indirect_call(callback, r_u8(0x80035320u), 0x800D0D24u);
        }
    }
    w_u8(r_u32(0x80035308u), (uint8)saved_bank);
}

uint32 sub_8002703C(uint32 a0, uint32 a1)
{
    uint32 result, i;
    w_u32(0x800D0D3Cu, moh_draft_missing_call(0x80015DB8u, 0xFFFFFFFFu) + 960u);
    w_u32(0x800D0D40u, 0u);
    w_u32(0x800D0D44u, 0x80015200u);
    do
    {
        if (moh_draft_cd_timeout()) return 0xFFFFFFFFu;
        moh_draft_cd_dispatch(0x800271A4u);
        result = r_u8(0x80035320u);
        if (result == 2u || result == 5u)
        {
            w_u8(0x80035320u, 2u);
            if (a1) for (i = 0u; i < 8u; ++i) w_u8(a1 + i, r_u8(0x800D0D24u + i));
            return result;
        }
    } while (!a0);
    return 0u;
}

uint32 sub_800272BC(uint32 a0, uint32 a1)
{
    uint32 result, source, i;
    w_u32(0x800D0D3Cu, moh_draft_missing_call(0x80015DB8u, 0xFFFFFFFFu) + 960u);
    w_u32(0x800D0D40u, 0u);
    w_u32(0x800D0D44u, 0x80015208u);
    do
    {
        if (moh_draft_cd_timeout()) return 0xFFFFFFFFu;
        moh_draft_cd_dispatch(0x80027424u);
        result = r_u8(0x80035322u);
        if (result)
        {
            w_u8(0x80035322u, 0u);
            source = 0x800D0D34u;
        }
        else
        {
            result = r_u8(0x80035321u);
            if (!result) continue;
            w_u8(0x80035321u, 0u);
            source = 0x800D0D2Cu;
        }
        if (a1) for (i = 0u; i < 8u; ++i) w_u8(a1 + i, r_u8(source + i));
        return result;
    } while (!a0);
    return 0u;
}
