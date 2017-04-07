/**
 * main.cpp
 * Start point of the scalar MIPS CPU simulator.
 * MIPT-MIPS Assignment 4.
 * Ladin Oleg.
 */

/* Generic C. */
#include <cstdlib>
#include <cstring>

/* Generic C++ */
#include <memory>

/* Simulator modules. */
#include "perf_sim.h"
#include "config.h"

int main( int argc, char** argv)
{
    /* Analysing and handling of inserted arguments */
    Config handler;
    handler.handleArgs( argc, argv);

    /* running simulation */
    PerfMIPS p_mips( handler.disassemblyOn());
    p_mips.run( handler.binaryFilename(), handler.numSteps());
    return 0;
}
