/*
 * perf_sim.cpp - mips performance simulator
 * Copyright 2015 MIPT-MIPS
 */

#ifndef PERF_SIM_H
#define PERF_SIM_H

#include <iostream>
#include <sstream>
#include <iomanip>

#include <simulator.h>
#include <infra/ports/ports.h>
#include <bpu/bpu.h>
#include <func_sim/func_sim.h>

template <typename ISA>
class PerfSim : public Simulator
{
    using FuncInstr = typename ISA::FuncInstr;
    using RF = typename ISA::RF;
    using Memory = typename ISA::Memory;
private:
    uint64 executed_instrs = 0;
    Cycle last_writeback_cycle = 0_Cl; // to handle possible deadlocks

    // the struture of data sent from fetch to decode stage 
    struct IfIdData {
        BPUpdateInfo bp_update;           // the structure of data to update BP unit
        uint32 raw = NO_VAL32;            // fetched instruction code
    };

    /* simulator units */
    RF* rf = nullptr;
    Addr PC = NO_VAL32;
    Addr new_PC = NO_VAL32;
    Memory* memory = nullptr;
    std::unique_ptr<BaseBP> bp = nullptr;
    BPUpdateInfo bp_update;

    /* MIPS functional simulator for internal checks */
    
    FuncSim<ISA> checker;
    void check( const FuncInstr& instr);

    /* all ports */
    std::unique_ptr<WritePort<IfIdData>> wp_fetch_2_decode = nullptr;
    std::unique_ptr<ReadPort<IfIdData>> rp_fetch_2_decode = nullptr;
    std::unique_ptr<WritePort<bool>> wp_decode_2_fetch_stall = nullptr;
    std::unique_ptr<ReadPort<bool>> rp_decode_2_fetch_stall = nullptr;

    std::unique_ptr<WritePort<FuncInstr>> wp_decode_2_decode = nullptr;
    std::unique_ptr<ReadPort<FuncInstr>> rp_decode_2_decode = nullptr;

    std::unique_ptr<WritePort<FuncInstr>> wp_decode_2_execute = nullptr;
    std::unique_ptr<ReadPort<FuncInstr>> rp_decode_2_execute = nullptr;

    std::unique_ptr<WritePort<FuncInstr>> wp_execute_2_memory = nullptr;
    std::unique_ptr<ReadPort<FuncInstr>> rp_execute_2_memory = nullptr;


    std::unique_ptr<WritePort<FuncInstr>> wp_memory_2_writeback = nullptr;
    std::unique_ptr<ReadPort<FuncInstr>> rp_memory_2_writeback = nullptr;

    std::unique_ptr<WritePort<bool>> wp_memory_2_all_flush = nullptr;
    std::unique_ptr<ReadPort<bool>> rp_fetch_flush = nullptr;
    std::unique_ptr<ReadPort<bool>> rp_decode_flush = nullptr;
    std::unique_ptr<ReadPort<bool>> rp_execute_flush = nullptr;
    std::unique_ptr<ReadPort<bool>> rp_memory_flush = nullptr;

    std::unique_ptr<WritePort<Addr>> wp_memory_2_fetch_target = nullptr;
    std::unique_ptr<ReadPort<Addr>> rp_memory_2_fetch_target = nullptr;

    std::unique_ptr<WritePort<BPUpdateInfo>> wp_memory_2_fetch = nullptr;
    std::unique_ptr<ReadPort<BPUpdateInfo>> rp_memory_2_fetch = nullptr;

    /* main stages functions */
    void clock_fetch( Cycle cycle);
    void clock_decode( Cycle cycle);
    void clock_execute( Cycle cycle);
    void clock_memory( Cycle cycle);
    void clock_writeback( Cycle cycle);
    FuncInstr read_instr( Cycle cycle);

public:
    explicit PerfSim( bool log);
    ~PerfSim() final { destroy_ports(); }
    void run( const std::string& tr, uint64 instrs_to_run) final;

    // Rule of five
    PerfSim( const PerfSim&) = delete;
    PerfSim( PerfSim&&) = delete;
    PerfSim operator=( const PerfSim&) = delete;
    PerfSim operator=( PerfSim&&) = delete;
};

#endif
