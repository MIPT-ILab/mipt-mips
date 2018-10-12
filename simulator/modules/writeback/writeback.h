/*
 * perf_sim.h - mips performance simulator
 * Copyright 2015-2018 MIPT-MIPS
 */

#ifndef WRITEBACK_H
#define WRITEBACK_H

#include <func_sim/func_sim.h>
#include <infra/exception.h>
#include <infra/ports/ports.h>
#include <modules/core/perf_instr.h>

struct CheckerMismatch final : Exception
{
    explicit CheckerMismatch(const std::string& msg)
        : Exception("Checker mismatch", msg)
    { }
};

struct Deadlock final : Exception
{
    explicit Deadlock(const std::string& msg)
        : Exception("Deadlock was detected", msg)
    { }
};

template <typename ISA>
class Writeback : public Log
{
    using FuncInstr = typename ISA::FuncInstr;
    using Instr = PerfInstr<FuncInstr>;
    using RegisterUInt = typename ISA::RegisterUInt;
    using Memory = typename ISA::Memory;
private:
    /* Instrumentation */
    uint64 instrs_to_run = 0;
    uint64 executed_instrs = 0;
    Cycle last_writeback_cycle = 0_cl;
    FuncSim<ISA> checker;
    void check( const FuncInstr& instr);

    /* Simulator internals */
    RF<ISA>* rf = nullptr;

    static constexpr const uint8 SRC_REGISTERS_NUM = 2;

    /* Input */
    std::unique_ptr<ReadPort<Instr>> rp_mem_datapath = nullptr;
    std::unique_ptr<ReadPort<Instr>> rp_execute_datapath = nullptr;

    /* Output */
    std::unique_ptr<WritePort<std::pair<RegisterUInt, RegisterUInt>>> wp_bypass = nullptr;
    std::unique_ptr<WritePort<bool>> wp_halt = nullptr;

public:
    explicit Writeback( bool log);
    void clock( Cycle cycle);
    void set_RF( RF<ISA>* value) { rf = value; }
    void init_checker( const Memory& mem);
    void set_target( const Target& value) { checker.set_target( value); }
    void set_instrs_to_run( uint64 value) { instrs_to_run = value; }
    auto get_executed_instrs() const { return executed_instrs; }
};

#endif
