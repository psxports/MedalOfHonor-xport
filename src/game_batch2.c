#include "game_shell_signatures.h"
#include "xport_trace.h"
#include "psx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32 shell_sub_80053418(uint32 model);

/* Native COP2 operations use the existing shared GTE register bank */
static void shell_renderer_load_vector(uint32 vertex, uint32 slot)
{
    xport_gte_write_data(slot * 2u, r_u32(vertex));
    xport_gte_write_data(slot * 2u + 1u, r_u32(vertex + 4u));
}

static uint32 shell_renderer_flag(void)
{
    PsxGteSnapshot snapshot;
    psx_gte_snapshot(&snapshot);
    return (uint32)snapshot.flag;
}

static uint32 shell_renderer_project(uint32 vertex, uint32 *xy, uint32 *projection, uint32 *flag)
{
    shell_renderer_load_vector(vertex, 0u);
    xport_gte_execute(0x180001u);
    *xy = xport_gte_read_data(14u);
    *projection = xport_gte_read_data(8u);
    *flag = shell_renderer_flag();
    return (uint32)((sint32)xport_gte_read_data(19u) >> 2);
}

static void shell_renderer_link(uint32 packet, uint32 ordering_entry)
{
    w_u32(packet, (r_u32(packet) & 0xFF000000u) | (r_u32(ordering_entry) & 0xFFFFFFu));
    w_u32(ordering_entry, (r_u32(ordering_entry) & 0xFF000000u) | (packet & 0xFFFFFFu));
}

static uint32 shell_renderer_outside(uint32 packet, uint32 xy_stride)
{
    sint32 x[3], y[3];
    uint32 vertex;
    for (vertex = 0u; vertex < 3u; ++vertex) {
        x[vertex] = (sint16)r_u16(packet + 8u + vertex * xy_stride);
        y[vertex] = (sint16)r_u16(packet + 10u + vertex * xy_stride);
    }
    return (y[0] < 0 && y[1] < 0 && y[2] < 0) ||
        (y[0] > 239 && y[1] > 239 && y[2] > 239) ||
        (x[0] < 0 && x[1] < 0 && x[2] < 0) ||
        (x[0] > 511 && x[1] > 511 && x[2] > 511);
}

static void shell_renderer_store_rgb(uint32 destination, uint32 color)
{
    w_u8(destination, (uint8)color);
    w_u8(destination + 1u, (uint8)(color >> 8));
    w_u8(destination + 2u, (uint8)(color >> 16));
}

