#include "game_draft_signatures.h"
#include "game_level_signatures.h"
#include "xport_trace.h"
#include "psx.h"

static sint32 moh_route_planar_distance(uint32 x, uint32 z)
{
    sint32 dx = (sint16)x, dz = (sint16)z, swap;
    if (dx < 0) dx = -dx;
    if (dz < 0) dz = -dz;
    if (dx < dz) { swap = dx; dx = dz; dz = swap; }
    return dx + (dz >> 2);
}

void level_sub_80040A0C(uint32 object)
{
    uint32 extra, route, points, point, previous, local, dx, dz, tx, tz;
    sint32 count, movement, distance, index, prior_index, mode;
    FUNCTION_MARKER(0x80040A0Cu, "LEVEL.BIN");
    extra = r_u32(object + 0x38Cu);
    route = r_u32(0x8009E290u) + (uint32)(sint32)(sint16)r_u16(extra + 0xB8u) * 8u;
    points = r_u32(route + 4u);
    count = (sint16)r_u16(route + 2u);
    movement = moh_route_planar_distance(
        (uint32)(sint32)(sint16)r_u16(object + 0x20Eu) - (uint32)(sint32)(sint16)r_u16(object + 0x25Au),
        (uint32)(sint32)(sint16)r_u16(object + 0x216u) - (uint32)(sint32)(sint16)r_u16(object + 0x262u));
    for (;;) {
        index = (sint16)r_u16(extra + 0xBCu);
        point = points + (uint32)index * 20u;
        dx = (uint16)(r_u16(point) - (uint32)(sint32)(sint16)r_u16(object + 0x20Eu));
        dz = (uint16)(r_u16(point + 4u) - (uint32)(sint32)(sint16)r_u16(object + 0x216u));
        distance = moh_route_planar_distance(dx, dz);
        prior_index = index - (sint16)r_u16(extra + 0xBEu);
        if (prior_index < 0) prior_index = 1;
        else if (prior_index >= count) prior_index = count - 2;
        previous = points + (uint32)prior_index * 20u;
        tx = (uint16)(r_u16(point) - r_u16(previous));
        tz = (uint16)(r_u16(point + 4u) - r_u16(previous + 4u));
        xport_gte_write_control(0u, dx);
        /* TODO: Original unused upper matrix word has no defined value */
        xport_gte_write_control(1u, dz);
        xport_gte_write_data(9u, tx);
        xport_gte_write_data(10u, 0u);
        xport_gte_write_data(11u, tz);
        xport_gte_execute(0x41E012u);
        if (movement < distance && distance >= 76 && (sint32)xport_gte_read_data(25u) > 0) {
            local = moh_draft_missing_call(0x80016718u, 16u);
            /* TODO: Original unused vector padding has no defined value */
            w_u16(local, dx); w_u16(local + 2u, 0u); w_u16(local + 4u, dz);
            w_u16(local + 8u, r_u16(object + 0x344u));
            w_u16(local + 10u, 0u);
            w_u16(local + 12u, r_u16(object + 0x350u));
            moh_draft_missing_call(0x80040180u, object, local + 8u, local, 0xC0u, (uint32)distance);
            moh_draft_missing_call(0x80036E44u, local);
            return;
        }
        mode = (sint16)r_u16(extra + 0xBAu);
        index = (sint16)r_u16(extra + 0xBCu);
        if (mode == 0) {
            if (index < count - 1) {
                w_u32(object + 0x38u, r_u32(object + 0x38u) | 0x400000u);
                w_u16(extra + 0xBCu, r_u16(extra + 0xBCu) + 1u);
            } else {
                w_u32(object + 0x38u, r_u32(object + 0x38u) | 0x1000000u);
                w_u16(extra + 0xBCu, 0u);
            }
        } else if (mode == 1) {
            if (index > 0) {
                w_u32(object + 0x38u, r_u32(object + 0x38u) | 0x400000u);
                w_u16(extra + 0xBCu, r_u16(extra + 0xBCu) - 1u);
            } else {
                w_u32(object + 0x38u, r_u32(object + 0x38u) | 0x800000u);
                w_u16(extra + 0xBCu, (uint32)count - 1u);
            }
        } else if (mode == 2) {
            if (index <= 0) {
                w_u32(object + 0x38u, r_u32(object + 0x38u) | 0x800000u);
                w_u16(extra + 0xBCu, 1u); w_u16(extra + 0xBEu, 1u);
            } else if (index < count - 1) {
                w_u32(object + 0x38u, r_u32(object + 0x38u) | 0x400000u);
                w_u16(extra + 0xBCu, r_u16(extra + 0xBCu) + r_u16(extra + 0xBEu));
            } else {
                w_u32(object + 0x38u, r_u32(object + 0x38u) | 0x1000000u);
                w_u16(extra + 0xBCu, (uint32)count - 2u); w_u16(extra + 0xBEu, 0xFFFFu);
            }
        }
    }
}

void level_sub_80040DF4(uint32 object)
{
    uint32 extra, flags, state, current;
    FUNCTION_MARKER(0x80040DF4u, "LEVEL.BIN");
    extra = r_u32(object + 0x38Cu);
    w_u32(extra + 0x94u, r_u32(object + 0x20Cu));
    current = r_u32(object + 0x210u);
    flags = r_u32(extra + 4u);
    w_u32(extra + 0x98u, current);
    state = r_u32(extra + 8u) & ~3u;
    current = r_u32(object + 0x214u);
    w_u32(extra + 8u, state);
    w_u32(extra + 0x9Cu, current);
    if (!(r_u32(extra + 4u) & 4u)) w_u32(extra + 8u, state & ~0x1Cu);
    if (flags & 1u)
        moh_draft_missing_call(0x800404E8u, object, r_u32(extra + 0x10u), r_u32(extra + 0x50u), 0u);
    if (flags & 0x4000u)
        moh_draft_missing_call(0x800404E8u, object, r_u32(extra + 0x10u), r_u32(extra + 0x50u), 1u);
    if (flags & 0x1000u) {
        current = r_u32(object + 0x38Cu);
        moh_draft_missing_call(0x800408B4u, object, r_u32(current + 0x40u), r_u32(extra + 0x80u),
            (uint32)(sint32)(sint16)r_u16(current + 0xA8u));
    }
    if (flags & 0x800u)
        moh_draft_missing_call(0x80040618u, object, r_u32(extra + 0x3Cu), r_u32(extra + 0x7Cu),
            (uint32)(sint32)(sint16)r_u16(extra + 0xA8u));
    if ((flags & 4u) && !(flags & 0x5C03u))
        moh_draft_missing_call(0x8003FFA4u, object, r_u32(extra + 0x18u), (uint32)(sint32)(sint16)r_u16(extra + 0xC0u));
    if (flags & 2u) moh_draft_missing_call(0x80040A0Cu, object);
    current = r_u32(object + 0x38Cu);
    w_u32(current + 8u, r_u32(current + 8u) & ~0x20u);
}
static uint32 moh_collision_square(uint32 x, uint32 y, uint32 *second)
{
    VECTOR input, output;
    input.vx = (sint16)x; input.vy = (sint16)y; input.vz = 0; input.pad = 0;
    /* TODO: Original third SQR input is unspecified; its result is not consumed */
    Square0(&input, &output); if (second) *second = (uint32)output.vy;
    return (uint32)output.vx;
}

static void moh_contact_matrix(uint32 matrix, uint32 words)
{
    uint32 index;
    for (index = 0u; index < words; ++index) xport_gte_write_control(index, r_u32(matrix + index * 4u));
}

static void moh_contact_transform(uint32 vector)
{
    xport_gte_write_data(0u, r_u32(vector)); xport_gte_write_data(1u, r_u32(vector + 4u));
    xport_gte_execute(0x486012u);
}

static sint32 moh_contact_dot(uint32 normal, uint32 vector)
{
    moh_contact_matrix(normal, 2u);
    xport_gte_write_data(9u, r_u16(vector)); xport_gte_write_data(10u, r_u16(vector + 2u));
    xport_gte_write_data(11u, r_u16(vector + 4u)); xport_gte_execute(0x41E012u);
    return (sint32)xport_gte_read_data(25u);
}

uint32 level_sub_8004B6DC(uint32 object, uint32 other, uint32 normal_out, uint32 mode)
{
    uint32 local, center, top, bottom, movement, work, output, predicted, rotation, planes, plane, index, bit;
    uint32 near_mask = 0u, inside_mask = 0u, edge_mask = 0u, result = 0u, value, a, b, product, numerator;
    sint32 distances[16], edges[16], x, y, z, largest, remaining, penetration, divisor, quotient;
    FUNCTION_MARKER(0x8004B6DCu, "LEVEL.BIN");
    if (!r_u16(other + 0x2EEu)) return 0u;
    local = sub_80016718(80u); if (!local) moh_draft_fail_fast("Object plane contact allocation", 0x8004B6DCu);
    center = local; top = local + 8u; bottom = local + 16u; movement = local + 24u;
    work = local + 32u; output = local + 40u; predicted = local + 56u; rotation = local + 64u;
    planes = r_u32(other + 0x300u);
    value = r_u16(object + 0x2E0u); w_u16(center, (uint16)value); w_u16(bottom, (uint16)value); w_u16(top, (uint16)value);
    value = r_u16(object + 0x212u); w_u16(top + 2u, (uint16)value);
    w_u16(center + 2u, r_u16(object + 0x2E2u)); w_u16(bottom + 2u, (uint16)(value - r_u16(object + 0x2D6u)));
    value = r_u16(object + 0x2E4u); w_u16(center + 4u, (uint16)value); w_u16(bottom + 4u, (uint16)value); w_u16(top + 4u, (uint16)value);
    w_u16(movement, (uint16)(r_u16(object + 0x27Cu) + r_u16(object + 0x21Au)));
    w_u16(movement + 2u, (uint16)(r_u16(object + 0x27Eu) + r_u16(object + 0x21Eu)));
    value = r_u32(object + 0x200u) + r_u32(object + 0x220u);
    w_u16(movement + 4u, (uint16)(r_u16(object + 0x280u) + (uint32)((sint32)value >> 16)));
    /* TODO: Original short-vector padding is unspecified and ignored by GTE */
    moh_draft_missing_call(0x8004AEA0u, other, center, center, top, top, bottom, bottom);
    if (!(r_u16(movement) | r_u16(movement + 2u) | r_u16(movement + 4u))) {
        w_u16(movement, (uint16)(r_u16(object + 0x20Eu) - r_u16(object + 0x25Au)));
        w_u16(movement + 2u, (uint16)(r_u16(object + 0x212u) - r_u16(object + 0x25Eu)));
        w_u16(movement + 4u, (uint16)(r_u16(object + 0x216u) - r_u16(object + 0x262u)));
    } else {
        if (r_u32(object + 12u) & 1u) {
            w_u32(rotation, 0u); w_u32(rotation + 4u, r_u32(object + 0x22Cu)); w_u32(rotation + 8u, r_u32(object + 0x230u));
            level_sub_8008D918(object + 0x20Cu, rotation, object + 0x270u, work); moh_contact_matrix(work, 8u);
        } else moh_contact_matrix(object + 0x340u, 8u);
        moh_contact_transform(movement);
        w_u16(movement, (uint16)xport_gte_read_data(9u)); w_u16(movement + 2u, (uint16)xport_gte_read_data(10u));
        w_u16(movement + 4u, (uint16)xport_gte_read_data(11u));
    }
    moh_draft_missing_call(0x8004ADC8u, other, movement, movement);
    if (r_u32(object + 0x18u) & 1u) { sub_80036E44(local); return 0u; }
    for (index = 0u, plane = planes; index < r_u16(other + 0x2EEu); ++index, plane += 28u) {
        if (index >= 16u) moh_draft_fail_fast("TODO: Original contact distance arrays overflow", 0x8004B6DCu);
        value = (uint32)(moh_contact_dot(plane + 4u, center) >> 15) - (uint32)(sint32)(sint16)r_u16(plane + 10u);
        distances[index] = (sint32)value;
        value = (uint32)(moh_contact_dot(plane + 4u, (sint16)r_u16(plane + 6u) < 0 ? top : bottom) >> 15) - (uint32)(sint32)(sint16)r_u16(plane + 10u);
        edges[index] = (sint32)value; bit = 1u << (index & 31u);
        if (distances[index] < 0) { near_mask |= bit; inside_mask |= bit; }
        else {
            value = 1u;
            if ((sint32)r_u16(object + 0x2E8u) < distances[index]) {
                x = (sint16)r_u16(movement); z = (sint16)r_u16(movement + 4u); y = (sint16)r_u16(movement + 2u);
                if (x < 0) x = -x; if (z < 0) z = -z; if (y < 0) y = -y;
                largest = x; if (largest < y) { largest = y; y = x; }
                remaining = z; if (largest < z) { remaining = largest; largest = z; }
                if (distances[index] - (largest + ((y + remaining) >> 2)) > 0) value = 0u;
            }
            if (value) near_mask |= bit;
        }
        if (edges[index] <= (sint16)r_u16(object + 0x3C8u)) edge_mask |= bit;
    }
    for (index = 0u, plane = planes; index < r_u16(other + 0x2EEu); ++index, plane += 28u) {
        if (index >= 16u) moh_draft_fail_fast("TODO: Original contact distance arrays overflow", 0x8004B6DCu);
        bit = 1u << (index & 31u);
        if ((sint32)r_u32(object + 0x21Cu) >= 0 && (r_u32(plane) & 1u) && (near_mask & bit) &&
            (inside_mask & r_u32(plane + 12u)) == r_u32(plane + 16u) && (near_mask & r_u32(plane + 20u)) == r_u32(plane + 24u)) {
            a = (uint32)(sint32)(sint16)r_u16(plane + 4u) * (uint32)(sint32)(sint16)r_u16(center);
            b = (uint32)(sint32)(sint16)r_u16(plane + 8u) * (uint32)(sint32)(sint16)r_u16(center + 4u);
            numerator = ((uint32)(sint32)(sint16)r_u16(plane + 10u) << 15) - (a + b); divisor = (sint16)r_u16(plane + 6u);
            if (!divisor || (divisor == -1 && numerator == 0x80000000u)) moh_draft_fail_fast("Object contact plane division trap", 0x8004B6DCu);
            quotient = (sint32)numerator / divisor; w_u32(object + 0x210u, r_u32(other + 0x210u) + ((uint32)quotient << 16));
            w_u32(other + 0x18u, r_u32(other + 0x18u) | 8u); value = r_u32(object + 0x18u);
            w_u32(object + 0x21Cu, 0u); w_u32(object + 0x18u, value | 1u); level_sub_8006D84C(object + 0x2Cu, other);
            value = r_u32(object + 0x18u); a = r_u32(object + 0x284u); w_u32(object + 0x18u, value | 4u);
            if (!a) result |= 1u; w_u32(object + 0x284u, 1u);
        }
        if (!mode && !(r_u16(movement) | r_u16(movement + 2u) | r_u16(movement + 4u))) continue;
        if (!(r_u32(plane) & 2u) || !(near_mask & bit) || distances[index] < 0 ||
            (inside_mask & r_u32(plane + 12u)) != r_u32(plane + 16u)) continue;
        a = r_u32(plane + 20u); b = near_mask & a;
        if (b != r_u32(plane + 24u) || (edge_mask & a) != b) continue;
        penetration = (sint32)((uint32)(sint32)(sint16)r_u16(object + 0x3C8u) - (uint32)edges[index]);
        if (penetration <= 0 && !mode) continue;
        if ((sint16)r_u16(center + 2u) < (sint16)r_u16(other + 0x2F2u)) continue;
        product = (uint32)(sint32)(sint16)r_u16(plane + 4u) * (uint32)penetration;
        w_u16(work + 2u, 0u); w_u16(work, (uint16)((sint32)((product << 1) + 0x8000u) >> 16));
        product = (uint32)(sint32)(sint16)r_u16(plane + 8u) * (uint32)penetration;
        w_u16(work + 4u, (uint16)((sint32)((product << 1) + 0x8000u) >> 16));
        moh_contact_matrix(other + 0x340u, 5u); moh_contact_transform(work);
        product = (0u - (uint32)(sint32)(sint16)r_u16(plane + 4u)) * (uint32)(sint32)(sint16)r_u16(object + 0x3C8u);
        w_u16(work, (uint16)((sint32)((product << 1) + 0x8000u) >> 16));
        product = (0u - (uint32)(sint32)(sint16)r_u16(plane + 8u)) * (uint32)(sint32)(sint16)r_u16(object + 0x3C8u);
        w_u16(work + 4u, (uint16)((sint32)((product << 1) + 0x8000u) >> 16));
        for (a = 0u; a < 3u; ++a) w_u32(output + a * 4u, xport_gte_read_data(25u + a));
        moh_contact_transform(work);
        if (!mode) {
            value = r_u32(output); w_u32(object + 0x20Cu, r_u32(object + 0x20Cu) + (value << 16));
            value = r_u32(output + 8u); w_u32(object + 0x214u, r_u32(object + 0x214u) + (value << 16));
        }
        for (a = 0u; a < 3u; ++a) w_u32(output + a * 4u, xport_gte_read_data(25u + a));
        w_u32(predicted, r_u32(object + 0x20Cu) + (r_u32(output) << 16));
        w_u32(predicted + 4u, r_u32(object + 0x210u) - (uint32)((sint32)r_u32(object + 0x2D4u) >> 1));
        w_u32(predicted + 8u, r_u32(object + 0x214u) + (r_u32(output + 8u) << 16));
        if (!mode && moh_contact_dot(movement, plane + 4u) >= 0) continue;
        if (!(r_u32(plane) & 8u)) w_u32(object + 0x38u, r_u32(object + 0x38u) | 0x8000u);
        else {
            value = r_u32(object + 0x38u); a = r_u32(object + 0x48u); w_u32(object + 0x38u, value | 0x40u);
            if ((sint32)a <= 0 && (r_u32(object + 12u) & 5u)) moh_draft_missing_call(0x8004AC58u, object, r_u32(other + 0xF8u), other, 0xFFFFFFFFu);
        }
        y = (sint16)r_u16(plane + 6u);
        if (y > 0 && (sint32)r_u32(object + 0x21Cu) <= 0) {
            if (!r_u32(object + 0x284u)) { product = (uint32)y * (uint32)penetration; w_u32(object + 0x1F4u, r_u32(object + 0x1F4u) + (((product << 1) + 0x8000u) & 0xFFFF0000u)); }
            w_u32(object + 0x21Cu, 0u); w_u32(other + 0x18u, r_u32(other + 0x18u) | 0x10u);
        }
        result |= 2u; w_u32(object + 0x200u, 0u); w_u32(object + 0x218u, 0u); w_u32(object + 0x220u, 0u);
        moh_contact_matrix(other + 0x340u, 5u); moh_contact_transform(plane + 4u);
        w_u16(normal_out, (uint16)xport_gte_read_data(9u)); w_u16(normal_out + 2u, (uint16)xport_gte_read_data(10u));
        w_u16(normal_out + 4u, (uint16)xport_gte_read_data(11u));
    }
    sub_80036E44(local); return result;
}

