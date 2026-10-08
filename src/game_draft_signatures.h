#ifndef MOH_GAME_DRAFT_SIGNATURES_H
#define MOH_GAME_DRAFT_SIGNATURES_H

#include "xport.h"
#define MOH_BYTE2(value) (((uint32)(value) >> 16) & 255u)
#define MOH_HIBYTE(value) (((uint32)(value) >> 8) & 255u)

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
uint32 sub_80017234(uint32 a1, uint32 a2);
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
uint32 sub_8001B3E0(uint32 callback);
/* TODO Entry uses incoming v0 as callback target */
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
uint32 sub_8001F740(uint32 input_t0, uint32 input_v1, uint32 input_hi, uint32 input_a2);
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
uint32 sub_80022D74(uint32 a1);
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
uint32 sub_80022E64(uint32 a1);
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
uint32 sub_800266BC(uint32 environment);
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
void sub_80036AF8(void);
void sub_80036B20(uint32 start, uint32 size);
/* TODO Draft ABI and unresolved volatile argument carriers */
uint32 sub_80036BF0(uint32 a1);
/* TODO CP0 critical-section adapter */
uint32 sub_80036C14(uint32 size, uint32 owner, uint32 flags);
/* TODO Draft ABI and unresolved volatile argument carriers */
void sub_80036E44(uint32 a1);
/* TODO Draft ABI and unresolved volatile argument carriers */
void sub_80037060(void);
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
uint32 moh_draft_missing_call(uint32 address, ...);
uint32 moh_draft_indirect_call(uint32 target, ...);
void *moh_draft_guest_pointer(uint32 address, uint32 bytes);
uint32 moh_draft_local_address(const void *buffer, uint32 bytes);

#endif
