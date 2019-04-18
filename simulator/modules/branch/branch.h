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
    private:
        uint64  num_branches       = 0;
        float64 num_mispredictions = 0;

        std::unique_ptr<ReadPort<Instr>> rp_datapath = nullptr;
        std::unique_ptr<WritePort<Instr>> wp_datapath = nullptr;

        std::unique_ptr<WritePort<bool>> wp_flush_all = nullptr;
        std::unique_ptr<ReadPort<bool>> rp_flush = nullptr;

        std::unique_ptr<WritePort<Target>> wp_flush_target = nullptr;
        std::unique_ptr<WritePort<BPInterface>> wp_bp_update = nullptr;

        std::unique_ptr<ReadPort<Instr>> rp_recive_datapath_from_mem = nullptr;

        std::unique_ptr<WritePort<bool>> wp_bypassing_unit_flush_notify = nullptr;
    
    public:
        explicit Branch( bool log);
        void clock( Cycle cycle);
        auto get_mispredict_rate() const { return ( num_branches != 0) 
                                                  ? num_mispredictions / num_branches * 100
                                                  : 0; }
};

#endif // BRANCH_H
