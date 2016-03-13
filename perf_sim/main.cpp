
#include <iostream>
#include <cstdlib>

#include <perf_sim.h>

int main( int argc, char* argv[])
{
    if ( argc != 4)
    {
        std::cout << "3 arguments required: mips_exe filename and amount of instrs to run and type" << endl;
        std::exit(EXIT_FAILURE);
    }

    PerfMIPS* mips = new PerfMIPS ();
    mips->run (std::string(argv[1]), atoi(argv[2]), atoi (argv[3]));
    delete mips;

    return 0;
}
