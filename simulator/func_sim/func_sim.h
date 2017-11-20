/*
 * func_sim.cpp - extremely simple MIPS simulator
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS
 */

#ifndef FUNC_SIM_H
#define FUNC_SIM_H

#include <memory>
#include <string>

#include <infra/types.h>
#include <infra/log.h>


class MIPSMemory;
class RF;

class MIPS : public Log
{
    private:
        std::unique_ptr<RF> rf;
        Addr PC = NO_VAL32;
        MIPSMemory* mem = nullptr;
    
    public:
        explicit MIPS( bool log = false);
        ~MIPS() final;

        // Rule of five
        MIPS( const MIPS&) = delete;
        MIPS( MIPS&&) = delete;
        MIPS& operator=( const MIPS&) = delete;
        MIPS& operator=( MIPS&&) = delete;

        void init( const std::string& tr);
        std::string step();
        void run(const std::string& tr, uint32 instrs_to_run);
};

#endif

