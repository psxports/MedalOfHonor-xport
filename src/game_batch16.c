#include "game_level2p_signatures.h"
#include "xport_trace.h"
#include "psx.h"

uint32 level2p_sub_8006547C(uint32 owner, uint32 other)
{
    uint16 delta[4];
    uint32 first, second, state, x, y, z, swap, distance, flags, limit, current, enabled, handle, target = 0, a, b, c, view;
    FUNCTION_MARKER(0x8006547C, "LEVEL2P.BIN");
    first = (uint32)(sint32)r_s16(owner + 0x20E); second = (uint32)(sint32)r_s16(other + 0x20E); state = r_u32(other + 0x388);
    delta[0] = (uint16)(first - second); x = (uint32)(sint32)(sint16)delta[0];
    first = (uint32)(sint32)r_s16(owner + 0x212); second = (uint32)(sint32)r_s16(other + 0x212); if ((sint32)x < 0) x = 0 - x;
    delta[1] = (uint16)(first - second); y = (uint32)(sint32)(sint16)delta[1];
    first = (uint32)(sint32)r_s16(owner + 0x216); second = (uint32)(sint32)r_s16(other + 0x216); if ((sint32)y < 0) y = 0 - y;
    delta[2] = (uint16)(first - second); z = (uint32)(sint32)(sint16)delta[2]; if ((sint32)z < 0) z = 0 - z;
    if (x < y) { swap = x; x = y; y = swap; } if (x < z) { swap = x; x = z; z = swap; } distance = x + ((y + z) >> 2);
    flags = r_u32(owner + 0x10);
    if (flags & 0x40000) {
        limit = r_u32(0x800942CC); current = r_u32(0x800942C8);
        if ((sint32)current >= (sint32)(limit - 1)) {
            enabled = r_u32(0x8009A078);
            if (enabled && (sint32)distance >= 0xDAD) {
                handle = r_u32(state + 12); if (handle) target = r_u32(0x80099460 + (handle & 0xFFFF) * 4);
                a = r_u16(target + 0x344); b = r_u16(target + 0x34A); c = r_u16(target + 0x350);
                /* TODO: Native delta mapping retains the original opaque upper halfword; pointers are unchecked */
                view = moh_draft_local_address(delta, sizeof(delta)); first = r_u32(view); second = r_u32(view + 4);
                xport_gte_write_control(0, first); xport_gte_write_control(1, second);
                xport_gte_write_data(9, a); xport_gte_write_data(10, b); xport_gte_write_data(11, c); xport_gte_execute(0x41E012);
                first = xport_gte_read_data(25);
                if ((sint32)first < 0) { w_u32(owner + 0x298, 1); w_u32(owner + 0x290, 1); return 0; }
            }
        }
    }
    limit = r_u32(state + 0xC4); return (sint32)limit >= (sint32)distance;
}

static void moh_scene_matrix_native(uint32 view)
{
    uint32 x, y, z, a, b, c;
    x = r_u32(view + 32); a = r_u32(0x80098B64); w_u32(view + 20, x + a);
    y = r_u32(view + 36); a = r_u32(0x80098B68); z = r_u32(view + 40); b = r_u32(0x80098B6C);
    w_u32(view + 24, y + a); w_u32(view + 28, z + b);
    a = r_u32(view); b = r_u32(view + 4); xport_gte_write_control(0, a); xport_gte_write_control(1, b);
    a = r_u32(view + 8); b = r_u32(view + 12); c = r_u32(view + 16);
    xport_gte_write_control(2, a); xport_gte_write_control(3, b); xport_gte_write_control(4, c);
    a = r_u32(view + 20); b = r_u32(view + 24); xport_gte_write_control(5, a); c = r_u32(view + 28);
    xport_gte_write_control(6, b); xport_gte_write_control(7, c);
}

void level2p_sub_800685CC(uint32 owner)
{
    uint32 buffer[11], view;
    FUNCTION_MARKER(0x800685CC, "LEVEL2P.BIN");
    /* TODO: Native semantic matrix/vector mapping remains fail-fast */
    view = moh_draft_local_address(buffer, sizeof(buffer)); moh_draft_missing_call(0x800154EC, view, owner + 0x74, 32);
    moh_draft_missing_call(0x800163E8, 0x80098B50, view + 20, view + 32); moh_draft_missing_call(0x8001BD08, 0x80098B50, view);
    moh_scene_matrix_native(view);
}

void level2p_sub_8008BC04(uint32 position, uint32 rotation, uint32 scale)
{
    uint32 buffer[11], view;
    FUNCTION_MARKER(0x8008BC04, "LEVEL2P.BIN");
    /* TODO: Native semantic matrix/vector mapping remains fail-fast */
    view = moh_draft_local_address(buffer, sizeof(buffer)); level2p_sub_8008BB44(position, rotation, scale, view);
    moh_draft_missing_call(0x800163E8, 0x80098B50, view + 20, view + 32); moh_draft_missing_call(0x8001BD08, 0x80098B50, view);
    moh_scene_matrix_native(view);
}

void level2p_sub_80068694(uint32 owner, uint32 scene)
{
    uint32 flags, mode, first, second;
    FUNCTION_MARKER(0x80068694, "LEVEL2P.BIN");
    flags = r_u32(owner); if (!(flags & 1)) return;
    mode = r_u32(owner + 8);
    if (mode) level2p_sub_800685CC(owner); else level2p_sub_8008BC04(owner + 0x3C, owner + 0x60, owner + 0x24);
    first = r_u32(scene + 0x2218); second = r_u32(scene + 0x221C); level2p_sub_8007FB10(owner + 0x94, first, second, 0x80);
}

void level2p_sub_8007D528(void)
{
    FUNCTION_MARKER(0x8007D528, "LEVEL2P.BIN");
    level2p_sub_8004285C(0x19, 4, 5, 0, 0x10000, 0, 0x10000, 0, 0, 0, 0, 0);
    level2p_sub_8004285C(0, 0, 0x36, 0, 0x10000, 0, 0x10000, 0, 0, 0, 0, 0);
    level2p_sub_8004285C(0, 5, 5, 0, 0x10000, 0, 0x10000, 0, 0, 0, 0, 0);
}

void level2p_sub_8005F088(void)
{
    uint32 value, flags, mode;
    FUNCTION_MARKER(0x8005F088, "LEVEL2P.BIN");
    /* The original extra A0 value30 is unread by 89E84 */
    value = level2p_sub_80089E84();
    if (!value) { level2p_sub_80089E8C(); return; }
    flags = r_u32(0x80098B8C); if (flags & 0xF1) goto finish;
scene_check:
    flags = r_u32(0x80098B8C); if (flags & 0xF9) goto scene_finished;
    level2p_sub_8005D944(); level2p_sub_80066C08(); level2p_sub_8007D528();
    flags = r_u32(0x80098B8C); if (flags & 0xF5) goto scene_check;
life:
    flags = r_u32(0x80098B8C);
    if (flags & 2) level2p_sub_8005D9F4(); else level2p_sub_8005D490();
frame_check:
    flags = r_u32(0x80098B8C);
    if (!(flags & 0xF7)) {
        value = level2p_sub_8005DB48();
        if (value) level2p_sub_80041744();
        else {
            level2p_sub_8005CCF4(); flags = r_u32(0x80098B8C);
            if (flags & 0xF5) level2p_sub_800614FC();
        }
        goto frame_check;
    }
    flags = r_u32(0x80098B8C); if (!(flags & 0xF5)) goto life;
    goto scene_check;
scene_finished:
    flags = r_u32(0x80098B8C); mode = 0;
    if (flags & 8) mode = 1; else if (flags & 4) mode = 4;
    if (mode) { level2p_sub_8005D790(mode); flags = r_u32(0x80098B8C); w_u32(0x80098B8C, flags | 0x20); }
    flags = r_u32(0x80098B8C); if (!(flags & 0xF1)) goto scene_check;
finish:
    flags = r_u32(0x80098B8C);
    if (flags & 1) { level2p_sub_80089E74(); mode = 3; }
    else if (flags & 0x10) { level2p_sub_80089E7C(); mode = 4; }
    else if (flags & 0xC0) mode = 2;
    else return;
    level2p_sub_8005D790(mode); flags = r_u32(0x80098B8C); w_u32(0x80098B8C, flags | 0x60);
}

uint32 level2p_sub_8005F280(void)
{
    uint16 rectangle[4];
    uint32 flags, first, second, red, green, blue, view, handle;
    FUNCTION_MARKER(0x8005F280, "LEVEL2P.BIN");
    flags = r_u32(0x800386A4); if (flags & 8) w_u32(0x800A94A0, 1);
    moh_draft_missing_call(0x80015B28, 0x8003C428, 20); moh_draft_missing_call(0x8001D1D8); moh_draft_missing_call(0x8001CA78);
    for (handle = 0; handle < 4; ++handle) moh_draft_missing_call(0x8001AB2C, 0xF2000000 | handle);
    level2p_sub_8004B01C(); level2p_sub_8005CBB8(); level2p_sub_80049E24();
    red = r_u8(0x80039D27); green = r_u8(0x80039D28); blue = r_u8(0x80039D29);
    rectangle[0] = 0x8E; rectangle[1] = 0xCF; rectangle[2] = 0x3C; rectangle[3] = 6;
    /* TODO: Native progress rectangle mapping remains fail-fast */
    view = moh_draft_local_address(rectangle, sizeof(rectangle)); moh_draft_missing_call(0x80016FBC, view, red, green, blue);
    moh_draft_missing_call(0x8001CB08, 0x8008D008); flags = r_u32(0x80098B8C);
    while (!(flags & 0x20)) {
        first = r_u32(0x800968D0); second = r_u32(0x800968D4); level2p_sub_80051120(0x8009696C, first, second, 400);
        flags = r_u32(0x800386A4); w_u32(0x80096968, (flags & 2) != 0); level2p_sub_8005F088(); flags = r_u32(0x80098B8C);
    }
    moh_draft_missing_call(0x8001CB08, 0);
    return 0;
}

uint32 level2p_sub_8004E368(uint32 input, uint32 mask, uint32 inverse)
{
    FUNCTION_MARKER(0x8004E368, "LEVEL2P.BIN");
    return inverse ? (input & mask) == 0 : (input & mask) == mask;
}

uint32 level2p_sub_8004E38C(uint32 count, uint32 port, uint32 mask, uint32 edge, uint32 inverse)
{
    uint32 index, record, input, value;
    FUNCTION_MARKER(0x8004E38C, "LEVEL2P.BIN");
    count &= 0xFFFF; record = 0x8008FEE4 + port * 40;
    /* TODO: Original port and input history spans are unchecked */
    for (index = 0; index < count; ++index, record += 4) {
        input = r_u32(record); value = level2p_sub_8004E368(input, mask, inverse);
        if (value) {
            if (!edge) return 1;
            input = r_u32(record + 4); value = level2p_sub_8004E368(input, mask, inverse); if (!value) return 1;
        }
    }
    return 0;
}

void level2p_sub_8005F418(void)
{
    uint32 index;
    FUNCTION_MARKER(0x8005F418, "LEVEL2P.BIN");
    for (index = 0; index < 12; ++index) w_u8(0x80098B94 + index * 88, 0);
}

static uint32 moh_pause_input_native(uint32 mask)
{
    uint32 port, count;
    port = r_u32(0x800968F8); count = r_u16(0x800968DC);
    return level2p_sub_8004E38C((count + 1) & 0xFFFF, port, mask, 1, 0);
}

static uint32 moh_pause_navigation_native(void)
{
    uint32 value, port;
    value = moh_pause_input_native(0x8000); if (value) return 1;
    value = moh_pause_input_native(0x2000); if (value) return 1;
    port = r_u32(0x800968F8); value = (uint32)(sint32)r_s8(0x8008FE58 + port);
    if (!value) return 0;
    value = level2p_sub_8005DA58(port); if (value) return 1;
    port = r_u32(0x800968F8); return level2p_sub_8005DAD0(port) != 0;
}

static void moh_pause_capture_native(uint32 saved)
{
    uint32 channel, value;
    if (!saved) {
        for (channel = 0; channel < 24; ++channel) {
            moh_draft_missing_call(0x80015B88, channel, 0x8009906C + channel * 4, 0x8009906E + channel * 4);
            moh_draft_missing_call(0x8001D120, channel, 0x800990CC + channel * 2);
            moh_draft_missing_call(0x80015C08, channel, 0, 0); moh_draft_missing_call(0x800157BC, channel, 0);
        }
        w_u32(0x80099054, 1);
    }
    value = r_u32(0x80099058); if (!value) w_u32(0x80099058, 1);
}

static void moh_pause_panels_native(void)
{
    uint32 panel;
    panel = level2p_sub_8004F500(0, 0, 0x200, 0x78, 1, 0x6D, 0x77, 0x52, 2, 0, 1); w_u32(0x80096884, panel);
    panel = level2p_sub_8004F500(0, 0, 0x200, 0x78, 1, 0x6D, 0x77, 0x52, 2, 0, 2); w_u32(0x80096888, panel);
}

static void moh_pause_normal_native(uint32 saved)
{
    uint32 value, red, green;
    moh_pause_capture_native(saved); moh_pause_panels_native();
    level2p_sub_8005ABFC(0x8003C380, 0xDC, 0x1C, 0x80, 0x80, 0, 0, 0xFFFFFFFF, 1, 1, 0);
    level2p_sub_8005ABFC(0x8003C388, 0x30, 0x44, 0x5E, 0x5E, 0x5E, 0, 0xFFFFFFFF, 1, 2, 0);
    value = r_u32(0x80096910);
    if (value) { level2p_sub_8005ABFC(0x8003C3AC, 0x1C, 0x3A, 100, 100, 100, 0, 0xFFFFFFFF, 1, 1, 0); w_u32(0x80096914, 1); }
    value = r_u32(0x80096908);
    if (value) { level2p_sub_8005ABFC(0x8003C3D4, 0x1C, 0x26, 100, 100, 100, 0, 0xFFFFFFFF, 1, 2, 0); w_u32(0x8009690C, 1); }
    value = r_u32(0x800968FC);
    if (value) { level2p_sub_8005ABFC(0x8003C3FC, 100, 0x58, 0x80, 0x40, 0x40, 0, 0xFFFFFFFF, 1, 2, 0); red = 0x3F; green = 0x3F; }
    else { level2p_sub_8005ABFC(0x8003C3FC, 100, 0x58, 0x3F, 0x3F, 0x3F, 0, 0xFFFFFFFF, 1, 2, 0); red = 0x80; green = 0x40; }
    level2p_sub_8005ABFC(0x8003C408, 0x184, 0x58, red, green, green, 0, 0xFFFFFFFF, 1, 2, 0);
}

static void moh_pause_confirmation_native(void)
{
    uint32 value, red, green;
    moh_pause_panels_native();
    level2p_sub_8005ABFC(0x8003C408, 0xE8, 0x44, 0x80, 0x60, 0x14, 0, 0xFFFFFFFF, 1, 1, 0);
    level2p_sub_8005ABFC(0x8003C410, 0xB2, 0x66, 0x80, 0x60, 0x14, 0, 0xFFFFFFFF, 1, 1, 0);
    level2p_sub_8005ABFC(0x8003C388, 0x30, 0x44, 0x5E, 0x5E, 0x5E, 0, 0xFFFFFFFF, 1, 2, 0);
    value = r_u32(0x800968F4);
    if (value) { level2p_sub_8005ABFC(0x8003C420, 0x136, 0x10, 0x3F, 0x3F, 0x3F, 0, 0xFFFFFFFF, 1, 2, 0); red = 0x80; green = 0x40; }
    else { level2p_sub_8005ABFC(0x8003C420, 0x136, 0x10, 0x80, 0x40, 0x40, 0, 0xFFFFFFFF, 1, 2, 0); red = 0x3F; green = 0x3F; }
    level2p_sub_8005ABFC(0x8003C424, 0xB2, 0x10, red, green, green, 0, 0xFFFFFFFF, 1, 2, 0);
}