void level_sub_80042524(uint32 object)
{
    uint32 object_out, index, handle = 0x80091194u, record = 0x80091190u, current;
    FUNCTION_MARKER(0x80042524u, "LEVEL.BIN");
    object_out = sub_80016718(4u);
    if (!object_out) moh_draft_fail_fast("Deleted object handle output allocation", 0x80042524u);
    w_u32(object_out, object);
    for (index = 1u; index < 24u; ++index) {
        if (level_sub_8006D7D0(handle, object_out)) {
            current = r_u32(object_out);
            w_u16(record + 0x2Cu, r_u16(current + 0x20Eu));
            w_u16(record + 0x2Eu, r_u16(current + 0x212u));
            w_u16(record + 0x30u, r_u16(current + 0x216u));
            level_sub_8006D84C(handle, 0u);
        }
        record += 0x34u; handle += 0x34u;
    }
    sub_80036E44(object_out);
}

void level_sub_8006B89C(uint32 object)
{
    uint32 current, next;
    FUNCTION_MARKER(0x8006B89Cu, "LEVEL.BIN");
    if (!object) return;
    current = r_u32(0x8009D64Cu);
    if (current == object) { w_u32(0x8009D64Cu, r_u32(object + 4u)); return; }
    while (current) {
        next = r_u32(current + 4u);
        if (next == object) { w_u32(current + 4u, r_u32(next + 4u)); return; }
        current = next;
    }
}

void level_sub_8006DA50(uint32 object)
{
    uint32 index;
    FUNCTION_MARKER(0x8006DA50u, "LEVEL.BIN");
    index = level_sub_8006D898(object); w_u32(0x8009DE78u + index * 4u, 1u);
}

void level_sub_8006A318(uint32 object)
{
    uint32 state;
    FUNCTION_MARKER(0x8006A318u, "LEVEL.BIN");
    moh_draft_missing_call(0x800665FCu, object, 0u);
    state = r_u32(object + 0x38Cu); if (state) sub_80036E44(state);
    moh_draft_missing_call(0x8008A4F8u, object, 2u, 0x8003CEC0u, 0x8003CEDCu, 0xBAu);
}

void level_sub_8006A37C(uint32 object)
{
    uint32 render;
    FUNCTION_MARKER(0x8006A37Cu, "LEVEL.BIN");
    render = r_u32(object + 0x100u);
    if (render) level_sub_8006B234(render, object, 2u);
    level_sub_8006A318(object);
}

void level_sub_80066918(uint32 message)
{
    uint32 head;
    FUNCTION_MARKER(0x80066918u, "LEVEL.BIN");
    head = r_u32(0x8009D5B4u); w_u32(0x8009D5B4u, message); w_u32(message + 0x20u, head);
}

void level_sub_800665FC(uint32 object, uint32 filter)
{
    uint32 trigger, limit, count, index, node, previous, next, object_out, matched;
    FUNCTION_MARKER(0x800665FCu, "LEVEL.BIN");
    trigger = r_u32(object + 0x1Cu);
    if (trigger && r_u32(r_u32(trigger + 4u) + 12u)) level_sub_800664A0(object, 0u, 0x1Au);
    limit = r_u32(0x8009D5ACu); count = limit ? r_u32(limit) : 0u;
    object_out = sub_80016718(4u);
    if (!object_out) moh_draft_fail_fast("Deleted object message output allocation", 0x800665FCu);
    w_u32(object_out, object);
    for (index = 0u; index < count; ++index) {
        previous = 0u; node = r_u32(r_u32(0x8009D5B0u) + index * 4u);
        while (node) {
            matched = 0u;
            if (r_u32(object_out)) {
                if (level_sub_8006D7D0(node, object_out) || level_sub_8006D7D0(node + 12u, object_out)) matched = 1u;
            }
            if (!matched && filter) matched = filter == r_u32(node + 4u) || filter == r_u32(node + 0x10u);
            if (matched) {
                next = r_u32(node + 0x20u);
                if (!previous) w_u32(r_u32(0x8009D5B0u) + index * 4u, next);
                else w_u32(previous + 0x20u, next);
                next = r_u32(node + 0x20u); level_sub_80066918(node); node = next;
            } else { previous = node; node = r_u32(node + 0x20u); }
        }
    }
    sub_80036E44(object_out);
}

uint32 level_sub_8007C030(uint32 object, uint32 other)
{
    uint32 kind, local, result, product, old, delta;
    sint32 dx, dz, abs_x, abs_z, maximum, minimum, overlap;
    FUNCTION_MARKER(0x8007C030u, "LEVEL.BIN");
    kind = r_u32(other + 8u);
    local = sub_80016718(16u);
    if (!local) moh_draft_fail_fast("Player contact output allocation", 0x8007C030u);
    if ((kind == 0x1402u || kind == 0x1405u) && r_u32(r_u32(other + 0x100u) + 0x6Cu) == 0x80u) {
        w_u16(local + 10u, 0u);
        dx = (sint16)r_u16(other + 0x20Eu) - (sint16)r_u16(object + 0x20Eu); w_u16(local + 8u, (uint16)dx);
        dx = (sint16)dx; dz = (sint16)r_u16(other + 0x216u) - (sint16)r_u16(object + 0x216u);
        abs_x = dx < 0 ? -dx : dx; abs_z = (sint16)dz; if (abs_z < 0) abs_z = -abs_z;
        w_u16(local + 12u, (uint16)dz);
        /* TODO: Original vector padding is unspecified */
        maximum = abs_x; minimum = abs_z; if (maximum < minimum) { maximum = abs_z; minimum = abs_x; }
        overlap = (sint16)r_u16(other + 0x2ECu) + (sint16)r_u16(object + 0x2ECu) - (maximum + (minimum >> 2));
        if (overlap <= 0) { sub_80036E44(local); return 0u; }
        moh_draft_missing_call(0x80016A40u, local + 8u, local + 8u);
        if (overlap > 0x41) overlap = 0x41;
        delta = 0u - (uint32)(sint32)(sint16)r_u16(local + 8u); product = delta * (uint32)overlap;
        old = r_u32(object + 0x1F0u); w_u32(object + 0x1F0u, old + ((uint32)((sint32)product >> 12) << 16));
        delta = 0u - (uint32)(sint32)(sint16)r_u16(local + 12u); product = delta * (uint32)overlap;
        old = r_u32(object + 0x1F8u);
        w_u32(object + 0x200u, 0u); w_u32(object + 0x218u, 0u); w_u32(object + 0x220u, 0u);
        w_u32(object + 0x1F8u, old + ((uint32)((sint32)product >> 12) << 16));
    } else {
        result = moh_draft_missing_call(0x8004B6DCu, object, other, local, 0u);
        if (!result) { sub_80036E44(local); return 0u; }
        level_sub_8006A76C(object);
    }
    sub_80036E44(local); return 1u;
}

uint32 level_sub_80083988(uint32 point, uint32 extra, uint32 center, uint32 radius)
{
    sint32 px, cx, py, pz, cy, cz;
    uint32 sum, combined;
    VECTOR input, output;
    FUNCTION_MARKER(0x80083988u, "LEVEL.BIN");
    px = (sint16)r_u16(point); cx = (sint16)r_u16(center);
    py = (sint16)r_u16(point + 2u); pz = (sint16)r_u16(point + 4u);
    cy = (sint16)r_u16(center + 2u); cz = (sint16)r_u16(center + 4u);
    input.vx = (sint16)(px - cx); input.vy = (sint16)(py - cy); input.vz = (sint16)(pz - cz); input.pad = 0;
    Square0(&input, &output);
    combined = extra + radius;
    sum = (uint32)output.vx + (uint32)output.vy + (uint32)output.vz;
    return (sint32)(combined * combined - sum) > 0;
}

uint32 level_sub_8004CB18(uint32 object, uint32 other)
{
    sint32 ox, x, oy, oz, y, z;
    uint32 radius, sum;
    VECTOR input, output;
    FUNCTION_MARKER(0x8004CB18u, "LEVEL.BIN");
    if (!r_u16(other + 0x2E8u) || !r_u16(object + 0x2E8u)) return 0u;
    ox = (sint16)r_u16(other + 0x2E0u); x = (sint16)r_u16(object + 0x2E0u);
    oy = (sint16)r_u16(other + 0x2E2u); oz = (sint16)r_u16(other + 0x2E4u);
    y = (sint16)r_u16(object + 0x2E2u); z = (sint16)r_u16(object + 0x2E4u);
    input.vx = (sint16)(ox - x); input.vy = (sint16)(oy - y); input.vz = (sint16)(oz - z); input.pad = 0;
    Square0(&input, &output);
    radius = r_u16(other + 0x2E8u) + r_u16(object + 0x2E8u);
    sum = (uint32)output.vx + (uint32)output.vy + (uint32)output.vz;
    return (sint32)(radius * radius - sum) > 0;
}

uint32 level_sub_8004C990(uint32 object, uint32 first_out, uint32 other, uint32 second_out)
{
    uint32 first_index = 0u, second_index, first_offset = 0u, second_offset, first, second, radius, combined, overlap, result = 0u;
    uint32 x, y, z;
    VECTOR input, output;
    FUNCTION_MARKER(0x8004C990u, "LEVEL.BIN");
    while (first_index < r_u16(object + 0x36u)) {
        first = r_u32(object + 0x304u) + first_offset;
        if (r_u32(first + 8u) != 7u) {
            radius = r_u32(first + 0x34u); second_index = 0u; second_offset = 0u;
            while (second_index < r_u16(other + 0x36u)) {
                second = r_u32(other + 0x304u) + second_offset;
                if (r_u32(second + 8u) != 7u) {
                    x = r_u32(second + 0x20u) - r_u32(first + 0x20u);
                    y = r_u32(second + 0x24u) - r_u32(first + 0x24u);
                    z = r_u32(second + 0x28u) - r_u32(first + 0x28u);
                    input.vx = (sint16)x; input.vy = (sint16)y; input.vz = (sint16)z; input.pad = 0; Square0(&input, &output);
                    combined = radius + r_u32(second + 0x34u);
                    overlap = combined * combined - ((uint32)output.vx + (uint32)output.vy + (uint32)output.vz);
                    if ((sint32)overlap > 0) {
                        w_u32(r_u32(object + 0x304u) + first_offset, 1u); w_u32(r_u32(other + 0x304u) + second_offset, 1u);
                        w_u32(object + 0x314u, overlap); w_u32(other + 0x314u, overlap);
                        if (!result) { result = 1u; w_u32(first_out, first_index); w_u32(second_out, second_index); }
                    }
                }
                ++second_index; second_offset += 0x48u;
            }
        }
        ++first_index; first_offset += 0x48u;
    }
    return result;
}

uint32 level_sub_800839F0(uint32 point, uint32 center, uint32 radius, uint32 lower, uint32 upper)
{
    uint32 swap, dx, dz, sum;
    sint32 dy;
    FUNCTION_MARKER(0x800839F0u, "LEVEL.BIN");
    lower = 0u - lower; upper = 0u - upper;
    if ((sint32)upper < (sint32)lower) { swap = upper; upper = lower; lower = swap; }
    dy = (sint16)r_u16(point + 2u) - (sint16)r_u16(center + 2u);
    if (dy < (sint32)lower || (sint32)upper < dy) return 0u;
    dx = (uint32)(sint32)(sint16)r_u16(point) - (uint32)(sint32)(sint16)r_u16(center);
    dz = (uint32)(sint32)(sint16)r_u16(point + 4u) - (uint32)(sint32)(sint16)r_u16(center + 4u);
    if ((sint32)radius < (sint32)(0u - dx) || (sint32)radius < (sint32)dx ||
        (sint32)radius < (sint32)(0u - dz) || (sint32)radius < (sint32)dz) return 0u;
    sum = dx * dx + dz * dz;
    return (sint32)(radius * radius - sum) > 0;
}

uint32 level_sub_80083D30(uint32 point, uint32 extra, uint32 descriptor)
{
    uint32 data, result;
    sint32 type;
    FUNCTION_MARKER(0x80083D30u, "LEVEL.BIN");
    data = r_u32(descriptor + 4u); type = (sint16)r_u16(data + 0x28u);
    if (type == 0) result = level_sub_80083988(point, extra, data + 0x10u, (uint32)(sint32)(sint16)r_u16(data + 0x2Au));
    else if (type == 1) result = level_sub_800839F0(point, data + 0x10u, (uint32)(sint32)(sint16)r_u16(data + 0x2Au),
        (uint32)(sint32)(sint16)r_u16(data + 0x2Cu), (uint32)(sint32)(sint16)r_u16(data + 0x2Eu));
    else if (type == 2) result = moh_draft_missing_call(0x80083AB8u, point, descriptor);
    else return 0u;
    return result != 0u;
}

