#include "game_shell_signatures.h"
#include "game_level_signatures.h"
#include "xport_trace.h"
#include "psx.h"
#include <stdlib.h>

void level_sub_80075034(void)
{
    FUNCTION_MARKER(0x80075034u, "LEVEL.BIN");
    while (level_sub_80074F9C()) {}
}

uint32 level_sub_800662EC(void)
{
    uint32 count;
    FUNCTION_MARKER(0x800662ECu, "LEVEL.BIN");
    count = r_u32(0x8009D5BCu) - r_u32(0x8009D5B8u);
    return (sint32)count < 0 ? count + 128u : count;
}

uint32 level_sub_800663B8(void)
{
    uint32 count, index, next;
    FUNCTION_MARKER(0x800663B8u, "LEVEL.BIN");
    count = level_sub_800662EC();
    for (index = 0u; (sint32)index < (sint32)count; ++index) {
        level_sub_80064858(0x8003C920u, index + 1u, count);
        if (level_sub_8006A204(0x800B6460u + r_u32(0x8009D5B8u) * 32u))
            level_sub_800672E0(0x800B6460u + r_u32(0x8009D5B8u) * 32u);
        next = r_u32(0x8009D5B8u) + 1u;
        w_u32(0x8009D5B8u, (sint32)next < 128 ? next : 0u);
    }
    return r_u32(0x8009D5B8u) == r_u32(0x8009D5BCu);
}

void level_sub_8005439C(void)
{
    uint32 slot, local, value;
    FUNCTION_MARKER(0x8005439Cu, "LEVEL.BIN");
    slot = r_u32(0x800AA184u);
    if (slot != 0xFFFFFFFFu) {
        if (!moh_draft_missing_call(0x80054238u, r_u32(0x80098970u + slot * 4u))) return;
        moh_draft_missing_call(0x80015B28u, 0x8003C124u);
    }
    w_u32(0x8009896Cu, r_u32(0x80099070u));
    moh_draft_missing_call(0x80054100u); moh_draft_missing_call(0x80054298u);
    value = r_u32(0x80098964u);
    if (!value) return;
    if (value != r_u32(0x80098968u)) moh_draft_missing_call(0x80089B04u, 5u, 32u);
    slot = r_u32(0x80099084u); moh_draft_missing_call(0x8008C8F4u, slot);
    local = sub_80016718(64u);
    if (!local) moh_draft_fail_fast("Texture request filename allocation", 0x8005439Cu);
    moh_draft_missing_call(0x8001BE18u, local, 0x8003C118u, r_u32(0x8009CC64u), r_u32(0x80099074u));
    moh_draft_missing_call(0x8008C718u, local, slot);
    w_u32(0x80098970u + slot * 4u, r_u32(0x80099074u)); sub_80036E44(local);
}

void level_sub_8006DA88(void)
{
    uint32 count, index, object, address, first;
    FUNCTION_MARKER(0x8006DA88u, "LEVEL.BIN");
    count = r_u32(0x8009D674u);
    for (index = 0u; index < count; ++index) {
        if (!r_u32(0x8009DE78u + index * 4u)) continue;
        object = r_u32(0x8009D678u + index * 4u); if (!object) continue;
        first = r_u32(0x8009D628u);
        w_u32(0x8009D678u + index * 4u, 0u); w_u32(0x8009DA78u + index * 4u, 0u);
        w_u32(0x8009DE78u + index * 4u, 0u);
        if (object == first) w_u32(0x8009D628u, 0u);
        for (address = 0x8009D62Cu; address <= 0x8009D658u; address += 4u)
            if (r_u32(address) == object) w_u32(address, 0u);
        for (address = 0x8009D660u; address < 0x8009D670u; address += 4u)
            if (r_u32(address) == object) w_u32(address, 0u);
    }
    while ((count = r_u32(0x8009D674u)) != 0u) {
        if (r_u32(0x8009D678u + (count - 1u) * 4u)) break;
        w_u32(0x8009D674u, count - 1u);
    }
}

uint32 level_sub_80074F9C(void)
{
    uint32 count, index, request, next;
    FUNCTION_MARKER(0x80074F9Cu, "LEVEL.BIN");
    count = r_u32(0x8009E288u);
    if (!count) return 0u;
    index = r_u32(0x8009E280u); request = r_u32(0x8009E294u + index * 4u);
    next = index + 1u; w_u32(0x8009E280u, next); w_u32(0x8009E294u + index * 4u, 0u);
    if (next >= 64u) w_u32(0x8009E280u, index - 63u);
    w_u32(0x8009E288u, count - 1u);
    if (count == 1u) { w_u32(0x8009E280u, 0u); w_u32(0x8009E284u, 0u); }
    moh_draft_missing_call(0x8007505Cu, request);
    return r_u32(0x8009E288u) < count;
}

static void moh_geometry_vertices(uint32 vertices, uint32 packed)
{
    uint32 first = vertices + (packed & 0x3FFu) * 8u;
    uint32 second = vertices + ((packed >> 7u) & 0x1FF8u);
    uint32 third = vertices + ((packed >> 17u) & 0x1FF8u);
    xport_gte_write_data(0u, r_u32(first)); xport_gte_write_data(1u, r_u32(first + 4u));
    xport_gte_write_data(2u, r_u32(second)); xport_gte_write_data(3u, r_u32(second + 4u));
    xport_gte_write_data(4u, r_u32(third)); xport_gte_write_data(5u, r_u32(third + 4u));
}

uint32 level_sub_8008080C(uint32 render, uint32 ordering_table, uint32 buffer_index,
    uint32 alternate, uint32 depth_out, uint32 direct)
{
    uint32 vertices, colors, packet, local, end, scale, code, table, triangle, count, owner, handle;
    uint32 packed, index, depth, parent, bucket, color, mask, drawn = 0u, ordering, tag;
    sint32 shade;
    PsxGteSnapshot snapshot;
    FUNCTION_MARKER(0x8008080Cu, "LEVEL.BIN");
    vertices = r_u32(render + 0x2Cu); shade = (sint16)r_u16(r_u32(0x8009CC64u) + 0xCCu);
    colors = r_u32(render + 0x48u); code = r_u32(render + 0x68u) ? 0x36u : 0x34u;
    xport_gte_write_data(6u, code << 24u);
    mask = r_u32(0x8009CC64u);
    w_u32(0x1F8001B0u, mask + 0xDCu); w_u32(0x1F8001C0u, 0u);
    w_u32(0x1F8001C4u, 0u); w_u32(0x1F8001C8u, 0xFFFFFFu);
    w_u32(0x1F8001ACu, r_u32(mask + 0xD8u) != 0u);
    local = sub_80016718(8u);
    if (!local) moh_draft_fail_fast("Geometry renderer semantic locals allocation", 0x8008080Cu);
    packet = level_sub_80053BD4(buffer_index, local); end = r_u32(local) - 40u; w_u32(local, end);
    scale = level_sub_80080480(render, local + 4u, direct); scale = direct ? 4u : scale + 1u;
    w_u32(0x1F8001A8u, 0xFFFFu); w_u32(0x1F8001D0u, ordering_table);
    table = r_u32(render + (alternate ? 0x28u : 0x20u)); triangle = table + 4u;
    count = r_u32(table); packed = r_u32(triangle + 12u); moh_geometry_vertices(vertices, packed);
    handle = r_u32(render + 0x54u); owner = handle ? r_u32(0x8009D678u + (handle & 0xFFFFu) * 4u) : 0u;
    while ((sint32)count > 0 && packet < r_u32(local)) {
        xport_gte_execute(0x280030u);
        tag = r_u32(triangle); color = r_u32(triangle + 4u); packed = r_u32(triangle + 8u);
        w_u32(packet + 12u, tag); w_u32(packet + 24u, color); w_u32(packet + 36u, packed);
        psx_gte_snapshot(&snapshot); w_u32(0x1F800190u, (uint32)snapshot.flag);
        xport_gte_execute(0x1400006u); moh_geometry_vertices(vertices, r_u32(triangle + 28u));
        if (!(r_u32(0x1F800190u) & 0x7FFD8000u) && (sint32)xport_gte_read_data(24u) > 0) {
            xport_gte_execute(0x158002Du); depth = xport_gte_read_data(7u);
            parent = 0u;
            if (owner) {
                parent = r_u32(render + 0x58u);
                if (!parent) {
                    handle = r_u32(render + 0x54u);
                    parent = handle ? r_u32(0x8009D678u + (handle & 0xFFFFu) * 4u) : 0u;
                    parent = r_u32(parent + 0x100u);
                }
            }
            if (owner && r_u32(parent + 0x74u)) {
                bucket = (depth << 2u) - (uint32)(sint32)(sint16)r_u16(parent + 0xA8u);
                w_u32(0x1F800198u, bucket);
                if ((sint32)bucket < 0) bucket = 0u;
                else bucket = (uint32)((sint32)bucket >> (r_u32(parent + 0xACu) & 31u));
            } else bucket = ((depth << 2u) - r_u32(local + 4u) + r_u32(0x1F8001C4u)) >> (scale & 31u);
            w_u32(0x1F800198u, bucket);
            if ((sint32)bucket >= 0) {
                if ((sint32)bucket >= 512) w_u32(0x1F800198u, 509u);
                w_u32(packet + 8u, xport_gte_read_data(12u)); w_u32(packet + 20u, xport_gte_read_data(13u));
                w_u32(packet + 32u, xport_gte_read_data(14u)); packed = r_u32(triangle + 12u);
                tag = r_u32(colors + (packed & 0x3FFu) * 4u);
                color = r_u32(colors + ((packed >> 8u) & 0xFFCu));
                depth = r_u32(colors + ((packed >> 18u) & 0xFFCu));
                w_u32(0x1F8001B4u, tag); w_u32(0x1F8001B8u, color); w_u32(0x1F8001BCu, depth);
                if (!alternate && r_u32(0x1F8001ACu)) {
                    mask = r_u32(0x1F8001C8u);
                    for (index = 0u; index < 3u; ++index) {
                        color = r_u32(0x1F8001B4u + index * 4u);
                        if ((color & mask) < 40u) {
                            tag = r_u32(r_u32(0x1F8001B0u) + (color & mask) * 20u + 12u);
                            w_u32(0x1F8001B4u + index * 4u, (color & 0xFF000000u) | (tag & mask));
                        }
                    }
                }
                if (owner) {
                    bucket = r_u32(0x1F800198u) - 15u; w_u32(0x1F800198u, bucket);
                    if ((sint32)bucket <= 0) w_u32(0x1F800198u, 1u);
                }
                if (shade) {
                    xport_gte_write_data(20u, r_u32(0x1F8001B4u)); xport_gte_write_data(21u, r_u32(0x1F8001B8u));
                    color = r_u32(0x1F8001BCu); xport_gte_write_data(22u, color); xport_gte_write_data(6u, color);
                    xport_gte_execute(0xF8002Au); ++drawn;
                    ordering = r_u32(0x1F8001D0u) + r_u32(0x1F800198u) * 4u;
                    w_u32(packet, r_u32(ordering) | 0x9000000u); w_u32(ordering, packet & 0xFFFFFFu);
                    w_u32(packet + 4u, xport_gte_read_data(20u)); w_u32(packet + 16u, xport_gte_read_data(21u));
                    w_u32(packet + 28u, xport_gte_read_data(22u)); w_u8(packet + 7u, (uint8)code);
                } else {
                    ++drawn; tag = r_u32(0x1F8001B4u); color = r_u32(0x1F8001B8u); depth = r_u32(0x1F8001BCu);
                    w_u32(packet + 4u, tag); w_u32(packet + 16u, color); w_u32(packet + 28u, depth); w_u8(packet + 7u, (uint8)code);
                    ordering = r_u32(0x1F8001D0u) + r_u32(0x1F800198u) * 4u;
                    w_u32(packet, r_u32(ordering) | 0x9000000u); w_u32(ordering, packet & 0xFFFFFFu);
                }
                packet += 40u;
            }
        }
        --count; triangle += 16u;
    }
    if (depth_out) w_u32(depth_out, r_u32(local + 4u));
    level_sub_80053BF4(packet); sub_80036E44(local); return (sint32)drawn > 0;
}

static void moh_visibility_corner(uint32 camera, uint32 plane, uint32 minimum, uint32 maximum, uint32 point[2])
{
    uint32 signs[3], x, y, z;
    if (plane == 0x50u) {
        signs[1] = r_u32(camera + plane + 4u); signs[0] = r_u32(camera + plane);
    } else {
        signs[0] = r_u32(camera + plane); signs[1] = r_u32(camera + plane + 4u);
    }
    signs[2] = r_u32(camera + plane + 8u);
    x = r_u16((sint32)signs[0] < 0 ? minimum : maximum);
    y = r_u16(((sint32)signs[1] < 0 ? minimum : maximum) + 2u);
    z = r_u16(((sint32)signs[2] < 0 ? minimum : maximum) + 4u);
    point[0] = x | (y << 16u); point[1] = z;
}

static void moh_visibility_project(const uint32 point[2])
{
    xport_gte_write_data(0u, point[0]); xport_gte_write_data(1u, point[1]);
    xport_gte_execute(0x180001u);
}

uint32 level_sub_8008E5B0(uint32 minimum, uint32 maximum)
{
    uint32 camera, point[2], index, screen, depth;
    PsxGteSnapshot snapshot;
    FUNCTION_MARKER(0x8008E5B0u, "LEVEL.BIN");
    camera = r_u32(r_u32(r_u32(0x8009D638u) + 0x388u) + 200u);
    for (index = 0u; index < 8u; ++index)
        xport_gte_write_control(index, r_u32(0x8009C804u + index * 4u));
    moh_visibility_corner(camera, 0x50u, minimum, maximum, point); moh_visibility_project(point);
    moh_visibility_corner(camera, 0x6Cu, minimum, maximum, point);
    psx_gte_snapshot(&snapshot);
    if ((uint32)snapshot.flag & 0x40000u) return 0u;
    moh_visibility_project(point); moh_visibility_corner(camera, 0x88u, minimum, maximum, point);
    screen = xport_gte_read_data(14u);
    if ((sint16)(uint16)screen < 0) return 0u;
    moh_visibility_project(point); moh_visibility_corner(camera, 0xA4u, minimum, maximum, point);
    screen = xport_gte_read_data(14u);
    if ((sint32)(sint16)(uint16)screen > (sint32)r_u32(0x8009C828u)) return 0u;
    moh_visibility_project(point); moh_visibility_corner(camera, 0xC0u, minimum, maximum, point);
    screen = xport_gte_read_data(14u);
    if ((sint16)(uint16)(screen >> 16u) < 0) return 0u;
    moh_visibility_project(point); moh_visibility_corner(camera, 0xDCu, minimum, maximum, point);
    screen = xport_gte_read_data(14u);
    if ((sint32)(sint16)(uint16)(screen >> 16u) > (sint32)r_u32(0x8009C82Cu)) return 0u;
    moh_visibility_project(point); depth = xport_gte_read_data(19u);
    return (sint32)(depth << 16u) <= (sint32)r_u32(camera + 64u);
}

