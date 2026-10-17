#include "game_shell_signatures.h"
#include "game_level_signatures.h"
#include "psx.h"
#include "xport_trace.h"

uint32 level_sub_80040F48(uint32 object, uint32 radius, uint32 excluded, uint32 mask, uint32 unused);

void level_sub_8005CB00(uint32 object, uint32 property, uint32 bits)
{
    uint32 current;
    FUNCTION_MARKER(0x8005CB00u, "LEVEL.BIN");
    current = level_sub_8005CBA8(property, object);
    level_sub_8005C97C(object, property, bits | current);
}

uint32 level_sub_80059E40(uint32 object, uint32 data, uint32 flags)
{
    uint32 property, bits;
    FUNCTION_MARKER(0x80059E40u, "LEVEL.BIN");
    property = level_sub_8005D148(1u, flags, r_u32(data + 4u), object);
    bits = level_sub_8005D148(2u, flags, r_u32(data + 8u), object);
    level_sub_8005CB00(object, property, bits);
    return data + 12u;
}

uint32 level_sub_8005B23C(uint32 object, uint32 data, uint32 flags)
{
    uint32 radius, excluded, mask, unused, selected, destination;
    FUNCTION_MARKER(0x8005B23Cu, "LEVEL.BIN");
    radius = moh_draft_missing_call(0x8005D148u, 2u, flags, r_u32(data + 8u), object);
    excluded = moh_draft_missing_call(0x8005D148u, 3u, flags, r_u32(data + 12u), object);
    mask = moh_draft_missing_call(0x8005D148u, 4u, flags, r_u32(data + 16u), object);
    unused = moh_draft_missing_call(0x8005D148u, 5u, flags, r_u32(data + 20u), object);
    selected = level_sub_80040F48(object, radius, excluded, mask, unused);
    destination = moh_draft_missing_call(0x8005D148u, 1u, flags, r_u32(data + 4u), object);
    moh_draft_missing_call(0x8005C97Cu, object, destination, selected);
    return data + 24u;
}

static sint32 first2_actor_distance(sint32 x, sint32 y, sint32 z)
{
    sint32 temporary;
    if (x < 0) x = -x;
    if (y < 0) y = -y;
    if (z < 0) z = -z;
    if (x < y) { temporary = x; x = y; y = temporary; }
    if (x < z) { temporary = x; x = z; z = temporary; }
    return x + ((y + z) >> 2);
}

uint32 level_sub_80040F48(uint32 object, uint32 radius, uint32 excluded, uint32 mask, uint32 unused)
{
    uint32 scene, candidate, count, index = 0u, selected = 0xFFFFFFFFu;
    sint32 best_radius = (sint32)radius;
    FUNCTION_MARKER(0x80040F48u, "LEVEL.BIN");
    if (best_radius < 0) best_radius = (sint32)(0u - (uint32)best_radius);
    scene = r_u32(r_u32(0x8009D648u) + 0x388u);
    candidate = r_u32(scene + 8u);
    count = r_u32(scene + 12u);
    while ((sint32)index < (sint32)count) {
        if (index != excluded) {
            uint32 invalid = moh_draft_missing_call(0x8006D8ACu, candidate + 0x30u);
            if (invalid || excluded == 0xFFFFFFFFu) {
                sint32 dx = (sint16)((uint32)(sint16)r_u16(object + 0x20Eu) - r_u16(candidate + 0x14u));
                sint32 dy = (sint16)((uint32)(sint16)r_u16(object + 0x212u) - r_u16(candidate + 0x16u));
                sint32 dz = (sint16)((uint32)(sint16)r_u16(object + 0x216u) - r_u16(candidate + 0x18u));
                sint32 distance = first2_actor_distance(dx, dy, dz);
                if (distance <= best_radius && ((r_u32(candidate + 12u) & mask) || mask == 0xFFFFFFFFu)) {
                    uint32 player = r_u32(0x8009D654u);
                    sint32 player_distance;
                    dx = (sint16)((uint32)(sint16)r_u16(player + 0x20Eu) - r_u16(candidate + 0x14u));
                    dy = (sint16)((uint32)(sint16)r_u16(player + 0x212u) - r_u16(candidate + 0x16u));
                    dz = (sint16)((uint32)(sint16)r_u16(player + 0x216u) - r_u16(candidate + 0x18u));
                    player_distance = first2_actor_distance(dx, dy, dz);
                    if ((distance < player_distance || mask == 8u) && player_distance < 10000) {
                        uint32 vector_xy = r_u32(candidate + 0x24u);
                        uint32 vector_zpad = r_u32(candidate + 0x28u);
                        sint32 dot;
                        /* TODO Original delta padding is undefined and affects other GTE rows; native padding is zero */
                        xport_gte_write_control(0u, (uint32)(uint16)dx);
                        xport_gte_write_control(1u, (uint32)(uint16)dz);
                        xport_gte_write_data(9u, vector_xy & 0xFFFFu);
                        xport_gte_write_data(10u, 0u);
                        xport_gte_write_data(11u, vector_zpad & 0xFFFFu);
                        xport_gte_execute(0x41E012u);
                        dot = (sint32)xport_gte_read_data(25u);
                        if ((dot >> 12) > 100 || mask == 8u) {
                            selected = index;
                            best_radius = distance;
                        }
                    }
                }
            }
        }
        index += 1u;
        candidate += 0x34u;
    }
    return selected;
}
