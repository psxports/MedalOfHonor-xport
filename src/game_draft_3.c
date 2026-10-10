#include "game_draft_signatures.h"
#include "psx.h"

/* Unverified C draft; integration is deferred */

uint32 sub_800191F8(uint32 a1, uint32 a2, uint32 a3)
{
    uint32 remaining = a3 - 1u;
    if (a3) do { w_u8(a1++, a2); --remaining; } while (remaining != 0xFFFFFFFFu);
    return remaining;
}
uint32 sub_80018738(uint32 a1) { uint32 p = r_u32(0x8002E020u); w_u32(p, a1); return p; }
uint32 sub_800187DC(uint32 a1)
{
    w_u32(r_u32(0x8002E020u), a1 | 0x10000000u);
    return r_u32(r_u32(0x8002E01Cu)) & 0xFFFFFFu;
}
uint32 sub_8001CB9C(void) { return r_u16(0x8002E1D2u); }
uint32 sub_80022560(void) { return r_u32(0x8003458Cu); }
void sub_80022E14(uint32 a1) { w_u32(0x800D0A7Cu, a1); }
uint32 sub_80026AB8(uint32 a1) { return a1 ? a1 ^ 0x80000000u : 0u; }
uint32 sub_80017EB8(uint32 a1, uint32 a2) { return ((a2 & 0x7FFu) << 11) | (a1 & 0x7FFu) | 0xE5000000u; }
uint32 sub_800201F8(void) { return moh_draft_missing_call(0x80029460u, 0u); }
uint32 sub_8001F6EC(uint32 a1) { sub_80027990(a1); return 1u; }
uint32 sub_80022B9C(void) { return moh_draft_missing_call(0x8002AF58u) == 0u; }
uint32 sub_800251D8(uint32 a1)
{
    uint32 value = r_u16(0x1F801120u);
    w_u32(0x800D0A94u, a1); w_u32(0x800D0A90u, value);
    return value;
}
uint32 sub_80036BF0(uint32 a1) { return sub_80036C14(a1, 0u, 0u); }
uint32 sub_8001D1B0(uint32 a1)
{
    uint32 old = r_u32(0x800D0A84u); w_u32(0x800D0A84u, a1); return old;
}
uint32 sub_80019740(uint32 a1)
{
    uint32 i;
    w_u32(a1, 0u); w_u8(a1 + 4u, 0u);
    for (i = 8u; i >= 5u; --i) w_u8(a1 + i, 0u);
    w_u32(a1 + 12u, 0u); w_u32(a1 + 16u, 0u); w_u32(a1 + 20u, 0u);
    return a1 - 1u;
}
uint32 sub_8001CA78(void) { return moh_draft_indirect_call(r_u32(r_u32(0x8002F258u) + 12u)); }
uint32 sub_8001582C(void)
{
    SsInit();
    w_u32(0x800D094Cu, 60u);
    w_u32(0x800D0950u, 0u);
    w_u32(0x800D0148u, 0u);
    return 60u;
}
void sub_8002ADFC(void)
{
    uint32 n = r_u32(0x800358F0u);
    if ((sint32)n > 0) { moh_draft_missing_call(0x80028264u, 1u, 0x800D1798u, n); w_u32(0x800358F0u, 0u); }
}
uint32 sub_80021748(uint32 a1, uint32 a2, uint32 a3)
{
    uint32 p = 2u * a1 + r_u32(0x800302BCu);
    w_u16(p, a3 ? a2 >> (r_u32(0x800302E4u) & 31u) : a2); return p;
}
void sub_80023C18(uint32 a1, uint32 a2)
{
    uint32 i;
    if (a1) { if (a2) for (i = 0u; i < 4u; ++i) w_u8(a1 + i, r_u8(a2 + i)); else w_u8(a1, 0u); }
}
uint32 sub_80025298(uint32 a1)
{
    uint32 result = moh_draft_indirect_call(r_u32(0x8003474Cu), a1);
    w_u32(0x800347B4u, result); w_u8(r_u32(a1 + 60u), 0u);
    return sub_80024328(a1, 0xFFFFFFFEu);
}
uint32 sub_8001575C(uint32 a1, uint32 a2, uint32 a3)
{
    uint32 size = a2 > 0x7EFF0u ? 0x7EFF0u : a2;
    sub_800216C4(a1, size, a3); if (!r_u32(0x800302F4u)) w_u32(0x800302F0u, 0u); return size;
}
uint32 sub_8001F67C(uint32 a1, uint32 a2, uint32 a3)
{
    uint32 handle = moh_draft_missing_call(0x8001A070u, a1 & 255u, a2, 0u, 0u), result;
    if (!handle) return 0u;
    do { result = moh_draft_missing_call(0x8001A658u, handle, a3) & 255u; } while (!result);
    return result == 2u;
}
uint32 sub_80016E38(uint32 a1)
{
    return (uint32)DrawSync((sint32)a1);
}
uint32 sub_80022D74(uint32 a1)
{
    if ((sint32)r_u32(0x80034618u) <= 0 && r_u32(0x800345D4u) && r_u32(0x800345F4u) == 1u)
    {
        w_u32(0x800345F0u, 31u); w_u32(0x800345F4u, 2u); w_u32(0x800345F8u, 11u);
        /* TODO Caller keeps an unspecified a1 carrier for the command parameter */
        return sub_80022EA4(a1 & 255u, moh_draft_missing_call(0x80022DCCu));
    }
    return 0u;
}
uint32 sub_80017ED4(uint32 a1)
{
    if (!a1) return 0u;
    return ((r_u8(a1 + 2u) >> 3) << 15) | ((r_u8(a1) >> 3) << 10) | 0xE2000000u
        | ((((0u - (uint32)(sint32)(short)r_u16(a1 + 6u)) & 255u) >> 3) << 5)
        | (((0u - (uint32)(sint32)(short)r_u16(a1 + 4u)) & 255u) >> 3);
}
uint32 sub_80027990(uint32 a1)
{
    w_u8(r_u32(0x80035308u), 2u); w_u8(r_u32(0x80035318u), r_u8(a1));
    w_u8(r_u32(0x8003530Cu), r_u8(a1 + 1u)); w_u8(r_u32(0x80035308u), 3u);
    w_u8(r_u32(0x80035314u), r_u8(a1 + 2u)); w_u8(r_u32(0x80035318u), r_u8(a1 + 3u));
    w_u8(r_u32(0x8003530Cu), 32u); return 0u;
}
uint32 sub_8002A8F0(uint32 a1)
{
    uint32 value = a1 << (r_u32(0x800302E4u) & 31u), p = r_u32(0x800358E4u);
    if (!p) return 0u;
    for (;; p += 8u)
    {
        uint32 flags = r_u32(p), start;
        if (flags & 0x80000000u) continue;
        start = flags & 0xFFFFFFFu;
        if (flags & 0x40000000u) return 0u;
        if (start >= value || value < start + r_u32(p + 4u)) return 1u;
    }
}
uint32 sub_800247C8(void)
{
    uint32 p = r_u32(0x800347ACu);
    w_u32(r_u32(0x800347A8u), 0xFFFFFF7Fu);
    if (r_u16(p + 4u) & 0x80u)
        for (;;) { if (sub_800251F8()) return 0u; if (!(r_u16(r_u32(0x800347ACu) + 4u) & 0x80u)) break; }
    p = r_u32(0x800347ACu); w_u16(p + 10u, r_u16(p + 10u) | 0x10u); return 1u;
}
uint32 sub_800251F8(void)
{
    uint32 value = r_u16(0x1F801120u), base = r_u32(0x800D0A90u), span;
    if (r_u16(0x1F801120u) < base) { span = r_u16(0x1F801128u); value = r_u16(0x1F801120u) + (span ? span : 0x10000u); }
    value -= base; if (!(r_u16(0x1F801124u) & 0x200u)) value = (uint32)((sint32)value >> 3);
    return (sint32)value >= (sint32)r_u32(0x800D0A94u);
}
uint32 sub_80017E20(uint32 a1, uint32 a2)
{
    sint32 x = (short)a1, y = (short)a2, width = (short)r_u16(0x8002DF1Cu), height = (short)r_u16(0x8002DF1Eu);
    if (x < 0) x = 0; else if (x > width - 1) x = (short)(width - 1);
    if (y < 0) y = 0; else if (y > height - 1) y = (short)(height - 1);
    return (((uint32)y & 0x3FFu) << 10) | ((uint32)x & 0x3FFu) | 0xE4000000u;
}
uint32 sub_80017234(uint32 a1, uint32 a2)
{
    /* TODO Original entry also receives its diagnostic level in v0 */
    moh_draft_indirect_call(r_u32(r_u32(0x8002DF10u) + 44u), a1, a2);
    w_u32(0x8002DFD8u, (0x8002DFC4u & 0xFFFFFFu) | 0x4000000u);
    w_u32(a1, 0x8002DFD8u & 0xFFFFFFu); return a1;
}
uint32 sub_8001B3E0(uint32 callback)
{
    /* TODO Callback is passed in the original v0 carrier */
    uint32 p = moh_draft_indirect_call(callback), code;
    if (r_u8(p + 55u) || r_u8(p + 56u) || (p != r_u32(p + 16u) && r_u8(p + 57u)) || r_u8(r_u32(p + 48u)))
    { code = r_u8(p + 73u); if (code == 2u || code == 3u) return 1u; if (code == 6u) return 4u; }
    return r_u8(p + 73u);
}

