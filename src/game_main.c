#include "xport.h"
#include "xport_trace.h"

const uint32 xport_gpu_graph_type_address = 0x8002DF18u;

void moh_load_executable(void);

/* Guest startup state shared with the deferred translated callees */
uint32 moh_guest_sp;
uint32 moh_guest_fp;
uint32 moh_guest_gp;
uint32 moh_guest_ra;

void sub_80036B20(uint32 start, uint32 size);
#if defined(_MSC_VER)
__declspec(noreturn) void sub_80037060(void);
#else
_Noreturn void sub_80037060(void);
#endif

void sub_8001DFD4(void)
{
    uint32 address = 0x800CA2A0u;
    uint32 stack_top;
    uint32 reserved_stack;
    uint32 heap_size;

    FUNCTION_MARKER(0x8001DFD4u, "SLUS_009.74");
    do
    {
        w_u32(address, 0u);
        address += 4u;
    } while (address < 0x800D17E8u);

    /* Original loaded stack metadata makes ADDI -8 non-overflowing */
    stack_top = r_u32(0x80039C38u) - 8u;
    moh_guest_sp = stack_top | 0x80000000u;
    reserved_stack = r_u32(0x80039C3Cu);
    heap_size = stack_top - reserved_stack - 0x000D17E8u;
    w_u32(0x8002F280u, heap_size);
    w_u32(0x8002F27Cu, 0x800D17E8u);
    w_u32(0x800CA2A0u, moh_guest_ra);
    moh_guest_gp = 0x80035D1Cu;
    moh_guest_fp = moh_guest_sp;

    moh_guest_ra = 0x8001E064u;
    sub_80036B20(0x800D17ECu, heap_size);
    moh_guest_ra = r_u32(0x800CA2A0u);
    moh_guest_ra = 0x8001E078u;
    sub_80037060();
}

void xport_main(void)
{
    /* BIOS Exec JALR link verified against the pinned cold-boot snapshot */
    moh_load_executable();
    moh_guest_ra = 0xBFC03D60u;
    sub_8001DFD4();
}
