#include "psx.h"
#include "game_shell_signatures.h"
#include "xport_trace.h"

void shell_sub_80053798(uint32 source, uint32 destination);
void shell_sub_800538C4(uint32 source, uint32 destination);
void shell_sub_800539F4(uint32 parameters, uint32 first, uint32 second);

uint32 shell_sub_80053418(uint32 model)
{
    uint32 counter, first, second, index, axis, words[4];
    FUNCTION_MARKER(0x80053418u, "SHELL.BIN");
    if (!model) return (uint32)-2;
    if ((sint32)r_u32(model + 0x380u) <= 0) return (uint32)-8;
    if (!r_u8(model + 0x2E8u)) return 2u;
    if (!r_u32(model + 0x2ECu)) return 3u;
    counter = r_u32(model + 0x4CCu);
    if ((sint32)counter < 0) return (uint32)-8;
    if (!counter) {
        if (r_u32(model + 0x2ECu) != 2u) return 0u;
        first = model + (r_u8(model + 0x4D0u) ? 0x338u : 0x2F0u);
        second = model + (r_u8(model + 0x4D0u) ? 0x2F0u : 0x338u);
        shell_sub_800539F4(model + 0x2E8u, first, second);
        shell_sub_80053798(first, model + 0x3CCu);
        w_u8(model + 0x4D0u, r_u8(model + 0x4D0u) == 0u);
    }
    shell_sub_800538C4(model + 0x3CCu, model + 0x384u);
    for (index = 0u; index < 16u; index += 4u) {
        for (axis = 0u; axis < 4u; ++axis)
            words[axis] = r_u32(model + 0x384u + (index + axis) * 4u);
        for (axis = 0u; axis < 4u; ++axis)
            w_u32(model + 0x2A4u + (index + axis) * 4u, words[axis]);
    }
    w_u32(model + 0x2E4u, r_u32(model + 0x3C4u));
    w_u32(model + 0x18u, r_u32(model + 0x3C8u));
    counter = r_u32(model + 0x4CCu) + 1u;
    w_u32(model + 0x4CCu, counter);
    if (counter == r_u32(model + 0x380u)) w_u32(model + 0x4CCu, 0u);
    for (index = 0u; index < 3u; ++index) {
        for (axis = 0u; axis < 3u; ++axis) {
            first = model + 0x3CCu + index * 16u + axis * 4u;
            second = model + 0x44Cu + index * 16u + axis * 4u;
            w_u32(first, r_u32(first) + r_u32(second));
        }
        for (axis = 0u; axis < 3u; ++axis) {
            first = model + 0x3FCu + index * 16u + axis * 4u;
            second = model + 0x47Cu + index * 16u + axis * 4u;
            w_u32(first, r_u32(first) + r_u32(second));
        }
    }
    for (index = 0u; index < 2u; ++index) {
        for (axis = 0u; axis < 3u; ++axis) {
            first = model + 0x42Cu + index * 16u + axis * 4u;
            second = model + 0x4ACu + index * 16u + axis * 4u;
            w_u32(first, r_u32(first) + r_u32(second));
        }
    }
    return 0u;
}

void shell_sub_80053798(uint32 source, uint32 destination)
{
    uint32 row, axis;
    FUNCTION_MARKER(0x80053798u, "SHELL.BIN");
    for (row = 0u; row < 3u; ++row) {
        for (axis = 0u; axis < 3u; ++axis)
            w_u32(destination + row * 16u + axis * 4u,
                (uint32)(sint32)(sint16)r_u16(source + row * 6u + axis * 2u) << 4);
        for (axis = 0u; axis < 3u; ++axis)
            w_u32(destination + 0x30u + row * 16u + axis * 4u,
                (uint32)(sint32)(sint16)r_u16(source + 0x20u + row * 2u + axis * 6u) << 4);
    }
    for (row = 0u; row < 2u; ++row)
        for (axis = 0u; axis < 3u; ++axis)
            w_u32(destination + 0x60u + row * 16u + axis * 4u,
                (uint32)r_u8(source + 0x40u + axis) << 16);
}

