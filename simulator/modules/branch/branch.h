/**
 * Yauheni Sharamed SHaramed.EI@phystech.edu
 * branch.h - Simulation of branch mispredict detection stage
 * Copyright 2015-2018 MIPT-MIPS
 */


#ifndef BRANCH_H
#define BRANCH_H

#include <infra/ports/ports.h>
#include <modules/core/perf_instr.h>

class FuncMemory;

template <typename ISA>
class Branch : public Log
{
    using FuncInstr = typename ISA::FuncInstr;
    using Instr = PerfInstr<FuncInstr>;
    using RegisterUInt = typename ISA::RegisterUInt;
    using InstructionOutput = std::pair< RegisterUInt, RegisterUInt>;
    
    private:
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
};


#endif // BRANCH_H