uint32 shell_sub_80053C8C(uint32 model, uint32 flags)
{
    static const uint32 fields[7] = {36u, 44u, 52u, 60u, 68u, 52u, 52u};
    static const uint32 strides[7] = {52u, 68u, 84u, 140u, 148u, 84u, 84u};
    static const uint32 index_offsets[7] = {40u, 56u, 64u, 128u, 128u, 64u, 64u};
    static const uint32 packet_offsets[7] = {0u, 0u, 0u, 48u, 48u, 0u, 0u};
    static const uint32 packet_sizes[7] = {20u, 28u, 32u, 40u, 40u, 32u, 32u};
    static const uint32 xy_strides[7] = {4u, 8u, 8u, 12u, 12u, 8u, 8u};
    uint32 buffer = r_u8(0x8007BF90u), ordering, type, field, block, vertices;
    uint32 face, vertex, plane, xy, projection, flag, depth, clip, packet;
    uint32 normals[3], addresses[3], entry, rgb, scratch;
    uint32 depth_bias, sequence, next, accumulated;
    uint8 u_offset = 0u, v_offset = 0u;
    PSX_RECT disabled_window = {0, 0, 0, 0};
    FUNCTION_MARKER(0x80053C8Cu, "SHELL.BIN");
    ordering = r_u8(model + 1240u) ? r_u32(r_u32(0x8007BF4Cu) + 112u) : 0u;
    if (!ordering) ordering = r_u32(r_u32(0x8007BF4Cu) + 116u);
    for (field = 36u; field <= 68u; field += 8u) {
        if (r_u32(model + field))
            w_u32(0x8007BB3Cu, r_u32(0x8007BB3Cu) + r_u32(model + field + 4u));
    }
    if (!r_u8(model + 1260u)) {
        /* Test all four screen planes before processing the model */
        for (plane = 0u; plane < 4u; ++plane) {
            uint32 visible = 0u;
            for (vertex = 0u; vertex < 8u; ++vertex) {
                depth = shell_renderer_project(model + 136u + vertex * 8u, &xy, &projection, &flag);
                if (depth < 1249u && (sint32)flag >= 0) {
                    sint32 coordinate;
                    if (plane == 0u) coordinate = (sint16)(xy >> 16);
                    else if (plane == 1u) coordinate = 511 - (sint16)xy;
                    else if (plane == 2u) coordinate = 239 - (sint16)(xy >> 16);
                    else coordinate = (sint16)xy;
                    if (coordinate >= 0) { visible = 1u; break; }
                }
            }
            if (!visible) return 1u;
        }
    }
    depth_bias = r_u32(model + 1236u);
    shell_sub_80053418(model);
    type = r_u32(model + 20u);
    if (type <= 6u) {
        field = fields[type];
        block = r_u32(model + field);
        if (type == 4u) {
            /* Capture both offsets before advancing the texture accumulators */
            u_offset = r_u8(model + 92u) & (uint8)(r_u32(model + 76u) >> 16);
            v_offset = r_u8(model + 93u) & (uint8)r_u16(model + 82u);
            w_u32(model + 76u, r_u32(model + 76u) + r_u32(model + 84u));
            w_u32(model + 80u, r_u32(model + 80u) + r_u32(model + 88u));
        }
        vertices = r_u32(r_u32(r_u32(model + 8u) + 20u) + 12u);
        w_u32(model + 1268u, vertices);
        if ((type == 3u || type == 4u) && r_u8(model + 1272u)) vertices = r_u32(model + 1276u);
        for (face = 0u; (sint32)face < (sint32)r_u32(model + field + 4u); ++face) {
            packet = block + packet_offsets[type] + buffer * packet_sizes[type];
            for (vertex = 0u; vertex < 3u; ++vertex) {
                addresses[vertex] = vertices + (uint32)(sint32)(sint16)r_u16(block + index_offsets[type] + vertex * 2u) * 8u;
                if (type == 1u || type == 3u || type == 4u)
                    normals[vertex] = r_u32(model + 108u) + (uint32)(sint32)(sint16)r_u16(block + index_offsets[type] + 6u + vertex * 2u) * 8u;
            }
            if (type == 2u && (flags & 2u))
                shell_renderer_store_rgb(packet + 4u, r_u32(model + 24u));
            for (vertex = 0u; vertex < 3u; ++vertex) shell_renderer_load_vector(addresses[vertex], vertex);
            xport_gte_execute(0x280030u);
            flag = shell_renderer_flag();
            xport_gte_execute(0x1400006u);
            clip = xport_gte_read_data(24u);
            for (vertex = 0u; vertex < 3u; ++vertex)
                w_u32(packet + 8u + vertex * xy_strides[type], xport_gte_read_data(12u + vertex));
            projection = xport_gte_read_data(8u);
            xport_gte_execute(0x158002Du);
            depth = xport_gte_read_data(7u);
            if (depth >= 1249u || (sint32)flag < 0 || (sint32)clip <= 0) goto rejected;
            if (type == 1u || type == 3u || type == 4u) {
                for (vertex = 0u; vertex < 3u; ++vertex) shell_renderer_load_vector(normals[vertex], vertex);
                xport_gte_write_data(6u, r_u32(model + 24u));
                xport_gte_execute(0x118043Fu);
            }
            if (shell_renderer_outside(packet, xy_strides[type])) goto rejected;
            entry = ordering + depth * 4u - depth_bias * 4u + 1024u;
            if (type == 5u) {
                for (vertex = 0u; vertex < 3u; ++vertex) {
                    uint32 uv = r_u32(model + 116u) + (uint32)(sint32)(sint16)r_u16(block + 76u + vertex * 2u) * 4u;
                    w_u8(packet + 12u + vertex * 8u, r_u8(uv));
                    w_u8(packet + 13u + vertex * 8u, r_u8(uv + 1u));
                }
                shell_renderer_link(packet, entry);
            } else if (type == 6u) {
                uint32 uv_addresses[3];
                for (vertex = 0u; vertex < 3u; ++vertex)
                    uv_addresses[vertex] = r_u32(model + 116u) + (uint32)(sint32)(sint16)r_u16(block + 76u + vertex * 2u) * 4u;
                shell_renderer_link(packet, entry);
                scratch = r_u32(model + 132u) + face * 84u + buffer * 32u;
                for (vertex = 0u; vertex < 3u; ++vertex) {
                    w_u16(scratch + 8u + vertex * 8u, r_u16(packet + 8u + vertex * 8u));
                    w_u16(scratch + 10u + vertex * 8u, r_u16(packet + 10u + vertex * 8u));
                }
                for (vertex = 0u; vertex < 3u; ++vertex) {
                    w_u8(scratch + 12u + vertex * 8u, r_u8(uv_addresses[vertex]));
                    w_u8(scratch + 13u + vertex * 8u, r_u8(uv_addresses[vertex] + 1u));
                }
                shell_renderer_link(scratch, entry - 4u);
            } else if (type == 4u) {
                uint32 window = block + buffer * 12u;
                SetTexWindow(moh_draft_guest_pointer(window + 24u, 12u), &disabled_window);
                shell_renderer_link(window + 24u, entry);
                for (vertex = 0u; vertex < 3u; ++vertex) {
                    w_u8(packet + 12u + vertex * 12u, (uint8)(r_u8(block + 140u + vertex) + u_offset));
                    w_u8(packet + 13u + vertex * 12u, (uint8)(r_u8(block + 143u + vertex) + v_offset));
                }
                shell_renderer_link(packet, entry);
                SetTexWindow(moh_draft_guest_pointer(window, 12u), (PSX_RECT *)moh_draft_guest_pointer(model + 94u, 8u));
                shell_renderer_link(window, entry);
                for (vertex = 0u; vertex < 3u; ++vertex) {
                    rgb = xport_gte_read_data(20u + vertex);
                    shell_renderer_store_rgb(packet + 4u + vertex * 12u, rgb);
                }
            } else {
                shell_renderer_link(packet, entry);
                if (type == 1u || type == 3u) {
                    for (vertex = 0u; vertex < 3u; ++vertex) {
                        rgb = xport_gte_read_data(20u + vertex);
                        shell_renderer_store_rgb(packet + 4u + vertex * xy_strides[type], rgb);
                    }
                }
            }
            block += strides[type];
            continue;
rejected:
            w_u32(0x8007BB38u, r_u32(0x8007BB38u) + 1u);
            block += strides[type];
        }
        if (r_u8(0x8007BB45u)) {
            /* TODO Translate the optional bounding-box debug renderer */
            moh_draft_missing_call(0x80055A08u, model, ordering);
        }
        if ((type == 1u || type == 3u || type == 4u) && r_u8(0x8007BB44u))
            moh_draft_missing_call(0x80055A00u, model, ordering);
    }
    if (!r_u8(model + 1272u)) {
        accumulated = r_u32(model + 1252u) + r_u32(model + 1256u);
        w_u32(model + 1252u, accumulated);
        w_u8(model + 16u, 0u);
        if ((sint32)r_u32(model + 1244u) < (sint32)accumulated) {
            w_u32(model + 1252u, r_u32(model + 1248u));
            sequence = r_u32(model + 8u);
            next = r_u32(r_u32(sequence + 20u) + 4u);
            w_u32(sequence + 20u, next);
            sequence = r_u32(model + 8u);
            if (r_u32(sequence + 20u)) return 0u;
            w_u32(sequence + 20u, r_u32(sequence + 12u));
            w_u8(model + 16u, 1u);
        }
    }
    return 0u;
}