void shell_sub_800538C4(uint32 source, uint32 destination)
{
    uint32 row, axis;
    FUNCTION_MARKER(0x800538C4u, "SHELL.BIN");
    for (row = 0u; row < 3u; ++row) {
        for (axis = 0u; axis < 3u; ++axis)
            w_u16(destination + row * 6u + axis * 2u,
                (uint16)((sint32)r_u32(source + row * 16u + axis * 4u) >> 4));
        for (axis = 0u; axis < 3u; ++axis)
            w_u16(destination + 0x20u + row * 2u + axis * 6u,
                (uint16)((sint32)r_u32(source + 0x30u + row * 16u + axis * 4u) >> 4));
    }
    for (row = 0u; row < 2u; ++row)
        for (axis = 0u; axis < 3u; ++axis)
            w_u8(destination + 0x40u + row * 4u + axis,
                (uint8)r_u16(source + 0x62u + axis * 4u));
}

void shell_sub_80063CF4(const uint32 *first, const uint32 *second,
    sint32 duration, uint32 *output)
{
    uint32 start[3], end[3], axis, divisor, delta;
    FUNCTION_MARKER(0x80063CF4u, "SHELL.BIN");
    for (axis = 0u; axis < 3u; ++axis)
        start[axis] = moh_draft_missing_call(0x8001D590u, first[axis]);
    for (axis = 0u; axis < 3u; ++axis)
        end[axis] = moh_draft_missing_call(0x8001D590u, second[axis]);
    if (duration < 1) {
        for (axis = 0u; axis < 3u; ++axis) output[axis] = 0u;
        return;
    }
    delta = moh_draft_missing_call(0x8001D3F0u, end[0], start[0]);
    divisor = moh_draft_missing_call(0x8001D590u, (uint32)duration);
    delta = moh_draft_missing_call(0x8001D1F0u, delta, divisor);
    output[0] = moh_draft_missing_call(0x8001E11Cu, delta);
    for (axis = 1u; axis < 3u; ++axis) {
        delta = moh_draft_missing_call(0x8001D3F0u, end[axis], start[axis]);
        delta = moh_draft_missing_call(0x8001D1F0u, delta, divisor);
        output[axis] = moh_draft_missing_call(0x8001E11Cu, delta);
    }
}

void shell_sub_800539F4(uint32 parameters, uint32 first, uint32 second)
{
    uint32 row, axis, start[3], end[3], output[4], destination;
    sint32 duration = (sint32)r_u32(parameters + 0x98u);
    FUNCTION_MARKER(0x800539F4u, "SHELL.BIN");
    for (row = 0u; row < 3u; ++row) {
        for (axis = 0u; axis < 3u; ++axis) {
            start[axis] = (uint32)(sint32)(sint16)r_u16(first + row * 6u + axis * 2u) << 4;
            end[axis] = (uint32)(sint32)(sint16)r_u16(second + row * 6u + axis * 2u) << 4;
        }
        destination = parameters + 0x164u + row * 16u;
        /* TODO: Original copies an uninitialized local padding word */
        output[3] = r_u32(destination + 12u);
        shell_sub_80063CF4(start, end, duration, output);
        for (axis = 0u; axis < 4u; ++axis) w_u32(destination + axis * 4u, output[axis]);
        for (axis = 0u; axis < 3u; ++axis) {
            start[axis] = (uint32)(sint32)(sint16)r_u16(first + 0x20u + row * 2u + axis * 6u) << 4;
            end[axis] = (uint32)(sint32)(sint16)r_u16(second + 0x20u + row * 2u + axis * 6u) << 4;
        }
        destination = parameters + 0x194u + row * 16u;
        output[3] = r_u32(destination + 12u);
        shell_sub_80063CF4(start, end, duration, output);
        for (axis = 0u; axis < 4u; ++axis) w_u32(destination + axis * 4u, output[axis]);
    }
    for (row = 0u; row < 2u; ++row) {
        for (axis = 0u; axis < 3u; ++axis) {
            start[axis] = (uint32)r_u8(first + 0x40u + axis) << 16;
            end[axis] = (uint32)r_u8(second + 0x40u + axis) << 16;
        }
        destination = parameters + 0x1C4u + row * 16u;
        output[3] = r_u32(destination + 12u);
        shell_sub_80063CF4(start, end, duration, output);
        for (axis = 0u; axis < 4u; ++axis) w_u32(destination + axis * 4u, output[axis]);
    }
}

