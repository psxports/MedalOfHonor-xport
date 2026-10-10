#ifndef MOH_GAME_SHELL_SIGNATURES_H
#define MOH_GAME_SHELL_SIGNATURES_H

#include "game_draft_signatures.h"
uint32 moh_native_movie_screen(uint32 screen);
uint32 moh_native_play_movie(uint32 media);
uint32 shell_title_find_screen(uint32 name, const char *host_name);

uint32 shell_sub_8005FC28(void);
void shell_sub_8005FE04(void);
uint32 shell_sub_800451F0(uint32 bytes);
uint32 shell_sub_80045260(uint32 bytes);
uint32 shell_sub_8003CFD8(uint32 bytes);
uint32 shell_sub_8005F7A0(uint32 packet_count, uint32 secondary);
uint32 shell_sub_8005F89C(uint32 mode, uint32 background, uint32 red, uint32 green, uint32 blue);
void shell_sub_80045390(void);
void shell_sub_80078018(void);
void shell_sub_8007692C(void);
void shell_native_card_init(uint32 mode);
uint32 shell_native_card_start(void);
void shell_sub_80078D70(void);
void shell_sub_80077F10(void);
void shell_sub_80078C4C(void);
uint32 shell_sub_80078CB8(void);
void shell_sub_8006118C(uint32 mode);
void shell_sub_800619C0(uint32 state);
uint32 shell_sub_8005F580(uint32 filename, uint32 unused, uint32 work_buffer);
uint32 shell_resource_find(uint32 resource, uint32 name, uint32 *index);
uint32 shell_sub_8003D3A0(uint32 data, uint32 bytes, uint32 flags);
uint32 shell_sub_8003D3E8(uint32 data, uint32 flags, uint32 bytes);
uint32 shell_sub_8003D500(uint32 list);
sint32 shell_sub_8003D594(uint32 texture, uint32 flags, uint32 data);
void shell_sub_800767E0(uint32 data, uint32 texture);
void shell_sub_80056C58(void);
void shell_sub_800631D0(void);
void shell_sub_80063834(uint32 volume);
void shell_sub_800637FC(void);
void shell_sub_80063940(const void *media, const void *rectangle);
void shell_sub_80040D10(void);
void shell_sub_80060B0C(void);
void shell_sub_8004DC40(void);
uint32 shell_sub_80041790(void);
uint32 shell_sub_80048E98(void);
uint32 shell_sub_80048F14(uint32 owner);
uint32 shell_sub_80048FB0(uint32 owner);
uint32 shell_sub_8004904C(uint32 owner);
uint32 shell_sub_800490F0(uint32 owner);
uint32 shell_sub_80049190(uint32 owner);
uint32 shell_sub_80049230(uint32 owner);
uint32 shell_sub_80049E90(uint32 script, uint32 bytes);
uint32 shell_sub_80058928(void);
uint32 shell_sub_8006349C(void);
uint32 shell_sub_8006FA48(void);
uint32 shell_sub_8004E3E0(void);
uint32 shell_sub_8005C120(void);
uint32 shell_sub_80057B50(void);
uint32 shell_sub_8005A720(void);
uint32 shell_sub_80056230(void);
uint32 shell_sub_8003ECE0(void);
uint32 shell_sub_80046690(void);
uint32 shell_sub_80043518(void);
uint32 shell_sub_80042C58(void);
void shell_sub_80043DF0(uint32 screen);
uint32 shell_sub_8004D838(uint32 screen);
uint32 shell_sub_8003BE58(void);
uint32 shell_sub_8003BEE8(uint32 animation, sint32 mode);
void shell_sub_80063510(uint32 bank, sint32 sample, uint32 voice, uint32 left,
    uint32 right, uint32 rate, uint32 variation);
uint32 shell_sub_80062F70(uint32 resource);
void shell_sub_80045328(uint32 pointer);
uint16 shell_sub_8006377C(uint32 sample, uint32 programs, uint32 header);
sint32 shell_sub_80063314(uint32 resource, uint32 slot);
sint32 shell_sub_800638D4(uint32 resource, uint32 slot);


uint32 shell_sub_8004FB30(void);
uint32 shell_sub_8003FD60(void);
uint32 shell_sub_8003D9D8(void);

uint32 shell_sub_80066830(void);

uint32 shell_sub_80067820(void);
uint32 shell_sub_800687A0(void);

uint32 shell_sub_80074D20(void);

uint32 shell_sub_80063E30(void);

uint32 shell_sub_80069C68(void);

uint32 shell_sub_80070960(void);

uint32 shell_sub_80072038(void);

uint32 shell_sub_8006D760(void);

uint32 shell_sub_80072D38(void);

uint32 shell_sub_8006A888(void);

uint32 shell_sub_8006C600(void);

uint32 shell_sub_8006CDF8(void);

void shell_sub_8004A8BC(void);

uint32 shell_sub_8003D03C(void);

void shell_sub_80060C7C(void);
void shell_sub_80061F00(void);

sint32 shell_sub_8003D8EC(uint32 node);
sint32 shell_sub_8003D890(uint32 list);

void shell_sub_8005F3A4(uint32 player);

void shell_sub_8005D590(void);

uint32 shell_sub_8004D9D8(void);

void shell_sub_80056CB4(uint32 first, uint32 second);

void shell_sub_800419E8(uint32 screen);

void shell_sub_80058CC8(uint32 screen);
void shell_sub_80058FE0(uint32 screen);
void shell_sub_8004E0BC(uint32 data, uint32 name);