uint32 level_sub_8008E9DC(uint32 object)
{
    uint32 local, axis, coordinate, radius, result;
    FUNCTION_MARKER(0x8008E9DCu, "LEVEL.BIN");
    local = sub_80016718(16u);
    if (!local) moh_draft_fail_fast("Object visibility bounds allocation", 0x8008E9DCu);
    for (axis = 0u; axis < 3u; ++axis) {
        coordinate = r_u16(object + 0x2E0u + axis * 2u); radius = r_u16(object + 0x2E8u);
        w_u16(local + axis * 2u, (uint16)(coordinate - radius));
    }
    for (axis = 0u; axis < 3u; ++axis) {
        coordinate = r_u16(object + 0x2E0u + axis * 2u); radius = r_u16(object + 0x2E8u);
        w_u16(local + 8u + axis * 2u, (uint16)(coordinate + radius));
    }
    result = moh_draft_missing_call(0x8008E5B0u, local, local + 8u) != 0u;
    sub_80036E44(local); return result;
}

uint32 level_sub_800460F4(uint32 object)
{
    uint32 state, flags, type, weapon, slot, capacity, loaded, difference, reserve, sound_flags;
    sint32 amount, sound, bank;
    FUNCTION_MARKER(0x800460F4u, "LEVEL.BIN");
    state = r_u32(object + 0x388u); flags = r_u32(0x800386A4u) & 0x40u;
    type = r_u8(state + 0x55u); weapon = 0x80093640u + type * 68u;
    slot = state + (uint32)((sint32)(sint8)r_u8(state + 0x54u) * 2);
    capacity = r_u16(weapon + 10u); loaded = r_u16(slot + 0x66u);
    difference = capacity - loaded; amount = (sint32)(sint16)(uint16)difference;
    if (amount <= 0) return 1u;
    if (flags) w_u16(slot + 0x66u, (uint16)capacity);
    else {
        reserve = r_u16(slot + 0x56u);
        if (amount < (sint32)(sint16)(uint16)reserve) {
            w_u16(slot + 0x66u, (uint16)capacity);
            slot = state + (uint32)((sint32)(sint8)r_u8(state + 0x54u) * 2);
            w_u16(slot + 0x56u, (uint16)(r_u16(slot + 0x56u) - difference));
        } else {
            if ((sint32)(sint16)(uint16)reserve <= 0) return (sint32)(loaded << 16u) > 0;
            w_u16(slot + 0x66u, (uint16)(loaded + reserve));
            slot = state + (uint32)((sint32)(sint8)r_u8(state + 0x54u) * 2);
            w_u16(slot + 0x56u, 0u);
        }
    }
    w_u32(0x80039CBCu, r_u32(0x80039CBCu) + 1u); type = r_u8(state + 0x55u);
    if (type == 3u || type == 10u) {
        slot = state + (uint32)((sint32)(sint8)r_u8(state + 0x54u) * 2);
        if (r_u16(slot + 0x66u)) moh_draft_missing_call(0x8005C97Cu, object, 16u, 0u);
    } else if (!r_u32(state + 0x13Cu)) {
        moh_draft_missing_call(0x8003F454u, object, 9u, 1u, 0u);
        weapon = 0x80093640u + (uint32)r_u8(state + 0x55u) * 68u;
        sound = (sint32)(sint16)r_u16(weapon + 32u);
        if (sound >= 0) {
            sound_flags = (r_u16(weapon + 42u) >> 3u) & 2u;
            bank = (sint32)(sint16)r_u16(weapon + 34u);
            if (bank < 0) bank = (sint32)(sint16)r_u16(r_u32(r_u32(object + 0x100u) + 8u) + 200u);
            moh_draft_missing_call(0x80041CE8u, (uint32)bank, (uint32)sound, object, 0x20000u,
                0u, 0x7FFFu, 0u, 0u, 0u, 0u, sound_flags);
        }
    }
    moh_draft_missing_call(0x800467ECu, object); return 1u;
}

void level_sub_80046A88(uint32 object)
{
    uint32 first, second, value;
    FUNCTION_MARKER(0x80046A88u, "LEVEL.BIN");
    first = moh_draft_missing_call(0x8005CBA8u, 2u, object);
    second = moh_draft_missing_call(0x8005CBA8u, 6u, object);
    if ((sint32)second < (sint32)first) moh_draft_missing_call(0x8006B8F0u, object);
    else {
        moh_draft_missing_call(0x8003F8F0u, object);
        value = moh_draft_missing_call(0x8005CBA8u, 3u, object); w_u32(object + 0x234u, value << 16u);
        value = moh_draft_missing_call(0x8005CBA8u, 4u, object); w_u32(object + 0x238u, value << 16u);
        value = moh_draft_missing_call(0x8005CBA8u, 5u, object); w_u32(object + 0x23Cu, value << 16u);
    }
}

static uint32 moh_sound_owner(uint32 handle_address)
{
    uint32 handle = r_u32(handle_address);
    return handle ? r_u32(0x8009D678u + (handle & 0xFFFFu) * 4u) : 0u;
}

void level_sub_8004137C(void)
{
    uint32 local, bits, index, row, handle, owner, age, fade, status, shift, high, left, right, counter, limit, product, step;
    sint32 divisor;
    FUNCTION_MARKER(0x8004137Cu, "LEVEL.BIN");
    local = sub_80016718(8u);
    if (!local) moh_draft_fail_fast("Sound channel semantic locals allocation", 0x8004137Cu);
    w_u32(local + 4u, 0u);
    moh_draft_missing_call(0x80015A9Cu, 0x800910D0u);
    if (r_u32(0x80090FC8u)) {
        moh_draft_missing_call(0x800158DCu, 1u, r_u32(0x80090FC8u)); index = 0u;
        while (r_u32(0x80090FC8u)) {
            if (r_u32(0x80090FC8u) & 1u) w_u8(0x800910D0u + index, 1u);
            bits = r_u32(0x80090FC8u) >> 1u; ++index; w_u32(0x80090FC8u, bits);
        }
        w_u32(0x80090FC8u, 0u);
    }
    for (index = 1u; index < 24u; ++index) {
        row = 0x8009115Cu + index * 52u; handle = row + 4u;
        status = r_u8(0x800910D0u + index);
        if (!(status & 1u)) {
            if (!r_u8(row + 14u)) {
                moh_draft_missing_call(0x8006D84Cu, row + 4u, 0u);
                w_u8(row + 18u, 0u); w_u16(row + 40u, 0u);
            } else {
                w_u8(row + 14u, 0u); owner = moh_sound_owner(handle);
                moh_draft_missing_call(0x80042250u, (uint32)(sint32)(sint16)r_u16(row + 12u),
                    r_u32(row + 8u), owner, r_u32(row + 24u), r_u32(row + 28u),
                    r_u32(row + 32u), r_u32(row + 36u), 0u, 1u, index, 0u, 0u);
            }
            continue;
        }
        age = (r_u16(row + 40u) + 1u) & 0xFFFFu; w_u16(row + 40u, (uint16)age);
        if (age >= 351u && !r_u8(row + 14u)) {
            moh_draft_missing_call(0x800425C4u, index); continue;
        }
        if (!moh_draft_missing_call(0x8006D8ACu, handle)) {
            owner = moh_sound_owner(handle);
            if (r_u32(owner + 12u) & 0x80000u) moh_draft_missing_call(0x8006D84Cu, handle, 0u);
            else {
                owner = moh_sound_owner(handle); w_u16(row + 44u, r_u16(owner + 0x20Eu));
                owner = moh_sound_owner(handle); w_u16(row + 46u, r_u16(owner + 0x212u));
                owner = moh_sound_owner(handle); w_u16(row + 48u, r_u16(owner + 0x216u));
            }
        }
        status = r_u8(0x800910D0u + index); fade = r_u8(row + 18u);
        if (status == 3u && !fade) {
            w_u32(local + 4u, r_u32(local + 4u) | (1u << (index & 31u))); continue;
        }
        if (fade) {
            moh_draft_missing_call(0x80015B88u, index, local, local + 2u);
            fade = r_u8(row + 18u);
            high = (uint32)(((uint64)fade * 0x24924925u) >> 32u);
            shift = (((high + ((fade - high) >> 1u)) >> 2u) & 0xFFu) + 3u;
            left = r_u16(local); right = r_u16(local + 2u);
            left -= (uint32)((sint32)(sint16)(uint16)left >> (shift & 31u));
            right -= (uint32)((sint32)(sint16)(uint16)right >> (shift & 31u));
            w_u16(local, (uint16)left); w_u16(local + 2u, (uint16)right);
            if ((sint32)(left << 16u) < 0) w_u16(local, 0u);
            if ((sint32)(right << 16u) < 0) w_u16(local + 2u, 0u);
            moh_draft_missing_call(0x80015C08u, index, (uint32)(sint32)(sint16)r_u16(local),
                (uint32)(sint32)(sint16)r_u16(local + 2u));
            fade = (r_u8(row + 18u) - 1u) & 0xFFu; w_u8(row + 18u, (uint8)fade);
            if (!fade) moh_draft_missing_call(0x80042664u, index, local + 4u);
        } else if (!(r_u32(row) & 1u) && r_u32(0x80090FCCu) == 3u) {
            moh_draft_missing_call(0x80042AA4u, index, local, local + 2u, (uint32)r_u8(row + 19u));
            counter = r_u8(row + 16u); limit = r_u8(row + 15u);
            if (counter < limit) {
                counter = (counter + 1u) & 0xFFu; limit = r_u8(row + 15u);
                w_u8(row + 16u, (uint8)counter); product = limit * counter;
                if (!product) product = 1u;
                divisor = (sint32)(sint16)(uint16)product;
                if (!divisor) moh_draft_fail_fast("Sound volume division trap", 0x8004137Cu);
                left = (uint32)((sint32)(sint16)r_u16(local) / divisor);
                right = (uint32)((sint32)(sint16)r_u16(local + 2u) / divisor);
                w_u16(local, (uint16)left); w_u16(local + 2u, (uint16)right);
            }
            moh_draft_missing_call(0x80015C08u, index, (uint32)(sint32)(sint16)r_u16(local),
                (uint32)(sint32)(sint16)r_u16(local + 2u));
        }
    }
    step = r_u32(0x80090FCCu); w_u32(0x80090FCCu, step + 1u);
    if ((sint32)step >= 3) w_u32(0x80090FCCu, 0u);
    if (r_u32(local + 4u)) moh_draft_missing_call(0x800158DCu, 0u, r_u32(local + 4u));
    sub_80036E44(local);
}

uint32 level_sub_8005959C(uint32 object, uint32 data, uint32 frame)
{
    uint32 random, value, special = 0u, mode, lower, upper, width, selected;
    FUNCTION_MARKER(0x8005959Cu, "LEVEL.BIN");
    random = moh_draft_missing_call(0x8001552Cu);
    random = (uint32)((sint32)random % 100);
    value = moh_draft_missing_call(0x8005D148u, 4u, frame, (uint32)r_u8(data + 1u), object);
    if ((sint32)value < (sint32)random)
        special = moh_draft_missing_call(0x8005D148u, 4u, frame, (uint32)r_u8(data + 1u), object) != 0u;
    if (special) {
        mode = moh_draft_missing_call(0x8005D148u, 1u, frame, (uint32)r_u8(data), object);
        selected = 0xFFFFFFFFu;
    } else {
        lower = moh_draft_missing_call(0x8005D148u, 2u, frame, (uint32)(sint32)(sint16)r_u16(data + 4u), object);
        upper = moh_draft_missing_call(0x8005D148u, 3u, frame, (uint32)(sint32)(sint16)r_u16(data + 6u), object);
        mode = moh_draft_missing_call(0x8005D148u, 1u, frame, (uint32)r_u8(data), object);
        random = moh_draft_missing_call(0x8001552Cu); width = upper - lower + 1u;
        if (!width || (width == 0xFFFFFFFFu && random == 0x80000000u))
            moh_draft_fail_fast("Animation random range division trap", 0x8005959Cu);
        selected = lower + (uint32)((sint32)random % (sint32)width);
    }
    moh_draft_missing_call(0x8005C97Cu, object, mode, selected);
    return data + 8u;
}

uint32 level_sub_80058734(uint32 object, uint32 data, uint32 frame)
{
    uint32 value, flags;
    FUNCTION_MARKER(0x80058734u, "LEVEL.BIN");
    if (!(r_u32(data) & 4u)) {
        value = moh_draft_missing_call(0x8005D148u, 1u, frame, r_u32(data + 4u), object);
        w_u32(object + 0x200u, value);
    }
    if (!(r_u32(data) & 8u)) {
        value = moh_draft_missing_call(0x8005D148u, 2u, frame, r_u32(data + 8u), object);
        w_u32(object + 0x21Cu, value);
    }
    flags = r_u32(data);
    if (flags & 2u) return data + 12u;
    if (!(flags & 16u)) {
        value = moh_draft_missing_call(0x8005D148u, 3u, frame, r_u32(data + 12u), object);
        w_u32(object + 0x218u, value);
    }
    return data + 16u;
}

