/*
 * perf_sim.cpp - mips performance simulator
 * Copyright 2015-2018 MIPT-MIPS
 */

#include "perf_sim.h"
#include <func_sim/instr_memory.h>
#include <memory/elf/elf_loader.h>

#include <chrono>
#include <iostream>

namespace config {
    static const AliasedValue<std::string> units_to_log = { "l", "logs", "nothing", "print logs for modules"};
    static const Switch topology_dump = { "tdump", "module topology dump into topology.json" };
} // namespace config

template <ISA I>
PerfSim<I>::PerfSim( std::endian endian, std::string_view isa)
    : CycleAccurateSimulator( isa)
    , endian( endian)
    , fetch( this), decode( this), execute( this), mem( this), branch( this), writeback( this, endian)
{
    decode.set_RF( &rf);
    writeback.set_RF( &rf);
    writeback.set_driver( I::create_driver( this));

    set_writeback_bandwidth( Port::BW);

    init_portmap();
    enable_logging( config::units_to_log);
    topology_dumping( config::topology_dump, "topology.json");
}

template <ISA I>
void PerfSim<I>::set_memory( std::shared_ptr<FuncMemory> m)
{
    memory = m;
    auto imemory = std::make_unique<InstrMemoryCached<I>>( endian);
    imemory->set_memory( m);
    fetch.set_memory( std::move( imemory));
    mem.set_memory( m);
}

template <ISA I>
void PerfSim<I>::set_target( const Target& target)
{
    writeback.set_target( target, curr_cycle);
}

template<ISA I>
Addr PerfSim<I>::get_pc() const
{
    return writeback.get_next_PC();
}

template<ISA I>
Trap PerfSim<I>::run( uint64 instrs_to_run)
{
    current_trap = Trap( Trap::NO_TRAP);

    writeback.set_instrs_to_run( instrs_to_run);

    start_time = std::chrono::high_resolution_clock::now();

    while (current_trap == Trap::NO_TRAP)
        clock();

    dump_statistics();

    return current_trap;
}

template<ISA I>
void PerfSim<I>::clock()
{
    clock_tree( curr_cycle);
    curr_cycle.inc();
}

template<ISA I>
void PerfSim<I>::clock_tree( Cycle cycle)
{
    fetch.clock( cycle);
    decode.clock( cycle);
    execute.clock( cycle);
    mem.clock( cycle);
    branch.clock( cycle);
    writeback.clock( cycle);
    if ( rp_halt->is_ready( cycle))
        current_trap = rp_halt->read( cycle);
    sout << "******************\n";
}

auto get_rate( int total, float64 piece)
{
    return total != 0 ? ( piece / total * 100) : 0;
}

template<ISA I>
void PerfSim<I>::dump_statistics() const
{
    auto executed_instrs = writeback.get_executed_instrs();
    auto now_time = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration<double, std::milli>(now_time - start_time).count();
    auto frequency = double{ curr_cycle} / time; // cycles per millisecond = kHz
    auto ipc = 1.0 * executed_instrs / double{ curr_cycle};
    auto simips = executed_instrs / time;
    auto decode_mispredict_rate = 1.0 * get_rate( decode.get_jumps_num(), decode.get_mispredictions_num());
    auto branch_mispredict_rate = 1.0 * get_rate( branch.get_jumps_num(), branch.get_mispredictions_num());
    
    std::cout << std::endl << "****************************"
              << std::endl << "instrs:     " << executed_instrs
              << std::endl << "cycles:     " << curr_cycle
              << std::endl << "IPC:        " << ipc
              << std::endl << "sim freq:   " << frequency << " kHz"
              << std::endl << "sim IPS:    " << simips    << " kips"
              << std::endl << "instr size: " << sizeof(Instr) << " bytes"
              << std::endl << "mispredict: detected on decode stage - " << decode_mispredict_rate << "%"
              << std::endl << "            detected on branch stage - " << branch_mispredict_rate << "%"
              << std::endl << "****************************"
              << std::endl;
}

template <ISA I>
uint64 PerfSim<I>::read_gdb_register( size_t regno) const
{
    if ( regno == Register::get_gdb_pc_index())
        return get_pc();

    return read_register( Register::from_gdb_index( regno));
}

template <ISA I>
void PerfSim<I>::write_gdb_register( size_t regno, uint64 value)
{
    if ( regno == Register::get_gdb_pc_index())
        set_pc( value);
    else
        write_register( Register::from_gdb_index( regno), value);
}

#include <mips/mips.h>
#include <risc_v/risc_v.h>

template class PerfSim<MIPSI>;
template class PerfSim<MIPSII>;
template class PerfSim<MIPSIII>;
template class PerfSim<MIPSIV>;
template class PerfSim<MIPS32>;
template class PerfSim<MIPS64>;
template class PerfSim<MARS>;
template class PerfSim<MARS64>;
template class PerfSim<RISCV32>;
template class PerfSim<RISCV64>;
template class PerfSim<RISCV128>;