uint32 shell_sub_8005A430(uint32 screen)
{
    uint32 data, first_button, second_button = 0u, pressed, bank, voice;
    FUNCTION_MARKER(0x8005A430u, "SHELL.BIN");
    data = r_u32(screen + 80u);
    if (!r_u8(0x8007BA88u)) return 0u;
    if (r_u8(0x8007BBC4u)) return 0u;
    if ((sint32)r_u32(0x8007BBD0u) <= 0) {
        w_u8(0x8007BBB1u, 1u);
        return 10u;
    }
    w_u32(0x8007BBD0u, r_u32(0x8007BBD0u) - 1u);
    first_button = moh_draft_missing_call(0x8005F258u, 0x16u) & 255u;
    pressed = first_button;
    if (!pressed) {
        second_button = moh_draft_missing_call(0x8005F258u, 0x18u) & 255u;
        pressed = second_button;
    }
    if (!pressed) pressed = moh_draft_missing_call(0x8005F258u, 0x1Eu) & 255u;
    if (!pressed) pressed = moh_draft_missing_call(0x8005F258u, 0x20u) & 255u;
    if (pressed) {
        w_u32(0x8007BBD0u, 0x3C0u);
        if (first_button || second_button || r_u32(r_u32(screen + 28u) + 56u) >= 2u)
            return 2u;
        bank = r_u32(data);
        if (!bank) {
            w_u32(0x8007C218u, (uint32)-2);
            return 0u;
        }
        voice = shell_sub_8006349C();
        w_u32(0x8007C374u, voice);
        w_u32(r_u32(bank + 12u) + 92u, voice);
        shell_sub_80063510((uint32)(sint32)(sint16)r_u16(bank), 23,
            voice, r_u8(bank + 16u), r_u8(bank + 17u), r_u32(bank + 4u), r_u32(bank + 8u));
        return 0u;
    }
    pressed = moh_draft_missing_call(0x8005F258u, 2u) & 255u;
    if (!pressed) pressed = moh_draft_missing_call(0x8005F258u, 14u) & 255u;
    if (pressed) {
        w_u32(0x8007BBD0u, 0x3C0u);
        bank = r_u32(data);
        if (!bank) w_u32(0x8007C218u, (uint32)-2);
        else {
            voice = shell_sub_8006349C();
            w_u32(0x8007C374u, voice);
            w_u32(r_u32(bank + 12u) + 20u, voice);
            shell_sub_80063510((uint32)(sint32)(sint16)r_u16(bank), 5,
                voice, r_u8(bank + 16u), r_u8(bank + 17u), r_u32(bank + 4u), r_u32(bank + 8u));
        }
        return 3u;
    }
    pressed = moh_draft_missing_call(0x8005F258u, 3u) & 255u;
    if (!pressed) pressed = moh_draft_missing_call(0x8005F258u, 15u) & 255u;
    if (pressed) {
        w_u32(0x8007BBD0u, 0x3C0u);
        bank = r_u32(data);
        if (!bank) w_u32(0x8007C218u, (uint32)-2);
        else {
            voice = shell_sub_8006349C();
            w_u32(0x8007C374u, voice);
            w_u32(r_u32(bank + 12u) + 20u, voice);
            shell_sub_80063510((uint32)(sint32)(sint16)r_u16(bank), 5,
                voice, r_u8(bank + 16u), r_u8(bank + 17u), r_u32(bank + 4u), r_u32(bank + 8u));
        }
        return 4u;
    }
    return 0u;
}

uint32 shell_sub_8005F258(uint32 button)
{
    uint32 offset = (uint32)button * 4u, alias;
    FUNCTION_MARKER(0x8005F258u, "SHELL.BIN");
    if (r_u8(0x8007BD66u + offset) && (uint32)button - 18u >= 8u &&
        !r_u8(0x8007BD67u + offset)) {
        w_u8(0x8007BD67u + offset, 1u);
        if ((sint32)button < 18) {
            w_u8(0x8007BCDFu + offset, 1u);
            return 1u;
        }
        if ((sint32)button < 26) return 1u;
        alias = r_u32(0x8007BCA8u + ((uint32)button - 26u) * 4u);
        if (alias == 0xFFFFFFFFu) return 1u;
        w_u8(0x8007BCDFu + alias * 4u, 1u);
        w_u8(0x8007BD67u + alias * 4u, 1u);
        return 1u;
    }
    if (!r_u8(0x8007BCDEu + offset) || (uint32)button - 26u < 8u ||
        r_u8(0x8007BCDFu + offset)) return 0u;
    w_u8(0x8007BCDFu + offset, 1u);
    if ((sint32)button < 18) {
        w_u8(0x8007BD67u + offset, 1u);
        return 1u;
    }
    if ((sint32)button >= 26) return 1u;
    alias = r_u32(0x8007BC88u + ((uint32)button - 18u) * 4u);
    if (alias == 0xFFFFFFFFu) return 1u;
    w_u8(0x8007BCDFu + alias * 4u, 1u);
    w_u8(0x8007BD67u + alias * 4u, 1u);
    return 1u;
}



