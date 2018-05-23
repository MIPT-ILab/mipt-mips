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
#include <modules/fetch/fetch.h>
#include <modules/decode/decode.h>
#include <modules/execute/execute.h>
#include <modules/mem/mem.h>
#include <modules/writeback/writeback.h>

#include "perf_instr.h"

class BasicPerfSim : public Simulator
{
protected:
    Cycle curr_cycle = 0_Cl;
    void dump_simfreq(const std::chrono::milliseconds& time, uint64 executed_instrs, size_t instr_size) const;
public:
    explicit BasicPerfSim( bool log) : Simulator( log) { }
    ~BasicPerfSim() override { destroy_ports(); }
};

template <typename ISA>
class PerfSim : public BasicPerfSim
{
    using FuncInstr = typename ISA::FuncInstr;
    using Instr = PerfInstr<FuncInstr>;
    using Memory = typename ISA::Memory;

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

    void dump_simfreq(const std::chrono::milliseconds& time, uint64 executed_instrs) const {
        BasicPerfSim::dump_simfreq(time, executed_instrs, sizeof(Instr));
    }
public:
    PerfSim( bool log);
    void run( const std::string& tr, uint64 instrs_to_run) final;
    void set_PC( Addr value) final;

    // Rule of five
    PerfSim( const PerfSim&) = delete;
    PerfSim( PerfSim&&) = delete;
    PerfSim operator=( const PerfSim&) = delete;
    PerfSim operator=( PerfSim&&) = delete;
};

#endif
