#include "game_draft_signatures.h"
#include "game_level_signatures.h"
#include "xport_trace.h"
#include "psx.h"
static uint32 moh_model_owner(uint32 render);

static uint32 moh_model_control_word(const PsxGteSnapshot *snapshot, uint32 index)
{
    uint32 element = index * 2u;
    if (index < 4u) return (uint32)(uint16)snapshot->rotation.m[element / 3u][element % 3u]
        | ((uint32)(uint16)snapshot->rotation.m[(element + 1u) / 3u][(element + 1u) % 3u] << 16u);
    if (index == 4u) return (uint32)(sint32)snapshot->rotation.m[2][2];
    return (uint32)snapshot->translation[index - 5u];
}

uint32 level_sub_80080480(uint32 render, uint32 depth_out, uint32 direct)
{
    uint32 handle, owner, parent = 0u, model, stack, index, depth, attached, rotated = 0u;
    sint32 scale, bound, value;
    PsxGteSnapshot snapshot;
    FUNCTION_MARKER(0x80080480u, "LEVEL.BIN");
    if (direct) { w_u32(depth_out, 0u); return 0u; }
    handle = r_u32(render + 0x54u);
    owner = handle ? r_u32(0x8009D678u + (handle & 0xFFFFu) * 4u) : 0u;
    attached = owner != 0u;
    if (owner) {
        parent = r_u32(render + 0x58u);
        if (!parent) {
            handle = r_u32(render + 0x54u);
            owner = handle ? r_u32(0x8009D678u + (handle & 0xFFFFu) * 4u) : 0u;
            parent = r_u32(owner + 0x100u);
        }
    }
    if (attached) {
        rotated = r_u32(parent + 0x74u) == 0u;
        if (!rotated) w_u32(0x8009E8E0u, parent);
    }
    model = r_u32((rotated ? parent : render) + 8u);
    bound = (sint16)r_u16(model + 0x40u); value = (sint16)r_u16(model + 0x3Eu);
    if (bound <= value) bound = value;
    value = (sint16)r_u16(model + 0x3Cu); if (bound <= value) bound = value;
    scale = bound - 2; if (scale < 0) scale = 0;
    if (rotated) {
        stack = r_u32(0x8009D320u); psx_gte_snapshot(&snapshot);
        for (index = 0u; index < 8u; ++index) w_u32(0x8009D324u + stack + index * 4u, moh_model_control_word(&snapshot, index));
        w_u32(0x8009D320u, stack + 32u);
        handle = r_u32(render + 0x54u);
        owner = handle ? r_u32(0x8009D678u + (handle & 0xFFFFu) * 4u) : 0u;
        level_sub_8008D850(owner);
    }
    xport_gte_write_data(0u, 0u); xport_gte_write_data(1u, 0u); xport_gte_execute(0x180001u);
    depth = xport_gte_read_data(19u);
    if (rotated) {
        stack = r_u32(0x8009D320u) - 32u; w_u32(0x8009D320u, stack);
        for (index = 0u; index < 8u; ++index) xport_gte_write_control(index, r_u32(0x8009D324u + stack + index * 4u));
    }
    depth -= 1u << (((uint32)scale + 9u) & 31u);
    w_u32(depth_out, depth); if ((sint32)depth < 0) w_u32(depth_out, 0u);
    return (uint32)scale;
}

static void moh_model_project(uint32 point)
{
    xport_gte_write_data(0u, r_u32(point)); xport_gte_write_data(1u, r_u32(point + 4u));
    xport_gte_execute(0x180001u);
}

static void moh_model_store_projection(uint32 point)
{
    uint32 flags;
    PsxGteSnapshot snapshot;
    psx_gte_snapshot(&snapshot); flags = (uint32)snapshot.flag;
    if ((flags & 0x80020000u) == 0x80000000u) w_u16(point + 4u, 0xFFFFu);
    else {
        w_u32(point, xport_gte_read_data(14u)); w_u32(point + 4u, xport_gte_read_data(19u));
    }
}

static void moh_model_depth_bounds(uint32 point, uint32 *minimum, uint32 *maximum)
{
    sint32 depth = (sint16)r_u16(point + 4u);
    uint32 bits = r_u16(point + 4u);
    if (depth != -1) {
        if (depth < (sint16)*minimum) *minimum = bits;
        if ((sint16)*maximum < depth) *maximum = bits;
    }
}

uint32 level_sub_80080DD4(uint32 render, uint32 ordering_table, uint32 buffer_index, uint32 direct)
{
    uint32 geometry, descriptor, material, model, local, packet, scratch, end, count, code, offset, saved, axis;
    uint32 previous, point, minimum = 0x7FFFu, maximum = 0xFFFF8000u, shift = 4u, lower = 1u;
    uint32 triangles, triangle, vertices[3], owner, colors[3], index, drawn = 0u, tag, header, next, value;
    sint32 depth, bucket;
    PsxGteSnapshot snapshot;
    FUNCTION_MARKER(0x80080DD4u, "LEVEL.BIN");
    (void)buffer_index;
    if (!r_u32(render + 0x88u)) return 0u;
    geometry = r_u32(render + 0x84u); descriptor = r_u32(render + 0x7Cu);
    material = r_u32(r_u32(r_u32(render + 0x74u) + 4u) + 4u); model = r_u32(render + 0x80u);
    local = sub_80016718(4u);
    if (!local) moh_draft_fail_fast("Animated model packet bound allocation", 0x80080DD4u);
    packet = level_sub_80053BD4(r_u32(0x8009C83Cu), local);
    end = r_u32(local) - 40u; count = r_u32(descriptor + 4u); scratch = packet + count * 40u;
    if (scratch > end) { sub_80036E44(local); return 0u; }
    code = r_u32(render + 0x68u) ? 0x36u : 0x34u;
    if (scratch + r_u32(model + 4u) * 16u > end) { sub_80036E44(local); return 0u; }
    offset = r_u32(0x8009D320u); saved = 0x8009D324u + offset;
    psx_gte_snapshot(&snapshot);
    for (axis = 0u; axis < 8u; axis += 2u) {
        header = moh_model_control_word(&snapshot, axis); next = moh_model_control_word(&snapshot, axis + 1u);
        w_u32(saved + axis * 4u, header); w_u32(saved + axis * 4u + 4u, next);
    }
    w_u32(0x8009D320u, offset + 32u);
    level_sub_80084718(render, geometry, material, scratch);
    offset = r_u32(0x8009D320u) - 32u; saved = 0x8009D324u + offset;
    w_u32(0x8009D320u, offset);
    for (axis = 0u; axis < 8u; ++axis) xport_gte_write_control(axis, r_u32(saved + axis * 4u));
    previous = r_u32(scratch); moh_model_project(previous); moh_model_store_projection(previous);
    point = previous + 8u; moh_model_project(point); index = 1u;
    while ((sint32)index < (sint32)(r_u32(0x1F8003FCu) - 1u)) {
        moh_model_depth_bounds(previous, &minimum, &maximum);
        moh_model_store_projection(point); previous = point; point = previous + 8u;
        moh_model_project(point); ++index;
    }
    moh_model_depth_bounds(previous, &minimum, &maximum);
    moh_model_store_projection(point); moh_model_depth_bounds(point, &minimum, &maximum);
    if (!direct) { lower = 2u; shift = (uint32)(((sint32)(sint16)maximum - (sint16)minimum) >> 9); }
    triangles = r_u32(descriptor); owner = moh_model_owner(render); triangle = triangles + 20u;
    for (index = 0u; (sint32)index < (sint32)count; ++index, triangles += 28u, triangle += 28u) {
        for (axis = 0u; axis < 3u; ++axis)
            vertices[axis] = r_u32(scratch + r_u8(triangle + 3u + axis * 2u) * 4u)
                + r_u8(triangle + 2u + axis * 2u) * 8u;
        if ((sint16)r_u16(vertices[0] + 4u) < 0 || (sint16)r_u16(vertices[1] + 4u) < 0
            || (sint16)r_u16(vertices[2] + 4u) < 0) continue;
        value = r_u32(vertices[0]); header = r_u32(vertices[1]); next = r_u32(vertices[2]);
        xport_gte_write_data(12u, value); xport_gte_write_data(14u, next); xport_gte_write_data(13u, header);
        xport_gte_execute(0x1400006u);
        if ((sint32)xport_gte_read_data(24u) <= 0) continue;
        for (axis = 0u; axis < 3u; ++axis) xport_gte_write_data(17u + axis, r_u16(vertices[axis] + 4u));
        xport_gte_execute(0x158002Du); depth = (sint32)(xport_gte_read_data(7u) << 2u);
        if (!direct) { depth -= (sint16)minimum; if (depth < 0) depth = 0; }
        bucket = depth >> (shift & 31u);
        colors[0] = r_u32(triangles); colors[1] = r_u32(triangle - 16u); colors[2] = r_u32(triangle - 12u);
        value = r_u32(0x8009CC64u);
        w_u32(packet + 8u, r_u32(vertices[0])); w_u32(packet + 20u, r_u32(vertices[1]));
        w_u32(packet + 32u, r_u32(vertices[2]));
        if (r_u16(value + 0xCCu)) {
            for (axis = 0u; axis < 3u; ++axis) xport_gte_write_data(20u + axis, colors[axis]);
            xport_gte_write_data(6u, colors[2]); xport_gte_execute(0xF8002Au);
            for (axis = 0u; axis < 3u; ++axis) w_u32(packet + 4u + axis * 12u, xport_gte_read_data(20u + axis));
        } else {
            w_u32(packet + 4u, r_u32(triangles)); w_u32(packet + 16u, r_u32(triangle - 16u));
            w_u32(packet + 28u, r_u32(triangle - 12u));
        }
        w_u8(packet + 3u, 9u); w_u8(packet + 7u, (uint8)code);
        w_u32(packet + 12u, r_u32(triangle - 8u)); w_u32(packet + 24u, r_u32(triangle - 4u));
        w_u16(packet + 36u, r_u16(triangle));
        if (owner) bucket = (sint32)((uint32)bucket - 15u);
        if (bucket < (sint32)lower) bucket = (sint32)lower;
        if (bucket >= 512) bucket = 511;
        tag = ordering_table + (uint32)bucket * 4u;
        header = r_u32(packet); next = r_u32(tag);
        w_u32(packet, (header & 0xFF000000u) | (next & 0xFFFFFFu));
        value = packet & 0xFFFFFFu; next = r_u32(tag); packet += 40u;
        w_u32(tag, (next & 0xFF000000u) | value); ++drawn;
    }
    value = r_u16(render + 0xAAu);
    w_u32(0x1F8003FCu, minimum | (maximum << 16u)); w_u16(render + 0xA8u, (uint16)minimum);
    w_u32(render + 0xACu, shift); w_u16(render + 0xAAu, (uint16)((value & 0x8000u) | maximum));
    level_sub_80053BF4(packet); sub_80036E44(local); return drawn > 0u;
}

static sint32 moh_model_divide(uint32 numerator, sint32 denominator)
{
    // TODO Preserve original arithmetic trap diagnostics
    if (!denominator || (denominator == -1 && numerator == 0x80000000u))
        moh_draft_fail_fast("Model interpolation division trap", 0x80085648u);
    return (sint32)numerator / denominator;
}

uint32 sub_8001BC68(uint32 input)
{
    uint32 leading, even, normalized, value;
    sint32 exponent;
    FUNCTION_MARKER(0x8001BC68u, "SLUS_009.74");
    xport_gte_write_data(30u, input); leading = xport_gte_read_data(31u);
    if (leading == 32u) return 0u;
    even = leading & ~1u; exponent = (sint32)(19u - even) >> 1;
    normalized = even >= 24u ? input << ((even - 24u) & 31u)
        : (uint32)((sint32)input >> ((24u - even) & 31u));
    value = (uint32)(sint32)(sint16)r_u16(0x8003440Cu + (normalized - 64u) * 2u);
    return exponent >= 0 ? value << ((uint32)exponent & 31u) : value >> ((0u - (uint32)exponent) & 31u);
}

