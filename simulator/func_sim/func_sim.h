/*
 * func_sim.cpp - extremely simple simulator
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS
 */

#ifndef FUNC_SIM_H
#define FUNC_SIM_H

#include "instr_memory.h"
#include "rf/rf.h"

#include <infra/exception.h>

#include <simulator.h>

#include <memory>
#include <string>

struct BearingLost final : Exception
{
    BearingLost() : Exception("Bearing lost", "10 nops in a row") { }
};

template <typename ISA>
class FuncSim : public Simulator
{
    using FuncInstr = typename ISA::FuncInstr;

    private:
        RF<ISA> rf;
        Addr PC = NO_VAL32;
        uint64 sequence_id = 0;
        std::unique_ptr<FuncMemory> mem = nullptr;
        InstrMemoryCached<FuncInstr> imem;

        uint64 nops_in_a_row = 0;
        void update_and_check_nop_counter( const FuncInstr& instr);

    public:
        explicit FuncSim( bool log = false);

        void init( const std::string& tr);
        void init( const FuncMemory& other_mem);
        FuncInstr step();
        Trap run(const std::string& tr, uint64 instrs_to_run) final;
        void set_target(const Target& target) final {
            PC = target.address;
            sequence_id = target.sequence_id;
        }
};

#endif
