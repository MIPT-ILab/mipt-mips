/*
 * func_sim.cpp - extremely simple simulator
 * Copyright 2018 MIPT-MIPS
 */

#include "func_sim.h"
#include <kernel/kernel.h>

template <typename ISA>
FuncSim<ISA>::FuncSim( unsigned int delayed_slots_to_enable, bool log):
    Simulator( log),
    freezed_instruction_ptr( nullptr),
    enabled_delayed_slots( delayed_slots_to_enable),
    instr_to_execute_before_freezed( 0),
    freezed_jump_instruction( false),
    garbage_freezed_instruction( false)
{ }

template <typename ISA>
void FuncSim<ISA>::set_memory( std::shared_ptr<FuncMemory> m)
{
    mem = std::move( m);
    imem.set_memory( mem);
}

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

    FuncInstr instr = fetch_instruction();

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

    // Check if there undeleted instruction
    if (garbage_freezed_instruction)
        delete freezed_instruction_ptr;

    // dump
    return instr;
}

template <typename ISA>
Trap FuncSim<ISA>::run( uint64 instrs_to_run)
{
    nops_in_a_row = 0;
    for ( uint32 i = 0; i < instrs_to_run; ++i) {
        const auto& instr = step();
        sout << instr << std::endl;

        switch ( instr.trap_type()) {
        case Trap::HALT:
            return Trap::HALT;
        case Trap::SYSCALL:
            if ( kernel.get() && !kernel->execute())
                return Trap::SYSCALL;
            break;
        default:
            break;
        }
    }
    return Trap::NO_TRAP;
}

template <typename ISA>
typename FuncSim<ISA>::FuncInstr FuncSim<ISA>::fetch_instruction()
{
    if (enabled_delayed_slots == 0)
        return imem.fetch_instr( PC);

    if (freezed_jump_instruction && instr_to_execute_before_freezed == 0) {
        freezed_jump_instruction = false;
        return *freezed_instruction_ptr;
    } else if (freezed_jump_instruction && instr_to_execute_before_freezed > 0) {
        instr_to_execute_before_freezed--;
        return imem.fetch_instr( PC);
    } else { //(freezed_jump_instruction == false)
        FuncInstr fetched_instr = imem.fetch_instr( PC);
        if (fetched_instr.is_jump()) {
            freezed_instruction_ptr = new FuncInstr( fetched_instr);
            freezed_jump_instruction = true;
            instr_to_execute_before_freezed = enabled_delayed_slots - 1;
            return imem.fetch_instr( PC);
        }
        return fetched_instr;
    }
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

