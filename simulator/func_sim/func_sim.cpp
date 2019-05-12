/*
 * func_sim.cpp - extremely simple simulator
 * Copyright 2018 MIPT-MIPS
 */
 
#include "driver.h"
#include "func_sim.h"
#include <kernel/kernel.h>

#include <iostream>
#include <sstream>
#include <stdexcept>

template <typename ISA>
FuncSim<ISA>::FuncSim( Endian endian, bool log)
    : Simulator( log)
    , imem( endian)
    , kernel( Kernel::create_dummy_kernel())
    , driver( ISA::create_driver( log, this))
{
    if ( driver == nullptr)
        setup_trap_handler( "stop_on_halt");
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
        pc[i] = current_pc;
    }
    pc[instr.get_delayed_slots()] = instr.get_new_PC();
    if (delayed_slots > 0) {
        for (size_t i = 0; i < delayed_slots; ++i)
            pc[i] = pc[i + 1];
        --delayed_slots;
    }
    else {
        delayed_slots = instr.get_delayed_slots();
    }
}

static SyscallResult execute_syscall( Kernel* kernel)
{
    do try {
        return kernel->execute();
    }
    catch (const BadInputValue& e) {
        std::cerr << e.what();
    } while (true);

    return SyscallResult::UNSUPPORTED;
}

template <typename ISA>
void FuncSim<ISA>::handle_syscall( FuncInstr* instr)
{
    auto result = execute_syscall( kernel.get());
    switch ( result) {
    case SyscallResult::HALT:        instr->set_trap( Trap( Trap::HALT)); break;
    case SyscallResult::IGNORED:     instr->set_trap( Trap( Trap::SYSCALL)); break;
    case SyscallResult::UNSUPPORTED: instr->set_trap( Trap( Trap::UNSUPPORTED_SYSCALL)); break;
    default: instr->set_trap( Trap( Trap::NO_TRAP));
    }
}

template <typename ISA>
Trap FuncSim<ISA>::run( uint64 instrs_to_run)
{
    nops_in_a_row = 0;
    for ( uint64 i = 0; i < instrs_to_run; ++i) {
        auto instr = step();
        sout << instr << std::endl;
        if ( instr.trap_type() == Trap::SYSCALL)
            handle_syscall( &instr);

        auto trap = driver->handle_trap( instr);
        if ( trap != Trap::NO_TRAP)
            return trap;
    }
    return Trap(Trap::NO_TRAP);
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
void FuncSim<ISA>::setup_trap_handler( const std::string& mode)
{
    if ( !mode.empty())
        driver = Driver::construct( mode, this, false);
}
template <typename ISA>
int FuncSim<ISA>::get_exit_code() const noexcept
{
    return kernel->get_exit_code();
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

