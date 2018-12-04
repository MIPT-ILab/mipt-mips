/**
 * decode.h - simulation of decode stage
 * Copyright 2015-2018 MIPT-MIPS
 */


#ifndef DECODE_H
#define DECODE_H

#include "bypass/data_bypass.h"

#include <func_sim/rf/rf.h>
#include <infra/ports/ports.h>
#include <modules/core/perf_instr.h>

template <typename ISA>
class Decode : public Log
{
    using FuncInstr = typename ISA::FuncInstr;
    using Register = typename ISA::Register;
    using Instr = PerfInstr<FuncInstr>;
    using BypassingUnit = DataBypass<ISA>;

    private:
        RF<ISA>* rf = nullptr;
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
        
        Instr read_instr( Cycle cycle);

    public:
        explicit Decode( bool log);
        void clock( Cycle cycle);
        void set_RF( RF<ISA>* value) { rf = value;}
};


#endif // DECODE_H