uint32 level_sub_80046B2C(uint32 trigger, uint32 material)
{
    uint32 data, object, type, serial, speed, value;
    FUNCTION_MARKER(0x80046B2Cu, "LEVEL.BIN");
    data = r_u32(trigger + 4u);
    object = moh_draft_missing_call(0x8006B5B8u, 3u, 0u, r_u32(material), 21u, 0u, 0u, 0u, material);
    if (!object) return object;
    type = r_u32(material + 0xFCu);
    w_u32(object + 12u, 0x3008004u); w_u32(object + 20u, 0u);
    w_u32(object + 16u, 0x211u); w_u32(object + 8u, type);
    value = (uint32)(sint32)(sint16)r_u16(data + 0x5Eu); serial = r_u8(0x800939D8u);
    w_u32(object + 0x294u, value); w_u32(object + 0x290u, value);
    speed = (uint32)r_u8(data + 0x5Cu); w_u8(0x800939D8u, (uint8)(serial + 1u));
    w_u32(object + 0x36Cu, 0x80046A88u); w_u32(object + 0x370u, 0x8006A5B4u);
    w_u16(object + 0x2ECu, 0u); w_u16(object + 0x2E8u, 0u); w_u8(object + 0x35u, (uint8)serial);
    w_u32(object + 0x374u, 0u); w_u32(object + 0x378u, 0u); w_u32(object + 0x384u, 0x8006A37Cu);
    speed <<= 16u; w_u32(object + 0xF0u, speed); w_u32(object + 0xF4u, speed);
    data = r_u32(trigger + 4u);
    moh_draft_missing_call(0x8005C97Cu, object, 1u, (uint32)(sint32)(sint16)r_u16(data + 0x34u));
    return object;
}

uint32 level_sub_80061350(uint32 trigger, uint32 material)
{
    uint32 type, target;
    FUNCTION_MARKER(0x80061350u, "LEVEL.BIN");
    type = r_u32(material + 0xFCu);
    switch (type) {
    case 0x1402u: target = 0x800563F0u; break;
    case 0x1405u: target = 0x80054E4Cu; break;
    case 0x1770u: case 0x1778u: target = 0x8005D678u; break;
    case 0x1771u: target = 0x80046B2Cu; break;
    case 0x1772u: target = 0x8005E2ACu; break;
    case 0x1774u: target = 0x800905C0u; break;
    case 0x1775u: target = 0x8005E510u; break;
    case 0x1776u: target = 0x8005E648u; break;
    case 0x1777u: case 0x1779u: case 0x177Au: target = 0x8005E7B0u; break;
    default: return 0u;
    }
    /* MIPS carries trigger, material and a zero mode into every constructor */
    return moh_draft_missing_call(target, trigger, material, 0u);
}

uint32 level_sub_8006D6F0(uint32 identity, uint32 resource)
{
    uint32 object, render, model;
    sint32 animation;
    FUNCTION_MARKER(0x8006D6F0u, "LEVEL.BIN");
    object = moh_draft_missing_call(0x80061350u, resource, identity);
    if (!object) return object;
    render = r_u32(object + 0x100u);
    if (!render || !r_u32(render + 8u)) return object;
    moh_draft_missing_call(0x8006A76Cu, object);
    render = r_u32(object + 0x100u); model = r_u32(render + 8u);
    animation = (sint32)(sint16)r_u16(model + 0x94u);
    if (animation >= 0) {
        moh_draft_missing_call(0x8003F454u, object, (uint32)animation, 1u, 0u);
        moh_draft_missing_call(0x8003F8F0u, object);
    }
    return object;
}

void level_sub_80069714(uint32 packet, uint32 script)
{
    uint32 top, stack, filter, record, first = 0u, second = 0u, identity = 0xFFFFFFFFu, entry, handle, actor;
    FUNCTION_MARKER(0x80069714u, "LEVEL.BIN");
    top = r_u32(0x8009D5A8u); stack = top - 2u;
    w_u32(0x8009D5A8u, top - 1u); w_u32(0x8009D5A8u, stack);
    filter = r_u32(0x800AD264u + (top - 1u) * 8u); record = r_u32(0x800AD264u + top * 8u);
    switch (filter) {
    case 0u: break;
    case 1u: case 2u:
        identity = r_u32(0x800AD264u + stack * 8u);
        --stack; w_u32(0x8009D5A8u, stack); break;
    case 3u: case 4u:
        second = r_u32(0x800AD264u + stack * 8u);
        first = r_u32(0x800AD264u + (stack - 1u) * 8u);
        w_u32(0x8009D5A8u, stack - 1u); stack -= 2u; w_u32(0x8009D5A8u, stack); break;
    case 5u:
        second = r_u32(0x800AD264u + stack * 8u);
        first = r_u32(0x800AD264u + (stack - 1u) * 8u);
        identity = r_u32(0x800AD264u + (stack - 2u) * 8u);
        w_u32(0x8009D5A8u, stack - 1u); w_u32(0x8009D5A8u, stack - 2u);
        stack -= 3u; w_u32(0x8009D5A8u, stack); break;
    default:
        moh_draft_missing_call(0x80015B28u, 0x8003CE68u, filter); return;
    }
    entry = moh_draft_missing_call(0x800694C8u, record, script);
    if (!entry) return;
    handle = r_u32(packet); actor = handle ? r_u32(0x8009D678u + (handle & 0xFFFFu) * 4u) : 0u;
    moh_draft_missing_call(0x80066C14u, (uint32)r_u16(entry), actor, r_u32(packet + 12u), filter, first, second, identity);
}

uint32 level_sub_8006A204(uint32 packet)
{
    uint32 object = 0u, identity = 0u, handle, list, kind, local, result = 0u;
    FUNCTION_MARKER(0x8006A204u, "LEVEL.BIN");
    if (!moh_draft_missing_call(0x8006D8ACu, packet + 4u)) {
        handle = r_u32(packet + 4u);
        if (handle) object = r_u32(0x8009D678u + (handle & 0xFFFFu) * 4u);
    }
    if (!object) {
        identity = r_u32(packet + 16u);
        if (!identity) return 0u;
    }
    kind = r_u32(packet + 24u); list = r_u32(r_u32(0x8009D5B0u) + kind * 4u);
    if (!kind) return 1u;
    if (!object) {
        while (list) {
            if (identity == r_u32(list + 4u)) return 1u;
            list = r_u32(list + 32u);
        }
        return 0u;
    }
    local = sub_80016718(4u);
    if (!local) moh_draft_fail_fast("Queue object reference allocation", 0x8006A204u);
    w_u32(local, object);
    while (list) {
        if (moh_draft_missing_call(0x8006D7D0u, list, local)) { result = 1u; break; }
        list = r_u32(list + 32u);
    }
    sub_80036E44(local); return result;
}

uint32 level_sub_80066318(void)
{
    uint32 index;
    FUNCTION_MARKER(0x80066318u, "LEVEL.BIN");
    index = r_u32(0x8009D5B8u);
    if (index != r_u32(0x8009D5BCu)) {
        if (moh_draft_missing_call(0x8006A204u, 0x800B6460u + index * 32u))
            moh_draft_missing_call(0x800672E0u, 0x800B6460u + r_u32(0x8009D5B8u) * 32u);
        index = r_u32(0x8009D5B8u) + 1u;
        w_u32(0x8009D5B8u, (sint32)index < 128 ? index : 0u);
    }
    return r_u32(0x8009D5B8u) == r_u32(0x8009D5BCu);
}

void level_sub_80090A30(void)
{
    uint32 state;
    FUNCTION_MARKER(0x80090A30u, "LEVEL.BIN");
    state = r_u32(0x800AB8E8u);
    switch (state) {
    case 0u: moh_draft_missing_call(0x80066318u); break;
    case 1u:
        moh_draft_missing_call(0x80066318u); moh_draft_missing_call(0x80074F9Cu); break;
    case 2u:
        moh_draft_missing_call(0x80066318u); moh_draft_missing_call(0x8006DA88u);
        /* Fall through to the second service stage */
    case 3u:
        moh_draft_missing_call(0x80066318u); moh_draft_missing_call(0x8005439Cu);
        w_u32(0x800AB8E8u, 0xFFFFFFFFu); break;
    default: w_u32(0x800AB8E8u, 0xFFFFFFFFu); break;
    }
    w_u32(0x800AB8E8u, r_u32(0x800AB8E8u) + 1u);
}

void level_sub_80090AEC(void)
{
    uint32 state;
    FUNCTION_MARKER(0x80090AECu, "LEVEL.BIN");
    state = r_u32(0x800AB8ECu);
    switch (state) {
    case 0u: moh_draft_missing_call(0x800663B8u); break;
    case 1u: moh_draft_missing_call(0x80075034u); break;
    case 2u: level_sub_80090A30(); break;
    default: w_u32(0x800AB8ECu, 0xFFFFFFFFu); break;
    }
    w_u32(0x800AB8ECu, r_u32(0x800AB8ECu) + 1u);
}

void level_sub_80090B80(void)
{
    uint32 time, prior, toggle, buffer;
    sint32 skipped;
    FUNCTION_MARKER(0x80090B80u, "LEVEL.BIN");
    w_u32(0x800AB8E4u, 0u); w_u32(0x800AB8E0u, 1u);
    level_sub_80090A30();
    time = moh_draft_missing_call(0x80015DB8u, 0xFFFFFFFFu); prior = r_u32(0x80097FCCu);
    if (prior == time) {
        do {
            level_sub_80090AEC();
            time = moh_draft_missing_call(0x80015DB8u, 0xFFFFFFFFu);
        } while (r_u32(0x80097FCCu) == time);
        prior = r_u32(0x80097FCCu);
    }
    if (prior == time - 1u) {
        do {
            level_sub_80090A30();
            time = moh_draft_missing_call(0x80015DB8u, 0xFFFFFFFFu);
        } while (r_u32(0x80097FCCu) == time - 1u);
    }
    toggle = 1u - r_u32(0x800AB8F0u); prior = r_u32(0x80097FCCu);
    skipped = (sint32)(time - prior + toggle) >> 1;
    w_u32(0x800AB8F0u, toggle); w_u32(0x8009A5D0u, skipped > 0 ? (uint32)skipped - 1u : 0u);
    w_u32(0x800AB8E0u, 0u);
    if (!r_u32(0x800AB8E4u)) {
        buffer = r_u32(0x8009C824u); w_u32(0x800AB8E4u, 1u);
        buffer = buffer == 0u; w_u32(0x8009C824u, buffer);
        moh_draft_missing_call(0x80017408u, 0x8009A7A0u + buffer * 20u);
    }
    buffer = r_u32(0x8009C824u);
    moh_draft_missing_call(0x80017348u, 0x8009A630u + buffer * 184u);
    w_u32(0x80097FCCu, time);
}

void level_sub_800504A0(void)
{
    FUNCTION_MARKER(0x800504A0u, "LEVEL.BIN");
    if (r_u32(0x80097FD0u)) {
        while (moh_draft_missing_call(0x80016E38u, 1u))
            w_u32(0x80097FDCu, r_u32(0x80097FDCu) + 1u);
    }
    level_sub_80090B80();
}

void level_sub_800504F8(void)
{
    uint32 ordering;
    FUNCTION_MARKER(0x800504F8u, "LEVEL.BIN");
    level_sub_800504A0();
    ordering = r_u32(0x8009C824u) ? 0x8009AFC4u : 0x8009BFC4u;
    moh_draft_missing_call(0x800172D8u, ordering);
    w_u32(0x80097FB4u, 0u);
}

void level_sub_8004E8FC(uint32 render_context, uint32 mask_index)
{
    uint32 buffer, local, packet, offset, record, index, multiplier, count, node, ordering, header, tag, page;
    FUNCTION_MARKER(0x8004E8FCu, "LEVEL.BIN");
    buffer = r_u32(render_context + 0x221Cu);
    local = sub_80016718(4u);
    if (!local) moh_draft_fail_fast("Overlay packet bound allocation", 0x8004E8FCu);
    packet = level_sub_80053BD4(buffer, local); buffer *= 24u;
    w_u32(local, r_u32(local) - 12u);
    for (offset = 0u; offset < 0x114Cu; offset += 0x1ECu) {
        record = 0x80096E60u + offset;
        if (!r_u8(record + 0x1E3u) || !(r_u32(record + 0x1E8u) & (1u << (mask_index & 31u)))) continue;
        if (r_u32(local) < packet) {
            /* The original passes the bound word to this early cursor commit */
            level_sub_80053BF4(r_u32(local)); break;
        }
        multiplier = r_u8(record + 0x1E2u) ? 5u : 1u;
        count = r_u16(record + 0x1E0u) * multiplier;
        for (index = 0u; index < count; ++index) {
            node = record + buffer + index * 48u;
            ordering = r_u32(render_context + 0x2218u); header = r_u32(node); tag = r_u32(ordering);
            w_u32(node, (header & 0xFF000000u) | (tag & 0xFFFFFFu));
            ordering = r_u32(render_context + 0x2218u); tag = r_u32(ordering);
            w_u32(ordering, (tag & 0xFF000000u) | (node & 0xFFFFFFu));
            count = r_u16(record + 0x1E0u) * multiplier;
        }
        page = moh_draft_missing_call(0x8001B968u, 0u, (uint32)r_u8(record + 0x1E4u), 0u, 0u);
        moh_draft_missing_call(0x8001B8C8u, packet, 1u, 0u, page & 0xFFFFu, 0u);
        ordering = r_u32(render_context + 0x2218u); header = r_u32(packet); tag = r_u32(ordering);
        w_u32(packet, (header & 0xFF000000u) | (tag & 0xFFFFFFu));
        ordering = r_u32(render_context + 0x2218u); tag = r_u32(ordering);
        node = packet; packet += 12u;
        w_u32(ordering, (tag & 0xFF000000u) | (node & 0xFFFFFFu));
    }
    level_sub_80053BF4(packet); sub_80036E44(local);
}

void level_sub_8008019C(uint32 object, uint32 render_context)
{
    uint32 state, node, ordering, header, tag;
    FUNCTION_MARKER(0x8008019Cu, "LEVEL.BIN");
    state = r_u32(object + 0x388u);
    level_sub_8004E8FC(render_context, (uint32)r_u8(state + 0x152u));
    node = state + r_u32(render_context + 0x221Cu) * 40u + 0xB8u;
    ordering = r_u32(render_context + 0x2218u); header = r_u32(node); tag = r_u32(ordering);
    w_u32(node, (header & 0xFF000000u) | (tag & 0xFFFFFFu));
    node = state + r_u32(render_context + 0x221Cu) * 40u + 0xB8u;
    ordering = r_u32(render_context + 0x2218u); tag = r_u32(ordering);
    w_u32(ordering, (tag & 0xFF000000u) | (node & 0xFFFFFFu));
}