uint32 shell_sub_8003CA30(uint32 animation)
{
    uint32 model, frame, next, sequence, index, axis, words[4];
    uint32 source, target, accumulator, step, count, shift, values[3];
    sint32 selected;
    FUNCTION_MARKER(0x8003CA30u, "SHELL.BIN");
    if (!animation) return (uint32)-2;
    model = r_u32(animation + 84u);
    if (!model) return (uint32)-5;
    if (!r_u8(animation + 88u)) return (uint32)-7;
    frame = r_u32(animation + 72u);
    w_u32(frame + 836u, 0u);
    frame = r_u32(animation + 72u);
    for (index = 0u; index < 64u; index += 4u) {
        for (axis = 0u; axis < 4u; ++axis)
            words[axis] = r_u32(frame + 24u + (index + axis) * 4u);
        for (axis = 0u; axis < 4u; ++axis)
            w_u32(frame + 292u + (index + axis) * 4u, words[axis]);
    }
    for (axis = 0u; axis < 3u; ++axis) words[axis] = r_u32(frame + 280u + axis * 4u);
    for (axis = 0u; axis < 3u; ++axis) w_u32(frame + 548u + axis * 4u, words[axis]);
    next = r_u32(r_u32(animation + 72u) + 4u);
    w_u32(animation + 72u, next);
    if (!next) w_u32(animation + 72u, r_u32(animation + 64u));
    frame = r_u32(animation + 72u);
    selected = (sint32)r_u32(frame + 20u);
    if (selected <= (sint32)(r_u32(model + 12u) - 1u)) {
        sequence = r_u32(model);
        for (index = 0u; (sint32)index < selected; ++index)
            sequence = r_u32(sequence + 4u);
        w_u32(model + 8u, sequence);
        w_u8(model + 16u, 0u);
        w_u32(model + 1252u, r_u32(model + 1248u));
        w_u32(sequence + 20u, r_u32(sequence + 12u));
    }
    frame = r_u32(animation + 72u);
    if (r_u32(frame + 16u) != 1u) return 0u;
    next = r_u32(frame + 4u);
    source = r_u32(model + 1268u);
    if (!next) next = r_u32(animation + 64u);
    sequence = r_u32(r_u32(animation + 84u));
    selected = (sint32)r_u32(next + 20u);
    while (sequence && r_u32(sequence + 8u) != (uint32)selected)
        sequence = r_u32(sequence + 4u);
    accumulator = r_u32(animation + 104u);
    target = r_u32(r_u32(sequence + 12u) + 12u);
    step = r_u32(animation + 108u);
    count = r_u32(model + 1264u);
    shift = r_u32(r_u32(animation + 72u) + 844u) & 31u;
    for (index = 0u; (sint32)index < (sint32)count; ++index) {
        for (axis = 0u; axis < 3u; ++axis)
            w_u32(accumulator + index * 16u + axis * 4u,
                (uint32)r_u16(source + index * 8u + axis * 2u) << 16);
        for (axis = 0u; axis < 3u; ++axis)
            values[axis] = (uint32)r_u16(target + index * 8u + axis * 2u) << 16;
        for (axis = 0u; axis < 3u; ++axis)
            w_u32(step + index * 16u + axis * 4u,
                (uint32)((sint32)(values[axis] - r_u32(accumulator + index * 16u + axis * 4u)) >> shift));
    }
    moh_draft_missing_call(0x800154ECu, r_u32(animation + 100u), source,
        r_u32(model + 1264u) << 3);
    return 0u;
}

