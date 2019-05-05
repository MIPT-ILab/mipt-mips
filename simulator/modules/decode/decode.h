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
        uint64 num_jumps          = 0;
        uint64 num_mispredictions = 0;

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
        
        Instr read_instr( Cycle cycle, bool& from_stall);

    public:
        explicit Decode( bool log);
        void clock( Cycle cycle);
        void set_RF( RF<FuncInstr>* value) { rf = value;}
        void set_wb_bandwidth( uint32 wb_bandwidth) { bypassing_unit->set_bandwidth( wb_bandwidth);}
        auto get_mispredictions_num() const { return num_mispredictions; }
        auto get_jumps_num() const { return num_jumps; }

        bool is_misprediction( const Instr& instr, const BPInterface& bp_data) const
        {
            if ( ( instr.is_direct_jump() || instr.is_indirect_jump()) && !bp_data.is_taken)
                return true;

            // 'likely' branches, which are not in BTB, are purposely considered as mispredictions
            if ( instr.is_likely_branch() && !bp_data.is_hit)
                return true;

            return ( ( instr.is_direct_jump() || instr.is_branch())
                && bp_data.target != instr.get_decoded_target()
                && bp_data.is_taken);
        }
};


#endif // DECODE_H