void level_sub_8007CAF8(uint32 object, uint32 first, uint32 second)
{
    uint32 buffer, ordering, local, packet, state, mode, flags, x, y, index, current, header, tag;
    uint32 x0[4], y0[4], x1[4], y1[4];
    FUNCTION_MARKER(0x8007CAF8u, "LEVEL.BIN");
    buffer = r_u32(0x8009C83Cu); ordering = r_u32(0x8009C838u);
    local = sub_80016718(4u);
    if (!local) moh_draft_fail_fast("Crosshair packet bound allocation", 0x8007CAF8u);
    packet = level_sub_80053BD4(buffer, local);
    w_u32(local, r_u32(local) - 16u);
    if (packet + 64u > r_u32(local)) { sub_80036E44(local); return; }
    state = r_u32(object + 0x388u); mode = r_u32(state + 0x1C8u);
    if (mode == 6u) { sub_80036E44(local); return; }
    flags = r_u32(state);
    if (!(flags & 0x20u)) { sub_80036E44(local); return; }
    if (!mode && !(flags & 0x1800000u)) {
        /* TODO Translate the alternate aiming renderer when reached */
        moh_draft_missing_call(0x8007C71Cu, object, first, second);
        sub_80036E44(local); return;
    }
    x = r_u16(state + 0x190u);
    if (!mode) x += r_u16(state + 0x194u);
    y = r_u16(state + 0x192u);
    if (!mode) y += r_u16(state + 0x196u);
    mode = r_u32(state + 0x1C8u);
    x0[0] = x; x1[0] = x;
    if (mode == 5u) {
        y0[0] = y - 8u; y1[0] = y + 6u;
        x0[1] = x - 24u; y0[1] = y - 18u; x1[1] = x + 24u; y1[1] = y - 18u;
        x0[2] = x - 32u; y0[2] = y - 12u; x1[2] = x - 8u; y1[2] = y + 16u;
        x0[3] = x + 32u; y0[3] = y - 12u; x1[3] = x + 8u; y1[3] = y + 16u;
    } else {
        y0[0] = y - 12u; y1[0] = y - 5u;
        x0[1] = x; y0[1] = y + 12u; x1[1] = x; y1[1] = y + 5u;
        x0[2] = x - 18u; y0[2] = y; x1[2] = x - 8u; y1[2] = y;
        x0[3] = x + 18u; y0[3] = y; x1[3] = x + 8u; y1[3] = y;
    }
    for (index = 0u; index < 4u; ++index) {
        current = packet + index * 16u;
        /* TODO Bind the original line packet SDK service when reached */
        moh_draft_missing_call(0x8001DB40u, current);
        if (!index) w_u16(current + 10u, (uint16)y0[index]);
        header = 0u;
        if (!index || mode != 5u) header = r_u32(current);
        if (index) {
            if (index == 1u && mode != 5u) w_u16(current + 10u, (uint16)y0[index]);
            else w_u16(current + 8u, (uint16)x0[index]);
        }
        w_u8(current + 4u, 0x80u); w_u8(current + 5u, 0x20u); w_u8(current + 6u, 0x20u);
        if (!index || (index == 1u && mode != 5u)) w_u16(current + 8u, (uint16)x0[index]);
        else w_u16(current + 10u, (uint16)y0[index]);
        w_u16(current + 12u, (uint16)x1[index]); w_u16(current + 14u, (uint16)y1[index]);
        if (index && mode == 5u) header = r_u32(current);
        tag = r_u32(ordering + 4u);
        w_u32(current, (header & 0xFF000000u) | (tag & 0xFFFFFFu));
        tag = r_u32(ordering + 4u);
        w_u32(ordering + 4u, (tag & 0xFF000000u) | (current & 0xFFFFFFu));
    }
    level_sub_80053BF4(packet + 64u); sub_80036E44(local);
}

uint32 sub_80013F98(uint32 ordering_tag, uint32 last, uint32 end)
{
    uint32 header, next, tail, node, tag, result;
    FUNCTION_MARKER(0x80013F98u, "SLUS_009.74");
    header = r_u32(last); end &= 0xFFFFFFu;
    for (;;) {
        next = header & 0xFFFFFFu;
        result = header >> 24u;
        if ((header >> 24u) >= 2u) break;
        last = next | 0x80000000u;
        if (next == end) return result;
        header = r_u32(last);
    }
    tail = r_u32(ordering_tag); w_u32(ordering_tag, last & 0xFFFFFFu);
    while (next != end) {
        node = next | 0x80000000u; tag = r_u32(node); next = tag & 0xFFFFFFu;
        result = tag >> 24u;
        if ((tag >> 24u) >= 2u) {
            result = node & 0xFFFFFFu;
            w_u32(last, (header & 0xFF000000u) | (node & 0xFFFFFFu));
            header = tag; last = node;
        }
    }
    w_u32(last, (header & 0xFF000000u) | (tail & 0xFFFFFFu));
    return result;
}

uint32 sub_80013E58(uint32 quaternion, uint32 matrix)
{
    uint32 a, b, x, y, z, w, doubled_x, doubled_y, doubled_w, first[3], second[3], value;
    FUNCTION_MARKER(0x80013E58u, "SLUS_009.74");
    a = r_u32(quaternion); b = r_u32(quaternion + 4u);
    x = (uint32)(sint32)(sint16)a; y = (uint32)((sint32)a >> 16);
    z = (uint32)(sint32)(sint16)b; w = (uint32)((sint32)b >> 16);
    xport_gte_write_control(0u, (w << 16u) | (x & 0xFFFFu));
    value = (y << 16u) | (z & 0xFFFFu);
    xport_gte_write_control(1u, value); xport_gte_write_control(3u, value);
    xport_gte_write_control(2u, ((0u - z) & 0xFFFFu) | (w << 16u));
    xport_gte_write_control(4u, 0u - x);
    doubled_x = x + x; doubled_w = w + w; doubled_y = y + y;
    xport_gte_write_data(0u, (doubled_w << 16u) | (doubled_x & 0xFFFFu));
    xport_gte_write_data(1u, 0u); xport_gte_execute(0x486012u);
    first[0] = xport_gte_read_data(25u); first[1] = xport_gte_read_data(26u); first[2] = xport_gte_read_data(27u);
    xport_gte_write_data(0u, doubled_y & 0xFFFFu); xport_gte_write_data(1u, doubled_w);
    xport_gte_execute(0x486012u);
    w_u16(matrix, (uint16)(first[0] - 0x1000u)); w_u16(matrix + 6u, (uint16)first[1]);
    w_u16(matrix + 12u, (uint16)first[2]);
    second[0] = xport_gte_read_data(25u); second[1] = xport_gte_read_data(26u); second[2] = xport_gte_read_data(27u);
    xport_gte_write_data(0u, (0u - doubled_y) << 16u); xport_gte_write_data(1u, doubled_x);
    xport_gte_execute(0x486012u);
    w_u16(matrix + 2u, (uint16)second[0]); w_u16(matrix + 8u, (uint16)(second[1] - 0x1000u));
    w_u16(matrix + 14u, (uint16)second[2]);
    value = ((uint32)((sint32)(z * z) >> 12) << 1u) + ((uint32)((sint32)(w * w) >> 12) << 1u) - 0x1000u;
    a = xport_gte_read_data(25u); b = xport_gte_read_data(26u);
    w_u16(matrix + 4u, (uint16)a); w_u16(matrix + 10u, (uint16)b); w_u16(matrix + 16u, (uint16)value);
    return matrix;
}

void sub_80013AE4(uint32 node, uint32 matrix, uint32 output, uint32 render)
{
    uint32 local_words[8];
    uint32 id, quaternions, local, axis, word, magnitude, high[3], low[3], product[3], translation[3];
    uint32 table, vertices, count, destination, index, point, child, positions, x, y, z;
    FUNCTION_MARKER(0x80013AE4u, "SLUS_009.74");
    id = r_u16(node); word = r_u32(0x8009EAB4u); quaternions = r_u32(render + 0x88u);
    if (id > word || !quaternions) return;
    /* TODO Native address bridge for the original addressable local matrix */
    local = moh_draft_local_address(local_words, sizeof(local_words));
    sub_80013E58(quaternions + id * 8u, local);
    for (axis = 0u; axis < 3u; ++axis) {
        word = (uint32)(sint32)(sint16)r_u16(node + 2u + axis * 2u) << 19u;
        if ((sint32)word < 0) {
            magnitude = 0u - word;
            high[axis] = 0u - (uint32)((sint32)magnitude >> 15);
            low[axis] = 0u - (magnitude & 0x7FFFu);
        } else { high[axis] = (uint32)((sint32)word >> 15); low[axis] = word & 0x7FFFu; }
    }
    for (axis = 0u; axis < 3u; ++axis) xport_gte_write_data(9u + axis, high[axis]);
    for (axis = 0u; axis < 5u; ++axis) xport_gte_write_control(axis, r_u32(matrix + axis * 4u));
    xport_gte_execute(0x41E012u);
    for (axis = 0u; axis < 3u; ++axis) product[axis] = xport_gte_read_data(25u + axis);
    for (axis = 0u; axis < 3u; ++axis) xport_gte_write_data(9u + axis, low[axis]);
    xport_gte_execute(0x49E012u);
    for (axis = 0u; axis < 3u; ++axis) translation[axis] = r_u32(matrix + 20u + axis * 4u);
    for (axis = 0u; axis < 3u; ++axis)
        translation[axis] += xport_gte_read_data(25u + axis) + (product[axis] << 3u);
    for (axis = 0u; axis < 3u; ++axis)
        xport_gte_write_control(5u + axis, (uint32)((sint32)((translation[axis] << 1u) + 0x80000u) >> 20));
    table = r_u32(render + 0x80u);
    for (axis = 0u; axis < 4u; ++axis) xport_gte_write_control(axis, r_u32(local + axis * 4u));
    table = r_u32(table); xport_gte_write_control(4u, r_u32(local + 16u));
    word = r_u32(0x8009EAB8u); w_u32(local + 20u, translation[0]);
    if (id > word) return;
    w_u32(local + 24u, translation[1]); w_u32(local + 28u, translation[2]);
    vertices = r_u32(table + id * 8u); count = r_u32(table + id * 8u + 4u);
    word = r_u32(0x1F8003FCu); destination = r_u32(0x8009EABCu);
    w_u32(0x1F8003FCu, word + count); w_u32(output + id * 4u, destination);
    w_u32(0x8009EABCu, destination + count * 8u);
    index = 0u;
    while (index != count) {
        point = vertices + index * 8u;
        xport_gte_write_data(0u, r_u32(point)); xport_gte_write_data(1u, r_u32(point + 4u));
        xport_gte_execute(0x480012u);
        point = destination + index * 8u; ++index;
        x = xport_gte_read_data(25u); y = xport_gte_read_data(26u); z = xport_gte_read_data(27u);
        w_u16(point, (uint16)x); w_u16(point + 2u, (uint16)y); w_u16(point + 4u, (uint16)z);
    }
    child = r_u32(node + 16u); if (child) sub_80013AE4(child, matrix, output, render);
    child = r_u32(node + 12u); if (child) sub_80013AE4(child, local, output, render);
    positions = r_u32(render + 0x94u) + id * 8u;
    for (axis = 0u; axis < 3u; ++axis)
        w_u16(positions + axis * 2u, (uint16)((sint32)((translation[axis] << 1u) + 0x80000u) >> 20));
}

void moh_native_model_transform(uint32 node, uint32 matrix, uint32 output, uint32 render)
{
    uint32 id, quaternions, local, axis, word, magnitude, high[3], low[3], product[3], translation[3];
    uint32 table, vertices, count, destination, index, point, child, positions, x, y, z;
    id = r_u16(node); word = r_u32(0x8009EAB4u); quaternions = r_u32(render + 0x88u);
    if (id > word || !quaternions) return;
    local = sub_80016718(32u);
    if (!local) moh_draft_fail_fast("Model part matrix allocation", 0x80013AE4u);
    sub_80013E58(quaternions + id * 8u, local);
    for (axis = 0u; axis < 3u; ++axis) {
        word = (uint32)(sint32)(sint16)r_u16(node + 2u + axis * 2u) << 19u;
        if ((sint32)word < 0) {
            magnitude = 0u - word;
            high[axis] = 0u - (uint32)((sint32)magnitude >> 15);
            low[axis] = 0u - (magnitude & 0x7FFFu);
        } else { high[axis] = (uint32)((sint32)word >> 15); low[axis] = word & 0x7FFFu; }
    }
    for (axis = 0u; axis < 3u; ++axis) xport_gte_write_data(9u + axis, high[axis]);
    for (axis = 0u; axis < 5u; ++axis) xport_gte_write_control(axis, r_u32(matrix + axis * 4u));
    xport_gte_execute(0x41E012u);
    for (axis = 0u; axis < 3u; ++axis) product[axis] = xport_gte_read_data(25u + axis);
    for (axis = 0u; axis < 3u; ++axis) xport_gte_write_data(9u + axis, low[axis]);
    xport_gte_execute(0x49E012u);
    for (axis = 0u; axis < 3u; ++axis) translation[axis] = r_u32(matrix + 20u + axis * 4u);
    for (axis = 0u; axis < 3u; ++axis)
        translation[axis] += xport_gte_read_data(25u + axis) + (product[axis] << 3u);
    for (axis = 0u; axis < 3u; ++axis)
        xport_gte_write_control(5u + axis, (uint32)((sint32)((translation[axis] << 1u) + 0x80000u) >> 20));
    table = r_u32(render + 0x80u);
    for (axis = 0u; axis < 4u; ++axis) xport_gte_write_control(axis, r_u32(local + axis * 4u));
    table = r_u32(table); xport_gte_write_control(4u, r_u32(local + 16u));
    word = r_u32(0x8009EAB8u); w_u32(local + 20u, translation[0]);
    if (id > word) { sub_80036E44(local); return; }
    w_u32(local + 24u, translation[1]); w_u32(local + 28u, translation[2]);
    vertices = r_u32(table + id * 8u); count = r_u32(table + id * 8u + 4u);
    word = r_u32(0x1F8003FCu); destination = r_u32(0x8009EABCu);
    w_u32(0x1F8003FCu, word + count); w_u32(output + id * 4u, destination);
    w_u32(0x8009EABCu, destination + count * 8u);
    index = 0u;
    while (index != count) {
        point = vertices + index * 8u;
        xport_gte_write_data(0u, r_u32(point)); xport_gte_write_data(1u, r_u32(point + 4u));
        xport_gte_execute(0x480012u);
        point = destination + index * 8u; ++index;
        x = xport_gte_read_data(25u); y = xport_gte_read_data(26u); z = xport_gte_read_data(27u);
        w_u16(point, (uint16)x); w_u16(point + 2u, (uint16)y); w_u16(point + 4u, (uint16)z);
    }
    child = r_u32(node + 16u); if (child) moh_native_model_transform(child, matrix, output, render);
    child = r_u32(node + 12u); if (child) moh_native_model_transform(child, local, output, render);
    positions = r_u32(render + 0x94u) + id * 8u;
    for (axis = 0u; axis < 3u; ++axis)
        w_u16(positions + axis * 2u, (uint16)((sint32)((translation[axis] << 1u) + 0x80000u) >> 20));
    sub_80036E44(local);
}