void level_sub_800840E0(uint32 object)
{
    uint32 identity, old_identity, lists, list, old_list, table, cursor, old_cursor;
    uint32 end, old_end, bits, incoming, changed, bit, old_bit, value, old_value;
    FUNCTION_MARKER(0x800840E0u, "LEVEL.BIN");
    identity = r_u32(object + 0x390u); lists = r_u32(0x8009EC5Cu); old_identity = r_u32(object + 0x398u);
    list = r_u32(lists + identity * 4u); old_list = r_u32(lists + old_identity * 4u);
    table = r_u32(r_u32(r_u32(0x8009D648u) + 0x388u) + 0x14u); cursor = list + 2u;
    if (identity == 0xFFFFu) {
        if (old_identity == identity) return;
        bits = r_u32(object + 0x39Cu); old_cursor = old_list + 2u;
        while (bits) {
            if (bits & 1u) moh_draft_missing_call(0x8008406Cu, object, table + (uint32)(sint32)(sint16)r_u16(old_cursor) * 8u);
            bits >>= 1; old_cursor += 2u;
        }
        return;
    }
    if (old_identity == 0xFFFFu) {
        bits = r_u32(object + 0x394u);
        while (bits) {
            if (bits & 1u) moh_draft_missing_call(0x80083EF0u, object, table + (uint32)(sint32)(sint16)r_u16(cursor) * 8u);
            bits >>= 1; cursor += 2u;
        }
        return;
    }
    if (identity == old_identity) {
        incoming = r_u32(object + 0x394u); bits = r_u32(object + 0x39Cu);
        changed = incoming ^ bits; incoming &= changed; bits &= changed; old_cursor = cursor;
        while (bits) {
            if (bits & 1u) moh_draft_missing_call(0x8008406Cu, object, table + (uint32)(sint32)(sint16)r_u16(old_cursor) * 8u);
            bits >>= 1; old_cursor += 2u;
        }
        while (incoming) {
            if (incoming & 1u) moh_draft_missing_call(0x80083EF0u, object, table + (uint32)(sint32)(sint16)r_u16(cursor) * 8u);
            incoming >>= 1; cursor += 2u;
        }
        return;
    }
    bit = old_bit = 1u; old_cursor = old_list + 2u;
    end = cursor + (0xFFu - r_u8(list)) * 2u; old_end = old_cursor + (0xFFu - r_u8(old_list)) * 2u;
    for (;;) {
        value = (uint32)(sint32)(sint16)r_u16(cursor); old_value = (uint32)(sint32)(sint16)r_u16(old_cursor);
        if (cursor == end || old_cursor == old_end) break;
        if (value == old_value) {
            incoming = (r_u32(object + 0x394u) & bit) != 0u; bits = (r_u32(object + 0x39Cu) & old_bit) != 0u;
            if (incoming != bits) moh_draft_missing_call(bits ? 0x8008406Cu : 0x80083EF0u, object, table + value * 8u);
        }
        if (value <= old_value) { cursor += 2u; bit <<= 1; }
        if (old_value <= value) { old_cursor += 2u; old_bit <<= 1; }
    }
    if (cursor == end) {
        while (old_cursor != old_end) {
            if (r_u32(object + 0x39Cu) & old_bit) moh_draft_missing_call(0x8008406Cu, object, table + (uint32)(sint32)(sint16)r_u16(old_cursor) * 8u);
            old_cursor += 2u; old_bit <<= 1;
        }
    }
    while (cursor != end) {
        if (r_u32(object + 0x394u) & bit) moh_draft_missing_call(0x80083EF0u, object, table + (uint32)(sint32)(sint16)r_u16(cursor) * 8u);
        cursor += 2u; bit <<= 1;
    }
}

void level_sub_80083EF0(uint32 object, uint32 descriptor)
{
    uint32 flags;
    FUNCTION_MARKER(0x80083EF0u, "LEVEL.BIN");
    flags = r_u32(object + 12u);
    if (flags & 1u) {
        if (!r_u32(r_u32(descriptor + 4u) + 12u)) moh_draft_missing_call(0x80074E10u, descriptor);
        else moh_draft_missing_call(0x80083DE8u, object, descriptor);
    } else if (flags & 0x50u) moh_draft_missing_call(0x80083E6Cu, object, descriptor);
}

void level_sub_80083DE8(uint32 object, uint32 descriptor)
{
    uint32 list, count;
    FUNCTION_MARKER(0x80083DE8u, "LEVEL.BIN");
    list = r_u32(r_u32(descriptor + 4u) + 0x38u);
    if (!list) return;
    count = r_u32(list); list += 4u;
    while (count) {
        moh_draft_missing_call(0x800662CCu, r_u32(list), 0u, descriptor, object);
        list += 4u; --count;
    }
}

void level_sub_80074E10(uint32 descriptor)
{
    uint32 data, identity;
    FUNCTION_MARKER(0x80074E10u, "LEVEL.BIN");
    data = r_u32(descriptor + 4u); identity = r_u32(data);
    if (identity == 0x4D078F23u) moh_draft_missing_call(0x80054378u, data, descriptor);
    else if (identity < 0x4D078F24u) {
        if (identity != 0x271BD574u && identity == 0x3011B913u) moh_draft_missing_call(0x80074BB8u, 0x4Cu, descriptor);
    } else if (identity == 0xD1BA7D1Eu) w_u32(0x8009C840u, r_u32(0x8009C840u) | 0x24u);
}

uint32 level_sub_80066EA0(uint32 message, uint32 actor, uint32 trigger, uint32 sender, uint32 selected)
{
    uint32 limit, node, type, accepted, candidate, resource, actor_out;
    FUNCTION_MARKER(0x80066EA0u, "LEVEL.BIN");
    limit = r_u32(0x8009D5ACu);
    if (limit ? (sint32)message >= (sint32)r_u32(limit) : (sint32)message >= 0) {
        moh_draft_missing_call(0x80015B28u, 0x8003CB90u, message); return 0u;
    }
    actor_out = sub_80016718(4u);
    if (!actor_out) moh_draft_fail_fast("Message actor output allocation", 0x80066EA0u);
    w_u32(actor_out, actor);
    node = r_u32(r_u32(0x8009D5B0u) + message * 4u);
    while (node) {
        type = r_u32(node + 8u); accepted = 0u;
        switch (type) {
        case 0u: accepted = 1u; break;
        case 1u: case 4u:
            candidate = trigger ? trigger : r_u32(r_u32(actor_out) + 0x1Cu);
            if (candidate && r_u32(node + 0x14u) == (uint32)(sint32)(sint16)r_u16(r_u32(candidate + 4u) + 0x34u)) accepted = 1u;
            break;
        case 2u:
            accepted = moh_draft_missing_call(0x8005351Cu, r_u32(actor_out), trigger, r_u32(node + 0x14u)) != 0u;
            break;
        case 3u:
            if (r_u32(actor_out) && moh_draft_missing_call(0x8006D7D0u, node + 12u, actor_out)) accepted = 1u;
            else if (trigger && trigger == r_u32(node + 0x10u)) accepted = 1u;
            break;
        case 5u:
            resource = r_u32(node + 12u);
            if (resource) resource = r_u32(0x8009D678u + (resource & 0xFFFFu) * 4u);
            accepted = moh_draft_missing_call(0x8005359Cu, r_u32(actor_out), trigger, resource, r_u32(node + 0x10u), r_u32(node + 0x14u)) != 0u;
            break;
        default:
            moh_draft_missing_call(0x80015B28u, 0x8003CBB8u, type); sub_80036E44(actor_out); return 0u;
        }
        if (accepted && (!selected || selected == r_u32(node + 4u))) {
            moh_draft_missing_call(0x80064858u, 0x8003CBF4u, 0u);
            moh_draft_missing_call(0x800670E8u, node, r_u32(actor_out), trigger, sender, message);
        }
        node = r_u32(node + 0x20u);
    }
    sub_80036E44(actor_out); return 1u;
}

uint32 level_sub_800662CC(uint32 message, uint32 actor, uint32 trigger, uint32 sender)
{
    FUNCTION_MARKER(0x800662CCu, "LEVEL.BIN");
    return level_sub_80066EA0(message, actor, trigger, sender, 0u);
}

uint32 level_sub_8005359C(uint32 actor, uint32 trigger, uint32 filter_actor, uint32 filter_trigger, uint32 category)
{
    uint32 data = 0u, filter_data = 0u, list, filter_list, count, filter_count, cursor, filter_cursor, end, filter_end, value, filter_value;
    FUNCTION_MARKER(0x8005359Cu, "LEVEL.BIN");
    if (actor) trigger = r_u32(actor + 0x1Cu);
    if (trigger) data = r_u32(trigger + 4u);
    if (filter_actor) filter_trigger = r_u32(filter_actor + 0x1Cu);
    if (filter_trigger) filter_data = r_u32(filter_trigger + 4u);
    list = r_u32(data + 0x48u); if (!list) return 0u;
    filter_list = r_u32(filter_data + 0x48u); if (!filter_list) return 0u;
    count = r_u32(list); filter_count = r_u32(filter_list);
    if (!count || !filter_count) return 0u;
    cursor = list + 4u; filter_cursor = filter_list + 4u;
    end = cursor + count * 4u; filter_end = filter_cursor + filter_count * 4u;
    while (cursor < end && r_u16(cursor + 2u) < category) cursor += 4u;
    while (filter_cursor < filter_end && r_u16(filter_cursor + 2u) < category) filter_cursor += 4u;
    while (cursor < end && filter_cursor < filter_end) {
        value = r_u32(cursor); if ((value >> 16) != category) return 0u;
        filter_value = r_u32(filter_cursor); if ((filter_value >> 16) != category) return 0u;
        if (value < filter_value) cursor += 4u;
        else { filter_cursor += 4u; if (filter_value == value) return 1u; }
    }
    return 0u;
}

uint32 level_sub_80064870(uint32 object)
{
    FUNCTION_MARKER(0x80064870u, "LEVEL.BIN");
    return object ? r_u32(object + 12u) & 0x80000u : 1u;
}

uint32 level_sub_800830BC(uint32 object, uint32 other)
{
    uint32 state, other_state, identity, index = 0u, stats;
    FUNCTION_MARKER(0x800830BCu, "LEVEL.BIN");
    if (r_u32(other + 12u) & 0x20180000u) return 0u;
    state = r_u32(object + 0x388u); identity = r_u32(state); other_state = r_u32(other + 0x388u);
    identity = r_u16(0x8009E8F2u + identity * 20u); stats = other_state;
    if ((sint32)r_u32(0x8009E874u) > 0) {
        do {
            if (r_u8(0x8009E878u + index) == identity) {
                if ((sint16)r_u16(stats + 0x56u) < (sint16)r_u16(stats + 0x5Eu)) {
                    moh_draft_missing_call(0x8005F60Cu, 2u);
                    moh_draft_missing_call(0x8005F3E8u, r_u32(0x8009367Cu + identity * 68u), 10u, 0x46u, 0u,
                        0u, 0u, 1u, 0x28u, 2u, 1u << (r_u8(other_state + 0x152u) & 31u), 0x28u, 11u, 10u, 8u, 4u, 0u);
                    moh_draft_missing_call(0x80041CE8u, 0u, 0x20u, other, 0x10000u, 0u, 0x10000u, 0x3333u, 0u, 0u, 0u, 0u);
                    moh_draft_missing_call(0x8004301Cu, other, index, r_u32(state + 0x14u));
                    moh_draft_missing_call(0x8006B8F0u, object);
                }
                return 1u;
            }
            ++index; stats += 2u;
        } while ((sint32)index < (sint32)r_u32(0x8009E874u));
    }
    if ((sint32)index >= 4) moh_draft_missing_call(0x80089B04u, 2u, 8u);
    return 0u;
}

void level_sub_8005F60C(uint32 type)
{
    uint32 index = 0u, record = 0x80099F4Cu;
    FUNCTION_MARKER(0x8005F60Cu, "LEVEL.BIN");
    type &= 0xFFu;
    while ((sint32)index < (sint32)r_u32(0x80099ED0u)) {
        if (r_u8(record + 31u) == type) level_sub_8005F5A0(index);
        else { record += 32u; ++index; }
    }
}

void level_sub_8004301C(uint32 object, uint32 index, uint32 amount)
{
    uint32 state, slot, entry, flags;
    sint32 current, maximum, sound, kind;
    FUNCTION_MARKER(0x8004301Cu, "LEVEL.BIN");
    state = r_u32(object + 0x388u); slot = state + index * 2u;
    current = (sint16)r_u16(slot + 0x56u);
    if (current < 0) w_u16(slot + 0x56u, (uint16)amount);
    else {
        maximum = (sint16)r_u16(slot + 0x5Eu); amount += (uint32)current;
        w_u16(slot + 0x56u, (uint16)((sint32)amount < maximum ? amount : (uint32)maximum));
    }
    moh_draft_missing_call(0x800467ECu, object);
    entry = 0x80093640u + (uint32)(sint32)(sint8)r_u8(0x8009E878u + index) * 68u;
    kind = (sint16)r_u16(entry + 0x20u); if (kind < 0) return;
    flags = (r_u16(0x8009366Au + r_u8(state + 0x55u) * 68u) >> 3) & 2u;
    sound = (sint16)r_u16(entry + 0x22u);
    if (sound < 0) sound = (sint16)r_u16(r_u32(entry + 0x40u) + 0xC8u);
    moh_draft_missing_call(0x80041CE8u, (uint32)sound, (uint32)kind, object, 0x10000u,
        0u, 0x7FFFu, 0u, 0u, 0u, 0u, flags);
}

uint32 level_sub_8005F3E8(uint32 resource, uint32 x, uint32 y, uint32 red, uint32 green, uint32 blue,
    uint32 enabled, uint32 size, uint32 kind, uint32 mask, uint32 extra, uint32 duration,
    uint32 red_step, uint32 green_step, uint32 blue_step, uint32 type)
{
    uint32 index, record, magnitude;
    sint32 first, second, third;
    FUNCTION_MARKER(0x8005F3E8u, "LEVEL.BIN");
    red &= 0xFFu; green &= 0xFFu; blue &= 0xFFu;
    red_step &= 0xFFu; green_step &= 0xFFu; blue_step &= 0xFFu;
    index = moh_draft_missing_call(0x8005F28Cu, resource, x, y, red, green, blue, enabled & 0xFFu, size, kind & 0xFFu, mask, type) & 0xFFu;
    /* Original byte-masked result cannot equal the word sentinel */
    if ((sint32)duration < 0) {
        magnitude = 0u - duration;
        if (!blue_step || !green_step || !red_step) moh_draft_fail_fast("Effect color division by zero", 0x8005F3E8u);
        first = (sint32)(magnitude - blue) / (sint32)blue_step;
        second = (sint32)(magnitude - green) / (sint32)green_step;
        third = (sint32)(magnitude - red) / (sint32)red_step;
        if (second < first) first = second; if (third < first) first = third; duration = (uint32)first;
    }
    record = 0x80099F4Cu + index * 32u;
    w_u32(record + 0x14u, duration); w_u8(record + 0x1Cu, (uint8)red_step);
    w_u8(record + 0x1Du, (uint8)green_step); w_u8(record + 0x1Eu, (uint8)blue_step);
    w_u32(record + 0x10u, extra); return index;
}

uint32 level_sub_800670E8(uint32 message, uint32 actor, uint32 trigger, uint32 sender, uint32 identity)
{
    uint32 next, handle, resource, slot, current;
    FUNCTION_MARKER(0x800670E8u, "LEVEL.BIN");
    next = r_u32(0x8009D5BCu) + 1u;
    if ((sint32)next < 0x80 ? r_u32(0x8009D5B8u) == next : !r_u32(0x8009D5B8u)) {
        moh_draft_missing_call(0x80015B28u, 0x8003CC30u); return 0u;
    }
    handle = r_u32(message); resource = handle ? r_u32(0x8009D678u + (handle & 0xFFFFu) * 4u) : 0u;
    if (resource) {
        handle = r_u32(message); resource = handle ? r_u32(0x8009D678u + (handle & 0xFFFFu) * 4u) : 0u;
        if (level_sub_80064870(resource)) return 1u;
    }
    if (actor && level_sub_80064870(actor)) actor = 0u;
    if (sender && level_sub_80064870(sender)) sender = 0u;
    slot = 0x800B6460u + r_u32(0x8009D5BCu) * 32u;
    w_u32(slot, r_u32(message + 0x18u)); w_u32(slot + 4u, r_u32(message));
    w_u32(slot + 0x10u, r_u32(message + 4u)); level_sub_8006D84C(slot + 8u, actor);
    slot = 0x800B6460u + r_u32(0x8009D5BCu) * 32u;
    w_u32(slot + 0x14u, trigger); level_sub_8006D84C(slot + 12u, sender);
    current = r_u32(0x8009D5BCu); slot = 0x800B6460u + current * 32u;
    w_u32(slot + 0x1Cu, r_u32(message + 0x1Cu)); next = current + 1u; w_u32(slot + 0x18u, identity);
    w_u32(0x8009D5BCu, (sint32)next < 0x80 ? next : 0u); return 1u;
}

