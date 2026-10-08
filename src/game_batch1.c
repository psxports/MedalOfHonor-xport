#include "game_shell_signatures.h"
#include "xport_trace.h"
#include "psx.h"
#include <stdlib.h>
#include <string.h>

uint32 shell_sub_80053150(uint32 model)
{
    uint32 sequence = shell_sub_80045260(32u), previous;
    FUNCTION_MARKER(0x80053150u, "SHELL.BIN");
    if (!sequence) { w_u32(0x8007BB4Cu, (uint32)-3); return 0u; }
    previous = r_u32(model + 4u);
    if (!previous) {
        w_u32(model + 8u, sequence); w_u32(model, sequence);
    } else {
        w_u32(previous + 4u, sequence); w_u32(sequence, previous);
    }
    w_u32(model + 4u, sequence);
    w_u32(model + 12u, r_u32(model + 12u) + 1u);
    w_u32(sequence + 28u, model);
    return sequence;
}

uint32 shell_sub_80052FE8(uint32 sequence)
{
    uint32 frame = shell_sub_80045260(20u), previous;
    FUNCTION_MARKER(0x80052FE8u, "SHELL.BIN");
    if (!frame) { w_u32(0x8007BB4Cu, (uint32)-3); return 0u; }
    previous = r_u32(sequence + 16u);
    if (!previous) {
        w_u32(sequence + 20u, frame); w_u32(sequence + 12u, frame);
    } else {
        w_u32(previous + 4u, frame); w_u32(frame, previous);
    }
    w_u32(sequence + 16u, frame);
    w_u32(sequence + 24u, r_u32(sequence + 24u) + 1u);
    w_u32(frame + 16u, sequence);
    return frame;
}

/* Build both buffered primitives from each original model face */
uint32 shell_model_build_packets(uint32 model, uint32 source, uint32 flags, uint32 color)
{
    static const uint32 strides[7] = {52u, 68u, 84u, 140u, 148u, 84u, 84u};
    static const uint32 fields[7] = {36u, 44u, 52u, 60u, 68u, 52u, 52u};
    static const uint32 index_offsets[7] = {40u, 56u, 64u, 128u, 128u, 64u, 64u};
    static const uint32 lengths[7] = {4u, 6u, 7u, 9u, 9u, 7u, 7u};
    static const uint32 codes[7] = {32u, 48u, 36u, 52u, 52u, 36u, 36u};
    uint32 type = r_u32(model + 20u), count = r_u32(source + 8u);
    uint32 packets, face, packet, offset, buffer, vertex, channel, position;
    if (type > 6u) return 0u;
    packets = sub_80036BF0(count * strides[type]);
    if (!packets) { w_u32(0x8007BB4Cu, (uint32)-3); return (uint32)-3; }
    if (type == 6u) {
        uint32 scratch = sub_80036BF0(count * 84u);
        w_u32(model + 132u, scratch);
        if (!scratch) { w_u32(0x8007BB4Cu, (uint32)-3); return (uint32)-3; }
    }
    w_u32(model + fields[type], packets); w_u32(model + fields[type] + 4u, count);
    face = r_u32(source + 32u);
    for (offset = 0u; (sint32)offset < (sint32)count; ++offset, face += 36u, packets += strides[type]) {
        for (position = 0u; position < 12u; ++position)
            w_u8(packets + index_offsets[type] + position, r_u8(face + 2u + position));
        for (buffer = 0u; buffer < 2u; ++buffer) {
            if (type == 0u) packet = packets + buffer * 20u;
            else if (type == 1u) packet = packets + buffer * 28u;
            else if (type == 3u || type == 4u) packet = packets + 48u + buffer * 40u;
            else packet = packets + buffer * 32u;
            w_u8(packet + 3u, lengths[type]);
            w_u8(packet + 7u, codes[type] | ((flags & 64u) ? 2u : 0u));
            if (type == 2u || type == 5u || type == 6u) {
                for (vertex = 0u; vertex < 3u; ++vertex) {
                    w_u8(packet + 12u + vertex * 8u, r_u8(face + 14u + vertex * 2u));
                    w_u8(packet + 13u + vertex * 8u, r_u8(face + 15u + vertex * 2u));
                }
                w_u16(packet + 22u, r_u16(face + 32u));
                w_u16(packet + 14u, r_u16(face + 34u));
            }
            if (type == 3u) {
                for (vertex = 0u; vertex < 3u; ++vertex) {
                    w_u8(packet + 12u + vertex * 12u, r_u8(face + 14u + vertex * 2u));
                    w_u8(packet + 13u + vertex * 12u, r_u8(face + 15u + vertex * 2u));
                }
            }
            if (type == 3u || type == 4u) {
                w_u16(packets + buffer * 40u + 74u, r_u16(face + 32u));
                w_u16(packets + buffer * 40u + 62u, r_u16(face + 34u));
            }
            if (type == 4u) {
                for (vertex = 0u; vertex < 3u; ++vertex) {
                    w_u8(packets + 140u + vertex, r_u8(face + 14u + vertex * 2u));
                    w_u8(packets + 143u + vertex, r_u8(face + 15u + vertex * 2u));
                }
            }
            for (vertex = 0u; vertex < ((type == 1u || type == 3u || type == 4u) ? 3u : 1u); ++vertex) {
                position = 4u + vertex * (type == 1u ? 8u : 12u);
                for (channel = 0u; channel < 3u; ++channel) {
                    uint8 value;
                    if (color) value = r_u8(color + channel);
                    else if (type == 2u || type == 5u || type == 6u) value = 128u;
                    else value = r_u8(face + 20u + vertex * 4u + channel);
                    w_u8(packet + position + channel, value);
                }
            }
        }
    }
    return 0u;
}

