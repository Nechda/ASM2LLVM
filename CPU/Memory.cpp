#include "CPU.h"

#define CHECK_MEMORY_BOUND()\
    do{\
    if (addr >= myCPU.ramSize)\
    {\
        logger.push(\
            "Invalid memory access",\
            "You are trying to access non-existent memory."\
        );\
        return nullptr;\
    }\
    }while(0)

void* CPU::convertVirtualAddrToPhysical(ui32 addr)
{

    if ( myCPU.ControlRegister.GPT == -1
    ||   myCPU.ControlRegister.CR.pureMappingAddr > addr )
    {
        CHECK_MEMORY_BOUND();
        return &myCPU.RAM[addr];
    }

    union
    {
        struct
        {
            ui32 offset     : 12;
            ui32 pageIndex  : 10;
            ui32 tableIndex : 10;
        };
        ui32 addr;
    }dividedAddr;

    dividedAddr.addr = addr;
    ui32 pageTable = myCPU.RAM[myCPU.ControlRegister.GPT + (dividedAddr.tableIndex << 2)];
    if (pageTable == -1)
    {
        logger.push(
            "Invalid memory access",
            "Your address [%p] is not supported by loaded GPT",
            addr
        );
        return nullptr;
    }

    union
    {
        struct
        {
            ui32 physAddr   : 20;
            ui32 permission : 2;
            ui32 reserved   : 10;
        };
        ui32 info;
    }pageInfo;

    pageInfo.info = *((ui32*)&myCPU.RAM[pageTable + dividedAddr.pageIndex]);

    addr = (pageInfo.physAddr << 12) + dividedAddr.offset;
    CHECK_MEMORY_BOUND();
    return &myCPU.RAM[addr];
}