uint32 shell_sub_8004C328(uint32 screen)
{
    uint32 group, image, widget, texture, model;
    FUNCTION_MARKER(0x8004C328u, "SHELL.BIN");
    if (!screen) return (uint32)-2;
    for (group = r_u32(screen + 32u); group; group = r_u32(group + 180u)) {
        image = r_u32(group + 112u);
        if (image && r_u32(image + 28u)) {
            if (shell_sub_8003D890(r_u32(image + 28u)) < 0) return (uint32)-4;
            w_u32(image + 28u, 0u);
        }
        widget = r_u32(group + 136u);
        if (widget) {
            model = r_u32(widget + 64u);
            if (model) {
                if ((sint32)moh_draft_missing_call(0x80053358u, model) < 0) return (uint32)-4;
                w_u32(r_u32(group + 136u) + 64u, 0u);
            }
            widget = r_u32(group + 136u);
            texture = r_u32(widget + 56u);
            if (texture) {
                if (shell_sub_8003D890(texture) < 0) return (uint32)-4;
                w_u32(r_u32(group + 136u) + 56u, 0u);
                widget = r_u32(group + 136u);
            }
            texture = r_u32(widget + 60u);
            if (texture) {
                if (shell_sub_8003D890(texture) < 0) return (uint32)-4;
                w_u32(r_u32(group + 136u) + 60u, 0u);
            }
        }
        for (image = r_u32(group + 120u); image; image = r_u32(image + 208u)) {
            texture = r_u32(image + 28u);
            if (texture) {
                if (shell_sub_8003D890(texture) < 0) return (uint32)-4;
                w_u32(image + 28u, 0u);
            }
        }
        for (widget = r_u32(group + 144u); widget; widget = r_u32(widget + 312u)) {
            model = r_u32(widget + 64u);
            if (model) {
                if ((sint32)moh_draft_missing_call(0x80053358u, model) < 0) return (uint32)-4;
                w_u32(widget + 64u, 0u);
            }
            texture = r_u32(widget + 56u);
            if (texture) {
                if (shell_sub_8003D890(texture) < 0) return (uint32)-4;
                w_u32(widget + 56u, 0u);
            }
            texture = r_u32(widget + 60u);
            if (texture) {
                if (shell_sub_8003D890(texture) < 0) return (uint32)-4;
                w_u32(widget + 60u, 0u);
            }
        }
    }
    return 0u;
}

uint32 shell_sub_80053314(uint32 model);

uint32 shell_sub_80053358(uint32 model)
{
    static const uint32 fields[8] = {36u, 44u, 52u, 60u, 68u, 132u, 116u, 120u};
    uint32 index;
    FUNCTION_MARKER(0x80053358u, "SHELL.BIN");
    if ((sint32)shell_sub_80053314(model) < 0) return (uint32)-4;
    for (index = 0u; index < 8u; ++index)
        shell_sub_80045328(r_u32(model + fields[index]));
    if (!r_u8(model + 17u)) {
        shell_sub_80045328(r_u32(model + 104u));
        shell_sub_80045328(r_u32(model + 108u));
    }
    shell_sub_80045328(model);
    return 0u;
}

uint32 shell_sub_80053070(uint32 frame)
{
    uint32 sequence, next, previous;
    FUNCTION_MARKER(0x80053070u, "SHELL.BIN");
    next = r_u32(frame + 4u);
    sequence = r_u32(frame + 16u);
    if (next) {
        w_u32(next, r_u32(frame));
        previous = r_u32(frame);
        if (!previous) goto update_endpoints;
        w_u32(previous + 4u, r_u32(frame + 4u));
    }
    previous = r_u32(frame);
    if (previous) {
        w_u32(previous + 4u, r_u32(frame + 4u));
        next = r_u32(frame + 4u);
        if (next) w_u32(next, r_u32(frame));
    }
update_endpoints:
    if (r_u32(sequence + 12u) == frame) w_u32(sequence + 12u, r_u32(frame + 4u));
    if (r_u32(sequence + 16u) == frame) w_u32(sequence + 16u, r_u32(frame));
    shell_sub_80045328(frame);
    w_u32(sequence + 24u, r_u32(sequence + 24u) - 1u);
    return 0u;
}

uint32 shell_sub_800532D0(uint32 sequence)
{
    uint32 frame;
    FUNCTION_MARKER(0x800532D0u, "SHELL.BIN");
    while ((frame = r_u32(sequence + 12u)) != 0u)
        if ((sint32)shell_sub_80053070(frame) < 0) return (uint32)-4;
    return 0u;
}

uint32 shell_sub_800531D8(uint32 sequence)
{
    uint32 model, next, previous;
    FUNCTION_MARKER(0x800531D8u, "SHELL.BIN");
    model = r_u32(sequence + 28u);
    shell_sub_800532D0(sequence);
    next = r_u32(sequence + 4u);
    if (next) {
        w_u32(next, r_u32(sequence));
        previous = r_u32(sequence);
        if (!previous) goto update_endpoints;
        w_u32(previous + 4u, r_u32(sequence + 4u));
    }
    previous = r_u32(sequence);
    if (previous) {
        w_u32(previous + 4u, r_u32(sequence + 4u));
        next = r_u32(sequence + 4u);
        if (next) w_u32(next, r_u32(sequence));
    }
update_endpoints:
    if (r_u32(model) == sequence) w_u32(model, r_u32(sequence + 4u));
    if (r_u32(model + 4u) == sequence) w_u32(model + 4u, r_u32(sequence));
    shell_sub_80045328(sequence);
    w_u32(model + 12u, r_u32(model + 12u) - 1u);
    return 0u;
}