uint32 level2p_sub_8005DB48(void)
{
    uint32 value, count, disconnected = 0, pressed = 0, port, panel, choice, owner, first, second, channel;
    FUNCTION_MARKER(0x8005DB48, "LEVEL2P.BIN");
    /* TODO: Original controller, panel and sound snapshot spans are unchecked */
    value = level2p_sub_800504E0();
    if (!value) {
        value = r_u32(0x800968E8); if (value) goto paused;
        value = r_u32(0x80096908); if (value) { disconnected = 1; w_u32(0x800968F8, 1); }
        count = r_u16(0x800968DC); value = level2p_sub_8004E38C((count + 1) & 0xFFFF, 1, 0x800, 1, 0);
        if (value) { pressed = 1; w_u32(0x800968F8, 1); }
        value = r_u32(0x80096910); if (value) { disconnected = 1; w_u32(0x800968F8, 0); }
        count = r_u16(0x800968DC); value = level2p_sub_8004E38C((count + 1) & 0xFFFF, 0, 0x800, 1, 0);
        if (value) { pressed = 1; w_u32(0x800968F8, 0); }
        if (disconnected || pressed) {
            w_u32(0x80099064, pressed ? 0 : 1); w_u32(0x80099068, pressed ? 0 : 10);
            w_u32(0x800968E8, 1); w_u32(0x800968FC, 1); value = r_u32(0x80099054); w_u32(0x800968F4, 1);
            moh_pause_normal_native(value);
        }
    }
    value = r_u32(0x800968E8); if (!value) return 0;
paused:
    value = r_u32(0x800968EC);
    if (value) {
        value = r_u32(0x80096900);
        if (!value) {
            panel = r_u32(0x80096888); w_u32(0x80096900, 1); level2p_sub_8004F900(panel);
            panel = r_u32(0x80096884); level2p_sub_8004F900(panel); level2p_sub_8005AF7C(1); moh_pause_confirmation_native(); return 0;
        }
        value = moh_pause_input_native(0x40);
        if (!value) value = r_u32(0x800968F0);
        if (value) {
            value = r_u32(0x80096904); w_u32(0x800968F0, 1);
            if (!value) { owner = r_u32(0x8009943C); level2p_sub_800420C4(0, 0x13, owner, 0x20000, 0, 0x8000, 0, 0, 0, 0, 0); w_u32(0x80096904, 1); }
            port = r_u32(0x800968F8); value = r_u32(0x8008FE5C + port * 4); if (value & 0x40) return 0;
            choice = r_u32(0x800968F4); w_u32(0x80096904, 0);
            if (!choice) {
                panel = r_u32(0x80096888); w_u32(0x80096900, 0); level2p_sub_8004F900(panel);
                panel = r_u32(0x80096884); level2p_sub_8004F900(panel); level2p_sub_8005AF7C(1); level2p_sub_8005F418();
                value = r_u32(0x80098B8C); w_u32(0x80098B8C, value | 0x60); return 1;
            }
            w_u32(0x800968F0, 0); goto resume;
        }
        value = moh_pause_navigation_native();
        if (value) {
            owner = r_u32(0x8009943C); level2p_sub_800420C4(0, 0x21, owner, 0x10000, 0, 0x10000, 0, 0, 0, 0, 0);
            choice = r_u32(0x800968F4); panel = r_u32(0x80096884); w_u32(0x800968F4, choice ^ 1); level2p_sub_8004F900(panel);
            panel = r_u32(0x80096888); level2p_sub_8004F900(panel); level2p_sub_8005AF7C(1); moh_pause_confirmation_native(); return 0;
        }
        value = r_u32(0x80099064);
        if (!value) { value = r_u32(0x80099068); w_u32(0x80099068, value + 1); if ((sint32)value >= 10) w_u32(0x80099064, 1); return 0; }
        value = moh_pause_input_native(0x800); if (value) goto resume;
        return 0;
    }
    value = moh_pause_navigation_native();
    if (value) {
        owner = r_u32(0x8009943C); level2p_sub_800420C4(0, 0x21, owner, 0x10000, 0, 0x10000, 0, 0, 0, 0, 0);
        choice = r_u32(0x800968FC); panel = r_u32(0x80096888); w_u32(0x800968FC, choice ^ 1); level2p_sub_8004F900(panel);
        panel = r_u32(0x80096884); level2p_sub_8004F900(panel); level2p_sub_8005AF7C(1); value = r_u32(0x80099054); moh_pause_normal_native(value);
    } else {
        value = moh_pause_input_native(0x40); if (!value) value = r_u32(0x800968F0);
        if (value) {
            value = r_u32(0x80096904); w_u32(0x800968F0, 1);
            if (!value) { owner = r_u32(0x8009943C); level2p_sub_800420C4(0, 0x13, owner, 0x20000, 0, 0x8000, 0, 0, 0, 0, 0); w_u32(0x80096904, 1); }
            port = r_u32(0x800968F8); value = r_u32(0x8008FE5C + port * 4);
            if (!(value & 0x40)) {
                choice = r_u32(0x800968FC); w_u32(0x80096904, 0); w_u32(0x800968F0, 0); if (choice) goto resume;
                w_u32(0x800968EC, 1); w_u32(0x80096920, 0); w_u32(0x80096924, 0);
            }
        } else {
            value = r_u32(0x80099064);
            if (!value) { value = r_u32(0x80099068); w_u32(0x80099068, value + 1); if ((sint32)value >= 10) w_u32(0x80099064, 1); }
            else { value = moh_pause_input_native(0x800); if (value) goto resume; }
        }
    }
    first = r_u32(0x80096910); second = r_u32(0x80096914);
    if (first == second) { first = r_u32(0x80096908); second = r_u32(0x8009690C); }
    if (first != second) {
        panel = r_u32(0x80096888); level2p_sub_8004F900(panel); panel = r_u32(0x80096884); level2p_sub_8004F900(panel);
        level2p_sub_8005AF7C(1); value = r_u32(0x80099054); moh_pause_normal_native(value);
    }
    value = r_u32(0x80096910); if (!value) w_u32(0x80096914, 0);
    value = r_u32(0x80096908); if (!value) w_u32(0x8009690C, 0);
    return 0;
resume:
    panel = r_u32(0x80096888); level2p_sub_8004F900(panel); panel = r_u32(0x80096884); w_u32(0x80096900, 0); level2p_sub_8004F900(panel);
    level2p_sub_8005AF7C(1); w_u32(0x800968EC, 0); w_u32(0x800968E8, 0); w_u32(0x800968F0, 0); w_u32(0x80096920, 0);
    value = r_u32(0x80099054); w_u32(0x80096924, 0);
    if (value) {
        for (channel = 0; channel < 24; ++channel) {
            first = (uint32)(sint32)r_s16(0x8009906C + channel * 4); second = (uint32)(sint32)r_s16(0x8009906E + channel * 4);
            moh_draft_missing_call(0x80015C08, channel, first, second); value = r_u16(0x800990CC + channel * 2); moh_draft_missing_call(0x800157BC, channel, value);
        }
        w_u32(0x80099054, 0);
    }
    value = r_u32(0x80099058); if (value) w_u32(0x80099058, 0);
    return 0;
}

uint32 level2p_sub_8008175C(uint32 owner)
{
    static const uint32 models[32] = {
        0xE4F2812F, 0x0BD11E86, 0xD7D56DF3, 0x82292D75, 0x7995EE22, 0x30C394AB, 0x5EDAF160, 0xB669339C,
        0x03929C69, 0x98CD33D8, 0x839771D3, 0x745C9F92, 0xC56C576D, 0x4EFA45A0, 0xE4F2812F, 0x0BD11E86,
        0xD7D56DF3, 0x82292D75, 0x7995EE22, 0x30C394AB, 0x82292D75, 0xB669339C, 0x03929C69, 0x98CD33D8,
        0x839771D3, 0x745C9F92, 0xC56C576D, 0xD7D56DF3, 0x30C394AB, 0x03929C69, 0xC56C576D, 0x839771D3
    };
    uint8 packet[96];
    uint32 state, flags, model = 0x4EFA45A0, value, view, z, target, first_player, id, created, created_state, count, index, first, second, a, b;
    FUNCTION_MARKER(0x8008175C, "LEVEL2P.BIN");
    flags = r_u32(0x800386A4); state = r_u32(owner + 0x388);
    if (flags & 1) { value = sub_8001552C(); model = models[value & 31]; }
    /* TODO: Native packet mapping remains fail-fast; untouched packet bytes and flag bits remain opaque */
    /* TODO: Original actor, created object and count-dependent state spans are unchecked */
    value = (uint32)(sint32)r_s16(owner + 0x20E); view = moh_draft_local_address(packet, sizeof(packet)); w_u16(view + 0x10, value);
    value = (uint32)(sint32)r_s16(owner + 0x212); w_u16(view + 0x12, value);
    flags = r_u32(view + 0x58); z = (uint32)(sint32)r_s16(owner + 0x216);
    w_u16(view + 0x5E, 0); w_u16(view + 0x36, 0); w_u32(view + 0x58, flags & 0xFFFFFFFE);
    flags = r_u32(view + 0x30); w_u16(view + 0x14, z); w_u32(view + 0x30, flags & 0xFFFFFFF7);
    target = r_u32(0x80099444);
    first_player = r_u32(0x8009943C);
    if (target ? target == first_player : owner == first_player) first_player = r_u32(0x80099440);
    w_u32(0x80099444, first_player);
    flags = r_u32(0x800386A4); id = 0xFFFFFFFF;
    if (flags & 1) {
        target = r_u32(0x80099444); first_player = r_u32(0x8009943C); id = target == first_player ? 0x7FC2236C : 0xE6CB72D6;
    }
    /* The original extra A3 owner is unread by 82518 and its first helper */
    created = level2p_sub_80082518(view, model, id); level2p_sub_80066BA8(created);
    target = r_u32(0x80099444); created_state = r_u32(created + 0x388); w_u32(created + 0x1C, 0);
    level2p_sub_80068A04(created_state + 0x20, target);
    if (model == 0x4EFA45A0) {
        count = r_u16(0x8009A664); created_state = r_u32(created + 0x388); index = 0;
        if (count) {
            do {
                first = (uint32)(sint32)r_s16(state + index * 2 + 0x56);
                second = (uint32)(sint32)r_s16(state + index * 2 + 0x66);
                a = r_u16(state + index * 2 + 0x56); b = r_u16(state + index * 2 + 0x66);
                if ((sint32)(first + second) > 0) { w_u16(created_state + index * 2 + 0x24, a + b); w_u8(created_state + index + 0x2C, index); }
                else { w_u16(created_state + index * 2 + 0x24, 0); w_u8(created_state + index + 0x2C, 0); }
                index = (index + 1) & 0xFFFF; count = r_u16(0x8009A664);
            } while (index < count);
        }
        while (index < 4) {
            w_u16(created_state + index * 2 + 0x24, 0); w_u8(created_state + index + 0x2C, 0); index = (index + 1) & 0xFFFF;
        }
    }
    return created;
}

void level2p_sub_800708A0(uint32 player, uint32 source)
{
    uint32 input, index, record, assigned, value, sensitivity;
    FUNCTION_MARKER(0x800708A0, "LEVEL2P.BIN");
    input = 0x800B5D08 + player * 32 + source * 8; record = 0x800B5D48 + player * 0xA0;
    /* TODO: Original player, source and controller record spans are unchecked */
    for (index = 0; index < 4; ++index, record += 0x28) {
        assigned = r_u32(record);
        if (assigned == source) {
            value = r_u8(input); sensitivity = r_u32(record + 0x20); w_u8(record + 0xC, value);
            value = r_u8(input + 1); w_u8(record + 0xD, value); value = r_u8(input + 2); w_u8(record + 0xE, value);
            value = r_u8(input + 3); w_u8(record + 0xF, value); value = r_u8(input + 4); w_u8(record + 0x10, value);
            level2p_sub_8007045C(player, index, sensitivity, 1);
        }
    }
}

void level2p_sub_80070808(uint32 player, uint32 source)
{
    uint32 input;
    FUNCTION_MARKER(0x80070808, "LEVEL2P.BIN");
    input = 0x800B5D08 + player * 32 + source * 8;
    w_u8(input + 1, 0xFF); w_u8(input, 0); w_u8(input + 2, 0x5B); w_u8(input + 3, 0x5B); w_u8(input + 4, 0x5B);
    level2p_sub_800708A0(player, source);
}

void level2p_sub_80070858(uint32 player)
{
    uint32 source;
    FUNCTION_MARKER(0x80070858, "LEVEL2P.BIN");
    for (source = 0; source < 4; ++source) level2p_sub_80070808(player, source);
}

void level2p_sub_80074480(uint32 owner, uint32 port)
{
    uint32 state, flags, value, source, divisor, step;
    FUNCTION_MARKER(0x80074480, "LEVEL2P.BIN");
    state = r_u32(owner + 0x388); flags = r_u32(state); w_u32(state, (flags | 2) & 0xFFFFFFEF);
    value = level2p_sub_80068A64(owner + 0x2C);
    if (value) { w_u32(owner + 0x2D4, 0x960000); w_u16(owner + 0x2DA, 0); }
    if ((sint32)port < 0) return;
    level2p_sub_8007045C(port, 0, 2, 4); level2p_sub_8007045C(port, 1, 2, 4);
    level2p_sub_8006FFC0(port, 0, 2, 4); level2p_sub_8006FFC0(port, 1, 2, 4);
    source = r_u8(state + 0x726); value = level2p_sub_800700F8(source, 1); divisor = value + 1;
    if (!divisor) { moh_draft_fail_fast("MIPS BREAK 0x1C00", 0x80074540); return; }
    step = (uint32)(0x10000000 / (sint32)divisor); source = r_u8(state + 0x726);
    w_u32(state + 0x4B4, 0); w_u32(state + 0x4C0, 0); w_u32(state + 0x4B0, step); w_u32(state + 0x4BC, 0 - step);
    value = level2p_sub_800700F8(source, 0); divisor = value + 1;
    if (!divisor) { moh_draft_fail_fast("MIPS BREAK 0x1C00", 0x80074590); return; }
    step = (uint32)(0x10000000 / (sint32)divisor); source = r_u8(state + 0x726);
    w_u32(state + 0x4C8, 0); w_u32(state + 0x4D4, 0); w_u32(state + 0x4B8, step); w_u32(state + 0x4C4, 0 - step);
    value = level2p_sub_800700F8(source, 3); divisor = value + 1;
    if (!divisor) { moh_draft_fail_fast("MIPS BREAK 0x1C00", 0x800745E0); return; }
    step = (uint32)(0x10000000 / (sint32)divisor); flags = r_u32(state);
    w_u32(state + 0x4D0, 0); w_u32(state + 0x4DC, 0); w_u32(state + 0x4CC, step); w_u32(state + 0x4D8, 0 - step);
    w_u32(state + 0x3C, flags & 2 ? 0xFFFFFF6A : 0xFFFFFF10);
}

