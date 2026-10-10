#include "game_draft_signatures.h"
#include "game_level_signatures.h"
#include "xport_trace.h"
#include "psx.h"
#include <stdio.h>
#include <stdlib.h>

void level_sub_80053AC0(void)
{
    uint32 material = r_u32(0x8009CC64u), index = 0u, offset = 220u, record, timer, count, frame, frames;
    FUNCTION_MARKER(0x80053AC0u, "LEVEL.BIN");
    if (!r_u32(material + 216u)) return;
    do {
        record = material + offset;
        if (r_u32(record)) {
            timer = r_u32(record + 16u) - 1u; w_u32(record + 16u, timer);
            if ((sint32)timer <= 0) {
                frame = r_u32(record + 8u); count = r_u32(record); ++frame; w_u32(record + 8u, frame);
                if (frame >= count) w_u32(record + 8u, 0u);
                frame = r_u32(record + 8u); frames = r_u32(record + 4u);
                w_u32(record + 16u, r_u8(frames + frame * 4u + 3u));
                frame = r_u32(record + 8u); w_u32(record + 12u, r_u32(frames + frame * 4u));
            }
        }
        count = r_u32(r_u32(0x8009CC64u) + 216u); ++index; offset += 20u;
    } while (index < count);
}

void level_sub_80053C00(uint32 object)
{
    FUNCTION_MARKER(0x80053C00u, "LEVEL.BIN");
    (void)object; level_sub_80053AC0();
}

void level_sub_80082E88(uint32 object)
{
    FUNCTION_MARKER(0x80082E88u, "LEVEL.BIN");
    /* Original callback consists only of JR RA and its NOP delay slot */
    (void)object;
}

void level_sub_8006B8F0(uint32 object)
{
    uint32 flags, owner, definition, retained, callback;
    sint32 type;
    FUNCTION_MARKER(0x8006B8F0u, "LEVEL.BIN");
    if (!object) return;
    flags = r_u32(object + 12u); if (flags & 0x80000u) return;
    owner = r_u32(object + 28u); w_u32(object + 12u, flags | 0x80000u);
    if (owner) {
        retained = (flags >> 29) & 1u; definition = r_u32(owner + 4u); type = (sint32)r_u32(definition + 12u);
        if (type == 2 || type == 3 || type == 6) { if (r_u32(definition + 48u) & 8u) retained = 0u; }
        else retained = 0u;
        if (retained) { owner = r_u32(object + 28u); w_u32(owner, r_u32(owner) | 2u); }
        owner = r_u32(object + 28u); w_u32(owner, r_u32(owner) & 0xFFFFFFFEu);
    }
    moh_draft_missing_call(0x80042524u, object); moh_draft_missing_call(0x8006B89Cu, object); moh_draft_missing_call(0x8006DA50u, object);
    callback = r_u32(object + 900u); if (callback) moh_draft_indirect_call(callback, object);
}

void level_sub_8004F488(uint32 object)
{
    uint32 state = r_u32(object + 904u), value, first, second, third, index, destination;
    FUNCTION_MARKER(0x8004F488u, "LEVEL.BIN");
    value = r_u32(state + 4u);
    if ((sint32)value > 0) {
        --value; w_u32(state + 4u, value);
        if (!value && (r_u32(state) & 1u)) w_u32(object + 880u, 0x8004F670u);
        return;
    }
    if ((sint32)r_u32(state + 8u) > 0) {
        if (r_u32(state) & 1u) {
            value = r_u8(state + 24u); first = r_u8(state + 16u); second = r_u8(state + 18u); third = r_u8(state + 20u);
            w_u8(state + 24u, (uint8)(value + first));
            value = r_u8(state + 25u); first = r_u8(state + 26u);
            w_u8(state + 25u, (uint8)(value + second)); w_u8(state + 26u, (uint8)(first + third));
        }
        if (r_u32(state) & 4u) {
            value = r_u32(state + 108u) - r_u32(state + 112u); w_u32(state + 108u, value);
            if ((sint32)value < 0) value = 0u;
            w_u32(state + 108u, value);
        }
        w_u32(state + 8u, r_u32(state + 8u) - 1u); return;
    }
    for (index = 0u; index < 6u; ++index) {
        value = r_u32(0x8009C83Cu); if (index >= 3u) value = 1u - value;
        destination = state + value * 40u + 32u + index % 3u;
        value = r_u8(state + 24u + index % 3u); w_u8(destination, (uint8)value);
    }
    value = r_u32(state + 12u);
    if ((sint32)value > 0) w_u32(state + 12u, value - 1u);
    else level_sub_8006B8F0(object);
}

void level_sub_8004CEE4(uint32 object, uint32 material, uint32 matrix, uint32 records, uint32 count, uint32 camera_mode)
{
    uint32 record, index, axis, first, second, vertices, quaternion, packed0, packed1;
    uint16 point[3], offset[3], rotation[9];
    sint32 qx, qy, qz, qw, value, bound;
    uint32 products[3][3];
    FUNCTION_MARKER(0x8004CEE4u, "LEVEL.BIN");
    count &= 0xFFFFu;
    for (index = 0u, record = records; index < count; ++index, record += 72u) {
        if (!r_u32(record + 24u)) continue;
        first = r_u16(record + 28u); second = r_u16(record + 30u);
        if (first == 0xFFFEu || first == 0xFFFFu) {
            point[0] = r_u16(record + 64u); point[1] = r_u16(record + 66u); point[2] = r_u16(record + 68u);
            if (first == 0xFFFEu) point[1] = (uint16)(point[1] + r_u16(r_u32(r_u32(0x8009D638u) + 0x388u) + 148u));
        } else {
            if (r_u32(material + 132u)) {
                offset[0] = offset[1] = offset[2] = 0u;
                vertices = r_u32(material + 148u); first = vertices + first * 8u; second = vertices + second * 8u;
                if ((r_u32(record + 64u) || r_u16(record + 68u)) && r_u32(material + 136u)) {
                    quaternion = r_u32(material + 136u) + (r_u16(record + 28u) * 8u);
                    packed0 = r_u32(quaternion); packed1 = r_u32(quaternion + 4u);
                    qx = (sint16)packed0; qy = (sint16)(packed0 >> 16); qz = (sint16)packed1; qw = (sint16)(packed1 >> 16);
                    xport_gte_write_control(0u, ((uint32)qw << 16) | (uint16)qx);
                    packed1 = ((uint32)qy << 16) | (uint16)qz;
                    xport_gte_write_control(1u, packed1); xport_gte_write_control(3u, packed1);
                    xport_gte_write_control(2u, ((uint32)qw << 16) | (uint16)(-qz)); xport_gte_write_control(4u, (uint32)(-qx));
                    xport_gte_write_data(0u, ((uint32)qw << 17) | (uint16)(qx * 2)); xport_gte_write_data(1u, 0u);
                    xport_gte_execute(0x486012u);
                    for (axis = 0u; axis < 3u; ++axis) products[0][axis] = xport_gte_read_data(25u + axis);
                    xport_gte_write_data(0u, (uint16)(qy * 2)); xport_gte_write_data(1u, (uint32)qw << 1);
                    xport_gte_execute(0x486012u);
                    for (axis = 0u; axis < 3u; ++axis) products[1][axis] = xport_gte_read_data(25u + axis);
                    xport_gte_write_data(0u, (0u - ((uint32)qy << 1)) << 16); xport_gte_write_data(1u, (uint32)qx << 1);
                    xport_gte_execute(0x486012u);
                    products[2][0] = xport_gte_read_data(25u); products[2][1] = xport_gte_read_data(26u);
                    rotation[0] = (uint16)(products[0][0] - 0x1000u); rotation[1] = (uint16)products[1][0]; rotation[2] = (uint16)products[2][0];
                    rotation[3] = (uint16)products[0][1]; rotation[4] = (uint16)(products[1][1] - 0x1000u); rotation[5] = (uint16)products[2][1];
                    rotation[6] = (uint16)products[0][2]; rotation[7] = (uint16)products[1][2];
                    rotation[8] = (uint16)((((qw * qw) >> 12) + ((qz * qz) >> 12)) * 2 - 0x1000);
                    for (axis = 0u; axis < 4u; ++axis) xport_gte_write_control(axis, rotation[axis * 2u] | ((uint32)rotation[axis * 2u + 1u] << 16));
                    /* TODO Original matrix padding is unspecified and unused by this command */
                    xport_gte_write_control(4u, (uint32)(sint32)(sint16)rotation[8]);
                    for (axis = 5u; axis < 8u; ++axis) xport_gte_write_control(axis, 0u);
                    xport_gte_write_data(0u, r_u32(record + 64u)); xport_gte_write_data(1u, r_u32(record + 68u));
                    xport_gte_execute(0x480012u);
                    for (axis = 0u; axis < 3u; ++axis) offset[axis] = (uint16)xport_gte_read_data(25u + axis);
                }
                for (axis = 0u; axis < 3u; ++axis) point[axis] = (uint16)(offset[axis] + (((sint16)r_u16(first + axis * 2u) + (sint16)r_u16(second + axis * 2u)) >> 1));
            } else {
                vertices = r_u32(r_u32(record + 24u) + 44u); first = vertices + first * 8u; second = vertices + second * 8u;
                for (axis = 0u; axis < 3u; ++axis) point[axis] = (uint16)(((sint16)r_u16(first + axis * 2u) + (sint16)r_u16(second + axis * 2u)) >> 1);
            }
            if (camera_mode) point[1] = (uint16)(point[1] + r_u16(r_u32(r_u32(0x8009D638u) + 0x388u) + 148u));
        }
        for (axis = 0u; axis < 8u; ++axis) xport_gte_write_control(axis, r_u32(matrix + axis * 4u));
        /* TODO Original vector padding is unspecified and unused by this command */
        xport_gte_write_data(0u, point[0] | ((uint32)point[1] << 16)); xport_gte_write_data(1u, point[2]); xport_gte_execute(0x480012u);
        for (axis = 0u; axis < 3u; ++axis) w_u32(record + 32u + axis * 4u, xport_gte_read_data(25u + axis));
        for (axis = 0u; axis < 3u; ++axis) {
            value = (sint32)(r_u32(record + 32u + axis * 4u) - (uint32)(sint32)(sint16)r_u16(record + 52u));
            bound = (sint16)r_u16(object + 0x308u + axis * 2u);
            if (bound < value) value = bound;
            w_u16(object + 0x308u + axis * 2u, (uint16)value);
        }
        for (axis = 0u; axis < 3u; ++axis) {
            value = (sint32)(r_u32(record + 32u + axis * 4u) + (uint32)(sint32)(sint16)r_u16(record + 52u));
            bound = (sint16)r_u16(object + 0x30Eu + axis * 2u);
            if (value <= bound) value = bound;
            w_u16(object + 0x30Eu + axis * 2u, (uint16)value);
        }
    }
}

void level_sub_8004D43C(uint32 object)
{
    uint32 local = 0u, matrix, index, count;
    sint32 bone = (sint16)r_u16(object + 0x2EAu);
    FUNCTION_MARKER(0x8004D43Cu, "LEVEL.BIN");
    if (bone == -1) {
        if (r_u32(object + 12u) & 1u) {
            local = sub_80016718(44u);
            w_u32(local + 32u, 0u); w_u32(local + 36u, r_u32(object + 0x22Cu)); w_u32(local + 40u, r_u32(object + 0x230u));
            moh_draft_missing_call(0x8008D918u, object + 0x20Cu, local + 32u, object + 0x270u, local);
            matrix = local;
        } else matrix = object + 0x340u;
        for (index = 0u; index < 8u; ++index) xport_gte_write_control(index, r_u32(matrix + index * 4u));
        xport_gte_write_data(0u, r_u32(object + 0x2D8u)); xport_gte_write_data(1u, r_u32(object + 0x2DCu));
        xport_gte_execute(0x480012u);
        for (index = 0u; index < 3u; ++index) w_u16(object + 0x2E0u + index * 2u, (uint16)xport_gte_read_data(25u + index));
        if (local) sub_80036E44(local);
    } else moh_draft_missing_call(0x8008E354u, object, (uint32)bone, object + 0x2E0u, 0u, 0u);
    if (r_u16(object + 54u)) {
        count = r_u16(object + 54u);
        for (index = 0u; index < 3u; ++index) w_u16(object + 0x308u + index * 2u, 0x8001u);
        for (index = 0u; index < 3u; ++index) w_u16(object + 0x30Eu + index * 2u, 0x7FFFu);
        index = r_u32(object + 12u) & 1u;
        matrix = r_u32(object + 0x100u); local = r_u32(object + 0x304u);
        moh_draft_missing_call(0x8004CEE4u, object, matrix, object + 0x340u, local, count, index);
    }
}
void level_sub_80066290(void)
{
    FUNCTION_MARKER(0x80066290u, "LEVEL.BIN");
    if (r_u32(0x8009D5ACu)) w_u32(0x8009D5ACu, 0u);
    level_sub_8006685C();
    w_u32(0x8009D5BCu, 0u);
    w_u32(0x8009D5B8u, 0u);
}

