#include "game_shell_signatures.h"
#include "psx.h"
#include "xport_trace.h"

uint32 sub_800269C8(void);

uint32 sub_8001AB5C(uint32 counter)
{
    uint32 control, mask, value;
    FUNCTION_MARKER(0x8001AB5Cu, "SLUS_009.74");
    control = r_u32(0x8002E128u);
    mask = r_u32(0x8002E130u + (counter & 0xFFFFu) * 4u);
    value = r_u32(control + 4u);
    w_u32(control + 4u, value & ~mask);
    return 1u;
}

uint32 sub_8001D140(uint32 callback)
{
    uint32 previous;
    FUNCTION_MARKER(0x8001D140u, "SLUS_009.74");
    previous = r_u32(0x800D0A80u);
    w_u32(0x800D0A80u, callback);
    return previous;
}

uint32 sub_80016D40(uint32 callback)
{
    uint32 previous;
    FUNCTION_MARKER(0x80016D40u, "SLUS_009.74");
    if (r_u8(0x8002DF1Au) >= 2u)
        moh_draft_indirect_call(r_u32(0x8002DF14u), 0x80014828u, callback);
    previous = r_u32(0x8002DF24u);
    w_u32(0x8002DF24u, callback);
    return previous;
}

uint32 sub_8001CB6C(void)
{
    FUNCTION_MARKER(0x8001CB6Cu, "SLUS_009.74");
    return moh_draft_indirect_call(r_u32(r_u32(0x8002F258u) + 16u));
}

uint32 sub_8001D160(void)
{
    FUNCTION_MARKER(0x8001D160u, "SLUS_009.74");
    return sub_800269C8();
}

void moh_native_spu_quit(void)
{
    /* Native SPU teardown replaces BIOS interrupt and event teardown */
    SpuQuit();
    if (r_u32(0x800303BCu) == 1u) {
        w_u32(0x800303BCu, 0u);
        w_u32(0x800302F4u, 0u);
        w_u32(0x800302F8u, 0u);
        /* TODO Native teardown owns original 2ACD0, 2B058 and 1D1A0 event effects */
    }
}

uint32 sub_8001CFB4(void)
{
    uint32 interrupt_mask, interrupt_status, dma_control;
    FUNCTION_MARKER(0x8001CFB4u, "SLUS_009.74");
    if (!r_u16(0x8002E1D0u)) return 0u;
    /* TODO Raw callback teardown requires a native interrupt adapter */
    moh_draft_missing_call(0x8001AF88u);
    interrupt_mask = r_u32(0x8002F260u);
    dma_control = r_u32(0x8002F264u);
    w_u16(0x8002E202u, r_u16(interrupt_mask));
    interrupt_status = r_u32(0x8002F25Cu);
    w_u32(0x8002E204u, r_u32(dma_control));
    w_u16(interrupt_mask, 0u);
    w_u16(interrupt_status, r_u16(interrupt_mask));
    dma_control = r_u32(0x8002F264u);
    w_u32(dma_control, r_u32(dma_control) & 0x77777777u);
    moh_draft_missing_call(0x800269B8u);
    w_u16(0x8002E1D0u, 0u);
    return 0x8002E1D0u;
}

uint32 sub_8001AF68(void)
{
    FUNCTION_MARKER(0x8001AF68u, "SLUS_009.74");
    return moh_draft_missing_call(0x80021B80u, 1u);
}

uint32 sub_80016D30(void)
{
    FUNCTION_MARKER(0x80016D30u, "SLUS_009.74");
    return r_u8(0x8002DF1Au);
}

uint32 sub_8002666C(uint32 callback)
{
    FUNCTION_MARKER(0x8002666Cu, "SLUS_009.74");
    return moh_draft_missing_call(0x8001CAA8u, 9u, callback);
}

uint32 sub_80022E54(void)
{
    FUNCTION_MARKER(0x80022E54u, "SLUS_009.74");
    return r_u8(0x80034607u);
}

void sub_8001DFB0(uint32 allocation)
{
    FUNCTION_MARKER(0x8001DFB0u, "SLUS_009.74");
    if (allocation) sub_80036E44(allocation);
}

uint32 sub_800269C8(void)
{
    FUNCTION_MARKER(0x800269C8u, "SLUS_009.74");
    if (r_u32(0x800303BCu) != 1u) return 1u;
    w_u32(0x800303BCu, 0u);
    moh_draft_missing_call(0x8001AF88u);
    w_u32(0x800302F4u, 0u);
    w_u32(0x800302F8u, 0u);
    sub_8002ACD0(0u);
    moh_draft_missing_call(0x8002B058u, r_u32(0x80030354u));
    moh_draft_missing_call(0x8001D1A0u, r_u32(0x80030354u));
    return moh_draft_missing_call(0x8001AD38u);
}