void level2p_sub_80076118(uint32 owner, uint32 state, uint32 port)
{
    uint32 input, target, value, layout, inverted, option, view, flags, mode, message;
    FUNCTION_MARKER(0x80076118, "LEVEL2P.BIN");
    /* TODO: Original port, actor and controller state spans are unchecked */
    input = r_u32(0x8008FE5C + port * 4);
    if ((input & 0x101F) == 0x101F) {
        target = r_u32(0x8009943C); if (owner == target) target = r_u32(0x80099440);
        w_u32(0x80099444, target); level2p_sub_8008175C(owner);
    }
    value = r_u32(state + 0x708);
    if (value) {
        w_u32(owner + 0x218, 0); w_u32(owner + 0x21C, 0); w_u32(owner + 0x220, 0); w_u32(owner + 0x200, 0);
        value = r_u32(state + 0x708); w_u32(state + 0x708, value - 1); return;
    }
    value = r_u8(0x8008FE58 + port); w_u8(state + 0x727, value);
    if (port) {
        layout = r_u32(0x800386D8); inverted = r_u32(0x800386BC); option = r_u32(0x800386C8); view = r_u32(0x800386CC);
    } else {
        layout = r_u32(0x800386D4); inverted = r_u32(0x800386A8); option = r_u32(0x800386B4); view = r_u32(0x800386B8);
    }
    if (view) {
        w_u16(state + 0x688, 0xD0); w_u16(state + 0x68A, 0x2B); flags = r_u32(state); flags |= 0x400000;
    } else {
        w_u16(state + 0x688, 0x40); flags = r_u32(state); w_u16(state + 0x68A, 15); flags &= 0xFFBFFFFF;
    }
    w_u32(state, flags); flags = r_u32(state); w_u32(state, option ? flags & 0xFFF7FFFF : flags | 0x80000);
    if (layout != 1 && layout != 2 && layout != 3 && layout != 4) layout = 0;
    mode = (uint32)(sint32)r_s8(state + 0x727);
    if (!mode) {
        level2p_sub_80070980(port, 5, inverted); level2p_sub_80070858(port); w_u16(state + 0x76C, 5); message = 0x8003CF44;
    } else {
        level2p_sub_80070980(port, layout, inverted); level2p_sub_80070858(port); w_u16(state + 0x76C, layout); message = 0x8003CF60;
    }
    w_u16(state + 0x76E, inverted); moh_draft_missing_call(0x80015B28, message); w_u32(state + 0x708, 30);
    flags = r_u32(state); w_u32(state, flags & 0xFFFFFFEF); flags = r_u32(state);
    level2p_sub_800742C8(owner, port); if (flags & 2) level2p_sub_80074480(owner, port);
}

/* Preserve the two aligned reads of the original LWL/LWR pair */
static uint32 moh_resource_lwl_lwr(uint32 address)
{
    uint32 high, low, shift;
    high = r_u32((address + 3) & 0xFFFFFFFC); low = r_u32(address & 0xFFFFFFFC); shift = (address & 3) * 8;
    return shift ? (low >> shift) | (high << (32 - shift)) : low;
}

/* Preserve both writes and partial-word merges of SWL/SWR */
static void moh_resource_swl_swr(uint32 address, uint32 value)
{
    uint32 aligned, index, mask, previous;
    aligned = (address + 3) & 0xFFFFFFFC; index = (address + 3) & 3;
    if (index == 3) w_u32(aligned, value);
    else {
        mask = (1 << ((index + 1) * 8)) - 1; previous = r_u32(aligned);
        w_u32(aligned, (previous & ~mask) | (value >> (24 - index * 8)));
    }
    aligned = address & 0xFFFFFFFC; index = address & 3;
    if (!index) w_u32(aligned, value);
    else {
        mask = 0xFFFFFFFF << (index * 8); previous = r_u32(aligned);
        w_u32(aligned, (previous & ~mask) | (value << (index * 8)));
    }
}

static void moh_level_progress_native(uint32 rectangle, uint32 width)
{
    uint32 red, green, blue;
    red = r_u8(0x80039D27); green = r_u8(0x80039D28); blue = r_u8(0x80039D29);
    w_u16(rectangle, 0x8E); w_u16(rectangle + 2, 0xCF); w_u16(rectangle + 4, width); w_u16(rectangle + 6, 6);
    moh_draft_missing_call(0x80016FBC, rectangle, red, green, blue); moh_draft_missing_call(0x80016E38, 0);
}

uint32 level2p_sub_8008695C(uint32 mission, uint32 level)
{
    static const uint32 taf_formats[3] = {0x8003D6C0, 0x8003D6E4, 0x8003D6F0};
    static const uint32 banks[3] = {4, 5, 1};
    static const uint32 bsd_formats[3] = {0x8003D744, 0x8003D750, 0x8003D75C};
    static const uint32 inherited_words[12] = {8, 0x18, 0x20, 0x24, 0x28, 0x2C, 0x30, 0x34, 0x38, 0x84, 0x9C, 0xC4};
    uint8 filename[128]; uint16 rectangle[4]; uint32 selected[2];
    uint32 filename_view, rectangle_view, selected_view, value, first, second, index, table, path, file, data, base, scene;
    uint32 record, end, count, additional, total, model, parent, parent_index, id, field, source, destination, inner;
    uint32 registered = 0, owner, state, red, green, blue, fog, enabled;
    FUNCTION_MARKER(0x8008695C, "LEVEL2P.BIN");
    value = r_u32(0x80039C78); w_u32(0x800B60E8, value);
    value = r_u32(0x80039C7C); w_u32(0x800B60EC, value);
    value = r_u32(0x80039C70); w_u32(0x800B60F0, value); level2p_sub_80085F80(0);
    /* TODO: Native filename, rectangle and loader-output mappings remain fail-fast */
    /* TODO: Original format output, resource tables, parent lookup and scene spans are unchecked */
    value = r_u8(0x8008FCC0); filename_view = moh_draft_local_address(filename, sizeof(filename));
    if (!value) {
        value = r_u32(0x80096968);
        moh_draft_missing_call(0x8001BE18, filename_view, 0x8003D68C, mission, level, mission, level, value);
    } else moh_draft_missing_call(0x8001BE18, filename_view, 0x8003D6B0, 0x8008FCC0, mission, level);
    file = moh_draft_missing_call(0x800363FC, filename_view, 0, 0);
    if (file) { data = level2p_sub_80085E40(file); level2p_sub_80085EF4(data, 0); moh_draft_missing_call(0x80036E44, file); }
    rectangle_view = moh_draft_local_address(rectangle, sizeof(rectangle)); moh_level_progress_native(rectangle_view, 0x5A);
    for (index = 0; index < 3; ++index) {
        value = r_u32(0x800B60E8 + index * 4); table = index == 2 ? 0x8009B77C : 0x8009B694;
        path = r_u32(table + value * 4);
        moh_draft_missing_call(0x8001BE18, filename_view, taf_formats[index], index == 2 ? 0x8003D6FC : 0x8003D6CC, path);
        file = moh_draft_missing_call(0x800363FC, filename_view, 0, 0); data = level2p_sub_80085E40(file);
        level2p_sub_80085EF4(data, banks[index]); moh_draft_missing_call(0x80036E44, file);
    }
    value = r_u8(0x8008FCC0);
    if (!value) moh_draft_missing_call(0x8001BE18, filename_view, 0x8003D714, mission, level, mission, level);
    else moh_draft_missing_call(0x8001BE18, filename_view, 0x8003D734, 0x8008FCC0, mission, level);
    selected_view = moh_draft_local_address(selected, sizeof(selected)); base = level2p_sub_8004AD5C(filename_view, selected_view);
    moh_level_progress_native(rectangle_view, 0x78); w_u32(0x80098FB0, base); level2p_sub_800864D8();
    scene = r_u32(0x80098FB0); enabled = r_u32(scene + 0xA0);
    if (enabled) { w_u16(scene + 0xB0, 0); w_u16(scene + 0xB2, 0); w_u16(scene + 0xB4, 0); w_u16(scene + 0xB6, 0); }
    w_u32(0x8009B798, 0);
    value = r_u32(0x800B60E8); path = r_u32(0x8009B694 + value * 4); count = r_u32(selected_view);
    w_u32(0x8009B790, count + 1);
    moh_draft_missing_call(0x8001BE18, filename_view, bsd_formats[0], 0x8003D6CC, path); level2p_sub_800867C0(filename_view);
    for (index = 1; index < 3; ++index) {
        value = r_u32(0x800B60E8 + index * 4); table = index == 2 ? 0x8009B77C : 0x8009B694;
        path = r_u32(table + value * 4);
        moh_draft_missing_call(0x8001BE18, filename_view, bsd_formats[index], index == 2 ? 0x8003D6FC : 0x8003D6CC, path);
        level2p_sub_800867C0(filename_view);
    }
    scene = r_u32(0x80098FB0); w_u32(0x8009B794, base + 0x5A8); end = scene + 0x320; record = scene;
    do {
        value = r_u32(record + 0xDC);
        if (value) { value = r_u32(record + 0xE0); w_u32(record + 0xE0, base + value); }
        else w_u32(record + 0xE0, 0);
        w_u32(record + 0xEC, 0); record += 20;
    } while (record < end);
    scene = r_u32(0x80098FB0); record = scene;
    for (index = 0; index < 20; ++index, record += 16) { w_u16(record + 0x40A, 0); w_u16(record + 0x408, 0); }
    scene = r_u32(0x80098FB0);
    for (index = 0; index < 9; ++index) {
        field = 0x544 + index * 8; value = r_u32(scene + field); if (value) w_u32(scene + field, base + value);
        if (index < 8 && ((index & 1) || value)) scene = r_u32(0x80098FB0);
    }
    moh_level_progress_native(rectangle_view, 0x96);
    scene = r_u32(0x80098FB0); level2p_sub_800860FC(scene);
    scene = r_u32(0x80098FB0); level2p_sub_80086178(scene);
    scene = r_u32(0x80098FB0); level2p_sub_800861EC(scene);
    scene = r_u32(0x80098FB0); level2p_sub_80086294(scene);
    scene = r_u32(0x80098FB0); level2p_sub_80086310(scene);
    scene = r_u32(0x80098FB0); level2p_sub_80086384(scene);
    count = r_u32(0x8009B790); index = 0;
    if (count) {
        do {
            model = level2p_sub_80065BB8(index); scene = r_u32(0x80098FB0); level2p_sub_800864E0(model, scene, base);
            count = r_u32(0x8009B790); index += 1;
        } while (index < count);
    }
    index = 0;
    for (;;) {
        count = r_u32(0x8009B790); additional = r_u32(0x8009B798); total = count + additional;
        if (index >= total) break;
        model = level2p_sub_80065BB8(index); value = r_u32(model + 0xE8);
        if (value != 0xFFFFFFFF) {
            parent = 0; parent_index = 0; count = r_u32(0x8009B790); additional = r_u32(0x8009B798); total = count + additional;
            if (total) {
                do {
                    parent = level2p_sub_80065BB8(parent_index); id = r_u32(parent); value = r_u32(model + 0xE8);
                    if (id == value) break;
                    count = r_u32(0x8009B790); additional = r_u32(0x8009B798); parent_index += 1; total = count + additional;
                } while (parent_index < total);
            }
            value = r_u32(model + 0xB0);
            if (!value) { value = r_u32(parent + 0xB0); w_u32(model + 0xB0, value); value = r_u32(parent + 0xB4); w_u32(model + 0xB4, value); }
            value = r_u32(model + 0xA0);
            if (!value) {
                value = r_u32(parent + 0xA0); w_u32(model + 0xA0, value);
                if (!value) { value = r_u32(parent + 0xA0); w_u32(model + 0xA0, value); }
            }
            value = r_u32(model + 0xA4);
            if (!value) {
                value = r_u32(model + 0xA8);
                if (!value) {
                    value = r_u32(model + 0xAC);
                    if (!value) {
                        value = r_u32(parent + 0xA4); w_u32(model + 0xA4, value);
                        value = r_u32(parent + 0xA8); w_u32(model + 0xA8, value); value = r_u32(parent + 0xAC); w_u32(model + 0xAC, value);
                    }
                }
            }
            value = r_u16(model + 0xC8); if (!value) { value = r_u16(parent + 0xC8); w_u16(model + 0xC8, value); }
            value = r_u32(model + 4);
            if (!value) {
                value = r_u32(parent + 4); w_u32(model + 4, value);
                first = moh_resource_lwl_lwr(parent + 0x3C); second = moh_resource_lwl_lwr(parent + 0x40);
                moh_resource_swl_swr(model + 0x3C, first); moh_resource_swl_swr(model + 0x40, second);
                source = parent; destination = model;
                for (inner = 0; inner < 8; ++inner, source += 8, destination += 8) {
                    value = r_u16(source + 0x44); w_u16(destination + 0x44, value);
                    value = r_u16(source + 0x46); w_u16(destination + 0x46, value); value = r_u16(source + 0x48); w_u16(destination + 0x48, value);
                }
                value = r_u32(parent + 0x88); w_u32(model + 0x88, value);
                value = r_u16(parent + 0x8C); w_u16(model + 0x8C, value); value = r_u16(parent + 0x8E); w_u16(model + 0x8E, value);
                value = r_u32(parent + 0x90); w_u32(model + 0x90, value); value = r_u16(parent + 0x96); w_u16(model + 0x96, value);
                value = r_u32(parent + 0x98); w_u32(model + 0x98, value);
                value = r_u16(parent + 0xCC); w_u16(model + 0xCC, value); value = r_u16(parent + 0xCE); w_u16(model + 0xCE, value);
                value = r_u16(parent + 0xD0); w_u16(model + 0xD0, value);
            }
            value = r_u32(model + 12);
            if (!value) { value = r_u32(parent + 12); w_u32(model + 12, value); value = r_u16(parent + 0x94); w_u16(model + 0x94, value); }
            value = (uint32)(sint32)r_s16(model + 0xD4);
            if ((sint32)value < 0) {
                value = r_u16(parent + 0xD4); w_u16(model + 0xD4, value);
                first = moh_resource_lwl_lwr(parent + 0xD8); second = moh_resource_lwl_lwr(parent + 0xDC);
                moh_resource_swl_swr(model + 0xD8, first); moh_resource_swl_swr(model + 0xDC, second);
                value = (uint32)(sint32)r_s16(model + 0xD4);
                if ((sint32)value < 0) {
                    value = r_u16(parent + 0xD6); w_u16(model + 0xD6, value);
                    first = moh_resource_lwl_lwr(parent + 0xE0); second = moh_resource_lwl_lwr(parent + 0xE4);
                    moh_resource_swl_swr(model + 0xE0, first); moh_resource_swl_swr(model + 0xE4, second);
                }
            }
            value = r_u32(model + 0xC0);
            if (!value) { value = r_u32(parent + 0xC0); w_u32(model + 0xC0, value); value = r_u16(parent + 0xBE); w_u16(model + 0xBE, value); }
            value = r_u32(model + 0xB8);
            if (!value) { value = r_u32(parent + 0xB8); w_u32(model + 0xB8, value); value = r_u16(parent + 0xBC); w_u16(model + 0xBC, value); }
            value = r_u32(model + 0x10);
            if (!value) { value = r_u32(parent + 0x10); w_u32(model + 0x10, value); value = r_u32(parent + 0x14); w_u32(model + 0x14, value); }
            for (inner = 0; inner < 12; ++inner) {
                field = inherited_words[inner]; value = r_u32(model + field);
                if (!value) { value = r_u32(parent + field); w_u32(model + field, value); }
            }
            value = r_u32(model + 0xFC); if (value == 0xFFFFFFFF) { value = r_u32(parent + 0xFC); w_u32(model + 0xFC, value); }
            for (inner = 0; inner < 4; ++inner) {
                value = r_u32(model + 0xEC + inner * 4);
                if (value == 0xFFFFFFFF) {
                    for (field = 0; field < 4; ++field) { value = r_u32(parent + 0xEC + field * 4); w_u32(model + 0xEC + field * 4, value); }
                    break;
                }
            }
        }
        value = r_u32(model + 0xFC);
        if (value == 1) {
            id = r_u32(model); record = 0x8008F920;
            for (inner = 0; inner < 13; ++inner, record += 0x44) {
                value = r_u32(record);
                if (value == id) { w_u8(0x8009AB8C + registered, inner); registered += 1; w_u32(record + 0x40, model); break; }
            }
        }
        index += 1;
    }
    moh_level_progress_native(rectangle_view, 0xB4); w_u32(0x8009A664, registered); owner = level2p_sub_800538C0();
    scene = r_u32(0x80098FB0); state = r_u32(owner + 0x388); value = r_u32(scene + 0x59C); w_u32(0x80099430, owner);
    if (value) w_u32(scene + 0x59C, base + value);
    scene = r_u32(0x80098FB0); value = r_u32(scene + 0x53C);
    if (value) {
        owner = r_u32(0x80099430); value += base; w_u32(scene + 0x53C, value); w_u32(state, value); level2p_sub_8006F5D0(owner);
        scene = r_u32(0x80098FB0);
    }
    value = r_u32(scene + 0x598);
    if (value) { value += base; w_u32(scene + 0x598, value); level2p_sub_800863F8(value); }
    scene = r_u32(0x80098FB0); value = r_u32(scene + 0x540);
    if (value) {
        value += base; w_u32(scene + 0x540, value); w_u32(state + 4, value); w_u32(state + 8, value + 8);
        value = r_u32(value); w_u32(state + 12, value); scene = r_u32(0x80098FB0);
    }
    value = r_u32(scene + 0x5A0);
    if (value) { value += base; w_u32(scene + 0x5A0, value); level2p_sub_8006F740(value); }
    else { w_u32(0x8009A080, 0); w_u32(0x8009A07C, 0); }
    scene = r_u32(0x80098FB0); fog = (uint32)(sint32)r_s16(scene + 0xCC);
    if (fog) {
        red = r_u8(scene + 0xCE); green = r_u8(scene + 0xCF); blue = r_u8(scene + 0xD0);
        xport_gte_write_control(13, red << 4); xport_gte_write_control(14, green << 4); xport_gte_write_control(15, blue << 4);
        value = (uint32)(sint32)r_s16(scene + 0xD4);
        if (value) { xport_gte_write_control(21, 0x800); xport_gte_write_control(22, 0x800); xport_gte_write_control(23, 0x800); }
        else {
            red = r_u8(scene + 0xCE); green = r_u8(scene + 0xCF); blue = r_u8(scene + 0xD0);
            xport_gte_write_control(21, red << 4); xport_gte_write_control(22, green << 4); xport_gte_write_control(23, blue << 4);
        }
        scene = r_u32(0x80098FB0); fog = (uint32)(sint32)r_s16(scene + 0xCC);
    } else {
        xport_gte_write_control(13, 0); xport_gte_write_control(14, 0); xport_gte_write_control(15, 0);
        red = r_u8(scene + 0xCE); green = r_u8(scene + 0xCF); blue = r_u8(scene + 0xD0);
        xport_gte_write_control(21, red << 4); xport_gte_write_control(22, green << 4); xport_gte_write_control(23, blue << 4);
    }
    moh_draft_missing_call(0x8001C6D8, fog, 400); level2p_sub_80061250(); level2p_sub_800866F4();
    owner = r_u32(0x80099430); level2p_sub_80066BA8(owner); level2p_sub_8005C7F0();
    moh_level_progress_native(rectangle_view, 0xD2); level2p_sub_800416A8(); w_u32(0x8009B690, 1);
    return 0;
}