void level_sub_8008A634(void)
{
    uint32 index = 0u;
    FUNCTION_MARKER(0x8008A634u, "LEVEL.BIN");
    while ((sint32)index < (sint32)r_u32(0x8009F888u)) {
        sub_80036E44(r_u32(0x8009F88Cu + index * 8u));
        ++index;
    }
    w_u32(0x8009F888u, 0u);
}

void level_sub_80061D08(uint32 player, uint32 mode)
{
    uint32 state = r_u32(player + 0x388u), count;
    FUNCTION_MARKER(0x80061D08u, "LEVEL.BIN");
    /* The original helper does not read the forwarded mode */
    (void)mode;
    level_sub_80061A2C(player, state);
    w_u32(0x8009A594u, 0u);
    w_u32(0x8009A590u, 0u);
    count = r_u16(state + 0x140u);
    w_u16(state + 0x142u, 0u);
    w_u32(state + 0x144u, 0u);
    w_u32(state + 0x148u, 0u);
    w_u32(state + 0x14Cu, 0u);
    w_u32(0x8009A598u, 0u);
    w_u16(state + 0x140u, (uint16)(count + 1u));
    w_u32(0x8009A59Cu, 0u);
    if (r_u32(0x80038640u))
        w_u32(player + 0xF4u, r_u32(0x80038774u + r_u32(0x80038664u) * 36u + r_u32(0x80038660u) * 152u));
    else w_u32(player + 0xF4u, 0x800000u);
    level_sub_8007555C(player);
    level_sub_8006A76C(player);
}

void level_sub_80074EA8(void)
{
    uint32 index;
    FUNCTION_MARKER(0x80074EA8u, "LEVEL.BIN");
    w_u32(0x8009E280u, 0u);
    w_u32(0x8009E284u, 0u);
    w_u32(0x8009E288u, 0u);
    for (index = 0u; index < 64u; ++index) w_u32(0x8009E390u - index * 4u, 0u);
}

void level_sub_800540AC(void)
{
    FUNCTION_MARKER(0x800540ACu, "LEVEL.BIN");
    w_u32(0x80099070u, 0u);
    w_u32(0x80098964u, 0u);
    w_u32(0x80098968u, 0u);
}

void level_sub_80054580(void)
{
    uint32 index;
    FUNCTION_MARKER(0x80054580u, "LEVEL.BIN");
    for (index = 0u; index < 4u; ++index) level_sub_80054554(index);
    for (index = 0u; index < 99u; ++index) w_u8(0x80099B1Au - index, 255u);
}

void level_sub_80075684(void)
{
    uint32 state, record, index;
    sint32 count;
    FUNCTION_MARKER(0x80075684u, "LEVEL.BIN");
    state = r_u32(r_u32(0x8009D648u) + 0x388u);
    count = (sint32)r_u32(state + 16u);
    record = r_u32(state + 20u);
    for (index = 0u; (sint32)index < count; ++index, record += 8u)
        w_u32(record, r_u32(record) & 0xFFFFFFFCu);
}

void level_sub_80075208(void)
{
    uint32 state, record, index = 0u;
    sint32 type;
    FUNCTION_MARKER(0x80075208u, "LEVEL.BIN");
    state = r_u32(r_u32(0x8009D648u) + 0x388u);
    record = r_u32(state + 20u);
    while ((sint32)index < (sint32)r_u32(state + 16u)) {
        type = (sint32)r_u32(r_u32(record + 4u) + 12u);
        if (type > 0 && type < 7) level_sub_800665BC(0u, record);
        ++index;
        record += 8u;
    }
}

void level_sub_80061158(void)
{
    FUNCTION_MARKER(0x80061158u, "LEVEL.BIN");
}

void level_sub_80052C18(void)
{
    FUNCTION_MARKER(0x80052C18u, "LEVEL.BIN");
    w_u32(0x80097FE0u, 0xFFFFFFFFu);
    w_u32(0x80097FE4u, 0u);
    w_u8(0x80098184u, 0u);
}

void level_sub_800888C4(void)
{
    uint32 scene;
    sint32 coordinate;
    FUNCTION_MARKER(0x800888C4u, "LEVEL.BIN");
    scene = r_u32(0x8009CC64u);
    if (r_u32(scene + 0x594u)) {
        coordinate = (sint16)r_u16(scene + 0x594u);
        moh_draft_missing_call(0x80015B28u, 0x8003D420u, coordinate, coordinate);
        scene = r_u32(0x8009CC64u);
        coordinate = (sint16)r_u16(scene + 0x596u);
        moh_draft_missing_call(0x80015B28u, 0x8003D448u, coordinate, coordinate);
        scene = r_u32(0x8009CC64u);
        w_u16(0x800B7AF0u, 0u);
        w_u16(0x800B7AF2u, r_u16(scene + 0x594u));
        coordinate = r_u16(scene + 0x596u);
        w_u32(0x8009EC68u, 1u);
        w_u16(0x800B7AF4u, (uint16)coordinate);
        level_sub_80088170();
    } else moh_draft_missing_call(0x80015B28u, 0x8003D470u);
    level_sub_80088744(0x8009EC94u, 255u);
}

void level_sub_80088980(void)
{
    uint32 scene, packet = 0x800B7850u, group, index, red, green, blue, y;
    FUNCTION_MARKER(0x80088980u, "LEVEL.BIN");
    scene = r_u32(0x8009CC64u);
    red = r_u8(scene + 0x58Cu);
    w_u8(0x800B7A91u, (uint8)red);
    green = r_u8(scene + 0x58Eu);
    w_u8(0x800B7A92u, (uint8)green);
    blue = r_u8(scene + 0x58Du);
    w_u8(0x800B7A94u, (uint8)(red >> 1));
    w_u8(0x800B7A95u, (uint8)(green >> 1));
    w_u8(0x800B7A93u, (uint8)blue);
    w_u8(0x800B7A96u, (uint8)(blue >> 1));
    for (group = 0u; group < 2u; ++group) {
        y = 210u;
        for (index = 0u; index < 8u; ++index, packet += 36u, y -= 30u) {
            w_u16(packet + 10u, (uint16)y);
            w_u16(packet + 18u, (uint16)y);
            w_u8(packet + 3u, 8u);
            w_u8(packet + 7u, 0x38u);
            w_u16(packet + 8u, 0u);
            w_u16(packet + 16u, 512u);
            w_u16(packet + 24u, 0u);
            w_u16(packet + 26u, (uint16)(y + 30u));
            w_u16(packet + 32u, 512u);
            w_u16(packet + 34u, (uint16)(y + 30u));
        }
    }
}

void level_sub_80061DE0(void)
{
    uint32 index;
    FUNCTION_MARKER(0x80061DE0u, "LEVEL.BIN");
    w_u32(0x80093C58u, 0xFFFFFFFFu);
    for (index = 0u; index < 100u; ++index) w_u32(0x80096DDCu - index * 128u, 0u);
}

void level_sub_80053DB0(void)
{
    uint32 index = 0u, filename;
    int length;
    FUNCTION_MARKER(0x80053DB0u, "LEVEL.BIN");
    w_u32(0x80098984u, r_u32(r_u32(0x8009CC64u) + 0x84u));
    if ((sint32)r_u32(0x80098984u) <= 0) return;
    filename = sub_80036BF0(40u);
    if (!filename) moh_draft_fail_fast("Missing texture filename buffer", 0x80053DB0u);
    while ((sint32)index < (sint32)r_u32(0x80098984u)) {
        length = snprintf((char *)moh_draft_guest_pointer(filename, 40u), 40u,
            (const char *)moh_draft_guest_pointer(0x8003C118u, 1u),
            (const char *)moh_draft_guest_pointer(r_u32(0x8009CC64u), 1u),
            (sint32)r_u32(0x80099060u + index * 4u));
        /* TODO Confirm all scene prefixes fit the original 40-byte buffer */
        if (length < 0 || length >= 40) moh_draft_fail_fast("Texture filename overflow", 0x80053DB0u);
        level_sub_8008C850(filename, index);
        w_u32(0x80098970u + index * 4u, r_u32(0x80099060u + index * 4u));
        ++index;
    }
    sub_80036E44(filename);
}

uint32 level_sub_8004F748(uint32 flags, uint32 mode, uint32 duration, uint32 delay,
    uint32 red, uint32 green, uint32 blue, uint32 end_red, uint32 end_green, uint32 end_blue, uint32 rectangle)
{
    uint32 object, state, offset, copy;
    sint32 divisor, red_step, green_step, blue_step;
    FUNCTION_MARKER(0x8004F748u, "LEVEL.BIN");
    red &= 255u; green &= 255u; blue &= 255u;
    end_red &= 255u; end_green &= 255u; end_blue &= 255u;
    object = level_sub_8006B5B8(0u, 0x74u, 0xFFFFFFFFu, 0u, 0u, 0u, 0u, 0u);
    if (!object) return 0u;
    w_u32(object + 8u, 50u);
    w_u32(object + 12u, 0x20000u);
    w_u32(object + 16u, 0x203u);
    w_u32(object + 20u, 0u);
    w_u32(object + 0x36Cu, 0x8004F488u);
    state = r_u32(object + 0x388u);
    if (mode) w_u32(object + 0x370u, 0u);
    else if (flags & 1u) w_u32(object + 0x370u, 0x8004F670u);
    divisor = (sint16)duration;
    if (!divisor) moh_draft_fail_fast("Original fade duration divide trap", 0x8004F844u);
    red_step = ((sint32)end_red - (sint32)red) / divisor;
    green_step = ((sint32)end_green - (sint32)green) / divisor;
    blue_step = ((sint32)end_blue - (sint32)blue) / divisor;
    w_u32(object + 0x374u, 0u);
    w_u32(object + 0x378u, 0u);
    w_u32(object + 0x384u, 0x8006A318u);
    w_u32(state, flags); w_u32(state + 4u, mode);
    w_u32(state + 8u, duration); w_u32(state + 12u, delay);
    w_u8(state + 24u, (uint8)red); w_u8(state + 25u, (uint8)green); w_u8(state + 26u, (uint8)blue);
    w_u16(state + 16u, (uint16)red_step); w_u16(state + 18u, (uint16)green_step); w_u16(state + 20u, (uint16)blue_step);
    if (flags & 4u) {
        if (!duration) moh_draft_fail_fast("Original fade scale divide trap", 0x8004F934u);
        w_u32(state + 108u, 0x300000u);
        w_u32(state + 112u, 0x300000u / duration);
    } else { w_u32(state + 108u, 0u); w_u32(state + 112u, 0u); }
    setPolyFT4((POLY_FT4 *)moh_draft_guest_pointer(state + 28u, 40u));
    setPolyFT4((POLY_FT4 *)moh_draft_guest_pointer(state + 68u, 40u));
    w_u8(state + 32u, (uint8)red); w_u8(state + 33u, (uint8)green); w_u8(state + 34u, (uint8)blue);
    w_u8(state + 72u, (uint8)red); w_u8(state + 73u, (uint8)green); w_u8(state + 74u, (uint8)blue);
    for (copy = 0u; copy < 2u; ++copy) {
        offset = copy * 40u;
        w_u8(state + offset + 40u, 252u); w_u8(state + offset + 41u, 0u);
        w_u8(state + offset + 48u, 255u); w_u8(state + offset + 49u, 0u);
        w_u8(state + offset + 56u, 252u); w_u8(state + offset + 57u, 3u);
        w_u8(state + offset + 64u, 255u); w_u8(state + offset + 65u, 3u);
    }
    w_u16(state + 50u, (uint16)getTPage(0, 1, 960, 0));
    w_u16(state + 90u, (uint16)getTPage(0, 1, 960, 0));
    setSemiTrans((POLY_FT4 *)moh_draft_guest_pointer(state + 28u, 40u), 1);
    setSemiTrans((POLY_FT4 *)moh_draft_guest_pointer(state + 68u, 40u), 1);
    { uint32 page0 = r_u16(state + 50u), page1 = r_u16(state + 90u);
      w_u16(state + 50u, (uint16)((page0 & 0xFF9Fu) | 0x40u));
      w_u16(state + 90u, (uint16)((page1 & 0xFF9Fu) | 0x40u)); }
    w_u16(state + 42u, (uint16)getClut(992, 253));
    w_u16(state + 82u, (uint16)getClut(992, 253));
    for (copy = 0u; copy < 2u; ++copy) {
        offset = state + copy * 40u;
        if (rectangle) {
            w_u16(offset + 36u, r_u16(rectangle));
            w_u16(offset + 38u, r_u16(rectangle + 2u));
            w_u16(offset + 44u, (uint16)(r_u16(rectangle) + r_u16(rectangle + 4u)));
            w_u16(offset + 46u, r_u16(rectangle + 2u));
            w_u16(offset + 52u, r_u16(rectangle));
            w_u16(offset + 54u, (uint16)(r_u16(rectangle + 2u) + r_u16(rectangle + 6u)));
            w_u16(offset + 60u, (uint16)(r_u16(rectangle) + r_u16(rectangle + 4u)));
            w_u16(offset + 62u, (uint16)(r_u16(rectangle + 2u) + r_u16(rectangle + 6u)));
        } else {
            w_u16(offset + 36u, 0u); w_u16(offset + 38u, 0u);
            w_u16(offset + 44u, 0u); w_u16(offset + 46u, 240u);
            w_u16(offset + 52u, 512u); w_u16(offset + 54u, 0u);
            w_u16(offset + 60u, 512u); w_u16(offset + 62u, 240u);
        }
    }
    level_sub_8006B834(object);
    return object;
}

