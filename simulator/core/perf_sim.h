/*
 * perf_sim.h - mips performance simulator
 * Copyright 2015-2018 MIPT-MIPS
 */

#ifndef PERF_SIM_H
#define PERF_SIM_H

#include <array>
#include <iostream>
#include <sstream>
#include <iomanip>

#include <simulator.h>
#include <infra/ports/ports.h>
#include <fetch/fetch.h>
#include <decode/decode.h>
#include <execute/execute.h>
#include <mem/mem.h>
#include <writeback/writeback.h>

#include "perf_instr.h"

template <typename ISA>
class PerfSim : public Simulator
{
    using FuncInstr = typename ISA::FuncInstr;
    using Instr = PerfInstr<FuncInstr>;
    using Memory = typename ISA::Memory;

    Cycle curr_cycle = 0_Cl;

    /* simulator units */
    std::unique_ptr<RF<ISA>> rf = nullptr;
    std::unique_ptr<Memory> memory = nullptr;
    Fetch<ISA> fetch;
    Decode<ISA> decode;
    Execute<ISA> execute;
    Mem<ISA> mem;
    Writeback<ISA> writeback;

    /* ports */
    std::unique_ptr<WritePort<Addr>> wp_core_2_fetch_target = nullptr;
    std::unique_ptr<ReadPort<bool>> rp_halt = nullptr;

public:
    explicit PerfSim( bool log);
    ~PerfSim() final { destroy_ports(); }
    void run( const std::string& tr, uint64 instrs_to_run) final;
    void set_PC( Addr value) final;

    // Rule of five
    PerfSim( const PerfSim&) = delete;
    PerfSim( PerfSim&&) = delete;
    PerfSim operator=( const PerfSim&) = delete;
    PerfSim operator=( PerfSim&&) = delete;
};

#endif