uint32 shell_sub_80051EF4(uint32 source, uint32 parameters, uint32 flags, uint32 color, uint32 callback, uint32 enabled)
{
    uint32 model = shell_sub_80045260(1280u), type = flags & 63u;
    uint32 vertices, normals, faces, sequences, pointer, index, sequence, frame, data, axis, corner;
    sint16 minimum[3], maximum[3], value;
    FUNCTION_MARKER(0x80051EF4u, "SHELL.BIN");
    if (!model) { w_u32(0x8007BB4Cu, (uint32)-3); return 0u; }
    vertices = r_u32(source); normals = r_u32(source + 4u);
    faces = r_u32(source + 8u); sequences = r_u32(source + 16u);
    w_u32(model + 20u, type); w_u32(model + 32u, 5u); w_u8(model + 17u, (uint8)enabled);
    w_u32(model + 1236u, callback); w_u32(model + 1264u, vertices);
    if (color) w_u32(model + 24u, r_u32(color));
    else { w_u8(model + 24u, 128u); w_u8(model + 25u, 128u); w_u8(model + 26u, 128u); }
    if (type == 1u || type == 3u) w_u8(model + 28u, 1u);
    pointer = source + 40u; w_u32(source + 20u, pointer);
    pointer += sequences * 4u; w_u32(model + 1268u, pointer); w_u32(source + 24u, pointer);
    pointer += vertices * 8u; w_u32(source + 28u, pointer);
    pointer += normals * 8u; w_u32(source + 32u, pointer);
    pointer += faces * 36u; w_u32(source + 36u, pointer);
    if (parameters) {
        for (index = 0u; index < 160u; index += 4u) w_u32(model + 584u + index, r_u32(parameters + index));
    } else {
        w_u16(model + 680u, 0xF000u);
        w_u16(model + 708u, 0x1000u); w_u16(model + 714u, 0x1000u); w_u16(model + 720u, 0x1000u);
        w_u16(model + 676u, 0u); w_u16(model + 678u, 0u);
        w_u8(model + 740u, 128u); w_u8(model + 741u, 128u); w_u8(model + 742u, 128u);
    }
    if ((sint32)shell_model_build_packets(model, source, flags, color) < 0) goto release_model;
    w_u32(model + 104u, r_u32(source + 36u)); w_u32(model + 112u, vertices); w_u32(model + 108u, r_u32(source + 28u));
    data = r_u32(model + 104u);
    for (index = 0u; (sint32)index < (sint32)sequences; ++index) {
        uint32 frame_index, count = r_u32(r_u32(source + 20u) + index * 4u);
        sequence = shell_sub_80053150(model);
        if (!sequence) { w_u32(0x8007BB4Cu, (uint32)-7); goto release_sequences; }
        w_u32(sequence + 8u, index);
        for (frame_index = 0u; (sint32)frame_index < (sint32)count; ++frame_index) {
            frame = shell_sub_80052FE8(sequence);
            if (!frame) { w_u32(0x8007BB4Cu, (uint32)-6); goto release_sequences; }
            w_u32(frame + 8u, frame_index); w_u32(frame + 12u, data); data += vertices * 8u;
        }
    }
    w_u32(model + 8u, r_u32(model));
    sequence = r_u32(model); frame = r_u32(sequence + 12u); data = r_u32(frame + 12u);
    for (axis = 0u; axis < 3u; ++axis) minimum[axis] = maximum[axis] = (sint16)r_u16(data + axis * 2u);
    for (frame = r_u32(sequence + 12u); frame; frame = r_u32(frame + 4u)) {
        data = r_u32(frame + 12u);
        for (index = 0u; (sint32)index < (sint32)vertices; ++index)
            for (axis = 0u; axis < 3u; ++axis) {
                value = (sint16)r_u16(data + index * 8u + axis * 2u);
                if (value < minimum[axis]) minimum[axis] = value;
                if (value > maximum[axis]) maximum[axis] = value;
            }
    }
    for (corner = 0u; corner < 8u; ++corner) {
        static const uint8 selections[8][3] = {{0,0,0},{1,0,0},{1,0,1},{0,0,1},{0,1,0},{1,1,0},{1,1,1},{0,1,1}};
        for (axis = 0u; axis < 3u; ++axis)
            w_u16(model + 136u + corner * 8u + axis * 2u, (uint16)(selections[corner][axis] ? maximum[axis] : minimum[axis]));
    }
    w_u32(model + 1244u, 1u); w_u32(model + 1248u, 0u); w_u32(model + 1252u, 0u); w_u32(model + 1256u, 2u);
    if (type == 5u || type == 6u) {
        uint32 lookup = sub_80036BF0(vertices * 4u), output, vertex;
        if (!lookup) { w_u32(0x8007BB4Cu, (uint32)-3); goto release_model; }
        output = sub_80036BF0(vertices * 8u);
        if (!output) { shell_sub_80045328(lookup); w_u32(0x8007BB4Cu, (uint32)-3); goto release_model; }
        w_u32(model + 116u, lookup); w_u32(model + 120u, output); w_u32(model + 124u, vertices);
        for (vertex = 0u; (sint32)vertex < (sint32)vertices; ++vertex) {
            uint32 matches[64], slots[64], count = 0u, face, slot;
            VECTOR average = {0}; SVECTOR normalized = {0};
            for (face = 0u; (sint32)face < r_s32(model + 56u); ++face) {
                pointer = r_u32(model + 52u) + face * 84u;
                for (slot = 0u; slot < 3u; ++slot) if ((sint16)r_u16(pointer + 64u + slot * 2u) == (sint32)vertex) {
                    if (count == 64u) { w_u32(0x8007BB4Cu, (uint32)-10); goto release_normals; }
                    matches[count] = pointer; slots[count++] = slot;
                    data = r_u32(model + 108u) + (uint32)(sint32)(sint16)r_u16(pointer + 70u) * 8u;
                    average.vx += (sint16)r_u16(data); average.vy += (sint16)r_u16(data + 2u); average.vz += (sint16)r_u16(data + 4u);
                }
            }
            if (!count) abort();
            average.vx /= (sint32)count; average.vy /= (sint32)count; average.vz /= (sint32)count;
            VectorNormal(&average, (VECTOR *)&average);
            normalized.vx = (sint16)average.vx; normalized.vy = (sint16)average.vy; normalized.vz = (sint16)average.vz;
            w_u16(output + vertex * 8u, (uint16)normalized.vx); w_u16(output + vertex * 8u + 2u, (uint16)normalized.vy); w_u16(output + vertex * 8u + 4u, (uint16)normalized.vz);
            for (index = 0u; index < count; ++index) w_u16(matches[index] + 76u + slots[index] * 2u, (uint16)vertex);
        }
    }
    return model;
release_normals:
    shell_sub_80045328(r_u32(model + 120u)); shell_sub_80045328(r_u32(model + 116u));
    goto release_model;
release_sequences:
    moh_draft_missing_call(0x80053314u, model);
release_model:
    shell_sub_80045328(r_u32(model + 132u)); shell_sub_80045328(r_u32(model + 36u));
    shell_sub_80045328(r_u32(model + 44u)); shell_sub_80045328(r_u32(model + 52u)); shell_sub_80045328(r_u32(model + 60u));
    shell_sub_80045328(model); return 0u;
}