void level_sub_80054554(uint32 slot)
{
    FUNCTION_MARKER(0x80054554u, "LEVEL.BIN");
    w_u32(0x8009931Cu + slot * 648u, 0u);
    w_u32(0x80099318u + slot * 648u, 0u);
}

void level_sub_80061A2C(uint32 player, uint32 state)
{
    uint32 index, count, weapon, record, amount, initial_health, unlimited;
    sint32 first = -1, special = -1;
    FUNCTION_MARKER(0x80061A2Cu, "LEVEL.BIN");
    level_sub_8006192C();
    initial_health = r_u32(player + 0xF0u);
    w_u32(player + 0x38u, 0u); w_u32(player + 0x3Cu, 0u); w_u32(player + 0x40u, 0u);
    w_u32(player + 0x284u, 0u); w_u32(player + 0xF4u, initial_health);
    level_sub_8007BC0C(player, 1u);
    count = r_u32(0x8009E874u);
    for (index = 0u; (sint32)index < (sint32)count; ++index) {
        weapon = 0x8009E878u + index;
        record = 0x80093640u + r_u8(weapon) * 68u;
        if (r_u16(record + 42u) & 1u) {
            w_u16(record + 6u, 0u);
            w_u16(0x80093648u + r_u8(weapon) * 68u, 0u);
        }
        unlimited = r_u32(0x800386A4u) & 64u;
        amount = r_u16(0x80093648u + r_u8(weapon) * 68u);
        w_u16(state + index * 2u + 94u, (uint16)amount);
        if (!unlimited)
            amount = r_u16(0x80093646u + r_u8(weapon) * 68u);
        w_u16(state + index * 2u + 86u, (uint16)amount);
        amount = r_u16(state + index * 2u + 86u);
        w_u16(state + index * 2u + 102u, 0u);
        if ((sint16)amount != -1) {
            if ((sint16)first == -1) first = (sint32)index;
            if ((sint16)special == -1 && (r_u16(0x8009366Au + r_u8(weapon) * 68u) & 8u))
                special = (sint32)index;
        }
    }
    if ((sint16)special == -1) {
        count = r_u32(0x8009E874u);
        for (index = 0u; (sint32)index < (sint32)count; ++index) {
            if (r_u16(0x8009366Au + r_u8(0x8009E878u + index) * 68u) & 8u) {
                w_u16(state + index * 2u + 86u, 0u);
                break;
            }
        }
    }
    if ((sint16)first == -1) {
        w_u16(state + 86u, 0u); w_u8(state + 84u, 0u);
    } else w_u8(state + 84u, (uint8)first);
    level_sub_8007BD1C(player);
    w_u32(player + 0x38u, r_u32(player + 0x38u) | 0x20000u);
    level_sub_8003FBFC(player);
    level_sub_8003F8F0(player);
    w_u32(player + 0x288u, 1u); w_u32(player + 0x28Cu, 1u); w_u32(player + 0x21Cu, 0u);
    level_sub_8007DD4C(player);
    level_sub_80080240(player, 0u);
}

void level_sub_80088744(uint32 positions, uint32 count)
{
    uint32 random, color = 0x8009EC6Cu, color_index, index, output = 0x8009F48Cu;
    sint32 y, radius, cosine, sine;
    FUNCTION_MARKER(0x80088744u, "LEVEL.BIN");
    random = moh_draft_missing_call(0x8001552Cu);
    color_index = ((uint32)((sint32)random >> 12)) & 7u;
    count &= 65535u;
    for (index = 0u; index < count; ++index, output += 4u, positions += 8u) {
        color += color_index;
        w_u8(output, r_u8(color)); w_u8(output + 1u, r_u8(color + 1u));
        color_index = (color_index + 1u) & 7u;
        w_u8(output + 2u, r_u8(color + 2u));
        random = moh_draft_missing_call(0x8001552Cu);
        y = (sint16)(0u - ((r_u8(r_u32(0x8009CC64u) + 0x58Fu) << 8) + ((random * 0x5FFFu) >> 15)));
        w_u16(positions + 2u, (uint16)y);
        random = moh_draft_missing_call(0x8001552Cu);
        radius = (sint32)moh_draft_missing_call(0x80016188u, (uint32)(0x6FFF - y) * (uint32)(y + 0x6FFF));
        random = (random >> 3) & 65535u;
        cosine = rcos((sint32)random); sine = rsin((sint32)random);
        w_u16(positions, (uint16)((sint32)((uint32)radius * (uint32)cosine) >> 12));
        w_u16(positions + 4u, (uint16)((sint32)((uint32)radius * (uint32)sine) >> 12));
    }
}

void level_sub_8006192C(void)
{
    uint32 index;
    FUNCTION_MARKER(0x8006192Cu, "LEVEL.BIN");
    for (index = 0u; index < 27u; ++index) w_u32(0x80039CA8u + index * 4u, 0u);
    for (index = 0u; index < 13u; ++index) w_u16(0x800939A0u - index * 68u, 0u);
}

uint32 level_sub_8003FBFC(uint32 player)
{
    uint32 controller = 0u, layer, block, index, record, flags, mask, buttons;
    uint32 result = 0u, active, frame, cursor = 0u, offset, target, instruction;
    FUNCTION_MARKER(0x8003FBFCu, "LEVEL.BIN");
    if (r_u32(player + 12u) & 1u) controller = r_u8(r_u32(player + 0x388u) + 0x152u);
    if ((sint32)r_u32(player + 0x44u) > 0) {
        flags = r_u32(player + 0x38u); mask = r_u32(player + 0x44u);
        w_u32(player + 0x38u, flags & 0xFFFFF9FFu); w_u32(player + 0x44u, mask - 1u);
    }
    if ((sint32)r_u32(player + 0x48u) > 0) {
        flags = r_u32(player + 0x38u); mask = r_u32(player + 0x48u);
        w_u32(player + 0x38u, flags & 0xFFE7FE1Fu); w_u32(player + 0x48u, mask - 1u);
    }
    for (layer = 0u; layer < 2u; ++layer) {
        block = player + 0x104u + layer * 0x74u;
        if (!r_u32(block + 16u)) continue;
        frame = r_u32(player + 0x100u) + 0x5Cu;
        for (index = 0u; index < r_u32(player + 0x1ECu); ++index) {
            offset = index * 60u;
            record = r_u32(block + 32u) + offset;
            flags = r_u32(record);
            if (!flags) { if (result) break; else continue; }
            active = (r_u32(record + 4u) & 0xC0u) != 0u;
            if ((flags & 1u) && !moh_draft_missing_call(0x8004D590u, controller, r_u32(record + 8u), active, 0u)) continue;
            if ((flags & 2u) && active) {
                record = r_u32(block + 32u) + offset;
                mask = r_u32(record + 12u);
                buttons = r_u32(0x80093B7Cu + controller * 4u) & mask;
                if (buttons || !(r_u32(0x80093B84u + controller * 4u) & mask)) continue;
            }
            if (flags & 4u) {
                mask = r_u32(r_u32(block + 32u) + offset + 16u);
                if ((mask & r_u32(player + 0x38u)) != mask) continue;
            }
            if ((flags & 8u) && (r_u32(r_u32(block + 32u) + offset + 20u) & r_u32(player + 0x38u))) continue;
            if (flags & 16u) {
                mask = r_u32(r_u32(block + 32u) + offset + 24u);
                if ((mask & r_u32(player + 0x3Cu)) != mask) continue;
            }
            if ((flags & 32u) && (r_u32(r_u32(block + 32u) + offset + 28u) & r_u32(player + 0x3Cu))) continue;
            if (flags & 64u) {
                mask = r_u32(r_u32(block + 32u) + offset + 32u);
                if ((mask & r_u32(player + 0x40u)) != mask) continue;
            }
            if ((flags & 128u) && (r_u32(r_u32(block + 32u) + offset + 36u) & r_u32(player + 0x40u))) continue;
            record = r_u32(block + 32u) + offset;
            instruction = record + 40u;
            target = r_u8(record + 43u);
            if (target == 0x26u) {
                target = r_u32(block + 16u);
                mask = r_u16(record + 40u);
                target = r_u32(target + 12u);
                w_u16(block + 24u, 65535u); w_u16(block + 28u, 0u); w_u16(block + 30u, 0u);
                w_u32(block + 20u, target + mask * 4u);
            } else if (target != 0x38u) {
                if (!cursor) {
                    cursor = sub_80036BF0(4u);
                    if (!cursor) moh_draft_fail_fast("Missing animation instruction cursor", 0x8003FBFCu);
                }
                w_u32(cursor, instruction);
                level_sub_8005D18C(cursor, player, block, frame, 0u, layer);
            }
            result = 1u;
            break;
        }
    }
    if (cursor) sub_80036E44(cursor);
    return result;
}

void level_sub_8007DD4C(uint32 player)
{
    uint32 state, value, weapon, record, enabled, sound_flags;
    sint32 slot, bank, sample;
    FUNCTION_MARKER(0x8007DD4Cu, "LEVEL.BIN");
    state = r_u32(player + 0x388u);
    if (r_u32(state) & 0x200040u) {
        value = r_u32(state + 0xA4u) >> 2;
        w_u32(state + 0xA4u, value);
        if (value < 400u) w_u32(state + 0xA4u, 400u);
        if (r_u32(0x800AA168u) != 250u) w_u32(0x800AA168u, 250u);
        w_u32(state, r_u32(state) & 0xFFDFFFBFu);
    }
    w_u32(state, r_u32(state) & 0xFFFFFFDFu);
    slot = (sint8)r_u8(state + 84u);
    w_u8(state + 85u, r_u8(0x8009E878u + (uint32)slot));
    level_sub_8006B234(r_u32(player + 0x100u), player, 0u);
    weapon = r_u8(state + 85u);
    record = 0x80093640u + weapon * 68u;
    w_u16(state + 0x156u, r_u16(record + 40u));
    w_u32(state + 64u, (uint32)(sint32)(sint16)r_u16(record + 4u));
    level_sub_8006B1C4(r_u32(player + 0x100u), r_u32(record), 0x147u, player);
    level_sub_8003F454(player, 2, 1u, 0u);
    level_sub_8003F8F0(player);
    level_sub_80046574(player);
    slot = (sint8)r_u8(state + 84u);
    enabled = 0u;
    if (!r_u16(state + (uint32)slot * 2u + 102u)) {
        weapon = r_u8(0x8009E878u + (uint32)slot);
        enabled = (r_u16(0x8009366Au + weapon * 68u) & 1u) == 0u;
    }
    level_sub_8005C97C(player, 16u, enabled);
    level_sub_8004249C(player, 10u);
    record = 0x80093640u + r_u8(state + 85u) * 68u;
    sound_flags = (r_u16(record + 42u) >> 3) & 2u;
    sample = (sint16)r_u16(record + 24u);
    if (sample >= 0) {
        bank = (sint16)r_u16(record + 26u);
        if (bank < 0) bank = (sint16)r_u16(r_u32(r_u32(player + 0x100u) + 8u) + 200u);
        level_sub_80041CE8((uint32)bank, sample, player, 0x10000u,
            0u, 0x8000u, 0x3334u, 0u, 0u, 0u, sound_flags);
    }
    level_sub_8005C97C(player, 17u, 0u);
    value = r_u32(state);
    w_u32(state + 0x13Cu, 0u);
    if (value & 2u) w_u32(player + 0x2D4u, 0x500000u);
    record = 0x80093640u + r_u8(state + 85u) * 68u;
    level_sub_8006DCAC(state + 0x1D4u, player, 0u, (uint32)r_u16(record + 52u), record + 54u);
    w_u32(r_u32(player + 0x304u) + 16u, 0x50000u);
    w_u32(r_u32(player + 0x304u) + 20u, 0x50000u);
}

void level_sub_80080240(uint32 object, uint32 delta)
{
    uint32 state, total, amount, record;
    sint32 weapon;
    FUNCTION_MARKER(0x80080240u, "LEVEL.BIN");
    state = r_u32(r_u32(0x8009D654u) + 0x388u);
    if (!delta) return;
    total = r_u32(0x80039CB8u) + delta;
    amount = r_u16(state + 0x142u) + delta;
    w_u16(state + 0x142u, (uint16)amount);
    w_u32(0x80039CB8u, total);
    if ((sint16)amount < 0) { w_u32(0x80039CB8u, 0u); w_u16(state + 0x142u, 0u); }
    weapon = (sint16)r_u16(object + 0x3CAu);
    if (weapon == -1) {
        if (!(r_u32(object + 12u) & 1u)) return;
        weapon = r_u8(state + 85u);
    } else if (weapon >= 13) return;
    record = 0x80093670u + (uint32)weapon * 68u;
    w_u16(record, (uint16)(r_u16(record) + 1u));
}

