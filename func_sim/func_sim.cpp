#include <iostream>

#include <func_sim/mips_memory.h>

#include "func_sim.h"

MIPS::~MIPS() { delete mem; }

std::string MIPS::step()
{
    // fetch
    uint32 instr_bytes = mem->fetch( PC);

    // decode
    FuncInstr instr(instr_bytes, PC);

    // read sources
    rf.read_sources( instr);

    // execute
    instr.execute();

    // load/store
    mem->load_store(instr);

    // writeback
    rf.write_dst( instr);

    // PC update
    PC = instr.get_new_PC();

    // dump
    return instr.Dump();
}

void MIPS::init( const std::string& tr)
{
    assert( mem == nullptr);
    mem = new MIPSMemory(tr.c_str());
    PC = mem->startPC();
}

void MIPS::run(const std::string& tr, uint32 instrs_to_run)
{
    init( tr);
    for (uint32 i = 0; i < instrs_to_run; ++i)
        std::cout << step() << std::endl;
}

