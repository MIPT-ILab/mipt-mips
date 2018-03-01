/*
 * perf_sim.h - mips performance simulator
 * Copyright 2015-2018 MIPT-MIPS
 */

#ifndef WRITEBACK_H
#define WRITEBACK_H

#include <infra/ports/ports.h>
#include <func_sim/func_sim.h>
#include <core/perf_instr.h>

template <typename ISA>
class Writeback : public Log
{
    using FuncInstr = typename ISA::FuncInstr;
    using Instr = PerfInstr<FuncInstr>;
private:
    /* Instrumentation */
    uint64 instrs_to_run = 0;
    uint64 executed_instrs = 0;
    Cycle last_writeback_cycle = 0_Cl;
    FuncSim<ISA> checker;
    void check( const FuncInstr& instr);

    /* Simulator internals */
    RF<ISA>* rf = nullptr;

    static constexpr const std::size_t SRC_REGISTERS_NUM = 2;

    /* Input */
    std::unique_ptr<ReadPort<Instr>> rp_datapath = nullptr;

    /* Output */
    std::unique_ptr<WritePort<uint64>> wp_bypass = nullptr;
    std::unique_ptr<WritePort<bool>> wp_halt = nullptr;

public:
    explicit Writeback( bool log);
    void clock( Cycle cycle);
    void set_RF( RF<ISA>* value) { rf = value; }
    void set_PC( Addr value) { checker.set_PC( value); }
    void set_instrs_to_run( uint64 value) { instrs_to_run = value; }
    void init_checker( const std::string& tr) { checker.init( tr); }
    auto get_executed_instrs() const { return executed_instrs; }
};

#endif
