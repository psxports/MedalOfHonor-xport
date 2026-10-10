#include "game_shell_signatures.h"
#include "xport_trace.h"
#include "psx.h"
#include <stdio.h>
#include <stdlib.h>

void level_sub_8004A634(void)
{
    uint32 mission, level, table, index;
    FUNCTION_MARKER(0x8004A634u, "LEVEL.BIN");
    moh_draft_missing_call(0x8001AF68u);
    w_u8(0x800939FCu, 0u);
    w_u8(0x80093A02u, 1u);
    mission = r_u32(0x80038660u);
    w_u8(0x80093A04u, 3u);
    w_u8(0x80093A05u, 0u);
    level = r_u32(0x80038664u);
    table = mission * 152u + level * 36u;
    w_u32(0x800939F4u, mission + 1u);
    w_u32(0x800939F8u, level + 1u);
    w_u8(0x800939FDu, r_u8(0x8003864Cu));
    w_u8(0x800939FEu, (uint8)(r_u32(0x80038654u) * 42u));
    w_u8(0x800939FFu, r_u8(0x800386DCu));
    w_u8(0x80093A00u, r_u8(0x80038668u));
    w_u8(0x80093A01u, r_u8(0x800386DEu));
    w_u8(0x80093A03u, r_u32(0x80038650u) == 0u);
    w_u8(0x80093A06u, r_u8(0x800386ACu));
    for (index = 0u; index < 13u; ++index) {
        w_u16(0x80093646u + index * 68u, r_u16(0x80038758u + table + index * 2u));
        w_u16(0x80038D5Cu + index * 4u, 0xFFFFu);
        w_u16(0x80038D5Eu + index * 4u, 0u);
    }
    w_u8(0x80038D4Du, 0u);
    w_u8(0x80038D4Cu, 0u);
    w_u32(0x80038D58u, 0u);
    w_u32(0x80038D54u, 0u);
    moh_draft_missing_call(0x80015B28u, 0x8003A18Cu);
    moh_draft_missing_call(0x80015B28u, 0x8003A1B8u, (uint32)r_u8(0x800939FCu));
    moh_draft_missing_call(0x80015B28u, 0x8003A1C4u, r_u32(0x800939F4u));
    moh_draft_missing_call(0x80015B28u, 0x8003A1D0u, r_u32(0x800939F8u));
    moh_draft_missing_call(0x80015B28u, 0x8003A1DCu, (uint32)r_u8(0x800939FDu));
    moh_draft_missing_call(0x80015B28u, 0x8003A1E8u, (uint32)r_u8(0x800939FEu));
    moh_draft_missing_call(0x80015B28u, 0x8003A1F4u, (uint32)r_u8(0x80093A03u));
    moh_draft_missing_call(0x80015B28u, 0x8003A200u, (uint32)r_u8(0x800939FFu));
    moh_draft_missing_call(0x80015B28u, 0x8003A20Cu, (uint32)r_u8(0x80093A00u));
    moh_draft_missing_call(0x80015B28u, 0x8003A218u, (uint32)r_u8(0x80093A02u));
    moh_draft_missing_call(0x80015B28u, 0x8003A224u, (uint32)r_u8(0x80093A01u));
}

void level_sub_8008A554(void)
{
    uint32 index;
    FUNCTION_MARKER(0x8008A554u, "LEVEL.BIN");
    for (index = 0u; index < 275u; ++index) w_u32(0x800A011Cu - index * 8u, 0u);
    w_u32(0x8009F888u, 0u);
}

void level_sub_8004DDAC(void)
{
    uint32 index;
    FUNCTION_MARKER(0x8004DDACu, "LEVEL.BIN");
    moh_draft_missing_call(0x8001B228u, 0x80093BA0u, 0x80093BC4u);
    moh_draft_missing_call(0x8001B4A8u);
    w_u32(0x80093B84u, 0u);
    w_u32(0x80093B88u, 0u);
    for (index = 0u; index < 10u; ++index) {
        w_u32(0x80093C04u + index * 4u, 0u);
        w_u32(0x80093C2Cu + index * 4u, 0u);
    }
}

void level_sub_80061590(void)
{
    FUNCTION_MARKER(0x80061590u, "LEVEL.BIN");
}

void level_sub_80061568(void)
{
    FUNCTION_MARKER(0x80061568u, "LEVEL.BIN");
    moh_draft_missing_call(0x80016B60u, 3u);
    moh_draft_missing_call(0x80016CD4u, 0u);
}

void level_sub_80049410(void)
{
    FUNCTION_MARKER(0x80049410u, "LEVEL.BIN");
    moh_draft_missing_call(0x80019E20u);
    moh_draft_missing_call(0x80019640u, 0x800493A8u);
}

