#include <iostream>

#include <mips/mips_memory.h>
#include <mips/mips_rf.h>

#include "func_sim.h"

FuncSim::FuncSim( bool log) : Log( log), rf( new MIPSRF) { }

FuncSim::~FuncSim()
{
    delete mem;
}

MIPSInstr FuncSim::step()
{
    // fetch instruction
    MIPSInstr instr = mem->fetch_instr( PC);

    // read sources
    rf->read_sources( &instr);

    // execute
    instr.execute();

    // load/store
    mem->load_store( &instr);

    // writeback
    rf->write_dst( instr);

    // trap check
    instr.check_trap();

    // PC update
    PC = instr.get_new_PC();

    // dump
    return instr;
}

void FuncSim::init( const std::string& tr)
{
    assert( mem == nullptr);
    mem = new MIPSMemory( tr);
    PC = mem->startPC();
}

void FuncSim::run( const std::string& tr, uint32 instrs_to_run)
{
    init( tr);
    for ( uint32 i = 0; i < instrs_to_run; ++i)
        sout << step() << std::endl;
}

