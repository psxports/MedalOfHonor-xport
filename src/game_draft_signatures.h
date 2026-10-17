#ifndef MOH_GAME_DRAFT_SIGNATURES_H
#define MOH_GAME_DRAFT_SIGNATURES_H

#include "xport.h"
void sub_8001C6D8(uint32 distance, uint32 divisor);
uint32 sub_8001552C(void);
uint32 sub_8001D590(uint32 value);
uint32 sub_8001D7B0(uint32 first, uint32 second);
uint32 sub_8001D880(uint32 first, uint32 second);
uint32 sub_8001D7E0(uint32 first, uint32 second);
uint32 sub_8001D700(uint32 first, uint32 second);
uint32 sub_8001D660(uint32 first, uint32 second);
#define MOH_BYTE2(value) (((uint32)(value) >> 16) & 255u)
#define MOH_HIBYTE(value) (((uint32)(value) >> 8) & 255u)
void moh_draft_fail_fast(const char *operation, uint32 address);
uint32 moh_native_music_enabled(void);
uint32 sub_8001AB5C(uint32 counter);
uint32 sub_8001D140(uint32 callback);
uint32 sub_80016D40(uint32 callback);
uint32 sub_8001CB6C(void);
uint32 sub_8001D160(void);
uint32 sub_800269C8(void);
void moh_native_spu_quit(void);
uint32 sub_8001CFB4(void);
uint32 sub_8001AF68(void);
uint32 sub_80016D30(void);
void sub_8001C714(uint32 value);
void sub_8002669C(uint32 value);
void sub_800266AC(uint32 value);
uint32 sub_8002666C(uint32 callback);
uint32 sub_80016A70(uint32 x, uint32 y, uint32 z, uint32 output);
uint32 sub_80022E54(void);
void sub_8001DFB0(uint32 allocation);
uint32 sub_8001716C(uint32 ordering_table, uint32 count, uint32 diagnostic_level);