static sint32 moh_collision_edge(uint32 first, uint32 second, uint32 corner)
{
    xport_gte_write_data(12u, first); xport_gte_write_data(13u, second); xport_gte_write_data(14u, corner);
    xport_gte_execute(0x1400006u); return (sint32)xport_gte_read_data(24u);
}

static uint32 moh_collision_pair(sint32 horizontal, sint32 vertical)
{
    return (uint16)horizontal | ((uint32)(uint16)vertical << 16);
}

static uint32 moh_sky_step(uint32 color, uint32 divisor)
{
    switch (divisor) {
    case 2u: return color >> 1;
    case 3u: return (color * 0x555u) >> 12;
    case 4u: return color >> 2;
    case 5u: return (color * 0x333u) >> 12;
    case 6u: return (color * 0x2ABu) >> 12;
    case 7u: return (color * 0x249u) >> 12;
    case 8u: return color >> 3;
    default: return 0u;
    }
}

static void moh_sky_band(uint32 packet, uint32 red, uint32 green, uint32 blue, uint32 dr, uint32 dg, uint32 db)
{
    w_u8(packet + 20u, (uint8)red); w_u8(packet + 28u, (uint8)red);
    w_u8(packet + 21u, (uint8)green); w_u8(packet + 29u, (uint8)green);
    w_u8(packet + 22u, (uint8)blue); w_u8(packet + 30u, (uint8)blue);
    w_u8(packet + 4u, (uint8)(red + dr)); w_u8(packet + 5u, (uint8)(green + dg)); w_u8(packet + 6u, (uint8)(blue + db));
    w_u8(packet + 12u, (uint8)(red + dr)); w_u8(packet + 13u, (uint8)(green + dg)); w_u8(packet + 14u, (uint8)(blue + db));
}

void level_sub_80088A5C(uint32 buffer_index)
{
    uint32 packet, horizon, index, dr = 0u, dg = 0u, db = 0u, red = 0u, green = 0u, blue = 0u, count;
    sint32 angle, band;
    FUNCTION_MARKER(0x80088A5Cu, "LEVEL.BIN");
    packet = 0x800B7850u + buffer_index * 0x120u;
    angle = (sint16)r_u16(r_u32(r_u32(0x8009D654u) + 0x388u) + 0x17Au) >> 2;
    if (angle > 0x7F) angle = 0x7F; if (angle < -0x7F) angle = -0x80;
    band = (angle + 1) >> 5; horizon = (uint32)((band >= 0 ? 4 : 3) - band);
    w_u8(0x8009EC92u, (uint8)horizon); horizon &= 0xFFu;
    if (horizon) {
        dr = moh_sky_step(r_u8(0x800B7A94u), horizon + 1u);
        dg = moh_sky_step(r_u8(0x800B7A95u), horizon + 1u);
        db = moh_sky_step(r_u8(0x800B7A96u), horizon + 1u);
        index = 0u;
        do {
            moh_sky_band(packet, red, green, blue, dr, dg, db);
            red += dr; green += dg; blue += db; ++index; packet += 36u;
        } while (index <= r_u8(0x8009EC92u));
    }
    horizon = r_u8(0x8009EC92u);
    if (horizon < 7u) {
        dr = moh_sky_step((uint32)r_u8(0x800B7A91u) - r_u8(0x800B7A94u), 8u - horizon);
        dg = moh_sky_step((uint32)r_u8(0x800B7A92u) - r_u8(0x800B7A95u), 8u - horizon);
        db = moh_sky_step((uint32)r_u8(0x800B7A93u) - r_u8(0x800B7A96u), 8u - horizon);
    }
    red = r_u8(0x800B7A94u); green = r_u8(0x800B7A95u); blue = r_u8(0x800B7A96u);
    count = 7u - r_u8(0x8009EC92u);
    for (index = 0u; (sint32)index < (sint32)count; ++index) {
        moh_sky_band(packet, red, green, blue, dr, dg, db);
        red += dr; green += dg; blue += db; packet += 36u;
    }
}

void level_sub_80064550(void)
{
    uint32 index, record, packet, tag, value, address;
    FUNCTION_MARKER(0x80064550u, "LEVEL.BIN");
    for (index = 0u; index < 12u; ++index) {
        record = 0x8009C844u + index * 88u;
        if (!r_u8(record + 4u) || (r_u32(record) & 8u)) continue;
        packet = 0x8009C84Cu + r_u32(0x8009C83Cu) * 40u + index * 88u;
        tag = r_u32(0x8009C838u) + (uint32)(sint32)(sint8)r_u8(record + 7u) * 4u;
        value = r_u32(packet); address = r_u32(tag);
        w_u32(packet, (value & 0xFF000000u) | (address & 0xFFFFFFu));
        tag = (uint32)(sint32)(sint8)r_u8(record + 7u) * 4u + r_u32(0x8009C838u);
        address = 0x8009C84Cu + index * 88u + r_u32(0x8009C83Cu) * 40u;
        value = r_u32(tag); w_u32(tag, (value & 0xFF000000u) | (address & 0xFFFFFFu));
    }
}

uint32 level_sub_8007711C(uint32 point, uint32 radius, uint32 height, uint32 first, uint32 second, uint32 third, uint32 plane)
{
    uint32 vertices[3] = { first, second, third }, axis, index, half, center_y, horizontal;
    sint32 normal_x, normal_z, abs_x, abs_z, ax, ay, bx, by, sx, sy, projected;
    uint32 positive;
    FUNCTION_MARKER(0x8007711Cu, "LEVEL.BIN");
    half = (uint32)((sint32)height >> 1); center_y = (uint32)(sint32)(sint16)r_u16(point + 2u) + half;
    normal_x = (sint16)r_u16(plane); normal_z = (sint16)r_u16(plane + 4u);
    abs_x = normal_x < 0 ? -normal_x : normal_x; abs_z = normal_z < 0 ? -normal_z : normal_z;
    axis = abs_x < abs_z ? 0u : 4u;
    horizontal = (uint32)(sint32)(sint16)r_u16(point + axis);
    positive = axis == 0u ? normal_z < 0 : normal_x > 0;
    for (index = 0u; index < 3u; ++index) {
        ax = (sint16)r_u16(vertices[index] + axis); ay = (sint16)r_u16(vertices[index] + 2u);
        bx = (sint16)r_u16(vertices[(index + 1u) % 3u] + axis); by = (sint16)r_u16(vertices[(index + 1u) % 3u] + 2u);
        if (positive) {
            sx = (sint32)(ay <= by ? horizontal - radius : horizontal + radius);
            sy = (sint32)(ax <= bx ? center_y + half : center_y - half);
        } else {
            sx = (sint32)(ay <= by ? horizontal + radius : horizontal - radius);
            sy = (sint32)(ax <= bx ? center_y - half : center_y + half);
        }
        projected = moh_collision_edge(moh_collision_pair(ax, ay), moh_collision_pair(bx, by), moh_collision_pair(sx, sy));
        if (positive ? projected < 0 : projected > 0) return 0u;
    }
    return 1u;
}

uint32 level_sub_800776C0(uint32 point, uint32 radius, uint32 height, uint32 filter, uint32 first, uint32 second, uint32 third, uint32 plane)
{
    uint32 vertices[3] = { first, second, third }, magnitude, radius_squared, second_square, mask, index, vertical;
    sint32 x, z, y, top, a, b, c, minimum, maximum, normal_x, normal_y, normal_z, ax, az, bx, bz, sx, sz, projected;
    uint32 packed[3];
    FUNCTION_MARKER(0x800776C0u, "LEVEL.BIN");
    x = (sint16)r_u16(point); z = (sint16)r_u16(point + 4u);
    a = (sint16)r_u16(first);
    if ((sint32)((uint32)x + radius) < a) return 0u;
    b = (sint16)r_u16(third); c = (sint16)r_u16(second); maximum = b > c ? b : c;
    if (maximum < (sint32)((uint32)x - radius)) return 0u;
    b = (sint16)r_u16(second + 4u); a = (sint16)r_u16(first + 4u); c = (sint16)r_u16(third + 4u);
    minimum = a < b ? a : b; if (c < minimum) minimum = c;
    if ((sint32)((uint32)z + radius) < minimum) return 0u;
    maximum = a > b ? a : b; if (c > maximum) maximum = c;
    if (maximum < (sint32)((uint32)z - radius)) return 0u;
    a = (sint16)r_u16(first + 2u); b = (sint16)r_u16(second + 2u); y = (sint16)r_u16(point + 2u);
    minimum = a < b ? a : b; maximum = a > b ? a : b; c = (sint16)r_u16(third + 2u);
    if (c < minimum) minimum = c; else if (c > maximum) maximum = c;
    if (maximum < y) return 0u;
    top = (sint32)((uint32)y + height); if (top < minimum) return 0u;
    magnitude = (sint32)radius < 0 ? 0u - radius : radius;
    radius_squared = moh_collision_square(magnitude & 0xFFFFu, magnitude >> 16, 0);
    normal_z = (sint16)r_u16(plane + 4u); normal_x = (sint16)r_u16(plane); normal_y = (sint16)r_u16(plane + 2u);
    if (normal_z < 0) normal_z = -normal_z; if (normal_x < 0) normal_x = -normal_x; if (normal_y < 0) normal_y = -normal_y;
    vertical = (uint32)(normal_y < (normal_x > normal_z ? normal_x : normal_z));
    a = (sint16)r_u16(first + 2u); b = (sint16)r_u16(second + 2u); c = (sint16)r_u16(third + 2u);
    mask = (uint32)(a < top && y < a) | ((uint32)(b < top && y < b) << 1) | ((uint32)(c < top && y < c) << 2);
    for (index = 0u; index < 3u; ++index) {
        if (!(mask & (1u << index))) continue;
        a = (uint16)(r_u16(vertices[index]) - (uint32)x); b = (uint16)(r_u16(vertices[index] + 4u) - (uint32)z);
        magnitude = moh_collision_square((uint32)a, (uint32)b, &second_square);
        if (magnitude + second_square < radius_squared) {
            if (vertical || !filter) return 1u;
            return (sint16)r_u16(plane + 2u) >= 0;
        }
    }
    if (vertical) return level_sub_8007711C(point, radius, height, first, second, third, plane);
    if (filter && (sint16)r_u16(plane + 2u) < 0) return 0u;
    ax = (sint16)r_u16(first); az = (sint16)r_u16(first + 4u);
    bx = (sint16)r_u16(second); bz = (sint16)r_u16(second + 4u);
    packed[0] = moh_collision_pair(ax, az); packed[1] = moh_collision_pair(bx, bz);
    normal_y = (sint16)r_u16(plane + 2u);
    for (index = 0u; index < 3u; ++index) {
        if (index) {
            ax = (sint16)r_u16(vertices[index]); az = (sint16)r_u16(vertices[index] + 4u);
            bx = (sint16)r_u16(vertices[(index + 1u) % 3u]); bz = (sint16)r_u16(vertices[(index + 1u) % 3u] + 4u);
        }
        if (normal_y >= 0) {
            sx = (sint32)(az <= bz ? (uint32)x - radius : (uint32)x + radius);
            sz = (sint32)(ax <= bx ? (uint32)z + radius : (uint32)z - radius);
        } else {
            sx = (sint32)(az <= bz ? (uint32)x + radius : (uint32)x - radius);
            sz = (sint32)(ax <= bx ? (uint32)z - radius : (uint32)z + radius);
        }
        projected = moh_collision_edge(packed[index], packed[(index + 1u) % 3u], moh_collision_pair(sx, sz));
        if (!index) {
            a = (sint16)r_u16(third); b = (sint16)r_u16(third + 4u); packed[2] = moh_collision_pair(a, b);
        }
        if (normal_y >= 0 ? projected < 0 : projected > 0) return 0u;
    }
    return 1u;
}

sint32 level_sub_8007848C(uint32 point, uint32 radius, uint32 height, uint32 faces, uint32 indices,
    uint32 face_count, uint32 movement, uint32 filter, uint32 auxiliary, uint32 planes, uint32 distances,
    uint32 capacity, uint32 initial_count, uint32 mode)
{
    uint32 count = initial_count, index = 0u, face, record, vertices, plane, first, second, third, accepted, duplicate;
    uint32 auxiliary_out = auxiliary + count * 2u, planes_out = planes + count * 4u, distances_out = distances + count * 2u;
    uint32 copy_out = 0x8009E430u + count * 32u, words[2], copy_index, source;
    FUNCTION_MARKER(0x8007848Cu, "LEVEL.BIN");
    face_count = (uint32)(sint32)(sint16)face_count;
    while ((sint32)index < (sint32)face_count) {
        face = faces + (uint32)(sint32)(sint16)r_u16(indices) * 10u;
        record = r_u32(0x800B7848u);
        plane = (uint32)(sint32)(sint16)r_u16(face + 6u);
        first = (uint32)(sint32)(sint16)r_u16(face); second = (uint32)(sint32)(sint16)r_u16(face + 2u);
        third = (uint32)(sint32)(sint16)r_u16(face + 4u); accepted = r_u16(face + 8u);
        vertices = r_u32(record + 0x1Cu); plane = r_u32(record + 0x20u) + plane * 8u;
        first = vertices + first * 8u; second = vertices + second * 8u; third = vertices + third * 8u;
        w_u16(plane + 6u, (uint16)accepted);
        if (!moh_draft_missing_call(0x800776C0u, point, radius, height, filter, first, second, third, plane)) goto next_face;
        accepted = 1u;
        if (mode) {
            xport_gte_write_control(0u, r_u32(movement)); xport_gte_write_control(1u, r_u32(movement + 4u));
            xport_gte_write_data(9u, r_u16(plane)); xport_gte_write_data(10u, r_u16(plane + 2u)); xport_gte_write_data(11u, r_u16(plane + 4u));
            xport_gte_execute(0x41E012u); accepted = xport_gte_read_data(25u) >> 31;
        }
        if (!accepted || !moh_draft_missing_call(0x80078350u, point, radius, height, plane)) goto next_face;
        duplicate = 0u;
        if ((sint32)count > 0) {
            accepted = r_u16(indices); source = auxiliary;
            while ((sint32)duplicate < (sint32)count && (sint16)r_u16(source) != (sint16)accepted) { ++duplicate; source += 2u; }
            if ((sint32)duplicate < (sint32)count) goto next_face;
        }
        if (!mode) {
            for (copy_index = 0u; copy_index < 4u; ++copy_index) {
                source = copy_index == 0u ? first : copy_index == 1u ? second : copy_index == 2u ? third : plane;
                words[0] = r_u32(source); words[1] = r_u32(source + 4u);
                w_u32(copy_out + copy_index * 8u, words[0]); w_u32(copy_out + copy_index * 8u + 4u, words[1]);
            }
        }
        w_u32(planes_out, plane); planes_out += 4u; copy_out += 32u;
        accepted = r_u16(face + 8u); ++count; w_u16(distances_out, (uint16)accepted); distances_out += 2u;
        w_u16(auxiliary_out, r_u16(indices)); auxiliary_out += 2u;
        if ((sint32)count >= (sint32)capacity) return (sint32)(count - initial_count);
next_face:
        indices += 2u; ++index;
    }
    return (sint32)(count - initial_count);
}