static uint32 shell_title_name_equal(uint32 left, uint32 right, const char *host_name)
{
    uint32 index = 0u;
    if (!left || (!right && !host_name)) return 0u;
    while (r_u8(left + index)) {
        uint8 left_byte = (uint8)moh_draft_missing_call(0x8001E2ACu, (uint32)r_u8(left + index));
        uint8 right_byte = (uint8)moh_draft_missing_call(0x8001E2ACu,
            host_name ? (uint32)(uint8)host_name[index] : (uint32)r_u8(right + index));
        if (left_byte != right_byte) return 0u;
        ++index;
    }
    return (host_name ? (uint8)host_name[index] : r_u8(right + index)) == 0u;
}

uint32 shell_title_find_screen(uint32 name, const char *host_name)
{
    uint32 screen = r_u32(0x8007BA6Cu);
    while (screen) {
        if (shell_title_name_equal(screen, name, host_name)) return screen;
        screen = r_u32(screen + 128u);
    }
    return 0u;
}

static void shell_title_play_sample(uint32 sound, uint32 sample)
{
    uint32 voice;
    if (!sound) { w_u32(0x8007C218u, (uint32)-2); return; }
    voice = shell_sub_8006349C();
    w_u32(0x8007C374u, voice);
    w_u32(r_u32(sound + 12u) + sample * 4u, voice);
    shell_sub_80063510((uint32)(sint32)(sint16)r_u16(sound), (sint32)sample, voice,
        r_u8(sound + 16u), r_u8(sound + 17u), r_u32(sound + 4u), r_u32(sound + 8u));
}

