/*
 * func_sim.cpp - extremely simple MIPS simulator
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS
 */

#ifndef FUNC_SIM_H
#define FUNC_SIM_H

#include <func_sim/rf.h>

class MIPSMemory;

class MIPS
{
    private:
        RF rf;
        Addr PC = NO_VAL32;
        MIPSMemory* mem = nullptr;

        MIPS( const MIPS&) = delete;
        MIPS& operator=( const MIPS&) = delete;
    public:
        MIPS() : rf() { }
        ~MIPS();

        void init( const std::string& tr);
        std::string step();
        void run(const std::string& tr, uint32 instrs_to_run);
};

#endif