uint32 level_sub_8006A96C(uint32 instance, uint32 material, uint32 flags, uint32 object)
{
    uint32 bytes = 0u, cursor = 0u, pointer, count, packed, index, offset, source, destination;
    FUNCTION_MARKER(0x8006A96Cu, "LEVEL.BIN");
    w_u32(instance + 0x6Cu, 128u);
    w_u32(instance, 0u); w_u32(instance + 0x68u, 0u);
    w_u32(instance + 4u, flags); w_u32(instance + 8u, material);
    if (!(flags & 1u) && r_u32(material + 0x1Cu)) {
        pointer = r_u32(material + 0x2Cu);
        if (pointer) bytes = r_u32(pointer) * 16u + 20u;
        pointer = r_u32(material + 0x30u);
        if (pointer) bytes += 8u + r_u32(pointer) * 4u;
        pointer = r_u32(material + 0x34u);
        if (pointer) bytes += 20u + r_u32(pointer) * 16u;
    }
    if (!(flags & 2u)) bytes += (uint32)r_u16(material + 0x8Cu) * 8u;
    if (!r_u32(material + 0x14u)) {
        if (!(flags & 0x40u)) bytes += (uint32)r_u16(material + 0x8Cu) * 8u;
        if (!(flags & 0x100u)) bytes += (uint32)r_u16(material + 0x8Cu) * 4u;
    }
    if (!(flags & 4u)) bytes += (uint32)r_u16(material + 0x8Eu) * 8u;
    if (!(flags & 16u)) bytes += 4096u;
    if ((sint32)moh_draft_missing_call(0x8006A910u, 0x42A7A5EFu) >= 0)
        bytes += (uint32)r_u16(material + 0xBEu) * 176u;
    pointer = r_u32(material + 8u);
    if (pointer) bytes += (uint32)r_u8(r_u32(pointer + 4u) + 1u) * 24u;
    else {
        w_u32(instance + 0x74u, 0u); w_u16(instance + 0x70u, 0u); w_u16(instance + 0x72u, 0u);
        w_u32(instance + 0x7Cu, 0u); w_u32(instance + 0x84u, 0u); w_u32(instance + 0x88u, 0u);
        w_u32(instance + 0x90u, 0u); w_u32(instance + 0x78u, 0u); w_u32(instance + 0x80u, 0u);
        w_u32(instance + 0x8Cu, 0u); w_u32(instance + 0x94u, 0u);
    }
    if ((sint32)bytes > 0) {
        pointer = r_u32(material + 0xFCu);
        cursor = pointer == 0x1402u || pointer == 0x1405u ?
            moh_draft_missing_call(0x8008ED30u, bytes) : sub_80036BF0(bytes);
        if (!cursor) return 0u;
        w_u32(instance, cursor);
    }
    pointer = r_u32(material + 8u);
    if (pointer) {
        w_u16(instance + 0x70u, r_u16(pointer));
        pointer = r_u32(material + 8u);
        w_u32(instance + 0x78u, pointer + 4u);
        w_u16(instance + 0x72u, r_u8(r_u32(pointer + 4u) + 1u));
        w_u32(instance + 0x74u, r_u32(material + 8u));
        w_u32(instance + 0x7Cu, r_u32(material + 0x38u));
        w_u32(instance + 0x80u, r_u32(material + 0x84u));
        source = r_u32(material + 0x9Cu);
        count = (uint32)r_u16(instance + 0x72u) * 8u;
        w_u32(instance + 0x90u, cursor); cursor += count;
        w_u32(instance + 0x8Cu, cursor); cursor += count;
        w_u32(instance + 0x94u, cursor); w_u32(instance + 0x84u, source); cursor += count;
        source = r_u32(r_u32(r_u32(instance + 0x78u)) + 4u);
        moh_draft_missing_call(0x800154ECu, r_u32(instance + 0x8Cu), r_u32(source + 16u), count);
        source = r_u32(r_u32(r_u32(instance + 0x78u)) + 4u);
        moh_draft_missing_call(0x800154ECu, r_u32(instance + 0x90u), r_u32(source + 16u), (uint32)r_u16(instance + 0x72u) * 8u);
        w_u32(instance + 0x88u, 0u);
        packed = r_u32(r_u32(r_u32(r_u32(instance + 0x78u)) + 4u) + 4u);
        w_u16(instance + 0x98u, (uint16)((sint32)(packed << 22) >> 22));
        packed = r_u32(r_u32(r_u32(r_u32(instance + 0x78u)) + 4u) + 4u);
        w_u16(instance + 0x9Au, (uint16)((sint32)(packed << 11) >> 21));
        packed = r_u32(r_u32(r_u32(r_u32(instance + 0x78u)) + 4u) + 4u);
        w_u16(instance + 0x9Cu, (uint16)((sint32)(packed << 1) >> 22));
        source = r_u32(instance + 0x98u); destination = r_u32(instance + 0x9Cu);
        w_u32(instance + 0xA0u, source); w_u32(instance + 0xA4u, destination);
    }
    if (!(flags & 1u) && r_u32(material + 0x1Cu)) {
        for (offset = 0x14u; offset <= 0x28u; offset += 4u) w_u32(instance + offset, 0u);
        pointer = r_u32(material + 0x2Cu);
        if (pointer) w_u32(instance + 0x20u, pointer);
        pointer = r_u32(material + 0x30u);
        if (pointer) {
            w_u32(instance + 0x24u, cursor);
            source = r_u32(material + 0x30u); count = r_u32(source) * 4u + 8u;
            destination = cursor; cursor += count;
            moh_draft_missing_call(0x800154ECu, destination, source, count);
        }
        pointer = r_u32(material + 0x34u);
        if (pointer) w_u32(instance + 0x28u, pointer);
    } else {
        w_u32(instance + 0x14u, r_u32(material + 0x20u));
        w_u32(instance + 0x1Cu, r_u32(material + 0x28u));
        w_u32(instance + 0x18u, r_u32(material + 0x24u));
        w_u32(instance + 0x20u, r_u32(material + 0x2Cu));
        w_u32(instance + 0x24u, r_u32(material + 0x30u));
        w_u32(instance + 0x28u, r_u32(material + 0x34u));
        if (!r_u32(material + 0x1Cu)) {
            w_u32(material + 0x1Cu, 1u);
            w_u32(instance + 4u, r_u32(instance + 4u) | 8u);
        }
    }
    w_u16(instance + 0x3Cu, r_u16(material + 0x8Cu));
    if (!(flags & 2u)) {
        w_u32(instance + 0x2Cu, cursor);
        count = (uint32)r_u16(material + 0x8Cu) * 8u; source = r_u32(material + 0x88u);
        destination = cursor; cursor += count;
        moh_draft_missing_call(0x800154ECu, destination, source, count);
    } else w_u32(instance + 0x2Cu, r_u32(material + 0x88u));
    if (!r_u32(material + 0x14u)) {
        if (!(flags & 0x40u)) { w_u32(instance + 0x30u, cursor); cursor += (uint32)r_u16(material + 0x8Cu) * 8u; }
        w_u32(instance + 0x34u, 0xFFFFFFFFu); w_u32(instance + 0x38u, 0xFFFFFFFFu);
        if (!(flags & 0x100u)) {
            w_u32(instance + 0x48u, cursor);
            moh_draft_missing_call(0x800154ECu, cursor, r_u32(material + 0xC4u), (uint32)r_u16(material + 0x8Cu) * 4u);
            cursor += (uint32)r_u16(material + 0x8Cu) * 4u;
        } else w_u32(instance + 0x48u, r_u32(material + 0xC4u));
    }
    w_u16(instance + 0x42u, r_u16(material + 0x8Eu));
    if (!(flags & 4u)) {
        if (!r_u32(instance + 0x44u)) w_u32(instance + 0x44u, 0u);
        else {
            w_u32(instance + 0x44u, cursor);
            count = (uint32)r_u16(material + 0x8Eu) * 8u; source = r_u32(material + 0x90u);
            destination = cursor; cursor += count;
            moh_draft_missing_call(0x800154ECu, destination, source, count);
        }
    } else w_u32(instance + 0x44u, r_u32(material + 0x90u));
    if (!(flags & 16u)) { w_u32(instance + 0x4Cu, cursor); cursor += 2048u; w_u32(instance + 0x50u, cursor); }
    else { w_u32(instance + 0x4Cu, 0u); w_u32(instance + 0x50u, 0u); }
    w_u16(instance + 0x5Cu, 0xFFFFu); w_u16(instance + 0x5Eu, 0u);
    moh_draft_missing_call(0x8006D84Cu, instance + 0x54u, 0u);
    w_u32(instance + 0x58u, 0u);
    if (object) {
        w_u32(object + 0x2D4u, r_u32(material + 0xA0u));
        w_u32(object + 0x270u, r_u32(material + 0xA4u));
        w_u32(object + 0x274u, r_u32(material + 0xA8u));
        w_u32(object + 0x278u, r_u32(material + 0xACu));
        w_u16(object + 0x36u, r_u16(material + 0xBEu));
        for (index = 0u, offset = 0u; index < (uint32)r_u16(material + 0xBEu); ++index, offset += 72u) {
#define MODEL_COPY_WORD(DST, SRC) w_u32(r_u32(object + 0x304u) + offset + (DST), r_u32(r_u32(material + 0xC0u) + offset + (SRC)))
#define MODEL_COPY_HALF(FIELD) w_u16(r_u32(object + 0x304u) + offset + (FIELD), r_u16(r_u32(material + 0xC0u) + offset + (FIELD)))
            MODEL_COPY_WORD(4u, 4u); MODEL_COPY_WORD(8u, 12u); MODEL_COPY_WORD(12u, 12u);
            MODEL_COPY_HALF(28u); MODEL_COPY_HALF(30u);
            MODEL_COPY_WORD(52u, 52u); MODEL_COPY_WORD(48u, 48u); MODEL_COPY_WORD(56u, 56u);
            MODEL_COPY_WORD(16u, 16u); MODEL_COPY_WORD(20u, 20u);
            MODEL_COPY_HALF(64u); MODEL_COPY_HALF(66u); MODEL_COPY_HALF(68u);
#undef MODEL_COPY_WORD
#undef MODEL_COPY_HALF
            w_u32(r_u32(object + 0x304u) + offset + 24u, instance);
            if (flags & 0x200u) {
                destination = r_u32(object + 0x304u) + offset;
                w_u32(destination + 52u, r_u32(destination + 52u) << 1);
                destination = r_u32(object + 0x304u) + offset;
                w_u32(destination + 48u, r_u32(destination + 48u) << 1);
                destination = r_u32(object + 0x304u) + offset;
                w_u32(destination + 56u, r_u32(destination + 56u) << 1);
            }
            w_u32(r_u32(object + 0x304u) + offset + 60u, 0u);
        }
    }
    return 1u;
}

uint32 level_sub_8003F454(uint32 object, sint32 animation, uint32 force, uint32 channel)
{
    uint32 instance, geometry, descriptor, state, mask, accepted = 0u;
    uint32 source, destination, bytes, first, second, count, index, offset;
    sint32 selected = (sint32)(sint16)(uint32)animation;
    FUNCTION_MARKER(0x8003F454u, "LEVEL.BIN");
    instance = r_u32(object + 0x100u);
    geometry = r_u32(instance + 8u);
    descriptor = r_u32(geometry + 0xB8u) + (uint32)selected * 16u;
    if (selected < 0) return 0u;
    state = object + channel * 116u + 0x104u;
    if (!force) {
        mask = r_u32(descriptor);
        if (mask & r_u32(state + 4u)) {
            w_u32(state + 16u, descriptor);
            w_u16(state + 26u, (uint16)animation);
            w_u32(state + 20u, r_u32(descriptor + 12u));
            instance = r_u32(object + 0x100u);
            accepted = 1u;
            if (!r_u32(instance + 0x74u)) {
                destination = r_u32(instance + 0x30u);
                bytes = (uint32)r_u16(instance + 0x3Cu) * 8u;
                source = r_u32(instance + 0x2Cu);
                moh_draft_missing_call(0x800154ECu, destination, source, bytes);
            } else {
                first = r_u32(instance + 0x98u); second = r_u32(instance + 0x9Cu);
                w_u32(instance + 0xA0u, first); w_u32(instance + 0xA4u, second);
                instance = r_u32(object + 0x100u);
                source = r_u32(instance + 0x88u);
                if (source) {
                    bytes = (uint32)r_u16(instance + 0x72u) * 8u;
                    destination = r_u32(instance + 0x8Cu);
                    moh_draft_missing_call(0x800154ECu, destination, source, bytes);
                }
            }
            w_u16(r_u32(object + 0x100u) + 0x3Eu, 4u);
            w_u16(r_u32(object + 0x100u) + 0x40u, 0xFFFFu);
        } else if (!(mask & r_u32(state)) &&
            ((mask & r_u32(state + 8u)) || (mask & r_u32(state + 12u)) || r_u32(state + 16u))) {
            return 0u;
        }
    }
    if (!accepted) {
        w_u32(state + 16u, descriptor);
        w_u16(state + 26u, (uint16)animation);
        w_u32(state + 20u, r_u32(descriptor + 12u));
        accepted = 1u;
        w_u16(r_u32(object + 0x100u) + 0x3Eu, 0u);
    }
    w_u32(object + 0x1FCu, 0u);
    w_u32(state, 0u); w_u32(state + 4u, 0xFFFFFFFFu);
    w_u32(state + 8u, 0u); w_u32(state + 12u, 0u);
    w_u32(object + 0x28Cu, r_u32(object + 0x288u));
    if (!channel) {
        moh_draft_missing_call(0x8004C64Cu, object, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu);
        geometry = r_u32(r_u32(object + 0x100u) + 8u);
        count = r_u32(geometry + 0xB0u);
        if (count) {
            if (r_u32(object + 0x1ECu) < count) {
                bytes = r_u32(object + 0x1ECu) * 60u;
                moh_draft_missing_call(0x800154ECu, r_u32(state + 32u), r_u32(geometry + 0xB4u), bytes);
            } else {
                bytes = count * 60u;
                moh_draft_missing_call(0x800154ECu, r_u32(state + 32u), r_u32(geometry + 0xB4u), bytes);
                index = count; offset = bytes;
                while (index < r_u32(object + 0x1ECu)) {
                    w_u32(r_u32(state + 32u) + offset, 0u);
                    ++index; offset += 60u;
                }
            }
        } else {
            index = 0u; offset = 0u;
            while (index < r_u32(object + 0x1ECu)) {
                w_u32(r_u32(state + 32u) + offset, 0u);
                ++index; offset += 60u;
            }
        }
    } else {
        index = 0u; offset = 0u;
        while (index < r_u32(object + 0x1ECu)) {
            w_u32(r_u32(state + 32u) + offset, 0u);
            ++index; offset += 60u;
        }
    }
    w_u16(state + 24u, 0xFFFFu);
    w_u16(state + 28u, 0u); w_u16(state + 30u, 0u);
    return 1u;
}

