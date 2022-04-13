/**
 * decode.h - simulation of decode stage
 * Copyright 2015-2018 MIPT-MIPS
 */


#ifndef DECODE_H
#define DECODE_H

#include "bypass/data_bypass.h"

#include <func_sim/rf/rf.h>
#include <modules/core/perf_instr.h>
#include <modules/ports_instance.h>

template <typename FuncInstr>
class Decode : public Module
{
    using Register = typename FuncInstr::Register;
    using Instr = PerfInstr<FuncInstr>;
    using BypassingUnit = DataBypass<FuncInstr>;
    static constexpr const uint8 SRC_REGISTERS_NUM = 2;

public:
    explicit Decode( Module* parent);
    void clock( Cycle cycle);
    void set_RF( RF<FuncInstr>* value) { rf = value;}
    void set_wb_bandwidth( uint32 wb_bandwidth) { bypassing_unit->set_bandwidth( wb_bandwidth);}
    auto get_mispredictions_num() const { return num_mispredictions; }
    auto get_jumps_num() const { return num_jumps; }

private:
    auto read_instr( Cycle cycle) const;
    bool is_flush( Cycle cycle) const;
    static bool is_misprediction( const Instr& instr, const BPInterface& bp_data);

    uint64 num_jumps          = 0;
    uint64 num_mispredictions = 0;

    RF<FuncInstr>* rf = nullptr;
    std::unique_ptr<BypassingUnit> bypassing_unit = nullptr;

    /* Inputs */
    ReadPort<Instr>* rp_datapath = make_read_port<Instr>("FETCH_2_DECODE", Port::LATENCY);
    ReadPort<Instr>* rp_stall_datapath = make_read_port<Instr>("DECODE_2_DECODE", Port::LATENCY);
    ReadPort<bool>* rp_flush = make_read_port<bool>("BRANCH_2_ALL_FLUSH", Port::LATENCY);
    ReadPort<Instr>* rp_bypassing_unit_notify = make_read_port<Instr>("DECODE_2_BYPASSING_UNIT_NOTIFY", Port::LATENCY);
    ReadPort<bool>* rp_bypassing_unit_flush_notify = make_read_port<bool>("BRANCH_2_BYPASSING_UNIT_FLUSH_NOTIFY", Port::LATENCY);
    ReadPort<bool>* rp_flush_fetch = make_read_port<bool>("DECODE_2_FETCH_FLUSH", Port::LATENCY);
    ReadPort<bool>* rp_trap = make_read_port<bool>("WRITEBACK_2_ALL_FLUSH", Port::LATENCY);

    /* Outputs */
    WritePort<Instr>* wp_datapath = make_write_port<Instr>("DECODE_2_EXECUTE", Port::BW);
    WritePort<Instr>* wp_stall_datapath = make_write_port<Instr>("DECODE_2_DECODE", Port::BW);
    WritePort<bool>* wp_stall = make_write_port<bool>("DECODE_2_FETCH_STALL", Port::BW);
    WritePort<Instr>* wp_bypassing_unit_notify = make_write_port<Instr>("DECODE_2_BYPASSING_UNIT_NOTIFY", Port::BW);
    WritePort<BPInterface>* wp_bp_update = make_write_port<BPInterface>("DECODE_2_FETCH", Port::BW);
    WritePort<bool>* wp_flush_fetch = make_write_port<bool>("DECODE_2_FETCH_FLUSH", Port::BW);
    WritePort<Target>* wp_flush_target = make_write_port<Target>("DECODE_2_FETCH_TARGET", Port::BW);

    std::array<WritePort<BypassCommand>*, SRC_REGISTERS_NUM> wps_command =
    {
        make_write_port<BypassCommand>("DECODE_2_EXECUTE_SRC1_COMMAND", Port::BW),
        make_write_port<BypassCommand>("DECODE_2_EXECUTE_SRC2_COMMAND", Port::BW)
    };      
};


#endif // DECODE_H