void level_sub_80041260(void)
{
    uint32 index;
    FUNCTION_MARKER(0x80041260u, "LEVEL.BIN");
    moh_draft_missing_call(0x8001582Cu);
    moh_draft_missing_call(0x80015B68u, 0x3FFFu, 0x3FFFu);
    moh_draft_missing_call(0x8001586Cu, 1u);
    moh_draft_missing_call(0x80015BE8u, 0x7FFFu, 0x7FFFu);
    moh_draft_missing_call(0x80042790u);
    for (index = 0u; index < 8u; ++index) w_u32(0x80091144u - index * 4u, 0u);
    w_u32(0x800A0130u, 0x1010u);
    w_u32(0x80090FC8u, 0u);
    moh_draft_missing_call(0x8008A6A8u);
}

void level_sub_8005F260(void)
{
    uint32 index;
    FUNCTION_MARKER(0x8005F260u, "LEVEL.BIN");
    for (index = 0u; index < 50u; ++index) w_u32(0x8009A56Cu - index * 32u, 0u);
    w_u32(0x80099ED0u, 0u);
}

void level_sub_8008EDDC(uint32 index)
{
    uint32 offset = index * 0x2814u;
    FUNCTION_MARKER(0x8008EDDCu, "LEVEL.BIN");
    w_u32(0x800BA314u + offset, 0u);
    w_u32(0x800BA318u + offset, 1u);
}

void level_sub_8008EC14(uint32 index)
{
    uint32 offset = index * 0x1308u;
    FUNCTION_MARKER(0x8008EC14u, "LEVEL.BIN");
    w_u32(0x800C5678u + offset, 0u);
    w_u32(0x800C567Cu + offset, 1u);
}

void level_sub_80042790(void)
{
    uint32 index;
    FUNCTION_MARKER(0x80042790u, "LEVEL.BIN");
    for (index = 0u; index < 7u; ++index) {
        w_u32(0x8009112Cu + index * 4u, 0u);
        w_u16(0x8009114Au + index * 2u, 0xFFFFu);
    }
}

void level_sub_8008A6A8(void)
{
    uint32 allocation;
    FUNCTION_MARKER(0x8008A6A8u, "LEVEL.BIN");
    allocation = r_u32(0x800A0130u);
    w_u32(0x800A0144u, 0u);
    w_u32(0x800A0134u, allocation);
    w_u32(0x800A0138u, allocation + 0x5000u);
    w_u32(0x800A0130u, allocation + 0xA000u);
}


void level_sub_800501B8(uint32 display)
{
    FUNCTION_MARKER(0x800501B8u, "LEVEL.BIN");
    w_u32(display + 0x2208u, 540u);
    w_u32(display + 0x220Cu, 240u);
}

void level_sub_800501CC(uint32 display, sint32 x, sint32 y)
{
    uint8 enabled;
    FUNCTION_MARKER(0x800501CCu, "LEVEL.BIN");
    SetDefDrawEnv((DRAWENV *)moh_draft_guest_pointer(display + 16u, sizeof(DRAWENV)),
        0, 0, (sint32)r_u32(display), (sint32)r_u32(display + 4u));
    SetDefDrawEnv((DRAWENV *)moh_draft_guest_pointer(display + 200u, sizeof(DRAWENV)),
        x, y, (sint32)r_u32(display), (sint32)r_u32(display + 4u));
    w_u8(display + 38u, 0u);
    w_u8(display + 222u, 0u);
    enabled = r_u32(0x80097FD4u) == 0u;
    w_u8(display + 39u, enabled);
    w_u8(display + 223u, enabled);
    SetDefDispEnv((DISPENV *)moh_draft_guest_pointer(display + 384u, sizeof(DISPENV)),
        x, y, (sint32)r_u32(display), (sint32)r_u32(display + 4u));
    SetDefDispEnv((DISPENV *)moh_draft_guest_pointer(display + 404u, sizeof(DISPENV)),
        0, 0, (sint32)r_u32(display), (sint32)r_u32(display + 4u));
    DrawSync(0);
    SetDispMask(1);
}


void level_sub_8006240C(void)
{
    FUNCTION_MARKER(0x8006240Cu, "LEVEL.BIN");
    moh_draft_missing_call(0x80015B28u, 0x8003C514u);
    w_u32(0x8009A5D8u, 0u);
    w_u32(0x8009A5DCu, 0u);
    moh_draft_missing_call(0x80087308u, r_u32(0x800939F4u), r_u32(0x800939F8u));
    w_u8(0x8009A648u, 1u);
    w_u8(0x8009A700u, 1u);
    w_u8(0x8009A646u, 1u);
    w_u8(0x8009A6FEu, 1u);
    w_u8(0x8009A6A4u, 1u);
    w_u8(0x8009A75Cu, 1u);
    w_u8(0x8009A6A2u, 1u);
    w_u8(0x8009A75Au, 1u);
    w_u32(0x8009C840u, r_u32(0x8009C840u) & 0xFFFFFFFBu);
    moh_draft_missing_call(0x80062358u);
    w_u32(0x8009C840u, r_u32(0x8009C840u) & 0xFFFFFEFFu);
}