static void moh_model_blend(uint32 previous, uint32 current, uint32 output, uint32 count, uint32 mode)
{
    uint32 index, axis, sum, word, negative, angle, portion, weight_previous, weight_current;
    sint32 a[4], b[4], dot, tolerance, root, value;
    for (index = 0u; index < count; ++index, previous += 8u, current += 8u, output += 8u) {
        a[0] = (sint16)r_u16(previous); b[0] = (sint16)r_u16(current);
        if (mode == 1u) {
            b[1] = (sint16)r_u16(current + 2u); a[1] = (sint16)r_u16(previous + 2u);
            a[2] = (sint16)r_u16(previous + 4u); b[2] = (sint16)r_u16(current + 4u);
            b[3] = (sint16)r_u16(current + 6u); a[3] = (sint16)r_u16(previous + 6u);
        } else {
            for (axis = 1u; axis < 4u; ++axis) {
                a[axis] = (sint16)r_u16(previous + axis * 2u);
                b[axis] = (sint16)r_u16(current + axis * 2u);
            }
        }
        sum = 0x800u;
        for (axis = 0u; axis < 4u; ++axis) sum += (uint32)a[axis] * (uint32)b[axis];
        dot = (sint32)sum >> 12; negative = (uint32)dot >> 31u;
        tolerance = (sint32)r_u32(0x8009EAB0u);
        if (dot >= (sint32)(0x1000u - (uint32)tolerance) && dot <= (sint32)(0x1000u + (uint32)tolerance)) {
            for (axis = 0u; axis < 4u; ++axis)
                w_u16(output + axis * 2u, (uint16)(mode == 1u ? a[axis] : b[axis]));
            continue;
        }
        if (negative) dot = (sint32)(0u - (uint32)dot);
        if ((sint32)(0x1000u - (uint32)dot) < (sint32)r_u32(0x8009EAACu)) {
            for (axis = 0u; axis < 4u; ++axis) {
                if (mode == 1u) {
                    word = negative ? 0u - (uint32)a[axis] - (uint32)b[axis] : (uint32)b[axis] - (uint32)a[axis];
                    value = (sint32)((uint32)a[axis] + (uint32)((sint32)(word + 2u) >> 2));
                } else if (mode == 2u) {
                    word = negative ? (uint32)a[axis] - (uint32)b[axis] : (uint32)a[axis] + (uint32)b[axis];
                    value = (sint32)(word + 1u) >> 1;
                } else if (negative) value = ((sint32)((uint32)a[axis] + (uint32)b[axis] + 2u) >> 2) - b[axis];
                else value = b[axis] + ((a[axis] - b[axis] + 2) >> 2);
                w_u16(output + axis * 2u, (uint16)value);
            }
            continue;
        }
        word = 0x1000800u - (uint32)dot * (uint32)dot;
        root = (sint32)moh_draft_missing_call(0x8001BC68u, (uint32)((sint32)word >> 12));
        angle = moh_draft_missing_call(0x8001536Cu, (uint32)root, (uint32)dot);
        portion = (uint32)((sint32)(angle + (mode == 2u ? 1u : 2u)) >> (mode == 2u ? 1 : 2));
        word = moh_draft_missing_call(0x8001C9A8u, angle - portion);
        weight_previous = (uint32)moh_model_divide(word << 17u, root);
        word = moh_draft_missing_call(0x8001C9A8u, portion);
        weight_current = (uint32)moh_model_divide(word << 17u, root);
        if (negative) weight_current = 0u - weight_current;
        for (axis = 0u; axis < 4u; ++axis) {
            word = mode == 3u ? weight_current * (uint32)a[axis] + weight_previous * (uint32)b[axis]
                : weight_previous * (uint32)a[axis] + weight_current * (uint32)b[axis];
            w_u16(output + axis * 2u, (uint16)((sint32)(word + 0x10000u) >> 17));
        }
    }
}

void level_sub_80085648(uint32 object, uint32 render)
{
    sint32 frame, animation, prior_frame, transition, override_frame, elapsed, position[3], old;
    uint32 count, current, previous, output, table, packed, axis;
    FUNCTION_MARKER(0x80085648u, "LEVEL.BIN");
    (void)object;
    frame = (sint16)r_u16(render + 0x5Eu); animation = (sint16)r_u16(render + 0x5Cu);
    prior_frame = (sint16)r_u16(render + 0x60u); transition = (sint16)r_u16(render + 0x3Eu);
    if (animation < 0) { w_u16(render + 0x62u, 0xFFFFu); w_u16(render + 0x64u, 0xFFFFu); return; }
    if (!transition && animation == (sint16)r_u16(render + 0x62u) && frame == (sint16)r_u16(render + 0x64u)) return;
    if (transition > 0 && (sint16)r_u16(render + 0x62u) != -1) {
        table = r_u32(r_u32(r_u32(render + 0x78u) + (uint32)animation * 4u) + 4u);
        current = r_u32(render + 0x88u); count = r_u8(table + (uint32)frame * 20u + 15u);
        if (!current) {
            current = moh_draft_missing_call(0x8008492Cu, render, (uint32)animation, (uint32)frame, r_u32(render + 0x90u));
            w_u32(render + 0x88u, current); return;
        }
        override_frame = (sint16)r_u16(render + 0x40u); frame = override_frame >= 0 ? override_frame : prior_frame;
        w_u32(render + 0x34u, (uint32)animation); w_u32(render + 0x38u, (uint32)frame);
        current = moh_draft_missing_call(0x8008492Cu, render, (uint32)animation, (uint32)frame, 0x8009EAC0u);
        for (axis = 0u; axis < 3u; ++axis) {
            table = r_u32(r_u32(r_u32(render + 0x78u) + (uint32)animation * 4u) + 4u);
            packed = r_u32(table + (uint32)frame * 20u + 4u);
            position[axis] = axis == 0u ? (sint32)(packed << 22u) >> 22
                : axis == 1u ? (sint32)(packed << 11u) >> 21 : (sint32)(packed << 1u) >> 22;
        }
        elapsed = 4 - transition;
        if (!elapsed) {
            w_u32(render + 0x88u, r_u32(render + 0x8Cu));
            for (axis = 0u; axis < 3u; ++axis) w_u16(render + 0x98u + axis * 2u, r_u16(render + 0xA0u + axis * 2u));
        } else if (elapsed >= 1 && elapsed <= 3) {
            previous = r_u32(render + 0x8Cu); output = r_u32(render + 0x90u);
            moh_model_blend(previous, current, output, count, (uint32)elapsed);
            w_u32(render + 0x88u, r_u32(render + 0x90u));
            for (axis = 0u; axis < 3u; ++axis) {
                old = (sint16)r_u16(render + 0xA0u + axis * 2u);
                w_u16(render + 0x98u + axis * 2u, (uint16)(elapsed == 3 ? position[axis] - ((position[axis] - old) >> 2)
                    : old + ((position[axis] - old) >> (elapsed == 2 ? 1 : 2))));
            }
        }
    } else {
        if (transition > 0) w_u16(render + 0x3Eu, 0u);
        current = moh_draft_missing_call(0x8008492Cu, render, (uint32)animation, (uint32)frame, r_u32(render + 0x90u));
        w_u32(render + 0x88u, current);
    }
    w_u16(render + 0x62u, (uint16)animation); w_u16(render + 0x64u, (uint16)frame);
}

uint32 level_sub_8008492C(uint32 render, uint32 animation, uint32 frame, uint32 output)
{
    uint32 model, record, count, offsets, first, last, mode, payload, table, current, second;
    sint32 difference;
    FUNCTION_MARKER(0x8008492Cu, "LEVEL.BIN");
    model = r_u32(r_u32(render + 0x78u) + animation * 4u);
    record = r_u32(model + 4u) + frame * 20u;
    offsets = r_u8(record + 14u); count = r_u8(record + 15u);
    first = frame - (offsets & 15u); last = frame + (offsets >> 4u);
    payload = r_u32(record + 16u);
    difference = (sint32)(last - first);
    if (payload) mode = 0u;
    else if (difference == 2 || difference == -2) mode = 2u;
    else {
        mode = frame - first;
        if ((sint32)mode < 0) mode = 0u - mode;
    }
    if (!mode) moh_draft_missing_call(0x800847CCu, r_u32(record + 16u), output, count);
    else {
        table = r_u32(model + 4u); second = table + last * 20u;
        current = moh_draft_missing_call(0x800847CCu, r_u32(table + first * 20u + 16u), 0x1F800000u, count);
        moh_draft_missing_call(0x800847CCu, r_u32(second + 16u), current, count);
        if (mode >= 1u && mode <= 3u) moh_model_blend(0x1F800000u, current, output, count, mode);
    }
    return output;
}

static void moh_model_unpack_first(uint32 a, uint32 b, uint32 output)
{
    uint32 combined = (uint32)((sint32)((uint32)((sint32)b >> 12) << 28u) >> 20)
        | ((a >> 12u) & 0xF0u) | (a & 15u);
    w_u16(output + 6u, (uint16)((uint32)((sint32)a >> 20) << 1u));
    w_u16(output, (uint16)((uint32)((sint32)(a << 16u) >> 20) << 1u));
    w_u16(output + 2u, (uint16)((sint32)(b << 20u) >> 19));
    w_u16(output + 4u, (uint16)(combined << 1u));
}

uint32 level_sub_800847CC(uint32 input, uint32 output, uint32 count)
{
    sint32 pairs;
    uint32 a, b, c, combined;
    FUNCTION_MARKER(0x800847CCu, "LEVEL.BIN");
    pairs = (sint32)count >> 1;
    while (pairs > 0) {
        count -= 2u; b = r_u32(input + 4u); c = r_u32(input + 8u); a = r_u32(input);
        input += 12u; --pairs;
        moh_model_unpack_first(a, b, output);
        combined = ((uint32)((sint32)c >> 28) << 8u) | ((c & 15u) << 4u) | ((b >> 16u) & 15u);
        w_u16(output + 14u, (uint16)((uint32)((sint32)(c << 16u) >> 20) << 1u));
        w_u16(output + 8u, (uint16)((uint32)((sint32)b >> 20) << 1u));
        w_u16(output + 10u, (uint16)((uint32)((sint32)(c << 4u) >> 20) << 1u));
        w_u16(output + 12u, (uint16)(combined << 1u));
        output += 16u;
    }
    if (count & 1u) {
        a = r_u32(input); b = r_u32(input + 4u);
        moh_model_unpack_first(a, b, output); output += 8u;
    }
    return output;
}

static uint32 moh_effect_actor(uint32 effect)
{
    uint32 handle = r_u32(effect + 0x18u);
    return handle ? r_u32(0x8009D678u + (handle & 0xFFFFu) * 4u) : 0u;
}

static uint32 moh_model_owner(uint32 render)
{
    uint32 handle = r_u32(render + 0x54u);
    return handle ? r_u32(0x8009D678u + (handle & 0xFFFFu) * 4u) : 0u;
}

uint32 level_sub_80082948(uint32 render, uint32 ordering_table, uint32 buffer_index, uint32 shade)
{
    uint32 actor, owner, table, direct = 0u, visible, target, first, last;
    sint32 range, near_depth, step, index, candidate;
    FUNCTION_MARKER(0x80082948u, "LEVEL.BIN");
    (void)shade;
    actor = moh_model_owner(render);
    if (actor) {
        owner = r_u32(render + 0x58u);
        if (!owner) owner = r_u32(moh_model_owner(render) + 0x100u);
        table = r_u32(owner + 0x4Cu + buffer_index * 4u);
        if (!(r_u32(owner + 4u) & 0x20u)) {
            moh_draft_missing_call(0x8001722Cu, table, 0x200u);
            w_u32(owner + 4u, r_u32(owner + 4u) | 0x20u);
        }
    } else {
        table = r_u32(render + 0x4Cu + buffer_index * 4u);
        if (!table) { direct = 1u; table = ordering_table; }
        else if (!(r_u32(render + 4u) & 0x20u)) {
            moh_draft_missing_call(0x8001722Cu, table, 0x200u);
            w_u32(render + 4u, r_u32(render + 4u) | 0x20u);
        }
    }
    visible = moh_draft_missing_call(r_u32(render + 0x6Cu) == 0x80u ? 0x80080DD4u : 0x800814C4u,
        render, table, buffer_index, direct) != 0u;
    if (!direct && visible && !moh_model_owner(render)) {
        candidate = (sint16)r_u16(0x1F8003FEu);
        near_depth = (sint16)r_u16(0x1F8003FCu);
        range = (candidate - near_depth) >> 4;
        step = range < 2 ? 511 : (sint16)(511 / range);
        near_depth >>= 4;
        if (render == r_u32(r_u32(0x8009D654u) + 0x100u))
            moh_draft_missing_call(0x80013F98u, ordering_table + 4u, table + 0x7FCu, table);
        else {
            if (near_depth < 2) near_depth = 2;
            // TODO Original zero step loops indefinitely
            if (!step) moh_draft_fail_fast("Zero animated model ordering-table step", 0x80082948u);
            index = 0;
            while (index < 512) {
                target = near_depth < 511 ? (uint32)near_depth : 511u;
                first = index ? (uint32)index - 1u : 0u;
                candidate = index + step;
                last = candidate - 1 < 511 ? table + (uint32)candidate * 4u - 4u : table + 0x7FCu;
                moh_draft_missing_call(0x80013F98u, ordering_table + target * 4u, last, table + first * 4u);
                near_depth = (sint32)target + 1; index += step;
            }
        }
    }
    return visible;
}