sint32 level_sub_800787C0(uint32 point, uint32 radius, uint32 height, uint32 movement,
    uint32 auxiliary, uint32 planes, uint32 distances, uint32 capacity, uint32 filter, uint32 mode)
{
    struct { sint16 node, left, right, near_z, far_z; } pending[8];
    uint32 record, nodes, node, faces, count = 0u, depth = 0u, index;
    sint32 x, z, left, right, near_z, far_z, split, child, added;
    FUNCTION_MARKER(0x800787C0u, "LEVEL.BIN");
    x = (sint16)r_u16(point); record = r_u32(0x800B7848u); z = (sint16)r_u16(point + 4u);
    left = (sint16)r_u16(record); right = (sint16)r_u16(record + 4u);
    near_z = (sint16)r_u16(record + 2u); far_z = (sint16)r_u16(record + 6u);
    nodes = r_u32(record + 0x14u); faces = r_u32(record + 0x24u); node = nodes;
    for (;;) {
        if ((sint16)r_u16(node) < 0) {
            record = r_u32(0x800B7848u);
            index = r_u32(record + 0x18u) + (uint32)(sint32)(sint16)r_u16(node + 2u) * 2u;
            child = (sint16)(uint16)~r_u16(node);
            added = level_sub_8007848C(point, radius, height, faces, index, (uint32)child, movement,
                filter, auxiliary, planes, distances, capacity, count, mode);
            if (added > 0) { count += (uint32)added; if ((sint32)count >= (sint32)capacity) return (sint32)count; }
            if (!depth) return (sint32)count;
            --depth; left = pending[depth].left; right = pending[depth].right;
            near_z = pending[depth].near_z; far_z = pending[depth].far_z; child = pending[depth].node;
        } else {
            child = (sint16)r_u16(node + 2u);
            if (child >= 0) {
                split = (sint32)((uint32)left + r_u16(node + 4u));
                if ((sint32)((uint32)x - radius) < split) {
                    if (split < (sint32)((uint32)x + radius) && depth < 8u) {
                        pending[depth].left = (sint16)split; pending[depth].right = (sint16)right;
                        pending[depth].near_z = (sint16)near_z; pending[depth].far_z = (sint16)far_z;
                        pending[depth].node = (sint16)r_u16(node + 2u); ++depth;
                    }
                    child = (sint16)r_u16(node); right = (sint32)((uint32)split - 1u);
                } else left = split;
            } else {
                split = (sint32)((uint32)near_z + r_u16(node + 4u));
                if ((sint32)((uint32)z - radius) < split) {
                    if (split < (sint32)((uint32)z + radius) && depth < 8u) {
                        pending[depth].left = (sint16)left; pending[depth].right = (sint16)right;
                        pending[depth].near_z = (sint16)split; pending[depth].far_z = (sint16)far_z;
                        pending[depth].node = (sint16)(uint16)~r_u16(node + 2u); ++depth;
                    }
                    child = (sint16)r_u16(node); far_z = (sint32)((uint32)split - 1u);
                } else { near_z = split; child = ~child; }
            }
        }
        node = nodes + (uint32)child * 8u;
    }
}

void level_sub_80089A0C(uint32 view)
{
    uint32 buffer_index, packet, ordering_table, index;
    FUNCTION_MARKER(0x80089A0Cu, "LEVEL.BIN");
    buffer_index = r_u32(view + 0x221Cu);
    if (r_u32(0x8009EC88u) == buffer_index) return;
    packet = 0x800B7850u + buffer_index * 0x120u;
    index = r_u32(0x8009A5E0u); ordering_table = r_u32(view + 0x2218u);
    w_u32(0x8009EC88u, buffer_index);
    if (!index) {
        if (r_u32(0x800939F4u) == 2u && r_u32(0x800939F8u) == 3u)
            moh_draft_missing_call(0x80089174u, buffer_index, 0x800939F4u);
        else moh_draft_missing_call(0x80088A5Cu, buffer_index, 0x800939F4u);
    }
    for (index = 0u; index < 8u; ++index, packet += 36u) {
        w_u32(packet, (r_u32(packet) & 0xFF000000u) | (r_u32(ordering_table + 0x7FCu) & 0xFFFFFFu));
        w_u32(ordering_table + 0x7FCu, (r_u32(ordering_table + 0x7FCu) & 0xFF000000u) | (packet & 0xFFFFFFu));
    }
}

uint32 level_sub_80053BD4(uint32 buffer_index, uint32 end_out)
{
    FUNCTION_MARKER(0x80053BD4u, "LEVEL.BIN");
    w_u32(end_out, r_u32(0x8009895Cu)); return r_u32(0x80098958u);
}

void level_sub_80053BF4(uint32 cursor)
{
    FUNCTION_MARKER(0x80053BF4u, "LEVEL.BIN");
    w_u32(0x80098958u, cursor);
}

void level_sub_80088280(uint32 view, uint32 buffer_index, uint32 ordering_table)
{
    uint32 axis, packet, x1, y1, x2, y3;
    PsxGteSnapshot snapshot;
    FUNCTION_MARKER(0x80088280u, "LEVEL.BIN");
    for (axis = 0u; axis < 6u; ++axis) xport_gte_write_data(axis, r_u32(0x800B7AF0u + axis * 4u));
    xport_gte_execute(0x280030u); psx_gte_snapshot(&snapshot);
    if ((sint32)snapshot.flag < 0) return;
    packet = 0x800B7AA0u + buffer_index * 40u;
    w_u32(packet + 8u, xport_gte_read_data(12u)); w_u32(packet + 16u, xport_gte_read_data(13u)); w_u32(packet + 24u, xport_gte_read_data(14u));
    x1 = r_u16(packet + 8u); y1 = r_u16(packet + 10u); x2 = r_u16(packet + 16u); y3 = r_u16(packet + 26u);
    w_u16(packet + 18u, (uint16)y1); w_u16(packet + 32u, (uint16)x2);
    w_u16(packet + 24u, (uint16)x1); w_u16(packet + 34u, (uint16)y3);
    w_u32(packet, (r_u32(packet) & 0xFF000000u) | (r_u32(ordering_table + 0x7F8u) & 0xFFFFFFu));
    w_u32(ordering_table + 0x7F8u, (r_u32(ordering_table + 0x7F8u) & 0xFF000000u) | (packet & 0xFFFFFFu));
}

uint32 level_sub_80088368(uint32 view)
{
    uint32 buffer_index, result, ordering_table, local, packet, group, group_byte, index, axis, offset, base;
    uint32 vector = 0x8009EC94u, color = 0x8009F48Cu, group_count = 0u, controls[8], red, green, blue, value;
    MATRIX saved;
    PsxGteSnapshot snapshot;
    FUNCTION_MARKER(0x80088368u, "LEVEL.BIN");
    buffer_index = r_u32(view + 0x221Cu); result = r_u32(0x8009EC8Cu);
    if (result == buffer_index) return result;
    ordering_table = r_u32(view + 0x2218u);
    local = sub_80016718(4u); if (!local) moh_draft_fail_fast("Background packet bound allocation", 0x80088368u);
    packet = level_sub_80053BD4(buffer_index, local); group = packet;
    w_u32(0x8009EC8Cu, buffer_index);
    ReadRotMatrix(&saved);
    for (axis = 0u; axis < 4u; ++axis) {
        index = axis * 2u;
        controls[axis] = (uint16)saved.m[index / 3u][index % 3u] | ((uint32)(uint16)saved.m[(index + 1u) / 3u][(index + 1u) % 3u] << 16);
    }
    controls[4] = (uint32)(sint32)saved.m[2][2];
    for (axis = 0u; axis < 3u; ++axis) controls[5u + axis] = (uint32)saved.t[axis];
    offset = r_u32(0x8009D320u); base = 0x8009D324u + offset;
    for (axis = 0u; axis < 8u; ++axis) w_u32(base + axis * 4u, controls[axis]);
    w_u32(0x8009D320u, offset + 32u);
    for (axis = 0u; axis < 5u; ++axis) xport_gte_write_control(axis, r_u32(0x8009C804u + axis * 4u));
    for (axis = 5u; axis < 8u; ++axis) xport_gte_write_control(axis, 0u);
    result = r_u32(local) < packet + 0xBF4u; sub_80036E44(local);
    /* Original capacity exit retains the pushed matrix and buffer marker */
    if (result) return result;
    group_byte = packet + 3u;
    for (index = 0u; index < 255u; ++index, vector += 8u, color += 4u) {
        xport_gte_write_data(0u, r_u32(vector)); xport_gte_write_data(1u, r_u32(vector + 4u));
        xport_gte_execute(0x180001u); psx_gte_snapshot(&snapshot);
        if ((uint32)snapshot.flag & 0x01806000u) continue;
        w_u8(0x8009EC84u, (uint8)((r_u8(0x8009EC84u) + 1u) & 7u));
        w_u8(packet + 3u, 2u); w_u8(packet + 7u, 0x68u);
        red = r_u8(color); green = r_u8(color + 1u); blue = r_u8(color + 2u);
        if (moh_draft_missing_call(0x8001552Cu) & 3u) {
            if (red > 32u) red -= 32u; if (green > 32u) green -= 32u; if (blue > 32u) blue -= 32u;
        }
        w_u32(packet + 8u, xport_gte_read_data(14u));
        w_u8(packet + 4u, (uint8)red); w_u8(packet + 5u, (uint8)green); w_u8(packet + 6u, (uint8)blue);
        if (group_count) {
            value = r_u8(group_byte); w_u32(packet, 0u); w_u8(group_byte, (uint8)(value + 3u));
            if (group_count == 4u) {
                group_count = 0xFFFFFFFFu; group_byte += 60u;
                w_u32(group, (r_u32(group) & 0xFF000000u) | (r_u32(ordering_table + 0x7FCu) & 0xFFFFFFu));
                value = group & 0xFFFFFFu; group += 60u;
                w_u32(ordering_table + 0x7FCu, (r_u32(ordering_table + 0x7FCu) & 0xFF000000u) | value);
            }
        }
        ++group_count; packet += 12u;
    }
    if (group_count) {
        w_u32(group, (r_u32(group) & 0xFF000000u) | (r_u32(ordering_table + 0x7FCu) & 0xFFFFFFu));
        w_u32(ordering_table + 0x7FCu, (r_u32(ordering_table + 0x7FCu) & 0xFF000000u) | (group & 0xFFFFFFu));
    }
    level_sub_80053BF4(packet);
    if (r_u32(0x8009EC68u)) level_sub_80088280(view, buffer_index, ordering_table);
    offset = r_u32(0x8009D320u) - 32u; base = 0x8009D324u + offset; w_u32(0x8009D320u, offset);
    for (axis = 0u; axis < 8u; ++axis) {
        value = r_u32(base + axis * 4u); xport_gte_write_control(axis, value); if (axis == 6u) result = value;
    }
    return result;
}

void level_sub_80053B8C(uint32 buffer_index)
{
    uint32 state, length, first;
    FUNCTION_MARKER(0x80053B8Cu, "LEVEL.BIN");
    state = r_u32(0x8009CC64u); length = r_u32(state + 0x98u);
    first = r_u32(0x80098980u) + length * buffer_index;
    w_u32(0x80098958u, first); w_u32(0x8009895Cu, first + r_u32(state + 0x98u) - 1u);
}

void level_sub_80050548(uint32 view)
{
    uint32 data, source, destination, offset, word, words[4], elapsed, ordering_table;
    FUNCTION_MARKER(0x80050548u, "LEVEL.BIN");
    data = r_u32(r_u32(r_u32(0x8009D654u) + 0x388u) + 0xA4u);
    w_u32(0x8009A628u, data); moh_draft_missing_call(0x8001BA68u, data);
    source = r_u32(view + 0x388u) + 0xE4u; destination = 0x8009C7E8u;
    for (offset = 0u; offset < 48u; offset += 16u) {
        for (word = 0u; word < 4u; ++word) words[word] = r_u32(source + offset + word * 4u);
        for (word = 0u; word < 4u; ++word) w_u32(destination + offset + word * 4u, words[word]);
    }
    for (word = 0u; word < 3u; ++word) words[word] = r_u32(source + 48u + word * 4u);
    for (word = 0u; word < 3u; ++word) w_u32(destination + 48u + word * 4u, words[word]);
    ordering_table = 0x8009A7C8u + (r_u32(0x8009C824u) << 12);
    w_u32(0x8009C838u, ordering_table); moh_draft_missing_call(0x8001722Cu, ordering_table, 0x200u);
    elapsed = r_u32(0x80098958u) - r_u32(0x80097FC4u);
    if ((sint32)r_u32(0x80097FD8u) < (sint32)elapsed) w_u32(0x80097FD8u, elapsed);
    data = r_u32(0x8009C83Cu) == 0u; w_u32(0x8009C83Cu, data);
    level_sub_80053B8C(data); w_u32(0x80097FC4u, r_u32(0x80098958u));
}

void level_sub_800503D4(void)
{
    FUNCTION_MARKER(0x800503D4u, "LEVEL.BIN");
    level_sub_80050548(r_u32(0x8009D638u));
    if (r_u8(r_u32(0x8009CC64u) + 0x58Fu)) moh_draft_missing_call(0x80088368u, 0x8009A620u);
    if (r_u32(r_u32(0x8009CC64u) + 0x58Cu) & 0xFFFFFFu) moh_draft_missing_call(0x80089A0Cu, 0x8009A620u);
    moh_draft_missing_call(0x80064550u);
    moh_draft_missing_call(0x8005F86Cu, r_u32(0x8009C838u), r_u32(0x8009C83Cu), 0u);
    moh_draft_missing_call(0x8006C714u, 0x8009A620u, r_u32(0x8009D64Cu));
    moh_draft_missing_call(0x8008019Cu, r_u32(0x8009D654u), 0x8009A620u);
    moh_draft_missing_call(0x800504F8u); w_u32(0x80097FB0u, 1u);
}

void level_sub_8005F5A0(uint32 index)
{
    uint32 count, source, destination, offset, words[4];
    FUNCTION_MARKER(0x8005F5A0u, "LEVEL.BIN");
    count = r_u32(0x80099ED0u) - 1u; w_u32(0x80099ED0u, count);
    source = 0x80099F4Cu + count * 32u; destination = 0x80099F4Cu + index * 32u;
    for (offset = 0u; offset < 4u; ++offset) words[offset] = r_u32(source + offset * 4u);
    for (offset = 0u; offset < 4u; ++offset) w_u32(destination + offset * 4u, words[offset]);
    for (offset = 0u; offset < 4u; ++offset) words[offset] = r_u32(source + 16u + offset * 4u);
    for (offset = 0u; offset < 4u; ++offset) w_u32(destination + 16u + offset * 4u, words[offset]);
}

void level_sub_8005F69C(void)
{
    uint32 index = 0u, record, timer, hold;
    sint32 red, green, blue;
    FUNCTION_MARKER(0x8005F69Cu, "LEVEL.BIN");
    while ((sint32)index < (sint32)r_u32(0x80099ED0u)) {
        record = 0x80099F4Cu + index * 32u; timer = r_u32(record + 12u);
        if (timer == 0xFFFFFFFFu) ++index;
        else {
            --timer; w_u32(record + 12u, timer);
            if ((sint32)timer > 0) ++index;
            else level_sub_8005F5A0(index);
        }
    }
    index = 0u; record = 0x80099F4Cu;
    while ((sint32)index < (sint32)r_u32(0x80099ED0u)) {
        timer = r_u32(record + 16u);
        if ((sint32)timer < 0) goto next_effect;
        if ((sint32)timer > 0) {
            hold = r_u32(record + 20u); w_u32(record + 16u, timer - 1u);
            if ((sint32)hold <= 0) goto next_effect;
            if (hold != 0x7FFFu) w_u32(record + 20u, hold - 1u);
            red = r_u8(record + 8u) + r_u8(record + 28u); if (red > 255) red = 255;
            green = r_u8(record + 9u) + r_u8(record + 29u); w_u8(record + 8u, (uint8)red); if (green > 255) green = 255;
            blue = r_u8(record + 10u) + r_u8(record + 30u); w_u8(record + 9u, (uint8)green); if (blue > 255) blue = 255;
        } else {
            red = r_u8(record + 8u) - r_u8(record + 28u); if (red <= 0) red = 0;
            green = r_u8(record + 9u) - r_u8(record + 29u); w_u8(record + 8u, (uint8)red); if (green <= 0) green = 0;
            blue = r_u8(record + 10u) - r_u8(record + 30u); w_u8(record + 9u, (uint8)green); if (blue <= 0) blue = 0;
        }
        w_u8(record + 10u, (uint8)blue);
next_effect:
        record += 32u; ++index;
    }
}