void level_sub_8006B234(uint32 instance, uint32 object, uint32 deferred)
{
    uint32 index = 0u, flags, data, type;
    FUNCTION_MARKER(0x8006B234u, "LEVEL.BIN");
    if (!instance) return;
    if (object) {
        while (index < r_u16(object + 54u)) {
            level_sub_8006B234(r_u32(r_u32(object + 0x304u) + index * 72u + 60u), 0u, 2u);
            ++index;
        }
    }
    flags = r_u32(instance + 4u);
    if (flags & 8u) {
        data = r_u32(instance + 8u);
        w_u32(instance + 4u, flags & 0xFFFFFFF7u);
        w_u32(data + 28u, 0u);
    }
    /* The original recursively supplied null object still reads guest address 8 */
    type = r_u32(object + 8u);
    if (type == 0x1402u || type == 0x1405u)
        moh_draft_missing_call(0x8008ECA0u, r_u32(instance), deferred);
    else {
        if (r_u32(instance)) level_sub_8008A4F8(r_u32(instance), deferred);
        w_u32(instance, 0u);
    }
}

uint32 level_sub_8006B1C4(uint32 instance, uint32 identity, uint32 flags, uint32 object)
{
    sint32 index;
    FUNCTION_MARKER(0x8006B1C4u, "LEVEL.BIN");
    index = (sint32)level_sub_8006A910(identity);
    if (index < 0) { w_u32(instance, 0u); return 0u; }
    return level_sub_8006A96C(instance, level_sub_8006A8FC((uint32)index), flags, object);
}

void level_sub_8004249C(uint32 object, uint32 value)
{
    uint32 index, view;
    FUNCTION_MARKER(0x8004249Cu, "LEVEL.BIN");
    view = sub_80036BF0(4u);
    if (!view) moh_draft_fail_fast("Missing voice object view", 0x8004249Cu);
    w_u32(view, object);
    for (index = 1u; index < 24u; ++index) {
        if (level_sub_8006D7D0(0x80091160u + index * 52u, view)) {
            w_u8(0x80091190u + (index - 1u) * 52u + 15u, 0u);
            w_u8(0x80091190u + (index - 1u) * 52u + 17u, (uint8)value);
            w_u8(0x80091190u + (index - 1u) * 52u + 18u, (uint8)value);
        }
    }
    sub_80036E44(view);
}

void level_sub_8006DCAC(uint32 destination, uint32 object, uint32 mode, uint32 kind, uint32 values)
{
    uint32 first, second;
    FUNCTION_MARKER(0x8006DCACu, "LEVEL.BIN");
    w_u16(destination, (uint16)kind); w_u16(destination + 2u, 0u);
    first = r_u32(values); second = r_u32(values + 4u);
    w_u32(destination + 4u, first); w_u32(destination + 8u, second);
    level_sub_8006D84C(destination + 24u, object);
    w_u32(destination + 28u, mode);
    level_sub_8006E004(destination + 112u, 1008u, 128u, 31u, 31u, 8u, 992u, 249u, 2u, 1u);
}

uint32 level_sub_8006D7D0(uint32 handle, uint32 object_view)
{
    uint32 reference, object = 0u;
    FUNCTION_MARKER(0x8006D7D0u, "LEVEL.BIN");
    if (level_sub_8006D8AC(handle)) return r_u32(object_view) == 0u;
    reference = r_u32(handle);
    if (reference) object = r_u32(0x8009D678u + (reference & 65535u) * 4u);
    return r_u32(object_view) == object;
}

void level_sub_8006E004(uint32 destination, uint32 x, uint32 y, uint32 width,
    uint32 height, uint32 depth, uint32 clut_x, uint32 clut_y, uint32 mode, uint32 enabled)
{
    FUNCTION_MARKER(0x8006E004u, "LEVEL.BIN");
    w_u16(destination, 0u);
    w_u16(destination + 16u, (uint16)width); w_u16(destination + 2u, (uint16)width);
    w_u16(destination + 6u, (uint16)x); w_u16(destination + 8u, (uint16)y);
    w_u8(destination + 22u, 0u); w_u8(destination + 23u, 0u);
    w_u16(destination + 18u, (uint16)height); w_u16(destination + 4u, (uint16)height);
    w_u16(destination + 10u, (uint16)depth); w_u16(destination + 12u, (uint16)clut_x);
    w_u16(destination + 14u, (uint16)clut_y);
    w_u8(destination + 20u, (uint8)mode); w_u8(destination + 21u, (uint8)enabled);
}

void level_sub_8008A4F8(uint32 allocation, uint32 deferred)
{
    uint32 count;
    FUNCTION_MARKER(0x8008A4F8u, "LEVEL.BIN");
    if (!deferred) { sub_80036E44(allocation); return; }
    count = r_u32(0x8009F888u);
    w_u32(0x8009F88Cu + count * 8u, allocation);
    w_u32(0x8009F890u + count * 8u, deferred);
    w_u32(0x8009F888u, count + 1u);
}

sint32 level_sub_80041AA0(void)
{
    uint32 voice, state;
    FUNCTION_MARKER(0x80041AA0u, "LEVEL.BIN");
    for (voice = 1u; voice < 24u; ++voice) {
        if (r_u8(0x8009115Cu + voice * 52u + 14u)) continue;
        state = r_u8(0x800910D0u + voice);
        if (!state || state == 3u) return (sint32)voice;
    }
    return -1;
}

sint32 level_sub_80041CE8(uint32 bank, sint32 sample, uint32 object, uint32 volume,
    uint32 volume_random, uint32 pitch, uint32 pitch_random, uint32 state, uint32 priority, uint32 distance, uint32 flags)
{
    sint32 voice, left, right, remainder, denominator;
    uint32 record, random, product, outputs = 0u;
    FUNCTION_MARKER(0x80041CE8u, "LEVEL.BIN");
    bank &= 65535u;
    if (bank >= 8u || sample < 0 || !r_u32(0x80091128u + bank * 4u)) return -1;
    voice = level_sub_80041AA0();
    if (voice == -1) return -1;
    record = 0x8009115Cu + (uint32)voice * 52u;
    w_u16(record + 12u, (uint16)bank); w_u32(record + 8u, (uint32)sample);
    product = volume << 7;
    w_u32(record + 20u, product);
    if ((sint32)product > 0xFF0000) w_u32(record + 20u, 0xFF0000u);
    w_u32(record + 24u, volume); w_u32(record + 28u, volume_random);
    w_u32(record + 32u, pitch); w_u8(record + 14u, (uint8)priority);
    w_u16(record + 40u, 0u); w_u32(record + 36u, pitch_random);
    w_u8(record + 19u, (uint8)distance);
    if (volume_random) {
        random = moh_draft_missing_call(0x8001552Cu);
        product = r_u32(record + 20u) * volume_random;
        denominator = (sint32)(product << 1);
        if (!denominator || (denominator == -1 && random == 0x80000000u))
            moh_draft_fail_fast("Original voice variation divide trap", 0x80041E3Cu);
        remainder = (sint32)random % denominator;
        w_u32(record + 20u, r_u32(record + 20u) + product - (uint32)remainder);
    }
    w_u8(record + 15u, (uint8)state); w_u8(record + 18u, 0u); w_u8(record + 16u, 0u);
    level_sub_8006D84C(record + 4u, object);
    w_u32(record, flags);
    if (!object || (r_u32(object + 12u) & 0x40000000u)) {
        product = r_u32(record + 20u);
        w_u16(record + 44u, 0u); w_u16(record + 46u, 0u); w_u16(record + 48u, 0u);
        random = r_u8(0x800939FEu);
        w_u32(record, r_u32(record) | 1u);
        left = (sint16)((sint32)((uint32)((sint32)product >> 10) * random) >> 8);
        right = left;
    } else {
        w_u16(record + 44u, r_u16(object + 0x20Eu));
        w_u16(record + 46u, r_u16(object + 0x212u));
        w_u16(record + 48u, r_u16(object + 0x216u));
        outputs = sub_80036BF0(4u);
        if (!outputs) moh_draft_fail_fast("Missing spatial voice outputs", 0x80041CE8u);
        level_sub_80042AA4((uint32)voice, outputs, outputs + 2u, (sint32)distance);
        left = (sint16)r_u16(outputs); right = (sint16)r_u16(outputs + 2u);
        if (!left && !right) { left = 1; right = 1; }
    }
    level_sub_80041B04((sint16)bank, sample, (uint32)voice, left, right, pitch, pitch_random);
    if (outputs) sub_80036E44(outputs);
    return voice;
}

void level_sub_80041B04(sint32 bank, sint32 sample, uint32 voice, sint32 left, sint32 right, uint32 pitch, uint32 pitch_random)
{
    uint32 table, product, base_pitch, random, offset = 0u, index;
    sint32 variation;
    FUNCTION_MARKER(0x80041B04u, "LEVEL.BIN");
    table = r_u32(0x80091108u + (uint32)bank * 4u);
    if (r_u16(table + (uint32)sample * 2u + 2u) < 13u) return;
    moh_draft_missing_call(0x80015C08u, voice, (uint32)(sint32)(sint16)left, (uint32)(sint32)(sint16)right);
    product = (uint32)(sint32)(sint16)r_u16(0x8009163Cu + (uint32)bank * 2u + (uint32)sample * 16u) * pitch;
    base_pitch = (uint32)((sint32)product >> 16);
    product = base_pitch * pitch_random;
    variation = (sint32)product >> 15;
    if (variation) {
        random = moh_draft_missing_call(0x8001552Cu);
        if (variation == -1 && random == 0x80000000u)
            moh_draft_fail_fast("Original voice pitch divide trap", 0x80041BF8u);
        base_pitch += (uint32)((sint32)product >> 16) - (uint32)((sint32)random % variation);
    }
    moh_draft_missing_call(0x800157BCu, voice, base_pitch & 65535u);
    if (sample >= 0) {
        table = r_u32(0x80091108u + (uint32)bank * 4u);
        for (index = 0u; (sint32)index <= sample; ++index) offset += (uint32)r_u16(table + index * 2u) * 8u;
    }
    moh_draft_missing_call(0x80015D38u, voice, r_u32(0x800910E8u + (uint32)bank * 4u) + offset);
    moh_draft_missing_call(0x8001559Cu, voice, 0u, 0u, 0u, 3u, 15u, 1u, 1u, 3u);
    w_u32(0x80090FC8u, r_u32(0x80090FC8u) | (1u << (voice & 31u)));
    w_u8(0x800910D0u + voice, 1u);
}

static sint32 life_fixed_reduce(uint32 product)
{
    if ((sint32)product < 0) product += 4095u;
    return (sint32)product >> 12;
}

void level_sub_800427F4(uint32 camera, uint32 position, uint32 output)
{
    uint32 state, handle, origin, index, x, y, z;
    FUNCTION_MARKER(0x800427F4u, "LEVEL.BIN");
    state = r_u32(camera + 0x388u);
    handle = r_u32(state + 12u);
    origin = handle ? r_u32(0x8009D678u + (handle & 65535u) * 4u) : 0u;
    if (origin) {
        handle = r_u32(state + 12u);
        origin = handle ? r_u32(0x8009D678u + (handle & 65535u) * 4u) : 0u;
    } else origin = camera;
    x = (uint16)(r_u16(position) - r_u16(origin + 0x20Eu));
    y = (uint16)(r_u16(position + 2u) - r_u16(origin + 0x212u));
    z = (uint16)(r_u16(position + 4u) - r_u16(origin + 0x216u));
    for (index = 0u; index < 5u; ++index) xport_gte_write_control(index, r_u32(state + 0x100u + index * 4u));
    xport_gte_write_control(5u, 0u); xport_gte_write_control(6u, 0u); xport_gte_write_control(7u, 0u);
    xport_gte_write_data(0u, x | (y << 16));
    /* The unused upper half of the original VZ word does not affect MVMVA */
    xport_gte_write_data(1u, z);
    xport_gte_execute(0x486012u);
    x = xport_gte_read_data(9u); y = xport_gte_read_data(10u); z = xport_gte_read_data(11u);
    w_u16(output, (uint16)x); w_u16(output + 2u, (uint16)y); w_u16(output + 4u, (uint16)z);
}

sint32 level_sub_80042998(sint32 distance, sint32 mode)
{
    uint32 denominator = 0x1B00u, product, attenuation;
    sint32 amount;
    FUNCTION_MARKER(0x80042998u, "LEVEL.BIN");
    if (distance < 256) distance = 256;
    if (mode == 1) denominator = 0x1F00u;
    else if (mode == 2) denominator = 0x2300u;
    else if (mode == 3) denominator = 0x2700u;
    product = ((uint32)distance - 256u) << 12;
    amount = (sint32)(4096u - (uint32)((sint32)product / (sint32)denominator));
    attenuation = amount > 0 ? (uint32)amount : 0u;
    return (sint32)(attenuation * r_u8(0x800939FEu)) >> 7;
}

