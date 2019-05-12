/*
 * checker.cpp - wrapper around functional simulator
 * to check state of performance simulator
 * Copyright 2015-2019 MIPT-MIPS
 */

#include "checker.h"
#include <kernel/kernel.h>

template <typename ISA>
void Checker<ISA>::init( Endian endian, const FuncMemory& outer_mem, Kernel* kernel)
{
    auto memory = FuncMemory::create_hierarchied_memory();
    sim = std::make_shared<FuncSim<ISA>>( endian);
    outer_mem.duplicate_to( memory);
    sim->set_memory( std::move( memory));
    kernel->add_replica_simulator( sim);
    kernel->add_replica_memory( memory);
    active = true;
}

template <typename ISA>
void Checker<ISA>::set_target( const Target& value)
{
    if ( active)
        sim->set_target( value);
}

template <typename ISA>
void Checker<ISA>::check( const FuncInstr& instr)
{
    if (!active)
        return;

    const auto func_dump = sim->step();

    if ( func_dump.is_same_checker(instr))
        return;
    
    std::ostringstream oss;
    oss << "Checker output: " << func_dump << std::endl
        << "PerfSim output: " << instr     << std::endl;

    throw CheckerMismatch(oss.str());
}

#include <mips/mips.h>
#include <risc_v/risc_v.h>

template class Checker<MIPSI>;
template class Checker<MIPSII>;
template class Checker<MIPSIII>;
template class Checker<MIPSIV>;
template class Checker<MIPS32>;
template class Checker<MIPS64>;
template class Checker<MARS>;
template class Checker<MARS64>;
template class Checker<RISCV32>;
template class Checker<RISCV64>;
template class Checker<RISCV128>;
