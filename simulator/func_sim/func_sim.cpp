/*
 * func_sim.cpp - extremely simple simulator
 * Copyright 2018 MIPT-MIPS
 */
 
#include "func_sim.h"
#include <kernel/kernel.h>

template <typename ISA>
FuncSim<ISA>::FuncSim( bool log) : Simulator( log), kernel( Kernel::create_dummy_kernel()) { }

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
    FuncInstr instr = imem.fetch_instr( PC);

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

static SyscallResult execute_syscall( Kernel* kernel)
{
    do try {
        return kernel->execute();
    }
    catch (const BadInputValue& e) {
        std::cerr << e.what();
    } while (true);

    return {SyscallResult::UNSUPPORTED, 0};
}

template <typename ISA>
Trap FuncSim<ISA>::handle_syscall()
{
    auto result = execute_syscall( kernel.get());
    switch ( result.type) {
    case SyscallResult::HALT:        exit_code = result.code; return Trap::HALT;
    case SyscallResult::IGNORED:     return Trap::SYSCALL;
    case SyscallResult::SUCCESS:     return Trap::NO_TRAP;
    case SyscallResult::UNSUPPORTED: return Trap::UNSUPPORTED_SYSCALL;
    default: assert( 0);
    }
    return Trap::NO_TRAP;
}

template<typename ISA>
Trap FuncSim<ISA>::step_system()
{
    const auto& instr = step();
    sout << instr << std::endl;

    if ( instr.is_syscall())
        return handle_syscall();

    return instr.trap_type();
}

template <typename ISA>
Trap FuncSim<ISA>::run( uint64 instrs_to_run)
{
    nops_in_a_row = 0;
    for ( uint32 i = 0; i < instrs_to_run; ++i) {
        auto trap = step_system();
        if ( trap == Trap::HALT)
            return Trap::HALT;
    }
    return Trap::NO_TRAP;
}

template <typename ISA>
Trap FuncSim<ISA>::run_single_step()
{
    auto trap = step_system();
    return trap == Trap::NO_TRAP ? Trap::BREAKPOINT : trap;
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

