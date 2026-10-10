#include "game_draft_signatures.h"
#include "game_shell_signatures.h"
#include "xport_trace.h"
#include "psx.h"

/* Native PsyQ adapters for the shell frame submission */
uint32 moh_native_draw_sync(uint32 mode)
{
    return (uint32)DrawSync((sint32)mode);
}

uint32 moh_native_put_draw_env(uint32 guest_environment)
{
    DRAWENV *environment = (DRAWENV *)moh_draft_guest_pointer(guest_environment, sizeof(DRAWENV));
    PutDrawEnv(environment);
    return guest_environment;
}

uint32 moh_native_put_disp_env(uint32 guest_environment)
{
    DISPENV *environment = (DISPENV *)moh_draft_guest_pointer(guest_environment, sizeof(DISPENV));
    PutDispEnv(environment);
    return guest_environment;
}

uint32 moh_native_draw_otag(uint32 guest_ordering_table)
{
    /* RAM pointers retain guest addresses in the linked packet tags */
    DrawOTag((uint32 *)moh_draft_guest_pointer(guest_ordering_table, sizeof(uint32)));
    /* TODO Original queue result is unspecified to the shell caller */
    return 0u;
}

uint32 shell_sub_8004B894(uint32 screen)
{
    uint32 animation, resource;
    FUNCTION_MARKER(0x8004B894u, "SHELL.BIN");
    if (!screen) return (uint32)-2;
    animation = r_u32(screen + 52u);
    if (!animation) return 2u;
    do
    {
        resource = r_u32(animation + 100u);
        if (resource)
        {
            if (shell_sub_8003D890(resource) < 0) return (uint32)-4;
            w_u32(animation + 100u, 0u);
        }
        resource = r_u32(animation + 104u);
        if (resource)
        {
            if (shell_sub_8003D890(resource) < 0) return (uint32)-4;
            w_u32(animation + 104u, 0u);
        }
        resource = r_u32(animation + 92u);
        if (resource)
        {
            if ((sint32)moh_draft_missing_call(0x80053358u, resource) < 0) return (uint32)-4;
            w_u32(animation + 92u, 0u);
        }
        resource = r_u32(animation + 96u);
        if (resource)
        {
            if ((sint32)moh_draft_missing_call(0x80053358u, resource) < 0) return (uint32)-4;
            w_u32(animation + 96u, 0u);
        }
        animation = r_u32(animation + 136u);
    } while (animation);
    return 0u;
}

static void brief_input_sound(uint32 data, uint32 sample)
{
    uint32 bank = r_u32(data), voice;
    if (!bank)
    {
        w_u32(0x8007C218u, (uint32)-2);
        return;
    }
    voice = shell_sub_8006349C();
    w_u32(0x8007C374u, voice);
    w_u32(r_u32(bank + 12u) + sample * 4u, voice);
    shell_sub_80063510((uint32)(sint32)(sint16)r_u16(bank), (sint32)sample, voice,
        r_u8(bank + 16u), r_u8(bank + 17u), r_u32(bank + 4u), r_u32(bank + 8u));
}

void shell_sub_8003FA7C(uint32 screen)
{
    FUNCTION_MARKER(0x8003FA7Cu, "SHELL.BIN");
    shell_sub_8004E1B8(r_u32(screen + 80u), 0u);
}

uint32 shell_sub_8003FAA0(uint32 screen)
{
    uint32 data = r_u32(screen + 80u);
    FUNCTION_MARKER(0x8003FAA0u, "SHELL.BIN");
    if (!r_u8(0x8007BA88u)) return 0u;
    if (shell_sub_8005F258(6u) & 255u)
    {
        if (r_u32(r_u32(screen + 28u) + 176u))
        {
            brief_input_sound(data, 5u);
            return 5u;
        }
        brief_input_sound(data, 7u);
        return 6u;
    }
    if (shell_sub_8005F258(12u) & 255u)
    {
        brief_input_sound(data, 6u);
        return 2u;
    }
    if (shell_sub_8005F258(7u) & 255u)
    {
        brief_input_sound(data, 5u);
        return 3u;
    }
    if ((shell_sub_8005F258(2u) & 255u) || (shell_sub_8005F258(14u) & 255u)) return 5u;
    if ((shell_sub_8005F258(3u) & 255u) || (shell_sub_8005F258(15u) & 255u)) return 4u;
    return 0u;
}