void level_sub_8006C08C(uint32 head)
{
    uint32 object = head, next, other, following, flags, callback, local, direction, first_out, second_out, distance_out;
    uint32 selected, selected_index, total, record, index, candidate, tag, contact, distance_squared;
    uint32 first_result, second_result, callback_result, closest_tag, closest_object;
    sint32 closest, distance, chosen_distance;
    VECTOR vector, square;
    FUNCTION_MARKER(0x8006C08Cu, "LEVEL.BIN");
    if (!head) return;
    local = sub_80016718(24u);
    if (!local) moh_draft_fail_fast("Object collision local allocation", 0x8006C08Cu);
    /* TODO: Original output locals and vector padding remain unspecified until written */
    direction = local; first_out = local + 8u; second_out = local + 12u; distance_out = local + 16u;
    while (object) {
        flags = r_u32(object + 12u); next = r_u32(object + 4u);
        if (flags & 0x80000u) goto next_object;
        flags = r_u32(object + 0x10u);
        if (flags & 2u) goto next_object;
        if (!(flags & 4u)) {
            other = next;
            while (other) {
                flags = r_u32(other + 12u); following = r_u32(other + 4u);
                if (!(flags & 0x80000u) && !(r_u32(other + 0x10u) & 6u) && (r_u32(object + 0x14u) & flags) &&
                    moh_draft_missing_call(0x8004CB18u, object, other)) {
                    callback = r_u32(object + 0x37Cu); if (callback) moh_draft_indirect_call(callback, object, other);
                    callback = r_u32(other + 0x37Cu); if (callback) moh_draft_indirect_call(callback, other, object);
                    if (moh_draft_missing_call(0x8004C990u, object, first_out, other, second_out)) {
                        callback = r_u32(object + 0x374u);
                        if (callback) moh_draft_indirect_call(callback, object, r_u32(first_out), other, r_u32(second_out));
                        callback = r_u32(other + 0x374u);
                        if (callback) moh_draft_indirect_call(callback, other, r_u32(second_out), object, r_u32(first_out));
                        if (r_u32(object + 12u) & 0x80000u) break;
                    }
                }
                other = following;
            }
            goto next_object;
        }
        closest = 999999999; tag = 0xFFFFFFFFu; selected = 0u; total = 0u; selected_index = 0u;
        contact = flags & 0x80000u ? distance_out : 0u;
        w_u16(direction, (uint16)((r_u32(object + 0x20Cu) - r_u32(object + 0x258u)) >> 16));
        w_u16(direction + 2u, (uint16)((r_u32(object + 0x210u) - r_u32(object + 0x25Cu)) >> 16));
        w_u16(direction + 4u, (uint16)((r_u32(object + 0x214u) - r_u32(object + 0x260u)) >> 16));
        vector.vx = (sint16)r_u16(direction); vector.vy = (sint16)r_u16(direction + 2u); vector.vz = (sint16)r_u16(direction + 4u); vector.pad = 0;
        Square0(&vector, &square); distance_squared = (uint32)square.vx + (uint32)square.vy + (uint32)square.vz;
        moh_draft_missing_call(0x80016A40u, direction, direction);
        if (!r_u16(direction) && !r_u16(direction + 2u) && !r_u16(direction + 4u)) {
            w_u16(direction, r_u16(object + 0x344u)); w_u16(direction + 2u, r_u16(object + 0x34Au)); w_u16(direction + 4u, r_u16(object + 0x350u));
        }
        other = head;
        while (other) {
            flags = r_u32(other + 12u); following = r_u32(other + 4u);
            if ((flags & 0x80000u) || (r_u32(other + 0x10u) & 6u) || !(r_u32(object + 0x14u) & flags) || other == object ||
                !moh_draft_missing_call(0x8004CD90u, object, other, direction, contact)) goto next_candidate;
            if (!(r_u32(object + 0x10u) & 0x80000u)) {
                callback = r_u32(object + 0x37Cu); if (callback) moh_draft_indirect_call(callback, object, other);
                if (moh_draft_missing_call(0x8004C968u, object, direction, distance_squared, other, second_out)) {
                    callback = r_u32(object + 0x374u);
                    if (callback) moh_draft_indirect_call(callback, object, 0xFFFFFFFFu, other, r_u32(second_out));
                    callback = r_u32(other + 0x374u);
                    if (callback) moh_draft_indirect_call(callback, other, r_u32(second_out), object, 0xFFFFFFFFu);
                    if (r_u32(object + 12u) & 0x80000u) break;
                }
            } else if (total < 159u) {
                if (r_u16(other + 0x36u)) {
                    if (!moh_draft_missing_call(0x8004C968u, object, direction, distance_squared, other, second_out)) goto next_candidate;
                    distance = (sint32)r_u32(distance_out);
                    if (distance < closest) { closest = distance; tag = r_u32(second_out); selected = other; selected_index = total; }
                    record = r_u32(0x8009D620u) + total * 12u; ++total;
                    distance = (sint16)r_u16(distance_out); candidate = r_u16(second_out);
                    w_u32(record, other); w_u16(record + 4u, (uint16)distance); w_u32(record + 8u, 0u); w_u16(record + 6u, (uint16)candidate);
                } else {
                    distance = (sint32)r_u32(distance_out);
                    if (distance < closest) { closest = distance; tag = 0xFFFFFFFFu; selected = other; selected_index = total; }
                    record = r_u32(0x8009D620u) + total * 12u; ++total; distance = (sint16)r_u16(distance_out);
                    w_u16(record + 4u, (uint16)distance); w_u32(record, other); w_u32(record + 8u, 0u); w_u16(record + 6u, 0xFFFFu);
                }
            }
next_candidate:
            other = following;
        }
        if (selected && (r_u32(object + 0x10u) & 0x80000u)) {
            first_result = 0u; second_result = 0u; closest_tag = 0u;
            for (;;) {
                w_u32(r_u32(0x8009D620u) + selected_index * 12u + 8u, 1u);
                callback = r_u32(object + 0x37Cu); callback_result = callback ? moh_draft_indirect_call(callback, object, selected) : 0u;
                if (!r_u16(selected + 0x36u) || tag == 0xFFFFFFFFu) { first_result = 0u; second_result = 0u; }
                else {
                    callback = r_u32(object + 0x374u);
                    if (callback) first_result = moh_draft_indirect_call(callback, object, 0xFFFFFFFFu, selected, tag);
                    callback = r_u32(selected + 0x374u);
                    if (callback) second_result = moh_draft_indirect_call(callback, selected, tag, object, 0xFFFFFFFFu);
                }
                if (callback_result || first_result || second_result) break;
                chosen_distance = 999999999; closest_object = 0u;
                record = r_u32(0x8009D620u);
                for (index = 0u; index < total; ++index, record += 12u) {
                    if (r_u32(record + 8u)) continue;
                    distance = (sint16)r_u16(record + 4u);
                    if (distance < (sint16)closest || distance >= chosen_distance) continue;
                    candidate = r_u32(record); if (candidate == selected) continue;
                    closest_object = candidate; closest_tag = r_u16(record + 6u); chosen_distance = distance; selected_index = index;
                }
                selected = closest_object; if (!selected) break;
                closest = chosen_distance; tag = (uint32)(sint32)(sint16)closest_tag;
            }
        }
        if (!(r_u32(object + 12u) & 0x80000u) && (r_u32(object + 0x10u) & 8u))
            moh_draft_indirect_call(r_u32(object + 0x378u), object, 0u);
next_object:
        object = next;
    }
    sub_80036E44(local);
}

void level_sub_8007D5C8(uint32 object, uint32 yaw, uint32 pitch, uint32 enabled)
{
    uint32 state, model, resource, first = 0u, second = 0u, mode = 0u, spawned = 0u;
    uint32 kind, row, data, volume = 0x17FFFu, value, ammunition, count, amount, magnitude, shift;
    sint32 sample, bank, rate;
    FUNCTION_MARKER(0x8007D5C8u, "LEVEL.BIN");
    state = r_u32(object + 0x388u);
    if (!(enabled & 0xFFu)) return;
    model = r_u32(object + 0x100u);
    if (r_u32(model + 0x84u)) {
        resource = r_u32(model + 8u); first = resource + 0xD8u; second = resource + 0xE0u;
        yaw = (uint32)(sint32)(sint16)r_u16(resource + 0xD4u); pitch = (uint32)(sint32)(sint16)r_u16(resource + 0xD6u);
    }
    ammunition = state + (uint32)(sint32)(sint8)r_u8(state + 0x54u) * 2u + 0x66u;
    if ((sint16)r_u16(ammunition) <= 0) return;
    value = r_u32(state + 0x1C8u);
    if (value == 0u || value == 1u || value == 7u || value == 8u) mode = 1u;
    w_u32(0x80039CA8u, r_u32(0x80039CA8u) + 1u);
    kind = r_u8(state + 0x55u); row = 0x80093640u + kind * 68u; data = r_u32(row + 0x2Cu);
    switch (kind) {
    case 0u: case 5u: case 8u: case 9u: case 12u:
        spawned = moh_draft_missing_call(0x80045A78u, object, yaw, pitch, 0u, 0u, first, second,
            kind == 12u ? 0x28000u : data, 0u, mode, 0x13F6u, 0x2000u, 0x20u, 0x08000450u);
        moh_draft_missing_call(0x8004D8DCu, r_u8(state + 0x152u), 0u, 100u, 3u, 0u);
        break;
    case 1u: case 6u: case 11u:
        spawned = moh_draft_missing_call(0x80045A78u, object, yaw, pitch, 0u, 0u, first, second,
            data, 11u, mode, 0x13F7u, 0x2000u, 0x20u, 0x08000450u);
        value = r_u32(state + 0x50u) + 0x10000u; w_u32(state + 0x50u, value);
        if ((sint32)value > 0x10000) w_u32(state + 0x50u, 0x10000u);
        moh_draft_missing_call(0x8004D8DCu, r_u8(state + 0x152u), 0u, 100u, 3u, 0u);
        break;
    case 2u:
        spawned = moh_draft_missing_call(0x80045CA0u, object, yaw, pitch, 0u, first, second, mode,
            0x13F1u, 0x2000u, 0u, 0x08000450u);
        moh_draft_missing_call(0x8004D8DCu, r_u8(state + 0x152u), 0u, 100u, 7u, 0u);
        break;
    case 4u:
        spawned = moh_draft_missing_call(0x80045F14u, object, yaw, pitch, 0u, 0u, first, second, mode,
            0x13F7u, 0x2000u, 0u, 0x08000450u);
        volume = 0x20000u; w_u16(spawned + 0x3CAu, r_u8(state + 0x55u));
        for (count = 1u; count < 6u; ++count) {
            spawned = moh_draft_missing_call(0x80045F14u, object, yaw, pitch, 0u, 0u, first, second, mode,
                0x13F7u, 0x2000u, 0u, 0x08000450u);
            if (count < 5u) w_u16(spawned + 0x3CAu, r_u8(state + 0x55u));
        }
        moh_draft_missing_call(0x8004D8DCu, r_u8(state + 0x152u), 0u, 100u, 7u, 0u);
        break;
    default:
        break;
    }
    if (spawned) w_u16(spawned + 0x3CAu, r_u8(state + 0x55u));
    if (!(r_u32(state) & 0x200040u)) moh_draft_missing_call(0x8006DCA0u, state + 0x1D4u);
    kind = r_u8(state + 0x55u); row = 0x80093640u + kind * 68u; sample = (sint16)r_u16(row + 0x14u);
    if (sample >= 0) {
        value = (r_u16(row + 0x2Au) >> 3) & 2u; bank = (sint16)r_u16(row + 0x16u);
        if (bank < 0) bank = (sint16)r_u16(r_u32(r_u32(object + 0x100u) + 8u) + 0xC8u);
        if (kind == 5u || kind == 11u) volume = 0x20000u;
        moh_draft_missing_call(0x80041CE8u, (uint32)bank, (uint32)sample, object, volume,
            0u, 0x10000u, 0x1800u, 0u, 0u, 0u, value);
    }
    if (r_u8(state + 0x55u) != 7u) {
        row = 0x80093640u + r_u8(state + 0x55u) * 68u;
        if (!(r_u16(row + 0x2Au) & 0x10u)) {
            value = moh_draft_missing_call(0x8001552Cu);
            if (((sint32)(value * 100u) >> 15) > 50) moh_draft_missing_call(0x80088040u, object, object + 0x20Cu, 0x14000u);
        }
    }
    ammunition = state + (uint32)(sint32)(sint8)r_u8(state + 0x54u) * 2u + 0x66u;
    value = (uint16)(r_u16(ammunition) - 1u); w_u16(ammunition, (uint16)value);
    if (!value) moh_draft_missing_call(0x8005C97Cu, object, 0x10u, 1u);
    if (!(r_u32(0x800386A4u) & 0x10u)) {
        amount = r_u32(state + 0x40u); magnitude = (sint32)amount < 0 ? 0u - amount : amount;
        w_u32(state + 0x44u, amount); w_u32(state + 0x48u, magnitude);
    } else {
        rate = (sint32)r_u32(0x80039D88u) >> 6;
        if (rate < 20) value = r_u32(state + 0x40u) << 2;
        else {
            shift = rate < 30 ? 0u : rate < 35 ? 1u : 2u;
            amount = r_u32(state + 0x40u); magnitude = (sint32)amount < 0 ? 0u - amount : amount;
            value = (uint32)((sint32)amount >> shift);
            if ((sint32)magnitude <= (sint32)(1u << shift)) value = (sint32)amount > 0 ? 1u : 0xFFFFFFFFu;
        }
        w_u32(state + 0x44u, value); value = r_u32(state + 0x44u);
        w_u32(state + 0x48u, (sint32)value < 0 ? 0u - value : value);
    }
    moh_draft_missing_call(0x800467ECu, object);
}

void level_sub_8007B350(uint32 object)
{
    uint32 state, kind, value, row;
    FUNCTION_MARKER(0x8007B350u, "LEVEL.BIN");
    state = r_u32(object + 0x388u);
    if (!(r_u32(object + 0x38u) & 0x4000u)) {
        kind = r_u8(state + 0x55u);
        if (kind == 3u || kind == 10u) {
            value = moh_draft_missing_call(0x8005CBA8u, 1u, object);
            row = 0x80093640u + r_u8(state + 0x55u) * 68u;
            moh_draft_missing_call(0x8007CF4Cu, object, (uint32)(sint32)(sint16)r_u16(row + 12u),
                (uint32)(sint32)(sint16)r_u16(row + 14u), value & 0xFFu, r_u32(state + 0x11Cu));
        } else if (kind == 7u) {
            if (moh_draft_missing_call(0x8005CBA8u, 1u, object)) {
                value = r_u32(state + 0x40u); w_u32(state + 0x44u, value);
                w_u32(state + 0x48u, (sint32)value < 0 ? 0u - value : value);
            }
        } else {
            row = 0x80093640u + r_u8(0x8009E878u + (uint32)(sint32)(sint8)r_u8(state + 0x54u)) * 68u;
            if (!(r_u16(row + 0x2Au) & 1u)) {
                value = moh_draft_missing_call(0x8005CBA8u, 1u, object);
                row = 0x80093640u + r_u8(state + 0x55u) * 68u;
                moh_draft_missing_call(0x8007D5C8u, object, (uint32)(sint32)(sint16)r_u16(row + 12u),
                    (uint32)(sint32)(sint16)r_u16(row + 14u), value & 0xFFu);
            }
        }
    }
    moh_draft_missing_call(0x8005C97Cu, object, 1u, 0u);
}

void level_sub_80084400(uint32 object)
{
    uint32 point, identity, player, table, cursor, end, bit, mask = 0u, descriptor, data, value, call;
    FUNCTION_MARKER(0x80084400u, "LEVEL.BIN");
    point = sub_80016718(8u);
    if (!point) moh_draft_fail_fast("Object region local allocation", 0x80084400u);
    /* TODO: Original final local halfword is unspecified */
    value = r_u16(object + 0x20Eu); identity = r_u32(object + 0x390u); player = r_u32(object + 12u) & 1u;
    w_u16(point, (uint16)value); w_u16(point + 2u, r_u16(object + 0x212u)); w_u16(point + 4u, r_u16(object + 0x216u));
    table = r_u32(r_u32(r_u32(0x8009D648u) + 0x388u) + 0x14u);
    if (identity != 0xFFFFu) {
        data = r_u32(r_u32(0x8009EC5Cu) + identity * 4u); cursor = data + 2u;
        value = 0xFFu - r_u8(data); end = cursor + value * 2u; bit = 1u << (value & 31u);
        while (cursor != end) {
            descriptor = table + (uint32)(sint32)(sint16)r_u16(cursor) * 8u; data = r_u32(descriptor + 4u);
            call = 0u;
            if (player) {
                if (!r_u32(data + 12u) || r_u32(data + 0x38u) || r_u32(data + 0x3Cu) || r_u32(data + 0x4Cu)) call = 1u;
            } else if (r_u32(data + 12u)) {
                if (r_u32(data + 0x40u) || r_u32(data + 0x44u)) call = 1u;
            }
            if (call && moh_draft_missing_call(0x80083D30u, point, 0u, descriptor)) mask |= bit;
            cursor += 2u; mask >>= 1;
        }
    }
    w_u32(object + 0x394u, mask); moh_draft_missing_call(0x800840E0u, object);
    identity = r_u32(object + 0x390u); mask = r_u32(object + 0x394u);
    w_u32(object + 0x398u, identity); w_u32(object + 0x39Cu, mask); sub_80036E44(point);
}