uint32 level2p_sub_80085BF8(uint32 owner)
{
    uint32 data, value, first, second, index, source, destination, x, y, z, scale, product;
    uint32 count, table, entry, size, record, next, inner, offset, point;
    FUNCTION_MARKER(0x80085BF8, "LEVEL2P.BIN");
    data = r_u32(owner + 4); w_u32(owner + 0x98, 0);
    /* TODO: Original model, count and animation record spans are unchecked */
    if (data) {
        value = r_u16(data); w_u16(owner + 0x96, value); value = r_u16(data + 4); w_u16(owner + 0x8C, value);
        first = moh_resource_lwl_lwr(data + 8); second = moh_resource_lwl_lwr(data + 12);
        moh_resource_swl_swr(owner + 0x3C, first); moh_resource_swl_swr(owner + 0x40, second);
        first = moh_resource_lwl_lwr(data + 16); second = moh_resource_lwl_lwr(data + 20);
        moh_resource_swl_swr(owner + 0xCC, first); moh_resource_swl_swr(owner + 0xD0, second);
        source = data; destination = owner;
        for (index = 0; index < 8; ++index, source += 8, destination += 8) {
            first = moh_resource_lwl_lwr(source + 0x18); second = moh_resource_lwl_lwr(source + 0x1C);
            moh_resource_swl_swr(destination + 0x44, first); moh_resource_swl_swr(destination + 0x48, second);
            x = (uint32)(sint32)r_s16(destination + 0x44); scale = r_u32(owner + 0xA4); product = x * scale;
            y = (uint32)(sint32)r_s16(destination + 0x46); w_u16(destination + 0x44, (uint32)((sint32)product >> 16));
            scale = r_u32(owner + 0xA8); product = y * scale;
            z = (uint32)(sint32)r_s16(destination + 0x48); w_u16(destination + 0x46, (uint32)((sint32)product >> 16));
            scale = r_u32(owner + 0xAC); product = z * scale; w_u16(destination + 0x48, (uint32)((sint32)product >> 16));
        }
        count = r_u16(owner + 0x96);
        if (!count) { w_u32(owner + 0x98, 0); return 0; }
        table = moh_draft_missing_call(0x80036BF0, count << 4); w_u32(owner + 0x98, table);
        if (table) {
            record = data + 0x58; count = r_u16(owner + 0x96); index = 0;
            while (index < count) {
                table = r_u32(owner + 0x98); entry = table + (index << 4);
                w_u16(entry, 0xFFFF); w_u16(entry + 2, 0xFFFF);
                next = r_u32(record); size = r_u16(record + 4); w_u16(entry + 4, size); w_u32(entry + 8, record + 8);
                value = record + size * 24 + 8; size = r_u16(entry + 4); w_u32(entry + 12, value);
                inner = 0; offset = 0;
                while (inner < size) {
                    point = r_u32(entry + 8) + offset; value = r_u16(point + 2);
                    if (value != 0xFFFF) { first = r_u32(entry + 12); first = (first - record) >> 2; w_u16(point + 2, value - first); }
                    size = r_u16(entry + 4); inner += 1; offset += 24;
                }
                count = r_u16(owner + 0x96); index += 1; record += next;
            }
            return 0;
        }
    }
    value = r_u32(owner + 0x98); if (value) moh_draft_missing_call(0x80036E44, value);
    w_u32(owner + 0x98, 0); w_u32(owner, 0xFFFFFFFF);
    return 0xFFFFFFFF;
}

uint32 level2p_sub_80077FA4(uint32 owner, uint32 state)
{
    static const uint32 active[13] = {0, 1, 5, 6, 0, 2, 1, 6, 0, 0, 6, 1, 0};
    static const uint32 idle[13] = {7, 8, 9, 9, 7, 7, 8, 9, 7, 7, 9, 8, 7};
    uint32 flags, kind;
    FUNCTION_MARKER(0x80077FA4, "LEVEL2P.BIN");
    flags = r_u32(state); kind = r_u8(state + 0x55);
    if (kind >= 13) return 7;
    return flags & 0x20 ? active[kind] : idle[kind];
}

void level2p_sub_800786F4(uint32 owner)
{
    uint32 player, state, value, port, mode, angle, delta, previous, gain, result;
    FUNCTION_MARKER(0x800786F4, "LEVEL2P.BIN");
    player = r_u32(0x8009943C); state = r_u32(owner + 0x388); w_u8(state + 0x726, owner == player ? 0 : 1);
    w_u32(owner + 0xF0, 0x200000); w_u32(owner + 0xF4, 0x200000); w_u32(state + 0x6EC, 0xFFFFFFFF);
    level2p_sub_80078148(owner, 0);
    w_u32(state + 0x72C, 0); w_u32(owner + 0x2D4, 0xF00000); w_u32(state + 0x34, 0xF00000);
    value = r_u8(0x8008FCE0); port = r_u8(state + 0x726);
    w_u32(state, 0x400000); w_u16(state + 0x688, 0xD0); w_u8(state + 0x727, 0xFF); w_u16(state + 0x68A, 0x2B); w_u32(state + 8, value);
    level2p_sub_80076118(owner, state, port);
    w_u32(state + 0x6F0, 0); w_u32(state + 0x6F8, 0); w_u32(state + 0x700, 0);
    w_u32(state + 0x4C, 0); w_u32(state + 0x50, 0); w_u32(state + 0x6FC, 0); w_u32(state + 0x708, 0); w_u32(state + 0x70C, 0);
    level2p_sub_8005A1B8(owner, 14, 0); level2p_sub_8005A1B8(owner, 2, 0);
    w_u32(owner + 0x240, 0); w_u32(owner + 0x244, 0); w_u32(owner + 0x248, 0); w_u32(owner + 0x284, 1);
    w_u32(state + 0x1F0, 0); w_u32(state + 0x33C, 0xFFFFFFFF); w_u16(state + 0x4EC, 0xFFFF); w_u16(state + 0x714, 0); w_u32(state + 0x7D0, 0);
    level2p_sub_80046DD8(); level2p_sub_8006F890(owner);
    mode = r_u32(state + 0x844);
    if (mode < 2) {
        angle = r_u32(owner + 0x228); delta = r_u32(owner + 0x234); angle += delta;
        gain = r_u32(state + 0x7F8); previous = r_u32(state + 0x7F4); w_u32(state + 0x7FC, angle - (previous - (gain << 2)));
        angle = r_u32(owner + 0x22C); delta = r_u32(owner + 0x238); angle += delta;
        gain = r_u32(state + 0x804); previous = r_u32(state + 0x800); w_u32(state + 0x808, angle - (previous + (gain << 3)));
    } else {
        angle = r_u32(owner + 0x228); delta = r_u32(owner + 0x234); previous = r_u32(state + 0x7F4); w_u32(state + 0x7FC, angle + delta - previous);
        angle = r_u32(owner + 0x22C); delta = r_u32(owner + 0x238); previous = r_u32(state + 0x800); w_u32(state + 0x808, angle + delta - previous);
    }
    level2p_sub_8006591C(owner); level2p_sub_8004DFBC(owner); value = level2p_sub_80065BF8(0x3C8662);
    if ((sint32)value <= 0) w_u32(state + 0x704, 0);
    else { value = level2p_sub_80065BB8(value); w_u32(state + 0x704, value); }
    value = r_u32(owner + 0x370); w_u32(state + 0x678, 400); w_u32(state + 0x680, 1); w_u32(state + 0x684, value);
    result = level2p_sub_80077FA4(owner, state); w_u32(state + 0x844, result);
    if (result == 5) { w_u16(state + 0x80C, 0x10E); value = 0x37; }
    else { w_u16(state + 0x80C, 0x100); value = 0x47; }
    w_u16(state + 0x80E, value); w_u16(state + 0x810, 0); w_u16(state + 0x812, 0);
    w_u32(state + 0x848, result); w_u16(state + 0x84C, 0); w_u16(state + 0x84E, 0);
}

void level2p_sub_8006F454(uint32 data)
{
    uint32 offset, value;
    FUNCTION_MARKER(0x8006F454, "LEVEL2P.BIN");
    for (offset = 0x38; offset <= 0x50; offset += 4) {
        value = r_u32(data + offset); if (value) w_u32(data + offset, data + value);
    }
}

void level2p_sub_8006F4E8(uint32 data)
{
    uint32 value;
    FUNCTION_MARKER(0x8006F4E8, "LEVEL2P.BIN");
    value = r_u32(data + 0x60); if (value) w_u32(data + 0x60, data + value);
}

void level2p_sub_8006F504(uint32 data)
{
    uint32 value;
    FUNCTION_MARKER(0x8006F504, "LEVEL2P.BIN");
    value = r_u32(data + 0x60); if (value) w_u32(data + 0x60, data + value);
}

void level2p_sub_8006F520(uint32 data)
{
    uint32 value;
    FUNCTION_MARKER(0x8006F520, "LEVEL2P.BIN");
    value = r_u32(data + 0x74); if (value) w_u32(data + 0x74, data + value);
}

uint32 level2p_sub_8006F5D0(uint32 owner)
{
    uint32 state, data, count, base, table, record, value, kind, index = 0;
    FUNCTION_MARKER(0x8006F5D0, "LEVEL2P.BIN");
    state = r_u32(owner + 0x388); data = r_u32(state);
    if (!data) return 0xFFFFFFFF;
    count = r_u32(data); w_u32(state + 0x14, data + 16); w_u32(state + 0x10, count);
    base = data + count * 8 + 16; count = r_u32(state + 0x10); w_u32(0x80094C68, base);
    table = r_u32(state + 0x14); w_u32(state + 0x18, base); record = table + 4;
    /* TODO: Original live signed count, descriptor table and relocated pointers are unchecked */
    while ((sint32)index < (sint32)count) {
        base = r_u32(0x80094C68); value = r_u32(record); data = base + value; w_u32(record, data); kind = r_u32(data + 12);
        switch (kind) {
        case 2:
            data = r_u32(record); index += 1; level2p_sub_8006F454(data);
            data = r_u32(record); level2p_sub_8006F4E8(data); break;
        case 3:
            data = r_u32(record); index += 1; level2p_sub_8006F454(data);
            data = r_u32(record); level2p_sub_8006F520(data); break;
        case 6:
            data = r_u32(record); index += 1; level2p_sub_8006F454(data);
            data = r_u32(record); level2p_sub_8006F504(data); break;
        case 1: case 4: case 5:
            data = r_u32(record); level2p_sub_8006F454(data); index += 1; break;
        default: index += 1; break;
        }
        count = r_u32(state + 0x10); record += 8;
    }
    return 0;
}

void level2p_sub_800864E0(uint32 owner, uint32 scene, uint32 base)
{
    static const uint32 fields[3] = {0x38, 0x84, 0x9C};
    static const uint32 bases[3] = {0x564, 0x574, 0x55C};
    static const uint32 before[4] = {0xB4, 0x88, 0x90, 0xC4};
    static const uint32 after[5] = {0x24, 0x2C, 0x30, 0x34, 0xC0};
    uint32 value, table, count, index, entry, relocated, offset, field;
    FUNCTION_MARKER(0x800864E0, "LEVEL2P.BIN");
    /* TODO: Original model and variable relocation-table spans are unchecked */
    value = r_u32(owner + 8);
    if (value == 0xFFFFFFFF) w_u32(owner + 8, 0);
    else {
        table = base + value; w_u32(owner + 8, table); count = r_u16(table);
        if (count) {
            index = 0;
            do {
                entry = table + 4 + index * 4; value = r_u32(entry);
                if (value == 0xFFFFFFFF) w_u32(entry, 0);
                else { relocated = r_u32(scene + 0x544); w_u32(entry, value + relocated); }
                index = (index + 1) & 0xFFFF; count = r_u16(table);
            } while (index < count);
        }
    }
    for (index = 0; index < 3; ++index) {
        field = owner + fields[index]; value = r_u32(field);
        if (value == 0xFFFFFFFF) w_u32(field, 0);
        else { relocated = r_u32(scene + bases[index]); w_u32(field, value + relocated); }
    }
    for (index = 0; index < 4; ++index) { field = owner + before[index]; value = r_u32(field); if (value) w_u32(field, base + value); }
    value = r_u32(owner + 4);
    if (value) { w_u32(owner + 4, base + value); level2p_sub_80085BF8(owner); }
    value = r_u32(owner + 12);
    if (value) { w_u32(owner + 12, base + value); level2p_sub_8003FA98(owner); }
    for (index = 0; index < 5; ++index) { offset = after[index]; value = r_u32(owner + offset); if (value) w_u32(owner + offset, base + value); }
}

