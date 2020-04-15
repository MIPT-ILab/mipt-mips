/**
 * mem.h - Simulation of memory stage
 * Copyright 2015-2018 MIPT-MIPS
 */


#ifndef MEM_H
#define MEM_H

#include <func_sim/operation.h>
#include <modules/core/perf_instr.h>
#include <modules/ports_instance.h>

class FuncMemory;

template <typename FuncInstr>
class Mem : public Module
{
    using Instr = PerfInstr<FuncInstr>;
    using RegisterUInt = typename FuncInstr::RegisterUInt;
    using InstructionOutput = std::array< RegisterUInt, MAX_DST_NUM>;
    
    private:
        std::shared_ptr<FuncMemory> memory;

        WritePort<Instr>* wp_datapath = nullptr;
        ReadPort<Instr>* rp_datapath = nullptr;

        ReadPort<bool>* rp_flush = nullptr;
        ReadPort<bool>* rp_trap = nullptr;

        WritePort<InstructionOutput>* wp_bypass = nullptr;

    public:
        explicit Mem( Module* parent);
        void clock( Cycle cycle);
        void set_memory( const std::shared_ptr<FuncMemory>& mem) { memory = mem; }
};


#endif // MEM_H
