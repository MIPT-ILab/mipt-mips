/*
 * func_sim.cpp - extremely simple simulator
 * Copyright 2018 MIPT-MIPS
 */
 
#include "func_sim.h"
#include <infra/memory/elf/elf_loader.h>

template <typename ISA>
FuncSim<ISA>::FuncSim( bool log) : Simulator( log), mem( new Memory) { }

template <typename ISA>
void FuncSim<ISA>::update_and_check_nop_counter( const typename FuncSim<ISA>::FuncInstr& instr)
{
    if ( instr.is_nop())
        ++nops_in_a_row;
    else
        nops_in_a_row = 0;

    if (nops_in_a_row > 10)
        throw BearingLost();
}

template <typename ISA>
typename FuncSim<ISA>::FuncInstr FuncSim<ISA>::step()
{
    // fetch instruction
    FuncInstr instr = mem->fetch_instr( PC);

    // set sequence_id
    instr.set_sequence_id(sequence_id);
    sequence_id++;

    // read sources
    rf.read_sources( &instr);

    // execute
    instr.execute();

    // load/store
    mem->load_store( &instr);

    // writeback
    rf.write_dst( instr);

    // trap check
    instr.check_trap();

    // PC update
    PC = instr.get_new_PC();

    // Check whether we execute nops
    update_and_check_nop_counter( instr);

    // dump
    return instr;
}

template <typename ISA>
void FuncSim<ISA>::init( const std::string& tr)
{
    ::load_elf_file( mem.get(), tr);
    PC = mem->startPC();
    nops_in_a_row = 0;
}

template <typename ISA>
Trap FuncSim<ISA>::run( const std::string& tr, uint64 instrs_to_run)
{
    init( tr);
    for ( uint32 i = 0; i < instrs_to_run; ++i) {
        const auto& instr = step();
        sout << instr << std::endl;

        if ( instr.has_trap())
            return instr.trap_type();
    }
    return Trap::NO_TRAP;
}

#include <mips/mips.h>
#include <risc_v/risc_v.h>

template class FuncSim<MIPSI>;
template class FuncSim<MIPSII>;
template class FuncSim<MIPSIII>;
template class FuncSim<MIPSIV>;
template class FuncSim<MIPS32>;
template class FuncSim<MIPS64>;
template class FuncSim<RISCV32>;
template class FuncSim<RISCV64>;
template class FuncSim<RISCV128>;