void level2p_sub_800866F4(void)
{
    uint32 first, second, previous, descriptor, player, target;
    FUNCTION_MARKER(0x800866F4, "LEVEL2P.BIN");
    first = level2p_sub_80048038(); w_u32(0x80099418, first); second = level2p_sub_80048038();
    first = r_u32(0x80099418); previous = r_u32(0x80099434); w_u32(0x8009941C, second);
    w_u32(first + 4, second); w_u32(second + 4, previous); w_u32(0x80099434, first);
    descriptor = level2p_sub_8006F800(0); player = level2p_sub_8006EF50(1, descriptor);
    w_u32(0x8009943C, player); level2p_sub_800786F4(player);
    descriptor = level2p_sub_8006F800(1); player = level2p_sub_8006EF50(1, descriptor);
    w_u32(0x80099440, player); level2p_sub_800786F4(player);
    first = r_u32(0x80099418); target = r_u32(0x8009943C); level2p_sub_800497AC(first, target, 0);
    second = r_u32(0x8009941C); target = r_u32(0x80099440); level2p_sub_800497AC(second, target, 0);
}

void level2p_sub_800867C0(uint32 filename)
{
    uint32 selected[2], selected_view, file, base, value, count, records, index, record, previous;
    FUNCTION_MARKER(0x800867C0, "LEVEL2P.BIN");
    /* TODO: Native load-output mapping and original file/table/model spans remain unchecked */
    selected_view = moh_draft_local_address(selected, sizeof(selected)); file = moh_draft_missing_call(0x800363FC, filename, selected_view, 0);
    base = file + 0x800; value = r_u32(base + 0x544); count = r_u32(file + 0xDA4); records = file + 0xDA8;
    if (value) w_u32(base + 0x544, base + value);
    for (index = 0x54C; index <= 0x584; index += 8) { value = r_u32(base + index); if (value) w_u32(base + index, base + value); }
    level2p_sub_800860FC(base); level2p_sub_80086178(base); level2p_sub_800861EC(base);
    level2p_sub_80086294(base); level2p_sub_80086310(base); level2p_sub_80086384(base);
    record = records;
    for (index = 0; index < count; ++index, record += 0x100) level2p_sub_800864E0(record, base, base);
    previous = r_u32(0x8009B798); moh_draft_missing_call(0x800154EC, 0x8009B79C + (previous << 8), records, count << 8);
    previous = r_u32(0x8009B798); w_u32(0x8009B798, previous + count);
}

uint32 level2p_sub_80085E40(uint32 data)
{
    uint32 tim[5], view = 0, value, rectangle, pixels;
    FUNCTION_MARKER(0x80085E40, "LEVEL2P.BIN");
    value = r_u32(data);
    /* TODO: Original TIM spans are unchecked; native metadata mapping remains fail-fast */
    while (value == 0x10) {
        moh_draft_missing_call(0x8001DB80, data);
        if (!view) view = moh_draft_local_address(tim, sizeof(tim));
        moh_draft_missing_call(0x8001DB90, view);
        rectangle = r_u32(view + 12); pixels = r_u32(view + 16); data += 8;
        moh_draft_missing_call(0x8001704C, rectangle, pixels); value = r_u32(view);
        if (value & 8) {
            rectangle = r_u32(view + 4); pixels = r_u32(view + 8); moh_draft_missing_call(0x8001704C, rectangle, pixels);
            value = r_u32(data); data += value;
        }
        value = r_u32(data); data += value; value = r_u32(data);
    }
    moh_draft_missing_call(0x80016E38, 0);
    return data;
}

void level2p_sub_80085EF4(uint32 data, uint32 bank)
{
    uint32 count, index, record, offset, id;
    FUNCTION_MARKER(0x80085EF4, "LEVEL2P.BIN");
    count = r_u32(data); record = data;
    /* TODO: Original resource count, offset table and signed bank identifiers are unchecked */
    for (index = 0; index < count; ++index) {
        offset = r_u32(record + 4); record += 4; id = (uint32)(sint32)(sint16)(index + bank);
        level2p_sub_80041CC0(data + offset, id);
    }
}

uint32 level2p_sub_8006F740(uint32 data)
{
    uint32 count, record, index, size, value;
    FUNCTION_MARKER(0x8006F740, "LEVEL2P.BIN");
    count = (uint32)(sint32)r_s8(data); w_u32(0x8009A07C, count);
    record = moh_draft_missing_call(0x80036BF0, count << 3); w_u32(0x8009A080, record);
    if (!record) { w_u32(0x8009A07C, 0); return 0xFFFFFFFF; }
    data += 2; count = r_u32(0x8009A07C);
    /* TODO: Original signed counts and variable record spans are unchecked */
    for (index = 0; (sint32)index < (sint32)count; ++index, record += 8) {
        size = (uint32)(sint32)r_s8(data); data += 2;
        w_u32(record + 4, data); data += size * 20; w_u16(record + 2, size);
        value = r_u16(data); data += 4; w_u16(record, value);
    }
    return 0;
}

void level2p_sub_800860FC(uint32 scene)
{
    uint32 size, base, count, record, end, index, value;
    FUNCTION_MARKER(0x800860FC, "LEVEL2P.BIN");
    size = r_u32(scene + 0x550); base = r_u32(scene + 0x54C); count = r_u32(scene + 0x548);
    end = base + size * 20; record = r_u32(scene + 0x544);
    /* TODO: Original relocation source spans are unchecked */
    for (index = 0; index < count; ++index, record += 8) {
        value = r_u32(record + 4); w_u32(record + 4, value == 0xFFFFFFFF ? 0 : base + value);
        value = r_u32(record + 4);
        if (value >= end) { moh_draft_fail_fast("MIPS BREAK 0x3FF", 0x8008615C); return; }
    }
}

void level2p_sub_80086178(uint32 scene)
{
    uint32 size, base, count, record, end, index, value;
    FUNCTION_MARKER(0x80086178, "LEVEL2P.BIN");
    size = r_u32(scene + 0x558); base = r_u32(scene + 0x554); count = r_u32(scene + 0x550);
    end = base + size * 8; record = r_u32(scene + 0x54C);
    /* TODO: Original relocation source spans are unchecked */
    for (index = 0; index < count; ++index, record += 20) {
        value = r_u32(record + 16); w_u32(record + 16, value == 0xFFFFFFFF ? 0 : base + value);
        value = r_u32(record + 16);
        if (value >= end) { moh_draft_fail_fast("MIPS BREAK 0x3FF", 0x800861D0); return; }
    }
}

void level2p_sub_800861EC(uint32 scene)
{
    uint32 base, count, record, end, index, value;
    FUNCTION_MARKER(0x800861EC, "LEVEL2P.BIN");
    count = r_u32(scene + 0x560); base = r_u32(scene + 0x55C); end = base + count * 20; record = base;
    /* TODO: Original relocation source spans are unchecked */
    for (index = 0; index < count; ++index, record += 20) {
        value = r_u32(record + 12); w_u32(record + 12, value == 0xFFFFFFFF ? 0 : base + value);
        value = r_u32(record + 12);
        if (value >= end) { moh_draft_fail_fast("MIPS BREAK 0x3FF", 0x80086244); return; }
        value = r_u32(record + 16); w_u32(record + 16, value == 0xFFFFFFFF ? 0 : base + value);
        value = r_u32(record + 16);
        if (value >= end) { moh_draft_fail_fast("MIPS BREAK 0x3FF", 0x80086278); return; }
    }
}

void level2p_sub_80086294(uint32 scene)
{
    uint32 size, base, count, record, end, index, value;
    FUNCTION_MARKER(0x80086294, "LEVEL2P.BIN");
    size = r_u32(scene + 0x570); base = r_u32(scene + 0x56C); count = r_u32(scene + 0x568);
    end = base + size * 28; record = r_u32(scene + 0x564);
    /* TODO: Original relocation source spans are unchecked */
    for (index = 0; index < count; ++index, record += 8) {
        value = r_u32(record); w_u32(record, value == 0xFFFFFFFF ? 0 : base + value); value = r_u32(record);
        if (value >= end) { moh_draft_fail_fast("MIPS BREAK 0x3FF", 0x800862F4); return; }
    }
}

void level2p_sub_80086310(uint32 scene)
{
    uint32 size, base, count, record, end, index, value;
    FUNCTION_MARKER(0x80086310, "LEVEL2P.BIN");
    size = r_u32(scene + 0x580); base = r_u32(scene + 0x57C); count = r_u32(scene + 0x578);
    end = base + size * 8; record = r_u32(scene + 0x574);
    /* TODO: Original relocation source spans are unchecked */
    for (index = 0; index < count; ++index, record += 8) {
        value = r_u32(record); w_u32(record, value == 0xFFFFFFFF ? 0 : base + value); value = r_u32(record);
        if (value >= end) { moh_draft_fail_fast("MIPS BREAK 0x3FF", 0x80086368); return; }
    }
}

void level2p_sub_80086384(uint32 scene)
{
    uint32 size, base, count, record, end, index, value;
    FUNCTION_MARKER(0x80086384, "LEVEL2P.BIN");
    size = r_u32(scene + 0x588); base = r_u32(scene + 0x584); count = r_u32(scene + 0x580);
    end = base + size * 8; record = r_u32(scene + 0x57C);
    /* TODO: Original relocation source spans are unchecked */
    for (index = 0; index < count; ++index, record += 8) {
        value = r_u32(record); w_u32(record, value == 0xFFFFFFFF ? 0 : base + value); value = r_u32(record);
        if (value >= end) { moh_draft_fail_fast("MIPS BREAK 0x3FF", 0x800863DC); return; }
    }
}

void level2p_sub_800863F8(uint32 data)
{
    uint32 cursor = data + 4, next = data + 6, count, table, index, value;
    FUNCTION_MARKER(0x800863F8, "LEVEL2P.BIN");
    count = r_u32(data);
    if (count == 0xFFFFFFFF) { moh_draft_missing_call(0x80015B28, 0x8003D610); w_u32(0x8009DF9C, 0); return; }
    table = moh_draft_missing_call(0x80036BF0, (count + 1) * 4); w_u32(0x8009DF9C, table);
    if (!table) {
        moh_draft_missing_call(0x80015B28, 0x8003D644); moh_draft_fail_fast("MIPS BREAK 0", 0x80086464); return;
    }
    count = r_u32(data) + 1;
    /* TODO: Original live count, byte encoding and output table spans are unchecked */
    if ((sint32)count <= 0) return;
    table = r_u32(0x8009DF9C); index = 0;
    do {
        w_u32(table, cursor); table += 4; value = r_u8(cursor); index += 1;
        cursor = next + (0xFF - value) * 2; count = r_u32(data) + 1; next = cursor + 2;
    } while ((sint32)index < (sint32)count);
}

void level2p_sub_8005D944(void)
{
    uint32 mission, level, flags;
    FUNCTION_MARKER(0x8005D944, "LEVEL2P.BIN");
    moh_draft_missing_call(0x80015B28, 0x8003C370);
    mission = r_u32(0x8008FCD4); level = r_u32(0x8008FCD8);
    w_u32(0x800968E0, 0); w_u32(0x800968E4, 0); level2p_sub_8008695C(mission, level);
    w_u8(0x80096994, 1); w_u8(0x80096A4C, 1); w_u8(0x80096992, 1); w_u8(0x80096A4A, 1);
    w_u8(0x800969F0, 1); w_u8(0x80096AA8, 1); w_u8(0x800969EE, 1); w_u8(0x80096AA6, 1);
    flags = r_u32(0x80098B8C); w_u32(0x80098B8C, flags & 0xFFFFFFFB); level2p_sub_8005D850();
    flags = r_u32(0x80098B8C); w_u32(0x80098B8C, flags & 0xFFFFFEFF); level2p_sub_80068974(0);
    level2p_sub_8005D904();
}

uint32 level2p_sub_8005D9F4(void)
{
    uint32 flags, player;
    FUNCTION_MARKER(0x8005D9F4, "LEVEL2P.BIN");
    flags = r_u32(0x80098B8C);
    if (flags & 0x200) { player = r_u32(0x8009943C); level2p_sub_8005D708(player); }
    flags = r_u32(0x80098B8C);
    if (flags & 0x400) { player = r_u32(0x80099440); level2p_sub_8005D708(player); }
    return 0;
}

uint32 level2p_sub_8005DA58(uint32 player)
{
    uint32 value, previous;
    FUNCTION_MARKER(0x8005DA58, "LEVEL2P.BIN");
    value = level2p_sub_800706F4(player, 1, 1);
    if ((sint32)value > 0) {
        previous = r_u32(0x80096920);
        if ((sint32)previous <= 0) { w_u32(0x80096920, value); return 1; }
    } else if ((sint32)value < 0) {
        previous = r_u32(0x80096920);
        if ((sint32)previous >= 0) { w_u32(0x80096920, value); return 1; }
    } else w_u32(0x80096920, 0);
    return 0;
}

uint32 level2p_sub_8005DAD0(uint32 player)
{
    uint32 value, previous;
    FUNCTION_MARKER(0x8005DAD0, "LEVEL2P.BIN");
    value = level2p_sub_800706F4(player, 3, 1);
    if ((sint32)value > 0) {
        previous = r_u32(0x80096924);
        if ((sint32)previous <= 0) { w_u32(0x80096924, value); return 1; }
    } else if ((sint32)value < 0) {
        previous = r_u32(0x80096924);
        if ((sint32)previous >= 0) { w_u32(0x80096924, value); return 1; }
    } else w_u32(0x80096924, 0);
    return 0;
}

uint32 level2p_sub_800705BC(uint32 player, uint32 record, uint32 source, uint32 gated)
{
    uint32 mask, mode, value, threshold, scale, product, curve, gain, result, inverted;
    FUNCTION_MARKER(0x800705BC, "LEVEL2P.BIN");
    /* TODO: Original controller, curve and record indices are unchecked */
    if (gated) {
        mask = r_u32(record + 4);
        if (mask) {
            mode = r_u32(record + 8); value = level2p_sub_8004E288(player, mask, 0, mode == 0);
            if (!value) return 0;
        }
    }
    value = r_u8(0x8008FE50 + source + player * 4); threshold = r_u8(record + 0xE);
    if (value < threshold) {
        threshold = r_u8(record + 0xE); scale = r_u32(record + 0x14); product = (threshold - value) * scale;
        product = (uint32)((sint32)product >> 8); curve = r_u8(0x8009A19C + product);
        gain = r_u32(record + 0x20); gain = (uint32)((sint32)gain >> 8); result = (0 - curve) * gain;
    } else if (value < 0xA6) result = 0;
    else {
        threshold = r_u8(record + 0xE); scale = r_u32(record + 0x18); product = (value - threshold) * scale;
        product = (uint32)((sint32)product >> 8); gain = r_u32(record + 0x20); curve = r_u8(0x8009A19C + product);
        gain = (uint32)((sint32)gain >> 8); result = curve * gain;
    }
    inverted = r_u32(record + 0x24);
    return inverted ? 0 - result : result;
}

