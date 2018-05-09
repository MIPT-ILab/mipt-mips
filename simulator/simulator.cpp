/*
 * simulator.cpp - interface for simulator
 * Copyright 2018 MIPT-MIPS
 */
 
// Simulators
#include <func_sim/func_sim.h>
#include <modules/core/perf_sim.h>

// ISAs
#include <mips/mips.h>
#include <risc_v/risc_v.h>

#include "simulator.h"

std::unique_ptr<Simulator>
Simulator::create_simulator( const std::string& isa, bool functional_only, bool log)
{
    if ( isa == "mips32") {
        if (functional_only)
            return std::make_unique<FuncSim<MIPS32>>( log);
        return std::make_unique<PerfSim<MIPS32>>( log);
    }
    else if ( isa == "mips64") {
        if (functional_only)
            return std::make_unique<FuncSim<MIPS64>>( log);
        return std::make_unique<PerfSim<MIPS64>>( log);
    }

    return nullptr;
}

