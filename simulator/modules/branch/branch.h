/**
 * Yauheni Sharamed SHaramed.EI@phystech.edu
 * branch.h - Simulation of branch mispredict detection stage
 * Copyright 2015-2018 MIPT-MIPS
 */

#ifndef BRANCH_H
#define BRANCH_H

#include <func_sim/operation.h>
#include <modules/core/perf_instr.h>
#include <modules/ports_instance.h>

class FuncMemory;

template <typename FuncInstr>
class Branch : public Module
{
    using Instr = PerfInstr<FuncInstr>;
    using RegisterUInt = typename FuncInstr::RegisterUInt;
    using InstructionOutput = std::array< RegisterUInt, MAX_DST_NUM>;

    private:
        uint64 num_mispredictions = 0;
        uint64 num_jumps          = 0;

        ReadPort<Instr>* rp_datapath = nullptr;
        WritePort<Instr>* wp_datapath = nullptr;

        WritePort<bool>* wp_flush_all = nullptr;
        ReadPort<bool>* rp_flush = nullptr;
        ReadPort<bool>* rp_trap = nullptr;

        WritePort<Target>* wp_flush_target = nullptr;
        WritePort<BPInterface>* wp_bp_update = nullptr;

        ReadPort<Instr>* rp_recive_datapath_from_mem = nullptr;

        WritePort<InstructionOutput>* wp_bypass = nullptr;

        WritePort<bool>* wp_bypassing_unit_flush_notify = nullptr;

    public:
        explicit Branch( Module* parent);
        void clock( Cycle cycle);
        auto get_mispredictions_num() const { return num_mispredictions; }
        auto get_jumps_num() const { return num_jumps; }

        static bool is_misprediction( const Instr& instr, const BPInterface& bp_data)
        {
            if ( !bp_data.is_hit)
                if ( ( instr.is_common_branch() && instr.is_taken())
                || ( instr.is_likely_branch() && !instr.is_taken())
                || instr.is_indirect_jump())
                    return true;

            if ( bp_data.is_hit && ( bp_data.is_taken != instr.is_taken()))
                return true;

            return ( bp_data.is_taken || instr.is_indirect_jump()) && ( bp_data.target != instr.get_new_PC());
        }
};

#endif // BRANCH_H
