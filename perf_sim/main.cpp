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
    PerfMIPS p_mips( handler.disassembly_on);
    p_mips.run( handler.binary_filename, handler.num_steps);
    return 0;
}