void level_sub_80042AA4(uint32 voice, uint32 left_out, uint32 right_out, sint32 mode)
{
    PsxGteSnapshot snapshot;
    uint32 saved[8], index, depth, base, vector, product;
    sint32 x, y, z, largest, other, last, swap, magnitude, volume, left, right, scale;
    FUNCTION_MARKER(0x80042AA4u, "LEVEL.BIN");
    depth = r_u32(0x8009D320u);
    base = 0x8009D324u + depth;
    psx_gte_snapshot(&snapshot);
    for (index = 0u; index < 4u; ++index) saved[index] = xport_load_le32((const uint8 *)&snapshot.rotation + index * 4u);
    saved[4] = (uint32)(sint32)snapshot.rotation.m[2][2];
    saved[5] = (uint32)snapshot.translation[0]; saved[6] = (uint32)snapshot.translation[1]; saved[7] = (uint32)snapshot.translation[2];
    for (index = 0u; index < 6u; ++index) w_u32(base + index * 4u, saved[index]);
    w_u32(0x8009D320u, depth + 32u);
    w_u32(base + 24u, saved[6]); w_u32(base + 28u, saved[7]);
    vector = sub_80036BF0(8u);
    if (!vector) moh_draft_fail_fast("Missing spatial sound vector", 0x80042AA4u);
    level_sub_800427F4(r_u32(0x8009D638u), 0x80091188u + voice * 52u, vector);
    depth = r_u32(0x8009D320u) - 32u;
    base = 0x8009D324u + depth;
    w_u32(0x8009D320u, depth);
    for (index = 0u; index < 8u; ++index) xport_gte_write_control(index, r_u32(base + index * 4u));
    x = (sint16)r_u16(vector); y = (sint16)r_u16(vector + 2u); z = (sint16)r_u16(vector + 4u);
    largest = x < 0 ? -x : x;
    other = y < 0 ? -y : y;
    last = z < 0 ? -z : z;
    if (largest < other) { swap = largest; largest = other; other = swap; }
    if (largest < last) { swap = largest; largest = last; last = swap; }
    volume = level_sub_80042998(largest + ((other + last) >> 2), mode);
    z = (sint16)r_u16(vector + 4u);
    if (z > 400) {
        x = (sint16)((sint16)r_u16(vector) / (z - 400));
        w_u16(vector, (uint16)x);
    }
    x = (sint16)r_u16(vector);
    magnitude = x < 0 ? -x : x;
    if (magnitude > 1024) {
        if (x > 0) { left = 0; right = volume; }
        else { left = volume; right = 0; }
    } else {
        if (x >= 0) {
            right = life_fixed_reduce((uint32)(2048 + magnitude * 2) * (uint32)volume);
            left = life_fixed_reduce((uint32)(2048 - magnitude * 2) * (uint32)volume);
        } else {
            right = life_fixed_reduce((uint32)(2048 - magnitude * 2) * (uint32)volume);
            left = life_fixed_reduce((uint32)(2048 + magnitude * 2) * (uint32)volume);
        }
    }
    if (!r_u8(0x80093A03u)) { right = (sint32)((uint32)left + (uint32)right) >> 1; left = right; }
    scale = (sint16)r_u16(0x80091172u + voice * 52u);
    product = (uint32)right * (uint32)scale; right = life_fixed_reduce(product);
    product = (uint32)left * (uint32)scale; left = life_fixed_reduce(product);
    w_u16(left_out, (uint16)((uint32)left << 6));
    w_u16(right_out, (uint16)((uint32)right << 6));
    sub_80036E44(vector);
}

void level_sub_80069D04(void)
{
    uint32 top, operation, value;
    FUNCTION_MARKER(0x80069D04u, "LEVEL.BIN");
    top = r_u32(0x8009D5A8u);
    w_u32(0x8009D5A8u, top - 1u);
    w_u32(0x8009D5A8u, top - 2u);
    operation = r_u32(0x800AD264u + (top - 1u) * 8u);
    value = r_u32(0x800AD264u + top * 8u);
    level_sub_80052DC4(operation, value);
}

void level_sub_80052DC4(uint32 operation, uint32 value)
{
    uint32 filename, format, mission, level;
    int length;
    FUNCTION_MARKER(0x80052DC4u, "LEVEL.BIN");
    switch (operation) {
    case 0u:
        w_u16(r_u32(0x8009CC64u) + 0xCCu, (uint16)value);
        moh_draft_missing_call(0x8001C6D8u, (uint32)(sint32)(sint16)value, 400u);
        break;
    case 1u:
        if (!moh_native_music_enabled()) break;
        if (r_u32(0x8009A60Cu)) { w_u32(0x8009A608u, 1u); break; }
        w_u32(0x8009CC70u, 1u);
        if (r_u32(0x8009CC74u)) { moh_draft_missing_call(0x80015B28u, 0x8003C0A4u); break; }
        w_u32(0x8009CC74u, 1u);
        moh_draft_missing_call(0x8001C698u, 0x8008A7B8u);
        format = r_u32(0x80038658u) ? 0x8003C068u : 0x8003C088u;
        mission = r_u32(0x800939F4u); level = r_u32(0x800939F8u);
        filename = sub_80036BF0(32u);
        if (!filename) moh_draft_fail_fast("Missing music filename buffer", 0x80052DC4u);
        length = snprintf((char *)moh_draft_guest_pointer(filename, 32u), 32u,
            (const char *)moh_draft_guest_pointer(format, 1u), (sint32)mission, (sint32)level, (sint32)mission, (sint32)level);
        /* TODO Confirm all identifiers fit the original 32-byte filename */
        if (length < 0 || length >= 32) moh_draft_fail_fast("Music filename overflow", 0x80052DC4u);
        moh_draft_missing_call(0x8008A838u, filename);
        sub_80036E44(filename);
        break;
    case 2u: w_u32(0x80098624u, value); break;
    case 3u: w_u32(0x8009C840u, r_u32(0x8009C840u) | 0x24u); break;
    case 4u: moh_draft_missing_call(0x8005F068u, value); break;
    default: break;
    }
}

void level_sub_80069DEC(void)
{
    uint32 top, identity, label;
    FUNCTION_MARKER(0x80069DECu, "LEVEL.BIN");
    top = r_u32(0x8009D5A8u);
    w_u32(0x8009D5A8u, top - 1u); w_u32(0x8009D5A8u, top - 2u);
    identity = r_u32(0x800AD264u + top * 8u);
    label = r_u32(0x800AD264u + (top - 1u) * 8u);
    level_sub_8005372C(identity, label);
}

void level_sub_8005372C(uint32 identity, uint32 label)
{
    uint32 count, index;
    FUNCTION_MARKER(0x8005372Cu, "LEVEL.BIN");
    count = r_u32(0x80098620u);
    for (index = 0u; (sint32)index < (sint32)count; ++index) {
        if (r_u32(0x8009892Cu + index * 4u) == identity) {
            w_u32(0x800988DCu + index * 4u, 0u);
            w_u32(0x80098904u + index * 4u, r_u32(0x80098188u + label * 4u));
            w_u32(0x8009892Cu + index * 4u, identity);
            return;
        }
    }
    count = r_u32(0x80098620u);
    w_u32(0x80098620u, count + 1u);
    w_u32(0x800988DCu + count * 4u, 0u);
    w_u32(0x80098904u + count * 4u, r_u32(0x80098188u + label * 4u));
    w_u32(0x8009892Cu + count * 4u, identity);
}

void level_sub_80069A74(void)
{
    uint32 top, trigger, ignored;
    FUNCTION_MARKER(0x80069A74u, "LEVEL.BIN");
    top = r_u32(0x8009D5A8u);
    w_u32(0x8009D5A8u, top - 1u); w_u32(0x8009D5A8u, top - 2u);
    trigger = r_u32(0x800AD264u + top * 8u);
    ignored = r_u32(0x800AD264u + (top - 1u) * 8u);
    (void)ignored;
    level_sub_80052D70(trigger);
}

void level_sub_80052D70(uint32 trigger)
{
    uint32 flags;
    FUNCTION_MARKER(0x80052D70u, "LEVEL.BIN");
    flags = r_u32(trigger);
    if (!(flags & 1u)) {
        w_u32(trigger, flags | 1u);
        level_sub_8007505C(trigger);
    }
}

void level_sub_8007505C(uint32 trigger)
{
    uint32 geometry, value, pending = 0u;
    sint32 type;
    FUNCTION_MARKER(0x8007505Cu, "LEVEL.BIN");
    geometry = r_u32(trigger + 4u);
    type = (sint32)r_u32(geometry + 12u);
    if (type == 2 || type == 6) {
        value = r_u32(geometry + 96u);
        pending = level_sub_80074C54(r_u32(value + 4u), trigger);
    } else if (type == 3) {
        value = r_u32(geometry + 116u);
        pending = level_sub_80074C54(r_u32(value + 4u), trigger);
    } else if (type == 4) pending = level_sub_80074CE8(r_u32(geometry + 96u), trigger);
    if (pending) level_sub_80074EE8(trigger);
}

uint32 level_sub_80074C54(uint32 identity, uint32 trigger)
{
    sint32 resource;
    uint32 material, object;
    FUNCTION_MARKER(0x80074C54u, "LEVEL.BIN");
    resource = (sint32)level_sub_8006A910(identity);
    if (resource < 0) moh_draft_missing_call(0x80089B04u, 2u, 24u);
    material = level_sub_8006A8FC((uint32)resource);
    object = moh_draft_missing_call(0x8006D6F0u, material, trigger);
    if (!object) return 1u;
    level_sub_8006B834(object);
    w_u32(trigger, r_u32(trigger) | 1u); w_u32(object + 28u, trigger);
    level_sub_800665BC(object, 0u);
    level_sub_80074AD8(object, trigger);
    return 0u;
}

uint32 level_sub_80074CE8(uint32 identity, uint32 trigger)
{
    uint32 object, material;
    sint32 animation;
    FUNCTION_MARKER(0x80074CE8u, "LEVEL.BIN");
    object = level_sub_80083960(trigger, identity, 0xFFFFFFFFu);
    if (!object) return 1u;
    level_sub_80074AD8(object, trigger);
    material = r_u32(object + 0x100u);
    if (material && r_u32(material + 8u)) {
        level_sub_8006A76C(object);
        animation = (sint16)r_u16(r_u32(r_u32(object + 0x100u) + 8u) + 148u);
        if (animation >= 0) { level_sub_8003F454(object, animation, 1u, 0u); level_sub_8003F8F0(object); }
    }
    level_sub_8006B834(object);
    w_u32(trigger, r_u32(trigger) | 1u); w_u32(object + 28u, trigger);
    level_sub_800665BC(object, 0u);
    return 0u;
}

void level_sub_80074EE8(uint32 trigger)
{
    uint32 count, index;
    FUNCTION_MARKER(0x80074EE8u, "LEVEL.BIN");
    if (r_u32(0x8009E288u) >= 64u) {
        moh_draft_missing_call(0x80089B04u, 4u, 40u);
        index = r_u32(0x8009E280u);
        w_u32(0x8009E294u + index * 4u, 0u);
        count = r_u32(0x8009E288u);
        w_u32(0x8009E280u, index + 1u); w_u32(0x8009E288u, count - 1u);
    }
    count = r_u32(0x8009E288u); index = r_u32(0x8009E284u);
    w_u32(0x8009E288u, count + 1u);
    w_u32(0x8009E294u + index * 4u, trigger);
    w_u32(0x8009E284u, index + 1u);
    if (index + 1u >= 64u) w_u32(0x8009E284u, index - 63u);
}

void level_sub_8008335C(uint32 identity, uint32 index_out, uint32 resource_out)
{
    uint32 index, row = 0x8009E8E8u;
    FUNCTION_MARKER(0x8008335Cu, "LEVEL.BIN");
    for (index = 0u; index < 20u; ++index, row += 20u) {
        if (r_u32(row) == identity) {
            w_u32(index_out, index); w_u32(resource_out, r_u32(row + 4u));
            return;
        }
    }
}

void level_sub_800830B0(uint32 object, uint32 mode, uint32 geometry, uint32 state)
{
    FUNCTION_MARKER(0x800830B0u, "LEVEL.BIN");
    (void)object; (void)mode;
    w_u32(state + 20u, r_u8(geometry + 92u));
}