/* Prior GPU translation with guest register scaffolding removed */
uint32 sub_80018D40(uint32 a1)
{
    uint32 pointer, temporary, subtype = a1 & 7u, saved_mask = sub_8001CBAC(0u);
    w_u32(0x8002E044u, 0u); temporary = r_u32(0x8002E044u);
    w_u32(0x8002E050u, saved_mask); w_u32(0x8002E040u, temporary);
    if (subtype == 0u || subtype == 5u)
    {
        w_u32(r_u32(0x8002E02Cu), 0x401u); pointer = r_u32(0x8002E03Cu);
        w_u32(pointer, r_u32(pointer) | 0x800u); w_u32(r_u32(0x8002E020u), 0u);
        sub_800191F8(0x800CA304u, 0u, 0x1800u);
    }
    else if (subtype == 1u || subtype == 3u)
    {
        w_u32(r_u32(0x8002E02Cu), 0x401u); pointer = r_u32(0x8002E03Cu);
        w_u32(pointer, r_u32(pointer) | 0x800u);
        w_u32(r_u32(0x8002E020u), 0x2000000u); w_u32(r_u32(0x8002E020u), 0x1000000u);
    }
    sub_8001CBAC(r_u32(0x8002E050u)); return subtype == 0u ? sub_80019130(a1) : 0u;
}
/* Prior callback translation with guest register scaffolding removed */
uint32 sub_8001CE6C(uint32 a1, uint32 a2)
{
    uint32 slot = 0x8002E1D4u + (a1 << 2), old = r_u32(slot), pointer, active, bit;
    if (a2 != old && r_u16(0x8002E1D0u))
    {
        pointer = r_u32(0x8002F260u); active = r_u16(pointer); w_u16(pointer, 0u);
        bit = 1u << (a1 & 31u); w_u32(slot, a2);
        if (a2) { active |= bit; w_u16(0x8002E200u, r_u16(0x8002E200u) | bit); }
        else { active &= ~bit; w_u16(0x8002E200u, r_u16(0x8002E200u) & ~bit); }
        if (a1 == 0u) { moh_draft_missing_call(0x80022510u, a2 == 0u); moh_draft_missing_call(0x80022520u, 3u, a2 == 0u); }
        if (a1 == 4u) moh_draft_missing_call(0x80022520u, 0u, a2 == 0u);
        if (a1 == 5u) moh_draft_missing_call(0x80022520u, 1u, a2 == 0u);
        if (a1 == 6u) moh_draft_missing_call(0x80022520u, 2u, a2 == 0u);
        w_u16(r_u32(0x8002F260u), active);
    }
    return old;
}
/* Prior initialization translation with guest register scaffolding removed */
uint32 sub_8001CBC4(void)
{
    uint32 pointer, result;
    if (r_u16(0x8002E1D0u)) return 0u;
    pointer = r_u32(0x8002F260u); w_u16(pointer, 0u);
    w_u16(r_u32(0x8002F25Cu), r_u16(pointer)); w_u32(r_u32(0x8002F264u), 0x33333333u);
    sub_8001D0CC(0x8002E1D0u, 0x41Au);
    moh_native_irq_prepare(0x8002E208u, sub_8001CC9C);
    w_u32(0x8002E20Cu, 0x8002F1E8u); moh_draft_missing_call(0x800266F8u, 0x8002E208u);
    w_u16(0x8002E1D0u, 1u);
    result = moh_draft_missing_call(0x80022390u); w_u32(r_u32(0x8002F258u) + 20u, result);
    result = moh_draft_missing_call(0x80026708u); w_u32(r_u32(0x8002F258u) + 4u, result);
    moh_draft_missing_call(0x8001D1D8u); sub_8001AD38(); return 0x8002E1D0u;
}
/* Prior main translation reduced to its pseudocode structure */
void sub_80037060(void)
{
    uint32 module = 0u, record, i, state;
    moh_draft_missing_call(0x8001E07Cu); sub_8001CA78(); sub_80016B60(0u); sub_80016CD4(0u);
    for (;;)
    {
        sub_80036B20(r_u32(0x8002F27Cu), r_u32(0x8002F280u));
        moh_draft_missing_call(0x8001B228u, 0x80038DF0u, 0x80038E14u);
        moh_draft_missing_call(0x8001B4A8u);
        for (i = 0u; i < 5u; ++i) moh_draft_missing_call(0x80015DB8u, 0u);
        moh_draft_missing_call(0x80019E20u); sub_8001E0EC(1u); moh_draft_missing_call(0x80015DB8u, 0u);
        record = 0x800385A8u + 12u * module; sub_80037318(r_u32(record + 4u), 0x80039D90u);
        sub_8001AD98(); moh_draft_missing_call(0x80019F64u); sub_8001D180();
        moh_draft_missing_call(0x80015DB8u, 0u); sub_80036AF8(); moh_draft_missing_call(0x8001B574u);
        for (i = 0u; i < 4u; ++i) moh_draft_missing_call(0x80015DB8u, 0u);
        moh_draft_indirect_call(r_u32(record + 8u));
        sub_80016B60(3u); sub_80016E38(0u);
        for (i = 0u; i < 4u; ++i) moh_draft_missing_call(0x80015DB8u, 0u);
        sub_8001AF88();
        for (i = 0u; i < 4u; ++i) moh_draft_missing_call(0x8001AB5Cu, 0xF2000000u | i);
        sub_80016B60(1u); sub_8001D0F0(0u); moh_draft_missing_call(0x80019640u, 0u); sub_8001D1B0(0u);
        moh_draft_missing_call(0x8001D140u, 0u); moh_draft_missing_call(0x80016D40u, 0u); sub_8001CB08(0u);
        for (i = 0u; i < 10u; ++i) { sub_8001CAA8(0u, 0u); moh_draft_missing_call(0x80015B28u, 0x80039D14u, i); }
        moh_draft_missing_call(0x8001CB6Cu); sub_8001AD38(); sub_80016B60(3u);
        if (module) { module = 0u; continue; }
        module = 1u; if ((sint32)r_u32(0x80038660u) < 11) continue;
        module = 2u; state = r_u32(0x80039C74u); w_u32(0x80038664u, state);
        /* TODO Resolve the separate switch blocks outside this selected function set */
        if (state < 7u) moh_draft_indirect_call(r_u32(0x800361E0u + 4u * state));
    }
}