void level_sub_8004C64C(uint32 object, uint32 mask, sint32 value, sint32 radius, sint32 other)
{
    uint32 index = 0u, offset = 0u, record, squared;
    FUNCTION_MARKER(0x8004C64Cu, "LEVEL.BIN");
    if (!r_u16(object + 54u)) return;
    do {
        record = r_u32(object + 0x304u) + offset;
        if (r_u32(record + 4u) & mask) {
            if (value > 0) w_u32(record + 8u, (uint32)value);
            else if (value < 0) w_u32(record + 8u, r_u32(record + 12u));
            if (other > 0) w_u32(r_u32(object + 0x304u) + offset + 16u, (uint32)other);
            else if (other < 0) {
                record = r_u32(object + 0x304u) + offset;
                w_u32(record + 16u, r_u32(record + 20u));
            }
            if (radius > 0) {
                w_u32(r_u32(object + 0x304u) + offset + 52u, (uint32)radius);
                squared = (uint32)radius * (uint32)radius;
                w_u32(r_u32(object + 0x304u) + offset + 56u, squared);
            } else if (radius < 0) {
                record = r_u32(object + 0x304u) + offset;
                w_u32(record + 52u, r_u32(record + 48u));
                record = r_u32(object + 0x304u) + offset;
                squared = r_u32(record + 52u);
                w_u32(record + 56u, squared * squared);
            }
        }
        ++index; offset += 72u;
    } while (index < (uint32)r_u16(object + 54u));
}

uint32 level_sub_800590B4(uint32 object, uint32 instruction, uint32 flags)
{
    uint32 first, second;
    FUNCTION_MARKER(0x800590B4u, "LEVEL.BIN");
    first = moh_draft_missing_call(0x8005D148u, 1u, flags, (uint32)r_u16(instruction), object);
    second = moh_draft_missing_call(0x8005D148u, 2u, flags, r_u32(instruction + 4u), object);
    moh_draft_missing_call(0x8005C97Cu, object, first, second);
    return instruction + 8u;
}

uint32 level_sub_80059218(uint32 object, uint32 instruction, uint32 flags)
{
    uint32 result;
    FUNCTION_MARKER(0x80059218u, "LEVEL.BIN");
    result = moh_draft_missing_call(0x8005D148u, 1u, flags, (uint32)r_u16(instruction), object);
    w_u16(r_u32(0x80099EC4u) + 28u, (uint16)(result - 1u));
    w_u32(0x80099EC0u, 1u);
    return instruction + 4u;
}

void level_sub_80046C38(uint32 camera, uint32 x, uint32 y, uint32 z)
{
    uint32 data;
    FUNCTION_MARKER(0x80046C38u, "LEVEL.BIN");
    data = r_u32(camera + 0x388u);
    w_u16(data + 0xD0u, (uint16)x);
    w_u16(data + 0xD2u, (uint16)y);
    w_u16(data + 0xD4u, (uint16)z);
}

void level_sub_8004760C(uint32 state)
{
    uint32 first, second;
    FUNCTION_MARKER(0x8004760Cu, "LEVEL.BIN");
    w_u32(state + 0x94u, 0xFFFFFF10u);
    w_u32(state + 0x98u, 0xFFFFFF12u);
    w_u32(state + 0x20u, 0x00C80000u);
    w_u32(state + 0x2Cu, 0x00C80000u);
    w_u32(state + 0x38u, 0x00C80000u);
    w_u32(state + 0x78u, 0x00C80000u);
    w_u32(state + 0x84u, 0x00C80000u);
    first = r_u32(state + 0x98u);
    w_u32(state + 0x18u, 0u); w_u32(state + 0x24u, 0u); w_u32(state + 0x30u, 0u);
    w_u32(state + 0x48u, 0u); w_u32(state + 0x50u, 0u); w_u32(state + 0x54u, 0u);
    w_u32(state + 0x5Cu, 0u); w_u32(state + 0x60u, 0u); w_u32(state + 0x68u, 0u);
    w_u32(state + 0x80u, 0u);
    second = first;
    w_u32(state + 0x1Cu, first << 16);
    first = r_u32(state + 0x98u);
    w_u32(state + 0x28u, second << 16);
    second = r_u32(state + 0x94u);
    w_u32(state + 0x34u, first << 16);
    first = r_u32(state + 0x94u);
    w_u32(state + 0x4Cu, second << 16);
    second = first;
    w_u32(state + 0x58u, first << 16);
    first = r_u32(state + 0x94u);
    w_u32(state + 0x64u, second << 16);
    w_u32(state + 0x7Cu, first << 16);
    w_u32(state + 0x88u, first << 16);
    w_u32(state + 0x8Cu, 0u);
}

uint32 level_sub_8006D574(uint32 type, uint32 resource)
{
    uint32 object, instance, geometry;
    sint32 animation;
    FUNCTION_MARKER(0x8006D574u, "LEVEL.BIN");
    object = moh_draft_missing_call(0x80061168u, type, resource);
    if (!object) return 0u;
    instance = r_u32(object + 0x100u);
    if (!instance || !r_u32(instance + 8u)) return object;
    moh_draft_missing_call(0x8006A76Cu, object);
    geometry = r_u32(r_u32(object + 0x100u) + 8u);
    animation = (sint32)(sint16)r_u16(geometry + 0x94u);
    if (animation >= 0) {
        moh_draft_missing_call(0x8003F454u, object, (uint32)animation, 1u, 0u);
        moh_draft_missing_call(0x8003F8F0u, object);
    }
    return object;
}

void level_sub_8007BD1C(uint32 object)
{
    FUNCTION_MARKER(0x8007BD1Cu, "LEVEL.BIN");
    w_u32(object + 0x36Cu, 0x8007A6B0u);
    w_u32(object + 0x370u, 0x8007B8F4u);
    w_u32(object + 0x374u, 0x8007C1D0u);
    w_u32(object + 0x378u, 0x8007B4B8u);
    w_u32(object + 0x37Cu, 0x8007C030u);
    w_u32(object + 0x384u, 0x8007BA00u);
}

uint32 level_sub_80058C38(uint32 object, uint32 instruction, uint32 flags)
{
    FUNCTION_MARKER(0x80058C38u, "LEVEL.BIN");
    return moh_draft_missing_call(0x80058C78u, object, instruction);
}

uint32 level_sub_80058C58(uint32 object, uint32 instruction, uint32 flags)
{
    FUNCTION_MARKER(0x80058C58u, "LEVEL.BIN");
    return moh_draft_missing_call(0x80058C78u, object, instruction);
}

uint32 level_sub_80058F84(uint32 object, uint32 instruction, uint32 flags)
{
    uint32 field, mask;
    FUNCTION_MARKER(0x80058F84u, "LEVEL.BIN");
    field = r_u8(instruction) ? 16u : 12u;
    mask = moh_draft_missing_call(0x8005D148u, 1u, flags, r_u32(instruction + 4u), object);
    w_u32(object + field, r_u32(object + field) | mask);
    return instruction + 8u;
}

uint32 level_sub_800594AC(uint32 object, uint32 instruction, uint32 flags)
{
    uint32 value;
    FUNCTION_MARKER(0x800594ACu, "LEVEL.BIN");
    if (object) {
        value = moh_draft_missing_call(0x8005D148u, 1u, flags, (uint32)r_u8(instruction), object);
        moh_draft_missing_call(0x8008DAA8u, r_u32(object + 0x100u), value);
    }
    return instruction + 4u;
}

uint32 level_sub_80057D9C(uint32 object, uint32 instruction, uint32 flags)
{
    uint32 packed, instance, animation, controls, state, count, value, source, destination, bytes;
    uint32 first, second, material;
    sint32 mode, lookup;
    FUNCTION_MARKER(0x80057D9Cu, "LEVEL.BIN");
    packed = (uint32)r_u16(instruction);
    instance = r_u32(object + 0x100u);
    mode = (sint32)(sint16)packed >> 12;
    animation = packed & 0xFFFu;
    if (!r_u32(r_u32(instance + 0x78u) + animation * 4u))
        moh_draft_missing_call(0x80089B04u, 1u, 256u);
    controls = r_u32(0x80099EB4u);
    w_u16(controls, (uint16)animation);
    count = (uint32)r_u16(instruction + 4u);
    w_u16(controls + 2u, (uint16)count);
    if ((sint32)(sint16)count < 3) count = 3u;
    state = r_u32(0x80099EC4u);
    w_u16(controls + 4u, (uint16)count);
    value = (uint32)r_u16(instruction + 6u);
    w_u16(state + 24u, (uint16)value);
    if (value == 0xFFFFu) {
        if (!r_u32(instance + 0x74u)) {
            source = r_u32(r_u32(instance + 8u) + 0x98u) + animation * 16u;
            value = (uint32)r_u16(source + 4u);
        } else value = (uint32)r_u8(r_u32(r_u32(instance + 0x78u) + animation * 4u));
        w_u16(state + 24u, (uint16)(value - 1u));
    } else if ((sint32)(sint16)value == (sint32)(sint16)r_u16(controls + 2u)) {
        w_u16(state + 24u, 0xFFFFu);
    }
    if (!r_u32(0x80099EB0u)) {
        if (mode == 1) {
            if (!r_u32(instance + 0x74u)) {
                destination = r_u32(instance + 0x30u);
                bytes = (uint32)r_u16(instance + 0x3Cu) * 8u;
                source = r_u32(instance + 0x2Cu);
                moh_draft_missing_call(0x800154ECu, destination, source, bytes);
            } else {
                material = r_u32(object + 0x100u);
                first = r_u32(material + 0x98u); second = r_u32(material + 0x9Cu);
                w_u32(material + 0xA0u, first); w_u32(material + 0xA4u, second);
                material = r_u32(object + 0x100u);
                source = r_u32(material + 0x88u);
                if (source) {
                    bytes = (uint32)r_u16(material + 0x72u) * 8u;
                    destination = r_u32(material + 0x8Cu);
                    moh_draft_missing_call(0x800154ECu, destination, source, bytes);
                }
            }
            w_u16(instance + 0x3Eu, 4u);
            w_u16(instance + 0x40u, 0xFFFFu);
        } else if (mode == 2) {
            w_u16(instance + 0x3Eu, 0u);
        } else if ((sint32)(sint16)r_u16(instance + 0x3Eu) > 0) {
            w_u16(instance + 0x3Eu, (uint16)(r_u16(instance + 0x3Eu) - 1u));
        }
    }
    if (!r_u32(instance + 0x74u)) {
        source = r_u32(r_u32(instance + 8u) + 0x98u) + animation * 16u;
        if (!r_u16(source + 4u)) moh_draft_missing_call(0x80089B04u, 1u, 256u);
    } else {
        source = r_u32(r_u32(instance + 0x78u) + animation * 4u);
        value = (uint32)r_u8(source + 2u);
        if (!value) {
            material = r_u32(instance + 8u);
            source = r_u32(instance + 8u);
            w_u32(instance + 0x7Cu, r_u32(material + 0x38u));
            w_u32(instance + 0x80u, r_u32(source + 0x84u));
        } else {
            material = r_u32(instance + 8u);
            lookup = (sint32)moh_draft_missing_call(0x8006A910u, r_u32(material + (value - 1u) * 4u + 0xECu));
            if (lookup < 0) moh_draft_missing_call(0x80089B04u, 2u, 24u);
            material = moh_draft_missing_call(0x8006A8FCu, (uint32)lookup);
            w_u32(instance + 0x7Cu, r_u32(material + 0x38u));
            w_u32(instance + 0x80u, r_u32(material + 0x84u));
        }
    }
    moh_draft_missing_call(0x8003F1F8u, object);
    w_u16(r_u32(0x80099EC4u) + 28u, 0u);
    w_u32(object + 0x248u, 0u); w_u32(object + 0x244u, 0u); w_u32(object + 0x240u, 0u);
    w_u32(0x80099EC0u, 1u);
    return instruction + 8u;
}

void level_sub_800665BC(uint32 object, uint32 trigger)
{
    FUNCTION_MARKER(0x800665BCu, "LEVEL.BIN");
    if (!object && !trigger) moh_draft_missing_call(0x80015B28u, 0x8003C988u);
    else moh_draft_missing_call(0x800664A0u, object, trigger, 0u);
}

uint32 level_sub_8006D8AC(uint32 handle_ptr)
{
    uint32 slot;
    FUNCTION_MARKER(0x8006D8ACu, "LEVEL.BIN");
    if (!r_u32(handle_ptr)) return 1u;
    slot = moh_draft_missing_call(0x8006D898u, handle_ptr);
    if (r_u32(0x8009DA78u + slot * 4u) != (uint32)r_u16(handle_ptr + 2u)) return 1u;
    return r_u32(r_u32(0x8009D678u + slot * 4u)) == 0xDEDEDEDEu;
}

uint32 level_sub_8006D898(uint32 handle)
{
    FUNCTION_MARKER(0x8006D898u, "LEVEL.BIN");
    return (uint32)r_u16(handle);
}

void level_sub_80064858(uint32 format, ...)
{
    FUNCTION_MARKER(0x80064858u, "LEVEL.BIN");
    /* Original trace is disabled; argument-home stores are omitted for ordinary C ABI */
}