void shell_sub_80059030(uint32 screen, uint32 result)
{
    uint32 data = r_u32(screen + 80u), item, target, selected, mission;
    uint32 sample, saved_rate, index, pointer;
    sint32 random_value;
    char movie_name[64] = {0};
    FUNCTION_MARKER(0x80059030u, "SHELL.BIN");
    if (r_u8(0x8007BBC4u)) {
        if (r_u8(0x8007C22Cu + r_u32(0x8007BBCCu)) != 1u || (sint32)r_u32(0x8007BBC8u) >= 41) {
            w_u8(0x8007BBC4u, 0u);
            /* TODO Translate the fade transition service */
            moh_draft_missing_call(0x8004DE14u);
        }
        w_u32(0x8007BBC8u, r_u32(0x8007BBC8u) + 1u);
    }
    if (!r_u8(0x8007BB10u)) return;
    if (r_u32(0x8007BB14u) == 2u) {
        uint32 voice = r_u32(0x8007BBC0u);
        w_u8(0x8007C378u + voice, 1u);
        moh_draft_missing_call(0x8001E42Cu, (uint32)(sint32)(sint16)voice, 0u, 0u);
        w_u32(data + 20u, r_u32(screen + 28u));
        item = r_u32(screen + 52u);
        w_u8(screen + 117u, 1u);
        while (item) {
            /* TODO Translate the animation stop service */
            moh_draft_missing_call(0x8003CA30u, r_u32(item + 88u));
            item = r_u32(item + 136u);
        }
        if (r_u8(0x8007BBB1u)) {
            target = shell_title_find_screen(0x8003A7B0u, NULL);
            w_u32(0x8007BA68u, target);
            w_u8(0x8007BBB0u, r_u8(0x8007BBB0u) == 0u);
            return;
        }
        if (r_u8(0x8007BBB8u)) {
            w_u32(0x8007BA68u, shell_title_find_screen(0x8003A7C0u, NULL));
            return;
        }
        selected = r_u32(screen + 28u);
        index = r_u32(selected + 56u);
        if (index == 3u) {
            mission = r_u32(0x80038674u);
            if (mission == 6u) { mission = 7u; w_u32(0x80038674u, mission); }
            snprintf(movie_name, sizeof(movie_name), (const char *)moh_draft_guest_pointer(0x8003A7C8u, 1u), (sint32)(mission + 1u));
            w_u32(0x8007BA68u, shell_title_find_screen(0u, movie_name));
            w_u8(0x8007BFD0u, 1u);
            return;
        }
        if (index >= 2u) {
            w_u32(0x8007BA68u, r_u32(selected + 60u));
            return;
        }
        mission = r_u32(0x80038674u);
        if (!r_u32(0x80038D94u + mission * 12u)) {
            if (mission == 6u) { mission = 7u; w_u32(0x80038674u, mission); }
            snprintf(movie_name, sizeof(movie_name), (const char *)moh_draft_guest_pointer(0x8003A7C8u, 1u), (sint32)(mission + 1u));
            w_u32(0x8007BA68u, shell_title_find_screen(0u, movie_name));
        } else w_u32(0x8007BA68u, shell_title_find_screen(0x8003A7D4u, NULL));
        w_u8(0x8007BFD0u, 0u);
        /* TODO Translate the selected-level preparation service */
        moh_draft_missing_call(0x80062108u, 1u);
        return;
    }
    if ((sint16)r_u16(0x8007BBBAu) < 1) {
        random_value = (sint32)moh_draft_missing_call(0x8001552Cu);
        sample = (uint32)(random_value % 2 + 1);
        shell_title_play_sample(r_u32(data), sample);
        random_value = (sint32)moh_draft_missing_call(0x8001552Cu);
        w_u16(0x8007BBBAu, (uint16)(random_value % 300 + 150));
    } else if ((sint16)r_u16(0x8007BBBEu) < 1) {
        random_value = (sint32)moh_draft_missing_call(0x8001552Cu);
        sample = (uint32)(random_value % 2 + 8);
        saved_rate = r_u32(r_u32(data) + 4u);
        random_value = (sint32)moh_draft_missing_call(0x8001552Cu);
        w_u32(r_u32(data) + 4u, (uint32)(random_value % 49152 + 49152));
        shell_title_play_sample(r_u32(data), sample);
        random_value = (sint32)moh_draft_missing_call(0x8001552Cu);
        w_u32(r_u32(data) + 4u, saved_rate);
        w_u16(0x8007BBBEu, (uint16)(random_value % 300 + 150));
    } else if ((sint16)r_u16(0x8007BBBCu) < 1) {
        shell_title_play_sample(r_u32(data), 0u);
        random_value = (sint32)moh_draft_missing_call(0x8001552Cu);
        w_u16(0x8007BBBCu, (uint16)(random_value % 300 + 150));
    }
    w_u16(0x8007BBBAu, (uint16)(r_u16(0x8007BBBAu) - 1u));
    w_u16(0x8007BBBEu, (uint16)(r_u16(0x8007BBBEu) - 1u));
    w_u16(0x8007BBBCu, (uint16)(r_u16(0x8007BBBCu) - 1u));
    if (result == 5u) {
        w_u8(0x8007BBB8u, 1u);
        moh_draft_missing_call(0x8004DE14u);
    }
    if (result == 10u) { moh_draft_missing_call(0x8004DE14u); return; }
    if (result == 2u) {
        static const uint32 names[4] = {0x8003A7DCu, 0x8003A7E8u, 0x8003A7F4u, 0x8003A7FCu};
        static const uint32 samples[4] = {10u, 25u, 30u, 40u};
        for (index = 0u; index < 4u; ++index) {
            if (shell_title_name_equal(r_u32(r_u32(screen + 28u) + 60u), names[index], NULL)) break;
        }
        if (index < 4u) {
            moh_draft_missing_call(0x8004DE14u);
            shell_title_play_sample(r_u32(data), samples[index]);
        } else if (r_u32(r_u32(screen + 28u) + 56u) == 3u) {
            random_value = (sint32)moh_draft_missing_call(0x8001552Cu);
            w_u8(0x8007BBC4u, 1u);
            w_u32(0x8007BBC8u, 0u);
            shell_title_play_sample(r_u32(data), (uint32)(random_value % 3 + 50));
            w_u32(0x8007BBCCu, r_u32(0x8007C374u));
        } else {
            moh_draft_missing_call(0x8004DE14u);
            shell_title_play_sample(r_u32(data), 6u);
        }
    }
    if ((result == 3u || result == 4u) && screen) {
        uint32 end_field = result == 3u ? 36u : 32u;
        uint32 link_field = result == 3u ? 176u : 180u;
        if (!r_u32(screen + end_field)) return;
        selected = r_u32(screen + 28u);
        for (index = 0u; (sint32)index < (sint32)r_u32(screen + 40u); ++index) {
            selected = r_u32(selected + link_field);
            if (!selected) selected = r_u32(screen + end_field);
            if (!r_u8(selected + 81u)) {
                pointer = r_u32(screen + 44u);
                w_u32(screen + 28u, selected);
                if (pointer) w_u32(pointer, r_u32(selected + 56u));
                break;
            }
        }
    }
}