void shell_sub_8003C30C(uint32 first, uint32 second, sint32 duration, uint32 output)
{
    uint32 start[3], end[3], axis, divisor, delta;
    FUNCTION_MARKER(0x8003C30Cu, "SHELL.BIN");
    for (axis = 0u; axis < 3u; ++axis) start[axis] = moh_draft_missing_call(0x8001D590u, r_u32(first + axis * 4u));
    for (axis = 0u; axis < 3u; ++axis) end[axis] = moh_draft_missing_call(0x8001D590u, r_u32(second + axis * 4u));
    if (duration < 1) {
        for (axis = 0u; axis < 3u; ++axis) w_u32(output + axis * 4u, 0u);
        return;
    }
    delta = moh_draft_missing_call(0x8001D3F0u, end[0], start[0]);
    divisor = moh_draft_missing_call(0x8001D590u, (uint32)duration);
    delta = moh_draft_missing_call(0x8001D1F0u, delta, divisor);
    w_u32(output, moh_draft_missing_call(0x8001E11Cu, delta));
    for (axis = 1u; axis < 3u; ++axis) {
        delta = moh_draft_missing_call(0x8001D3F0u, end[axis], start[axis]);
        delta = moh_draft_missing_call(0x8001D1F0u, delta, divisor);
        w_u32(output + axis * 4u, moh_draft_missing_call(0x8001E11Cu, delta));
    }
}

void shell_sub_8003C448(uint32 first, uint32 second, sint32 duration, uint32 output)
{
    uint32 delta, divisor;
    FUNCTION_MARKER(0x8003C448u, "SHELL.BIN");
    if (duration < 1) { w_u32(output, 0u); return; }
    delta = moh_draft_missing_call(0x8001D590u, second - first);
    divisor = moh_draft_missing_call(0x8001D590u, (uint32)duration);
    delta = moh_draft_missing_call(0x8001D1F0u, delta, divisor);
    w_u32(output, moh_draft_missing_call(0x8001E11Cu, delta));
}

uint32 shell_sub_8003C110(uint32 animation)
{
    uint32 model, count, storage, frame, next, index, sequence;
    sint32 selected;
    FUNCTION_MARKER(0x8003C110u, "SHELL.BIN");
    if (!animation) return (uint32)-2;
    model = r_u32(animation + 84u);
    if (!model) return (uint32)-5;
    if (r_u32(animation + 64u) == r_u32(animation + 68u)) return (uint32)-6;
    if (r_u8(animation + 96u)) {
        count = r_u32(model + 1264u);
        storage = sub_80036BF0(count * 40u);
        w_u32(animation + 100u, storage); w_u32(animation + 104u, storage + count * 8u);
        w_u8(animation + 96u, 0u); w_u32(animation + 108u, storage + count * 24u);
    }
    for (frame = r_u32(animation + 64u); frame; frame = r_u32(frame + 4u)) {
        next = r_u32(frame + 4u);
        if (!next) next = r_u32(animation + 64u);
        for (index = 0u; index < 16u; ++index)
            shell_sub_8003C30C(frame + 24u + index * 16u, next + 24u + index * 16u,
                r_s32(frame + 840u), frame + 560u + index * 16u);
        for (index = 0u; index < 3u; ++index)
            shell_sub_8003C448(r_u32(frame + 280u + index * 4u), r_u32(next + 280u + index * 4u),
                r_s32(frame + 840u), frame + 816u + index * 4u);
    }
    w_u8(animation + 88u, 1u);
    model = r_u32(animation + 84u); selected = r_s32(r_u32(animation + 72u) + 20u);
    if (model && selected <= (sint32)(r_u32(model + 12u) - 1u)) {
        sequence = r_u32(model);
        for (index = 0u; (sint32)index < selected; ++index) sequence = r_u32(sequence + 4u);
        w_u32(model + 8u, sequence); w_u8(model + 16u, 0u);
        w_u32(model + 1252u, r_u32(model + 1248u));
        w_u32(sequence + 20u, r_u32(sequence + 12u));
    }
    return 0u;
}

