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
        std::shared_ptr<FuncMemory> memory;

        std::unique_ptr<WritePort<Instr>> wp_datapath = nullptr;
        std::unique_ptr<ReadPort<Instr>> rp_datapath = nullptr;

        std::unique_ptr<ReadPort<bool>> rp_flush = nullptr;

        std::unique_ptr<WritePort<InstructionOutput>> wp_bypass = nullptr;

        static constexpr const uint8 SRC_REGISTERS_NUM = 2;

    public:
        explicit Mem( bool log);
        void clock( Cycle cycle);
        void set_memory( std::shared_ptr<FuncMemory> mem) { memory = std::move( mem); }
};


#endif // MEM_H