uint32 shell_sub_80053314(uint32 model)
{
    uint32 sequence;
    FUNCTION_MARKER(0x80053314u, "SHELL.BIN");
    while ((sequence = r_u32(model)) != 0u)
        if ((sint32)shell_sub_800531D8(sequence) < 0) return (uint32)-4;
    return 0u;
}

void shell_sub_8003F014(uint32 screen)
{
    FUNCTION_MARKER(0x8003F014u, "SHELL.BIN");
    shell_sub_8004E16C(r_u32(screen + 80u));
}

static void brief_advance_group(uint32 screen, uint32 link, uint32 endpoint)
{
    uint32 group, index, selection;
    if (!screen || !r_u32(screen + endpoint)) return;
    group = r_u32(screen + 28u);
    if ((sint32)r_u32(screen + 40u) <= 0) return;
    for (index = 0u; ; ++index) {
        group = r_u32(group + link);
        if (!group) group = r_u32(screen + endpoint);
        if (!r_u8(group + 81u)) {
            w_u32(screen + 28u, group);
            selection = r_u32(screen + 44u);
            if (selection) w_u32(selection, r_u32(group + 56u));
            return;
        }
        if ((sint32)(index + 1u) >= (sint32)r_u32(screen + 40u)) return;
    }
}

static void brief_group_sound(uint32 data)
{
    uint32 bank = r_u32(data), voice;
    if (!bank) { w_u32(0x8007C218u, (uint32)-2); return; }
    voice = shell_sub_8006349C();
    w_u32(0x8007C374u, voice);
    w_u32(r_u32(bank + 12u) + 20u, voice);
    shell_sub_80063510((uint32)(sint32)(sint16)r_u16(bank), 5,
        voice, r_u8(bank + 16u), r_u8(bank + 17u), r_u32(bank + 4u), r_u32(bank + 8u));
}

void shell_sub_8003F6EC(uint32 screen, uint32 result)
{
    uint32 data, target, group;
    FUNCTION_MARKER(0x8003F6ECu, "SHELL.BIN");
    data = r_u32(screen + 80u);
    if (!r_u8(0x8007BB10u)) return;
    if (r_u32(0x8007BB14u) == 2u) {
        w_u8(screen + 117u, 1u);
        if (!r_u8(0x8007A250u)) {
            target = r_u32(r_u32(screen + 28u) + 60u);
            if (target) { w_u32(0x8007BA68u, target); return; }
        }
        w_u8(0x8007BFFDu, 1u);
        return;
    }
    if (result == 6u) { shell_sub_8004DE14(); return; }
    if (result == 3u) {
        group = r_u32(screen + 28u);
        if (!r_u32(group + 180u)) {
            w_u8(0x8007A250u, 1u);
            shell_sub_8004DE14();
            return;
        }
        brief_advance_group(screen, 180u, 32u);
    }
    if (result == 2u) {
        w_u8(0x8007A250u, 1u);
        shell_sub_8004DE14();
        return;
    }
    if (result == 4u && r_u32(r_u32(screen + 28u) + 180u)) {
        brief_group_sound(data);
        brief_advance_group(screen, 180u, 32u);
    }
    if (result != 5u || !r_u32(r_u32(screen + 28u) + 176u)) return;
    brief_group_sound(data);
    brief_advance_group(screen, 176u, 36u);
}

uint32 shell_sub_8005F5E4(uint32 resource)
{
    FUNCTION_MARKER(0x8005F5E4u, "SHELL.BIN");
    if (!resource) return (uint32)-2;
    shell_sub_80045328(resource);
    return 0u;
}

