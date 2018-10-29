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
    using Register = typename ISA::Register;
    using RegisterUInt = typename ISA::RegisterUInt;

    explicit PerfSim( bool log);
    ~PerfSim() override { destroy_ports(); }
    Trap run( uint64 instrs_to_run) final;
    void set_target( const Target& target) final;
    void set_memory( std::shared_ptr<FuncMemory> memory) final;
    void set_kernel( std::shared_ptr<Kernel> kernel) final { this->kernel = kernel; }
    void clock() final;
    void halt() final { force_halt = true; }
    void init_checker() final { writeback.init_checker( *memory); }

    size_t sizeof_register() const final { return bytewidth<RegisterUInt>; }

    uint64 read_cpu_register( uint8 regno) const final {
        return static_cast<uint64>( rf.read( Register::from_cpu_index( regno)));
    }

    void write_cpu_register( uint8 regno, uint64 value) final {
        rf.write( Register::from_cpu_index( regno), static_cast<RegisterUInt>( value));
    }

    uint64 read_cause_register() const {
        return static_cast<uint64>( rf.read( Register::cause));
    }

    void write_cause_register( uint64 value) {
        rf.write( Register::cause, static_cast<RegisterUInt>( value));
    }

    // Rule of five
    PerfSim( const PerfSim&) = delete;
    PerfSim( PerfSim&&) = delete;
    PerfSim operator=( const PerfSim&) = delete;
    PerfSim operator=( PerfSim&&) = delete;
private:
    using FuncInstr = typename ISA::FuncInstr;
    using Instr = PerfInstr<FuncInstr>;

    Cycle curr_cycle = 0_cl;
    decltype( std::chrono::high_resolution_clock::now()) start_time = {};
    bool force_halt = false;    

    /* simulator units */
    RF<ISA> rf;
    std::shared_ptr<FuncMemory> memory;
    std::weak_ptr<Kernel> kernel;

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