uint32 level2p_sub_800706F4(uint32 player, uint32 axis, uint32 gated)
{
    uint32 record, source;
    FUNCTION_MARKER(0x800706F4, "LEVEL2P.BIN");
    /* TODO: Original player and axis indices are unchecked */
    record = 0x800B5D48 + player * 0xA0 + axis * 0x28; source = r_u32(record);
    return level2p_sub_800705BC(player, record, source, gated);
}

uint32 level2p_sub_80085AD0(uint32 data)
{
    uint32 tim[5], view = 0, count = 0, value, rectangle, pixels;
    FUNCTION_MARKER(0x80085AD0, "LEVEL2P.BIN");
    value = r_u32(data);
    /* TODO: Original TIM block spans are unchecked; native metadata mapping remains fail-fast */
    while (value == 0x10) {
        moh_draft_missing_call(0x8001DB80, data);
        if (!view) view = moh_draft_local_address(tim, sizeof(tim));
        moh_draft_missing_call(0x8001DB90, view);
        rectangle = r_u32(view + 12); pixels = r_u32(view + 16); data += 8;
        moh_draft_missing_call(0x8001704C, rectangle, pixels);
        value = r_u32(view); count += 1;
        if (value & 8) {
            rectangle = r_u32(view + 4); pixels = r_u32(view + 8);
            moh_draft_missing_call(0x8001704C, rectangle, pixels);
            value = r_u32(data); data += value;
        }
        value = r_u32(data); data += value; value = r_u32(data);
    }
    return count;
}

uint32 level2p_sub_80085B88(uint32 filename)
{
    uint32 data, result;
    FUNCTION_MARKER(0x80085B88, "LEVEL2P.BIN");
    data = moh_draft_missing_call(0x800363FC, filename, 0, 0);
    if (!data) { moh_draft_missing_call(0x80015B28, 0x8003D560); return 0xFFFFFFFF; }
    result = level2p_sub_80085AD0(data);
    moh_draft_missing_call(0x80016E38, 0); moh_draft_missing_call(0x80036E44, data);
    return result;
}

void level2p_sub_8005F548(uint32 mode)
{
    uint32 area[2], area_view, rectangle_view, value, result = 0, red, green, blue;
    uint16 rectangle[4];
    FUNCTION_MARKER(0x8005F548, "LEVEL2P.BIN");
    area[0] = r_u32(0x8003C458); area[1] = r_u32(0x8003C45C);
    moh_draft_missing_call(0x80016E38, 0); level2p_sub_80089E0C(0x200, 0xF0, 5, 1, 0, 0);
    mode &= 0xFF;
    if (mode == 3) return;
    if (mode == 2) {
        result = level2p_sub_80085B88(0x8003C490);
        w_u8(0x80039D24, 0x4F); w_u8(0x80039D25, 0x4C); w_u8(0x80039D26, 0x5E);
    } else if (mode == 4) {
        value = r_u32(0x80094904);
        if (!value) {
            result = level2p_sub_80085B88(0x8003C480);
            w_u8(0x80039D24, 0xA3); w_u8(0x80039D25, 0x3F); w_u8(0x80039D26, 0x30);
        } else {
            value = r_u32(0x8008FCD4);
            if (value == 9) { value = r_u32(0x8008FCD8); value = value == 4; }
            else value = 0;
            if (value) {
                result = level2p_sub_80085B88(0x8003C460);
                w_u8(0x80039D24, 0x9B); w_u8(0x80039D25, 0x60); w_u8(0x80039D26, 0x27);
            } else {
                result = level2p_sub_80085B88(0x8003C470);
                w_u8(0x80039D24, 0xC5); w_u8(0x80039D25, 0x58); w_u8(0x80039D26, 10);
            }
        }
    }
    if ((sint32)result > 0) {
        red = r_u8(0x80039D24); green = r_u8(0x80039D25); blue = r_u8(0x80039D26);
        rectangle[0] = 0x8E; rectangle[1] = 0xCF; rectangle[2] = 0x1E; rectangle[3] = 6;
        /* TODO: Native rectangle and saved area mappings remain fail-fast */
        rectangle_view = moh_draft_local_address(rectangle, sizeof(rectangle));
        moh_draft_missing_call(0x80016FBC, rectangle_view, red, green, blue);
        area_view = moh_draft_local_address(area, sizeof(area));
        moh_draft_missing_call(0x800170AC, area_view, 0, 0xF0);
        moh_draft_missing_call(0x800170AC, area_view, 0x200, 0); moh_draft_missing_call(0x80016E38, 0);
    }
}

void level2p_sub_80089E0C(uint32 width, uint32 height, uint32 fade_in, uint32 duration, uint32 input_delay, uint32 fade_target)
{
    FUNCTION_MARKER(0x80089E0C, "LEVEL2P.BIN");
    level2p_sub_800896A8(0, 0, width, height, 0, fade_in & 0xFF, duration, input_delay, 0x80, fade_target & 0xFF, 0x800);
}

uint32 level2p_sub_800896A8(uint32 x, uint32 y, uint32 width, uint32 height, uint32 fade_in, uint32 fade_out, uint32 duration, uint32 input_delay, uint32 brightness, uint32 fade_target, uint32 input_mask)
{
    uint8 draw_environments[184], display_environments[40], sprites[2000], pages[600];
    uint32 draw, display, sprite_base, page_base, buffer, columns, rows, state = 0, ticks = 0, settled = 0, result = 0;
    uint32 initial_brightness, page, sprite, record, row, column, index, value, input, candidate, current_x;
    FUNCTION_MARKER(0x800896A8, "LEVEL2P.BIN");
    buffer = r_u32(0x80098B88); columns = (width >> 7) + 1; rows = (height >> 8) + 1;
    fade_in &= 0xFF; fade_out &= 0xFF; initial_brightness = brightness & 0xFF; fade_target &= 0xFF;
    moh_draft_missing_call(0x80016E38, 0);
    /* TODO: Native SDK buffers and packet mappings remain fail-fast; original buffer/dimension spans are unchecked */
    draw = moh_draft_local_address(draw_environments, sizeof(draw_environments));
    display = moh_draft_local_address(display_environments, sizeof(display_environments));
    sprite_base = moh_draft_local_address(sprites, sizeof(sprites)); page_base = moh_draft_local_address(pages, sizeof(pages));
    moh_draft_missing_call(0x8001BB28, draw, 0, 0, width, height);
    moh_draft_missing_call(0x8001BB28, draw + 92, 0, height == 0xF0 ? 0xF0 : 0, width, height);
    w_u8(draw + 22, 0); w_u8(draw + 114, 0); w_u8(draw + 23, 0); w_u8(draw + 115, 0); w_u8(draw + 24, 0); w_u8(draw + 116, 0);
    moh_draft_missing_call(0x8001BBE8, display, 0, height == 0xF0 ? 0xF0 : 0, width, height);
    moh_draft_missing_call(0x8001BBE8, display + 20, 0, 0, width, height);
    w_u8(display + 16, height > 0x100); w_u8(display + 36, height > 0x100);
    moh_draft_missing_call(0x80017348, draw + buffer * 92); moh_draft_missing_call(0x80017408, display + buffer * 20);
    for (index = 0; index < 2; ++index) {
        for (row = 0; (sint32)row < (sint32)rows; ++row) {
            if (!columns) continue;
            column = 0;
            do {
                record = sprite_base + index * 1000 + row * 200 + column * 20;
                moh_draft_missing_call(0x8001C748, record);
                w_u16(record + 8, column << 7); w_u16(record + 10, row << 8);
                w_u8(record + 12, 0); w_u8(record + 13, 0);
                w_u16(record + 16, column == columns - 1 ? width & 0x7F : 0x80);
                w_u16(record + 18, row == rows - 1 ? height & 0xFF : 0x100); w_u16(record + 14, 0);
                column += 1;
            } while ((sint32)column < (sint32)columns);
        }
    }
    moh_draft_missing_call(0x80016DA0, 1);
    if (!fade_in) state = 1;
    for (;;) {
        if (state == 3) {
            moh_draft_missing_call(0x80016E38, 0); moh_draft_missing_call(0x80015DB8, 0);
            moh_draft_missing_call(0x80017164, 0x80096B14 + (buffer << 12), 0x200);
            moh_draft_missing_call(0x80017164, 0x80096B14 + ((1 - buffer) << 12), 0x200);
            return result;
        }
        level2p_sub_8004E66C(); input = r_u32(0x8008FE5C);
        if (state == 1) {
            if (!duration) state = 3;
            else if (((sint32)input_delay < (sint32)ticks && (input & input_mask)) ||
                ((sint32)duration < (sint32)ticks && (sint32)duration > 0)) {
                state = 2; result = input & input_mask;
            }
        } else if (!state) {
            if (initial_brightness & 0x80) {
                candidate = (brightness & 0xFF) - fade_in;
                if ((sint32)candidate < 0x81) {
                    brightness = 0x80; settled += 1;
                    if ((sint32)settled >= 3) state = 1;
                } else brightness -= fade_in;
            } else {
                candidate = (brightness & 0xFF) + fade_in;
                if ((sint32)candidate < 0x80) brightness += fade_in;
                else {
                    brightness = 0x80; settled += 1;
                    if ((sint32)settled >= 3) state = 1;
                }
            }
        } else if (state == 2) {
            if (fade_target & 0x80) {
                candidate = (brightness & 0xFF) + fade_out;
                if ((sint32)candidate < (sint32)fade_target) brightness += fade_out;
                else { brightness = fade_target; state = 3; }
            } else {
                candidate = (brightness & 0xFF) - fade_out;
                if ((sint32)fade_target < (sint32)candidate) brightness -= fade_out;
                else { brightness = fade_target; state = 3; }
            }
        }
        moh_draft_missing_call(0x80017164, 0x80096B14 + (buffer << 12), 0x200);
        if (state != 1) {
            for (row = 0; (sint32)row < (sint32)rows; ++row) {
                if (!columns) continue;
                column = 0; current_x = x;
                do {
                    page = page_base + buffer * 300 + row * 60 + column * 12;
                    if (state - 2 < 2) value = moh_draft_missing_call(0x8001B968, 2, 1, column << 7, row << 8);
                    else value = moh_draft_missing_call(0x8001B968, 2, 1, current_x, y + (row << 8));
                    current_x += 0x80;
                    moh_draft_missing_call(0x8001B8C8, page, 0, 1, value & 0xFFFF, 0);
                    sprite = sprite_base + buffer * 1000 + row * 200 + column * 20;
                    w_u8(sprite + 4, brightness); w_u8(sprite + 5, brightness); w_u8(sprite + 6, brightness);
                    moh_draft_missing_call(0x8001DD10, 0x80096B14 + (buffer << 12), sprite);
                    moh_draft_missing_call(0x8001DD10, 0x80096B14 + (buffer << 12), page);
                    column += 1;
                } while ((sint32)column < (sint32)columns);
            }
        }
        moh_draft_missing_call(0x80016E38, 0); moh_draft_missing_call(0x80015DB8, 0);
        buffer = buffer == 0;
        moh_draft_missing_call(0x80017348, draw + buffer * 92); moh_draft_missing_call(0x80017408, display + buffer * 20);
        moh_draft_missing_call(0x800172D8, buffer ? 0x80096B14 : 0x80097B14); ticks += 1;
    }
}

void level2p_sub_8005D708(uint32 owner)
{
    uint32 state, handle, target = 0, flags;
    FUNCTION_MARKER(0x8005D708, "LEVEL2P.BIN");
    level2p_sub_8005D3B8(owner, 1);
    state = r_u32(owner + 0x388); handle = r_u32(state + 0xA0);
    /* TODO: Original handle table index and target are unchecked */
    if (handle) target = r_u32(0x80099460 + (handle & 0xFFFF) * 4);
    level2p_sub_80047EC8(target);
    flags = r_u32(0x80098B8C); flags &= 0xFFFFFFFD; flags &= 0xFFFFF9FF; w_u32(0x80098B8C, flags);
}

void level2p_sub_8005D788(void)
{
    FUNCTION_MARKER(0x8005D788, "LEVEL2P.BIN");
}

void level2p_sub_8005D790(uint32 mode)
{
    FUNCTION_MARKER(0x8005D790, "LEVEL2P.BIN");
    w_u8(0x8008FCDC, mode); mode &= 0xFF;
    switch (mode) {
    case 1: w_u32(0x80039C58, 3); level2p_sub_8005D788(); break;
    case 2: w_u32(0x80039C58, 1); break;
    case 3: w_u32(0x80039C58, 2); break;
    case 4: w_u32(0x80039C58, 0); level2p_sub_8005D788(); break;
    }
    level2p_sub_800895F8(); level2p_sub_80085F80(mode); level2p_sub_8005CC20();
}

void level2p_sub_8005D850(void)
{
    static const uint32 destinations[4] = {0x80096995, 0x80096A4D, 0x800969F1, 0x80096AA9};
    uint32 scene, enabled, index, value;
    FUNCTION_MARKER(0x8005D850, "LEVEL2P.BIN");
    scene = r_u32(0x80098FB0); enabled = r_u32(scene + 0xA0);
    if (enabled) return;
    for (index = 0; index < 4; ++index) {
        value = r_u8(scene + 0xCE); w_u8(destinations[index], value);
        value = r_u8(scene + 0xCF); w_u8(destinations[index] + 1, value);
        value = r_u8(scene + 0xD0); w_u8(destinations[index] + 2, value);
    }
}

void level2p_sub_8005D904(void)
{
    uint32 index, value;
    FUNCTION_MARKER(0x8005D904, "LEVEL2P.BIN");
    /* TODO: Original table index is unchecked */
    index = r_u32(0x80039C6C) * 4;
    value = r_u32(0x80096928 + index); w_u32(0x800968C8, value);
    value = r_u32(0x80096948 + index); w_u32(0x800968C4, value);
}

void level2p_sub_80085F80(uint32 mode)
{
    uint32 active, level, value, owner, next, callback, kind;
    FUNCTION_MARKER(0x80085F80, "LEVEL2P.BIN");
    active = r_u32(0x8009B690);
    if (!active) return;
    level = r_u32(0x8008FCD8);
    if ((sint32)level > 0) {
        value = r_u32(0x80039D88); moh_draft_missing_call(0x80015B28, 0x8003D57C, value << 4);
        moh_draft_missing_call(0x80016B60, 1); level2p_sub_8005C830(); level2p_sub_80085F78();
        moh_draft_missing_call(0x80015B28, 0x8003D594); level2p_sub_8005ABD0();
        owner = r_u32(0x80099434);
        /* TODO: Original actor list assumes valid pointers and no cycles */
        while (owner) {
            callback = r_u32(owner + 0x384); next = r_u32(owner + 4);
            if (callback) {
                kind = r_u32(owner + 8); moh_draft_missing_call(0x80015B28, 0x8003D5B0, kind);
                callback = r_u32(owner + 0x384); moh_draft_indirect_call(callback, owner);
            }
            owner = next;
        }
        value = r_u32(0x80039D88); moh_draft_missing_call(0x80015B28, 0x8003D57C, value << 4);
        level2p_sub_800613D4();
        w_u32(0x80099434, 0); w_u32(0x8009B790, 0); w_u32(0x8009B798, 0);
        moh_draft_missing_call(0x80015B28, 0x8003D5CC); level2p_sub_800890E4();
        moh_draft_missing_call(0x80015B28, 0x8003D5E8); moh_draft_missing_call(0x80036AF8);
        value = r_u32(0x80039D88); moh_draft_missing_call(0x80015B28, 0x8003D57C, value << 4);
        level2p_sub_8005F548(mode & 0xFF);
        moh_draft_missing_call(0x80015B28, 0x8003D5F8); level2p_sub_80042BC4();
    }
    w_u32(0x8009B690, 0);
}