uint32 level_sub_800833A0(uint32 geometry, uint32 identity, uint32 resource, uint32 mode)
{
    uint32 local = sub_80016718(16u), index, row, object, material, state, frame, callback;
    sint32 height, delta, dimensions[3], maximum;
    uint32 axis;
    FUNCTION_MARKER(0x800833A0u, "LEVEL.BIN");
    /* TODO Original missing identity leaves lookup outputs unspecified */
    w_u32(local, 0xFFFFFFFFu);
    level_sub_8008335C(identity, local, local + 4u);
    index = r_u32(local);
    if (index >= 20u) moh_draft_fail_fast("LEVEL actor identity absent from constructor table", identity);
    if (resource == 0xFFFFFFFFu) resource = r_u32(local + 4u);
    row = 0x8009E8E8u + index * 20u;
    object = level_sub_8006B5B8(3u, r_u16(row + 8u), resource, 0x157u, 0u, 0u, 0u, 0u);
    if (!object) {
        moh_draft_missing_call(0x80089B04u, 2u, 8u);
        sub_80036E44(local); return 0u;
    }
    w_u32(object + 8u, 0x65u); w_u32(object + 0x36Cu, 0x80082E88u);
    w_u32(object + 12u, 0x47028200u); w_u32(object + 16u, 0x40211u);
    w_u32(object + 0x374u, 0u); w_u32(object + 0x370u, 0x8006A5B4u);
    w_u32(object + 0x378u, 0u); frame = r_u32(row + 16u);
    w_u32(object + 0x384u, 0x8006A37Cu); w_u32(object + 20u, 1u);
    w_u32(object + 0x37Cu, frame);
    for (axis = 0u; axis < 3u; ++axis)
        w_u32(object + 0x20Cu + axis * 4u, (uint32)r_u16(geometry + 16u + axis * 2u) << 16);
    for (axis = 0u; axis < 3u; ++axis)
        w_u32(object + 0x228u + axis * 4u, (uint32)r_u16(geometry + 24u + axis * 2u) << 16);
    height = (sint16)level_sub_80060474(object, local + 8u);
    delta = (sint16)r_u16(geometry + 18u) - height;
    if (delta < 0) delta = -delta;
    if (delta < 55) {
        w_u16(geometry + 18u, (uint16)height); w_u32(object + 0x210u, (uint32)height << 16);
    }
    material = r_u32(object + 0x100u);
    w_u16(object + 0x2ECu, 0u); w_u16(object + 54u, 0u);
    material = r_u32(material + 8u);
    for (axis = 0u; axis < 3u; ++axis) {
        delta = (sint16)r_u16(material + 0x7Cu + axis * 2u) - (sint16)r_u16(material + 0x44u + axis * 2u);
        if (delta < 0) delta = -delta;
        dimensions[axis] = (sint16)delta;
    }
    maximum = dimensions[0] > dimensions[1] ? dimensions[0] : dimensions[1];
    if (dimensions[2] > maximum) maximum = dimensions[2];
    w_u16(object + 0x2E8u, (uint16)((maximum + 30) >> 1));
    for (axis = 0u; axis < 3u; ++axis)
        w_u16(object + 0x2D8u + axis * 2u, (uint16)(r_u16(material + 0x44u + axis * 2u) + (dimensions[axis] >> 1)));
    if (r_u32(geometry + 88u) & 1u) {
        w_u32(object + 12u, r_u32(object + 12u) | 0x8000u);
        delta = (sint16)r_u16(geometry + 94u);
        w_u32(object + 0x294u, (uint32)delta); w_u32(object + 0x290u, (uint32)delta);
    }
    state = r_u32(object + 0x388u);
    w_u32(state, index); w_u32(state + 4u, resource); w_u32(state + 8u, 900u);
    w_u32(state + 12u, (r_u32(geometry + 48u) & 8u) ? (uint32)(sint32)(sint16)r_u16(geometry + 54u) : 0xFFFFFFFFu);
    callback = r_u32(row + 12u);
    moh_draft_missing_call(callback, object, mode, geometry, state);
    sub_80036E44(local);
    return object;
}

uint32 level_sub_80083960(uint32 trigger, uint32 identity, uint32 resource)
{
    FUNCTION_MARKER(0x80083960u, "LEVEL.BIN");
    return level_sub_800833A0(r_u32(trigger + 4u), identity, resource, 0u);
}

void level_sub_8005FB20(void)
{
    sint32 count = (sint32)r_u32(0x80098984u), excluded;
    uint32 index, slot;
    FUNCTION_MARKER(0x8005FB20u, "LEVEL.BIN");
    w_u32(0x800AB940u, 0u);
    if (count <= 0) return;
    excluded = (sint32)r_u32(0x800AA184u);
    for (index = 0u; (sint32)index < count; ++index) if ((sint32)index != excluded) {
        slot = r_u32(0x800AB940u);
        w_u32(0x800AB950u + slot * 4u, 0x80090DE8u + index * 40u);
        w_u32(0x800AB940u, slot + 1u);
    }
}

uint32 level_sub_8005FB94(sint32 x, sint32 z, uint32 index)
{
    uint32 record = r_u32(0x800AB950u + index * 4u);
    sint32 minimum, maximum;
    FUNCTION_MARKER(0x8005FB94u, "LEVEL.BIN");
    minimum = (sint16)r_u16(record); maximum = (sint16)r_u16(record + 4u);
    if (x < minimum || maximum < x) return 0u;
    minimum = (sint16)r_u16(record + 2u); maximum = (sint16)r_u16(record + 6u);
    return z < minimum || maximum < z ? 0u : record;
}

uint32 level_sub_8005FC00(uint32 point, uint32 polygon)
{
    uint32 candidate = r_u32(0x800AB960u), vertices = r_u32(candidate + 28u);
    uint32 first, second, third, normal, packed[4];
    sint32 x, z, x1, x2, x3, z1, z2, z3, minimum, maximum;
    FUNCTION_MARKER(0x8005FC00u, "LEVEL.BIN");
    first = vertices + (uint32)(sint32)(sint16)r_u16(polygon) * 8u;
    second = vertices + (uint32)(sint32)(sint16)r_u16(polygon + 2u) * 8u;
    third = vertices + (uint32)(sint32)(sint16)r_u16(polygon + 4u) * 8u;
    normal = r_u32(candidate + 32u) + (uint32)(sint32)(sint16)r_u16(polygon + 6u) * 8u;
    x = (sint16)r_u16(point); z = (sint16)r_u16(point + 4u);
    if ((sint16)r_u16(normal + 2u) > 0) return 0u;
    x1 = (sint16)r_u16(first);
    if (x < x1) return 0u;
    x3 = (sint16)r_u16(third); x2 = (sint16)r_u16(second);
    maximum = x3 > x2 ? x3 : x2;
    if (maximum < x) return 0u;
    z2 = (sint16)r_u16(second + 4u); z1 = (sint16)r_u16(first + 4u); z3 = (sint16)r_u16(third + 4u);
    minimum = z1 < z2 ? z1 : z2; if (z3 < minimum) minimum = z3;
    if (z < minimum) return 0u;
    maximum = z1 > z2 ? z1 : z2; if (z3 > maximum) maximum = z3;
    if (maximum < z) return 0u;
    packed[0] = (uint32)(uint16)x | ((uint32)(uint16)z << 16);
    packed[1] = r_u16(first) | ((uint32)r_u16(first + 4u) << 16);
    packed[2] = r_u16(second) | ((uint32)r_u16(second + 4u) << 16);
    xport_gte_write_data(12u, packed[1]); xport_gte_write_data(13u, packed[2]); xport_gte_write_data(14u, packed[0]);
    xport_gte_execute(0x1400006u);
    packed[3] = r_u16(third) | ((uint32)r_u16(third + 4u) << 16);
    if ((sint32)xport_gte_read_data(24u) > 0) return 0u;
    xport_gte_write_data(12u, packed[2]); xport_gte_write_data(13u, packed[3]); xport_gte_write_data(14u, packed[0]);
    xport_gte_execute(0x1400006u);
    if ((sint32)xport_gte_read_data(24u) > 0) return 0u;
    xport_gte_write_data(12u, packed[3]); xport_gte_write_data(13u, packed[1]); xport_gte_write_data(14u, packed[0]);
    xport_gte_execute(0x1400006u);
    return (sint32)xport_gte_read_data(24u) <= 0 ? 1u : 0u;
}

sint32 level_sub_8005FDCC(uint32 polygon, uint32 point)
{
    uint32 candidate = r_u32(0x800AB960u), vertices = r_u32(candidate + 28u);
    uint32 normal = r_u32(candidate + 32u) + (uint32)(sint32)(sint16)r_u16(polygon + 6u) * 8u;
    sint32 divisor = (sint16)r_u16(normal + 2u), magnitude = divisor, numerator;
    uint32 sum;
    FUNCTION_MARKER(0x8005FDCCu, "LEVEL.BIN");
    if (magnitude < 0) magnitude = -magnitude;
    if (magnitude < 257) return (sint16)r_u16(vertices + (uint32)(sint32)(sint16)r_u16(polygon) * 8u + 2u);
    sum = (uint32)((sint16)r_u16(normal) * (sint16)r_u16(point));
    sum += (uint32)((sint16)r_u16(normal + 4u) * (sint16)r_u16(point + 4u));
    sum += (uint32)(sint32)(sint16)r_u16(polygon + 8u) << 15;
    numerator = (sint32)(0u - sum);
    return (sint16)(numerator / divisor);
}

sint32 level_sub_8005FEA0(uint32 point, uint32 polygons, uint32 indices, sint32 count, uint32 face_out)
{
    sint32 best = -32767, selected = -1, distance = 32767, index, height, y, delta;
    uint32 polygon;
    int selected_above = 0, selected_within = 0, above, within, replace;
    FUNCTION_MARKER(0x8005FEA0u, "LEVEL.BIN");
    count = (sint16)count;
    for (index = 0; index < count; ++index, indices += 2u) {
        polygon = polygons + (uint32)(sint32)(sint16)r_u16(indices) * 10u;
        if (!level_sub_8005FC00(point, polygon)) continue;
        polygon = polygons + (uint32)(sint32)(sint16)r_u16(indices) * 10u;
        height = (sint16)level_sub_8005FDCC(polygon, point);
        y = (sint16)r_u16(point + 2u); delta = height - y;
        if (delta < 0) delta = -delta;
        above = y < height; within = !above && y <= height + 32; replace = 0;
        if (above || within) {
            if (selected_above) replace = above ? height < best : 1;
            else replace = selected_within ? delta < distance : 1;
        }
        if (replace) {
            selected_above = above; selected_within = within; distance = delta; best = height;
            selected = (sint16)r_u16(indices);
        }
    }
    w_u32(face_out, (uint32)selected);
    return best;
}

sint32 level_sub_80060034(uint32 point, uint32 leaf_out, uint32 face_out)
{
    uint32 candidate = r_u32(0x800AB960u), tree = r_u32(candidate + 20u), node = tree;
    uint32 polygons = r_u32(candidate + 36u), indices;
    sint32 x = (sint16)r_u16(point), z = (sint16)r_u16(point + 4u);
    sint32 origin_x = (sint16)r_u16(candidate), origin_z = (sint16)r_u16(candidate + 2u);
    sint32 left, right, split;
    FUNCTION_MARKER(0x80060034u, "LEVEL.BIN");
    for (;;) {
        left = (sint16)r_u16(node);
        if (left < 0) {
            w_u32(leaf_out, r_u16(node + 6u));
            candidate = r_u32(0x800AB960u);
            indices = r_u32(candidate + 24u) + (uint32)(sint32)(sint16)r_u16(node + 2u) * 2u;
            return level_sub_8005FEA0(point, polygons, indices, (sint16)~r_u16(node), face_out);
        }
        right = (sint16)r_u16(node + 2u);
        if (right >= 0) {
            split = (sint32)((uint32)origin_x + r_u16(node + 4u));
            if (x < split) node = tree + (uint32)left * 8u;
            else { origin_x = split; node = tree + (uint32)right * 8u; }
        } else {
            split = (sint32)((uint32)origin_z + r_u16(node + 4u));
            if (z < split) node = tree + (uint32)left * 8u;
            else { origin_z = split; node = tree + (uint32)~right * 8u; }
        }
    }
}

sint32 level_sub_80060138(uint32 position, uint32 leaf_out, uint32 normal_out)
{
    uint32 point = sub_80016718(8u), candidate, polygon, normal, first, second;
    sint32 height, y, x, z, maximum;
    FUNCTION_MARKER(0x80060138u, "LEVEL.BIN");
    w_u16(point, r_u16(position + 2u)); w_u16(point + 2u, r_u16(position + 6u));
    w_u16(point + 4u, r_u16(position + 10u)); w_u32(0x800AB968u, position);
    height = level_sub_80060034(point, leaf_out, 0x800AB964u);
    if (normal_out && height != -32767) {
        candidate = r_u32(0x800AB960u);
        polygon = r_u32(candidate + 36u) + r_u32(0x800AB964u) * 10u;
        normal = r_u32(candidate + 32u) + (uint32)(sint32)(sint16)r_u16(polygon + 6u) * 8u;
        first = r_u32(normal); second = r_u32(normal + 4u);
        w_u32(normal_out, first); w_u32(normal_out + 4u, second);
        y = (sint16)r_u16(normal_out + 2u); w_u16(normal_out + 6u, 0u);
        if (y < 0) {
            z = (sint16)r_u16(normal_out + 4u); x = (sint16)r_u16(normal_out);
            if (z < 0) z = -z; if (x < 0) x = -x;
            maximum = (sint16)(z > x ? z : x);
            if (maximum > 0 && (-y * 16) / maximum < 28) w_u16(normal_out + 6u, 1u);
        }
    }
    sub_80036E44(point); return height;
}

