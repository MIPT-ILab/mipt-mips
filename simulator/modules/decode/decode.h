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
class Decode : public Log
{
    using Register = typename FuncInstr::Register;
    using Instr = PerfInstr<FuncInstr>;
    using BypassingUnit = DataBypass<FuncInstr>;

    private:
        RF<FuncInstr>* rf = nullptr;
        std::unique_ptr<BypassingUnit> bypassing_unit = nullptr;

        std::unique_ptr<WritePort<Instr>> wp_datapath = nullptr;
        std::unique_ptr<ReadPort<Instr>> rp_datapath = nullptr;

        std::unique_ptr<WritePort<Instr>> wp_stall_datapath = nullptr;
        std::unique_ptr<ReadPort<Instr>> rp_stall_datapath = nullptr;

        std::unique_ptr<WritePort<bool>> wp_stall = nullptr;

        std::unique_ptr<ReadPort<bool>> rp_flush = nullptr;
        
        static constexpr const uint8 SRC_REGISTERS_NUM = 2;

        std::array<std::unique_ptr<WritePort<BypassCommand<Register>>>, SRC_REGISTERS_NUM> wps_command;

        std::unique_ptr<WritePort<Instr>> wp_bypassing_unit_notify = nullptr;
        std::unique_ptr<ReadPort<Instr>> rp_bypassing_unit_notify = nullptr;

        std::unique_ptr<ReadPort<bool>> rp_bypassing_unit_flush_notify = nullptr;

        /* ports that are needed to handle flush at decode stage */
        std::unique_ptr<WritePort<bool>> wp_flush_fetch = nullptr;
        std::unique_ptr<ReadPort<bool>> rp_flush_fetch = nullptr;
        std::unique_ptr<WritePort<Target>> wp_flush_target = nullptr;
        std::unique_ptr<WritePort<BPInterface>> wp_bp_update = nullptr;
        
        Instr read_instr( Cycle cycle);

    public:
        explicit Decode( bool log);
        void clock( Cycle cycle);
        void set_RF( RF<FuncInstr>* value) { rf = value;}
        void set_wb_bandwidth( uint32 wb_bandwidth) { bypassing_unit->set_bandwidth( wb_bandwidth);}
};


#endif // DECODE_H