void level_sub_80061E14(void)
{
    uint32 channel, node, next, type, mission, level, path, format;
    int length;
    FUNCTION_MARKER(0x80061E14u, "LEVEL.BIN");
    moh_draft_missing_call(0x80015B28u, 0x8003C3D8u);
    moh_draft_missing_call(0x80015B28u, 0x8003C3ECu);
    for (channel = 1u; channel < 24u; ++channel)
        moh_draft_missing_call(0x800425C4u, channel);
    w_u32(0x80090FC8u, 0u);
    w_u32(0x800AA168u, 250u);
    w_u32(0x8009CC6Cu, 0u);
    w_u32(0x8009CC68u, 0u);
    w_u32(0x8009CC70u, 0u);
    w_u32(0x8009A60Cu, 1u);
    w_u32(0x8009A608u, 0u);
    w_u32(0x8009CC74u, 0u);
    w_u32(0x80098620u, 0u);
    DrawSync(0);
    for (node = r_u32(0x8009D64Cu); node; node = next) {
        type = r_u32(node + 8u);
        next = r_u32(node + 4u);
        if (!type || (type >= 4u && type != 8u))
            moh_draft_missing_call(0x8006B8F0u, node);
    }
    moh_draft_missing_call(0x8008AB48u);
    moh_draft_missing_call(0x80019640u, 0x800493A8u);
    moh_draft_missing_call(0x80066290u);
    moh_draft_missing_call(0x8008A634u);
    moh_draft_missing_call(0x8008EA70u);
    moh_draft_missing_call(0x80061D08u, r_u32(0x8009D654u), 0u);
    moh_draft_missing_call(0x8004770Cu, r_u32(0x8009D638u));
    moh_draft_missing_call(0x80074EA8u);
    moh_draft_missing_call(0x800540ACu);
    moh_draft_missing_call(0x80066108u);
    moh_draft_missing_call(0x80054580u);
    moh_draft_missing_call(0x800665BCu, r_u32(0x8009D654u), 0u);
    moh_draft_missing_call(0x80075684u);
    moh_draft_missing_call(0x80075208u);
    w_u32(0x8009C840u, r_u32(0x8009C840u) & 0xFFFFFFFDu);
    moh_draft_missing_call(0x80061158u);
    if (r_u32(0x800939F4u) == 9u && r_u32(0x800939F8u) == 4u)
        moh_draft_missing_call(0x80041FC4u, 0u, 81u, r_u32(0x8009D654u),
            0x10000u, 0u, 0x10000u, 0u, 0u, 0u, 0u, 0u);
    moh_draft_missing_call(0x80052C18u);
    moh_draft_missing_call(0x800888C4u);
    moh_draft_missing_call(0x80088980u);
    moh_draft_missing_call(0x80061DE0u);
    moh_draft_missing_call(0x80053DB0u);
    if (moh_native_music_enabled() && r_u32(0x8009A608u)) {
        w_u32(0x8009CC70u, 1u);
        if (!r_u32(0x8009CC74u)) {
            w_u32(0x8009CC74u, 1u);
            moh_draft_missing_call(0x8001C698u, 0x8008A7B8u);
            format = r_u32(0x80038658u) ? 0x8003C420u : 0x8003C440u;
            mission = r_u32(0x800939F4u);
            level = r_u32(0x800939F8u);
            path = sub_80036BF0(32u);
            if (!path) abort();
            length = snprintf((char *)moh_draft_guest_pointer(path, 32u), 32u,
                (const char *)moh_draft_guest_pointer(format, 1u),
                (int)mission, (int)level, (int)mission, (int)level);
            /* TODO: Verify the original 32-byte path capacity for all mission identifiers */
            if (length < 0 || length >= 32) abort();
            /* TODO: Confirm the filename is consumed synchronously by the music loader */
            moh_draft_missing_call(0x8008A838u, path);
            sub_80036E44(path);
        } else moh_draft_missing_call(0x80015B28u, 0x8003C45Cu);
    }
    w_u32(0x8009A60Cu, 0u);
    moh_draft_missing_call(0x8004F748u, 1u, 0u, 15u, 0u, 255u, 255u, 255u, 0u, 0u, 0u, 0u);
}