void shell_sub_80062108(uint32 mode)
{
    uint32 mission, variant, group, index;
    FUNCTION_MARKER(0x80062108u, "SHELL.BIN");
    w_u32(0x80039D1Cu, mode);
    if (mode != 3u) {
        if (mode == 2u) {
            w_u32(0x80038660u, 11u);
            w_u32(0x80038664u, r_u32(0x80039C74u));
            w_u32(0x80039D20u, r_u32(0x80039C58u));
        } else {
            mission = r_u32(0x80038674u);
            if (mission == 6u) { w_u32(0x80038674u, 7u); mission = 7u; }
            w_u32(0x80039C88u, mission);
            variant = r_u32(0x80038D94u + mission * 12u);
            w_u32(0x80039C84u, variant);
            w_u32(0x80038664u, variant);
            if (mission == 7u && variant == 2u) {
                w_u32(0x80038664u, 3u);
                for (index = 0u; index < 32u; index += 16u) {
                    uint32 words[4], word;
                    for (word = 0u; word < 4u; ++word) words[word] = r_u32(0x80038C60u + index + word * 4u);
                    for (word = 0u; word < 4u; ++word) w_u32(0x80038C84u + index + word * 4u, words[word]);
                }
                w_u32(0x80038CA4u, r_u32(0x80038C80u));
            }
            mission = r_u32(0x80038674u);
            group = r_u32(0x80038D90u + mission * 12u);
            variant = r_u32(0x80038664u);
            w_u32(0x80038660u, group);
            if ((sint16)r_u16(0x80038772u + group * 152u + variant * 36u) == -1)
                w_u32(0x800386A4u, r_u32(0x800386A4u) & 3u);
        }
    }
    for (index = 0u; index < 13u; ++index) {
        w_u16(0x80038D5Eu + index * 4u, 0u);
        w_u16(0x80038D5Cu + index * 4u, 0xFFFFu);
    }
    for (index = 0u; index < 6u; ++index) w_u8(0x80038D4Cu + index, 0u);
    w_u32(0x80038D58u, 0u);
    w_u32(0x80038D54u, 0u);
}

uint32 shell_sub_8004BAF0(uint32 screen)
{
    uint32 field, group, image, widget, texture, model;
    FUNCTION_MARKER(0x8004BAF0u, "SHELL.BIN");
    if (!screen) return (uint32)-2;
    for (field = 64u; field <= 68u; field += 4u) {
        group = r_u32(screen + field);
        image = group ? r_u32(group + 120u) : 0u;
        while (image) {
            texture = r_u32(image + 28u);
            if (texture) {
                if (shell_sub_8003D890(texture) < 0) return (uint32)-4;
                w_u32(image + 28u, 0u);
            }
            image = r_u32(image + 208u);
        }
    }
    for (field = 64u; field <= 68u; field += 4u) {
        group = r_u32(screen + field);
        widget = group ? r_u32(group + 144u) : 0u;
        while (widget) {
            model = r_u32(widget + 64u);
            if (model) {
                /* TODO Link the model destructor supplied by the cleanup cluster */
                moh_draft_missing_call(0x80053358u, model);
                w_u32(widget + 64u, 0u);
            }
            texture = r_u32(widget + 56u);
            if (texture) { shell_sub_8003D890(texture); w_u32(widget + 56u, 0u); }
            texture = r_u32(widget + 60u);
            if (texture) { shell_sub_8003D890(texture); w_u32(widget + 60u, 0u); }
            widget = r_u32(widget + 312u);
        }
    }
    return 0u;
}

