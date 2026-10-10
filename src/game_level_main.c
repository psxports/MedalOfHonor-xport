#include "game_level_signatures.h"
#include "xport_trace.h"
#include "psx.h"

uint32 level_sub_8008B3D4(void)
{
    FUNCTION_MARKER(0x8008B3D4u, "LEVEL.BIN");
    return 1u;
}

void level_sub_8006B894(void)
{
    FUNCTION_MARKER(0x8006B894u, "LEVEL.BIN");
}

uint32 level_sub_80064374(void)
{
    PSX_RECT rectangle = {142, 207, 60, 6};
    uint32 index;
    FUNCTION_MARKER(0x80064374u, "LEVEL.BIN");
    if (r_u32(0x800386A4u) & 8u) w_u32(0x800AA174u, 1u);
    moh_draft_missing_call(0x80015B28u, 0x8003C5E8u, 20u);
    moh_draft_missing_call(0x8001D1D8u);
    moh_draft_missing_call(0x8001CA78u);
    for (index = 0u; index < 4u; ++index)
        moh_draft_missing_call(0x8001AB2Cu, 0xF2000000u | index);
    level_sub_8004A634();
    level_sub_80061598();
    level_sub_8004943C();
    ClearImage(&rectangle, r_u8(0x80039D27u), r_u8(0x80039D28u), r_u8(0x80039D29u));
    moh_draft_missing_call(0x8001CB08u, 0x80090D38u);
    while (!(r_u32(0x8009C840u) & 32u)) {
        level_sub_800502B4(0x8009A620u, r_u32(0x8009A5C8u), r_u32(0x8009A5CCu), 400u);
        w_u32(0x8009A618u, (r_u32(0x800386A4u) & 2u) != 0u);
        level_sub_800640B4();
    }
    moh_draft_missing_call(0x8001CB08u, 0u);
    return 0u;
}

void level_sub_80087078(void)
{
    FUNCTION_MARKER(0x80087078u, "LEVEL.BIN");
}
