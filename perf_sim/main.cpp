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

int main( int argc, char* argv[])
{
    bool is_silent = true; // by default it's silent mode
    switch( argc)
    {
        case 3: // check arguments (silent mode)
            if ( ( argv[ 1] == nullptr) || ( atoi( argv[ 2]) < 0))
            {
                std::cerr << "ERROR: Wrong arguments!\n";
                std::exit( EXIT_FAILURE);
            }
            break;
        case 4: // check arguments (normal mode)
            if ( ( argv[ 1] == nullptr) || ( atoi( argv[ 2]) < 0) ||
                 ( strcmp( argv[ 3], "-d")))
            {
                std::cerr << "ERROR: Wrong arguments!\n";
                std::exit( EXIT_FAILURE);
            }
            is_silent = false; // disable silent mode
            break;
        default: // wrong number of arguments
            std::cerr << "ERROR: Wrong number of arguments!\n";
            std::exit( EXIT_FAILURE);
    }

    PerfMIPS p_mips( !is_silent);
    p_mips.run( argv[ 1], atoi( argv[ 2]));
    return 0;
}
