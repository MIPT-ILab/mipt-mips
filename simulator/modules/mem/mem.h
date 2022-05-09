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

        WritePort<Instr>* wp_datapath = make_write_port<Instr>("MEMORY_2_WRITEBACK", Port::BW);
        ReadPort<Instr>* rp_datapath = make_read_port<Instr>("EXECUTE_2_MEMORY", Port::LATENCY);

        ReadPort<bool>* rp_flush = make_read_port<bool>("BRANCH_2_ALL_FLUSH", Port::LATENCY);
        ReadPort<bool>* rp_trap = make_read_port<bool>("WRITEBACK_2_ALL_FLUSH", Port::LATENCY);

        WritePort<InstructionOutput>* wp_bypass = make_write_port<InstructionOutput>("MEMORY_2_EXECUTE_BYPASS", Port::BW);

    public:
        explicit Mem( Module* parent);
        void clock( Cycle cycle);
        void set_memory( const std::shared_ptr<FuncMemory>& mem) { memory = mem; }
};


#endif // MEM_H