void level2p_sub_80087A50(void)
{
    uint32 z, step, record, index, offset, x, y;
    FUNCTION_MARKER(0x80087A50, "LEVEL2P.BIN");
    z = (uint32)(sint32)r_s16(0x800B639C); step = (uint32)((sint32)(z << 5) / 200);
    record = 0x800B6348;
    for (index = 0; index < 2; ++index, record += 0x28) {
        w_u8(record + 3, 9); w_u8(record + 7, 0x2C); w_u16(record + 0x16, 15); w_u16(record + 0xE, 0x3FBF);
        w_u8(record + 0xC, 0xC0); w_u8(record + 0xD, 0x60); w_u8(record + 0x14, 0xDF); w_u8(record + 0x15, 0x60);
        w_u8(record + 0x1C, 0xC0); w_u8(record + 0x1D, 0x7F); w_u8(record + 0x24, 0xDF); w_u8(record + 0x25, 0x7F);
        w_u8(record + 4, 0x80); w_u8(record + 5, 0x80); w_u8(record + 6, 0x80);
    }
    offset = step * 0xAAB; offset = (uint32)((sint32)offset >> 12);
    x = r_u16(0x800B6398); w_u16(0x800B63A0, x + offset);
    y = r_u16(0x800B639A); z = r_u16(0x800B639C);
    w_u16(0x800B63A2, y); w_u16(0x800B63A4, z); w_u16(0x800B63A8, x);
    w_u16(0x800B63AA, y + step); w_u16(0x800B63AC, z);
}

void level2p_sub_80087B60(uint32 points, uint32 count)
{
    uint32 value, step, palette = 0x8009DFAC, colors = 0x8009E7CC, index, scene, bias, height, signed_height, square, angle, root, cosine, sine, product;
    FUNCTION_MARKER(0x80087B60, "LEVEL2P.BIN");
    value = sub_8001552C(); step = (uint32)((sint32)value >> 12) & 7; count &= 0xFFFF;
    /* TODO: Original advancing palette and point/color spans are unchecked */
    for (index = 0; index < count; ++index, points += 8, colors += 4) {
        palette += step;
        value = r_u8(palette); w_u8(colors, value); value = r_u8(palette + 1); w_u8(colors + 1, value);
        value = r_u8(palette + 2); step = (step + 1) & 7; w_u8(colors + 2, value);
        value = sub_8001552C(); scene = r_u32(0x80098FB0); bias = r_u8(scene + 0x58F);
        product = value * 0x5FFF; height = 0 - ((bias << 8) + (product >> 15)); signed_height = (uint32)(sint32)(sint16)height;
        square = (0x6FFF - signed_height) * (signed_height + 0x6FFF); w_u16(points + 2, height);
        value = sub_8001552C(); angle = (value >> 3) & 0xFFFF;
        root = moh_draft_missing_call(0x80016188, square);
        cosine = moh_draft_missing_call(0x8001C908, angle); sine = moh_draft_missing_call(0x8001C9A8, angle);
        product = root * cosine; w_u16(points, (uint32)((sint32)product >> 12));
        product = root * sine; w_u16(points + 4, (uint32)((sint32)product >> 12));
    }
}

void level2p_sub_80087CE0(void)
{
    uint32 scene, value, coordinate;
    FUNCTION_MARKER(0x80087CE0, "LEVEL2P.BIN");
    scene = r_u32(0x80098FB0); value = r_u32(scene + 0x594);
    if (value) {
        coordinate = (uint32)(sint32)r_s16(scene + 0x594); moh_draft_missing_call(0x80015B28, 0x8003D768, coordinate, coordinate);
        scene = r_u32(0x80098FB0); coordinate = (uint32)(sint32)r_s16(scene + 0x596);
        moh_draft_missing_call(0x80015B28, 0x8003D790, coordinate, coordinate);
        scene = r_u32(0x80098FB0); w_u16(0x800B6398, 0);
        coordinate = r_u16(scene + 0x594); w_u16(0x800B639A, coordinate);
        coordinate = r_u16(scene + 0x596); w_u32(0x8009DFA8, 1); w_u16(0x800B639C, coordinate);
        level2p_sub_80087A50();
    } else moh_draft_missing_call(0x80015B28, 0x8003D7B8);
    level2p_sub_80087B60(0x8009DFD4, 0xFF);
}

void level2p_sub_800894E8(uint32 channel)
{
    uint32 index, address, setting, volume;
    FUNCTION_MARKER(0x800894E8, "LEVEL2P.BIN");
    index = r_u8(0x8009F490); address = r_u32(0x8009F474 + index * 4); setting = r_u32(0x8003864C);
    moh_draft_missing_call(0x80015D38, channel, address);
    switch (setting) {
    case 1: volume = 0x800; break;
    case 2: volume = 0x1000; break;
    case 3: volume = 0x2000; break;
    case 4: volume = 0x3000; break;
    case 5: volume = 0x3800; break;
    case 6: volume = 0x3FFF; break;
    default: volume = 0; break;
    }
    moh_draft_missing_call(0x80015C08, channel, volume, volume); moh_draft_missing_call(0x800157BC, channel, 0x800);
    moh_draft_missing_call(0x8001559C, channel, 0, 0, 0, 3, 15, 1, 1, 3);
    /* Unsigned shift retains the original bit31 channel mask */
    moh_draft_missing_call(0x800158DC, 1, 1u << (channel & 31));
}

uint32 level2p_sub_800892E8(uint32 filename)
{
    uint32 buffer, value, file, size, cursor;
    FUNCTION_MARKER(0x800892E8, "LEVEL2P.BIN");
    moh_draft_missing_call(0x8001DD80, 0); moh_draft_missing_call(0x8001DD50, 1);
    buffer = r_u32(0x8009F484); w_u8(0x8009F490, 0);
    if (!buffer) w_u32(0x8009F484, 0x8009F49C);
    for (;;) {
        buffer = r_u32(0x8009F484); value = level2p_sub_8004A29C(filename, 1, 0x14, buffer);
        if (value) break;
        moh_draft_missing_call(0x80015B28, 0x8003D870); moh_draft_missing_call(0x8001D180); level2p_sub_80049DF8();
    }
    file = r_u32(0x8008FE14); buffer = r_u32(0x8009F484); size = r_u32(file + 4);
    w_u32(0x8009F488, buffer); w_u32(0x8009F48C, size); w_u32(0x8009F46C, size);
    level2p_sub_8004A484(); moh_draft_missing_call(0x8001DF40, 0);
    value = r_u32(0x8009F474); moh_draft_missing_call(0x800156DC, value);
    value = r_u32(0x8009F488); moh_draft_missing_call(0x8001575C, value, 0x5000);
    cursor = r_u32(0x8009F488); size = r_u32(0x8009F46C);
    w_u32(0x8009F488, cursor + 0x5000); w_u32(0x8009F46C, size - 0x5000); moh_draft_missing_call(0x80015C88, 1);
    value = r_u32(0x8009F478); moh_draft_missing_call(0x800156DC, value);
    cursor = r_u32(0x8009F488); w_u8(cursor + 0x4FF1, 3);
    cursor = r_u32(0x8009F488); moh_draft_missing_call(0x8001575C, cursor, 0x5000);
    size = r_u32(0x8009F46C); buffer = r_u32(0x8009F484);
    w_u32(0x8009F488, buffer); w_u32(0x8009F46C, size - 0x5000); moh_draft_missing_call(0x80015C88, 1);
    level2p_sub_8004A23C(1, 0x14); moh_draft_missing_call(0x8001DD50, 0);
    moh_draft_missing_call(0x8001C698, 0x80089268); moh_draft_missing_call(0x8001DF40, 0x80089188);
    value = r_u32(0x8009F474); moh_draft_missing_call(0x8001DEC0, 0, value);
    value = r_u32(0x8009F478); moh_draft_missing_call(0x8001DF70, value + 0x20);
    moh_draft_missing_call(0x8001DD80, 1); level2p_sub_800894E8(0);
    return 0;
}

void level2p_sub_8006FD40(void)
{
    uint32 scene, state, count, record, index, flags;
    FUNCTION_MARKER(0x8006FD40, "LEVEL2P.BIN");
    scene = r_u32(0x80099430); state = r_u32(scene + 0x388); count = r_u32(state + 0x10); record = r_u32(state + 0x14);
    /* TODO: Original cached signed count and descriptor span are unchecked */
    for (index = 0; (sint32)index < (sint32)count; ++index, record += 8) {
        flags = r_u32(record); w_u32(record, flags & 0xFFFFFFFC);
    }
}

void level2p_sub_8006F53C(void)
{
    uint32 scene, state, count, record, index = 0, data, kind;
    FUNCTION_MARKER(0x8006F53C, "LEVEL2P.BIN");
    scene = r_u32(0x80099430); state = r_u32(scene + 0x388); count = r_u32(state + 0x10); record = r_u32(state + 0x14);
    /* TODO: Original descriptor pointers and live signed count are unchecked */
    while ((sint32)index < (sint32)count) {
        data = r_u32(record + 4); kind = r_u32(data + 0xC);
        if ((sint32)kind > 0 && (sint32)kind < 7) level2p_sub_80061700(0, record);
        index += 1; count = r_u32(state + 0x10); record += 8;
    }
}

void level2p_sub_8006F1DC(void)
{
    uint32 index;
    FUNCTION_MARKER(0x8006F1DC, "LEVEL2P.BIN");
    w_u32(0x8009A070, 0); w_u32(0x8009A074, 0); w_u32(0x8009A078, 0);
    for (index = 0; index < 64; ++index) w_u32(0x8009A180 - index * 4, 0);
}

void level2p_sub_80087D9C(void)
{
    uint32 scene, first, second, third, buffer, index, record = 0x800B60F8, y, next_y;
    FUNCTION_MARKER(0x80087D9C, "LEVEL2P.BIN");
    scene = r_u32(0x80098FB0); first = r_u8(scene + 0x58C); w_u8(0x800B6338, first);
    second = r_u8(scene + 0x58E); w_u8(0x800B6339, second); third = r_u8(scene + 0x58D);
    w_u8(0x800B633B, first >> 1); w_u8(0x800B633C, second >> 1); w_u8(0x800B633A, third); w_u8(0x800B633D, third >> 1);
    for (buffer = 0; buffer < 2; ++buffer) {
        y = 210;
        for (index = 0; index < 8; ++index, record += 0x24) {
            next_y = y + 30;
            w_u16(record + 0xA, y); w_u16(record + 0x12, y); y -= 30;
            w_u8(record + 3, 8); w_u8(record + 7, 0x38); w_u16(record + 8, 0); w_u16(record + 0x10, 0x200);
            w_u16(record + 0x18, 0); w_u16(record + 0x1A, next_y); w_u16(record + 0x20, 0x200); w_u16(record + 0x22, next_y);
        }
    }
}

void level2p_sub_800890E4(void)
{
    uint32 count, index = 0, record = 0x8009EBCC, value;
    FUNCTION_MARKER(0x800890E4, "LEVEL2P.BIN");
    count = r_u32(0x8009EBC8);
    /* TODO: Original live signed count and buffer list span are unchecked */
    while ((sint32)index < (sint32)count) {
        value = r_u32(record); record += 8; moh_draft_missing_call(0x80036E44, value);
        count = r_u32(0x8009EBC8); index += 1;
    }
    w_u32(0x8009EBC8, 0);
}

void level2p_sub_800895F8(void)
{
    uint32 value;
    FUNCTION_MARKER(0x800895F8, "LEVEL2P.BIN");
    moh_draft_missing_call(0x8001DF40, 0); moh_draft_missing_call(0x8001C698, 0); moh_draft_missing_call(0x80019640, 0);
    w_u8(0x8008FE30, 0); w_u8(0x8008FE10, 0); moh_draft_missing_call(0x8001DD80, 0);
    moh_draft_missing_call(0x800158DC, 0, 1); level2p_sub_8004A484(); moh_draft_missing_call(0x8001AD98);
    moh_draft_missing_call(0x80015DB8, 0); moh_draft_missing_call(0x80015DB8, 0);
    value = r_u32(0x8009F494); moh_draft_missing_call(0x8001DF40, value);
    value = r_u32(0x8009F498); moh_draft_missing_call(0x8001C698, value);
    w_u32(0x8009F468, 0); w_u32(0x800A94C8, 0xFFFFFFFF); w_u32(0x8009F484, 0);
}

void level2p_sub_8005D490(void)
{
    uint8 filename[32];
    uint32 channel, owner, next, kind, player, flags, active, index, first, second, format, filename_view;
    FUNCTION_MARKER(0x8005D490, "LEVEL2P.BIN");
    moh_draft_missing_call(0x80015B28, 0x8003C2A0); moh_draft_missing_call(0x80015B28, 0x8003C2B4);
    for (channel = 1; channel < 24; ++channel) level2p_sub_80042714(channel);
    w_u32(0x8008D2AC, 0); w_u32(0x800A94B0, 250); w_u32(0x80099058, 0); w_u32(0x80099054, 0); w_u32(0x8009905C, 0);
    w_u32(0x8009691C, 1); w_u32(0x80096918, 0); w_u32(0x80099060, 0); w_u32(0x80094900, 0);
    moh_draft_missing_call(0x80016E38, 0); owner = r_u32(0x80099434);
    /* TODO: Original actor list assumes no cycles */
    while (owner) {
        kind = r_u32(owner + 8); next = r_u32(owner + 4);
        if (!kind || (kind > 3 && kind != 8)) level2p_sub_80066C64(owner);
        owner = next;
    }
    level2p_sub_800895F8(); moh_draft_missing_call(0x80019640, 0x80049D90);
    level2p_sub_800613D4(); level2p_sub_800890E4(); level2p_sub_80061250(); level2p_sub_80053FD8();
    level2p_sub_8006FD40(); level2p_sub_8006F53C();
    player = r_u32(0x8009943C); level2p_sub_8005D3B8(player, 0);
    owner = r_u32(0x80099418); level2p_sub_80047EC8(owner);
    level2p_sub_8006F1DC(); level2p_sub_80053B04();
    player = r_u32(0x80099440); level2p_sub_8005D3B8(player, 0);
    owner = r_u32(0x8009941C); level2p_sub_80047EC8(owner);
    player = r_u32(0x8009943C); level2p_sub_80061700(player, 0);
    player = r_u32(0x80099440); level2p_sub_80061700(player, 0);
    flags = r_u32(0x80098B8C); w_u32(0x80098B8C, flags & 0xFFFFFFFD);
    level2p_sub_8005C828(); level2p_sub_80087CE0(); level2p_sub_80087D9C(); level2p_sub_8005D45C(); level2p_sub_80053808();
    active = r_u32(0x80099060); w_u32(0x8009905C, 1);
    if (active) { moh_draft_missing_call(0x80015B28, 0x8003C324); return; }
    w_u32(0x80099060, 1); moh_draft_missing_call(0x8001C698, 0x80089268);
    index = r_u32(0x8008FCD8); index = (index - 1) * 4;
    first = r_u32(0x8009688C + index); flags = r_u32(0x80038658); second = r_u32(0x800968A8 + index);
    format = flags ? 0x8003C2E8 : 0x8003C308;
    /* TODO: Native filename mapping and original unbounded format output remain unresolved */
    filename_view = moh_draft_local_address(filename, sizeof(filename));
    moh_draft_missing_call(0x8001BE18, filename_view, format, first, second, first, second);
    level2p_sub_800892E8(filename_view);
}

void level2p_sub_8005D45C(void)
{
    uint32 index;
    FUNCTION_MARKER(0x8005D45C, "LEVEL2P.BIN");
    w_u32(0x8008FF38, 0xFFFFFFFF);
    for (index = 0; index < 100; ++index) w_u32(0x800930BC - index * 0x80, 0);
}