sint32 level_sub_8006028C(uint32 position, uint32 index_out, uint32 normal_out)
{
    uint32 local = sub_80016718(12u), index = 0u, candidate, first, second;
    sint32 best = -32767, height, reference, delta;
    int replace;
    FUNCTION_MARKER(0x8006028Cu, "LEVEL.BIN");
    level_sub_8005FB20();
    while ((sint32)index < (sint32)r_u32(0x800AB940u)) {
        candidate = level_sub_8005FB94((sint16)r_u16(position + 2u), (sint16)r_u16(position + 10u), index);
        w_u32(0x800AB960u, candidate);
        if (candidate) {
            height = (sint16)level_sub_80060138(position, local + 8u, local);
            replace = best == -32767;
            if (!replace && height != -32767) {
                reference = (sint32)r_u32(position + 4u);
                if (reference < height && reference < best) replace = height < best;
                else {
                    delta = (sint32)((uint32)best - (uint32)reference);
                    if (delta < 0) delta = (sint32)(0u - (uint32)delta);
                    replace = (sint32)((uint32)height - (uint32)reference) < delta;
                }
            }
            if (replace) {
                if (index_out) w_u32(index_out, r_u32(local + 8u));
                best = height;
                if (normal_out) {
                    /* TODO Original no-hit candidate may leave the local normal unspecified */
                    first = r_u32(local); second = r_u32(local + 4u);
                    w_u32(normal_out, first); w_u32(normal_out + 4u, second);
                }
            }
        }
        ++index;
    }
    sub_80036E44(local); return best;
}

sint32 level_sub_800603DC(uint32 object, uint32 normal_out)
{
    uint32 local = sub_80016718(20u), index, first, second;
    sint32 height;
    FUNCTION_MARKER(0x800603DCu, "LEVEL.BIN");
    first = r_u32(object + 0x20Cu); second = r_u32(object + 0x210u); index = r_u32(object + 0x214u);
    w_u32(local, first); w_u32(local + 4u, second); w_u32(local + 8u, index);
    w_u32(0x800AB96Cu, 0xFFFFu);
    height = (sint16)level_sub_8006028C(local, 0x800AB96Cu, local + 12u);
    index = r_u32(0x800AB96Cu);
    /* TODO Original normal output is unspecified when no surface was selected */
    first = r_u32(local + 12u); second = r_u32(local + 16u);
    w_u32(normal_out, first); w_u32(normal_out + 4u, second); w_u32(object + 0x390u, index);
    sub_80036E44(local); return height;
}

sint32 level_sub_80060474(uint32 object, uint32 normal_out)
{
    FUNCTION_MARKER(0x80060474u, "LEVEL.BIN");
    /* Native ABI omits the original CPU scratchpad stack switch */
    return (sint16)level_sub_800603DC(object, normal_out);
}

void level_sub_80088170(void)
{
    uint32 index, packet = 0x800B7AA0u, x, y, z;
    sint32 size;
    FUNCTION_MARKER(0x80088170u, "LEVEL.BIN");
    size = ((sint32)(sint16)r_u16(0x800B7AF4u) * 32) / 200;
    for (index = 0u; index < 2u; ++index, packet += 40u) {
        w_u8(packet + 3u, 9u); w_u8(packet + 7u, 0x2Cu);
        w_u16(packet + 22u, 15u); w_u16(packet + 14u, 0x3FBFu);
        w_u8(packet + 12u, 0xC0u); w_u8(packet + 13u, 0x60u);
        w_u8(packet + 20u, 0xDFu); w_u8(packet + 21u, 0x60u);
        w_u8(packet + 28u, 0xC0u); w_u8(packet + 29u, 0x7Fu);
        w_u8(packet + 36u, 0xDFu); w_u8(packet + 37u, 0x7Fu);
        w_u8(packet + 4u, 0x80u); w_u8(packet + 5u, 0x80u); w_u8(packet + 6u, 0x80u);
    }
    x = r_u16(0x800B7AF0u);
    w_u16(0x800B7AF8u, (uint16)(x + ((size * 2731) >> 12)));
    y = r_u16(0x800B7AF2u); z = r_u16(0x800B7AF4u);
    w_u16(0x800B7AFAu, (uint16)y); w_u16(0x800B7AFCu, (uint16)z);
    w_u16(0x800B7B00u, (uint16)x); w_u16(0x800B7B02u, (uint16)(y + size));
    w_u16(0x800B7B04u, (uint16)z);
}

void level_sub_8008C850(uint32 filename, uint32 index)
{
    uint32 local = sub_80016718(8u), data, material;
    FUNCTION_MARKER(0x8008C850u, "LEVEL.BIN");
    data = moh_draft_missing_call(0x80049AC4u, filename, local, r_u32(0x8009904Cu + index * 4u));
    material = 0x8009898Cu + index * 256u;
    w_u32(material + 16u, data);
    if (!level_sub_8008C514(material, index)) moh_draft_missing_call(0x80015B28u, 0x8003D66Cu);
    level_sub_8006A96C(0x80098D8Cu + index * 176u, material, 22u, 0u);
    sub_80036E44(local);
}

uint32 level_sub_8008C514(uint32 material, uint32 index)
{
    uint32 data = r_u32(material + 16u), animations, absp, base, cursor, entry, offset;
    sint32 maximum = 0, value;
    FUNCTION_MARKER(0x8008C514u, "LEVEL.BIN");
    w_u32(material + 4u, 0u); w_u32(material + 12u, 0u); w_u32(material + 152u, 0u);
    w_u32(material + 144u, 0u); w_u32(material + 196u, 0u); w_u32(material + 136u, 0u);
    for (offset = 52u; offset >= 32u; offset -= 4u) w_u32(material + offset, 0u);
    w_u16(material + 140u, 0u); w_u32(material + 136u, 0u);
    w_u16(material + 142u, 0u); w_u32(material + 144u, 0u); w_u32(material + 24u, 0u);
    if (!data) return 0u;
    w_u32(material + 20u, data + r_u32(data + 8u));
    value = (sint32)r_u32(data + 16u);
    w_u16(material + 140u, r_u16(data + 20u));
    w_u32(material + 136u, data + r_u32(data + 24u));
    w_u16(material + 142u, r_u16(data + 28u));
    w_u32(material + 144u, data + r_u32(data + 32u));
    animations = data + (uint32)value;
    w_u32(material + 24u, data + r_u32(data + 40u));
    level_sub_800545D8(data + r_u32(data + 56u), r_u32(data + 52u), index);
    absp = data + r_u32(data + 60u); base = r_u32(material + 20u);
    cursor = base; entry = base + 24u;
    do {
        if (!r_u32(entry - 12u)) {
            value = (sint32)r_u32(entry); if (maximum < value) maximum = value;
            value = (sint32)r_u32(entry + 4u); if (maximum < value) maximum = value;
            value = (sint32)r_u32(entry + 8u); if (maximum < value) maximum = value;
            value = (sint32)r_u32(entry); w_u32(entry, value < 0 ? 0u : base + (uint32)value);
            value = (sint32)r_u32(entry + 8u); w_u32(entry + 8u, value < 0 ? 0u : base + (uint32)value);
            value = (sint32)r_u32(entry + 4u); w_u32(entry + 4u, value < 0 ? 0u : base + (uint32)value);
            entry += 36u; cursor += 36u;
        } else {
            cursor += 28u; w_u32(entry, animations + r_u32(entry)); entry += 28u;
        }
    } while ((sint32)(cursor - base) <= maximum);
    if (!level_sub_8003F1B8(absp, index)) {
        moh_draft_missing_call(0x80015B28u, 0x8003D5F0u); return 0u;
    }
    return 1u;
}

void level_sub_800545D8(uint32 records, uint32 count, uint32 index)
{
    uint32 slot, entry = 0x80099098u + index * 648u, label, value;
    FUNCTION_MARKER(0x800545D8u, "LEVEL.BIN");
    if (!count) { level_sub_80054554(index); return; }
    if (index > 3u) moh_draft_missing_call(0x80089B04u, 5u, 32u);
    w_u32(entry + 640u, count); w_u32(entry + 644u, 1u);
    for (slot = 0u; slot < count; ++slot, entry += 20u) {
        w_u32(entry, records); w_u32(entry + 8u, records + r_u16(records + 22u));
        value = r_u16(records + 20u); w_u32(entry + 16u, 1u); w_u32(entry + 4u, records + value);
        label = r_u32(records + 12u);
        if (label < 99u) {
            value = r_u8(0x80099AB8u + label);
            if (value == 255u) { w_u8(0x80099AB8u + label, 0u); w_u32(entry + 12u, 0u); }
            else {
                w_u32(entry + 12u, r_u8(0x80099AB8u + label));
                moh_draft_missing_call(0x80054498u, entry, index);
            }
        } else w_u32(entry + 12u, 0u);
        records += r_u32(records);
    }
}

uint32 level_sub_8003EB44(uint32 data)
{
    uint32 output = r_u32(0x800AB8F8u), first, second, vertices, normals;
    sint32 count;
    FUNCTION_MARKER(0x8003EB44u, "LEVEL.BIN");
    w_u16(output, r_u16(data)); w_u16(output + 2u, r_u16(data + 2u));
    w_u16(output + 4u, r_u16(data + 4u)); w_u16(output + 6u, r_u16(data + 6u));
    w_u16(output + 8u, r_u16(data + 8u));
    count = (sint16)r_u16(output + 8u);
    w_u16(output + 10u, r_u16(data + 10u)); w_u16(output + 14u, r_u16(data + 12u));
    first = data + 18u; w_u16(output + 16u, r_u16(data + 14u));
    second = first + (uint32)count * 8u;
    count = (sint16)r_u16(output + 10u);
    vertices = r_u16(data + 16u);
    w_u32(output + 20u, first); w_u32(output + 24u, second);
    first = second + (uint32)count * 2u; w_u16(output + 12u, (uint16)vertices);
    if (first & 3u) first += 2u;
    count = (sint16)r_u16(output + 14u); vertices = first;
    second = (uint32)(sint32)(sint16)r_u16(output + 16u) * 8u;
    w_u32(output + 28u, vertices); normals = vertices + (uint32)count * 8u;
    w_u32(output + 32u, normals); w_u32(output + 36u, normals + second);
    return 1u;
}

uint32 level_sub_8003F1B8(uint32 data, uint32 index)
{
    FUNCTION_MARKER(0x8003F1B8u, "LEVEL.BIN");
    w_u32(0x800AB8F8u, 0x80090DE8u + index * 40u);
    return level_sub_8003EB44(data);
}

uint32 level_sub_8004F700(void)
{
    uint32 object = r_u32(0x8009D64Cu);
    FUNCTION_MARKER(0x8004F700u, "LEVEL.BIN");
    while (object) {
        if (r_u32(object + 8u) == 50u) return object;
        object = r_u32(object + 4u);
    }
    return 0u;
}

void level_sub_8008A580(void)
{
    uint32 index = 0u, entry = 0x8009F88Cu, count, last, value;
    sint32 remaining;
    FUNCTION_MARKER(0x8008A580u, "LEVEL.BIN");
    if ((sint32)r_u32(0x8009F888u) <= 0) return;
    do {
        remaining = (sint32)(r_u32(entry + 4u) - 1u); w_u32(entry + 4u, (uint32)remaining);
        if (remaining <= 0) {
            sub_80036E44(r_u32(entry));
            count = r_u32(0x8009F888u) - 1u; last = 0x8009F88Cu + count * 8u;
            w_u32(entry, r_u32(last)); value = r_u32(last + 4u);
            w_u32(0x8009F888u, count); w_u32(entry + 4u, value);
        }
        ++index; entry += 8u;
    } while ((sint32)index < (sint32)r_u32(0x8009F888u));
}

void level_sub_8008EAC8(void)
{
    uint32 index, first = 0x800B7B08u, second = 0x800C4378u, value, minimum;
    FUNCTION_MARKER(0x8008EAC8u, "LEVEL.BIN");
    for (index = 0u; index < 5u; ++index, first += 0x2814u, second += 0x1308u) {
        if (!r_u32(first + 0x2810u)) {
            value = r_u32(first + 0x280Cu);
            if ((sint32)value > 0) w_u32(first + 0x280Cu, value - 1u);
            if (!r_u32(first + 0x280Cu)) {
                value = r_u32(0x800AB8D0u);
                w_u32(0x800AB8C8u, r_u32(0x800AB8C8u) - 1u);
                minimum = r_u32(0x800AB8CCu);
                w_u32(0x800AB8C8u, (sint32)value > (sint32)minimum ? value : minimum);
                level_sub_8008EDDC(index);
            }
        }
        if (!r_u32(second + 0x1304u)) {
            value = r_u32(second + 0x1300u);
            if ((sint32)value > 0) w_u32(second + 0x1300u, value - 1u);
            if (!r_u32(second + 0x1300u)) {
                value = r_u32(0x800AB8D0u) - 1u; minimum = r_u32(0x800AB8CCu);
                w_u32(0x800AB8D0u, value);
                w_u32(0x800AB8C8u, (sint32)value > (sint32)minimum ? value : minimum);
                level_sub_8008EC14(index);
            }
        }
    }
}