void shell_sub_8004DD9C(void)
{
    FUNCTION_MARKER(0x8004DD9Cu, "SHELL.BIN");
    memset(moh_draft_guest_pointer(0x8007BA98u, 128u), 0, 128u);
    w_u8(0x8007BA98u, 255u); w_u8(0x8007BA99u, 255u); w_u8(0x8007BA9Au, 255u);
    w_u32(0x8007BB14u, 1u); w_u8(0x8007BA9Cu, 0u); w_u8(0x8007BA9Du, 0u); w_u8(0x8007BA9Eu, 0u);
    w_u32(0x8007BAC4u, r_u32(0x8007BA8Cu)); w_u8(0x8007BA88u, 0u);
}

static uint32 shell_title_image(uint32 group, uint32 index)
{
    uint32 image;
    if (!group || (sint32)(r_u32(group + 128u) - 1u) < (sint32)index) return 0u;
    for (image = r_u32(group + 120u); image; image = r_u32(image + 208u))
        if (r_u32(image + 216u) == index) return image;
    return 0u;
}

void shell_sub_8005A1EC(uint32 screen)
{
    uint32 group = r_u32(screen + 64u), data = r_u32(screen + 80u);
    uint32 image = shell_title_image(group, 3u), selected = r_u32(r_u32(screen + 28u) + 56u), variant, item;
    FUNCTION_MARKER(0x8005A1ECu, "SHELL.BIN");
    w_u8(image + 57u, selected == 6u); w_u8(r_u32(image + 208u) + 57u, selected != 6u);
    variant = r_u32(0x8007BBA8u);
    if (variant == 1u) {
        image = shell_title_image(group, 6u); w_u8(image + 57u, selected != 2u);
    } else if (variant == 2u) {
        image = shell_title_image(group, 5u);
        w_u8(image + 57u, selected == 5u); w_u8(r_u32(image + 208u) + 57u, selected != 5u);
    } else abort();
    for (item = r_u32(screen + 52u); item; item = r_u32(item + 136u))
        w_u32(r_u32(item + 88u) + 80u, selected < 2u);
    shell_sub_8004E1B8(data, 0u);
}

void shell_sub_8004E1B8(uint32 data, uint32 mode)
{
    uint32 screen = r_u32(data + 8u), delay;
    FUNCTION_MARKER(0x8004E1B8u, "SHELL.BIN");
    moh_draft_missing_call(0x8004BDB4u, screen);
    moh_draft_missing_call(0x8004C51Cu, screen, mode);
    moh_draft_missing_call(0x8004BF74u, screen);
    if (!r_u8(0x8007BB10u)) {
        w_u32(0x8007BA94u, 8u); w_u8(0x8007BA88u, 0u);
    } else {
        delay = r_u32(0x8007BA94u) - 1u; w_u32(0x8007BA94u, delay);
        if ((sint32)delay < 0) { w_u8(0x8007BA88u, 1u); w_u32(0x8007BA94u, (uint32)-1); }
    }
    if (r_s32(data + 16u) < 1 && !r_u8(0x8007BB10u))
        w_u8(0x8007BB10u, (uint8)moh_draft_missing_call(0x8004DE8Cu, 0x8007BA98u));
}

uint32 shell_sub_8004BDB4(uint32 screen)
{
    uint32 group = r_u32(screen + 64u), item, media, offset;
    FUNCTION_MARKER(0x8004BDB4u, "SHELL.BIN");
    if (!screen) return (uint32)-2;
    if (r_u32(screen + 68u)) moh_draft_missing_call(0x8004BEECu);
    if (!group) return 1u;
    media = r_u32(group + 160u);
    if (media) {
        memset(moh_draft_guest_pointer(0x8007BB5Cu, 40u), 0, 40u);
        for (offset = 0u; offset < 40u; ++offset) w_u8(0x8007BB5Cu + offset, r_u8(media + 36u + offset));
        w_u32(0x8007BB6Cu, r_u32(media + 28u)); w_u8(0x8007BB59u, r_u8(media + 32u) != 0u);
        return (sint32)moh_draft_missing_call(0x80056CC8u, media) < 0 ? (uint32)-9 : 0u;
    }
    for (item = r_u32(group + 120u); item; item = r_u32(item + 208u))
        moh_draft_missing_call(0x8004CFB4u, item, 1u);
    for (item = r_u32(group + 144u); item; item = r_u32(item + 312u))
        moh_draft_missing_call(0x8004D508u, item);
    return 0u;
}

