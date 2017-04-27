/**
 * main.cpp - entry point of the scalar MIPS CPU simulator
 * @author Ladin Oleg
 * Copyright 2017 MIPT-MIPS
 */

/* Generic C. */
#include <cstdlib>
#include <cstring>

/* Generic C++ */
#include <memory>

/* Simulator modules. */
#include <common/config.h>

#include <func_sim/func_sim.h>
#include "perf_sim.h"

namespace config {
    static Value<bool> disassembly_on = { "disassembly,d", false, "print disassembly"};
    static Value<std::string> binary_filename = { "binary,b", "", "input binary file", true};
    static Value<uint64> num_steps = { "numsteps,n", 1, "number of instructions to run", true};
    static Value<bool> functional_only = { "functional-only,f", false, "run functional simulation only"};
}

int main( int argc, char** argv)
{
    /* Analysing and handling of inserted arguments */
    config::handleArgs( argc, argv);

    /* running simulation */
    if ( !config::functional_only)
    {
        PerfMIPS p_mips( config::disassembly_on);
        p_mips.run( config::binary_filename,
                    config::num_steps);
    }
    else
    {
        MIPS mips;
        mips.run( config::binary_filename, config::num_steps);
    }

    return 0;
}
