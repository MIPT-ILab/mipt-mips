/*
 * perf_sim.cpp - mips performance simulator
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
#include <bypass/data_bypass.h>
#include <bpu/bp_interface.h>
#include <bpu/bpu.h>
#include <func_sim/func_sim.h>

#include "perf_instr.h"

template <typename ISA>
class PerfSim : public Simulator
{
    using FuncInstr = typename ISA::FuncInstr;
    using Instr = PerfInstr<FuncInstr>;
    using RF = typename ISA::RF;
    using Memory = typename ISA::Memory;
private:
    uint64 executed_instrs = 0;
    Cycle last_writeback_cycle = 0_Cl; // to handle possible deadlocks

    /* simulator units */
    RF* rf = nullptr;
    Memory* memory = nullptr;
    std::unique_ptr<BaseBP> bp = nullptr;
    std::unique_ptr<DataBypass> bypassing_unit = nullptr;

    /* MIPS functional simulator for internal checks */

    FuncSim<ISA> checker;
    void check( const FuncInstr& instr);

    /* all ports */
    std::unique_ptr<WritePort<Instr>> wp_fetch_2_decode = nullptr;
    std::unique_ptr<ReadPort<Instr>> rp_fetch_2_decode = nullptr;

    std::unique_ptr<WritePort<bool>> wp_decode_2_fetch_stall = nullptr;
    std::unique_ptr<ReadPort<bool>> rp_decode_2_fetch_stall = nullptr;

    std::unique_ptr<WritePort<Instr>> wp_decode_2_decode = nullptr;
    std::unique_ptr<ReadPort<Instr>> rp_decode_2_decode = nullptr;

    std::unique_ptr<WritePort<Instr>> wp_decode_2_execute = nullptr;
    std::unique_ptr<ReadPort<Instr>> rp_decode_2_execute = nullptr;

    std::unique_ptr<WritePort<Instr>> wp_execute_2_memory = nullptr;
    std::unique_ptr<ReadPort<Instr>> rp_execute_2_memory = nullptr;

    std::unique_ptr<WritePort<Instr>> wp_memory_2_writeback = nullptr;
    std::unique_ptr<ReadPort<Instr>> rp_memory_2_writeback = nullptr;

    std::unique_ptr<WritePort<bool>> wp_memory_2_all_flush = nullptr;
    std::unique_ptr<ReadPort<bool>> rp_fetch_flush = nullptr;
    std::unique_ptr<ReadPort<bool>> rp_decode_flush = nullptr;
    std::unique_ptr<ReadPort<bool>> rp_execute_flush = nullptr;
    std::unique_ptr<ReadPort<bool>> rp_memory_flush = nullptr;

    std::unique_ptr<WritePort<Addr>> wp_memory_2_fetch_target = nullptr;
    std::unique_ptr<ReadPort<Addr>> rp_memory_2_fetch_target = nullptr;

    std::unique_ptr<WritePort<Addr>> wp_target = nullptr;
    std::unique_ptr<ReadPort<Addr>> rp_target = nullptr;

    std::unique_ptr<WritePort<Addr>> wp_core_2_fetch_target = nullptr;
    std::unique_ptr<ReadPort<Addr>> rp_core_2_fetch_target = nullptr;

    std::unique_ptr<WritePort<Addr>> wp_hold_pc = nullptr;
    std::unique_ptr<ReadPort<Addr>> rp_hold_pc = nullptr;

    std::unique_ptr<WritePort<BPInterface>> wp_memory_2_bp = nullptr;
    std::unique_ptr<ReadPort<BPInterface>> rp_memory_2_bp = nullptr;

    std::unique_ptr<WritePort<uint64>> wp_execute_2_execute_bypass = nullptr;
    std::unique_ptr<WritePort<uint64>> wp_memory_2_execute_bypass = nullptr;
    std::unique_ptr<WritePort<uint64>> wp_writeback_2_execute_bypass = nullptr;

    static constexpr const std::size_t BYPASSING_STAGES_NUM = DataBypass::RegisterStage::get_bypassing_stages_number();
    std::array<std::unique_ptr<ReadPort<uint64>>, BYPASSING_STAGES_NUM> rps_stages_2_execute_src1_bypass;
    std::array<std::unique_ptr<ReadPort<uint64>>, BYPASSING_STAGES_NUM> rps_stages_2_execute_src2_bypass;

    static constexpr const uint32 BU_NOTIFYING_STAGES_NUM = 4;
    std::unique_ptr<WritePort<Instr>> wp_decode_2_bypassing_unit = nullptr;
    std::unique_ptr<WritePort<Instr>> wp_execute_2_bypassing_unit = nullptr;
    std::unique_ptr<WritePort<Instr>> wp_memory_2_bypassing_unit = nullptr;
    std::unique_ptr<WritePort<Instr>> wp_writeback_2_bypassing_unit = nullptr;
    std::array<std::unique_ptr<ReadPort<Instr>>, BU_NOTIFYING_STAGES_NUM> rps_stages_2_bypassing_unit;

    std::unique_ptr<WritePort<DataBypass::BypassCommand>> wp_decode_2_execute_src1_command = nullptr;
    std::unique_ptr<ReadPort<DataBypass::BypassCommand>> rp_decode_2_execute_src1_command = nullptr;

    std::unique_ptr<WritePort<DataBypass::BypassCommand>> wp_decode_2_execute_src2_command = nullptr;
    std::unique_ptr<ReadPort<DataBypass::BypassCommand>> rp_decode_2_execute_src2_command = nullptr;

    /* main stages functions */
    void clock_fetch( Cycle cycle);
    void clock_decode( Cycle cycle);
    void clock_execute( Cycle cycle);
    void clock_memory( Cycle cycle);
    void clock_writeback( Cycle cycle);
    Instr read_instr( Cycle cycle);
    Addr get_PC( Cycle cycle);

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