uint32 level_sub_800824D0(uint32 render, uint32 ordering_table, uint32 buffer_index, uint32 shade)
{
    uint32 actor, owner, alternate = 0u, direct = 0u, table, visible = 0u, depth_out, result, model, scale, index, step, target, first;
    sint32 depth = 0xFFFF, candidate, near_depth, maximum;
    FUNCTION_MARKER(0x800824D0u, "LEVEL.BIN");
    if (r_u32(render + 0x74u)) {
        if (r_u32(render + 4u) & 0x80u)
            return moh_draft_missing_call(0x80082CA8u, render, ordering_table, buffer_index, shade);
        return moh_draft_missing_call(0x80082948u, render, ordering_table, buffer_index, shade);
    }
    actor = moh_model_owner(render);
    if (actor) {
        owner = r_u32(render + 0x58u);
        if (!owner) owner = r_u32(moh_model_owner(render) + 0x100u);
        alternate = r_u32(owner + 0x74u) != 0u;
        table = r_u32(owner + 0x4Cu + buffer_index * 4u);
        if (!(r_u32(owner + 4u) & 0x20u)) {
            moh_draft_missing_call(0x8001722Cu, table, 0x200u);
            w_u32(owner + 4u, r_u32(owner + 4u) | 0x20u);
        }
    } else {
        table = r_u32(render + 0x4Cu + buffer_index * 4u);
        if (!table) { direct = 1u; table = ordering_table; }
        else if (!(r_u32(render + 4u) & 0x20u)) {
            moh_draft_missing_call(0x8001722Cu, table, 0x200u);
            w_u32(render + 4u, r_u32(render + 4u) | 0x20u);
        }
    }
    depth_out = sub_80016718(4u);
    if (!depth_out) moh_draft_fail_fast("Model depth allocation", 0x800824D0u);
    if (r_u32(render + 0x28u)) {
        result = moh_draft_missing_call(alternate ? 0x80081CDCu : 0x8008080Cu,
            render, table, buffer_index, 1u, depth_out, direct);
        visible = result != 0u; candidate = (sint32)r_u32(depth_out);
        if (candidate < depth) depth = candidate;
    }
    if (r_u32(render + 0x20u)) {
        result = moh_draft_missing_call(alternate ? 0x80081CDCu : 0x8008080Cu,
            render, table, buffer_index, 0u, depth_out, direct);
        visible = result != 0u || visible; candidate = (sint32)r_u32(depth_out);
        if (candidate < depth) depth = candidate;
    }
    if (!direct && visible && !moh_model_owner(render)) {
        near_depth = (sint32)((uint32)depth - shade) >> 4;
        if (near_depth < 0) near_depth = 0; else if (near_depth >= 512) near_depth = 511;
        model = r_u32(render + 8u);
        maximum = (sint16)r_u16(model + 0x3Eu);
        candidate = (sint16)r_u16(model + 0x40u);
        if (candidate > maximum) maximum = candidate;
        candidate = (sint16)r_u16(model + 0x3Cu);
        if (candidate >= maximum) maximum = candidate;
        scale = maximum < 4 ? 0u : (uint32)(maximum - 4);
        step = (uint32)((sint32)8 >> (scale & 31u));
        // TODO Original zero stride loops indefinitely
        if (!step) moh_draft_fail_fast("Zero model ordering-table stride", 0x800824D0u);
        index = 0u;
        while ((sint32)index < 512) {
            if (render == r_u32(r_u32(0x8009D654u) + 0x100u)) target = 1u;
            else {
                candidate = (sint32)((uint32)near_depth + (uint32)((sint32)index >> ((3u - scale) & 31u)));
                target = candidate < 511 ? (uint32)candidate : 511u;
            }
            first = index ? index - 1u : 0u; index += step;
            moh_draft_missing_call(0x80013F98u, ordering_table + target * 4u,
                table + index * 4u - 4u, table + first * 4u);
        }
    }
    sub_80036E44(depth_out); return visible;
}

void level_sub_8006D2E4(uint32 object, uint32 render_context, uint32 shade)
{
    uint32 visible, flags;
    FUNCTION_MARKER(0x8006D2E4u, "LEVEL.BIN");
    if (r_u32(object + 8u) == 1u) level_sub_8008D660(object);
    else level_sub_8008D850(object);
    visible = moh_draft_missing_call(0x800824D0u, r_u32(object + 0x100u),
        r_u32(render_context + 0x2218u), r_u32(render_context + 0x221Cu), shade);
    flags = r_u32(object + 12u); w_u32(object + 0x29Cu, visible);
    if (flags & 0x8000u) {
        if (visible) {
            flags = r_u32(object + 0x294u); w_u32(object + 0x298u, 1u); w_u32(object + 0x290u, flags);
        } else if (r_u32(object + 0x298u)) moh_draft_missing_call(0x8006A570u, object);
    }
}

void level_sub_8008D660(uint32 object)
{
    uint32 state, local, prepared, axis, selected, words[5], value;
    FUNCTION_MARKER(0x8008D660u, "LEVEL.BIN");
    state = r_u32(r_u32(0x8009D638u) + 0x388u);
    prepared = !(r_u32(state + 0x9Cu) & 0x1800u);
    local = sub_80016718(76u);
    if (!local) moh_draft_fail_fast("View model matrix allocation", 0x8008D660u);
    if (prepared) {
        for (axis = 0u; axis < 3u; ++axis) {
            value = r_u32(object + 0x228u + axis * 4u);
            value += r_u32(object + 0x240u + axis * 4u);
            w_u32(local + 64u + axis * 4u, value & 0xFFFFFFFu);
        }
        level_sub_8008D918(object + 0x20Cu, local + 64u, object + 0x270u, local);
    }
    for (axis = 0u; axis < 5u; ++axis) words[axis] = r_u32(0x8009C804u + axis * 4u);
    for (axis = 0u; axis < 5u; ++axis) w_u32(local + 32u + axis * 4u, words[axis]);
    for (axis = 0u; axis < 6u; ++axis)
        w_u16(local + 32u + axis * 2u, (uint16)((sint32)(sint16)r_u16(local + 32u + axis * 2u) * 3));
    if (!(r_u32(state + 0x9Cu) & 0x1800u)) {
        if (!prepared) moh_draft_fail_fast("Uninitialized view model rotation", 0x8008D660u);
        w_u32(local + 20u, 0u); w_u32(local + 24u, 0u); w_u32(local + 28u, 0u);
        moh_draft_missing_call(0x8001BD08u, local + 32u, local); selected = local;
    } else {
        // TODO Original alternate camera path reads uninitialized local translation
        moh_draft_fail_fast("Uninitialized alternate camera translation", 0x8008D660u);
        selected = local + 32u;
    }
    for (axis = 0u; axis < 8u; ++axis) xport_gte_write_control(axis, r_u32(selected + axis * 4u));
    sub_80036E44(local);
}

void level_sub_8006A5B4(uint32 object, uint32 render_context)
{
    uint32 transform = 1u, draw = 1u, flags, handle, actor, owner;
    FUNCTION_MARKER(0x8006A5B4u, "LEVEL.BIN");
    if ((r_u32(object + 12u) & 0x3000000u) && !moh_draft_missing_call(0x8008E9DCu)) {
        flags = r_u32(object + 12u);
        if (flags & 0x2000000u) transform = r_u32(object + 0x298u) == 0u;
        if (flags & 0x1000000u) draw = 0u;
    }
    if (transform) {
        if (r_u32(r_u32(object + 0x100u) + 0x74u)) level_sub_80086634(object);
        else level_sub_8008DA98();
    } else moh_draft_missing_call(0x8004D43Cu, object);
    if (draw) {
        owner = r_u32(0x8009D638u);
        handle = r_u32(r_u32(owner + 0x388u) + 0xA0u);
        actor = handle ? r_u32(0x8009D678u + (handle & 0xFFFFu) * 4u) : 0u;
        if (actor != object) {
            if (r_u32(object + 12u) & 1u) {
                moh_draft_missing_call(0x8001BA68u, 0x85u);
                moh_draft_missing_call(0x8006D2E4u, object, render_context, 0xC0u);
                moh_draft_missing_call(0x8001BA68u, r_u32(0x8009A628u));
            } else moh_draft_missing_call(0x8006D2E4u, object, render_context,
                (r_u32(object + 16u) & 0x80u) ? 0xFFFFFFC0u : 0x80u);
        }
    } else moh_draft_missing_call(0x8006A570u, object);
    // TODO Bind remaining draw and visibility services when reached
}

void level_sub_8006DD54(uint32 effect, uint32 render_context)
{
    sint32 count;
    uint32 local, phase, model, actor, owner, rotation, scale, matrix = 0u;
    FUNCTION_MARKER(0x8006DD54u, "LEVEL.BIN");
    count = (sint16)r_u16(effect + 2u);
    if (count <= 0) { w_u16(effect + 2u, 0u); return; }
    local = sub_80016718(76u);
    if (!local) moh_draft_fail_fast("Effect locals allocation", 0x8006DD54u);
    w_u8(local, (uint8)(0xFFFFFF80u - (r_u8(effect + 2u) << 5u)));
    w_u8(local + 1u, (uint8)(0xFFFFFF80u - (r_u8(effect + 2u) << 5u)));
    w_u8(local + 2u, (uint8)(0xFFFFFF80u - (r_u8(effect + 2u) << 5u)));
    phase = ((r_u32(0x8009D61Cu) & 7u) << 9u) + ((uint32)count << 7u);
    model = r_u32(effect + 0x1Cu);
    if (model) moh_draft_missing_call(0x8008E38Cu, model, (uint32)r_u16(effect), local + 24u);
    else {
        actor = moh_effect_actor(effect);
        if (r_u32(actor + 12u) & 1u) {
            owner = r_u32(0x8009D638u);
            w_u32(local + 64u, r_u32(owner + 0x20Cu));
            w_u32(local + 68u, r_u32(owner + 0x210u) - (r_u32(r_u32(owner + 0x388u) + 0x94u) << 16u));
            w_u32(local + 72u, r_u32(owner + 0x214u));
            rotation = moh_effect_actor(effect) + 0x228u;
            scale = moh_effect_actor(effect) + 0x270u;
            matrix = local + 32u;
            level_sub_8008D918(local + 64u, rotation, scale, matrix);
        }
        actor = moh_effect_actor(effect);
        moh_draft_missing_call(0x8008E354u, actor, (uint32)r_u16(effect), local + 24u, effect + 4u, matrix);
    }
    w_u32(local + 8u, (uint32)(sint32)(sint16)r_u16(local + 24u) << 16u);
    w_u32(local + 16u, (uint32)(sint32)(sint16)r_u16(local + 28u) << 16u);
    w_u32(local + 12u, (uint32)(sint32)(sint16)r_u16(local + 26u) << 16u);
    // TODO Bind unresolved effect and model services when reached
    moh_draft_missing_call(0x8006E058u, local + 8u, 0x800u, phase, local, effect + 0x70u,
        render_context, effect + 0x20u + r_u32(render_context + 0x221Cu) * 40u, 0u, 0u, 0u, 1u);
    w_u16(effect + 2u, (uint16)(r_u16(effect + 2u) - 1u));
    sub_80036E44(local);
}

void level_sub_8007B8F4(uint32 object, uint32 render_context)
{
    uint32 state, render, value, offset, flags;
    FUNCTION_MARKER(0x8007B8F4u, "LEVEL.BIN");
    state = r_u32(object + 0x388u);
    if (r_u32(state + 0xA4u) == 400u && !(r_u32(state) & 0xC000u)) {
        flags = r_u32(object + 16u); render = r_u32(object + 0x100u);
        value = r_u32(render + 0x6Cu);
        if (flags & 0x400u) {
            if (value >= 65u) w_u32(render + 0x6Cu, value - 2u);
        } else w_u32(render + 0x6Cu, value < 128u ? value + 2u : 128u);
        moh_draft_missing_call(0x8006DD54u, state + 0x1D4u, render_context);
        moh_draft_missing_call(0x8006A5B4u, object, render_context);
    }
    if (r_u32(state) & 0x100u) moh_draft_missing_call(0x8007B534u, object);
    else {
        offset = r_u8(state + 0x55u) * 68u;
        moh_draft_missing_call(0x8007CAF8u, object,
            (uint32)(sint32)(sint16)r_u16(0x8009364Cu + offset),
            (uint32)(sint32)(sint16)r_u16(0x8009364Eu + offset));
    }
}

void level_sub_8008BFFC(uint32 packet, uint32 x, uint32 y, uint32 width, uint32 height, uint32 clut_x, uint32 clut_y)
{
    uint32 left, right, page, clut;
    FUNCTION_MARKER(0x8008BFFCu, "LEVEL.BIN");
    height &= 0xFFFFu; clut_x &= 0xFFFFu; clut_y &= 0xFFFFu;
    w_u8(packet + 3u, 9u); w_u8(packet + 7u, 0x2Cu);
    w_u8(packet + 4u, 0x50u); w_u8(packet + 5u, 0x50u); w_u8(packet + 6u, 0x50u);
    left = (x & 63u) << 2u; right = ((x + width) & 63u) << 2u;
    w_u8(packet + 12u, (uint8)left); w_u8(packet + 28u, (uint8)left);
    w_u8(packet + 13u, (uint8)y); w_u8(packet + 20u, (uint8)right);
    w_u8(packet + 21u, (uint8)y); w_u8(packet + 36u, (uint8)right);
    w_u8(packet + 29u, (uint8)(y + height)); w_u8(packet + 37u, (uint8)(y + height));
    page = moh_draft_missing_call(0x8001B968u, 0u, 0u, x & 0xFFC0u, y & 0xFF00u);
    w_u16(packet + 22u, (uint16)page);
    clut = moh_draft_missing_call(0x8001BA18u, clut_x, clut_y); w_u16(packet + 14u, (uint16)clut);
}