void shell_sub_8004CDEC(uint32 group)
{
    uint32 image;
    FUNCTION_MARKER(0x8004CDECu, "SHELL.BIN");
    if (!group || group != r_u32(r_u32(group + 184u) + 28u)) return;
    image = r_u32(group + 112u);
    if (image) shell_sub_8004CFB4(image, 1u);
    for (image = r_u32(group + 120u); image; image = r_u32(image + 208u))
        shell_sub_8004CFB4(image, 1u);
}

uint32 shell_sub_80063070(uint32 bank)
{
    sint32 identifier;
    FUNCTION_MARKER(0x80063070u, "SHELL.BIN");
    if (!bank) return (uint32)-2;
    identifier = (sint16)r_u16(bank);
    w_u8(0x8007C21Cu + (uint32)identifier, 1u);
    moh_draft_missing_call(0x80020418u, (uint32)identifier);
    shell_sub_80045328(bank);
    return 0u;
}

void shell_sub_800630CC(void)
{
    uint32 index, allocation;
    FUNCTION_MARKER(0x800630CCu, "SHELL.BIN");
    for (index = 0u; index < 16u; ++index)
    {
        allocation = r_u32(0x8007C2C4u + index * 4u);
        if (allocation) shell_sub_80045328(allocation);
    }
}

void shell_sub_80063124(void)
{
    FUNCTION_MARKER(0x80063124u, "SHELL.BIN");
    moh_draft_missing_call(0x800201F8u);
    moh_draft_missing_call(0x8001FC78u, 0u);
    moh_draft_missing_call(0x8001F70Cu, 0u, 0u);
    moh_draft_missing_call(0x800156DCu, 0x66FFFu);
    moh_draft_missing_call(0x80020268u, 0x19000u);
    moh_draft_missing_call(0x80015DB8u, 10u);
    moh_draft_missing_call(0x800204C8u);
    moh_draft_missing_call(0x8001D160u);
    /* Close shell banks after the final SPU memory clear */
    SsQuit();
}

void shell_sub_8007699C(void)
{
    FUNCTION_MARKER(0x8007699Cu, "SHELL.BIN");
    while (r_u32(0x8008F868u)) { }
    moh_draft_missing_call(0x8001CB3Cu, 7u, 0u);
    moh_draft_missing_call(0x80078F4Cu);
}

void shell_sub_80078048(void)
{
    FUNCTION_MARKER(0x80078048u, "SHELL.BIN");
    moh_draft_missing_call(0x80079EF8u);
}

void shell_sub_80078F4C(void)
{
    uint32 previous, index;
    FUNCTION_MARKER(0x80078F4Cu, "SHELL.BIN");
    previous = sub_8001AF88();
    for (index = 0u; index < 8u; ++index)
        CloseEventPSX(r_u32(0x8008F918u + index * 4u));
    if (previous == 1u) sub_8001AD38();
}

uint32 shell_sub_80079EF8(void)
{
    FUNCTION_MARKER(0x80079EF8u, "SHELL.BIN");
    psx_bios_card_poll();
    SysDeqIntRP(2u, 0x74A8u);
    w_u32(0x74BCu, 0u);
    MemCardEnd();
    /* Native startup installs no executable BIOS patches */
    return 0u;
}

uint32 moh_native_cd_read_callback(uint32 callback)
{
    uint32 previous = r_u32(0x8002E08Cu);
    w_u32(0x8002E08Cu, callback);
    return previous;
}

uint32 moh_native_draw_sync_callback(uint32 callback)
{
    uint32 previous;
    if (r_u8(0x8002DF1Au) >= 2u)
        moh_draft_indirect_call(r_u32(0x8002DF14u), 0x80014828u, callback);
    /* Preserve the callback slot while native DrawSync completes synchronously */
    previous = r_u32(0x8002DF24u);
    w_u32(0x8002DF24u, callback);
    return previous;
}
