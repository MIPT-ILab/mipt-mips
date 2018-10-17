/*
 * perf_sim.cpp - mips performance simulator
 * Copyright 2015-2018 MIPT-MIPS
 */

#include "perf_sim.h"

#include <infra/memory/elf/elf_loader.h>

#include <chrono>
#include <iostream>

template <typename ISA>
PerfSim<ISA>::PerfSim(bool log) : 
    CycleAccurateSimulator( log),
    memory( new FuncMemory),
    fetch( log),
    decode( log),
    execute( log),
    mem( log),
    writeback( log)
{
    wp_core_2_fetch_target = make_write_port<Target>("CORE_2_FETCH_TARGET", PORT_BW, PORT_FANOUT);
    rp_halt = make_read_port<bool>("WRITEBACK_2_CORE_HALT", PORT_LATENCY);

    fetch.set_memory( memory.get());
    decode.set_RF( &rf);
    mem.set_memory( memory.get());
    writeback.set_RF( &rf);

    init_ports();
}

template <typename ISA>
void PerfSim<ISA>::set_target( const Target& target)
{
    wp_core_2_fetch_target->write( target, curr_cycle);
    writeback.set_target( target);
}

template<typename ISA>
Trap PerfSim<ISA>::run( const std::string& tr, uint64 instrs_to_run)
{
    force_halt = false;
    ::load_elf_file( memory.get(), tr);

    writeback.init_checker( *memory);
    writeback.set_instrs_to_run( instrs_to_run);

    set_target( Target( memory->startPC(), 0));

    start_time = std::chrono::high_resolution_clock::now();

    while (!is_halt())
        clock();

    dump_statistics();

    return Trap::NO_TRAP;
}

template<typename ISA>
bool PerfSim<ISA>::is_halt() const
{
    return rp_halt->is_ready( curr_cycle) || force_halt;
}

template<typename ISA>
void PerfSim<ISA>::clock()
{
    clean_up_ports( curr_cycle);
    clock_tree( curr_cycle);
    curr_cycle.inc();
}

template<typename ISA>
void PerfSim<ISA>::clock_tree( Cycle cycle)
{
    writeback.clock( cycle);
    fetch.clock( cycle);
    decode.clock( cycle);
    execute.clock( cycle);
    mem.clock( cycle);
}

template<typename ISA>
void PerfSim<ISA>::dump_statistics() const
{
    auto executed_instrs = writeback.get_executed_instrs();
    auto now_time = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration<double, std::milli>(now_time - start_time).count();
    auto frequency = static_cast<double>( curr_cycle) / time; // cycles per millisecond = kHz
    auto ipc = 1.0 * executed_instrs / static_cast<double>( curr_cycle);
    auto simips = executed_instrs / time;

    std::cout << std::endl << "****************************"
              << std::endl << "instrs:     " << executed_instrs
              << std::endl << "cycles:     " << curr_cycle
              << std::endl << "IPC:        " << ipc
              << std::endl << "sim freq:   " << frequency << " kHz"
              << std::endl << "sim IPS:    " << simips    << " kips"
              << std::endl << "instr size: " << sizeof(Instr) << " bytes"
              << std::endl << "****************************"
              << std::endl;
}

#include <mips/mips.h>
#include <risc_v/risc_v.h>

template class PerfSim<MIPSI>;
template class PerfSim<MIPSII>;
template class PerfSim<MIPSIII>;
template class PerfSim<MIPSIV>;
template class PerfSim<MIPS32>;
template class PerfSim<MIPS64>;
template class PerfSim<RISCV32>;
template class PerfSim<RISCV64>;
template class PerfSim<RISCV128>;