uint32 level_sub_80066C14(uint32 message, uint32 actor, uint32 trigger, uint32 filter,
    uint32 filter_actor, uint32 filter_trigger, uint32 filter_id)
{
    uint32 registry, node, previous = 0u, next, removed = 0u, matched, eligible, locals = 0u;
    FUNCTION_MARKER(0x80066C14u, "LEVEL.BIN");
    /* TODO: Ghidra switch pseudocode is partial; cases are reconstructed from complete MIPS */
    registry = r_u32(0x8009D5ACu);
    if (registry ? (sint32)message >= (sint32)r_u32(registry) : (sint32)message >= 0) {
        moh_draft_missing_call(0x80015B28u, 0x8003CB08u, message);
        return 0u;
    }
    node = r_u32(r_u32(0x8009D5B0u) + message * 4u);
    while (node) {
        next = r_u32(node + 32u);
        matched = 0u;
        eligible = 0u;
        if (actor) {
            if (!locals) {
                locals = sub_80036BF0(8u);
                if (!locals) abort();
                w_u32(locals, actor); w_u32(locals + 4u, filter_actor);
            }
            eligible = moh_draft_missing_call(0x8006D7D0u, node, locals);
        }
        if (eligible || (trigger && trigger == r_u32(node + 4u))) {
            if (!filter) matched = 1u;
            else if (r_u32(node + 8u) == filter) {
                switch (filter) {
                case 1u:
                case 2u:
                    matched = r_u32(node + 20u) == filter_id;
                    break;
                case 3u:
                case 4u:
                case 5u:
                    if (filter == 5u && r_u32(node + 20u) != filter_id) break;
                    if (filter_actor) {
                        if (!locals) {
                            locals = sub_80036BF0(8u);
                            if (!locals) abort();
                            w_u32(locals, actor); w_u32(locals + 4u, filter_actor);
                        }
                        matched = moh_draft_missing_call(0x8006D7D0u, node + 12u, locals + 4u);
                    }
                    if (!matched && filter_trigger && filter_trigger == r_u32(node + 16u)) matched = 1u;
                    break;
                default:
                    moh_draft_missing_call(0x80015B28u, 0x8003CB34u, filter);
                    if (locals) sub_80036E44(locals);
                    return 0u;
                }
            }
        }
        if (matched) {
            removed = 1u;
            if (!previous) w_u32(r_u32(0x8009D5B0u) + message * 4u, next);
            else w_u32(previous + 32u, next);
            moh_draft_missing_call(0x80066918u, node);
            if (filter_actor || filter_trigger) break;
        } else previous = node;
        node = next;
    }
    if (locals) sub_80036E44(locals);
    return removed;
}

void level_sub_80074AD8(uint32 object, uint32 resource)
{
    uint32 geometry, x, y, z;
    FUNCTION_MARKER(0x80074AD8u, "LEVEL.BIN");
    if (resource) {
        geometry = r_u32(resource + 4u);
        w_u32(object + 0x228u, (uint32)(sint32)(sint16)r_u16(geometry + 24u) << 16);
        w_u32(object + 0x22Cu, (uint32)(sint32)(sint16)r_u16(geometry + 26u) << 16);
        w_u32(object + 0x230u, (uint32)(sint32)(sint16)r_u16(geometry + 28u) << 16);
        w_u32(object + 0x20Cu, (uint32)(sint32)(sint16)r_u16(geometry + 16u) << 16);
        w_u32(object + 0x210u, (uint32)(sint32)(sint16)r_u16(geometry + 18u) << 16);
        w_u32(object + 0x214u, (uint32)(sint32)(sint16)r_u16(geometry + 20u) << 16);
        x = r_u32(object + 0x20Cu); y = r_u32(object + 0x210u); z = r_u32(object + 0x214u);
        w_u32(object + 0x258u, x); w_u32(object + 0x25Cu, y); w_u32(object + 0x260u, z);
    } else {
        w_u32(object + 0x214u, 0u); w_u32(object + 0x210u, 0u); w_u32(object + 0x20Cu, 0u);
        w_u32(object + 0x230u, 0u); w_u32(object + 0x22Cu, 0u); w_u32(object + 0x228u, 0u);
    }
    x = r_u32(object + 0x20Cu); y = r_u32(object + 0x210u); z = r_u32(object + 0x214u);
    w_u32(object + 0x3BCu, x); w_u32(object + 0x3C0u, y); w_u32(object + 0x3C4u, z);
}

void level_sub_8007BC0C(uint32 player, uint32 mode)
{
    uint32 data, value, camera, camera_data;
    FUNCTION_MARKER(0x8007BC0Cu, "LEVEL.BIN");
    data = r_u32(player + 0x388u);
    w_u32(player + 12u, 0x29u);
    w_u32(player + 16u, 0x20000u);
    value = r_u8(data + 0x152u) ? 0x22000u : 0x21000u;
    w_u32(player + 16u, value);
    w_u32(data + 0x8Cu, 1u);
    value = r_u32(data);
    w_u8(data + 0x54u, 0u);
    value &= 0x30480000u;
    w_u32(data, value); w_u32(data + 4u, value);
    w_u32(data + 0x40u, (uint32)(sint32)(sint16)r_u16(0x80093644u));
    value = mode ? (uint32)r_u8(data + 0x152u) : 0xFFFFFFFFu;
    moh_draft_missing_call(0x80079718u, player, value);
    w_u32(data + 0x1C0u, 0xFFFFFFFFu);
    w_u32(data + 0x13Cu, 0u); w_u32(data + 0x12Cu, 0u);
    w_u32(data + 0x50u, 0u); w_u32(data + 0x4Cu, 0u); w_u32(data + 0x1C4u, 0u);
    w_u16(player + 0x2DAu, 0xFF9Cu);
    w_u16(player + 0x2E8u, 200u); w_u16(player + 0x2ECu, 100u);
    camera = r_u32(0x8009D638u);
    w_u16(player + 0x3C8u, 128u);
    if (camera) {
        camera_data = r_u32(camera + 0x388u);
        w_u32(player + 0x228u, 0u);
        w_u32(data + 0x178u, 0u); w_u32(data + 0x17Cu, 0u); w_u32(data + 0x180u, 0u);
        moh_draft_missing_call(0x8007BADCu, player, data, camera_data);
    }
}

void level_sub_8007BADC(uint32 player, uint32 state, uint32 camera_state)
{
    sint32 x, z, angle, minimum, sine, cosine, numerator, quotient;
    uint32 distance;
    FUNCTION_MARKER(0x8007BADCu, "LEVEL.BIN");
    x = (sint32)(sint16)r_u16(camera_state + 0x26u);
    z = (sint32)(sint16)r_u16(camera_state + 0x2Eu);
    angle = (sint32)(sint16)r_u16(state + 0x17Au);
    if (x < 0) x = -x;
    if (z < 0) z = -z;
    minimum = x < z ? x : z;
    distance = (uint32)(x + z - (minimum >> 1));
    sine = (sint32)moh_draft_missing_call(0x8001C9A8u, (uint32)angle);
    cosine = (sint32)moh_draft_missing_call(0x8001C908u, (uint32)angle);
    numerator = (sint32)(0u - distance * (uint32)sine);
    if (!cosine || (cosine == -1 && numerator == (sint32)0x80000000u)) {
        /* Original signed division raises break 0x1C00 or 0x1800 */
        abort();
    }
    quotient = numerator / cosine;
    distance = (uint32)quotient + r_u32(camera_state + 0x98u);
    w_u32(camera_state + 0x28u, distance << 16);
    if (!r_u32(0x8009E830u)) {
        if ((sint32)(sint16)r_u16(player + 0x22Au) < -300) {
            if ((sint32)r_u32(0x800AA164u) < 2) w_u32(0x800AA164u, 2u);
        } else if ((sint32)r_u32(0x800AA164u) > 1) w_u32(0x800AA164u, 1u);
    }
}

uint32 level_sub_80076240(uint32 controller, uint32 mapping, uint32 config)

{
  uint32 uVar1;
  uint32 uVar2;
  uint32 uVar3;
  uint32 uVar4;
  
  FUNCTION_MARKER(0x80076240u, "LEVEL.BIN");
  do {
    if (mapping == 3) {
      moh_draft_missing_call(0x800756D8u,controller,0,2,0);
      moh_draft_missing_call(0x800756D8u,controller,1,0x40,0);
      moh_draft_missing_call(0x800756D8u,controller,2,0x20,0);
      moh_draft_missing_call(0x800756D8u,controller,3,4,0);
      moh_draft_missing_call(0x800756D8u,controller,4,8,0);
      moh_draft_missing_call(0x800756D8u,controller,5,8,0);
      moh_draft_missing_call(0x800756D8u,controller,6,0x800,0);
      moh_draft_missing_call(0x800756D8u,controller,7,0x400,0);
      moh_draft_missing_call(0x800756D8u,controller,8,1,0);
      moh_draft_missing_call(0x80075C6Cu,controller,0,3,0,0,r_u32(0x8009E3A8u),0);
      moh_draft_missing_call(0x80075C6Cu,controller,1,2,0,0,r_u32(0x8009E3A8u),0);
      moh_draft_missing_call(0x80075C6Cu,controller,2,1,0,0,0x3c0000,config);
      moh_draft_missing_call(0x80075C6Cu,controller,3,0,0,0,0x3c0000,0);
      moh_draft_missing_call(0x80075810u,controller,0,0x1000,0x4000,0x10,0,r_u32(0x8009E3A8u),0,1);
      moh_draft_missing_call(0x80075810u,controller,1,0x8000,0x2000,0x10,0,r_u32(0x8009E3A8u),0,1);
      moh_draft_missing_call(0x80075810u,controller,2,0x1000,0x4000,0x10,1,0x3c0000,config,1);
      uVar1 = 0x8000;
      uVar2 = 0x2000;
      uVar3 = 0x10;
      uVar4 = 1;
LAB_80076fdc:
      moh_draft_missing_call(0x80075810u,controller,3,uVar1,uVar2,uVar3,uVar4,0x3c0000,0,1);
      moh_draft_missing_call(0x800759A4u,controller,0,0x200000);
      moh_draft_missing_call(0x800759A4u,controller,1,0x200000);
      moh_draft_missing_call(0x800759A4u,controller,2,0x20000);
      moh_draft_missing_call(0x800759A4u,controller,3,0x20000);
      moh_draft_missing_call(0x80076094u,controller,1);
      w_u32(0x8009E398u + controller * 4u, 1u);
      return 0;
    }
    if (mapping < 4) {
      if (mapping == 1) {
        moh_draft_missing_call(0x800756D8u,controller,0,0x40,0);
        moh_draft_missing_call(0x800756D8u,controller,1,0x10,0);
        moh_draft_missing_call(0x800756D8u,controller,2,0x20,0);
        moh_draft_missing_call(0x800756D8u,controller,3,1,0);
        moh_draft_missing_call(0x800756D8u,controller,4,0x80,0);
        moh_draft_missing_call(0x800756D8u,controller,5,0x80,0);
        moh_draft_missing_call(0x800756D8u,controller,6,0x800,0);
        moh_draft_missing_call(0x800756D8u,controller,7,0x200,0);
        moh_draft_missing_call(0x800756D8u,controller,8,2,0);
        moh_draft_missing_call(0x80075C6Cu,controller,0,3,0,0,r_u32(0x8009E3A8u),0);
        moh_draft_missing_call(0x80075C6Cu,controller,1,0,0,0,r_u32(0x8009E3A8u),0);
        moh_draft_missing_call(0x80075C6Cu,controller,2,1,0,0,0x3c0000,config);
        moh_draft_missing_call(0x80075C6Cu,controller,3,2,0,0,0x3c0000,0);
        moh_draft_missing_call(0x80075810u,controller,0,0x1000,0x4000,0x10,0,r_u32(0x8009E3A8u),0,1);
        moh_draft_missing_call(0x80075810u,controller,1,4,8,0,0,r_u32(0x8009E3A8u),0,1);
        moh_draft_missing_call(0x80075810u,controller,2,0x1000,0x4000,0x10,1,0x3c0000,config,1);
        moh_draft_missing_call(0x80075810u,controller,3,0x8000,0x2000,0,0,0x3c0000,0,1);
        moh_draft_missing_call(0x800759A4u,controller,0,0x200000);
        moh_draft_missing_call(0x800759A4u,controller,1,0x200000);
        moh_draft_missing_call(0x800759A4u,controller,2,0x20000);
        moh_draft_missing_call(0x800759A4u,controller,3,0x20000);
        moh_draft_missing_call(0x80076094u,controller,1);
        w_u32(0x8009E398u + controller * 4u, 1u);
        return 0;
      }
      if (1 < mapping) {
        moh_draft_missing_call(0x800756D8u,controller,0,2,0);
        moh_draft_missing_call(0x800756D8u,controller,1,4,0);
        moh_draft_missing_call(0x800756D8u,controller,2,0x40,0);
        moh_draft_missing_call(0x800756D8u,controller,3,0x400,0);
        moh_draft_missing_call(0x800756D8u,controller,4,8,0);
        moh_draft_missing_call(0x800756D8u,controller,5,8,0);
        moh_draft_missing_call(0x800756D8u,controller,6,0x800,0);
        moh_draft_missing_call(0x800756D8u,controller,7,0x80,0);
        moh_draft_missing_call(0x800756D8u,controller,8,1,0);
        moh_draft_missing_call(0x80075C6Cu,controller,0,3,0,0,r_u32(0x8009E3A8u),0);
        moh_draft_missing_call(0x80075C6Cu,controller,1,0,0,0,r_u32(0x8009E3A8u),0);
        moh_draft_missing_call(0x80075C6Cu,controller,2,1,0,0,0x3c0000,config);
        moh_draft_missing_call(0x80075C6Cu,controller,3,2,0,0,0x3c0000,0);
        moh_draft_missing_call(0x80075810u,controller,0,0x1000,0x4000,0x20,0,r_u32(0x8009E3A8u),0,1);
        moh_draft_missing_call(0x80075810u,controller,1,0x8000,0x2000,0x20,1,r_u32(0x8009E3A8u),0,1);
        moh_draft_missing_call(0x80075810u,controller,2,0x1000,0x4000,0x20,1,0x3c0000,config,1);
        moh_draft_missing_call(0x80075810u,controller,3,0x8000,0x2000,0x20,0,0x3c0000,0,1);
        moh_draft_missing_call(0x800759A4u,controller,0,0x200000);
        moh_draft_missing_call(0x800759A4u,controller,1,0x200000);
        moh_draft_missing_call(0x800759A4u,controller,2,0x20000);
        moh_draft_missing_call(0x800759A4u,controller,3,0x20000);
        moh_draft_missing_call(0x80076094u,controller,1);
        w_u32(0x8009E398u + controller * 4u, 1u);
        return 0;
      }
      moh_draft_missing_call(0x800756D8u,controller,0,2,0);
      moh_draft_missing_call(0x800756D8u,controller,1,4,0);
      moh_draft_missing_call(0x800756D8u,controller,2,0x40,0);
      moh_draft_missing_call(0x800756D8u,controller,3,1,0);
      moh_draft_missing_call(0x800756D8u,controller,4,8,0);
      moh_draft_missing_call(0x800756D8u,controller,5,8,0);
      moh_draft_missing_call(0x800756D8u,controller,6,0x800,0);
      moh_draft_missing_call(0x800756D8u,controller,7,0x400,0);
      moh_draft_missing_call(0x800756D8u,controller,8,0x200,0);
      moh_draft_missing_call(0x80075C6Cu,controller,0,3,0,0,r_u32(0x8009E3A8u),0);
      moh_draft_missing_call(0x80075C6Cu,controller,1,2,0,0,r_u32(0x8009E3A8u),0);
      moh_draft_missing_call(0x80075C6Cu,controller,2,1,0,0,0x3c0000,config);
      moh_draft_missing_call(0x80075C6Cu,controller,3,0,0,0,0x3c0000,0);
      moh_draft_missing_call(0x80075810u,controller,0,0x1000,0x4000,0x10,0,r_u32(0x8009E3A8u),0,1);
      moh_draft_missing_call(0x80075810u,controller,1,0x8000,0x2000,0x10,0,r_u32(0x8009E3A8u),0,1);
      moh_draft_missing_call(0x80075810u,controller,2,0x1000,0x4000,0x10,1,0x3c0000,config,1);
      moh_draft_missing_call(0x80075810u,controller,3,0x8000,0x2000,0x10,1,0x3c0000,0,1);
      moh_draft_missing_call(0x800759A4u,controller,0,0x200000);
      moh_draft_missing_call(0x800759A4u,controller,1,0x200000);
      moh_draft_missing_call(0x800759A4u,controller,2,0x20000);
      moh_draft_missing_call(0x800759A4u,controller,3,0x20000);
      moh_draft_missing_call(0x80076094u,controller,1);
LAB_80076760:
      w_u32(0x8009E398u + controller * 4u, 0u);
      return 0;
    }
    if (mapping == 5) {
      moh_draft_missing_call(0x800756D8u,controller,0,0x40,0);
      moh_draft_missing_call(0x800756D8u,controller,1,0x10,0);
      moh_draft_missing_call(0x800756D8u,controller,2,0x20,0);
      moh_draft_missing_call(0x800756D8u,controller,3,1,2);
      moh_draft_missing_call(0x800756D8u,controller,4,0x80,0);
      moh_draft_missing_call(0x800756D8u,controller,5,0x80,0);
      moh_draft_missing_call(0x800756D8u,controller,6,0x800,0);
      moh_draft_missing_call(0x800756D8u,controller,7,3,0);
      moh_draft_missing_call(0x800756D8u,controller,8,2,0);
      moh_draft_missing_call(0x80075C6Cu,controller,0,3,0,0,r_u32(0x8009E3A8u),0);
      moh_draft_missing_call(0x80075C6Cu,controller,1,2,0,0,r_u32(0x8009E3A8u),0);
      moh_draft_missing_call(0x80075C6Cu,controller,2,1,0,0,0x680000,config);
      moh_draft_missing_call(0x80075C6Cu,controller,3,0,0,0,0x280000,0);
      moh_draft_missing_call(0x80075810u,controller,0,0x1000,0x4000,2,0,r_u32(0x8009E3A8u),0,1);
      moh_draft_missing_call(0x80075810u,controller,1,4,8,0,1,r_u32(0x8009E3A8u),0,1);
      moh_draft_missing_call(0x80075810u,controller,2,0x1000,0x4000,2,1,0x3c0000,config,1);
      moh_draft_missing_call(0x80075810u,controller,3,0x8000,0x2000,0,1,0x3c0000,0,1);
      moh_draft_missing_call(0x800759A4u,controller,0,0x200000);
      moh_draft_missing_call(0x800759A4u,controller,1,0x200000);
      moh_draft_missing_call(0x800759A4u,controller,2,0x20000);
      moh_draft_missing_call(0x800759A4u,controller,3,0x20000);
      moh_draft_missing_call(0x80076094u,controller,2);
      goto LAB_80076760;
    }
    if (mapping < 5) {
      moh_draft_missing_call(0x800756D8u,controller,0,0x80,0);
      moh_draft_missing_call(0x800756D8u,controller,1,0x20,0);
      moh_draft_missing_call(0x800756D8u,controller,2,0x10,0);
      moh_draft_missing_call(0x800756D8u,controller,3,1,0);
      moh_draft_missing_call(0x800756D8u,controller,4,0x40,0);
      moh_draft_missing_call(0x800756D8u,controller,5,0x40,0);
      moh_draft_missing_call(0x800756D8u,controller,6,0x800,0);
      moh_draft_missing_call(0x800756D8u,controller,7,0x400,0);
      moh_draft_missing_call(0x800756D8u,controller,8,2,0);
      moh_draft_missing_call(0x80075C6Cu,controller,0,3,0,0,r_u32(0x8009E3A8u),0);
      moh_draft_missing_call(0x80075C6Cu,controller,1,2,0,0,r_u32(0x8009E3A8u),0);
      moh_draft_missing_call(0x80075C6Cu,controller,2,1,0,0,0x3c0000,config);
      moh_draft_missing_call(0x80075C6Cu,controller,3,0,0,0,0x3c0000,0);
      moh_draft_missing_call(0x80075810u,controller,0,0x1000,0x4000,0x40,0,r_u32(0x8009E3A8u),0,1);
      moh_draft_missing_call(0x80075810u,controller,1,0x8000,0x2000,0,0,r_u32(0x8009E3A8u),0,1);
      moh_draft_missing_call(0x80075810u,controller,2,0x1000,0x4000,0x40,1,0x3c0000,config,1);
      uVar1 = 4;
      uVar2 = 8;
      uVar3 = 0;
      uVar4 = 0;
      goto LAB_80076fdc;
    }
    if (mapping != 0xffffffff) {
      return 0;
    }
    mapping = 3;
    if (r_u8(0x80093B78u + controller) == 0u) {
      mapping = 2;
      config = 0;
    }
    else {
      config = 0;
    }
  } while (1);
}