void shell_sub_8004D694(uint32 color)
{
    uint32 index, count, reverse;
    FUNCTION_MARKER(0x8004D694u, "SHELL.BIN");
    if (r_u8(0x8007BA78u) || r_s32(color + 44u) <= 0) { w_u32(color + 8u, r_u32(color)); return; }
    if (!r_u32(color + 48u)) {
        reverse = r_u8(color + 52u);
        moh_draft_missing_call(0x8004D794u, color, reverse ? color + 4u : color, reverse ? color : color + 4u);
        w_u8(color + 52u, reverse == 0u);
    }
    /* Read all displayed channels before advancing the accumulators */
    {
        uint8 red = (uint8)r_u16(color + 14u), green = (uint8)r_u16(color + 18u), blue = (uint8)r_u16(color + 22u);
        w_u32(color + 12u, r_u32(color + 12u) + r_u32(color + 28u));
        w_u8(color + 8u, red); w_u8(color + 9u, green); w_u8(color + 10u, blue);
    }
    for (index = 1u; index < 3u; ++index)
        w_u32(color + 12u + index * 4u, r_u32(color + 12u + index * 4u) + r_u32(color + 28u + index * 4u));
    count = r_u32(color + 48u) + 1u;
    w_u32(color + 48u, count == r_u32(color + 44u) ? 0u : count);
}

void shell_sub_8004CFB4(uint32 image, uint32 mode)
{
    uint32 texture, packet, vertex, channel, uv, color, depth, order;
    uint16 x, y, width, height;
    uint8 rotated;
    FUNCTION_MARKER(0x8004CFB4u, "SHELL.BIN");
    if (!image || r_u8(image + 57u) || !r_u32(image + 28u)) return;
    texture = r_u32(r_u32(image + 28u));
    packet = image + (uint32)r_u8(0x8007BF90u) * 40u + 60u;
    rotated = r_u8(image + 54u);
    for (vertex = 0u; vertex < 4u; ++vertex) {
        static const uint8 rotation[4] = {2u, 0u, 3u, 1u};
        uv = rotated ? rotation[vertex] : vertex;
        w_u8(packet + 12u + vertex * 8u, r_u8(texture + 52u + uv * 2u));
        w_u8(packet + 13u + vertex * 8u, r_u8(texture + 53u + uv * 2u));
    }
    x = r_u16(image + 46u); y = r_u16(image + 48u);
    if (r_u8(image + 55u)) { width = r_u16(image + 50u); height = r_u16(image + 52u); }
    else { width = r_u16(texture + (rotated ? 36u : 32u)); height = r_u16(texture + (rotated ? 32u : 36u)); }
    for (vertex = 0u; vertex < 4u; ++vertex) {
        w_u16(packet + 8u + vertex * 8u, (uint16)(x + ((vertex & 1u) ? width - 1u : 0u)));
        w_u16(packet + 10u + vertex * 8u, (uint16)(y + ((vertex & 2u) ? height - 1u : 0u)));
    }
    w_u8(packet + 3u, 9u); w_u8(packet + 7u, r_u8(image + 140u) ? 46u : 44u);
    if (mode == 1u) { shell_sub_8004D694(image + 144u); color = image + 152u; }
    else color = image + (mode == 0u ? 144u : 148u);
    for (channel = 0u; channel < 3u; ++channel) w_u8(packet + 4u + channel, r_u8(color + channel));
    w_u16(packet + 22u, r_u16(texture + 50u)); w_u16(packet + 14u, r_u16(texture + 48u));
    depth = r_u32(image + 200u);
    if ((sint32)depth < 0) depth = 0u;
    if ((sint32)depth > 1535) depth = 1535u;
    w_u32(image + 200u, depth);
    order = r_u32(r_u32(0x8007BF4Cu) + 116u) + depth * 4u;
    w_u32(packet, (r_u32(packet) & 0xFF000000u) | (r_u32(order) & 0xFFFFFFu));
    w_u32(order, (r_u32(order) & 0xFF000000u) | (packet & 0xFFFFFFu));
}

void shell_sub_8004D508(uint32 widget)
{
    uint32 x, y, model, type, offset;
    FUNCTION_MARKER(0x8004D508u, "SHELL.BIN");
    if (r_u8(widget + 300u)) return;
    x = r_u32(widget + 80u); y = r_u32(widget + 84u);
    if (x == 0x7FFFFFFFu) x = r_u32(0x8007BF70u);
    if (y == 0x7FFFFFFFu) y = r_u32(0x8007BF74u);
    SetGeomOffset((sint32)x, (sint32)y);
    model = r_u32(widget + 64u);
    if (model) {
        if (r_u32(model + 20u) == 2u) {
            w_u8(model + 24u, r_u8(widget + 112u)); w_u8(model + 25u, r_u8(widget + 112u)); w_u8(model + 26u, r_u8(widget + 112u));
        }
        if (r_u32(widget + 256u) != 4096u || r_u32(widget + 260u) != 4096u || r_u32(widget + 264u) != 4096u) {
            model = r_u32(widget + 64u); w_u8(model + 656u, 1u);
            for (offset = 0u; offset < 16u; offset += 4u) w_u32(model + 660u + offset, r_u32(widget + 256u + offset));
        }
        model = r_u32(widget + 64u);
        if (r_u32(model + 20u) - 5u < 2u) moh_draft_missing_call(0x80055CF8u, model);
        type = r_u32(model + 20u);
        if (type == 1u || type == 3u || type == 4u) moh_draft_missing_call(0x80063A3Cu, model + 584u);
        moh_draft_missing_call(0x80063AA8u, model + 584u);
        moh_draft_missing_call(0x80053C8Cu, model, 2u);
    }
    SetGeomOffset(r_s32(0x8007BF70u), r_s32(0x8007BF74u));
}