void level_sub_8008C0C8(void)
{
    uint32 buffer, index, record, packet, flags, update, axis, screen[3], depth[3], high, low, average, clips[3], tag, header, next;
    sint32 width, height, x, y;
    PsxGteSnapshot snapshot;
    FUNCTION_MARKER(0x8008C0C8u, "LEVEL.BIN");
    buffer = r_u32(0x8009C83Cu);
    tag = r_u32(0x8009C838u); width = (sint32)r_u32(0x8009C828u); height = (sint32)r_u32(0x8009C82Cu);
    w_u32(0x1F8003CCu, 0x7FF98000u); w_u32(0x1F8003BCu, tag);
    w_u32(0x1F800398u, (uint32)width); w_u32(0x1F80039Cu, (uint32)height);
    for (index = 0u; index < 100u; ++index) {
        record = 0x80093C5Cu + index * 128u; flags = r_u32(record);
        if (!(flags & 1u)) continue;
        for (axis = 0u; axis < 6u; ++axis) xport_gte_write_data(axis, r_u32(record + 4u + axis * 4u));
        xport_gte_execute(0x280030u);
        packet = record + buffer * 40u + 48u; update = buffer ? 4u : 2u;
        if (flags & update) {
            level_sub_8008BFFC(packet, r_u16(record + 36u), r_u16(record + 38u), r_u16(record + 40u),
                r_u16(record + 42u), r_u16(record + 44u), r_u16(record + 46u));
            w_u32(record, r_u32(record) & ~update);
        }
        psx_gte_snapshot(&snapshot); flags = (uint32)snapshot.flag;
        xport_gte_execute(0x1400006u);
        if ((sint32)flags < 0 && (flags & r_u32(0x1F8003CCu))) continue;
        if ((sint32)xport_gte_read_data(24u) <= 0) continue;
        for (axis = 0u; axis < 3u; ++axis) {
            screen[axis] = xport_gte_read_data(12u + axis); w_u32(0x1F8003A0u + axis * 4u, screen[axis]);
            depth[axis] = xport_gte_read_data(17u + axis);
        }
        high = depth[0] > depth[1] ? depth[0] : depth[1]; low = depth[0] > depth[1] ? depth[1] : depth[0];
        if (depth[2] > high) high = depth[2]; if (depth[2] < low) low = depth[2];
        average = (uint32)((sint32)(high * 3u + low) >> 4);
        if (((sint32)average < 16 && (sint32)high < 10) || (average >> 2u) >= 512u) continue;
        w_u32(0x1F80037Cu, average);
        for (axis = 0u; axis < 3u; ++axis) {
            x = (sint16)screen[axis]; y = (sint16)(screen[axis] >> 16u);
            clips[axis] = (x < 0 ? 1u : x >= width ? 2u : 0u) | (y < 0 ? 4u : y >= height ? 8u : 0u);
        }
        if (clips[0] & clips[1] & clips[2]) continue;
        xport_gte_write_data(0u, r_u32(record + 28u)); xport_gte_write_data(1u, r_u32(record + 32u));
        xport_gte_execute(0x180001u);
        for (axis = 0u; axis < 3u; ++axis) w_u32(packet + 8u + axis * 8u, r_u32(0x1F8003A0u + axis * 4u));
        average = r_u32(0x1F80037Cu) - 35u; w_u32(0x1F80037Cu, average);
        if ((sint32)average < 0) { average = 35u; w_u32(0x1F80037Cu, average); }
        w_u32(packet + 32u, xport_gte_read_data(14u));
        tag = r_u32(0x1F8003BCu) + ((uint32)((sint32)r_u32(0x1F80037Cu) >> 2) << 2u);
        header = r_u32(packet); next = r_u32(tag);
        w_u32(packet, (header & 0xFF000000u) | (next & 0xFFFFFFu));
        tag = r_u32(0x1F8003BCu) + ((uint32)((sint32)r_u32(0x1F80037Cu) >> 2) << 2u);
        w_u32(tag, (r_u32(tag) & 0xFF000000u) | (packet & 0xFFFFFFu));
    }
}

typedef struct MohSubdivisionVertex {
    sint16 x, y, z;
    uint16 depth, uv;
    uint32 screen, color, flags;
} MohSubdivisionVertex;

static MohSubdivisionVertex moh_subdivision_midpoint(MohSubdivisionVertex first, MohSubdivisionVertex second)
{
    MohSubdivisionVertex result;
    PsxGteSnapshot snapshot;
    uint32 axis, value;
    result.x = (sint16)(((sint32)first.x + second.x) >> 1);
    result.y = (sint16)(((sint32)first.y + second.y) >> 1);
    result.z = (sint16)(((sint32)first.z + second.z) >> 1);
    xport_gte_write_data(0u, (uint16)result.x | ((uint32)(uint16)result.y << 16u));
    xport_gte_write_data(1u, (uint32)(sint32)result.z);
    xport_gte_execute(0x180001u);
    psx_gte_snapshot(&snapshot); result.flags = (uint32)snapshot.flag;
    result.screen = xport_gte_read_data(14u); result.depth = (uint16)xport_gte_read_data(19u);
    result.uv = (uint16)((((first.uv & 255u) + (second.uv & 255u)) >> 1u) |
        (((((uint32)first.uv >> 8u) + ((uint32)second.uv >> 8u)) >> 1u) << 8u));
    result.color = 0u;
    for (axis = 0u; axis < 3u; ++axis) {
        value = (((first.color >> (axis * 8u)) & 255u) + ((second.color >> (axis * 8u)) & 255u)) >> 1u;
        result.color |= value << (axis * 8u);
    }
    return result;
}

static void moh_subdivision_record(uint32 record, const MohSubdivisionVertex vertices[3], uint16 clut, uint16 page, uint8 code)
{
    uint32 index;
    for (index = 0u; index < 3u; ++index) {
        w_u16(record + index * 8u, (uint16)vertices[index].x);
        w_u16(record + index * 8u + 2u, (uint16)vertices[index].y);
        w_u16(record + index * 8u + 4u, (uint16)vertices[index].z);
        w_u16(record + index * 8u + 6u, vertices[index].depth);
        w_u32(record + 24u + index * 4u, vertices[index].screen);
        w_u16(record + 36u + index * 4u, vertices[index].uv);
        w_u32(record + 48u + index * 4u, vertices[index].color);
    }
    w_u16(record + 38u, clut); w_u16(record + 42u, page); w_u8(record + 51u, code);
}

uint32 sub_80010A7C(uint32 triangle, uint32 packet)
{
    uint32 d0, d1, d2, high, low, average, index, screen[3], uv0, uv1, uv2, colors[3], tag, header;
    FUNCTION_MARKER(0x80010A7Cu, "SLUS_009.74");
    d0 = r_u16(triangle + 6u); d1 = r_u16(triangle + 14u); d2 = r_u16(triangle + 22u);
    high = d0 > d1 ? d0 : d1; low = d0 > d1 ? d1 : d0;
    if (d2 > high) high = d2; if (d2 < low) low = d2;
    average = (high * 3u + low) >> 4u;
    if ((sint32)average <= 0) return packet;
    for (index = 0u; index < 3u; ++index) screen[index] = r_u32(triangle + 24u + index * 4u);
    uv0 = r_u32(triangle + 36u); uv1 = r_u32(triangle + 40u); uv2 = r_u16(triangle + 44u);
    for (index = 0u; index < 3u; ++index) colors[index] = r_u32(triangle + 48u + index * 4u);
    for (index = 0u; index < 3u; ++index) w_u32(packet + 8u + index * 12u, screen[index]);
    w_u32(packet + 12u, uv0); w_u32(packet + 24u, uv1); w_u32(packet + 36u, uv2);
    for (index = 0u; index < 3u; ++index) w_u32(packet + 4u + index * 12u, colors[index]);
    tag = r_u32(0x1F800004u);
    w_u32(0x1F80000Cu, r_u32(0x1F80000Cu) + 1u);
    tag += average & ~3u; header = r_u32(tag);
    w_u32(packet, header | 0x09000000u); w_u32(tag, packet & 0xFFFFFFu);
    return packet + 40u;
}

uint32 sub_80010000(uint32 triangle, uint32 packet, uint32 mask, uint32 depth)
{
    static const uint32 permutations[8] = {0u, 0x24u, 0x09u, 0x12u, 0x12u, 0x09u, 0x24u, 0x64u};
    MohSubdivisionVertex input[3], a, b, c, ab, ac, bc, output[3];
    uint32 mode = mask & 7u, permutation, order[3], records, count, index, point, generated, child_mask, near;
    uint16 clut, page;
    uint8 code;
    FUNCTION_MARKER(0x80010000u, "SLUS_009.74");
    /* TODO Original jump-table entry zero transfers to address zero */
    if (!mode) moh_draft_fail_fast("Invalid subdivision mask", 0x80010000u);
    permutation = permutations[mode];
    near = r_u32(0x1F800010u) << 2u;
    for (index = 0u; index < 3u; ++index) {
        point = triangle + index * 8u;
        input[index].x = (sint16)r_u16(point); input[index].y = (sint16)r_u16(point + 2u);
        input[index].z = (sint16)r_u16(point + 4u); input[index].depth = r_u16(point + 6u);
        input[index].screen = r_u32(triangle + 24u + index * 4u);
        input[index].uv = r_u16(triangle + 36u + index * 4u);
        input[index].color = r_u32(triangle + 48u + index * 4u); input[index].flags = 0u;
        order[index] = (permutation >> (index * 2u)) & 3u;
    }
    a = input[order[0]]; b = input[order[1]]; c = input[order[2]];
    clut = r_u16(triangle + 38u); page = r_u16(triangle + 42u); code = r_u8(triangle + 51u);
    count = mode == 1u || mode == 2u || mode == 4u ? 3u : 4u;
    records = sub_80016718(count * 60u);
    if (!records) moh_draft_fail_fast("Subdivision records allocation", 0x80010000u);
    if (count == 3u) {
        ab = moh_subdivision_midpoint(a, b); ac = moh_subdivision_midpoint(a, c);
        output[0] = a; output[1] = ab; output[2] = ac;
        moh_subdivision_record(records, output, clut, page, code);
        output[0] = ab; output[1] = b; output[2] = ac;
        moh_subdivision_record(records + 60u, output, clut, page, code);
        output[0] = b; output[1] = c; output[2] = ac;
        moh_subdivision_record(records + 120u, output, clut, page, code);
        generated = ((sint32)ab.flags < 0 ? 16u : 0u) | (ab.depth < near ? 2u : 0u) |
            ((sint32)ac.flags < 0 ? 32u : 0u) | (ac.depth < near ? 4u : 0u);
        child_mask = generated | (mask & 9u);
        packet = (child_mask & 6u) && depth ? sub_80010000(records, packet, child_mask, depth - 1u) : sub_80010A7C(records, packet);
        child_mask = ((generated & 0x12u) >> 1u) | (generated & 0x24u);
        packet = (child_mask & 5u) && depth ? sub_80010000(records + 60u, packet, child_mask, depth - 1u) : sub_80010A7C(records + 60u, packet);
        child_mask = generated & 0x24u;
        packet = (child_mask & 4u) && depth ? sub_80010000(records + 120u, packet, child_mask, depth - 1u) : sub_80010A7C(records + 120u, packet);
    } else {
        ac = moh_subdivision_midpoint(a, c); bc = moh_subdivision_midpoint(b, c); ab = moh_subdivision_midpoint(a, b);
        output[0] = a; output[1] = ab; output[2] = ac;
        moh_subdivision_record(records, output, clut, page, code);
        output[0] = ab; output[1] = b; output[2] = bc;
        moh_subdivision_record(records + 60u, output, clut, page, code);
        output[0] = ac; output[1] = bc; output[2] = c;
        moh_subdivision_record(records + 120u, output, clut, page, code);
        output[0] = ac; output[1] = ab; output[2] = bc;
        moh_subdivision_record(records + 180u, output, clut, page, code);
        generated = ((sint32)ac.flags < 0 ? 8u : 0u) | (ac.depth < near ? 1u : 0u) |
            ((sint32)ab.flags < 0 ? 16u : 0u) | (ab.depth < near ? 2u : 0u) |
            ((sint32)bc.flags < 0 ? 32u : 0u) | (bc.depth < near ? 4u : 0u);
        packet = (generated & 7u) && (sint32)(depth - 1u) >= 0 ? sub_80010000(records + 180u, packet, generated, depth - 1u) : sub_80010A7C(records + 180u, packet);
        child_mask = (generated & 0x12u) | ((generated & 9u) << 2u);
        if ((child_mask & 6u) && depth) {
            child_mask |= mask & (9u << order[0]);
            packet = sub_80010000(records, packet, child_mask, depth - 1u);
        } else packet = sub_80010A7C(records, packet);
        child_mask = ((generated & 0x12u) >> 1u) | (generated & 0x24u) | (mask & (9u << order[1]));
        packet = (child_mask & 5u) && (sint32)(depth - 1u) >= 0 ? sub_80010000(records + 60u, packet, child_mask, depth - 1u) : sub_80010A7C(records + 60u, packet);
        child_mask = (generated & 9u) | ((generated & 0x24u) >> 1u) | (mask & (9u << order[2]));
        packet = (child_mask & 3u) && (sint32)(depth - 1u) >= 0 ? sub_80010000(records + 120u, packet, child_mask, depth - 1u) : sub_80010A7C(records + 120u, packet);
    }
    sub_80036E44(records); return packet;
}