/* Unverified draft ABI; integration is deferred */
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_8001575C(uint32 a1, uint32 a2, uint32 a3);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_8001582C(void);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80016B60(uint32 a0);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80016CD4(uint32 a0);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80016DA0(uint32 a0);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_80016E38(uint32 a1);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_80016EA0(uint32 a1, uint32 a2);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80016FBC(uint32 a0, uint32 a1, uint32 a2, uint32 a3);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_8001704C(uint32 a0, uint32 a1);
uint32 sub_800170AC(uint32 a1, uint32 a2, uint32 a3);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_80017234(uint32 ot, uint32 count, uint32 diagnostic_level);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_800172E0(uint32 ot, uint32 incoming_v0);
uint32 sub_80017348(uint32 a1);
uint32 sub_80017408(uint32 a1);
uint32 sub_80017AF8(uint32 a1, uint32 a2);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80017D68(uint32 a0, uint32 a1, uint32 a2);
uint32 sub_80017D88(uint32 a1, uint32 a2);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_80017E20(uint32 a1, uint32 a2);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_80017EB8(uint32 a1, uint32 a2);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_80017ED4(uint32 a1);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80017F6C(uint32 a0, uint32 a1);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_8001804C(uint32 a1, uint32 a2);
uint32 sub_8001827C(uint32 a1, uint32 a2);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_80018738(uint32 a1);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80018794(uint32 a0);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_800187DC(uint32 a1);
sint32 sub_80018830(uint32 callback, uint32 data, uint32 size, uint32 argument);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80018AE0(void);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_80018D40(uint32 a1);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80018E7C(uint32 a0);
uint32 sub_80018FB8(void);
sint32 sub_80018FEC(void);
uint32 sub_80019130(uint32 a1);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_800191F8(uint32 a1, uint32 a2, uint32 a3);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_80019740(uint32 a1);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_8001AC88(uint32 a0);
uint32 sub_8001AD38(void);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_8001AD98(void);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_8001ADD8(uint32 a1, uint32 a2);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_8001AEE8(uint32 a0);
uint32 sub_8001AF88(void);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_8001B3E0(uint32 port, uint32 entry_callback);
/* Internal body receives the callback loaded by the recovered entry */
uint32 sub_8001B730(uint32 context, uint32 value, uint32 mode, uint32 entry_callback);
/* TODO Variadic words are passed as a guest argument-list address */
uint32 sub_8001BE24(uint32 output, uint32 format, uint32 arguments);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_8001CA78(void);
uint32 sub_8001CAA8(uint32 index, uint32 callback);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_8001CAD8(uint32 argument0, uint32 argument1);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_8001CB08(uint32 a0);
uint32 sub_8001CB3C(uint32 slot, uint32 callback);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_8001CB9C(void);
uint32 sub_8001CBAC(uint32 a1);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_8001CBC4(void);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_8001CC9C(void);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_8001CE6C(uint32 a1, uint32 a2);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_8001D0CC(uint32 a0, uint32 a1);
uint32 sub_8001D0F0(uint32 callback);
/* TODO Tail-call argument carrier */
uint32 sub_8001D180(void);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_8001D1B0(uint32 callback);
/* Existing startup translation retained */
void sub_8001DFD4(void);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_8001E0EC(uint32 a0);
/* Unverified draft ABI; TODO Refine semantic parameter names */
void sub_8001F17C(void);
uint32 sub_8001F55C(uint32 a1, uint32 a2, uint32 a3);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_8001F67C(uint32 a1, uint32 a2, uint32 a3);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_8001F6EC(uint32 a1);
/* TODO Recover nonstandard incoming t0, v1, HI and a2 at callers */
uint32 sub_8001F740(uint32 first_high_product, uint32 first_dividend, uint32 second_high_product, uint32 second_dividend);
uint32 sub_8001FC58(void);
/* Unverified draft ABI; TODO Refine semantic parameter names */
void sub_8001FF08(uint32 a0);
/* Unverified draft ABI; TODO Refine semantic parameter names */
void sub_80020138(void);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_800201F8(void);
uint32 sub_800203F8(void);
uint32 sub_80020F48(uint32 a1);
uint32 sub_800211C8(uint32 a1, uint32 a2);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_80021388(void);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_80021444(uint32 a1, uint32 a2, uint32 a3);
uint32 sub_800216C4(uint32 a1, uint32 a2, uint32 a3);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_80021748(uint32 a1, uint32 a2, uint32 a3);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_8002178C(uint32 a0, uint32 a1);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80021888(void);
uint32 sub_800218D8(void);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_80022560(void);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_80022B9C(void);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80022BBC(void);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80022C54(void);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80022D34(void);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_80022D74(uint32 command, uint32 parameters);
void sub_80022DF0(uint32 a1);
void sub_80022DFC(uint32 a1);
/* Unverified draft ABI; TODO Refine semantic parameter names */
void sub_80022E08(uint32 a0);
/* TODO Draft ABI and unresolved volatile argument carriers */
void sub_80022E14(uint32 a1);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80022E20(uint32 a0);
uint32 sub_80022E38(void);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80022E48(void);
uint32 sub_80022E64(uint32 command, uint32 parameters);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80022EA4(uint32 a0, uint32 a1);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_8002307C(void);
uint32 sub_800233D4(uint32 a1, uint32 a2);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_800234E4(uint32 a0, uint32 a1);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_800236C8(uint32 a1, uint32 a2);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80023A3C(uint32 a0, uint32 a1);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80023AD0(uint32 a0, uint32 a1);
uint32 sub_80023B80(void);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80023BF8(void);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80023C08(void);
/* TODO Draft ABI and unresolved volatile argument carriers */
void sub_80023C18(uint32 a1, uint32 a2);
void sub_80023C60(uint32 a1, uint32 a2);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80023CAC(uint32 a0);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80023CDC(uint32 a0);
uint32 sub_80023D0C(void);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80023D74(void);
uint32 sub_80023F04(uint32 a1);
uint32 sub_80024238(uint32 a1);
uint32 sub_80024328(uint32 a1, uint32 a2);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_8002454C(uint32 a0, uint32 a1);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_800247C8(void);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80025130(uint32 a0, uint32 a1);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_800251D8(uint32 a1);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_800251F8(void);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_80025298(uint32 a1);
uint32 sub_800252E0(uint32 a1);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_800253B8(uint32 a1);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80025464(uint32 a0);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_80025528(uint32 a1);
/* TODO Native continuation adapter replaces guest saved-register snapshot */
uint32 sub_800266BC(uint32 environment, uint32 (*continuation)(void));
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_80026AB8(uint32 a1);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_80026AD8(void);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_8002703C(uint32 a0, uint32 a1);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_800272BC(uint32 a0, uint32 a1);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_80027584(uint32 a1, uint32 a2, uint32 a3, uint32 a4);
uint32 moh_draft_cd_timeout(void);
void moh_draft_cd_dispatch(uint32 site);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_80027990(uint32 a1);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80027A18(void);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_80027AEC(void);
sint32 sub_80027BDC(void);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_80027DC8(uint32 a1, uint32 a2, uint32 a3, uint32 a4);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80028A74(uint32 a0, uint32 a1);
uint32 sub_80028AD4(void);
void sub_80028BB0(uint32 a1, uint32 a2, uint32 a3);
uint32 sub_80029420(uint32 a1);
uint32 sub_80029448(void);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80029530(uint32 a0, uint32 a1, uint32 a2, uint32 a3);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_8002A8F0(uint32 a1);
uint32 sub_8002ACD0(uint32 callback);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_8002AD00(uint32 a0);
/* TODO Draft ABI and unresolved volatile argument carriers */
void sub_8002ADFC(void);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_8002AE48(uint32 a0);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_800362E8(uint32 a0, uint32 a1, uint32 a2);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_800363FC(uint32 a0, uint32 a1, uint32 a2);
uint32 sub_800369EC(uint32 status);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80036A70(uint32 status);
uint32 sub_80036AF8(void);
uint32 sub_80036B20(uint32 start, uint32 size);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_80036BF0(uint32 a1);
/* TODO CP0 critical-section adapter */
uint32 sub_80036C14(uint32 size, uint32 owner, uint32 flags);
/* TODO Draft ABI and unresolved volatile argument carriers */
void sub_80036E44(uint32 a1);
/* TODO Draft ABI and unresolved volatile argument carriers */
void sub_80037060(void);
uint32 moh_startup_initializers(void);
sint32 sub_80037318(uint32 filename, uint32 configuration);
sint32 sub_80037370(uint32 sectors, uint32 sector, uint32 destination);
uint32 sub_8003740C(uint32 a1, uint32 a2);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_800374EC(uint32 a0, uint32 a1);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_80037A0C(uint32 a1, uint32 a2, uint32 a3);
/* Unverified draft ABI; TODO Refine semantic parameter names */
uint32 sub_80037BB4(uint32 a0, uint32 a1);
uint32 sub_80037C70(uint32 a1, uint32 a2, uint32 a3);

