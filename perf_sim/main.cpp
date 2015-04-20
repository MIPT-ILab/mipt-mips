#include <iostream>
#include <cstdlib>
#include <cstring>

#include <perf_sim.h>

int main( int argc, char* argv[])
{
    if ( argc < 3)
    {
        std::cout << "2 arguments required: mips_exe filename and amount of instrs to run" << endl;
        std::exit(EXIT_FAILURE);
    }

    PerfMIPS* mips = new PerfMIPS();
    if ( argc == 4 && !strcmp( "-d", argv[3]))                  // The behavior of the program depends
        mips->run(std::string(argv[1]), atoi(argv[2]), false);  // on the amount of arguments given
    else
        mips->run(std::string(argv[1]), atoi(argv[2]));
    delete mips;

    return 0;
}
