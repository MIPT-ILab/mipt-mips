#include <iostream>

#include <mips/mips_memory.h>
#include <mips/mips_rf.h>
#include <mips/mips.h>

#include "func_sim.h"

template class FuncSim<MIPS>;

template <typename ISA>
FuncSim<ISA>::FuncSim( bool log) : Log( log), rf( new RF) { }

template <typename ISA>
FuncSim<ISA>::~FuncSim()
{
    delete mem;
}

template <typename ISA>
typename FuncSim<ISA>::FuncInstr FuncSim<ISA>::step()
{
    // fetch instruction
    FuncInstr instr = mem->fetch_instr( PC);

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

template <typename ISA>
void FuncSim<ISA>::init( const std::string& tr)
{
    assert( mem == nullptr);
    mem = new Memory( tr);
    PC = mem->startPC();
}

template <typename ISA>
void FuncSim<ISA>::run( const std::string& tr, uint32 instrs_to_run)
{
    init( tr);
    for ( uint32 i = 0; i < instrs_to_run; ++i)
        sout << step() << std::endl;
}
