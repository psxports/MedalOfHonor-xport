#include "game_draft_signatures.h"
#include "xport_trace.h"
#include "psx.h"

uint32 sub_8001556C(uint32 input, uint32 output)
{
    uint32 xy, z;
    FUNCTION_MARKER(0x8001556Cu, "SLUS_009.74");
    xy = r_u32(input);
    z = r_u32(input + 4u);
    xport_gte_write_data(0u, xy);
    xport_gte_write_data(1u, z);
    xport_gte_execute(0x486012u);
    w_u32(output, xport_gte_read_data(9u));
    w_u32(output + 4u, xport_gte_read_data(10u));
    w_u32(output + 8u, xport_gte_read_data(11u));
    return output;
}

uint32 sub_80016718(uint32 size)
{
    uint32 allocation;
    FUNCTION_MARKER(0x80016718u, "SLUS_009.74");
    if (!size) size = 1u;
    for (;;)
    {
        allocation = sub_80036BF0(size);
        if (allocation) return allocation;
        moh_draft_indirect_call(r_u32(0x80035D1Cu));
    }
}

uint32 sub_80036DC4(uint32 size, uint32 count, uint32 owner, uint32 flags)
{
    uint32 allocation, cursor, units;
    FUNCTION_MARKER(0x80036DC4u, "SLUS_009.74");
    if (count != 1u) size *= count;
    if (!size) return 0u;
    allocation = sub_80036C14(size, owner, flags);
    if (!allocation) return 0u;
    cursor = allocation;
    units = r_u32(allocation - 16u) - 1u;
    do
    {
        w_u32(cursor, 0u);
        w_u32(cursor + 4u, 0u);
        w_u32(cursor + 8u, 0u);
        w_u32(cursor + 12u, 0u);
        cursor += 16u;
        --units;
    } while (units);
    return allocation;
}

uint32 sub_80036DA0(uint32 size, uint32 count)
{
    FUNCTION_MARKER(0x80036DA0u, "SLUS_009.74");
    return sub_80036DC4(size, count, 0u, 0u);
}