uint32 shell_sub_80049324(uint32 screen)
{
    static const uint32 list_fields[4] = {100u, 120u, 164u, 144u};
    static const uint32 destructors[4] = {0x80049A6Cu, 0x80049B54u, 0x80049DA8u, 0x80049C60u};
    uint32 callback, node, group, field, index, next, previous;
    FUNCTION_MARKER(0x80049324u, "SHELL.BIN");
    if (!screen) return (uint32)-2;
    callback = r_u32(screen + 88u);
    /* Unopened native screens own layout data but no initialized media */
    if (moh_native_screen_initialized(screen)) {
        if (callback) moh_draft_indirect_call(callback, screen);
    } else if (r_u32(screen + 80u)) shell_sub_80045328(r_u32(screen + 80u));
    if ((sint32)shell_sub_8004C328(screen) < 0) return (uint32)-4;
    while ((node = r_u32(screen + 32u)) != 0u)
        if ((sint32)moh_draft_missing_call(0x800496A0u, node) < 0) return (uint32)-4;
    while ((node = r_u32(screen + 52u)) != 0u)
        if ((sint32)moh_draft_missing_call(0x800498E8u, node) < 0) return (uint32)-4;
    for (field = 64u; field <= 68u; field += 4u) {
        group = r_u32(screen + field);
        if (!group) continue;
        for (index = 0u; index < 4u; ++index) {
            while ((node = r_u32(group + list_fields[index])) != 0u) {
                if ((sint32)moh_draft_missing_call(destructors[index], node) < 0) return (uint32)-4;
                group = r_u32(screen + field);
            }
            if (index == 1u) group = r_u32(screen + field);
        }
        shell_sub_80045328(r_u32(screen + field));
    }
    node = r_u32(screen + 72u);
    if (node) shell_sub_80045328(node);
    next = r_u32(screen + 128u);
    if (next) {
        w_u32(next + 124u, r_u32(screen + 124u));
        previous = r_u32(screen + 124u);
        if (!previous) goto update_screen_endpoints;
        w_u32(previous + 128u, r_u32(screen + 128u));
    }
    previous = r_u32(screen + 124u);
    if (previous) {
        w_u32(previous + 128u, r_u32(screen + 128u));
        next = r_u32(screen + 128u);
        if (next) w_u32(next + 124u, r_u32(screen + 124u));
    }
update_screen_endpoints:
    moh_native_forget_screen(screen);
    if (r_u32(0x8007BA6Cu) == screen) w_u32(0x8007BA6Cu, r_u32(screen + 128u));
    if (r_u32(0x8007BA70u) == screen) w_u32(0x8007BA70u, r_u32(screen + 124u));
    shell_sub_80045328(screen);
    w_u32(0x8007BA74u, r_u32(0x8007BA74u) - 1u);
    return 0u;
}

uint32 shell_sub_800492D0(void)
{
    uint32 screen;
    FUNCTION_MARKER(0x800492D0u, "SHELL.BIN");
    while ((screen = r_u32(0x8007BA6Cu)) != 0u)
        if ((sint32)shell_sub_80049324(screen) < 0) return (uint32)-4;
    return 0u;
}

void shell_sub_80045430(void)
{
    uint32 index;
    FUNCTION_MARKER(0x80045430u, "SHELL.BIN");
    for (index = 0u; (sint32)index < (sint32)r_u32(0x8007A5FCu); ++index) {
        shell_sub_80045328(r_u32(0x8007A608u + index * 4u));
        w_u32(0x8007A608u + index * 4u, 0u);
    }
    shell_sub_80045328(r_u32(0x8007B654u));
    w_u32(0x8007B654u, 0u);
    moh_draft_missing_call(0x800158ACu, 0x8007A3A0u, 0u, 0x12B8u);
    w_u8(0x8007B658u, 0u);
}

void shell_sub_800453F8(void)
{
    FUNCTION_MARKER(0x800453F8u, "SHELL.BIN");
    moh_draft_missing_call(0x8007699Cu);
    moh_draft_missing_call(0x80078048u);
    moh_draft_missing_call(0x80016E38u, 0u);
    shell_sub_80045430();
}

uint32 shell_sub_80049A6C(uint32 node)
{
    uint32 owner, next, previous;
    FUNCTION_MARKER(0x80049A6Cu, "SHELL.BIN");
    if (!node) return (uint32)-2;
    next = r_u32(node + 128u);
    owner = r_u32(node + 132u);
    if (next) {
        w_u32(next + 124u, r_u32(node + 124u));
        previous = r_u32(node + 124u);
        if (!previous) goto update_endpoints;
        w_u32(previous + 128u, r_u32(node + 128u));
    }
    previous = r_u32(node + 124u);
    if (previous) {
        w_u32(previous + 128u, r_u32(node + 128u));
        next = r_u32(node + 128u);
        if (next) w_u32(next + 124u, r_u32(node + 124u));
    }
update_endpoints:
    if (r_u32(owner + 100u) == node) w_u32(owner + 100u, r_u32(node + 128u));
    if (r_u32(owner + 104u) == node) w_u32(owner + 104u, r_u32(node + 124u));
    shell_sub_80045328(node);
    w_u32(owner + 108u, r_u32(owner + 108u) - 1u);
    return 0u;
}