void shell_sub_8004C710(uint32 group, uint32 mode)
{
    uint32 item;
    FUNCTION_MARKER(0x8004C710u, "SHELL.BIN");
    if (!group) return;
    item = r_u32(group + 112u); if (item) shell_sub_8004CFB4(item, mode);
    item = r_u32(group + 116u); if (item) shell_sub_8004CFB4(item, mode);
    item = r_u32(group + 136u); if (item) shell_sub_8004D508(item);
    item = r_u32(group + 140u); if (item) shell_sub_8004D508(item);
}

uint32 shell_sub_8004C51C(uint32 screen, uint32 override)
{
    static const uint32 handlers[15] = {
        0x8004C710u, 0x8004C7A0u, 0x8004C860u, 0x8004C710u, 0x8004C948u,
        0x8004CA40u, 0x8004C710u, 0x8004CAF4u, 0x8004CBC0u, 0x8004CCACu,
        0x8004CD58u, 0x8004CDECu, 0x8004CE68u, 0x8004CEC8u, 0x8004CF44u
    };
    uint32 group, mode, active;
    FUNCTION_MARKER(0x8004C51Cu, "SHELL.BIN");
    if (!screen || override >= 16u) return (uint32)-2;
    if (override == 4u) {
        group = r_u32(screen + 28u);
        if (!r_u8(group + 80u)) shell_sub_8004C710(group, 1u);
        return 0u;
    }
    for (group = r_u32(screen + 32u); group; group = r_u32(group + 180u)) {
        active = group == r_u32(screen + 28u) || r_u8(group + 88u);
        mode = override ? override : r_u32(group + 84u);
        if (r_u8(group + 80u)) continue;
        if (mode < 1u || mode > 15u) return (uint32)-10;
        if (mode == 7u) active = 1u;
        if (handlers[mode - 1u] == 0x8004C710u) shell_sub_8004C710(group, active);
        else moh_draft_missing_call(handlers[mode - 1u], group, active);
    }
    return 0u;
}

void shell_sub_8004CAF4(uint32 group)
{
    uint32 item;
    FUNCTION_MARKER(0x8004CAF4u, "SHELL.BIN");
    if (!group || group != r_u32(r_u32(group + 184u) + 28u)) return;
    item = r_u32(group + 112u); if (item) shell_sub_8004CFB4(item, 1u);
    for (item = r_u32(group + 120u); item; item = r_u32(item + 208u))
        if (item == r_u32(group + 116u)) shell_sub_8004CFB4(item, 1u);
    for (item = r_u32(group + 144u); item; item = r_u32(item + 312u))
        if (item == r_u32(group + 140u)) shell_sub_8004D508(item);
}

uint32 shell_sub_8004BF74(uint32 screen)
{
    uint32 item, animation, source, model, frame, index, type, sequence;
    uint32 parameters[40]; sint32 selected;
    FUNCTION_MARKER(0x8004BF74u, "SHELL.BIN");
    if (!screen) return (uint32)-2;
    item = r_u32(screen + 52u); if (!item) return 2u;
    for (; item; item = r_u32(item + 136u)) {
        if (r_u8(item + 108u) || !r_u32(item + 88u)) continue;
        animation = r_u32(item + 88u);
        moh_draft_missing_call(0x8003C4ACu, animation);
        model = r_u32(item + 96u);
        if (!model || !r_u8(item + 110u)) continue;
        source = r_u32(item + 92u);
        for (index = 0u; index < 40u; ++index) parameters[index] = r_u32(source + 584u + index * 4u);
        for (index = 0u; index < 40u; ++index) w_u32(model + 584u + index * 4u, parameters[index]);
        w_u8(model + 656u, 1u);
        for (index = 0u; index < 4u; ++index) w_u32(model + 660u + index * 4u, r_u32(item + 112u + index * 4u));
        w_u8(model + 1272u, r_u8(source + 1272u)); w_u32(model + 1276u, r_u32(source + 1276u));
        frame = r_u32(r_u32(item + 88u) + 72u);
        if (!r_u32(frame + 836u)) {
            selected = r_s32(frame + 20u);
            model = r_u32(item + 96u);
            if (model && selected <= (sint32)(r_u32(model + 12u) - 1u)) {
                sequence = r_u32(model);
                for (index = 0u; (sint32)index < selected; ++index) sequence = r_u32(sequence + 4u);
                w_u32(model + 8u, sequence); w_u8(model + 16u, 0u);
                w_u32(model + 1252u, r_u32(model + 1248u)); w_u32(sequence + 20u, r_u32(sequence + 12u));
            }
        }
        frame = r_u32(r_u32(item + 88u) + 72u);
        SetGeomOffset((sint16)r_u16(frame + 550u), (sint16)r_u16(frame + 554u));
        model = r_u32(item + 96u);
        if (r_u32(model + 20u) - 5u < 2u) moh_draft_missing_call(0x80055CF8u, model);
        type = r_u32(model + 20u);
        if (type == 1u || type == 3u || type == 4u) moh_draft_missing_call(0x80063A3Cu, model + 584u);
        moh_draft_missing_call(0x80063AA8u, model + 584u); moh_draft_missing_call(0x80053C8Cu, model, 1u);
        SetGeomOffset(r_s32(0x8007BF70u), r_s32(0x8007BF74u));
    }
    return 0u;
}