static uint32 shell_brief_group(uint32 screen, uint32 wanted)
{
    uint32 group;
    if (!screen || (sint32)(r_u32(screen + 40u) - 1u) < (sint32)wanted) return 0u;
    group = r_u32(screen + 32u);
    while (group && r_u32(group + 56u) != wanted) group = r_u32(group + 180u);
    return group;
}

static uint32 shell_brief_image(uint32 group, uint32 wanted)
{
    uint32 image;
    if (!group || (sint32)(r_u32(group + 128u) - 1u) < (sint32)wanted) return 0u;
    image = r_u32(group + 120u);
    while (image && r_u32(image + 216u) != wanted) image = r_u32(image + 208u);
    return image;
}

void shell_sub_8003F068(uint32 screen)
{
    static const uint32 image_indices[3] = {0u, 2u, 1u};
    uint32 panel, position, group, image, mission, format;
    char filename[64];
    FUNCTION_MARKER(0x8003F068u, "SHELL.BIN");
    for (panel = 0u; panel < 3u; ++panel) {
        group = shell_brief_group(screen, panel);
        for (position = 0u; position < 3u; ++position) {
            image = shell_brief_image(group, image_indices[position]);
            mission = r_u32(0x80038674u);
            format = 0x80039E98u + (panel * 3u + position) * 24u;
            snprintf(filename, sizeof(filename), (const char *)moh_draft_guest_pointer(format, 1u),
                (sint32)(mission + 1u), (sint32)(r_u32(0x80038D94u + mission * 12u) + 1u));
            /* The original strcpy ignores a null destination */
            if (image) memcpy(moh_draft_guest_pointer(image, (uint32)strlen(filename) + 1u), filename, strlen(filename) + 1u);
        }
    }
}

void shell_sub_8003EF24(uint32 screen)
{
    uint32 data = r_u32(screen + 80u), mission = r_u32(0x80038674u), name, selected;
    char filename[64];
    FUNCTION_MARKER(0x8003EF24u, "SHELL.BIN");
    if (mission == 6u) { mission = 7u; w_u32(0x80038674u, mission); }
    snprintf(filename, sizeof(filename), (const char *)moh_draft_guest_pointer(0x80039E7Cu, 1u),
        (sint32)(mission + 1u), (sint32)(r_u32(0x80038D94u + mission * 12u) + 1u));
    shell_sub_8003F068(screen);
    /* Use a temporary resource name without recreating the guest stack */
    name = shell_sub_80045260(64u);
    if (!name) abort();
    memcpy(moh_draft_guest_pointer(name, (uint32)strlen(filename) + 1u), filename, strlen(filename) + 1u);
    shell_sub_8004E0BC(data, name);
    shell_sub_80045328(name);
    w_u8(0x8007A250u, 0u);
    selected = shell_brief_group(screen, 0u);
    w_u32(screen + 28u, selected);
}

