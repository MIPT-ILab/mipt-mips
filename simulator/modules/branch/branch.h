/**
 * Yauheni Sharamed SHaramed.EI@phystech.edu
 * branch.h - Simulation of branch mispredict detection stage
 * Copyright 2015-2018 MIPT-MIPS
 */

#ifndef BRANCH_H
#define BRANCH_H

#include <modules/core/perf_instr.h>
#include <modules/ports_instance.h>

class FuncMemory;

template <typename FuncInstr>
class Branch : public Log
{    
        using Instr = PerfInstr<FuncInstr>;
        using RegisterUInt = typename FuncInstr::RegisterUInt;
        using InstructionOutput = std::pair< RegisterUInt, RegisterUInt>;

    private:
        float64 num_mispredictions = 0;

        std::unique_ptr<ReadPort<Instr>> rp_datapath = nullptr;
        std::unique_ptr<WritePort<Instr>> wp_datapath = nullptr;

        std::unique_ptr<WritePort<bool>> wp_flush_all = nullptr;
        std::unique_ptr<ReadPort<bool>> rp_flush = nullptr;

        std::unique_ptr<WritePort<Target>> wp_flush_target = nullptr;
        std::unique_ptr<WritePort<BPInterface>> wp_bp_update = nullptr;

        std::unique_ptr<ReadPort<Instr>> rp_recive_datapath_from_mem = nullptr;

        std::unique_ptr<WritePort<InstructionOutput>> wp_bypass = nullptr;

        std::unique_ptr<WritePort<bool>> wp_bypassing_unit_flush_notify = nullptr;

        static constexpr const uint8 SRC_REGISTERS_NUM = 2;
    public:
        explicit Branch( bool log);
        void clock( Cycle cycle);
        auto get_mispredictions_num() const { return num_mispredictions; }

        template<typename Instr>
        bool is_misprediction( const Instr& instr, const BPInterface& bp_data) const
        {
            bool is_misprediction = false;

            if ( instr.is_indirect_jump())
                is_misprediction |= bp_data.target != instr.get_new_PC();

            if ( instr.is_common_branch())
                is_misprediction |= instr.is_taken();

            if ( instr.is_likely_branch())
                is_misprediction |= !instr.is_taken() || instr.get_decoded_target() != instr.get_new_PC();

            return is_misprediction;
        }
};

#endif // BRANCH_H
