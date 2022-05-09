/*
 * perf_sim.h - mips performance simulator
 * Copyright 2015-2018 MIPT-MIPS
 */

#ifndef WRITEBACK_H
#define WRITEBACK_H

#include "checker/checker.h"

#include <func_sim/driver/driver.h>
#include <func_sim/operation.h>
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

template <ISA I>
class Writeback final : public Module
{
    using FuncInstr = typename I::FuncInstr;
    using Instr = PerfInstr<FuncInstr>;
    using RegisterUInt = typename I::RegisterUInt;
    using InstructionOutput = std::array< RegisterUInt, MAX_DST_NUM>;

private:
    /* Instrumentation */
    uint64 instrs_to_run = 0;
    uint64 executed_instrs = 0;
    Cycle last_writeback_cycle = 0_cl;
    Addr next_PC = 0;
    const std::endian endian;
    Checker<I> checker;
    std::shared_ptr<Kernel> kernel;
    std::unique_ptr<Driver> driver;

    /* Simulator internals */
    RF<FuncInstr>* rf = nullptr;

    auto read_instructions( Cycle cycle);
    void writeback_instruction( const Writeback<I>::Instr& instr, Cycle cycle);
    void writeback_instruction_system( Writeback<I>::Instr* instr, Cycle cycle);
    void writeback_bubble( Cycle cycle);
    void set_writeback_target( const Target& value, Cycle cycle);
    void set_checker_target( const Target& value);

    /* Input */
    ReadPort<Instr>* rp_mem_datapath = make_read_port<Instr>("MEMORY_2_WRITEBACK", Port::LATENCY);
    ReadPort<Instr>* rp_execute_datapath = make_read_port<Instr>("EXECUTE_2_WRITEBACK", Port::LATENCY);
    ReadPort<Instr>* rp_branch_datapath = make_read_port<Instr>("BRANCH_2_WRITEBACK", Port::LATENCY);    
    ReadPort<bool>* rp_trap = make_read_port<bool>("WRITEBACK_2_ALL_FLUSH", Port::LATENCY);

    /* Output */
    WritePort<InstructionOutput>* wp_bypass = make_write_port<InstructionOutput>("WRITEBACK_2_EXECUTE_BYPASS", Port::BW);
    WritePort<Trap>* wp_halt = make_write_port<Trap>("WRITEBACK_2_CORE_HALT", Port::BW);
    WritePort<bool>* wp_trap = make_write_port<bool>("WRITEBACK_2_ALL_FLUSH", Port::BW);
    WritePort<Target>* wp_target = make_write_port<Target>("WRITEBACK_2_FETCH_TARGET", Port::BW);

public:
    Writeback( Module* parent, std::endian endian);

    // Keep dtors in the same translation unit
    ~Writeback() final;
    Writeback( const Writeback&) = delete;
    Writeback( Writeback&&) = delete;
    Writeback& operator=( const Writeback&) = delete;
    Writeback& operator=( Writeback&&) = delete;

    void clock( Cycle cycle);
    void set_RF( RF<FuncInstr>* value) { rf = value; }
    void disable_checker() { checker.disable(); }
    void set_target( const Target& value, Cycle cycle);
    void set_instrs_to_run( uint64 value) { instrs_to_run = value; }
    auto get_executed_instrs() const { return executed_instrs; }
    Addr get_next_PC() const { return next_PC; }
    int get_exit_code() const noexcept;
    void set_kernel( const std::shared_ptr<Kernel>& k, std::string_view isa);
    void set_driver( std::unique_ptr<Driver> d) { driver = std::move( d); }
    void enable_driver_hooks();
};

#endif
