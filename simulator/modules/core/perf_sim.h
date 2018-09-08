/*
 * perf_sim.h - mips performance simulator
 * Copyright 2015-2018 MIPT-MIPS
 */

#ifndef PERF_SIM_H
#define PERF_SIM_H

#include "perf_instr.h"

#include <infra/ports/ports.h>
#include <modules/decode/decode.h>
#include <modules/execute/execute.h>
#include <modules/fetch/fetch.h>
#include <modules/mem/mem.h>
#include <modules/writeback/writeback.h>
#include <simulator.h>

#include <array>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

template <typename ISA>
class PerfSim : public CycleAccurateSimulator
{
public:
    explicit PerfSim( bool log);
    ~PerfSim() final { destroy_ports(); }
    void run( const std::string& tr, uint64 instrs_to_run) final;
    void set_target( const Target& target) final;
    void clock() final;
    void halt() final { force_halt = true; }

    // Rule of five
    PerfSim( const PerfSim&) = delete;
    PerfSim( PerfSim&&) = delete;
    PerfSim operator=( const PerfSim&) = delete;
    PerfSim operator=( PerfSim&&) = delete;

    /* GDB interfaces stubs */
    void gdb_load( const std::string &tr) final { (void)tr; }
    void gdb_prepare() final {}
    void gdb_resume(int steps) final { (void)steps; }
    int gdb_mem_read( unsigned int addr, unsigned char *buf, int length) final {
        (void)addr; (void)buf; (void)length; return 0;
    }
    int gdb_mem_write( unsigned int addr, const unsigned char *buf, int length) final {
        (void)addr; (void)buf; (void)length; return 0;
    };

private:
    using FuncInstr = typename ISA::FuncInstr;
    using Instr = PerfInstr<FuncInstr>;
    using Memory = typename ISA::Memory;

    Cycle curr_cycle = 0_cl;
    decltype( std::chrono::high_resolution_clock::now()) start_time = {};
    bool force_halt = false;    

    /* simulator units */
    RF<ISA> rf;
    std::unique_ptr<Memory> memory = nullptr;
    Fetch<ISA> fetch;
    Decode<ISA> decode;
    Execute<ISA> execute;
    Mem<ISA> mem;
    Writeback<ISA> writeback;

    /* ports */
    std::unique_ptr<WritePort<Target>> wp_core_2_fetch_target = nullptr;
    std::unique_ptr<ReadPort<bool>> rp_halt = nullptr;

    void clock_tree( Cycle cycle);
    void dump_statistics() const;
    bool is_halt() const;
};

#endif