void sub_80036E44(uint32 a1)
{
    uint32 block = a1 - 16u, size = r_u32(block), checksum, next, previous, end, flags, diagnostic;
    /* Preserve native interrupt state while updating the game heap */
    uint32 status = (uint32)xport_bios_enter_critical();
    flags = r_u32(block + 8u);
    checksum = size + r_u32(block + 4u) + flags + r_u32(block + 12u);
    if (checksum != 0xB562439Fu) { diagnostic = 0x80036100u; goto error; }
    end = block + (size << 4);
    if (r_u32(end) + r_u32(end + 4u) + r_u32(end + 8u) + r_u32(end + 12u) != checksum)
    { diagnostic = 0x80036140u; goto error; }
    if ((sint32)flags >= 0) { diagnostic = 0x80036174u; goto error; }
    previous = r_u32(0x80039C40u);
    w_u32(block + 8u, 0u);
    size = r_u32(block);
    w_u32(0x80039D88u, r_u32(0x80039D88u) + size);
    w_u32(0x80039D78u, r_u32(0x80039D78u) - size);
    next = r_u32(previous + 4u);
    if (previous != next)
    {
        for (;;)
        {
            if ((previous < block && block < next) || (previous >= next && (previous < block || block < next))) break;
            previous = next; next = r_u32(previous + 4u);
        }
    }
    size = r_u32(block);
    if (block + (size << 4) == next)
    {
        w_u32(block, size + r_u32(next));
        w_u32(block + 4u, r_u32(next + 4u));
        if (previous == next) previous = block;
    }
    else w_u32(block + 4u, next);
    size = r_u32(previous);
    if (previous + (size << 4) == block)
    {
        w_u32(previous, size + r_u32(block));
        w_u32(previous + 4u, r_u32(block + 4u)); block = previous;
    }
    else
    {
        flags = r_u32(previous + 8u); w_u32(previous + 4u, block);
        w_u32(previous + 12u, 0xB562439Fu - (size + block + flags));
    }
    w_u32(0x80039C40u, previous);
    w_u32(block + 12u, 0xB562439Fu - (r_u32(block) + r_u32(block + 4u) + r_u32(block + 8u)));
    if (status) xport_bios_exit_critical();
    return;
error:
    if (status) xport_bios_exit_critical();
    moh_draft_missing_call(0x80015B28u, 0x80039C50u, diagnostic);
}
