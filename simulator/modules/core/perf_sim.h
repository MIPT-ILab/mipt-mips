/*
 * perf_sim.h - mips performance simulator
 * Copyright 2015-2018 MIPT-MIPS
 */

#ifndef PERF_SIM_H
#define PERF_SIM_H

#include "perf_instr.h"

#include <infra/ports/ports.h>
#include <modules/branch/branch.h>
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
    Trap run_single_step() final { return Trap::HALT; }
    Trap run_until_trap( uint64 /* instrs_to_run */) final { return Trap::HALT; }
    void set_target( const Target& target) final;
    void set_memory( std::shared_ptr<FuncMemory> memory) final;
    void set_kernel( std::shared_ptr<Kernel> k) final { kernel = std::move( k); }
    void clock() final;
    void halt() final { force_halt = true; }
    void init_checker() final { writeback.init_checker( *memory); }

    size_t sizeof_register() const final { return bytewidth<RegisterUInt>; }
    Addr get_pc() const final;
    
    uint64 read_cpu_register( uint8 regno) const final { return read_register( Register::from_cpu_index( regno)); }
    uint64 read_gdb_register( uint8 regno) const final;
    uint64 read_cause_register() const { return read_register( Register::cause()); }

    void write_cpu_register( uint8 regno, uint64 value) final { write_register( Register::from_cpu_index( regno), value); }
    void write_gdb_register( uint8 regno, uint64 value) final;
    void write_cause_register( uint64 value) { write_register( Register::cause(), value); }

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
    std::shared_ptr<Kernel> kernel;

    Fetch<ISA> fetch;
    Decode<ISA> decode;
    Execute<ISA> execute;
    Mem<ISA> mem;
    Branch<FuncInstr> branch;
    Writeback<ISA> writeback;

    /* ports */
    std::unique_ptr<WritePort<Target>> wp_core_2_fetch_target = nullptr;
    std::unique_ptr<ReadPort<bool>> rp_halt = nullptr;

    void clock_tree( Cycle cycle);
    void dump_statistics() const;
    bool is_halt() const;

    uint64 read_register( Register index) const { return narrow_cast<uint64>( rf.read( index)); }
    void write_register( Register index, uint64 value) { return rf.write( index, narrow_cast<RegisterUInt>( value)); }
};

#endif
