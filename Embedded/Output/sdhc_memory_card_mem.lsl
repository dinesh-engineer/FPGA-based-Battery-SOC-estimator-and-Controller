#include "sdhc_memory_card.lsl"
#define SDA_GROUP_REDEFINED
#define BOUNDS_GROUP_REDEFINED
#ifdef __SW_LSL
#include __SW_LSL
#endif

derivative system
{
    core sw
    {
        architecture = __SW_ARCH;
    }

    memory TSK3000A_1
    {
        mau = 8;
        type = nvram;
        size = 32768;
        map(dest=bus:sw:addr_bus, src_offset=0x0, dest_offset=0x0, size=32768);
    }

    memory XRAM
    {
        mau = 8;
        type = ram;
        size = 512k;
        map(dest=bus:sw:addr_bus, src_offset=0x0, dest_offset=0x1000000, size=512k);
    }

    memory XROM
    {
        mau = 8;
        type = rom;
        size = 512k;
        map(dest=bus:sw:addr_bus, src_offset=0x0, dest_offset=0x01080000, size=512k);
    }



    // Software Platform locate rules
    #ifdef __SWPLATFORM__
    #include "framecfg\swplatform.lsl"
    #endif

    // (Automatically generated)
    section_layout TSK3000:sw:main
    {
        group BOUNDS_GROUP_NAME (run_addr = mem:TSK3000A_1|mem:XROM, ordered, contiguous) select BOUNDS_GROUP_SELECT;
    }

    // (Automatically generated)
    section_layout TSK3000:sw:main
    {
        group SDA_GROUP_NAME (run_addr = mem:TSK3000A_1|mem:XRAM, ordered, contiguous) select SDA_GROUP_SELECT;
    }

    // (Automatically generated)
    section_layout TSK3000:sw:main
    {
        group (run_addr = mem:TSK3000A_1|mem:XROM) select "_lc__ctors";
    }

    // (Automatically generated)
    section_layout TSK3000:sw:main
    {
        group (run_addr = mem:TSK3000A_1|mem:XRAM) select "heap";
    }

    // (Automatically generated)
    section_layout TSK3000:sw:main
    {
        group (run_addr = mem:TSK3000A_1|mem:XROM) select "table";
    }

    // (Automatically generated)
    section_layout TSK3000:sw:main
    {
        group (run_addr = mem:TSK3000A_1|mem:XRAM) select "stack";
    }

    // (Automatically generated)
    section_layout TSK3000:sw:main
    {
        group (load_addr = mem:TSK3000A_1|mem:XROM) select "*";
    }

    // (Automatically generated)
    section_layout TSK3000:sw:main
    {
        group (run_addr = mem:TSK3000A_1|mem:XRAM) select ".data*";
    }

    // (Automatically generated)
    section_layout TSK3000:sw:main
    {
        group (run_addr = mem:TSK3000A_1|mem:XRAM) select ".bss*";
    }

    // (Automatically generated)
    section_layout TSK3000:sw:main
    {
        group (run_addr = mem:TSK3000A_1|mem:XROM) select ".rodata*";
    }

    // (Automatically generated)
    section_layout TSK3000:sw:main
    {
        group (run_addr = mem:TSK3000A_1|mem:XROM) select ".text.cstart*";
    }

    // (Automatically generated)
    section_layout TSK3000:sw:main
    {
        group (run_addr = mem:TSK3000A_1|mem:XROM) select ".text*";
    }

    // (Automatically generated)
    section_layout TSK3000:sw:main
    {
        group (run_addr = mem:TSK3000A_1|mem:XROM) select "_lc__dtors";
    }











}
