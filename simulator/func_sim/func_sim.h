/*
 * func_sim.cpp - extremely simple FuncSim simulator
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS
 */

#ifndef FUNC_SIM_H
#define FUNC_SIM_H

#include <memory>
#include <string>

#include <infra/types.h>
#include <infra/log.h>

#include <mips/mips_instr.h>

class MIPSMemory;
class RF;

class FuncSim : public Log
{
    private:
        std::unique_ptr<RF> rf;
        Addr PC = NO_VAL32;
        MIPSMemory* mem = nullptr;

    public:
        explicit FuncSim( bool log = false);
        ~FuncSim() final;

        // Rule of five
        FuncSim( const FuncSim&) = delete;
        FuncSim( FuncSim&&) = delete;
        FuncSim operator=( const FuncSim&) = delete;
        FuncSim operator=( FuncSim&&) = delete;

        void init( const std::string& tr);
        FuncInstr step();
        void run(const std::string& tr, uint32 instrs_to_run);
};

#endif