uint32 shell_sub_80049B54(uint32 node)
{
    uint32 owner, next, previous, texture;
    FUNCTION_MARKER(0x80049B54u, "SHELL.BIN");
    if (!node) return (uint32)-2;
    texture = r_u32(node + 28u);
    owner = r_u32(node + 212u);
    if (texture) shell_sub_8003D890(texture);
    next = r_u32(node + 208u);
    if (next) {
        w_u32(next + 204u, r_u32(node + 204u));
        previous = r_u32(node + 204u);
        if (!previous) goto update_endpoints;
        w_u32(previous + 208u, r_u32(node + 208u));
    }
    previous = r_u32(node + 204u);
    if (previous) {
        w_u32(previous + 208u, r_u32(node + 208u));
        next = r_u32(node + 208u);
        if (next) w_u32(next + 204u, r_u32(node + 204u));
    }
update_endpoints:
    if (r_u32(owner + 120u) == node) w_u32(owner + 120u, r_u32(node + 208u));
    if (r_u32(owner + 124u) == node) w_u32(owner + 124u, r_u32(node + 204u));
    shell_sub_80045328(node);
    w_u32(owner + 128u, r_u32(owner + 128u) - 1u);
    return 0u;
}

uint32 shell_sub_80049DA8(uint32 node)
{
    uint32 owner, next, previous;
    FUNCTION_MARKER(0x80049DA8u, "SHELL.BIN");
    if (!node) return (uint32)-2;
    next = r_u32(node + 84u);
    owner = r_u32(node + 88u);
    if (next) {
        w_u32(next + 80u, r_u32(node + 80u));
        previous = r_u32(node + 80u);
        if (!previous) goto update_endpoints;
        w_u32(previous + 84u, r_u32(node + 84u));
    }
    previous = r_u32(node + 80u);
    if (previous) {
        w_u32(previous + 84u, r_u32(node + 84u));
        next = r_u32(node + 84u);
        if (next) w_u32(next + 80u, r_u32(node + 80u));
    }
update_endpoints:
    if (r_u32(owner + 164u) == node) w_u32(owner + 164u, r_u32(node + 84u));
    if (r_u32(owner + 168u) == node) w_u32(owner + 168u, r_u32(node + 80u));
    shell_sub_80045328(node);
    w_u32(owner + 172u, r_u32(owner + 172u) - 1u);
    return 0u;
}

uint32 shell_sub_80049C60(uint32 node)
{
    uint32 owner, next, previous, allocation;
    FUNCTION_MARKER(0x80049C60u, "SHELL.BIN");
    if (!node) return (uint32)-2;
    allocation = r_u32(node + 64u);
    owner = r_u32(node + 316u);
    if (allocation) {
        shell_sub_80053358(allocation);
        w_u32(node + 64u, 0u);
    }
    allocation = r_u32(node + 56u);
    if (allocation) {
        shell_sub_8003D890(allocation);
        w_u32(node + 56u, 0u);
    }
    allocation = r_u32(node + 60u);
    if (allocation) {
        shell_sub_8003D890(allocation);
        w_u32(node + 60u, 0u);
    }
    next = r_u32(node + 312u);
    if (next) {
        w_u32(next + 308u, r_u32(node + 308u));
        previous = r_u32(node + 308u);
        if (!previous) goto update_endpoints;
        w_u32(previous + 312u, r_u32(node + 312u));
    }
    previous = r_u32(node + 308u);
    if (previous) {
        w_u32(previous + 312u, r_u32(node + 312u));
        next = r_u32(node + 312u);
        if (next) w_u32(next + 308u, r_u32(node + 308u));
    }
update_endpoints:
    if (r_u32(owner + 144u) == node) w_u32(owner + 144u, r_u32(node + 312u));
    if (r_u32(owner + 148u) == node) w_u32(owner + 148u, r_u32(node + 308u));
    shell_sub_80045328(node);
    w_u32(owner + 152u, r_u32(owner + 152u) - 1u);
    return 0u;
}