void level_sub_800756D8(uint32 controller, uint32 slot, uint32 buttons, uint32 mode)
{
    uint32 offset = (controller * 9u + slot) * 4u;
    FUNCTION_MARKER(0x800756D8u, "LEVEL.BIN");
    w_u32(0x800B7790u + offset, buttons);
    w_u32(0x800B77E0u + offset, mode);
}

void level_sub_800759A4(uint32 controller, uint32 slot, uint32 scale)
{
    uint32 axis = 0x800B75F0u + controller * 0xD0u + slot * 0x34u;
    FUNCTION_MARKER(0x800759A4u, "LEVEL.BIN");
    w_u32(axis + 0x24u, 0u);
    w_u32(axis + 0x28u, scale);
    w_u32(axis + 0x2Cu, 0u);
}

void level_sub_80076094(uint32 controller, uint32 mode)
{
    FUNCTION_MARKER(0x80076094u, "LEVEL.BIN");
    w_u32(0x8009E3A0u + controller * 4u, mode);
}

void level_sub_80076118(uint32 controller)
{
    uint32 slot = 0u;
    FUNCTION_MARKER(0x80076118u, "LEVEL.BIN");
    do {
        moh_draft_missing_call(0x800760C8u, controller, slot);
        slot += 1u;
    } while (slot < 4u);
}

void level_sub_800760C8(uint32 controller, uint32 slot)
{
    uint32 axis = 0x800B7470u + controller * 32u + slot * 8u;
    FUNCTION_MARKER(0x800760C8u, "LEVEL.BIN");
    w_u8(axis + 1u, 0xFFu);
    w_u8(axis, 0u);
    w_u8(axis + 2u, 0x5Bu);
    w_u8(axis + 3u, 0x5Bu);
    w_u8(axis + 4u, 0x5Bu);
    moh_draft_missing_call(0x80076160u, controller, slot);
}

void level_sub_80076160(uint32 controller, uint32 slot)
{
    uint32 index = 0u;
    uint32 axis = 0x800B7470u + controller * 32u + slot * 8u;
    uint32 record = 0x800B74B0u + controller * 0xA0u;
    FUNCTION_MARKER(0x80076160u, "LEVEL.BIN");
    do {
        if (r_u32(record) == slot) {
            uint32 scale = r_u32(record + 0x20u);
            w_u8(record + 0xCu, r_u8(axis));
            w_u8(record + 0xDu, r_u8(axis + 1u));
            w_u8(record + 0xEu, r_u8(axis + 2u));
            w_u8(record + 0xFu, r_u8(axis + 3u));
            w_u8(record + 0x10u, r_u8(axis + 4u));
            moh_draft_missing_call(0x80075D1Cu, controller, index, scale, 1u);
        }
        index += 1u;
        record += 0x28u;
    } while (index < 4u);
}

uint32 level_sub_8007BA20(uint32 player, uint32 state)
{
    FUNCTION_MARKER(0x8007BA20u, "LEVEL.BIN");
    if (!(r_u32(state) & 0x20u)) {
        switch (r_u8(state + 0x55u)) {
        case 0: case 4: case 5: case 8: case 9: case 12: return 7u;
        case 1: case 6: case 11: return 8u;
        case 2: case 3: case 7: case 10: return 9u;
        }
    } else {
        switch (r_u8(state + 0x55u)) {
        case 0: case 4: case 8: case 9: case 12: return 0u;
        case 1: case 6: case 11: return 1u;
        case 2: return 5u;
        case 3: case 7: case 10: return 6u;
        case 5: return 2u;
        }
    }
    return 7u;
}

void level_sub_80046874(uint32 player)

{
  uint32 state;
  
  state = r_u32(player + 0x388u);
  FUNCTION_MARKER(0x80046874u, "LEVEL.BIN");
  w_u8(state + 0xbfu, 0x2cu);
  w_u8(state + 0xe7u, 0x2cu);
  w_u8(state + 0xbbu, 9u);
  w_u8(state + 0xe3u, 9u);
  w_u8(state + 0xbcu, 0x80u);
  w_u8(state + 0xbdu, 0x80u);
  w_u8(state + 0xbeu, 0x80u);
  w_u8(state + 0xe4u, 0x80u);
  w_u8(state + 0xe5u, 0x80u);
  w_u8(state + 0xe6u, 0x80u);
  w_u16(state + 0xc0u, 0x19fu);
  w_u16(state + 0xc2u, 0xfu);
  w_u16(state + 200u, 0x1bfu);
  w_u16(state + 0xcau, 0xfu);
  w_u16(state + 0xd0u, 0x19fu);
  w_u16(state + 0xd2u, 0x23u);
  w_u16(state + 0xd8u, 0x1bfu);
  w_u16(state + 0xdau, 0x23u);
  w_u16(state + 0xe8u, 0x19fu);
  w_u16(state + 0xeau, 0xfu);
  w_u16(state + 0xf0u, 0x1bfu);
  w_u16(state + 0xf2u, 0xfu);
  w_u16(state + 0xf8u, 0x19fu);
  w_u16(state + 0xfau, 0x23u);
  w_u16(state + 0x100u, 0x1bfu);
  w_u16(state + 0x102u, 0x23u);
  w_u16(state + 0xf6u, 0xfu);
  w_u16(state + 0xceu, 0xfu);
  w_u8(state + 0x10au, 0u);
  w_u8(state + 0x10fu, 0u);
  moh_draft_missing_call(0x800467ECu, player);
  moh_draft_missing_call(0x8005F28Cu,state + 0x10c,0x1c5,0x12,100,100,100,0,0xffffffff,0xff,
               1u << (r_u8(state + 0x152u) & 31u),0);
  moh_draft_missing_call(0x8005F28Cu,state + 0x108,0x17c,0x12,100,100,100,0,0xffffffff,0xff,
               1u << (r_u8(state + 0x152u) & 31u),0);
  moh_draft_missing_call(0x80046574u, player);
  return;
}



void level_sub_80046700(uint32 destination, sint32 value, uint32 offset, uint32 kind)
{
    FUNCTION_MARKER(0x80046700u, "LEVEL.BIN");
    if (kind == 3u) {
        if (value < 100) {
            w_u8(destination + offset, 0x20u);
        } else {
            w_u8(destination + offset, (uint32)(value / 100) + 0x30u);
            value %= 100;
        }
        offset += 1u;
    }
    if (value < 10) {
        uint32 previous = r_u8(destination + offset - 1u);
        if (previous == 0x20u || kind == 2u)
            w_u8(destination + offset, 0x20u);
        else
            w_u8(destination + offset, 0x30u);
    } else {
        w_u8(destination + offset, (uint32)(value / 10) + 0x30u);
        value %= 10;
    }
    w_u8(destination + offset + 1u, (uint32)value + 0x30u);
}

void level_sub_8008AB48(void)
{
    FUNCTION_MARKER(0x8008AB48u, "LEVEL.BIN");
    moh_draft_missing_call(0x8001DF40u, 0u);
    moh_draft_missing_call(0x8001C698u, 0u);
    moh_draft_missing_call(0x80019640u, 0u);
    w_u8(0x80093B50u, 0u);
    w_u8(0x80093B30u, 0u);
    moh_draft_missing_call(0x8001DD80u, 0u);
    moh_draft_missing_call(0x800158DCu, 0u, 1u);
    moh_draft_missing_call(0x80049A9Cu);
    moh_draft_missing_call(0x8001AD98u);
    moh_draft_missing_call(0x80015DB8u, 0u);
    moh_draft_missing_call(0x80015DB8u, 0u);
    moh_draft_missing_call(0x8001DF40u, r_u32(0x800A0154u));
    moh_draft_missing_call(0x8001C698u, r_u32(0x800A0158u));
    w_u32(0x800A0128u, 0u);
    w_u32(0x800AA184u, 0xFFFFFFFFu);
    w_u32(0x800A0144u, 0u);
}

void level_sub_80049A9C(void)
{
    uint32 result_buffer;
    sint32 status;
    FUNCTION_MARKER(0x80049A9Cu, "LEVEL.BIN");
    result_buffer = sub_80036BF0(8u);
    if (!result_buffer) abort();
    do {
        status = (sint32)moh_draft_missing_call(0x800195D0u, result_buffer);
    } while (status > 0);
    sub_80036E44(result_buffer);
}