void shell_sub_80060D1C(void)
{
    static const struct { uint32 format, value, kind; } settings[] = {
        {0x8003AB4Cu, 0x800385DBu, 1u}, {0x8003AB64u, 0x800386DCu, 2u},
        {0x8003AB74u, 0x8003864Cu, 0u}, {0x8003AB88u, 0x80038650u, 0u},
        {0x8003ABA0u, 0x80038654u, 0u}, {0x8003ABB8u, 0x80038660u, 0u},
        {0x8003ABC8u, 0x80038664u, 0u}, {0x8003ABD4u, 0x80038668u, 0u},
        {0x8003ABE0u, 0x800386A8u, 0u}, {0x8003ABFCu, 0x800386ACu, 0u},
        {0x8003AC14u, 0x800386B0u, 0u}, {0x8003AC2Cu, 0x800386BCu, 0u},
        {0x8003AC48u, 0x800386C0u, 0u}, {0x8003AC60u, 0x800386C4u, 0u},
        {0x8003AC78u, 0x800386D0u, 0u}, {0x8003AC8Cu, 0x800386D4u, 0u},
        {0x8003ACACu, 0x800386D8u, 0u}, {0x8003ACCCu, 0x800386DFu, 1u},
        {0x8003ACE8u, 0x800386E0u, 1u}, {0x8003AD08u, 0x800386E1u, 1u},
        {0x8003AD28u, 0x800386E2u, 1u}, {0x8003AD4Cu, 0x800386E3u, 1u},
        {0x8003AD70u, 0x800386E6u, 1u}, {0x8003AD94u, 0x800386E7u, 1u},
        {0x8003ADB8u, 0x800386EAu, 1u}, {0x8003ADD8u, 0x800386EBu, 1u},
        {0x8003ADF8u, 0x800386ECu, 1u}, {0x8003AE1Cu, 0x800386EDu, 1u},
        {0x8003AE40u, 0x800386F0u, 1u}, {0x8003AE64u, 0x800386F1u, 1u},
        {0x8003AE88u, 0x800386F4u, 1u}, {0x8003AEA4u, 0x800386F5u, 1u},
        {0x8003AEC4u, 0x800386F6u, 1u}, {0x8003AEE4u, 0x800386F7u, 1u},
        {0x8003AF08u, 0x800386F8u, 1u}, {0x8003AF2Cu, 0x800386FBu, 1u},
        {0x8003AF50u, 0x800386FCu, 1u}, {0x8003AF74u, 0x800386FFu, 1u},
        {0x8003AF94u, 0x80038700u, 1u}, {0x8003AFB4u, 0x80038701u, 1u},
        {0x8003AFD8u, 0x80038702u, 1u}, {0x8003AFFCu, 0x80038705u, 1u},
        {0x8003B020u, 0x80038706u, 1u}
    };
    PSX_RECT rectangle = {0, 0, 1024, 512};
    uint32 index;
    uint8 red, green, blue;
    FUNCTION_MARKER(0x80060D1Cu, "SHELL.BIN");
    if (!r_u32(0x80038664u) || r_u32(0x80039D1Cu) == 2u) {
        red = 152u; green = 80u; blue = 32u;
    } else {
        red = 229u; green = 138u; blue = 65u;
    }
    w_u8(0x8007BFD6u, red); w_u8(0x80039D27u, red);
    w_u8(0x8007BFD7u, green); w_u8(0x80039D28u, green);
    w_u8(0x8007BFD8u, blue); w_u8(0x80039D29u, blue);
    ClearImage(&rectangle, 0u, 0u, 0u);
    DrawSync(0);
    w_u8(0x8007C010u, 0u);
    w_u32(0x8007C020u, 24u);
    shell_sub_8003D890(r_u32(0x8007C018u));
    shell_sub_8003D890(r_u32(0x8007C01Cu));
    w_u32(0x8007C01Cu, 0u); w_u32(0x8007C018u, 0u);
    shell_sub_8006118C(0u);
    /* TODO Link the remaining shutdown services through their native adapters */
    /* Stop playback before the final shell SPU cleanup */
    SsEnd();
    {
        uint32 bank = r_u32(0x8007BFB4u);
        if (bank) {
            w_u8(0x8007C21Cu + (uint32)(sint32)(sint16)r_u16(bank), 1u);
            shell_sub_80045328(bank);
        }
    }
    moh_draft_missing_call(0x800630CCu);
    moh_draft_missing_call(0x8005F5E4u, r_u32(0x8007BFBCu));
    shell_sub_8006118C(3u);
    moh_draft_missing_call(0x800492D0u);
    moh_draft_missing_call(0x80063124u);
    moh_draft_missing_call(0x8001B574u);
    moh_draft_missing_call(0x800453F8u);
    moh_draft_missing_call(0x8001D180u);
    shell_sub_8006118C(3u);
    shell_sub_8003D890(r_u32(0x8007C018u));
    w_u32(0x8007C018u, 0u);
    shell_sub_8003D890(r_u32(0x8007C01Cu));
    w_u32(0x8007C01Cu, 0u);
    moh_draft_missing_call(0x8005FB50u, 0u);
    moh_draft_missing_call(0x8003D03Cu);
    moh_draft_missing_call(0x80045234u);
    printf("%s", (const char *)moh_draft_guest_pointer(0x8003AAF4u, 1u));
    printf("%s", (const char *)moh_draft_guest_pointer(0x8003AB20u, 1u));
    for (index = 0u; index < sizeof(settings) / sizeof(settings[0]); ++index) {
        sint32 value = settings[index].kind == 0u ? (sint32)r_u32(settings[index].value) :
            settings[index].kind == 1u ? (sint32)r_u8(settings[index].value) : (sint32)(sint8)r_u8(settings[index].value);
        printf((const char *)moh_draft_guest_pointer(settings[index].format, 1u), value);
    }
}

