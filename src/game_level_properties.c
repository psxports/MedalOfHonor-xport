#include "game_draft_signatures.h"
#include "game_level_signatures.h"
#include "xport_trace.h"
#include "psx.h"

static uint32 property_vector_length(uint32 vector)
{
    uint32 result = moh_draft_missing_call(0x80056EB4u, vector);
    /* TODO Confirm that the synchronous helper does not retain the vector */
    sub_80036E44(vector);
    return result;
}

uint32 level_sub_8005CBA8(uint32 property, uint32 object)
{
    uint32 player, value, bit, node, count, vector, result, voice, record;
    sint32 delta, angle;
    FUNCTION_MARKER(0x8005CBA8u, "LEVEL.BIN");
    if (property - 0x60u > 0x1Fu)
    {
        if (!(property & 0x1C0u)) return r_u32(object + property * 4u + 0x4Cu);
        if (property & 0x100u)
        {
            bit = property ^ 0x100u;
            value = r_u32(object + ((uint32)((sint32)bit >> 5) << 2) + 0xECu);
        }
        else if (property & 0x40u) { bit = property - 0x40u; value = r_u32(object + 12u); }
        else if (property & 0x80u) { bit = property - 0x80u; value = r_u32(object + 16u); }
        else return 0u;
        return (value & (1u << (bit & 31u))) != 0u;
    }
    switch (property)
    {
    case 0x60:
        player = r_u32(0x8009D654u);
        return r_u16(r_u32(r_u32(player + 0x100u) + 8u) + 0xC8u);
    case 0x61: return r_u32(r_u32(0x8009D654u) + 16u) & 0x400u;
    case 0x62: return object + 0x3A4u;
    case 0x63:
        player = r_u32(0x8009D654u);
        value = (r_u16(player + 0x20Eu) - r_u16(object + 0x20Eu)) & 0xFFFFu;
        xport_gte_write_control(0u, value);
        value = (r_u16(player + 0x216u) - r_u16(object + 0x216u)) & 0xFFFFu;
        /* TODO Original control register 1 upper half came from undefined vector padding */
        xport_gte_write_control(1u, value);
        xport_gte_write_data(9u, r_u16(object + 0x344u));
        xport_gte_write_data(10u, 0u);
        xport_gte_write_data(11u, r_u16(object + 0x350u));
        xport_gte_execute(0x41E012u);
        return xport_gte_read_data(25u) >> 31;
    case 0x64: return r_u32(object + 0x29Cu);
    case 0x65: return r_u8(r_u32(r_u32(0x8009D654u) + 0x388u) + 0x55u);
    case 0x66: return (uint32)(sint32)(sint16)r_u16(r_u32(0x8009D654u) + 0x11Eu);
    case 0x67: return moh_draft_missing_call(0x8007570Cu, 0u, 0u);
    case 0x68:
        voice = r_u32(object + 0xFCu);
        if (voice - 1u >= 23u) return 0u;
        record = 0x8009115Cu + voice * 52u;
        if (r_u8(record + 14u) || !(r_u8(0x800910D0u + voice) & 1u)) return 0u;
        if (r_u16(record + 40u) >= 120u) return 0u;
        return (r_u32(record) & 1u) == 0u;
    case 0x69: return r_u32(0x80097FE0u);
    case 0x6B: return r_u32(r_u32(0x8009D654u) + 16u) & 0x800u;
    case 0x6C: return (r_u32(r_u32(r_u32(0x8009D654u) + 0x388u)) >> 1) & 1u;
    case 0x6D:
        count = 0u;
        node = r_u32(0x8009D64Cu);
        while (node)
        {
            if (r_u32(node + 8u) == 0x1402u && node != object &&
                !(r_u32(node + 12u) & 0x20080000u))
            {
                vector = sub_80016718(8u);
                w_u16(vector, (r_u32(object + 0x20Cu) - r_u32(node + 0x20Cu)) >> 16);
                w_u16(vector + 2u, (r_u32(object + 0x210u) - r_u32(node + 0x210u)) >> 16);
                w_u16(vector + 4u, (r_u32(object + 0x214u) - r_u32(node + 0x214u)) >> 16);
                /* TODO Original SVECTOR padding was undefined */
                result = moh_draft_missing_call(0x80056EB4u, vector) & 0xFFFFu;
                delta = (sint16)r_u16(vector + 2u);
                sub_80036E44(vector);
                if (delta < 0) delta = -delta;
                if (result < 3000u && delta < 1000) ++count;
            }
            node = r_u32(node + 4u);
        }
        return count;
    case 0x6E: return 0x8009817Cu;
    case 0x6F:
        return (uint32)((sint32)(sint16)r_u16(object + 0x212u) -
            (sint32)(sint16)r_u16(r_u32(0x8009D654u) + 0x212u));
    case 0x70: return r_u32(r_u32(r_u32(object + 0x100u) + 8u));
    case 0x71:
        angle = (sint16)moh_draft_missing_call(0x8003F858u, r_u32(0x8009D654u), object);
        delta = (sint16)moh_draft_missing_call(0x8003F858u, r_u32(0x8009D654u), object);
        if (angle < 0) delta = -delta;
        if (delta >= 600) return 0u;
        player = r_u32(0x8009D654u);
        vector = sub_80016718(48u);
        w_u32(vector, r_u32(player + 0x20Cu));
        w_u32(vector + 4u, r_u32(player + 0x210u) - r_u32(player + 0x2D4u));
        w_u32(vector + 8u, r_u32(player + 0x214u));
        w_u32(vector + 16u, r_u32(object + 0x20Cu));
        w_u32(vector + 20u, r_u32(object + 0x210u) - r_u32(object + 0x2D4u));
        w_u32(vector + 24u, r_u32(object + 0x214u));
        /* TODO Original VECTOR padding and output SVECTOR initial contents were undefined */
        result = moh_draft_missing_call(0x8003EF5Cu, vector, vector + 16u,
            vector + 32u, vector + 32u, vector + 40u, object + 0x318u, 0u);
        /* TODO Confirm that the synchronous helper does not retain temporary vectors */
        sub_80036E44(vector);
        return result >> 31;
    case 0x72: return r_u32(0x800939F4u);
    case 0x73: return r_u32(0x800939F8u);
    case 0x74:
        player = r_u32(0x8009D654u);
        vector = sub_80016718(8u);
        w_u16(vector, r_u16(player + 0x20Eu) - r_u16(object + 0x3A4u));
        w_u16(vector + 2u, r_u16(player + 0x212u) - r_u16(object + 0x3A6u));
        w_u16(vector + 4u, r_u16(player + 0x216u) - r_u16(object + 0x3A8u));
        /* TODO Original SVECTOR padding was undefined */
        return property_vector_length(vector);
    default: return 0u;
    }
}