void level_sub_8004D974(void)
{
    uint32 index, offset, packet, port, kind, value, analog, state, buttons, timer, vibration;
    FUNCTION_MARKER(0x8004D974u, "LEVEL.BIN");
    for (index = 0u; index < 2u; ++index) {
        offset = index * 4u; port = index * 16u; packet = 0x80093BA0u + index * 36u;
        buttons = r_u32(0x80093B7Cu + offset); analog = r_u32(0x80093B70u + offset);
        kind = r_u8(packet) ? 0u : r_u8(packet + 1u) >> 4;
        w_u32(0x80093B84u + offset, buttons); w_u32(0x80093B98u + offset, analog);
        w_u8(0x80093B94u + index, (uint8)kind); w_u8(0x80093B78u + index, 0u);
        if (!r_u32(0x80093B68u + offset)) {
            w_u32(0x80093B68u + offset, 1u);
            value = moh_draft_missing_call(0x8001B728u, port, 1u, 0u);
            moh_draft_missing_call(0x80015B28u, value ? 0x8003A310u : 0x8003A344u, index + 1u);
        }
        if (kind == 2u || kind == 7u) {
            w_u32(0x80093B70u + offset, r_u32(packet + 4u)); w_u8(0x80093B78u + index, 1u);
            value = moh_draft_missing_call(0x8001B2D8u, port, 2u, 0u);
            w_u32(0x80093BECu + offset, value);
            w_u32(0x80093BECu + offset, r_u32(index && value ? 0x800386C4u : 0x800386B0u));
            state = moh_draft_missing_call(0x8001B3D8u, port);
            if (state == 1u) { w_u32(0x80093B70u + offset, 0x80808080u); w_u32(0x80093B60u + offset, 1u); }
            if (r_u32(0x80093B60u + offset)) {
                moh_draft_missing_call(0x8001B888u, port, 0x80093BE8u + index * 2u, 2u);
                if (state == 2u) { w_u32(0x80093B60u + offset, 0u); w_u32(0x80093BECu + offset, 0u); }
                else if (state == 6u && r_u32(index ? 0x800386C4u : 0x800386B0u)) {
                    if (moh_draft_missing_call(0x8001B5B8u, port, 0x80093B58u)) {
                        w_u32(0x80093B60u + offset, 0u); w_u32(0x80093BECu + offset, 1u);
                    }
                }
            }
        }
        if (kind == 2u || kind == 4u || kind == 7u) {
            if (!index && r_u32(0x8009A600u)) {
                w_u32(0x8009A600u, 0u);
                value = moh_draft_missing_call(0x8001B728u, 0u, 1u, 0u);
                moh_draft_missing_call(0x80015B28u, value ? 0x8003A310u : 0x8003A344u, 1u);
            }
            buttons = ~(((uint32)r_u8(packet + 2u) << 8) | r_u8(packet + 3u));
            w_u32(0x80093B8Cu + offset, buttons); w_u32(0x80093B7Cu + offset, buttons);
        } else if (!kind) {
            if (!index) w_u32(0x8009A600u, 1u);
            w_u32(0x80093B8Cu + offset, 0u); w_u32(0x80093B7Cu + offset, 0u);
        }
        timer = r_u32(0x80093BFCu + offset); vibration = 0x80093BE8u + index * 2u;
        if (!timer) { w_u8(vibration, 0u); w_u8(vibration + 1u, 0u); }
        else {
            --timer; w_u32(0x80093BFCu + offset, timer);
            if (timer & r_u32(0x80093BF4u + offset)) w_u8(vibration + 1u, r_u8(vibration + 1u) >> 1);
        }
    }
}

void level_sub_8004D7DC(uint32 index, uint32 buttons)
{
    uint32 base = 0x80093C04u + index * 40u, first, second, third, fourth;
    sint32 cursor = 9;
    FUNCTION_MARKER(0x8004D7DCu, "LEVEL.BIN");
    do {
        first = r_u32(base + (uint32)(cursor - 1) * 4u); second = r_u32(base + (uint32)(cursor - 2) * 4u);
        third = r_u32(base + (uint32)(cursor - 3) * 4u); fourth = r_u32(base + (uint32)(cursor - 4) * 4u);
        w_u32(base + (uint32)cursor * 4u, first); w_u32(base + (uint32)(cursor - 1) * 4u, second);
        w_u32(base + (uint32)(cursor - 2) * 4u, third); w_u32(base + (uint32)(cursor - 3) * 4u, fourth);
        cursor -= 4;
    } while (cursor >= 4);
    while (cursor > 0) { w_u32(base + (uint32)cursor * 4u, r_u32(base + (uint32)(cursor - 1) * 4u)); --cursor; }
    w_u32(base, buttons);
}

void level_sub_8004DE3C(void)
{
    FUNCTION_MARKER(0x8004DE3Cu, "LEVEL.BIN");
    level_sub_8004D974();
    level_sub_8004D7DC(0u, r_u32(0x80093B7Cu));
    level_sub_8004D7DC(1u, r_u32(0x80093B80u));
}

static uint32 life_resolve_handle(uint32 handle)
{
    return handle ? r_u32(0x8009D678u + (handle & 0xFFFFu) * 4u) : 0u;
}

void level_sub_80052C38(void)
{
    uint32 count, first, second, result = 0u;
    FUNCTION_MARKER(0x80052C38u, "LEVEL.BIN");
    count = r_u8(0x80098184u);
    if (!count) return;
    first = life_resolve_handle(r_u32(0x80098178u));
    second = life_resolve_handle(r_u32(0x80098164u + (count - 1u) * 4u));
    if (!level_sub_8006D8AC(first) && !level_sub_8006D8AC(second))
        result = moh_draft_missing_call(0x8005A0C8u, second, first, 0u,
            (uint32)(sint32)(sint16)r_u16(r_u32(first + 0x38Cu) + 178u), 1u, 0u);
    if (result) w_u32(second + 60u, r_u32(second + 60u) | 0x10000u);
    w_u8(0x80098184u, r_u8(0x80098184u) - 1u);
}

void level_sub_8004A828(uint32 object)
{
    uint32 index = 0u, offset = 0u;
    FUNCTION_MARKER(0x8004A828u, "LEVEL.BIN");
    if (!r_u16(object + 54u)) return;
    do {
        w_u32(r_u32(object + 0x304u) + offset, 0u);
        ++index; offset += 72u;
    } while ((sint32)index < r_u16(object + 54u));
}

void level_sub_8006BA20(uint32 object)
{
    uint32 pending[16], depth = 0u, next, parent, check, flags, callback, result, attachment, material;
    uint32 local = 0u, axis, rotation[3], transformed[3], packed, delta;
    sint32 relative[3];
    FUNCTION_MARKER(0x8006BA20u, "LEVEL.BIN");
    w_u32(0x8009D61Cu, r_u32(0x8009D61Cu) + 1u);
    level_sub_80052C38();
    while (object) {
        next = r_u32(object + 4u);
        if (r_u32(object + 40u) != r_u32(0x8009D61Cu)) {
            for (;;) {
                parent = life_resolve_handle(r_u32(object + 44u));
                check = life_resolve_handle(r_u32(object + 44u));
                if (!check || r_u32(parent + 40u) == r_u32(0x8009D61Cu)) break;
                /* TODO Original sixteen-entry dependency array has no overflow guard */
                if (depth >= 16u) moh_draft_fail_fast("Object parent dependency overflow", 0x8006BA20u);
                pending[depth++] = object; object = parent;
            }
            w_u32(object + 40u, r_u32(0x8009D61Cu));
            level_sub_8004A828(object);
            parent = life_resolve_handle(r_u32(object + 44u));
            if (parent) {
                parent = life_resolve_handle(r_u32(object + 44u)); flags = r_u32(object + 24u);
                if (!(flags & 1u) || (r_u32(parent + 12u) & 0x80000u)) {
                    w_u32(object + 24u, 0u); level_sub_8006D84C(object + 44u, 0u);
                } else if (!(flags & 32u)) {
                    parent = life_resolve_handle(r_u32(object + 44u));
                    for (axis = 0u; axis < 3u; ++axis) {
                        delta = r_u32(parent + 0x20Cu + axis * 4u) - r_u32(parent + 0x258u + axis * 4u);
                        w_u32(object + 0x1F0u + axis * 4u, r_u32(object + 0x1F0u + axis * 4u) + delta);
                    }
                    rotation[0] = r_u32(parent + 0x228u) - r_u32(parent + 0x264u);
                    rotation[1] = r_u32(parent + 0x22Cu) - r_u32(parent + 0x268u);
                    rotation[2] = r_u32(parent + 0x230u) - r_u32(parent + 0x26Cu);
                    if (rotation[0] || rotation[1] || rotation[2]) {
                        if (!local) local = sub_80016718(40u);
                        if (!local) moh_draft_fail_fast("Missing object transform locals", 0x8006BA20u);
                        for (axis = 0u; axis < 3u; ++axis) w_u16(local + 32u + axis * 2u, (uint16)(rotation[axis] >> 16));
                        moh_draft_missing_call(0x8001676Cu, local + 32u, local);
                        w_u32(local + 28u, 0u); w_u32(local + 24u, 0u); w_u32(local + 20u, 0u);
                        for (axis = 0u; axis < 8u; ++axis) xport_gte_write_control(axis, r_u32(local + axis * 4u));
                        for (axis = 0u; axis < 3u; ++axis) {
                            delta = r_u32(object + 0x20Cu + axis * 4u) - r_u32(parent + 0x258u + axis * 4u);
                            relative[axis] = (sint16)(delta >> 16);
                            w_u16(local + 32u + axis * 2u, (uint16)relative[axis]);
                        }
                        xport_gte_write_data(0u, r_u32(local + 32u));
                        /* TODO The original upper vector pad is unspecified and unused by MVMVA */
                        packed = r_u16(local + 36u); xport_gte_write_data(1u, packed);
                        xport_gte_execute(0x486012u);
                        for (axis = 0u; axis < 3u; ++axis) transformed[axis] = xport_gte_read_data(25u + axis);
                        delta = r_u32(object + 0x228u); packed = r_u32(object + 0x230u);
                        w_u32(object + 0x228u, delta + rotation[0]);
                        delta = r_u32(object + 0x22Cu); w_u32(object + 0x230u, packed + rotation[2]);
                        w_u32(object + 0x22Cu, delta + rotation[1]);
                        for (axis = 0u; axis < 3u; ++axis)
                            w_u32(object + 0x1F0u + axis * 4u, r_u32(object + 0x1F0u + axis * 4u) + ((transformed[axis] - (uint32)relative[axis]) << 16));
                    }
                    w_u32(object + 24u, r_u32(object + 24u) & 0xFFFFFFFBu);
                }
            }
            callback = r_u32(object + 0x36Cu);
            if (callback) {
                moh_draft_indirect_call(callback, object);
                check = r_u32(object + 48u);
                w_u32(object + 56u, 0u); w_u32(object + 60u, 0u); w_u32(object + 64u, 0u);
                attachment = life_resolve_handle(check);
                if (attachment) {
                    attachment = life_resolve_handle(r_u32(object + 48u));
                    if (level_sub_8006D8AC(object + 48u) || (r_u32(attachment + 12u) & 0x80000u))
                        level_sub_8006D84C(object + 48u, 0u);
                    else {
                        attachment = life_resolve_handle(r_u32(object + 48u));
                        parent = life_resolve_handle(r_u32(object + 48u));
                        delta = (uint32)(sint32)(sint8)r_u8(object + 52u); material = r_u32(attachment + 0x100u);
                        if (!local) local = sub_80016718(40u);
                        if (!local) moh_draft_fail_fast("Missing attachment position local", 0x8006BA20u);
                        moh_draft_missing_call(0x8008DABCu, parent, material, delta, local, 0u, 0u);
                        for (axis = 0u; axis < 3u; ++axis) w_u32(object + 0x20Cu + axis * 4u, (uint32)r_u16(local + axis * 2u) << 16);
                    }
                }
                attachment = life_resolve_handle(r_u32(object + 48u));
                if (!attachment && !(r_u32(object + 12u) & 0x20000u)) {
                    result = moh_draft_missing_call(0x8006C83Cu, object);
                    if (r_u32(object + 12u) & 5u) moh_draft_missing_call(0x800845A8u, object);
                    if (result && (callback = r_u32(object + 0x378u)) != 0u) moh_draft_indirect_call(callback, object, result);
                }
                if (!(r_u32(object + 12u) & 0x80000u) && r_u32(object + 0x38Cu)) moh_draft_missing_call(0x80040DF4u, object);
            }
            flags = r_u32(object + 24u);
            if (!(flags & 2u)) w_u32(object + 24u, flags & 0xFFFFFFFEu);
            level_sub_8006A76C(object);
            if (r_u32(object + 12u) & 1u) moh_draft_missing_call(0x8007B350u, object);
            moh_draft_missing_call(0x8004D43Cu, object);
        }
        if (r_u32(object + 8u) == 2u) moh_draft_missing_call(0x8004913Cu, object);
        object = depth ? pending[--depth] : next;
    }
    if (local) sub_80036E44(local);
}
