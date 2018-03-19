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
#include <bypass/data_bypass.h>
#include <bpu/bp_interface.h>
#include <fetch/fetch.h>
#include <writeback/writeback.h>

#include "perf_instr.h"

template <typename ISA>
class PerfSim : public Simulator
{
    using FuncInstr = typename ISA::FuncInstr;
    using Instr = PerfInstr<FuncInstr>;
    using Memory = typename ISA::Memory;
    using BypassingUnit = DataBypass<ISA>;
private:
    Cycle curr_cycle = 0_Cl;

    /* simulator units */
    std::unique_ptr<RF<ISA>> rf = nullptr;
    Memory* memory = nullptr;
    std::unique_ptr<BypassingUnit> bypassing_unit = nullptr;
    Fetch<ISA> fetch;
    Writeback<ISA> writeback;

    /* all ports */
    std::unique_ptr<ReadPort<Instr>> rp_fetch_2_decode = nullptr;

    std::unique_ptr<WritePort<bool>> wp_decode_2_fetch_stall = nullptr;
 
    std::unique_ptr<WritePort<Instr>> wp_decode_2_decode = nullptr;
    std::unique_ptr<ReadPort<Instr>> rp_decode_2_decode = nullptr;

    std::unique_ptr<WritePort<Instr>> wp_decode_2_execute = nullptr;
    std::unique_ptr<ReadPort<Instr>> rp_decode_2_execute = nullptr;

    std::unique_ptr<WritePort<Instr>> wp_execute_2_memory = nullptr;
    std::unique_ptr<ReadPort<Instr>> rp_execute_2_memory = nullptr;

    std::unique_ptr<WritePort<Instr>> wp_memory_2_writeback = nullptr;

    std::unique_ptr<WritePort<bool>> wp_memory_2_all_flush = nullptr;

    std::unique_ptr<ReadPort<bool>> rp_decode_flush = nullptr;
    std::unique_ptr<ReadPort<bool>> rp_execute_flush = nullptr;
    std::unique_ptr<ReadPort<bool>> rp_memory_flush = nullptr;

    std::unique_ptr<WritePort<Addr>> wp_memory_2_fetch_target = nullptr;

    std::unique_ptr<WritePort<Addr>> wp_core_2_fetch_target = nullptr;

    std::unique_ptr<ReadPort<bool>> rp_halt = nullptr;

    std::unique_ptr<WritePort<BPInterface>> wp_memory_2_bp = nullptr;

    std::unique_ptr<WritePort<uint64>> wp_execute_2_execute_bypass = nullptr;
    std::unique_ptr<WritePort<uint64>> wp_memory_2_execute_bypass = nullptr;

    static constexpr const uint8 SRC_REGISTERS_NUM = 2;
    static constexpr const uint8 BYPASSING_UNIT_FLUSH_NOTIFIERS_NUM = 2;

    std::array<std::array<std::unique_ptr<ReadPort<uint64>>, RegisterStage::BYPASSING_STAGES_NUMBER>, SRC_REGISTERS_NUM>
        rps_stages_2_execute_sources_bypass;

    std::array<std::unique_ptr<WritePort<typename BypassingUnit::BypassCommand>>, SRC_REGISTERS_NUM>
        wps_decode_2_execute_command;
    std::array<std::unique_ptr<ReadPort<typename BypassingUnit::BypassCommand>>, SRC_REGISTERS_NUM>
        rps_decode_2_execute_command;

    std::unique_ptr<WritePort<Instr>> wp_decode_2_bypassing_unit = nullptr;
    std::unique_ptr<ReadPort<Instr>> rp_decode_2_bypassing_unit = nullptr;

    std::unique_ptr<WritePort<Instr>> wp_execute_2_bypassing_unit_flush = nullptr;
    std::unique_ptr<WritePort<Instr>> wp_memory_2_bypassing_unit_flush = nullptr;
    
    std::array<std::unique_ptr<ReadPort<Instr>>, BYPASSING_UNIT_FLUSH_NOTIFIERS_NUM> 
        rps_stages_2_bypassing_unit_flush;

    /* main stages functions */
    void clock_decode( Cycle cycle);
    void clock_execute( Cycle cycle);
    void clock_memory( Cycle cycle);
    Instr read_instr( Cycle cycle);

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