void shell_sub_80063A3C(uint32 parameters)
{
    MATRIX rotation = {0}, light = {0};
    FUNCTION_MARKER(0x80063A3Cu, "SHELL.BIN");
    RotMatrix((SVECTOR *)moh_draft_guest_pointer(parameters + 64u, 8u), &rotation);
    MulMatrix0((MATRIX *)moh_draft_guest_pointer(parameters + 92u, sizeof(MATRIX)), &rotation, &light);
    SetLightMatrix(&light);
    SetColorMatrix((MATRIX *)moh_draft_guest_pointer(parameters + 124u, sizeof(MATRIX)));
    SetBackColor(r_u8(parameters + 156u), r_u8(parameters + 157u), r_u8(parameters + 158u));
}

void shell_sub_80063AA8(uint32 parameters)
{
    MATRIX *model = (MATRIX *)moh_draft_guest_pointer(parameters, sizeof(MATRIX));
    MATRIX *camera = (MATRIX *)moh_draft_guest_pointer(0x8007E6F0u, sizeof(MATRIX));
    VECTOR negative = {0}, translated = {0}, position = {0}, result = {0};
    uint32 row, column; sint32 divisor, product;
    FUNCTION_MARKER(0x80063AA8u, "SHELL.BIN");
    RotMatrix((SVECTOR *)moh_draft_guest_pointer(parameters + 64u, 8u), model);
    if (r_u8(parameters + 72u)) ScaleMatrix(model, (VECTOR *)moh_draft_guest_pointer(parameters + 76u, sizeof(VECTOR)));
    RotMatrix((SVECTOR *)moh_draft_guest_pointer(0x8007E730u, 8u), camera);
    for (row = 0u; row < 3u; ++row) {
        uint32 sum = 0u;
        for (column = 0u; column < 3u; ++column) sum += r_u16(0x8007E6F0u + row * 6u + column * 2u);
        w_u16(0x8007E738u + row * 2u, (uint16)sum);
    }
    if (r_u8(0x8007BF7Cu)) {
        divisor = r_s32(0x8007BF84u);
        if (!divisor) abort();
        for (column = 0u; column < 3u; ++column) {
            product = (sint32)((uint32)(sint32)(sint16)r_u16(0x8007E6F6u + column * 2u) * r_u32(0x8007BF80u));
            if (product == (-2147483647 - 1) && divisor == -1) abort();
            w_u16(0x8007E6F6u + column * 2u, (uint16)(product / divisor));
        }
    }
    negative.vx = (sint32)(0u - r_u32(0x8007E720u));
    negative.vy = (sint32)(0u - r_u32(0x8007E724u));
    negative.vz = (sint32)(0u - r_u32(0x8007E728u));
    ApplyMatrixLV(camera, &negative, &translated);
    position.vx = r_s32(parameters + 48u); position.vy = r_s32(parameters + 52u); position.vz = r_s32(parameters + 56u);
    ApplyMatrixLV(camera, &position, &result);
    w_u32(0x8007E710u, (uint32)translated.vx + (uint32)result.vx);
    w_u32(0x8007E714u, (uint32)translated.vy + (uint32)result.vy);
    w_u32(0x8007E718u, (uint32)translated.vz + (uint32)result.vz);
    MulMatrix(camera, model);
    TransMatrix(camera, (VECTOR *)moh_draft_guest_pointer(0x8007E710u, 12u));
    SetRotMatrix(camera); SetTransMatrix(camera);
}

void shell_sub_8003CCEC(uint32 model, uint32 frame_parameters, uint32 alternate)
{
    uint32 axis, row, column, bank = alternate ? 112u : 0u;
    FUNCTION_MARKER(0x8003CCECu, "SHELL.BIN");
    for (axis = 0u; axis < 3u; ++axis) {
        w_u32(model + 632u + axis * 4u, (uint32)(sint32)(sint16)r_u16(frame_parameters + 226u + axis * 4u));
        w_u16(model + 648u + axis * 2u, r_u16(frame_parameters + 242u + axis * 4u));
    }
    SetGeomOffset((sint16)r_u16(frame_parameters + 258u), (sint16)r_u16(frame_parameters + 262u));
    w_u8(model + 656u, 1u);
    for (axis = 0u; axis < 3u; ++axis)
        w_u32(model + 660u + axis * 4u, (uint32)(sint32)(sint16)r_u16(frame_parameters + 266u));
    for (axis = 0u; axis < 3u; ++axis)
        w_u8(model + 740u + axis, (uint8)r_u16(frame_parameters + bank + 2u + axis * 4u));
    for (row = 0u; row < 3u; ++row)
        for (column = 0u; column < 3u; ++column) {
            w_u16(model + 676u + row * 6u + column * 2u,
                (uint16)(0u - r_u16(frame_parameters + bank + 18u + row * 16u + column * 4u)));
            w_u16(model + 708u + column * 6u + row * 2u,
                r_u16(frame_parameters + bank + 66u + row * 16u + column * 4u));
        }
}