uint32 shell_sub_8004B10C(uint32 image, uint32 primary, uint32 secondary);
uint32 shell_sub_8004B974(uint32 screen, uint32 primary, uint32 secondary);
uint32 shell_sub_8004C21C(uint32 screen, uint32 primary, uint32 secondary);

void shell_sub_8004AAA4(uint32 widget, uint32 parameters, uint32 color);
uint32 shell_sub_8004AC98(uint32 widget, uint32 primary, uint32 secondary);

uint32 shell_sub_8004B2DC(uint32 screen, uint32 primary, uint32 secondary);

uint32 shell_sub_80053150(uint32 model);
uint32 shell_sub_80052FE8(uint32 sequence);

uint32 shell_model_build_packets(uint32 model, uint32 source, uint32 flags, uint32 color);

uint32 shell_sub_80051EF4(uint32 source, uint32 parameters, uint32 flags, uint32 color, uint32 callback, uint32 enabled);

uint32 shell_sub_8003C110(uint32 animation);
void shell_sub_8003C30C(uint32 first, uint32 second, sint32 duration, uint32 output);
void shell_sub_8003C448(uint32 first, uint32 second, sint32 duration, uint32 output);

void shell_sub_8004DD9C(void);

void shell_sub_8005A1EC(uint32 screen);
void shell_sub_8004E1B8(uint32 data, uint32 mode);

uint32 shell_sub_8004BDB4(uint32 screen);

void shell_sub_8004CFB4(uint32 image, uint32 mode);
void shell_sub_8004D694(uint32 color);
uint32 shell_sub_8004DE8C(uint32 color);
void shell_sub_8004D794(uint32 color, uint32 first, uint32 second);
uint32 shell_sub_80053418(uint32 model);
void shell_sub_80053798(uint32 source, uint32 destination);
void shell_sub_800538C4(uint32 source, uint32 destination);
void shell_sub_800539F4(uint32 parameters, uint32 first, uint32 second);
void shell_sub_80063CF4(const uint32 *first, const uint32 *second, sint32 duration, uint32 *output);
uint32 shell_sub_80053C8C(uint32 model, uint32 flags);
uint32 shell_sub_8005A430(uint32 screen);
uint32 shell_sub_8005F258(uint32 button);
void shell_sub_80059030(uint32 screen, uint32 result);
void shell_sub_8004DE14(void);
uint32 shell_sub_8003CA30(uint32 animation);
void shell_sub_80062108(uint32 mode);
void shell_sub_80058F8C(uint32 screen);
void shell_sub_8004E16C(uint32 data);
uint32 shell_sub_8004B894(uint32 screen);
uint32 shell_sub_8004C328(uint32 screen);
uint32 shell_sub_8004BAF0(uint32 screen);
uint32 shell_sub_80053358(uint32 model);
void shell_sub_8003EF24(uint32 screen);
void shell_sub_8003F068(uint32 screen);
void shell_sub_8003F014(uint32 screen);
void shell_sub_8003F6EC(uint32 screen, uint32 result);
void shell_sub_8003FA7C(uint32 screen);
uint32 shell_sub_8003FAA0(uint32 screen);
void shell_sub_8004CDEC(uint32 group);
void shell_sub_80060D1C(void);
uint32 shell_sub_80063070(uint32 bank);
void shell_sub_800630CC(void);
void shell_sub_80063124(void);
uint32 shell_sub_8005F5E4(uint32 resource);
uint32 shell_sub_800492D0(void);
uint32 shell_sub_80049324(uint32 screen);
void shell_sub_800453F8(void);
void shell_sub_80045430(void);
void shell_sub_8005FB50(uint32 clear);
uint32 shell_sub_80045234(void);
uint32 moh_native_draw_sync(uint32 mode);
uint32 moh_native_put_draw_env(uint32 guest_env);
uint32 moh_native_put_disp_env(uint32 guest_env);
uint32 moh_native_draw_otag(uint32 guest_ot);

void shell_sub_8004D508(uint32 widget);

void shell_sub_8004C710(uint32 group, uint32 mode);
uint32 shell_sub_8004C51C(uint32 screen, uint32 override);

void shell_sub_8004CAF4(uint32 group);

uint32 shell_sub_8004BF74(uint32 screen);

void shell_sub_8003CCEC(uint32 model, uint32 frame_parameters, uint32 alternate);

uint32 shell_sub_8003C4AC(uint32 animation);
void shell_sub_80063A3C(uint32 parameters);
void shell_sub_80063AA8(uint32 parameters);

uint32 moh_native_screen_initialized(uint32 screen);
uint32 shell_sub_8003BE8C(uint32 animation);
uint32 shell_sub_8003C028(uint32 frame);
void shell_sub_8007699C(void);
void shell_sub_80078048(void);
void shell_sub_80078F4C(void);
uint32 shell_sub_80079EF8(void);
uint32 moh_native_cd_read_callback(uint32 callback);
uint32 moh_native_draw_sync_callback(uint32 callback);
void moh_native_note_screen_initialized(uint32 screen);
void moh_native_forget_screen(uint32 screen);
void shell_sub_80058FB0(uint32 screen);
void shell_sub_8003F038(uint32 screen);
uint32 shell_sub_800496A0(uint32 group);
uint32 shell_sub_800498E8(uint32 item);
uint32 shell_sub_80049A6C(uint32 node);
uint32 shell_sub_80049B54(uint32 node);
uint32 shell_sub_80049DA8(uint32 node);
uint32 shell_sub_80049C60(uint32 node);
#endif