static void moh_scene_vertices(uint32 vertices, const uint32 ids[3], uint32 short_z)
{
    uint32 index, point, z;
    for (index = 0u; index < 3u; ++index) {
        point = vertices + ids[index] * 8u;
        z = short_z ? r_u16(point + 4u) : r_u32(point + 4u);
        xport_gte_write_data(index * 2u + 1u, z);
        xport_gte_write_data(index * 2u, r_u32(point));
    }
}

uint32 sub_80010B60(uint32 packet, uint32 end, uint32 region, uint32 ordering_table)
{
    uint32 light, vertices, radius, index, position, leaf, triangle, next_triangle, remaining, packed;
    uint32 ids[3], colors[3], colors_out, parameters, table, classification, uv, texture, depth, factor, tag, point;
    uint32 count;
    FUNCTION_MARKER(0x80010B60u, "SLUS_009.74");
    count = r_u32(0x800AA19Cu); radius = r_u16(0x800AA18Cu); light = r_u32(0x8009D650u);
    w_u16(0x1F8003B8u, 0u); w_u16(0x1F8003BAu, 0u); vertices = r_u32(region + 44u);
    if (light) {
        for (index = 0u; index < 3u; ++index) {
            position = (uint32)((sint32)r_u32(light + 0x20Cu + index * 4u) >> 16);
            w_u16(0x1F800390u + index * 2u, (uint16)position);
            w_u16(0x1F800396u + index * 2u, (uint16)(position - radius));
            w_u16(0x1F80039Cu + index * 2u, (uint16)(position + radius));
        }
    }
    w_u32(0x1F8003A4u, r_u32(0x800AA168u));
    w_u32(0x1F800000u, 0x1F800050u);
    w_u16(0x1F8003CCu, (uint16)r_u32(0x8009C828u)); w_u16(0x1F8003CEu, (uint16)r_u32(0x8009C82Cu));
    parameters = r_u32(0x8009CC64u);
    colors_out = sub_80016718(12u);
    if (!colors_out) moh_draft_fail_fast("Scene colors allocation", 0x80010B60u);
    while ((sint32)--count >= 0 && packet + 640u < end) {
        leaf = r_u32(0x800AA1A0u + (uint32)count * 4u);
        if (light) w_u32(0x80011B24u, sub_80011AB0(leaf));
        triangle = r_u32(leaf + 24u); remaining = r_u32(leaf + 12u);
        if (!remaining) continue;
        packed = r_u32(triangle); ids[0] = packed & 0xFFFFu; ids[1] = packed >> 16u; ids[2] = r_u16(triangle + 4u);
        w_u16(0x1F8003BAu, (uint16)(r_u16(0x1F8003BAu) + remaining));
        moh_scene_vertices(vertices, ids, 0u); xport_gte_execute(0x280030u);
        while (remaining) {
            if (packet + 640u >= end) { sub_80036E44(colors_out); return packet; }
            --remaining; next_triangle = remaining ? triangle + 16u : triangle;
            classification = sub_80011214(next_triangle, vertices);
            if (!classification) {
                for (index = 0u; index < 3u; ++index) ids[index] = r_u16(0x1F8003C0u + index * 2u);
                xport_gte_execute(0x280030u); triangle = next_triangle; continue;
            }
            table = r_u32(r_u32(region + 8u) + 24u);
            for (index = 0u; index < 3u; ++index) w_u16(0x1F8003C6u + index * 2u, (uint16)ids[index]);
            colors[0] = r_u32(table + ids[0] * 4u);
            w_u16(0x1F8003B8u, (uint16)(r_u16(0x1F8003B8u) + 1u));
            colors[1] = r_u32(table + ids[1] * 4u); colors[2] = r_u32(table + ids[2] * 4u);
            if (r_u32(parameters + 0xD8u)) for (index = 0u; index < 3u; ++index)
                if ((colors[index] & 0xFF00FFu) < 40u)
                    colors[index] = (colors[index] & 0xFF000000u) |
                        (r_u32(parameters + 0xE8u + (colors[index] & 255u) * 20u) & 0xFFFFFFu);
            if (light && (sint32)(r_u32(0x80011B24u) - 1u) >= 0) {
                for (index = 0u; index < 3u; ++index) w_u32(colors_out + index * 4u, colors[index]);
                sub_800114B0(region, vertices, colors_out);
                for (index = 0u; index < 3u; ++index) colors[index] = r_u32(colors_out + index * 4u);
            }
            for (index = 0u; index < 3u; ++index) w_u32(packet + 8u + index * 12u, r_u32(0x1F80002Cu + index * 4u));
            uv = r_u32(triangle + 8u); depth = r_u16(parameters + 0xCCu);
            if (classification < 2u && depth < (r_u32(0x1F8003A8u) << 2u)) {
                for (index = 0u; index < 3u; ++index) {
                    factor = (r_u16(0x1F80001Au + index * 8u) - depth) * 2u;
                    if ((sint32)factor < 0) factor = 0u; else if (factor >= 4096u) factor = 4096u;
                    xport_gte_write_data(6u, colors[index]); xport_gte_write_data(8u, factor);
                    xport_gte_execute(0x780010u);
                    colors[index] = xport_gte_read_data(22u);
                }
            }
            w_u32(packet + 4u, colors[0]); w_u32(packet + 16u, colors[1]); w_u16(packet + 14u, (uint16)uv);
            w_u32(0x1F800044u, colors[0]); w_u32(packet + 28u, colors[2]); w_u32(0x1F800048u, colors[1]);
            if (classification < 2u) {
                xport_gte_execute(0x280030u);
                tag = ordering_table + (r_u32(0x1F8003A8u) & ~3u);
                w_u32(packet, r_u32(tag) | 0x09000000u); w_u32(tag, packet & 0xFFFFFFu);
                w_u16(packet + 24u, (uint16)(uv >> 16u)); texture = r_u32(triangle + 12u);
                for (index = 0u; index < 3u; ++index) ids[index] = r_u16(0x1F8003C0u + index * 2u);
                w_u16(packet + 26u, (uint16)(texture & 0xFFFu)); w_u16(packet + 36u, (uint16)(texture >> 16u));
                w_u16(packet + 12u, r_u16(triangle + 6u)); w_u8(packet + 7u, (uint8)(texture & 0x4000u ? 0x36u : 0x34u));
                packet += 40u;
            } else {
                w_u32(0x1F80004Cu, colors[2]); w_u16(0x1F80003Au, (uint16)uv);
                w_u32(0x1F8003ACu, end); w_u32(0x1F8003B0u, region); w_u32(0x1F8003B4u, ordering_table);
                for (index = 0u; index < 3u; ++index) {
                    point = vertices + r_u16(0x1F8003C6u + index * 2u) * 8u;
                    w_u32(0x1F800014u + index * 8u, r_u32(point));
                    w_u16(0x1F800018u + index * 8u, r_u16(point + 4u));
                }
                texture = r_u32(triangle + 12u);
                w_u16(0x1F80003Eu, (uint16)(texture & 0xFFFu)); w_u16(0x1F800040u, (uint16)(texture >> 16u));
                w_u16(0x1F80003Cu, (uint16)(uv >> 16u)); w_u32(0x1F800004u, ordering_table); w_u32(0x1F80000Cu, 0u);
                w_u8(0x1F800047u, (uint8)(texture & 0x4000u ? 0x36u : 0x34u));
                w_u32(0x1F800010u, r_u32(0x1F8003A4u)); w_u16(0x1F800038u, r_u16(triangle + 6u));
                /* Subdivision uses ordinary addressable C locals instead of a guest stack */
                packet = moh_draft_missing_call(0x80010000u, 0x1F800014u, packet, classification >> 1u, r_u32(0x800AA164u) - 1u);
                for (index = 0u; index < 3u; ++index) ids[index] = r_u16(0x1F8003C0u + index * 2u);
                moh_scene_vertices(vertices, ids, 1u); xport_gte_execute(0x280030u);
                end = r_u32(0x1F8003ACu); region = r_u32(0x1F8003B0u); ordering_table = r_u32(0x1F8003B4u);
            }
            triangle = next_triangle;
        }
    }
    sub_80036E44(colors_out); return packet;
}

uint32 sub_80011AB0(uint32 node)
{
    uint32 axis;
    FUNCTION_MARKER(0x80011AB0u, "SLUS_009.74");
    for (axis = 0u; axis < 3u; ++axis)
        if ((sint16)r_u16(0x1F800396u + axis * 2u) >= (sint16)r_u16(node + 6u + axis * 2u)) return 0u;
    for (axis = 0u; axis < 3u; ++axis)
        if ((sint16)r_u16(node + axis * 2u) >= (sint16)r_u16(0x1F80039Cu + axis * 2u)) return 0u;
    return 1u;
}

static uint32 moh_triangle_clip(uint32 screen, uint32 width, uint32 height)
{
    sint32 x = (sint16)screen, y = (sint16)(screen >> 16u);
    return (x < 0 ? 1u : (uint32)x >= width ? 2u : 0u) |
        (y < 0 ? 4u : (uint32)y >= height ? 8u : 0u);
}

uint32 sub_80011214(uint32 next_triangle, uint32 vertices)
{
    uint32 packed, ids[3], points[3], depth[3], screen[3], flags, index, high, low, average, split, near, clip[3];
    sint32 area;
    FUNCTION_MARKER(0x80011214u, "SLUS_009.74");
    packed = r_u32(next_triangle); ids[0] = packed & 0xFFFFu; ids[1] = packed >> 16u;
    w_u16(0x1F8003C0u, (uint16)ids[0]); w_u16(0x1F8003C2u, (uint16)ids[1]);
    ids[2] = r_u16(next_triangle + 4u);
    flags = xport_gte_read_data(31u); w_u16(0x1F8003C4u, (uint16)ids[2]);
    xport_gte_execute(0x1400006u);
    area = (sint32)xport_gte_read_data(24u);
    for (index = 0u; index < 3u; ++index) {
        points[index] = vertices + ids[index] * 8u;
        xport_gte_write_data(index * 2u, r_u32(points[index]));
        xport_gte_write_data(index * 2u + 1u, r_u32(points[index] + 4u));
        if (index < 2u) {
            depth[index] = xport_gte_read_data(17u + index);
            screen[index] = xport_gte_read_data(12u + index);
            w_u16(0x1F80001Au + index * 8u, (uint16)depth[index]);
            w_u32(0x1F80002Cu + index * 4u, screen[index]);
        }
    }
    depth[2] = xport_gte_read_data(19u);
    if (((flags & 0x80000000u) && (flags & 0x7FF98000u)) || area <= 0) return 0u;
    screen[2] = xport_gte_read_data(14u);
    low = depth[0] < depth[1] ? depth[0] : depth[1]; high = depth[0] < depth[1] ? depth[1] : depth[0];
    if (depth[2] <= low) low = depth[2]; if (high < depth[2]) high = depth[2];
    average = (high * 3u + low) >> 4u;
    w_u16(0x1F80002Au, (uint16)depth[2]); w_u32(0x1F800034u, screen[2]);
    near = r_u32(0x1F8003A4u); w_u32(0x1F8003A8u, average); split = 0u;
    if (near) {
        w_u16(0x1F800042u, (uint16)average); near <<= 2u;
        for (index = 0u; index < 3u; ++index) if (depth[index] < near) split |= 1u << index;
    }
    if ((high < 10u && average < 16u) || (average >> 2u) >= 512u) return 0u;
    for (index = 0u; index < 3u; ++index)
        clip[index] = moh_triangle_clip(screen[index], r_u16(0x1F8003CCu), r_u16(0x1F8003CEu));
    if (clip[0] & clip[1] & clip[2]) return 0u;
    return split ? split << 1u : 1u;
}

static sint32 moh_light_quotient(uint32 distance, uint32 divisor)
{
    sint32 numerator = (sint32)(distance << 8u), denominator = (sint32)divisor;
    /* TODO Original division exceptions fail fast on native */
    if (!denominator || (numerator == (sint32)0x80000000u && denominator == -1))
        moh_draft_fail_fast("Scene light division", 0x800114B0u);
    return numerator / denominator;
}

