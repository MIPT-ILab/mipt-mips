/*
 * perf_sim.h - mips performance simulator
 * Copyright 2015-2018 MIPT-MIPS
 */

#ifndef WRITEBACK_H
#define WRITEBACK_H

#include "checker/checker.h"

#include <infra/exception.h>
#include <modules/core/perf_instr.h>
#include <modules/ports_instance.h>

struct Deadlock final : Exception
{
    explicit Deadlock(const std::string& msg)
        : Exception("No instructions executed in recent cycles\n"
                    "Usually it indicates a bug in performance simulator\n"
                    , msg)
    { }
};

template <typename ISA>
class Writeback : public Log
{
    using FuncInstr = typename ISA::FuncInstr;
    using Instr = PerfInstr<FuncInstr>;
    using RegisterUInt = typename ISA::RegisterUInt;
private:
    /* Instrumentation */
    uint64 instrs_to_run = 0;
    uint64 executed_instrs = 0;
    Cycle last_writeback_cycle = 0_cl;
    Addr next_PC = 0;
    const Endian endian;
    Checker<ISA> checker;

    /* Simulator internals */
    RF<FuncInstr>* rf = nullptr;

    auto read_instructions( Cycle cycle);
    void writeback_instruction( const Instr& instr, Cycle cycle);
    void writeback_bubble( Cycle cycle);

    /* Input */
    std::unique_ptr<ReadPort<Instr>> rp_mem_datapath = nullptr;
    std::unique_ptr<ReadPort<Instr>> rp_execute_datapath = nullptr;
    std::unique_ptr<ReadPort<Instr>> rp_branch_datapath = nullptr;    
    std::unique_ptr<ReadPort<bool>> rp_trap = nullptr;

    /* Output */
    std::unique_ptr<WritePort<std::pair<RegisterUInt, RegisterUInt>>> wp_bypass = nullptr;
    std::unique_ptr<WritePort<bool>> wp_halt = nullptr;
    std::unique_ptr<WritePort<bool>> wp_trap = nullptr;
    std::unique_ptr<WritePort<Target>> wp_target = nullptr;

public:
    explicit Writeback( Endian endian, bool log);
    void clock( Cycle cycle);
    void set_RF( RF<FuncInstr>* value) { rf = value; }
    void init_checker( const FuncMemory& mem) { checker.init( endian, mem); }
    void set_target( const Target& value, Cycle cycle);
    void set_instrs_to_run( uint64 value) { instrs_to_run = value; }
    auto get_executed_instrs() const { return executed_instrs; }
    Addr get_next_PC() const { return next_PC; }
};

#endif
