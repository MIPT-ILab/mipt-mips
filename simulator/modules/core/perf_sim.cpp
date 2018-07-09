#include <cassert>

#include <iostream>
#include <chrono>

#include "perf_sim.h"

template <typename ISA>
PerfSim<ISA>::PerfSim(bool log) : 
    Simulator( log),
    rf( new RF<ISA>),
    fetch( log),
    decode( log),
    execute( log),
    mem( log),
    writeback( log)
{
    wp_core_2_fetch_target = make_write_port<Addr>("CORE_2_FETCH_TARGET", PORT_BW, PORT_FANOUT);
    rp_halt = make_read_port<bool>("WRITEBACK_2_CORE_HALT", PORT_LATENCY);

    init_ports();
}


template <typename ISA>
void PerfSim<ISA>::set_PC( Addr value)
{
    wp_core_2_fetch_target->write( value, curr_cycle);
    writeback.set_PC( value);
}

template<typename ISA>
void PerfSim<ISA>::run( const std::string& tr, uint64 instrs_to_run)
{
    memory = std::make_unique<Memory>( tr);
    fetch.set_memory( memory.get());
    decode.set_RF( rf.get());
    mem.set_memory( memory.get());
    writeback.set_instrs_to_run( instrs_to_run);
    writeback.set_RF( rf.get());
    writeback.init_checker( tr);

    set_PC( memory->startPC());

    auto t_start = std::chrono::high_resolution_clock::now();

    while (true)
    {
        if (rp_halt->is_ready( curr_cycle) && rp_halt->read( curr_cycle))
            break;

        writeback.clock( curr_cycle);
        fetch.clock( curr_cycle);
        decode.clock( curr_cycle);
        execute.clock( curr_cycle);
        mem.clock( curr_cycle);
        curr_cycle.inc();

        check_ports( curr_cycle);
    }

    auto t_end = std::chrono::high_resolution_clock::now();

    auto executed_instrs = writeback.get_executed_instrs();
    auto time = std::chrono::duration<double, std::milli>(t_end - t_start).count();
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
