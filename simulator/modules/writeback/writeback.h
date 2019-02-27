/*
 * perf_sim.h - mips performance simulator
 * Copyright 2015-2018 MIPT-MIPS
 */

#ifndef WRITEBACK_H
#define WRITEBACK_H

#include <func_sim/func_sim.h>
#include <infra/exception.h>
#include <modules/core/perf_instr.h>
#include <modules/ports_instance.h>

struct CheckerMismatch final : Exception
{
    explicit CheckerMismatch(const std::string& msg)
        : Exception("Performance simulator and functional simulator executed different instructions\n"
                    "Usually it indicates a bug in performance simulator\n"
                    "The different instructions are\n"
                    , msg)
    { }
};

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

    class Checker {
        std::shared_ptr<FuncSim<ISA>> sim;
        bool active = false;
    public:
        void check( const FuncInstr& instr);
        void init( const FuncMemory& mem);
        void set_target( const Target& value);
    } checker;

    /* Simulator internals */
    RF<FuncInstr>* rf = nullptr;

    static constexpr const uint8 SRC_REGISTERS_NUM = 2;

    auto read_instructions( Cycle cycle);
    void writeback_instruction( const Instr& instr, Cycle cycle);
    void writeback_bubble( Cycle cycle);

    /* Input */
    std::unique_ptr<ReadPort<Instr>> rp_mem_datapath = nullptr;
    std::unique_ptr<ReadPort<Instr>> rp_execute_datapath = nullptr;
    std::unique_ptr<ReadPort<Instr>> rp_branch_datapath = nullptr;    

    /* Output */
    std::unique_ptr<WritePort<std::pair<RegisterUInt, RegisterUInt>>> wp_bypass = nullptr;
    std::unique_ptr<WritePort<bool>> wp_halt = nullptr;

public:
    explicit Writeback( bool log);
    void clock( Cycle cycle);
    void set_RF( RF<FuncInstr>* value) { rf = value; }
    void init_checker( const FuncMemory& mem) { checker.init( mem); }
    void set_target( const Target& value);
    void set_instrs_to_run( uint64 value) { instrs_to_run = value; }
    auto get_executed_instrs() const { return executed_instrs; }
    Addr get_next_PC() const { return next_PC; }
};

#endif