void sub_800114B0(uint32 region, uint32 vertices, uint32 colors_out)
{
    uint32 distances[3], ids[3], index, axis, vertex, packed, xy, radius, table, color, factor, output, channel;
    sint32 center[3], scale, value;
    FUNCTION_MARKER(0x800114B0u, "SLUS_009.74");
    for (axis = 0u; axis < 3u; ++axis) center[axis] = (sint16)r_u16(0x1F800390u + axis * 2u);
    for (index = 0u; index < 3u; ++index) {
        ids[index] = r_u16(0x1F8003C6u + index * 2u); vertex = vertices + ids[index] * 8u;
        xy = r_u32(vertex);
        xport_gte_write_data(9u, (uint32)((sint32)(sint16)xy - center[0]));
        xport_gte_write_data(10u, (uint32)((sint32)(sint16)(xy >> 16u) - center[1]));
        xport_gte_write_data(11u, (uint32)((sint32)(sint16)r_u16(vertex + 4u) - center[2]));
        xport_gte_execute(0xA00428u);
        distances[index] = xport_gte_read_data(25u) + xport_gte_read_data(26u) + xport_gte_read_data(27u);
    }
    if (r_u32(0x800AA160u)) {
        table = r_u32(r_u32(region + 8u) + 24u); radius = r_u32(0x800AA190u);
        for (index = 0u; index < 3u; ++index) if (distances[index] <= radius) {
            vertex = table + ids[index] * 4u; color = r_u32(vertex);
            scale = (sint32)(0x10000u - (uint32)moh_light_quotient(distances[index],
                index == 0u ? (uint32)((sint32)radius >> 8) : radius >> 8u)) >> 3;
            for (axis = 0u; axis < 3u; ++axis) {
                channel = (color >> (axis * 8u)) & 255u;
                value = (sint32)(channel - (uint32)((sint32)(channel * (uint32)scale) >> 16));
                if (value < 1) value = 1;
                w_u8(vertex + axis, (uint8)value);
            }
            distances[index] = (distances[index] << 8u) >> 8u;
        }
    }
    radius = r_u32(0x800AA190u);
    for (index = 0u; index < 3u; ++index) if (distances[index] <= radius) {
        factor = 0x10000u - (uint32)moh_light_quotient(distances[index], radius >> 8u);
        packed = r_u32(colors_out + index * 4u); output = 0u;
        for (axis = 0u; axis < 3u; ++axis) {
            channel = ((packed >> (axis * 8u)) & 255u) + ((factor * r_u8(0x800AA188u + axis)) >> 16u);
            if (channel >= 256u) channel = 255u;
            output |= channel << (axis * 8u);
        }
        w_u32(colors_out + index * 4u, output);
    }
}

void level_sub_80061598(void)
{
    FUNCTION_MARKER(0x80061598u, "LEVEL.BIN");
    moh_draft_missing_call(0x8008A554u);
    moh_draft_missing_call(0x8008EA70u);
    moh_draft_missing_call(0x8004DDACu);
    moh_draft_missing_call(0x80061590u);
    moh_draft_missing_call(0x80061568u);
    moh_draft_missing_call(0x80049410u);
    moh_draft_missing_call(0x80041260u);
    moh_draft_missing_call(0x8005F260u);
    moh_draft_missing_call(0x800606E0u, 0u);
    moh_draft_missing_call(0x8007B780u);
}

uint32 level_sub_8004943C(void)
{
    FUNCTION_MARKER(0x8004943Cu, "LEVEL.BIN");
    return 0u;
}

void level_sub_800606E0(uint32 color)
{
    uint32 index, copy, coordinate, packet;
    sint32 cosine0, sine0, cosine1, sine1;
    sint32 coordinates[8];
    FUNCTION_MARKER(0x800606E0u, "LEVEL.BIN");
    for (index = 0u; index < 32u; ++index)
    {
        cosine0 = rcos((sint32)(index * 128u));
        sine0 = rsin((sint32)(index * 128u));
        cosine1 = rcos((sint32)((index + 1u) * 128u));
        sine1 = rsin((sint32)((index + 1u) * 128u));
        coordinates[0] = ((sine0 * 20 + 2048) >> 12) * 2 + 60;
        coordinates[1] = 44 - ((cosine0 * 20 + 2048) >> 12);
        coordinates[2] = ((sine0 * 14 + 2048) >> 12) * 2 + 60;
        coordinates[3] = 44 - ((cosine0 * 14 + 2048) >> 12);
        coordinates[4] = ((sine1 * 20 + 2048) >> 12) * 2 + 60;
        coordinates[5] = 44 - ((cosine1 * 20 + 2048) >> 12);
        coordinates[6] = ((sine1 * 14 + 2048) >> 12) * 2 + 60;
        coordinates[7] = 44 - ((cosine1 * 14 + 2048) >> 12);
        for (copy = 0u; copy < 2u; ++copy)
        {
            packet = 0x800AB970u + copy * 0xC00u + index * 24u;
            if (index & 1u) w_u32(packet, 0u);
            else w_u8(packet + 3u, 11u);
            w_u8(packet + 7u, 0x28u);
            for (coordinate = 0u; coordinate < 8u; ++coordinate)
                w_u16(packet + 8u + coordinate * 2u, (uint16)coordinates[coordinate]);
        }
    }
    moh_draft_missing_call(0x80060548u, color);
    for (index = 0u; index < 2u; ++index)
        moh_draft_missing_call(0x800604D0u, index, color);
}

void level_native_master_volume(uint32 left, uint32 right)
{
    SpuCommonAttr attribute = {0};
    attribute.mask = SPU_COMMON_MVOLL | SPU_COMMON_MVOLR;
    attribute.mvol.left = (sint16)(left & 0x7FFFu);
    attribute.mvol.right = (sint16)(right & 0x7FFFu);
    SpuSetCommonAttr(&attribute);
}

void level_native_cd_mix(uint32 enabled)
{
    SpuCommonAttr attribute = {0};
    attribute.mask = 0x100u;
    attribute.cd.mix = enabled != 0u;
    SpuSetCommonAttr(&attribute);
}

void level_native_cd_volume(uint32 left, uint32 right)
{
    SpuCommonAttr attribute = {0};
    attribute.mask = SPU_COMMON_CDVOLL | SPU_COMMON_CDVOLR;
    attribute.cd.volume.left = (sint16)left;
    attribute.cd.volume.right = (sint16)right;
    SpuSetCommonAttr(&attribute);
}

void level_sub_800604D0(uint32 buffer)
{
    uint32 packet = 0x800AD170u + buffer * 40u;
    FUNCTION_MARKER(0x800604D0u, "LEVEL.BIN");
    w_u8(packet + 3u, 9u);
    w_u8(packet + 7u, 0x2Eu);
    w_u16(packet + 22u, 0xFu);
    w_u16(packet + 14u, 0x3E3Eu);
    w_u8(packet + 13u, 0x60u);
    w_u8(packet + 21u, 0x60u);
    w_u8(packet + 12u, 0x80u);
    w_u8(packet + 20u, 0xBFu);
    w_u8(packet + 28u, 0x80u);
    w_u8(packet + 29u, 0x9Fu);
    w_u8(packet + 36u, 0xBFu);
    w_u8(packet + 37u, 0x9Fu);
    w_u8(packet + 4u, 0x80u);
    w_u8(packet + 5u, 0x80u);
    w_u8(packet + 6u, 0x80u);
}

void level_sub_800616D4(void)
{
    uint32 count, length, source;
    FUNCTION_MARKER(0x800616D4u, "LEVEL.BIN");
    w_u32(0x8009A5D8u, r_u32(0x8009A5D8u) + 1u);
    if (!r_u32(0x8009CC6Cu))
    {
        if (!r_u8(0x80093B30u) && r_u32(0x800AA184u) != 0xFFFFFFFFu)
            moh_draft_missing_call(0x8008C798u, r_u32(0x800AA184u));
        if (r_u32(0x800A0128u))
        {
            if (!r_u8(0x800A0150u))
            {
                length = r_u32(0x800A012Cu);
                source = r_u32(0x800A0148u);
                if ((sint32)length < 0x5000) w_u8(source + length - 15u, 3u);
                else w_u8(source + 0x4FF1u, 3u);
            }
            moh_draft_missing_call(0x8001575Cu, r_u32(0x800A0148u), 0x5000u);
            w_u32(0x800A0128u, 0u);
        }
    }
    moh_draft_missing_call(0x8008A580u);
    moh_draft_missing_call(0x8008EAC8u);
    moh_draft_missing_call(0x8004DE3Cu);
    if (r_u32(0x8009A5E0u))
    {
        count = r_u32(0x8009A5D0u);
        w_u32(0x8009A5C0u, 0u);
        if ((sint32)count > 3) count = 3u;
        w_u32(0x8009A5D0u, count);
        w_u32(0x8009A5D4u, count);
        do
        {
            if ((sint32)count > 0) moh_draft_missing_call(0x8004DE3Cu);
            --count;
        } while ((sint32)count >= 0);
    }
    else
    {
        count = r_u32(0x8009A5D0u);
        if ((sint32)count > 3) count = 3u;
        w_u32(0x8009A5D0u, count);
        w_u32(0x8009A5D4u, count);
        do
        {
            w_u32(0x8009A5DCu, r_u32(0x8009A5DCu) + 1u);
            moh_draft_missing_call(0x8006BA20u, r_u32(0x8009D64Cu));
            moh_draft_missing_call(0x8006C08Cu, r_u32(0x8009D64Cu));
            moh_draft_missing_call(0x8005F69Cu);
            if ((sint32)r_u32(0x8009A5D0u) > 0) moh_draft_missing_call(0x8004DE3Cu);
            if ((sint32)r_u32(0x8009A5C0u) > 9)
            {
                w_u32(0x8009A5C0u, r_u32(0x8009A5C0u) - 20u);
                moh_draft_missing_call(0x800662CCu, 0x59u, 0u, 0u, 0u);
            }
            count = r_u32(0x8009A5D0u) - 1u;
            w_u32(0x8009A5D0u, count);
        } while ((sint32)count >= 0);
        w_u32(0x8009A5D0u, 0u);
    }
    moh_draft_missing_call(0x800503D4u);
    moh_draft_missing_call(0x8004137Cu);
    moh_draft_missing_call(0x8006B894u);
}

uint32 level_sub_800869E0(uint32 data)
{
    uint32 flags, clut, image, next;
    PSX_RECT *rectangle;
    uint32 *pixels;
    FUNCTION_MARKER(0x800869E0u, "LEVEL.BIN");
    while (r_u32(data) == 0x10u)
    {
        /* Native TIM parsing retains the SDK cursor and guest block addresses */
        w_u32(0x800CBCA4u, data);
        flags = r_u32(data + 4u);
        clut = data + 8u;
        image = clut;
        if (flags & 8u) image += r_u32(clut) & ~3u;
        w_u32(0x800CBCA4u, image + (r_u32(image) & ~3u));
        rectangle = (PSX_RECT *)moh_draft_guest_pointer(image + 4u, sizeof(PSX_RECT));
        pixels = (uint32 *)moh_draft_guest_pointer(image + 12u,
            (uint32)(uint16)rectangle->w * (uint32)(uint16)rectangle->h * 2u);
        LoadImagePSX(rectangle, pixels);
        next = data + 8u;
        if (flags & 8u)
        {
            rectangle = (PSX_RECT *)moh_draft_guest_pointer(clut + 4u, sizeof(PSX_RECT));
            pixels = (uint32 *)moh_draft_guest_pointer(clut + 12u,
                (uint32)(uint16)rectangle->w * (uint32)(uint16)rectangle->h * 2u);
            LoadImagePSX(rectangle, pixels);
            next += r_u32(next);
        }
        data = next + r_u32(next);
    }
    DrawSync(0);
    return data;
}

void level_sub_80086A94(uint32 table, uint32 first_index)
{
    uint32 count = r_u32(table), index;
    FUNCTION_MARKER(0x80086A94u, "LEVEL.BIN");
    for (index = 0u; index < count; ++index)
        moh_draft_missing_call(0x800418E4u, table + r_u32(table + 4u + index * 4u),
            (uint32)(sint32)(sint16)(index + first_index));
}

void level_sub_80087294(void)
{
    uint32 node, object, player;
    FUNCTION_MARKER(0x80087294u, "LEVEL.BIN");
    node = moh_draft_missing_call(0x8004787Cu);
    object = r_u32(0x8009D64Cu);
    w_u32(0x8009D638u, node);
    w_u32(0x8009D64Cu, node);
    w_u32(node + 4u, object);
    object = moh_draft_missing_call(0x800754CCu, 0u);
    player = moh_draft_missing_call(0x80074BB8u, 1u, object);
    w_u32(0x8009D654u, player);
    moh_draft_missing_call(0x8007BE04u, player);
    moh_draft_missing_call(0x80048DC8u, r_u32(0x8009D638u), r_u32(0x8009D654u), 0u);
}

void level_sub_8006B834(uint32 scene)
{
    uint32 node = r_u32(0x8009D638u);
    FUNCTION_MARKER(0x8006B834u, "LEVEL.BIN");
    w_u32(scene + 4u, r_u32(node + 4u));
    w_u32(node + 4u, scene);
}

void level_sub_80061130(void)
{
    FUNCTION_MARKER(0x80061130u, "LEVEL.BIN");
    moh_draft_missing_call(0x80046874u, r_u32(0x8009D654u));
}

