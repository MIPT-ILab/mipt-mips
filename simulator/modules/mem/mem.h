/**
 * mem.h - Simulation of memory stage
 * Copyright 2015-2018 MIPT-MIPS
 */


#ifndef MEM_H
#define MEM_H

#include <infra/ports/ports.h>
#include <modules/core/perf_instr.h>

class FuncMemory;

template <typename ISA>
class Mem : public Log
{
    using FuncInstr = typename ISA::FuncInstr;
    using Instr = PerfInstr<FuncInstr>;
    using RegisterUInt = typename ISA::RegisterUInt;
    using InstructionOutput = std::pair< RegisterUInt, RegisterUInt>;

    private:
        FuncMemory* memory = nullptr;

        std::unique_ptr<WritePort<Instr>> wp_datapath = nullptr;
        std::unique_ptr<ReadPort<Instr>> rp_datapath = nullptr;

        std::unique_ptr<WritePort<bool>> wp_flush_all = nullptr;
        std::unique_ptr<ReadPort<bool>> rp_flush = nullptr;

        std::unique_ptr<WritePort<Target>> wp_flush_target = nullptr;
        std::unique_ptr<WritePort<BPInterface>> wp_bp_update = nullptr;

        static constexpr const uint8 SRC_REGISTERS_NUM = 2;

        std::unique_ptr<WritePort<InstructionOutput>> wp_bypass = nullptr;

        std::unique_ptr<WritePort<bool>> wp_bypassing_unit_flush_notify = nullptr;
    
    public:
        explicit Mem( bool log);
        void clock( Cycle cycle);
        void set_memory( FuncMemory* mem) { memory = mem; }
};


#endif // MEM_H
