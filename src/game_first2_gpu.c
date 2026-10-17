#include "game_draft_signatures.h"
#include "xport_trace.h"
#include "psx.h"

void sub_8002669C(uint32 value)
{
    FUNCTION_MARKER(0x8002669Cu, "SLUS_009.74");
    xport_gte_write_control(27u, value);
}

void sub_800266AC(uint32 value)
{
    FUNCTION_MARKER(0x800266ACu, "SLUS_009.74");
    xport_gte_write_control(28u, value);
}

void sub_8001C714(uint32 value)
{
    FUNCTION_MARKER(0x8001C714u, "SLUS_009.74");
    sub_8002669C(value);
    sub_800266AC(0x01400000u);
}

uint32 sub_8001716C(uint32 ordering_table, uint32 count, uint32 diagnostic_level)
{
    uint32 cursor = ordering_table, remaining, next;
    FUNCTION_MARKER(0x8001716Cu, "SLUS_009.74");
    /* Incoming diagnostic V0 is an explicit semantic argument */
    if (diagnostic_level >= 2u)
        moh_draft_indirect_call(r_u32(0x8002DF14u), 0x800148CCu, ordering_table, count);
    remaining = count - 1u;
    while (remaining)
    {
        --remaining;
        next = cursor + 4u;
        w_u8(cursor + 3u, 0u);
        w_u32(cursor, (r_u32(cursor) & 0xFF000000u) | (next & 0x00FFFFFFu));
        cursor = next;
    }
    /* The terminator links to the fixed packet address rather than its contents */
    w_u32(0x8002DFD8u, (0x8002DFC4u & 0x00FFFFFFu) | 0x04000000u);
    w_u32(cursor, 0x8002DFD8u & 0x00FFFFFFu);
    return cursor;
}