void level_sub_800412E0(void)
{
    uint32 channel;
    FUNCTION_MARKER(0x800412E0u, "LEVEL.BIN");
    for (channel = 1u; channel < 24u; ++channel)
        moh_draft_missing_call(0x800425C4u, channel);
    w_u32(0x80090FC8u, 0u);
}

uint32 level_sub_8004270C(uint32 sample, uint32 program_count, uint32 bank_header)
{
    uint32 index = 0u, entry = bank_header + 0x824u;
    uint32 limit = program_count << 4u, identifier, value;
    FUNCTION_MARKER(0x8004270Cu, "LEVEL.BIN");
    while ((sint32)index < (sint32)limit)
    {
        identifier = (uint32)r_u8(entry + 18u) | ((uint32)r_u8(entry + 19u) << 8u);
        if (sample == identifier - 1u)
        {
            value = r_u16(0x80090FD0u + ((uint32)r_u8(entry) + 4u) * 2u);
            return value < 0x1000u ? value : 0x1000u;
        }
        entry += 32u;
        ++index;
    }
    return 0x800u;
}

uint32 level_sub_8006A8FC(uint32 index)
{
    FUNCTION_MARKER(0x8006A8FCu, "LEVEL.BIN");
    return r_u32(0x8009EC58u) + (index << 8u);
}

void level_sub_8006D8A4(uint32 handle_out)
{
    FUNCTION_MARKER(0x8006D8A4u, "LEVEL.BIN");
    w_u32(handle_out, 0u);
}

uint32 level_sub_8006D84C(uint32 handle_out, uint32 object)
{
    FUNCTION_MARKER(0x8006D84Cu, "LEVEL.BIN");
    if (object) w_u32(handle_out, r_u32(object));
    else level_sub_8006D8A4(handle_out);
    return object;
}

void level_sub_8005D104(uint32 object)
{
    uint32 index;
    FUNCTION_MARKER(0x8005D104u, "LEVEL.BIN");
    for (index = 0u; index < 40u; ++index) w_u32(object + 0xE8u - index * 4u, 0u);
    w_u32(object + 0xECu, 0u);
}

void level_sub_8006B350(uint32 object)
{
    static const uint32 zero_words[19] = {
        0xF8u, 0x298u, 0x36Cu, 0x370u, 0x374u, 0x378u, 0x37Cu, 0x380u,
        0x384u, 0x20Cu, 0x210u, 0x214u, 0x228u, 0x22Cu, 0x230u,
        0x218u, 0x21Cu, 0x220u, 0x200u
    };
    static const uint32 zero_halves_before[6] = {0x2DCu, 0x2DAu, 0x2D8u, 0x2E4u, 0x2E2u, 0x2E0u};
    static const uint32 zero_halves_after[12] = {
        0x2B4u, 0x2B6u, 0x2B8u, 0x2BCu, 0x2BEu, 0x2C0u,
        0x2C4u, 0x2C6u, 0x2C8u, 0x2CCu, 0x2CEu, 0x2D0u
    };
    uint32 index, nested, section;
    FUNCTION_MARKER(0x8006B350u, "LEVEL.BIN");
    w_u32(object + 8u, 0u); w_u32(object + 12u, 0u);
    w_u32(object + 16u, 0u); w_u32(object + 24u, 0u); w_u32(object + 40u, 0u);
    moh_draft_missing_call(0x8006D84Cu, object + 44u, 0u);
    moh_draft_missing_call(0x8006D84Cu, object + 48u, 0u);
    nested = r_u32(object + 0x100u);
    w_u32(object + 0xF4u, 0x640000u);
    w_u32(object + 0x294u, 1u); w_u32(object + 0x290u, 1u);
    for (index = 0u; index < 15u; ++index) w_u32(object + zero_words[index], 0u);
    w_u32(object + 0x270u, 0x10000u); w_u32(object + 0x274u, 0x10000u);
    w_u32(object + 0x278u, 0x10000u);
    for (index = 15u; index < 19u; ++index) w_u32(object + zero_words[index], 0u);
    w_u32(object + 0x204u, 0u); w_u32(object + 0x1FCu, 0u);
    if (nested)
    {
        w_u16(r_u32(object + 0x100u) + 0x5Cu, 0xFFFFu);
        w_u16(r_u32(object + 0x100u) + 0x5Eu, 0u);
        w_u16(r_u32(object + 0x100u) + 0x62u, 0xFFFFu);
        w_u16(r_u32(object + 0x100u) + 0x64u, 0u);
        w_u16(r_u32(object + 0x100u) + 0x3Eu, 0u);
        w_u32(r_u32(object + 0x100u) + 8u, 0u);
    }
    for (index = 0u; index < 2u; ++index)
    {
        section = object + index * 0x74u;
        w_u32(section + 0x114u, 0u); w_u16(section + 0x11Eu, 0xFFFFu);
        w_u32(section + 0x104u, 0u); w_u32(section + 0x108u, 0u);
        w_u32(section + 0x10Cu, 0u); w_u32(section + 0x110u, 0u);
        w_u16(section + 0x122u, 0u); w_u32(section + 0x114u, 0u);
        w_u16(section + 0x120u, 0u); w_u16(section + 0x11Cu, 0xFFFFu);
    }
    w_u32(object + 0x288u, 1u); w_u32(object + 0x28Cu, 1u); w_u32(object + 0x284u, 1u);
    w_u32(object + 0x24Cu, 0u); w_u32(object + 0x250u, 0u); w_u32(object + 0x254u, 0u);
    w_u32(object + 0x1F4u, 0u); w_u16(object + 0x36u, 0u);
    for (index = 0u; index < 6u; ++index) w_u16(object + zero_halves_before[index], 0u);
    w_u16(object + 0x2E8u, 100u); w_u16(object + 0x2ECu, 0u); w_u32(object + 0x2D4u, 0u);
    moh_draft_missing_call(0x8005D104u, object);
    w_u8(object + 0x2A0u, 0x40u); w_u8(object + 0x2A1u, 0x40u); w_u8(object + 0x2A2u, 0x40u);
    w_u16(object + 0x2A4u, 0x80u); w_u16(object + 0x2A6u, 0x80u); w_u16(object + 0x2A8u, 0x80u);
    w_u16(object + 0x2ACu, 0x1000u); w_u16(object + 0x2AEu, 0x1000u); w_u16(object + 0x2B0u, 0x1000u);
    w_u32(object + 0x224u, 0x10000u);
    for (index = 0u; index < 12u; ++index) w_u16(object + zero_halves_after[index], 0u);
    w_u32(object + 0x44u, 0u); w_u32(object + 0x48u, 0u);
    w_u32(object + 0x3Cu, 0u); w_u32(object + 0x40u, 0u);
    w_u32(object + 0x38u, 0u); w_u32(object + 0x3A0u, 0u);
    w_u16(object + 0x2EAu, 0xFFFFu); w_u16(object + 0x3C8u, 100u);
    w_u16(object + 0x3CAu, 0xFFFFu);
}

void level_sub_8006A76C(uint32 object)
{
    SVECTOR rotation = {0};
    VECTOR translation = {0};
    VECTOR scale;
    MATRIX *matrix;
    FUNCTION_MARKER(0x8006A76Cu, "LEVEL.BIN");
    scale.vx = (sint32)r_u32(0x8003CEE8u);
    scale.vy = (sint32)r_u32(0x8003CEECu);
    scale.vz = (sint32)r_u32(0x8003CEF0u);
    scale.pad = (sint32)r_u32(0x8003CEF4u);
    rotation.vx = (sint16)r_u16(object + 0x22Au);
    rotation.vy = (sint16)r_u16(object + 0x22Eu);
    rotation.vz = (sint16)r_u16(object + 0x232u);
    matrix = (MATRIX *)moh_draft_guest_pointer(object + 0x340u, sizeof(MATRIX));
    /* Native PsyQ adapters for original SDK 1AF98, 16218 and 165D8 */
    RotMatrix(&rotation, matrix);
    translation.vx = (sint16)r_u16(object + 0x20Eu);
    translation.vy = (sint16)r_u16(object + 0x212u);
    translation.vz = (sint16)r_u16(object + 0x216u);
    TransMatrix(matrix, &translation);
    scale.vx = (sint32)r_u32(object + 0x270u) >> 4;
    scale.vy = (sint32)r_u32(object + 0x274u) >> 4;
    scale.vz = (sint32)r_u32(object + 0x278u) >> 4;
    ScaleMatrix(matrix, &scale);
}

void level_sub_800467EC(uint32 player)
{
    uint32 state, weapon;
    sint32 index, first, second;
    FUNCTION_MARKER(0x800467ECu, "LEVEL.BIN");
    state = r_u32(player + 0x388u);
    index = (sint8)r_u8(state + 0x54u);
    weapon = state + (uint32)index * 2u;
    first = (sint16)r_u16(weapon + 0x66u);
    second = (sint16)r_u16(weapon + 0x56u);
    if ((sint8)r_u8(0x8009E878u + (uint32)index) == 7)
    {
        w_u32(state + 0x108u, 0u);
        w_u32(state + 0x10Cu, 0u);
    }
    else
    {
        moh_draft_missing_call(0x80046700u, state + 0x108u, (uint32)first, 0u, 2u);
        moh_draft_missing_call(0x80046700u, state + 0x10Cu, (uint32)second, 0u, 3u);
    }
}

sint32 level_sub_8005F28C(uint32 value, uint32 x, uint32 y, uint32 red,
    uint32 green, uint32 blue, uint32 kind, uint32 duration, uint32 id,
    uint32 mask, uint32 out_id)
{
    uint32 slot, record, scan, candidate, count;
    FUNCTION_MARKER(0x8005F28Cu, "LEVEL.BIN");
    slot = r_u32(0x80099ED0u);
    if ((sint32)slot >= 50) return -1;
    record = 0x80099F4Cu + slot * 32u;
    w_u32(record, value);
    w_u16(record + 4u, x);
    w_u16(record + 6u, y);
    w_u8(record + 8u, red);
    w_u8(record + 9u, green);
    w_u8(record + 10u, blue);
    w_u8(record + 11u, kind);
    w_u32(record + 12u, duration);
    if ((id & 0xFFu) == 0xFEu)
    {
        scan = 0u;
        candidate = (moh_draft_missing_call(0x8001552Cu) & 0x7Fu) + 0x78u;
        if ((sint32)r_u32(0x80099ED0u) > 0)
        {
            do
            {
                id = r_u8(0x80099F4Cu + scan * 32u + 31u);
                ++scan;
                if (id == (candidate & 0xFFu))
                {
                    scan = 0u;
                    candidate = (moh_draft_missing_call(0x8001552Cu) & 0x7Fu) + 0x78u;
                }
            } while ((sint32)scan < (sint32)r_u32(0x80099ED0u));
        }
        w_u8(out_id, candidate);
        w_u8(0x80099F4Cu + scan * 32u + 31u, candidate);
    }
    else w_u8(record + 31u, id);
    count = r_u32(0x80099ED0u);
    w_u32(record + 16u, 0xFFFFFFFEu);
    w_u32(record + 24u, mask);
    w_u32(0x80099ED0u, count + 1u);
    return (sint32)slot;
}

void level_sub_800469E0(void)
{
    static const uint32 sources[13] = {
        0x8009E834u, 0x8009E838u, 0x8009E83Cu, 0x8009E840u,
        0x8009E844u, 0x8009E848u, 0x8009E84Cu, 0x8009E870u,
        0x8009E850u, 0x8009E854u, 0x8009E858u, 0x8009E85Cu, 0x8009E860u
    };
    uint32 values[13], index;
    FUNCTION_MARKER(0x800469E0u, "LEVEL.BIN");
    for (index = 0u; index < 13u; ++index) values[index] = r_u32(sources[index]);
    for (index = 0u; index < 13u; ++index) w_u32(0x8009367Cu + index * 0x44u, values[index]);
}

void level_sub_800758B4(uint32 controller, uint32 slot, uint32 scale, uint32 mode)
{
    uint32 record;
    FUNCTION_MARKER(0x800758B4u, "LEVEL.BIN");
    record = 0x800B75F0u + controller * 0xD0u + slot * 0x34u;
    if (mode == 2u) scale = r_u32(record + 0x18u);
    else if (mode == 3u) scale *= r_u32(record + 0x18u);
    else if (mode == 4u) scale = (uint32)((sint32)r_u32(record + 0x18u) >> (scale & 31u));
    else if (mode == 5u) scale = r_u32(record + 0x18u) << (scale & 31u);
    if (r_u32(record + 0x20u))
    {
        w_u32(record + 0x10u, scale);
        w_u32(record + 0x14u, 0u - scale);
    }
    else
    {
        w_u32(record + 0x10u, 0u - scale);
        w_u32(record + 0x14u, scale);
    }
    if (!mode) w_u32(record + 0x18u, scale);
    w_u32(record + 0x1Cu, scale);
}

