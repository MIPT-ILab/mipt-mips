#include "func_sim.h"
#include <cstdlib>
#include <string>

int main( int argc, char **argv)
{
    if ( argc != 3) 
        return EXIT_FAILURE; 
    int instr_to_run = atoi( argv[2]);
    std::string name( argv[1]);

    MIPS* mips = new MIPS;
    mips->run( name, instr_to_run);

    return EXIT_SUCCESS; 
}
