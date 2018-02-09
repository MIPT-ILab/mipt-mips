/*
 * simulator.cpp - interface for simulator
 * Copyright 2018 MIPT-MIPS
 */
 
// Simulators
#include <func_sim/func_sim.h>
#include <core/perf_sim.h>

// ISAs
#include <mips/mips.h>

#include "simulator.h"

std::unique_ptr<Simulator>
Simulator::create_simulator(const std::string& isa, bool functional_only, bool log)
{
    if (isa == "mips" && functional_only)
        return std::make_unique<FuncSim<MIPS>>(log);
    
    if (isa == "mips" && !functional_only)
        return std::make_unique<PerfSim<MIPS>>(log);
    
    return nullptr;
}
