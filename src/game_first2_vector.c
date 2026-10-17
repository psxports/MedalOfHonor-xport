#include "game_draft_signatures.h"
#include "psx.h"
#include "xport_trace.h"
#include <stdint.h>

static uint32 vector_trapping_add(uint32 left, uint32 right, uint32 address)
{
    int64_t sum = (int64_t)(sint32)left + (int64_t)(sint32)right;
    if (sum < INT32_MIN || sum > INT32_MAX)
        moh_draft_fail_fast("Vector normalization signed addition overflow", address);
    return (uint32)sum;
}

uint32 sub_80016A70(uint32 x, uint32 y, uint32 z, uint32 output)
{
    uint32 squared_x, squared_y, squared_z, sum, leading, shift, normalized, factor;
    SVECTOR input;
    VECTOR product;
    FUNCTION_MARKER(0x80016A70u, "SLUS_009.74");
    xport_gte_write_data(9u, x);
    xport_gte_write_data(10u, y);
    xport_gte_write_data(11u, z);
    xport_gte_execute(0xA00428u);
    squared_x = xport_gte_read_data(25u);
    squared_y = xport_gte_read_data(26u);
    squared_z = xport_gte_read_data(27u);
    sum = vector_trapping_add(squared_x, squared_y, 0x80016A90u);
    sum = vector_trapping_add(sum, squared_z, 0x80016A94u);
    xport_gte_write_data(30u, sum);
    leading = xport_gte_read_data(31u) & 0xFFFFFFFEu;
    shift = (uint32)((sint32)(31u - leading) >> 1);
    if ((sint32)(leading - 24u) < 0)
        normalized = (uint32)((sint32)sum >> ((24u - leading) & 31u));
    else
        normalized = sum << ((leading - 24u) & 31u);
    /* TODO The original zero sum still reads the table at index -64 and shifts by 31 */
    factor = (uint32)(sint32)(sint16)r_u16(0x8002DD3Cu + (normalized - 64u) * 2u);
    xport_gte_write_data(8u, factor);
    xport_gte_write_data(9u, x);
    xport_gte_write_data(10u, y);
    xport_gte_write_data(11u, z);
    input.vx = (sint16)x; input.vy = (sint16)y; input.vz = (sint16)z; input.pad = 0;
    gte_gpf0(&input, (sint16)factor, &product);
    x = xport_gte_read_data(25u);
    y = xport_gte_read_data(26u);
    z = xport_gte_read_data(27u);
    x = (uint32)((sint32)x >> (shift & 31u));
    y = (uint32)((sint32)y >> (shift & 31u));
    z = (uint32)((sint32)z >> (shift & 31u));
    /* TODO Output is the agreed guest pointer bridge for the original carried T0/T1/T2 results */
    w_u32(output, x);
    w_u32(output + 4u, y);
    w_u32(output + 8u, z);
    return sum;
}