/* TODO Supply missing BIOS, SDK and address adapters during integration */
uint32 moh_native_irq_return(void);
void moh_native_irq_prepare(uint32 environment, uint32 (*handler)(void));
void moh_native_dma_complete(uint32 channel);
uint32 moh_native_reset_graph(uint32 mode);
uint32 moh_native_load_file(uint32 filename, uint32 size_address, uint32 destination);
uint32 moh_native_load_resource(uint32 filename, uint32 destination, uint32 capacity);
sint32 moh_native_load_overlay(uint32 filename, uint32 destination);
uint32 moh_native_cd_reset(void);
uint32 moh_native_cd_stop(void);
uint32 moh_native_draw_sync(uint32 mode);
uint32 moh_native_sound_init(void);
uint32 moh_draft_missing_call(uint32 address, ...);
uint32 sub_8001DB60(uint32 value);
uint32 sub_8001AF98(uint32 rotation, uint32 matrix);
uint32 sub_800165D8(uint32 matrix, uint32 scale);
uint32 moh_draft_indirect_call(uint32 target, ...);
void *moh_draft_guest_pointer(uint32 address, uint32 bytes);
uint32 moh_draft_local_address(const void *buffer, uint32 bytes);
uint32 sub_80036DA0(uint32 size, uint32 count);
uint32 sub_80036DC4(uint32 size, uint32 count, uint32 owner, uint32 flags);
uint32 moh_native_asset_lba(const char *name);
uint32 moh_native_find_asset(uint32 result, uint32 filename);
uint32 sub_80016718(uint32 size);
uint32 sub_80016248(sint32 angle, uint32 matrix);
uint32 sub_80016218(uint32 matrix, uint32 translation);
uint32 sub_80016B30(uint32 input, uint32 output, uint32 flags_out);
uint32 sub_80016A10(uint32 input, uint32 output);
uint32 sub_80016578(uint32 left, uint32 right, uint32 output);
uint32 sub_800163E8(uint32 matrix, uint32 input, uint32 output);
uint32 sub_80011AB0(uint32 node);
uint32 sub_80011214(uint32 next_triangle, uint32 vertices);
void sub_800114B0(uint32 region, uint32 vertices, uint32 colors_out);
uint32 sub_8001BC68(uint32 input);
void sub_80013AE4(uint32 node, uint32 matrix, uint32 output, uint32 render);
void moh_native_model_transform(uint32 node, uint32 matrix, uint32 output, uint32 render);
uint32 sub_80013E58(uint32 quaternion, uint32 matrix);
uint32 sub_80013F98(uint32 ordering_tag, uint32 last, uint32 end);
uint32 sub_80010B60(uint32 packet, uint32 end, uint32 region, uint32 ordering_table);
uint32 sub_80010000(uint32 triangle, uint32 packet, uint32 mask, uint32 depth);
uint32 sub_80010A7C(uint32 triangle, uint32 packet, uint32 first_depth, uint32 second_depth, uint32 third_depth);
uint32 sub_8001556C(uint32 input, uint32 output);