void level_sub_800845A8(uint32 object)
{
    FUNCTION_MARKER(0x800845A8u, "LEVEL.BIN");
    if (r_u32(object + 12u) & 0x51u) level_sub_80084400(object);
}

uint32 level_sub_80078A38(uint32 x, uint32 z, uint32 index)
{
    uint32 bounds = r_u32(0x800B7838u + index * 4u);
    sint32 left, right, near_z, far_z;
    FUNCTION_MARKER(0x80078A38u, "LEVEL.BIN");
    left = (sint16)r_u16(bounds); right = (sint16)r_u16(bounds + 4u);
    if ((sint32)x < left || right < (sint32)x) return 0u;
    near_z = (sint16)r_u16(bounds + 2u); far_z = (sint16)r_u16(bounds + 6u);
    return (sint32)z >= near_z && (sint32)z <= far_z ? bounds : 0u;
}

uint32 level_sub_80078AA4(uint32 x, uint32 z, uint32 radius, uint32 index)
{
    uint32 result, bounds, dx, dz;
    sint32 left, right, near_z, far_z, nearest_x, nearest_z;
    FUNCTION_MARKER(0x80078AA4u, "LEVEL.BIN");
    result = level_sub_80078A38(x, z, index);
    if (result) return result;
    bounds = r_u32(0x800B7838u + index * 4u);
    left = (sint16)r_u16(bounds); right = (sint16)r_u16(bounds + 4u);
    near_z = (sint16)r_u16(bounds + 2u); far_z = (sint16)r_u16(bounds + 6u);
    nearest_x = left;
    if (left < (sint32)x && ((sint32)x >= right ||
        (sint32)((uint32)right - x) < (sint32)(x - (uint32)left))) nearest_x = right;
    nearest_z = near_z;
    if (near_z < (sint32)z && ((sint32)z >= far_z ||
        (sint32)((uint32)far_z - z) < (sint32)(z - (uint32)near_z))) nearest_z = far_z;
    dx = x - (uint32)nearest_x; dz = z - (uint32)nearest_z;
    return (sint32)(dz * dz + dx * dx) < (sint32)(radius * radius) ? bounds : 0u;
}

void level_sub_800770A8(void)
{
    uint32 index = 0u, count = r_u32(0x80098984u), excluded, record = 0x80090DE8u, destination;
    FUNCTION_MARKER(0x800770A8u, "LEVEL.BIN");
    w_u32(0x800B7828u, 0u);
    if ((sint32)count <= 0) return;
    excluded = r_u32(0x800AA184u);
    do {
        if (index != excluded) {
            destination = r_u32(0x800B7828u); w_u32(0x800B7838u + destination * 4u, record);
            w_u32(0x800B7828u, destination + 1u);
        }
        ++index; record += 40u;
    } while ((sint32)index < (sint32)count);
}

sint32 level_sub_80078E2C(uint32 point, uint32 radius, uint32 height, uint32 movement, uint32 auxiliary,
                        uint32 planes, uint32 distances, uint32 capacity, uint32 filter, uint32 mode)
{
    uint32 index = 0u, count = 0u, current_auxiliary = auxiliary, current_planes = planes, current_distances = distances, value;
    FUNCTION_MARKER(0x80078E2Cu, "LEVEL.BIN");
    if ((sint32)r_u32(0x800B7828u) <= 0) return 0;
    do {
        value = moh_draft_missing_call(0x80078AA4u, (uint32)(sint32)(sint16)r_u16(point), (uint32)(sint32)(sint16)r_u16(point + 4u), radius, index);
        w_u32(0x800B7848u, value);
        if (value) {
            value = moh_draft_missing_call(0x800787C0u, point, radius, height, movement, current_auxiliary, current_planes, current_distances, capacity, filter, mode);
            if (value) {
                count += value; current_auxiliary = auxiliary + count * 2u; current_distances = distances + count * 2u; current_planes = planes + count * 4u;
            }
        }
        if ((sint32)count >= (sint32)capacity) break;
        value = r_u32(0x800B7828u); ++index;
    } while ((sint32)index < (sint32)value);
    return (sint32)count;
}

sint32 level_sub_80078D1C(uint32 start, uint32 count, uint32 previous, uint32 position, uint32 radius,
                        uint32 height, uint32 planes, uint32 distances)
{
    uint32 index, plane, best = 0x7FFFFFFFu, selected = 0xFFFFFFFFu, value;
    FUNCTION_MARKER(0x80078D1Cu, "LEVEL.BIN");
    for (index = start; (sint32)index < (sint32)count; ++index) {
        plane = r_u32(planes + index * 4u); if (!plane) continue;
        w_u16(plane + 6u, r_u16(distances + index * 2u));
        value = moh_draft_missing_call(0x80078BCCu, plane, position, height, radius);
        if ((sint32)value >= 0) continue;
        value = moh_draft_missing_call(0x80078BCCu, plane, previous, height, 0u);
        if ((sint32)value < (sint32)best) { best = value; selected = index; }
    }
    return (sint32)selected;
}

void level_sub_80078C4C(uint32 plane, uint32 position, uint32 previous, uint32 height, uint32 radius)
{
    uint32 distance, x, z, previous_z;
    FUNCTION_MARKER(0x80078C4Cu, "LEVEL.BIN");
    distance = (uint32)((sint32)moh_draft_missing_call(0x80078BCCu, plane, position, height, radius) >> 8);
    x = (uint32)((sint32)(sint16)r_u16(plane) >> 7) * distance;
    z = (uint32)((sint32)(sint16)r_u16(plane + 4u) >> 7) * distance;
    x = r_u32(position) - r_u32(previous) - x; previous_z = r_u32(previous + 8u);
    z = r_u32(position + 8u) - previous_z - z;
    w_u32(position, x + r_u32(previous)); w_u32(position + 8u, z + r_u32(previous + 8u));
}

static uint32 moh_motion_plane_dot(uint32 vector, uint32 plane)
{
    uint32 axis;
    xport_gte_write_control(0u, r_u32(vector)); xport_gte_write_control(1u, r_u32(vector + 4u));
    for (axis = 0u; axis < 3u; ++axis) xport_gte_write_data(9u + axis, r_u16(plane + axis * 2u));
    xport_gte_execute(0x41E012u); return xport_gte_read_data(25u);
}

sint32 level_sub_80078F60(uint32 object, uint32 movement, uint32 previous, uint32 mode_out)
{
    uint32 local, point, planes, auxiliary, distances, delta, cross, facing, state, position, radius, height, lower, range;
    uint32 index, plane, slot, flags, value, distance, distance_bias, absolute, offset, best[3], output, mask;
    sint32 count, remaining, selected, result, cross_y;
    uint32 changed = 0u, clear;
    FUNCTION_MARKER(0x80078F60u, "LEVEL.BIN");
    /* TODO Unverified full collision translation; missing geometry calls fail fast */
    position = object + 0x20Cu; radius = (uint32)(sint32)(sint16)r_u16(object + 0x2ECu);
    height = (uint32)(sint32)(sint16)r_u16(object + 0x2D6u); state = r_u32(object + 0x38Cu); lower = height - 16u;
    best[0] = best[1] = best[2] = 0x7FFFFFFFu;
    local = sub_80016718(304u);
    if (!local) moh_draft_fail_fast("Collision local allocation", 0x80078F60u);
    point = local; planes = local + 8u; auxiliary = local + 136u; distances = local + 200u;
    delta = local + 264u; cross = local + 272u; facing = local + 288u;
    /* TODO Original short-vector padding is unspecified */
    w_u16(point, r_u16(position + 2u)); w_u16(point + 2u, (uint16)(r_u16(position + 6u) - lower)); w_u16(point + 4u, r_u16(position + 10u));
    w_u32(mode_out, 0u); moh_draft_missing_call(0x800770A8u);
    range = radius; flags = 1u;
    if (state) {
        value = r_u32(state + 8u); mask = r_u32(state + 4u); w_u32(state + 8u, value & 0xFFFFFFE3u);
        if (mask & 4u) {
            slot = state + (uint32)(sint32)(sint16)r_u16(state + 0xC0u) * 2u;
            range = (uint32)(sint32)(sint16)r_u16(slot + 0xCCu);
            value = r_u32(state + 0xD8u);
            w_u16(facing + 2u, 0u); index = r_u16(object + 0x344u); if (value) index = 0u - index; w_u16(facing, (uint16)index);
            index = r_u16(object + 0x350u); if (value) index = 0u - index; w_u16(facing + 4u, (uint16)index);
            slot = state + (uint32)(sint32)(sint16)r_u16(state + 0xC0u) * 2u;
            w_u16(slot + 0xCCu, r_u16(slot + 0xD0u));
            if ((sint32)range < (sint32)radius) range = radius;
            flags = 0u;
        }
    }
    count = (sint32)moh_draft_missing_call(0x80078E2Cu, point, range, height - 20u, movement, auxiliary, planes, distances, 32u, 1u, flags);
    if (!count) { sub_80036E44(local); return 0; }
    /* TODO Original fixed arrays have no overflow guard */
    if (count > 32) moh_draft_fail_fast("Collision plane array overflow", 0x80078F60u);
    if (state && (r_u32(state + 4u) & 4u)) {
        for (index = 0u; (sint32)index < count; ++index) {
            plane = r_u32(planes + index * 4u); if (!plane) continue;
            absolute = sub_8001DB60((uint32)(sint32)(sint16)r_u16(plane + 2u));
            w_u16(plane + 6u, r_u16(distances + index * 2u));
            if ((sint32)absolute < 0x5A80) {
                value = moh_motion_plane_dot(facing, plane);
                if (((sint32)value >> 12) < 12000) {
                    for (slot = 0u; slot < 3u; ++slot) xport_gte_write_control(slot * 2u, (uint32)(sint32)(sint16)r_u16(facing + slot * 2u));
                    for (slot = 0u; slot < 3u; ++slot) xport_gte_write_data(9u + slot, (uint32)(sint32)(sint16)r_u16(plane + slot * 2u));
                    xport_gte_execute(0x178000Cu);
                    for (slot = 0u; slot < 3u; ++slot) w_u32(cross + slot * 4u, xport_gte_read_data(25u + slot));
                    value = moh_motion_plane_dot(point, plane); distance_bias = (uint32)(sint32)(sint16)r_u16(distances + index * 2u);
                    distance = value + distance_bias;
                    value = moh_motion_plane_dot(point, plane) + distance_bias;
                    if ((sint32)distance < 0) distance = 0u - value; else distance = value;
                    cross_y = (sint32)r_u32(cross + 4u); absolute = sub_8001DB60((uint32)cross_y);
                    slot = 3u;
                    if ((sint32)absolute <= 0x5000) slot = 0u;
                    else if (cross_y > 0x5000) slot = 1u;
                    else if (cross_y < -0x5000) slot = 2u;
                    if (slot < 3u && (sint32)distance < (sint32)best[slot]) {
                        value = r_u32(facing); mask = r_u32(facing + 4u); w_u32(state + 0xECu, value); w_u32(state + 0xF0u, mask);
                        value = r_u32(plane); mask = r_u32(plane + 4u);
                        output = state + 0xF4u + slot * 16u; w_u32(output, value); w_u32(output + 4u, mask); w_u32(output + 8u, distance);
                        cross_y = (sint32)r_u32(cross + 4u); flags = r_u32(state + 8u); best[slot] = distance;
                        w_u32(output + 12u, (uint32)cross_y); w_u32(state + 8u, flags | (4u << slot));
                    }
                }
            }
            offset = index * 32u;
            result = (sint32)moh_draft_missing_call(0x800776C0u, point, radius, height - 20u, 1u, 0x8009E430u + offset, 0x8009E438u + offset, 0x8009E440u + offset, plane);
            clear = 0u;
            if (result) {
                value = moh_motion_plane_dot(movement, plane);
                if ((sint32)value >= 0 || !moh_draft_missing_call(0x80078350u, point, radius, height - 20u, plane)) clear = 1u;
            } else clear = 1u;
            if (clear) w_u32(planes + index * 4u, 0u);
        }
    }
    remaining = count;
    do {
        selected = (sint32)moh_draft_missing_call(0x80078D1Cu, 0u, (uint32)count, previous, position, radius, lower, planes, distances);
        if (selected < 0) break;
        changed = 1u; --remaining;
        plane = r_u32(planes + (uint32)selected * 4u); w_u16(plane + 6u, r_u16(distances + (uint32)selected * 2u));
        moh_draft_missing_call(0x80078C4Cu, plane, position, previous, lower, radius + 4u);
    } while (remaining >= 0);
    for (index = 0u; index < 3u; ++index) w_u16(delta + index * 2u, (uint16)(r_u16(position + index * 4u + 2u) - r_u16(previous + index * 4u + 2u)));
    if (!changed) { w_u32(mode_out, 2u); sub_80036E44(local); return 0; }
    for (index = 0u; index < 3u; ++index) w_u16(point + index * 2u, r_u16(position + index * 4u + 2u));
    result = (sint32)moh_draft_missing_call(0x80078E2Cu, point, radius, height - 20u, delta, auxiliary, planes, distances, 32u, 1u, 1u);
    if (!result) w_u32(mode_out, 2u);
    else {
        selected = (sint32)moh_draft_missing_call(0x80078D1Cu, 0u, (uint32)result, previous, position, radius, lower, planes, distances);
        w_u32(mode_out, selected < 0 ? 2u : 1u);
    }
    sub_80036E44(local); return result;
}

static uint32 moh_motion_product(uint32 left, uint32 right)
{
    return (uint32)((sint32)(left * right) >> 12);
}

uint32 sub_800165D8(uint32 matrix, uint32 scale)
{
    uint32 factors[3], word, low, high, index;
    FUNCTION_MARKER(0x800165D8u, "SLUS_009.74");
    for (index = 0u; index < 3u; ++index) factors[index] = r_u32(scale + index * 4u);
    for (index = 0u; index < 4u; ++index) {
        word = r_u32(matrix + index * 4u);
        low = moh_motion_product((uint32)(sint32)(sint16)word, factors[(index * 2u) % 3u]);
        high = moh_motion_product((uint32)((sint32)word >> 16), factors[(index * 2u + 1u) % 3u]);
        w_u32(matrix + index * 4u, (low & 0xFFFFu) | (high << 16));
    }
    /* Original final word store also writes the matrix padding halfword */
    w_u32(matrix + 16u, moh_motion_product((uint32)(sint32)(sint16)r_u32(matrix + 16u), factors[2]));
    return matrix;
}

