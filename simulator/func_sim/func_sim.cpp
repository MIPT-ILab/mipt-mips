/*
 * func_sim.cpp - extremely simple simulator
 * Copyright 2018 MIPT-MIPS
 */

#include "driver/driver.h"
#include "func_sim.h"
#include <kernel/kernel.h>

#include <iostream>
#include <sstream>
#include <stdexcept>

template <typename ISA>
FuncSim<ISA>::FuncSim( std::endian endian, bool log, std::string_view isa)
    : BasicFuncSim( isa)
    , imem( endian)
    , driver( ISA::create_driver( this))
{
    if ( log)
        sout.enable();
}

template <typename ISA>
void FuncSim<ISA>::set_memory( std::shared_ptr<FuncMemory> m)
{
    mem = std::move( m);
    imem.set_memory( mem);
}

template <typename ISA>
void FuncSim<ISA>::update_and_check_nop_counter( const FuncInstr& instr)
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
    FuncInstr instr = imem.fetch_instr( pc[0]);
    instr.set_sequence_id(sequence_id);
    sequence_id++;
    rf.read_sources( &instr);
    instr.execute();
    mem->load_store( &instr);
    rf.write_dst( instr);
    update_pc( instr);
    update_and_check_nop_counter( instr);
    return instr;
}

template <typename ISA>
void FuncSim<ISA>::update_pc( const FuncInstr& instr)
{
    auto current_pc = pc[0];
    for (size_t i = 0; i < instr.get_delayed_slots(); ++i) {
        current_pc += 4;
        pc.at(i) = current_pc;
    }
    pc.at(instr.get_delayed_slots()) = instr.get_new_PC();
    if (delayed_slots > 0) {
        for (size_t i = 0; i < delayed_slots; ++i)
            pc.at(i) = pc.at(i + 1);
        --delayed_slots;
    }
    else {
        delayed_slots = instr.get_delayed_slots();
    }
}

template <typename ISA>
Trap FuncSim<ISA>::driver_step( const Operation& instr)
{
    return driver->handle_trap( instr);
}

template <typename ISA>
Trap FuncSim<ISA>::run( uint64 instrs_to_run)
{
    nops_in_a_row = 0;
    for ( uint64 i = 0; i < instrs_to_run; ++i) {
        auto instr = step();
        sout << instr << std::endl;
        kernel->handle_instruction( &instr);
        auto result_trap = driver_step( instr);
        if ( result_trap != Trap::NO_TRAP)
            return result_trap;
    }
    return Trap(Trap::BREAKPOINT);
}

template <typename ISA>
uint64 FuncSim<ISA>::read_gdb_register( size_t regno) const
{
    if ( regno == Register::get_gdb_pc_index())
        return get_pc();

    return read_register( Register::from_gdb_index( regno));
}

template <typename ISA>
void FuncSim<ISA>::write_gdb_register( size_t regno, uint64 value)
{
    if ( regno == Register::get_gdb_pc_index())
        set_pc( value);
    else
        write_register( Register::from_gdb_index( regno), value);
}

template <typename ISA>
int FuncSim<ISA>::get_exit_code() const noexcept
{
    return kernel->get_exit_code();
}

template <typename ISA>
void FuncSim<ISA>::enable_driver_hooks()
{
    driver = Driver::create_hooked_driver( driver.get());
}

#include <mips/mips.h>
#include <risc_v/risc_v.h>

template class FuncSim<MIPSI>;
template class FuncSim<MIPSII>;
template class FuncSim<MIPSIII>;
template class FuncSim<MIPSIV>;
template class FuncSim<MIPS32>;
template class FuncSim<MIPS64>;
template class FuncSim<MARS>;
template class FuncSim<MARS64>;
template class FuncSim<RISCV32>;
template class FuncSim<RISCV64>;
template class FuncSim<RISCV128>;

