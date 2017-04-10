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
#include "perf_sim.h"
#include <func_sim/func_sim.h>
#include "config.h"

int main( int argc, char** argv)
{
    /* Analysing and handling of inserted arguments */
    Config handler;
    handler.handleArgs( argc, argv);

    /* running simulation */
    if ( !handler.functional_only)
    {
        PerfMIPS p_mips( handler.disassembly_on);
        p_mips.run( handler.binary_filename, handler.num_steps);
    }
    else
    {
        MIPS mips;
        mips.run( handler.binary_filename, handler.num_steps);
    }

    return 0;
}