void level2p_sub_8005D3B8(uint32 owner, uint32 mode)
{
    uint32 state, value;
    FUNCTION_MARKER(0x8005D3B8, "LEVEL2P.BIN");
    state = r_u32(owner + 0x388);
    /* The original extra A2 carrier is unread by 5D0DC */
    level2p_sub_8005D0DC(owner, state);
    value = r_u16(state + 0x714); w_u16(state + 0x714, value + 1);
    if (mode) level2p_sub_8006F9B8(owner);
    else { level2p_sub_8006F890(owner); level2p_sub_80061700(owner, 0); }
    value = r_u32(0x800993FC); w_u32(owner + 0x3BC, value - 1);
    level2p_sub_8006591C(owner); level2p_sub_80065A20(owner); level2p_sub_8004DFBC(owner);
}

uint32 level2p_sub_8006F800(uint32 player)
{
    uint32 scene, state, count, record, index, data, kind, id;
    FUNCTION_MARKER(0x8006F800, "LEVEL2P.BIN");
    scene = r_u32(0x80099430); state = r_u32(scene + 0x388); count = r_u32(state + 0x10); record = r_u32(state + 0x14);
    /* TODO: Original signed count, descriptor pointers and identifiers are unchecked */
    for (index = 0; (sint32)index < (sint32)count; ++index, record += 8) {
        data = r_u32(record + 4); kind = r_u32(data);
        if (kind == 0x8877B646) {
            id = (uint32)(sint32)r_s16(data + 0x34);
            if (id == player) return record;
        }
    }
    level2p_sub_800885B8(4, 0x38);
    return 0;
}

uint32 level2p_sub_8006F890(uint32 owner)
{
    uint32 state, port, record, data, value, x, y, z, pitch, yaw, roll, delta, previous;
    FUNCTION_MARKER(0x8006F890, "LEVEL2P.BIN");
    state = r_u32(owner + 0x388); port = r_u8(state + 0x726); record = level2p_sub_8006F800(port);
    /* TODO: Original descriptor result is dereferenced without a null check */
    data = r_u32(record + 4); value = (uint32)(sint32)r_s16(data + 0x10); w_u32(owner + 0x20C, value << 16);
    data = r_u32(record + 4); value = (uint32)(sint32)r_s16(data + 0x12); w_u32(owner + 0x210, value << 16);
    data = r_u32(record + 4); value = (uint32)(sint32)r_s16(data + 0x14); w_u32(owner + 0x214, value << 16);
    data = r_u32(record + 4); state = r_u32(owner + 0x388); value = (uint32)(sint32)r_s16(data + 0x18);
    y = r_u32(owner + 0x210); w_u32(owner + 0x228, value << 16);
    data = r_u32(record + 4); z = r_u32(owner + 0x214); value = (uint32)(sint32)r_s16(data + 0x1A);
    pitch = r_u32(owner + 0x228); w_u32(owner + 0x22C, value << 16);
    data = r_u32(record + 4); x = r_u32(owner + 0x20C); yaw = r_u32(owner + 0x22C);
    roll = (uint32)(sint32)r_s16(data + 0x1C);
    w_u32(owner + 0x258, x); w_u32(owner + 0x25C, y); w_u32(owner + 0x260, z);
    w_u32(owner + 0x264, pitch); w_u32(owner + 0x268, yaw);
    roll <<= 16; w_u32(owner + 0x230, roll); w_u32(owner + 0x26C, roll); w_u32(state + 0x7F4, pitch);
    value = r_u32(owner + 0x22C); w_u32(state + 0x800, value);
    value = r_u32(owner + 0x228); delta = r_u32(owner + 0x234); previous = r_u32(state + 0x7F4);
    w_u32(state + 0x7FC, value + delta - previous);
    value = r_u32(owner + 0x22C); delta = r_u32(owner + 0x238); previous = r_u32(state + 0x800);
    w_u32(state + 0x808, value + delta - previous);
    return 1;
}

uint32 level2p_sub_8006F9B8(uint32 owner)
{
    uint32 point[3];
    uint32 point_view = 0, scene, state, other, table, count, record, index, data, kind, flags, value;
    uint32 candidates = 0, safe = 0, selected, previous, result, height, delta;
    FUNCTION_MARKER(0x8006F9B8, "LEVEL2P.BIN");
    scene = r_u32(0x80099430); state = r_u32(scene + 0x388); other = r_u32(0x8009943C);
    table = r_u32(state + 0x14); count = r_u32(state + 0x10);
    if (owner == other) other = r_u32(0x80099440);
    /* TODO: Original table, other-player pointer and signed count are unchecked */
    record = table;
    for (index = 0; (sint32)index < (sint32)count; ++index, record += 8) {
        flags = r_u32(record);
        if (flags & 3) continue;
        data = r_u32(record + 4); kind = r_u32(data);
        if (kind != 0xFD092133) continue;
        /* TODO: Native local point mapping remains a fail-fast boundary */
        if (!point_view) point_view = moh_draft_local_address(point, sizeof(point));
        value = (uint32)(sint32)r_s16(data + 0x10); w_u32(point_view, value << 16);
        data = r_u32(record + 4); value = (uint32)(sint32)r_s16(data + 0x12); candidates += 1; w_u32(point_view + 4, value << 16);
        data = r_u32(record + 4); value = (uint32)(sint32)r_s16(data + 0x14); w_u32(point_view + 8, value << 16);
        result = level2p_sub_8006883C(point_view, owner + 0x20C);
        if ((sint32)result > 0x7CFFFFF) {
            result = level2p_sub_8006883C(point_view, other + 0x20C);
            if ((sint32)result > 0x7CFFFFF) safe += 1;
        }
    }
    if (!candidates) return level2p_sub_8006F890(owner);
    value = sub_8001552C(); value *= safe; selected = (uint32)((sint32)value >> 15); record = table;
    for (index = 0; (sint32)index < (sint32)count; ++index, record += 8) {
        flags = r_u32(record);
        if (flags & 3) continue;
        data = r_u32(record + 4); kind = r_u32(data);
        if (kind != 0xFD092133) continue;
        value = (uint32)(sint32)r_s16(data + 0x10); w_u32(point_view, value << 16);
        data = r_u32(record + 4); value = (uint32)(sint32)r_s16(data + 0x12); w_u32(point_view + 4, value << 16);
        data = r_u32(record + 4); value = (uint32)(sint32)r_s16(data + 0x14); w_u32(point_view + 8, value << 16);
        result = level2p_sub_8006883C(point_view, owner + 0x20C);
        if ((sint32)result <= 0x7CFFFFF && safe) continue;
        result = level2p_sub_8006883C(point_view, other + 0x20C);
        if ((sint32)result <= 0x7CFFFFF && safe) continue;
        previous = selected; selected -= 1;
        if (previous) continue;
        data = r_u32(record + 4); value = (uint32)(sint32)r_s16(data + 0x10); w_u32(owner + 0x20C, value << 16);
        data = r_u32(record + 4); value = (uint32)(sint32)r_s16(data + 0x12); w_u32(owner + 0x210, value << 16);
        data = r_u32(record + 4); value = (uint32)(sint32)r_s16(data + 0x14); w_u32(owner + 0x214, value << 16);
        data = r_u32(record + 4); value = (uint32)(sint32)r_s16(data + 0x18); w_u32(owner + 0x228, value << 16);
        data = r_u32(record + 4); value = (uint32)(sint32)r_s16(data + 0x1A); w_u32(owner + 0x22C, value << 16);
        data = r_u32(record + 4); value = (uint32)(sint32)r_s16(data + 0x1C); w_u32(owner + 0x230, value << 16);
        height = level2p_sub_8003F140(owner + 0x20C, 0, 1, 0, 0, 1);
        value = (uint32)(sint32)(sint16)height;
        if ((sint32)value != -0x7FFF) { data = r_u32(record + 4); w_u16(data + 0x12, height); w_u32(owner + 0x210, value << 16); }
        state = r_u32(owner + 0x388); value = r_u32(owner + 0x228); w_u32(state + 0x7F4, value);
        value = r_u32(owner + 0x22C); w_u32(state + 0x800, value);
        value = r_u32(owner + 0x228); delta = r_u32(owner + 0x234); previous = r_u32(state + 0x7F4); w_u32(state + 0x7FC, value + delta - previous);
        value = r_u32(owner + 0x22C); delta = r_u32(owner + 0x238); previous = r_u32(state + 0x800); w_u32(state + 0x808, value + delta - previous);
        return 1;
    }
    return 0;
}

void level2p_sub_8006FD98(uint32 player, uint32 button, uint32 mask, uint32 mode)
{
    uint32 offset;
    FUNCTION_MARKER(0x8006FD98, "LEVEL2P.BIN");
    /* TODO: Original player and button indices are unchecked */
    offset = (player * 9 + button) * 4;
    w_u32(0x800B6028 + offset, mask); w_u32(0x800B6078 + offset, mode);
}

void level2p_sub_800703AC(uint32 player, uint32 axis, uint32 source, uint32 parameter4, uint32 parameter5, uint32 sensitivity, uint32 inverted)
{
    uint32 record, input, value;
    FUNCTION_MARKER(0x800703AC, "LEVEL2P.BIN");
    /* TODO: Original player, axis and source indices are unchecked */
    record = 0x800B5D48 + player * 0xA0 + axis * 0x28;
    w_u32(record, source); w_u32(record + 4, parameter4); w_u32(record + 8, parameter5);
    input = 0x800B5D08 + player * 32 + source * 8;
    value = r_u8(input); w_u8(record + 0xC, value);
    value = r_u8(input + 1); w_u8(record + 0xD, value);
    value = r_u8(input + 2); w_u8(record + 0xE, value);
    value = r_u8(input + 3); w_u8(record + 0xF, value);
    value = r_u8(input + 4); w_u32(record + 0x24, inverted); w_u8(record + 0x10, value);
    level2p_sub_8007045C(player, axis, sensitivity, 0);
}

void level2p_sub_8006FF1C(uint32 player, uint32 axis, uint32 positive, uint32 negative, uint32 parameter5, uint32 parameter6, uint32 sensitivity, uint32 inverted, uint32 reset)
{
    uint32 record;
    FUNCTION_MARKER(0x8006FF1C, "LEVEL2P.BIN");
    /* TODO: Original player and axis indices are unchecked */
    record = 0x800B5E88 + player * 0xD0 + axis * 0x34;
    w_u32(record + 4, negative); w_u32(record, positive); w_u32(record + 8, parameter5);
    w_u32(record + 0xC, parameter6); w_u32(record + 0x20, inverted);
    level2p_sub_8006FFC0(player, axis, sensitivity, 0);
    if (reset) { w_u32(record + 0x28, 0x200000); w_u32(record + 0x2C, 0); w_u32(record + 0x30, 2); }
}

void level2p_sub_800700B0(uint32 player, uint32 axis, uint32 gain)
{
    uint32 record;
    FUNCTION_MARKER(0x800700B0, "LEVEL2P.BIN");
    record = 0x800B5E88 + player * 0xD0 + axis * 0x34;
    w_u32(record + 0x24, 0); w_u32(record + 0x28, gain); w_u32(record + 0x2C, 0);
}

void level2p_sub_800707D4(uint32 player, uint32 mode)
{
    FUNCTION_MARKER(0x800707D4, "LEVEL2P.BIN");
    w_u32(0x8009A190 + player * 4, mode);
}

uint32 level2p_sub_80070980(uint32 player, uint32 layout, uint32 inverted)
{
    static const uint32 buttons[6][9] = {
        {2, 4, 0x40, 1, 8, 8, 0x800, 0x400, 0x200},
        {0x40, 0x10, 0x20, 1, 0x80, 0x80, 0x800, 0x200, 2},
        {2, 4, 0x40, 0x400, 8, 8, 0x800, 0x80, 1},
        {2, 0x40, 0x20, 4, 8, 8, 0x800, 0x400, 1},
        {0x80, 0x20, 0x10, 1, 0x40, 0x40, 0x800, 0x400, 2},
        {0x40, 0x10, 0x20, 1, 0x80, 0x80, 0x800, 3, 2}
    };
    static const uint32 sources[6][4] = {
        {3, 2, 1, 0}, {3, 0, 1, 2}, {3, 0, 1, 2}, {3, 2, 1, 0}, {3, 2, 1, 0}, {3, 2, 1, 0}
    };
    static const uint32 button_axes[6][4][4] = {
        {{0x1000, 0x4000, 0x10, 0}, {0x8000, 0x2000, 0x10, 0}, {0x1000, 0x4000, 0x10, 1}, {0x8000, 0x2000, 0x10, 1}},
        {{0x1000, 0x4000, 0x10, 0}, {4, 8, 0, 0}, {0x1000, 0x4000, 0x10, 1}, {0x8000, 0x2000, 0, 0}},
        {{0x1000, 0x4000, 0x20, 0}, {0x8000, 0x2000, 0x20, 1}, {0x1000, 0x4000, 0x20, 1}, {0x8000, 0x2000, 0x20, 0}},
        {{0x1000, 0x4000, 0x10, 0}, {0x8000, 0x2000, 0x10, 0}, {0x1000, 0x4000, 0x10, 1}, {0x8000, 0x2000, 0x10, 1}},
        {{0x1000, 0x4000, 0x40, 0}, {0x8000, 0x2000, 0, 0}, {0x1000, 0x4000, 0x40, 1}, {4, 8, 0, 0}},
        {{0x1000, 0x4000, 2, 0}, {4, 8, 0, 1}, {0x1000, 0x4000, 2, 1}, {0x8000, 0x2000, 0, 1}}
    };
    uint32 index, value, sensitivity, source_sensitivity, button_sensitivity, axis_inverted;
    FUNCTION_MARKER(0x80070980, "LEVEL2P.BIN");
    if (layout == 0xFFFFFFFF) {
        value = (uint32)(sint32)r_s8(0x8008FE58 + player); layout = value ? 3 : 2; inverted = 0;
    }
    if (layout > 5) return 0;
    /* TODO: Original player index and controller records are unchecked */
    for (index = 0; index < 9; ++index) level2p_sub_8006FD98(player, index, buttons[layout][index], layout == 5 && index == 3 ? 2 : 0);
    sensitivity = r_u32(0x8009A198);
    level2p_sub_800703AC(player, 0, sources[layout][0], 0, 0, sensitivity, 0);
    sensitivity = r_u32(0x8009A198);
    level2p_sub_800703AC(player, 1, sources[layout][1], 0, 0, sensitivity, 0);
    source_sensitivity = layout == 5 ? 0x680000 : 0x3C0000;
    level2p_sub_800703AC(player, 2, sources[layout][2], 0, 0, source_sensitivity, inverted);
    source_sensitivity = layout == 5 ? 0x280000 : 0x3C0000;
    level2p_sub_800703AC(player, 3, sources[layout][3], 0, 0, source_sensitivity, 0);
    for (index = 0; index < 4; ++index) {
        button_sensitivity = index < 2 ? r_u32(0x8009A198) : 0x3C0000;
        axis_inverted = index == 2 ? inverted : 0;
        level2p_sub_8006FF1C(player, index, button_axes[layout][index][0], button_axes[layout][index][1], button_axes[layout][index][2], button_axes[layout][index][3], button_sensitivity, axis_inverted, 1);
    }
    level2p_sub_800700B0(player, 0, 0x200000); level2p_sub_800700B0(player, 1, 0x200000);
    level2p_sub_800700B0(player, 2, 0x20000); level2p_sub_800700B0(player, 3, 0x20000);
    level2p_sub_800707D4(player, layout == 5 ? 2 : 1);
    w_u32(0x8009A188 + player * 4, layout != 0 && layout != 5);
    return 0;
}