void sub_8001874C(void);
uint32 sub_8002B068(uint32 pattern, uint32 record);
void sub_8002B758(void);
void sub_80024880(uint32 state, uint32 command, uint32 buffer, uint32 count);
uint32 sub_80025150(uint32 state);
uint32 sub_800251C4(uint32 state);
uint32 sub_80028B90(uint32 callback);
uint32 sub_80017F54(void);
uint32 sub_80019654(void);
uint32 sub_80020CE8(void);
uint32 sub_80022530(void);
uint32 sub_80022B7C(void);
uint32 sub_80025164(uint32 state, uint32 value);
uint32 sub_80025184(uint32 state, uint32 value);
uint32 sub_800251A4(uint32 state, uint32 value);
uint32 sub_8001880C(uint32 callback, uint32 data, uint32 argument);
uint32 sub_80028B60(uint32 callback);
uint32 sub_800191D0(void);
uint32 sub_800218B0(void);
uint32 sub_80024858(void);
uint32 sub_8002BA68(uint32 first, uint32 second);
uint32 sub_8001AB90(uint32 counter);
uint32 sub_80024B84(uint32 state);
uint32 sub_80018754(uint32 source, uint32 count);
uint32 sub_800201A4(void);
uint32 sub_80020158(void);
uint32 sub_800135F8(uint32 bounds, uint32 scratch);
uint32 sub_8001D054(void);
uint32 sub_80024890(uint32 state);
uint32 sub_8002334C(void);
uint32 sub_8001AA90(uint32 counter, uint32 target, uint32 flags);
uint32 sub_80024C90(uint32 state);
uint32 sub_80024BBC(uint32 state, uint32 bytes);
uint32 sub_8002BFC8(uint32 bank, uint32 sequence);
uint32 sub_800145DC(uint32 ordering_tag, uint32 first, uint32 end);
uint32 sub_8001449C(uint32 quaternion, uint32 matrix);
uint32 sub_80023950(uint32 event, uint32 response);
uint32 sub_8001B2E0(uint32 port, uint32 property, uint32 index, uint32 entry_callback);
uint32 sub_8001B5C0(uint32 port, uint32 buffer, uint32 entry_callback);
uint32 sub_8001B890(uint32 port, uint32 buffer, uint32 length, uint32 entry_callback);
uint32 sub_8001B2D8(uint32 port, uint32 property, uint32 index);
uint32 sub_8001B3D8(uint32 port);
uint32 sub_8001B5B8(uint32 port, uint32 buffer);
uint32 sub_8001B728(uint32 port, uint32 value, uint32 mode);
uint32 sub_8001B888(uint32 port, uint32 buffer, uint32 length);
uint32 sub_80017900(uint32 packet, uint32 environment);
uint32 sub_800184B8(uint32 rectangle, uint32 destination);
uint32 sub_80024914(uint32 state);
uint32 sub_80024D38(uint32 state);
uint32 sub_8001F7AC(uint32 selector_or_format, uint32 arguments, uint32 initial_length);
uint32 sub_8001F79C(uint32 selector_or_format, uint32 arguments);
uint32 sub_80014128(uint32 node, uint32 matrix, uint32 output, uint32 render);
uint32 sub_800125C4(uint32 triangle, uint32 packet, uint32 first_depth, uint32 second_depth, uint32 third_depth, uint32 scratch);
uint32 sub_80012D5C(uint32 next_triangle, uint32 vertices, uint32 scratch);
uint32 sub_80011B48(uint32 triangle, uint32 packet, uint32 mask, uint32 depth);
void sub_80012FF8(uint32 region, uint32 vertices, uint32 colors_out, uint32 scratch);
uint32 sub_80013698(uint32 root);

uint32 sub_800126A8(uint32 cursor, uint32 limit, uint32 resource, uint32 ordering_table);

#endif
