/*
 * fetch.h - simulator of fetch unit
 * Copyright 2015-2018 MIPT-MIPS
 */

#ifndef FETCH_H
#define FETCH_H

#include <infra/ports/ports.h>
#include <core/perf_instr.h>
#include <bpu/bpu.h>
 
template <typename ISA>
class Fetch : public Log
{
    using FuncInstr = typename ISA::FuncInstr;
    using Instr = PerfInstr<FuncInstr>;
    using Memory = typename ISA::Memory;
private:
    Memory* memory = nullptr;
    std::unique_ptr<BaseBP> bp = nullptr;

    /* Input signals */
    std::unique_ptr<ReadPort<bool>> rp_decode_2_fetch_stall = nullptr;
    std::unique_ptr<ReadPort<bool>> rp_fetch_flush = nullptr;
    
    /* Input signals - BP */
    std::unique_ptr<ReadPort<BPInterface>> rp_memory_2_bp = nullptr;
    
    /* Input signals - PC values */
    std::unique_ptr<ReadPort<Addr>> rp_memory_2_fetch_target = nullptr;
    std::unique_ptr<ReadPort<Addr>> rp_core_2_fetch_target = nullptr;
    std::unique_ptr<ReadPort<Addr>> rp_hold_pc = nullptr;
    std::unique_ptr<ReadPort<Addr>> rp_target = nullptr;

    /* Outputs */
    std::unique_ptr<WritePort<Instr>> wp_fetch_2_decode = nullptr;
    std::unique_ptr<WritePort<Addr>> wp_hold_pc = nullptr;
    std::unique_ptr<WritePort<Addr>> wp_target = nullptr;

    Addr get_PC( Cycle cycle);
    
public:
    explicit Fetch( bool log);
    void clock( Cycle cycle);
    void set_memory( Memory* mem) { memory = mem; }
};

#endif
    