uint32 shell_sub_8003C4AC(uint32 animation)
{
    uint32 model, frame, index, axis, saved[4], next, sequence, callback, count, start, target, accum, steps, output, value;
    uint8 scaled; sint32 selected;
    FUNCTION_MARKER(0x8003C4ACu, "SHELL.BIN");
    if (!animation) return (uint32)-2;
    model = r_u32(animation + 84u); if (!model) return (uint32)-5;
    if (!r_u8(animation + 88u)) return (uint32)-7;
    scaled = r_u8(model + 656u);
    for (index = 0u; index < 4u; ++index) saved[index] = r_u32(model + 660u + index * 4u);
    frame = r_u32(animation + 72u);
    shell_sub_8003CCEC(model, frame + 292u, r_u32(animation + 80u));
    frame = r_u32(animation + 72u); w_u32(frame + 836u, r_u32(frame + 836u) + 1u);
    frame = r_u32(animation + 72u);
    if (r_s32(frame + 840u) < r_s32(frame + 836u)) {
        w_u32(frame + 836u, 0u); frame = r_u32(animation + 72u);
        for (index = 0u; index < 268u; index += 4u) w_u32(frame + 292u + index, r_u32(frame + 24u + index));
        next = r_u32(frame + 4u); w_u32(animation + 72u, next);
        if (!next) w_u32(animation + 72u, r_u32(animation + 64u));
        frame = r_u32(animation + 72u); selected = r_s32(frame + 20u);
        if (selected <= (sint32)(r_u32(model + 12u) - 1u)) {
            sequence = r_u32(model);
            for (index = 0u; (sint32)index < selected; ++index) sequence = r_u32(sequence + 4u);
            w_u32(model + 8u, sequence); w_u8(model + 16u, 0u);
            w_u32(model + 1252u, r_u32(model + 1248u)); w_u32(sequence + 20u, r_u32(sequence + 12u));
        }
        callback = r_u32(animation + 92u);
        if (callback && r_s32(frame + 828u) > 0) moh_draft_indirect_call(callback, r_u32(frame + 828u), r_u32(frame + 832u));
        frame = r_u32(animation + 72u);
        if (r_u32(frame + 16u) == 1u) {
            next = r_u32(frame + 4u); if (!next) next = r_u32(animation + 64u);
            sequence = r_u32(r_u32(animation + 84u));
            while (sequence && r_u32(sequence + 8u) != r_u32(next + 20u)) sequence = r_u32(sequence + 4u);
            target = r_u32(r_u32(sequence + 12u) + 12u); start = r_u32(model + 1268u);
            accum = r_u32(animation + 104u); steps = r_u32(animation + 108u); count = r_u32(model + 1264u);
            for (index = 0u; (sint32)index < (sint32)count; ++index)
                for (axis = 0u; axis < 3u; ++axis) {
                    value = (uint32)r_u16(start + index * 8u + axis * 2u) << 16u;
                    w_u32(accum + index * 16u + axis * 4u, value);
                    value = ((uint32)r_u16(target + index * 8u + axis * 2u) << 16u) - value;
                    w_u32(steps + index * 16u + axis * 4u, (uint32)((sint32)value >> (r_u32(frame + 844u) & 31u)));
                }
            memmove(moh_draft_guest_pointer(r_u32(animation + 100u), count * 8u), moh_draft_guest_pointer(start, count * 8u), count * 8u);
        }
    } else {
        for (index = 0u; index < 16u; ++index)
            for (axis = 0u; axis < 3u; ++axis) {
                output = frame + 292u + index * 16u + axis * 4u;
                w_u32(output, r_u32(output) + r_u32(frame + 560u + index * 16u + axis * 4u));
            }
        frame = r_u32(animation + 72u);
        for (axis = 0u; axis < 3u; ++axis) w_u32(frame + 548u + axis * 4u, r_u32(frame + 548u + axis * 4u) + r_u32(frame + 816u + axis * 4u));
        frame = r_u32(animation + 72u);
        if (r_u32(frame + 16u) == 1u) {
            accum = r_u32(animation + 104u); steps = r_u32(animation + 108u); output = r_u32(animation + 100u);
            for (index = 0u; (sint32)index < r_s32(model + 1264u); ++index)
                for (axis = 0u; axis < 3u; ++axis) {
                    start = accum + index * 16u + axis * 4u; value = r_u32(start) + r_u32(steps + index * 16u + axis * 4u);
                    w_u32(start, value); w_u16(output + index * 8u + axis * 2u, (uint16)(value >> 16u));
                }
        }
    }
    frame = r_u32(animation + 72u);
    if (r_u32(frame + 16u) == 1u) { w_u8(model + 1272u, 1u); w_u32(model + 1276u, r_u32(animation + 100u)); }
    else w_u8(model + 1272u, 0u);
    if (r_u32(model + 20u) - 5u < 2u) moh_draft_missing_call(0x80055CF8u, model);
    value = r_u32(model + 20u);
    if (value == 1u || value == 3u || value == 4u) moh_draft_missing_call(0x80063A3Cu, model + 584u);
    moh_draft_missing_call(0x80063AA8u, model + 584u); moh_draft_missing_call(0x80053C8Cu, model, 1u);
    SetGeomOffset(r_s32(0x8007BF70u), r_s32(0x8007BF74u));
    w_u8(model + 656u, scaled);
    for (index = 0u; index < 4u; ++index) w_u32(model + 660u + index * 4u, saved[index]);
    return 0u;
}