uint32 shell_sub_800496A0(uint32 group)
{
    uint32 owner, callback, child, object, previous, next;
    FUNCTION_MARKER(0x800496A0u, "SHELL.BIN");
    if (!group) return (uint32)-2;
    callback = r_u32(group + 68u);
    owner = r_u32(group + 184u);
    if (callback) moh_draft_indirect_call(callback, group);
    while ((child = r_u32(group + 100u)) != 0u) {
        /* TODO Link the child layout destructors through the native dispatcher */
        if ((sint32)moh_draft_missing_call(0x80049A6Cu, child) < 0) return (uint32)-4;
    }
    while ((child = r_u32(group + 120u)) != 0u) {
        if ((sint32)moh_draft_missing_call(0x80049B54u, child) < 0) return (uint32)-4;
    }
    while ((child = r_u32(group + 144u)) != 0u) {
        if ((sint32)moh_draft_missing_call(0x80049C60u, child) < 0) return (uint32)-4;
    }
    object = r_u32(group + 112u);
    if (object) {
        child = r_u32(object + 28u);
        if (child) shell_sub_8003D890(child);
        shell_sub_80045328(r_u32(group + 112u));
        w_u32(group + 112u, 0u);
    }
    object = r_u32(group + 136u);
    if (object) {
        child = r_u32(object + 64u);
        if (child) shell_sub_80053358(child);
        object = r_u32(group + 136u);
        child = r_u32(object + 56u);
        if (child) { shell_sub_8003D890(child); object = r_u32(group + 136u); }
        child = r_u32(object + 60u);
        if (child) shell_sub_8003D890(child);
        shell_sub_80045328(r_u32(group + 136u));
        w_u32(group + 136u, 0u);
    }
    previous = r_u32(group + 180u);
    if (previous) {
        w_u32(previous + 176u, r_u32(group + 176u));
        next = r_u32(group + 176u);
        if (!next) goto update_owner;
        w_u32(next + 180u, r_u32(group + 180u));
    }
    next = r_u32(group + 176u);
    if (next) {
        w_u32(next + 180u, r_u32(group + 180u));
        previous = r_u32(group + 180u);
        if (previous) w_u32(previous + 176u, r_u32(group + 176u));
    }
update_owner:
    if (r_u32(owner + 32u) == group) w_u32(owner + 32u, r_u32(group + 180u));
    if (r_u32(owner + 36u) == group) w_u32(owner + 36u, r_u32(group + 176u));
    shell_sub_80045328(group);
    w_u32(owner + 40u, r_u32(owner + 40u) - 1u);
    return 0u;
}

uint32 shell_sub_800498E8(uint32 item)
{
    uint32 owner, object, previous, next;
    FUNCTION_MARKER(0x800498E8u, "SHELL.BIN");
    if (!item) return (uint32)-2;
    object = r_u32(item + 100u);
    owner = r_u32(item + 140u);
    if (object) {
        if (shell_sub_8003D890(object) < 0) return (uint32)-4;
        w_u32(item + 100u, 0u);
    }
    object = r_u32(item + 104u);
    if (object) {
        if (shell_sub_8003D890(object) < 0) return (uint32)-4;
        w_u32(item + 104u, 0u);
    }
    object = r_u32(item + 92u);
    if (object) {
        if ((sint32)shell_sub_80053358(object) < 0) return (uint32)-4;
        w_u32(item + 92u, 0u);
    }
    object = r_u32(item + 88u);
    if (object) {
        /* TODO Link the animation destructor through the native dispatcher */
        if ((sint32)moh_draft_missing_call(0x8003BE8Cu, object) < 0) return (uint32)-4;
        w_u32(item + 88u, 0u);
    }
    previous = r_u32(item + 136u);
    if (previous) {
        w_u32(previous + 132u, r_u32(item + 132u));
        next = r_u32(item + 132u);
        if (!next) goto update_owner;
        w_u32(next + 136u, r_u32(item + 136u));
    }
    next = r_u32(item + 132u);
    if (next) {
        w_u32(next + 136u, r_u32(item + 136u));
        previous = r_u32(item + 136u);
        if (previous) w_u32(previous + 132u, r_u32(item + 132u));
    }
update_owner:
    if (r_u32(owner + 52u) == item) w_u32(owner + 52u, r_u32(item + 136u));
    if (r_u32(owner + 56u) == item) w_u32(owner + 56u, r_u32(item + 132u));
    shell_sub_80045328(item);
    w_u32(owner + 60u, r_u32(owner + 60u) - 1u);
    return 0u;
}

uint32 shell_sub_8003C028(uint32 frame)
{
    uint32 owner, previous, next;
    FUNCTION_MARKER(0x8003C028u, "SHELL.BIN");
    if (!frame) return (uint32)-2;
    next = r_u32(frame + 4u);
    owner = r_u32(frame + 12u);
    if (next) {
        w_u32(next + 8u, r_u32(frame + 8u));
        previous = r_u32(frame + 8u);
        if (!previous) goto update_owner;
        w_u32(previous + 4u, r_u32(frame + 4u));
    }
    previous = r_u32(frame + 8u);
    if (previous) {
        w_u32(previous + 4u, r_u32(frame + 4u));
        next = r_u32(frame + 4u);
        if (next) w_u32(next + 8u, r_u32(frame + 8u));
    }
update_owner:
    if (r_u32(owner + 64u) == frame) w_u32(owner + 64u, r_u32(frame + 4u));
    if (r_u32(owner + 68u) == frame) w_u32(owner + 68u, r_u32(frame + 8u));
    shell_sub_80045328(frame);
    w_u32(owner + 76u, r_u32(owner + 76u) - 1u);
    return 0u;
}

uint32 shell_sub_8003BE8C(uint32 animation)
{
    uint32 frame;
    FUNCTION_MARKER(0x8003BE8Cu, "SHELL.BIN");
    if (!animation) return (uint32)-2;
    while ((frame = r_u32(animation + 64u)) != 0u) {
        if ((sint32)shell_sub_8003C028(frame) < 0) return (uint32)-4;
    }
    shell_sub_80045328(animation);
    return 0u;
}
