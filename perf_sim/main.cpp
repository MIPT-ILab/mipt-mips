/**
 * main.cpp
 * Start point of the scalar MIPS CPU simulator.
 * MIPT-MIPS Assignment 4.
 * Ladin Oleg.
 */

/* Generic C. */
#include <cstdlib>
#include <cstring>

/* Simulator modules. */
#include <perf_sim.h>

using namespace std;

int main( int argc, char* argv[])
{
    bool is_silent = true; // by default it's silent mode
    switch( argc)
    {
        case 3: // check arguments (silent mode)
            if ( ( argv[ 1] == nullptr) || ( atoi( argv[ 2]) < 0))
            {
                cerr << "ERROR: Wrong arguments!\n";
                exit( EXIT_FAILURE);
            }
            break;
        case 4: // check arguments (normal mode)
            if ( ( argv[ 1] == nullptr) || ( atoi( argv[ 2]) < 0) ||
                 ( strcmp( argv[ 3], "-d")))
            {
                cerr << "ERROR: Wrong arguments!\n";
                exit( EXIT_FAILURE);
            }
            is_silent = false; // disable silent mode
            break;
        default: // wrong number of arguments
            cerr << "ERROR: Wrong number of arguments!\n";
            exit( EXIT_FAILURE);
    }

    PerfMIPS* p_mips = new PerfMIPS;
    p_mips->run( argv[ 1], atoi( argv[ 2]), is_silent);
    delete p_mips;
    return 0;
}