void level_sub_80075810(uint32 controller, uint32 slot, uint32 negative_button,
    uint32 positive_button, uint32 modifier, uint32 mode, uint32 scale,
    uint32 invert, uint32 enabled)
{
    uint32 record;
    FUNCTION_MARKER(0x80075810u, "LEVEL.BIN");
    record = 0x800B75F0u + controller * 0xD0u + slot * 0x34u;
    w_u32(record + 4u, positive_button);
    w_u32(record, negative_button);
    w_u32(record + 8u, modifier);
    w_u32(record + 12u, mode);
    w_u32(record + 32u, invert);
    moh_draft_missing_call(0x800758B4u, controller, slot, scale, 0u);
    if (enabled)
    {
        w_u32(record + 0x28u, 0x200000u);
        w_u32(record + 0x2Cu, 0u);
        w_u32(record + 0x30u, 2u);
    }
}

uint32 level_sub_80066894(void)
{
    uint32 node;
    FUNCTION_MARKER(0x80066894u, "LEVEL.BIN");
    node = r_u32(0x8009D5B4u);
    if (!node)
    {
        moh_draft_missing_call(0x80015B28u, 0x8003CA0Cu);
        return 0u;
    }
    w_u32(0x8009D5B4u, r_u32(node + 32u));
    level_sub_8006D84C(node, 0u);
    w_u32(node + 4u, 0u);
    w_u32(node + 8u, 0u);
    level_sub_8006D84C(node + 12u, 0u);
    w_u32(node + 0x10u, 0u);
    w_u32(node + 0x14u, 0xFFFFFFFFu);
    w_u32(node + 0x18u, 0u);
    w_u32(node + 0x1Cu, 0xFFFFFFFFu);
    w_u32(node + 0x20u, 0u);
    return node;
}

void level_sub_80069514(uint32 packet_view, uint32 script, uint32 selected)
{
    uint32 top, stack, filter, record_id, first = 0u, second = 0u;
    uint32 id = 0xFFFFFFFFu, record, actor, handle;
    FUNCTION_MARKER(0x80069514u, "LEVEL.BIN");
    top = r_u32(0x8009D5A8u);
    stack = top - 2u;
    w_u32(0x8009D5A8u, top - 1u);
    w_u32(0x8009D5A8u, stack);
    filter = r_u32(0x800AD264u + (top - 1u) * 8u);
    record_id = r_u32(0x800AD264u + top * 8u);
    switch (filter)
    {
    case 0: break;
    case 1:
    case 2:
        stack = r_u32(0x8009D5A8u);
        id = r_u32(0x800AD264u + stack * 8u);
        w_u32(0x8009D5A8u, stack - 1u);
        break;
    case 3:
    case 4:
        stack = r_u32(0x8009D5A8u);
        second = r_u32(0x800AD264u + stack * 8u);
        first = r_u32(0x800AD264u + (stack - 1u) * 8u);
        w_u32(0x8009D5A8u, stack - 1u);
        w_u32(0x8009D5A8u, stack - 2u);
        break;
    case 5:
        stack = r_u32(0x8009D5A8u);
        second = r_u32(0x800AD264u + stack * 8u);
        first = r_u32(0x800AD264u + (stack - 1u) * 8u);
        id = r_u32(0x800AD264u + (stack - 2u) * 8u);
        w_u32(0x8009D5A8u, stack - 1u);
        w_u32(0x8009D5A8u, stack - 2u);
        w_u32(0x8009D5A8u, stack - 3u);
        break;
    default:
        moh_draft_missing_call(0x80015B28u, 0x8003CE10u, filter);
        return;
    }
    record = level_sub_800694C8(record_id, script);
    if (!record) return;
    handle = r_u32(packet_view);
    actor = handle ? r_u32(0x8009D678u + (handle & 0xFFFFu) * 4u) : 0u;
    moh_draft_missing_call(0x8006692Cu, (uint32)r_u16(record), actor,
        r_u32(packet_view + 12u), filter, first, second, id,
        script + (uint32)r_u16(record + 2u), selected);
}

uint32 level_sub_80066000(uint32 object, uint32 trigger, uint32 offset)
{
    uint32 base;
    FUNCTION_MARKER(0x80066000u, "LEVEL.BIN");
    if (trigger) base = r_u32(trigger + 4u) + 32u;
    else
    {
        if (!object) return 0u;
        base = object + 32u;
    }
    return base + offset;
}

uint32 level_sub_800694C8(uint32 index, uint32 table)
{
    FUNCTION_MARKER(0x800694C8u, "LEVEL.BIN");
    if (index < r_u32(table)) return table + 4u + index * 4u;
    moh_draft_missing_call(0x80015B28u, 0x8003CDD0u, index);
    return 0u;
}

uint32 level_sub_8006602C(uint32 object, uint32 trigger)
{
    uint32 data;
    sint32 type;
    FUNCTION_MARKER(0x8006602Cu, "LEVEL.BIN");
    if (trigger) return r_u32(r_u32(trigger + 4u) + 0x54u);
    if (!object) return 0xFFFFFFFFu;
    if (moh_draft_missing_call(0x8006D8ACu, object)) return 0xFFFFFFFFu;
    data = r_u32(r_u32(object + 0x1Cu) + 4u);
    type = (sint32)r_u32(data + 12u);
    if (type == 3) return r_u32(data + 0x78u);
    if (type < 4)
    {
        if (type != 2) return 0xFFFFFFFFu;
    }
    else if (type != 4 && type != 6) return 0xFFFFFFFFu;
    return r_u32(data + 0x64u);
}

void level_sub_8008DAA8(uint32 instance, uint32 value)
{
    FUNCTION_MARKER(0x8008DAA8u, "LEVEL.BIN");
    if (instance) w_u32(instance + 0x68u, value);
}

uint32 level_sub_80061168(uint32 type, uint32 resource)
{
    uint32 data;
    FUNCTION_MARKER(0x80061168u, "LEVEL.BIN");
    data = resource ? r_u32(resource + 4u) : 0u;
    if (type == 1u)
        return moh_draft_missing_call(0x8007BD68u, resource, data);
    if (type == 0x4Cu)
        return moh_draft_missing_call(0x80061244u, resource, data);
    return 0u;
}

uint32 level_sub_80059C88(uint32 object, uint32 instruction, uint32 flags)
{
    uint32 left, right, block, base, yes_offset, no_offset, offset;
    sint32 opcode, index;
    int condition;
    FUNCTION_MARKER(0x80059C88u, "LEVEL.BIN");
    left = level_sub_8005D148(1u, flags, r_u32(instruction + 4u), object);
    opcode = (sint16)r_u16(instruction);
    right = level_sub_8005D148(3u, flags, r_u32(instruction + 8u), object);
    block = r_u32(0x80099EC4u);
    yes_offset = r_u16(instruction + 12u);
    index = (sint16)r_u16(block + 0x1Eu);
    no_offset = r_u16(instruction + 14u);
    if (!index) base = r_u32(r_u32(block + 0x10u) + 12u);
    else base = r_u32(block + ((uint32)index - 1u) * 16u + 0x2Cu);
    switch (opcode)
    {
    case 1: condition = left == right; break;
    case 2: condition = (sint32)right < (sint32)left; break;
    case 3: condition = (sint32)left < (sint32)right; break;
    case 4: condition = left != right; break;
    default: return instruction;
    }
    offset = (uint32)(sint32)(sint16)(condition ? yes_offset : no_offset);
    return base + offset * 4u;
}

uint32 level_sub_8005D148(uint32 position, uint32 flags, uint32 value, uint32 object)
{
    FUNCTION_MARKER(0x8005D148u, "LEVEL.BIN");
    if (flags & (1u << ((position - 1u) & 31u)))
        return moh_draft_missing_call(0x8005CBA8u, value, object);
    return value;
}

uint32 level_sub_8005D18C(uint32 cursor, uint32 object, uint32 block,
    uint32 frame, uint32 mode, uint32 layer)
{
    uint32 animation, instruction, word, handler, next;
    FUNCTION_MARKER(0x8005D18Cu, "LEVEL.BIN");
    w_u32(0x80099EB4u, frame);
    w_u32(0x80099EC4u, block);
    w_u32(0x80099EC0u, 0u);
    w_u32(0x80099EB0u, mode);
    animation = r_u32(block + 0x10u);
    w_u32(0x80099EB8u, layer);
    w_u32(0x80099EBCu, animation);
    instruction = r_u32(cursor);
    word = r_u32(instruction);
    handler = r_u32(0x80099B38u + (word >> 24) * 12u);
    next = moh_draft_indirect_call(handler, object, instruction, (word >> 16) & 0xFFu);
    w_u32(cursor, next);
    return r_u32(0x80099EC0u);
}

uint32 level_sub_8008CDF4(uint32 state)
{
    FUNCTION_MARKER(0x8008CDF4u, "LEVEL.BIN");
    w_u32(state + 0x18u, 0u);
    return state;
}

uint32 level_sub_8008CFE8(uint32 state)
{
    uint32 index;
    FUNCTION_MARKER(0x8008CFE8u, "LEVEL.BIN");
    w_u32(state + 0x3Cu, 0u);
    w_u32(state + 0x40u, 0u);
    for (index = 0u; index < 6u; ++index)
        moh_draft_missing_call(0x8008CDF4u, state + 0x44u + index * 0x1Cu);
    w_u32(state + 0xECu, 0u);
    return state;
}

uint32 level_sub_8006A910(uint32 identity)
{
    uint32 count, index, record;
    FUNCTION_MARKER(0x8006A910u, "LEVEL.BIN");
    if (identity == 0xFFFFFFFFu) return 0xFFFFFFFFu;
    count = r_u32(0x8009EC54u);
    if (!count) return 0xFFFFFFFFu;
    record = r_u32(0x8009EC58u);
    for (index = 0u; index < count; ++index, record += 0x100u)
        if (r_u32(record) == identity) return index;
    return 0xFFFFFFFFu;
}

uint32 level_sub_80074BB8(uint32 type, uint32 resource)
{
    uint32 scene;
    FUNCTION_MARKER(0x80074BB8u, "LEVEL.BIN");
    if (r_u32(resource) & 1u) return 0u;
    scene = moh_draft_missing_call(0x8006D574u, type, resource);
    if (!scene) return 0u;
    level_sub_8006B834(scene);
    w_u32(resource, r_u32(resource) | 1u);
    w_u32(scene + 0x1Cu, resource);
    if (r_u32(r_u32(resource + 4u) + 12u))
        moh_draft_missing_call(0x800665BCu, scene, 0u);
    moh_draft_missing_call(0x80074AD8u, scene, resource);
    return scene;
}

void level_sub_800425C4(uint32 voice)
{
    uint32 record;
    FUNCTION_MARKER(0x800425C4u, "LEVEL.BIN");
    if ((sint32)voice <= 0) return;
    moh_draft_missing_call(0x80015C08u, voice, 0u, 0u);
    moh_draft_missing_call(0x800157BCu, voice, 0u);
    moh_draft_missing_call(0x800158DCu, 0u, 1u << (voice & 31u));
    record = 0x8009115Cu + voice * 52u;
    w_u8(0x800910D0u + voice, 0u);
    w_u8(record + 0x12u, 0u);
    w_u8(record + 0x0Eu, 0u);
    level_sub_8006D84C(record + 4u, 0u);
    w_u16(record + 0x28u, 0u);
}

void level_sub_80087080(uint32 material, uint32 bsd, uint32 base)
{
    static const uint32 material_fields[3] = {0x38u, 0x84u, 0x9Cu};
    static const uint32 bsd_fields[3] = {0x564u, 0x574u, 0x55Cu};
    static const uint32 leading_fields[4] = {0xB4u, 0x88u, 0x90u, 0xC4u};
    static const uint32 trailing_fields[5] = {0x24u, 0x2Cu, 0x30u, 0x34u, 0xC0u};
    uint32 value, list, index, slot, field;
    FUNCTION_MARKER(0x80087080u, "LEVEL.BIN");
    value = r_u32(material + 8u);
    if (value == 0xFFFFFFFFu) w_u32(material + 8u, 0u);
    else
    {
        list = base + value;
        w_u32(material + 8u, list);
        index = 0u;
        while (index < r_u16(list))
        {
            slot = list + 4u + index * 4u;
            value = r_u32(slot);
            w_u32(slot, value == 0xFFFFFFFFu ? 0u : value + r_u32(bsd + 0x544u));
            index = (index + 1u) & 0xFFFFu;
        }
    }
    for (index = 0u; index < 3u; ++index)
    {
        value = r_u32(material + material_fields[index]);
        w_u32(material + material_fields[index],
            value == 0xFFFFFFFFu ? 0u : value + r_u32(bsd + bsd_fields[index]));
    }
    for (index = 0u; index < 4u; ++index)
    {
        field = material + leading_fields[index];
        value = r_u32(field);
        if (value) w_u32(field, base + value);
    }
    value = r_u32(material + 4u);
    if (value)
    {
        w_u32(material + 4u, base + value);
        moh_draft_missing_call(0x80086798u, material);
    }
    value = r_u32(material + 12u);
    if (value)
    {
        w_u32(material + 12u, base + value);
        moh_draft_missing_call(0x8003F788u, material);
    }
    for (index = 0u; index < 5u; ++index)
    {
        field = material + trailing_fields[index];
        value = r_u32(field);
        if (value) w_u32(field, base + value);
    }
}