uint32 sub_8001AF98(uint32 rotation, uint32 matrix)
{
    uint32 angle, packed, sx, cx, sy, cy, sz, cz, mixed, other, value;
    FUNCTION_MARKER(0x8001AF98u, "SLUS_009.74");
    /* TODO Unverified C uses the original packed trigonometry table */
    angle = (uint32)(sint32)(sint16)r_u16(rotation);
    packed = r_u32(0x8003040Cu + (((sint32)angle < 0 ? 0u - angle : angle) & 0xFFFu) * 4u);
    sx = (uint32)(sint32)(sint16)packed; if ((sint32)angle < 0) sx = 0u - sx; cx = (uint32)((sint32)packed >> 16);
    angle = (uint32)(sint32)(sint16)r_u16(rotation + 2u);
    packed = r_u32(0x8003040Cu + (((sint32)angle < 0 ? 0u - angle : angle) & 0xFFFu) * 4u);
    sy = (uint32)(sint32)(sint16)packed; if ((sint32)angle < 0) sy = 0u - sy; cy = (uint32)((sint32)packed >> 16);
    angle = (uint32)(sint32)(sint16)r_u16(rotation + 4u);
    w_u16(matrix + 10u, (uint16)(0u - sx)); w_u16(matrix + 4u, (uint16)moh_motion_product(sy, cx));
    w_u16(matrix + 16u, (uint16)moh_motion_product(cy, cx));
    packed = r_u32(0x8003040Cu + (((sint32)angle < 0 ? 0u - angle : angle) & 0xFFFu) * 4u);
    sz = (uint32)(sint32)(sint16)packed; if ((sint32)angle < 0) sz = 0u - sz; cz = (uint32)((sint32)packed >> 16);
    w_u16(matrix + 6u, (uint16)moh_motion_product(sz, cx)); w_u16(matrix + 8u, (uint16)moh_motion_product(cz, cx));
    mixed = moh_motion_product(sy, sx);
    value = moh_motion_product(mixed, sz) + moh_motion_product(cy, cz); w_u16(matrix, (uint16)value);
    value = 0u - moh_motion_product(cy, sz) + moh_motion_product(mixed, cz); w_u16(matrix + 2u, (uint16)value);
    other = moh_motion_product(cy, sx);
    value = moh_motion_product(other, cz) + moh_motion_product(sy, sz); w_u16(matrix + 14u, (uint16)value);
    value = 0u - moh_motion_product(sy, cz) + moh_motion_product(other, sz); w_u16(matrix + 12u, (uint16)value);
    return matrix;
}

void level_sub_8006C808(uint32 object)
{
    uint32 values[6], axis;
    FUNCTION_MARKER(0x8006C808u, "LEVEL.BIN");
    for (axis = 0u; axis < 3u; ++axis) values[axis] = r_u32(object + 0x20Cu + axis * 4u);
    for (axis = 0u; axis < 3u; ++axis) values[axis + 3u] = r_u32(object + 0x228u + axis * 4u);
    for (axis = 0u; axis < 6u; ++axis) w_u32(object + 0x258u + axis * 4u, values[axis]);
}

void level_sub_8006A870(uint32 object, uint32 matrix)
{
    uint32 local, index;
    FUNCTION_MARKER(0x8006A870u, "LEVEL.BIN");
    local = sub_80016718(24u);
    if (!local) moh_draft_fail_fast("Motion local allocation", 0x8006A870u);
    for (index = 0u; index < 24u; ++index) w_u8(local + index, 0u);
    for (index = 0u; index < 3u; ++index) w_u16(local + index * 2u, r_u16(object + 0x22Au + index * 4u));
    moh_draft_missing_call(0x8001AF98u, local, matrix); sub_80016218(matrix, local + 8u);
    sub_80036E44(local);
}

void level_sub_8008D918(uint32 position, uint32 rotation, uint32 scale, uint32 matrix)
{
    uint32 local, axis;
    FUNCTION_MARKER(0x8008D918u, "LEVEL.BIN");
    local = sub_80016718(20u);
    if (!local) moh_draft_fail_fast("Motion transform local allocation", 0x8008D918u);
    /* TODO Original unused short-vector padding is unspecified */
    for (axis = 0u; axis < 3u; ++axis) w_u16(local + axis * 2u, r_u16(rotation + axis * 4u + 2u));
    moh_draft_missing_call(0x8001AF98u, local, matrix);
    for (axis = 0u; axis < 3u; ++axis) w_u32(local + 8u + axis * 4u, (uint32)(sint32)(sint16)r_u16(position + axis * 4u + 2u));
    sub_80016218(matrix, local + 8u);
    for (axis = 0u; axis < 3u; ++axis) w_u32(local + 8u + axis * 4u, (uint32)((sint32)r_u32(scale + axis * 4u) >> 4));
    moh_draft_missing_call(0x800165D8u, matrix, local + 8u); sub_80036E44(local);
}

static void moh_motion_restore_position(uint32 object)
{
    uint32 x = r_u32(object + 0x3BCu), y = r_u32(object + 0x3C0u), z = r_u32(object + 0x3C4u);
    w_u32(object + 0x20Cu, x); w_u32(object + 0x210u, y); w_u32(object + 0x214u, z);
}

uint32 level_sub_8006C83C(uint32 object)
{
    uint32 local, origin, movement, collision, hit, matrix, rotation, mode_out, old_ground, result = 0u;
    uint32 material, pose, frame, clip, x, y, z, value, flags, hit_result = 0u, axis, offset, mask;
    sint32 vertical, floor_height, difference, threshold;
    uint32 player_collision = 0u, actor_collision = 0u, line_collision = 0u;
    FUNCTION_MARKER(0x8006C83Cu, "LEVEL.BIN");
    /* TODO Unverified full C; missing collision and SDK services fail fast */
    level_sub_8006C808(object); local = sub_80016718(104u);
    if (!local) moh_draft_fail_fast("Physics local allocation", 0x8006C83Cu);
    origin = local + 16u; collision = local + 32u; hit = local + 40u; movement = local + 48u;
    matrix = local + 56u; rotation = local + 88u; mode_out = local + 8u;
    /* TODO Original unused short-vector and matrix padding is unspecified */
    for (axis = 0u; axis < 3u; ++axis) w_u32(origin + axis * 4u, r_u32(object + 0x20Cu + axis * 4u));
    old_ground = r_u32(object + 0x284u);
    x = r_u32(object + 0x228u); y = r_u32(object + 0x234u); z = r_u32(object + 0x238u); value = r_u32(object + 0x23Cu);
    x += y; y = r_u32(object + 0x230u); w_u32(object + 0x228u, x);
    x = r_u32(object + 0x22Cu); w_u32(object + 0x230u, y + value); material = r_u32(object + 0x100u); w_u32(object + 0x22Cu, x + z);
    if (material && (sint16)r_u16(material + 0x5Eu) >= 0 && (sint16)r_u16(material + 0x5Cu) >= 0) {
        frame = r_u16(material + 0x5Eu); clip = r_u16(material + 0x5Cu);
        if (r_u32(material + 0x74u)) pose = r_u32(r_u32(r_u32(material + 0x78u) + clip * 4u) + 4u) + frame * 20u + 8u;
        else pose = r_u32(r_u32(r_u32(material + 8u) + 0x98u) + clip * 16u + 8u) + frame * 24u + 16u;
        offset = r_u32(object + 8u) == 1u ? 0x240u : 0x228u;
        mask = offset == 0x240u ? 0x0FFFFFFFu : 0x0FFF0000u;
        w_u32(object + offset, r_u32(object + offset) + ((uint32)(sint32)(sint16)r_u16(pose) << 16));
        value = r_u32(object + offset + 4u) + ((uint32)(sint32)(sint16)r_u16(pose + 2u) << 16);
        x = r_u32(object + offset); w_u32(object + offset + 4u, value);
        z = (uint32)(sint32)(sint16)r_u16(pose + 4u); w_u32(object + offset, x & mask);
        w_u32(object + offset + 8u, r_u32(object + offset + 8u) + (z << 16));
        x = r_u32(object + offset + 4u); y = r_u32(object + offset + 8u);
        w_u32(object + offset + 4u, x & mask); w_u32(object + offset + 8u, y & mask);
    }
    value = r_u32(object + 0x1FCu);
    if (value) {
        value += r_u32(object + 0x200u); x = r_u32(object + 0x204u);
        if ((sint32)value > (sint32)x) value = x;
        w_u32(object + 0x200u, value);
    }
    w_u32(object + 0x220u, r_u32(object + 0x200u));
    if (!r_u32(object + 0x284u) && (r_u32(object + 12u) & 0x20u)) {
        if ((r_u32(object + 12u) & 1u) && (r_u32(r_u32(object + 0x388u)) & 0x10000000u)) { value = r_u32(object + 0x21Cu) + 0x2000u; x = 0x4B000u; }
        else { value = r_u32(object + 0x21Cu) + 0x20000u; x = 0x4B0000u; }
        w_u32(object + 0x21Cu, value); if ((sint32)value > (sint32)x) w_u32(object + 0x21Cu, x);
    }
    value = r_u32(object + 0x224u);
    if (value == 0x10000u) { x = r_u32(object + 0x218u) >> 12; z = r_u32(object + 0x220u) >> 12; y = r_u32(object + 0x21Cu) >> 12; }
    else {
        x = ((uint32)(sint32)(sint16)r_u16(object + 0x21Au) * value) >> 12;
        y = ((uint32)(sint32)(sint16)r_u16(object + 0x21Eu) * value) >> 12;
        z = ((uint32)(sint32)(sint16)r_u16(object + 0x222u) * value) >> 12;
    }
    w_u16(movement, (uint16)x); w_u16(movement + 2u, (uint16)y); w_u16(movement + 4u, (uint16)z);
    if (r_u32(object + 12u) & 1u) {
        w_u32(rotation, 0u); w_u32(rotation + 4u, r_u32(r_u32(object + 0x388u) + 0x184u)); w_u32(rotation + 8u, r_u32(object + 0x230u));
        level_sub_8008D918(object + 0x20Cu, rotation, object + 0x270u, matrix);
    } else level_sub_8006A870(object, matrix);
    for (axis = 0u; axis < 8u; ++axis) xport_gte_write_control(axis, r_u32(matrix + axis * 4u));
    xport_gte_write_data(0u, r_u32(movement)); xport_gte_write_data(1u, r_u32(movement + 4u)); xport_gte_execute(0x486012u);
    x = xport_gte_read_data(25u); y = xport_gte_read_data(26u); z = xport_gte_read_data(27u);
    w_u32(object + 0x20Cu, r_u32(object + 0x20Cu) + (x << 12) + r_u32(object + 0x1F0u));
    w_u32(object + 0x210u, r_u32(object + 0x210u) + (y << 12) + r_u32(object + 0x1F4u));
    w_u32(object + 0x214u, r_u32(object + 0x214u) + (z << 12) + r_u32(object + 0x1F8u));
    vertical = (sint32)y >> 4;
    if (!(r_u32(object + 0x14u) & r_u32(r_u32(0x8009D648u) + 12u))) goto finish;
    for (axis = 0u; axis < 3u; ++axis) w_u16(collision + axis * 2u,
        (uint16)((sint32)(sint16)r_u16(object + 0x20Eu + axis * 4u) - (sint32)(sint16)r_u16(object + 0x3BEu + axis * 4u)));
    w_u32(mode_out, 0u); flags = r_u32(object + 12u);
    if (flags & 8u) {
        if (flags & 1u) player_collision = 1u;
        else if (!(r_u32(object + 0x10u) & 1u)) {
            if (!(r_u32(object + 0x10u) & 0x44u)) actor_collision = 1u;
            else line_collision = 1u;
        }
    }
    if (player_collision || actor_collision) hit_result = (sint32)moh_draft_missing_call(0x80078F60u, object, collision, object + 0x3BCu, mode_out) > 0;
    else if (line_collision) hit_result = (sint32)moh_draft_missing_call(0x8003EF5Cu, object + 0x20Cu, origin, collision, hit, local + 12u, object + 0x318u, 0u) >= 0;
    value = r_u32(mode_out);
    if (player_collision && value == 1u) { moh_motion_restore_position(object); w_u32(object + 0x284u, old_ground); }
    else if (value == 2u) { result |= 2u; w_u32(object + 0x38u, r_u32(object + 0x38u) | 0x8000u); }
    if (hit_result) {
        if ((sint16)r_u16(collision + 2u) > 0x800 && vertical < 0 && (sint32)r_u32(object + 0x21Cu) > 0) {
            value = r_u32(object + 0x3C0u); w_u32(object + 0x21Cu, 0u); w_u32(object + 0x210u, value);
        }
        w_u32(object + 0x218u, 0u); w_u32(object + 0x220u, 0u);
        x = r_u32(collision); y = r_u32(collision + 4u); w_u32(object + 0x364u, x); w_u32(object + 0x368u, y);
        result |= 2u; w_u32(object + 0x38u, r_u32(object + 0x38u) | 0x8000u);
        if (!player_collision) {
            if (!(r_u32(object + 12u) & 0x80u)) {
                if (r_u32(object + 0x10u) & 0x44u) {
                    for (axis = 0u; axis < 3u; ++axis) w_u32(object + 0x20Cu + axis * 4u, (uint32)(sint32)(sint16)r_u16(hit + axis * 2u) << 16);
                } else if ((sint32)r_u32(mode_out) < 0) { moh_motion_restore_position(object); w_u32(object + 0x284u, old_ground); }
            } else {
                w_u32(object + 0x20Cu, r_u32(origin) + (((uint32)(sint32)(sint16)r_u16(collision) * 32u + 0x8000u) & 0xFFFF0000u));
                w_u32(object + 0x214u, r_u32(origin + 8u) + (((uint32)(sint32)(sint16)r_u16(collision + 4u) * 32u + 0x8000u) & 0xFFFF0000u));
            }
        }
    }
    if ((r_u32(object + 0x18u) & 1u) || (r_u32(object + 0x10u) & 0x10u) || (!r_u32(object + 0x284u) && !(r_u32(object + 12u) & 8u))) goto finish;
    value = (uint32)level_sub_80060474(object, movement); w_u16(0x800B7460u, (uint16)value); floor_height = (sint16)value;
    if (floor_height == -0x7FFF) {
        if ((r_u32(object + 12u) & 7u) && !(r_u32(object + 0x10u) & 0x10000u)) {
            moh_motion_restore_position(object); if (!old_ground) w_u32(object + 0x210u, r_u32(object + 0x208u));
            result |= 2u; w_u32(object + 0x284u, 1u); w_u32(object + 0x38u, r_u32(object + 0x38u) | 0x80008000u);
        }
        goto finish;
    }
    difference = (sint16)r_u16(object + 0x212u) - floor_height; w_u32(object + 0x208u, (uint32)floor_height << 16);
    if (r_u32(object + 0x284u)) {
        if ((uint32)difference + 0x20u < 0x121u || difference >= 0) { w_u32(object + 0x210u, (uint32)floor_height << 16); goto finish; }
        if (!(r_u32(object + 0x10u) & 0x200000u) || difference > -0x3A) {
            w_u32(object + 0x284u, 0u); w_u32(object + 0x38u, r_u32(object + 0x38u) | 1u);
        } else {
            result |= 0x80u; w_u32(object + 0x220u, 0u); w_u32(object + 0x21Cu, 0u); w_u32(object + 0x218u, 0u); w_u32(object + 0x200u, 0u);
            moh_motion_restore_position(object); w_u32(object + 0x284u, old_ground); w_u32(object + 0x38u, r_u32(object + 0x38u) | 0x80000000u);
        }
        goto finish;
    }
    if (!(r_u32(object + 12u) & 0x20u)) goto finish;
    if (difference < 0) { threshold = vertical < 0x30 ? vertical + 16 : 64; if (threshold < -difference) goto finish; }
    flags = r_u32(object + 12u);
    if (flags & 1u) { if (vertical < 0) goto finish; }
    else if (flags & 0x80u) {
        if ((uint32)(vertical < 0 ? -vertical : vertical) >= 8u) {
            if (!hit_result) { x = r_u32(movement); y = r_u32(movement + 4u); w_u32(object + 0x364u, x); w_u32(object + 0x368u, y); }
            floor_height = (sint16)r_u16(0x800B7460u); value = r_u32(object + 0x21Cu);
            result |= 1u; w_u32(object + 0x284u, 0u); if ((sint32)value < 0) value = 0u - value;
            w_u32(object + 0x210u, (uint32)floor_height << 16); w_u32(object + 0x21Cu, 0u - value); goto finish;
        }
    }
    result |= 1u; floor_height = (sint16)r_u16(0x800B7460u); w_u32(object + 0x284u, 1u);
    w_u32(object + 0x210u, (uint32)floor_height << 16); flags = r_u32(object + 0x38u);
    w_u32(object + 0x1F4u, 0u); w_u32(object + 0x21Cu, 0u); w_u32(object + 0x38u, flags & 0xFFFFFFFEu);
finish:
    w_u32(object + 0x1F4u, 0u); w_u32(object + 0x1F0u, 0u); w_u32(object + 0x1F8u, 0u);
    x = r_u32(object + 0x20Cu); y = r_u32(object + 0x210u); z = r_u32(object + 0x214u);
    w_u32(object + 0x3BCu, x); w_u32(object + 0x3C0u, y); w_u32(object + 0x3C4u, z); sub_80036E44(local); return result;
}
