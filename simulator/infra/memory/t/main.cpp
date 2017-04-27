// Generci C
#include <stdlib.h>

// Generic C++
#include <iostream>

// uArchSim modules
#include "../memory.h"

int main (int argc, char* argv[])
{
    // Only one argumnt is required, the name of an executable file
    const int num_of_args = 1;

    if ( argc - 1 == num_of_args)
    {
        // set the name of the executable file
        const char * file_name = argv[1];

        // create the functiona memory
        Memory func_mem( file_name, 32, 10, 12);

        // print content of the memory
        std::cout << func_mem.dump() << std::endl;

    } else if ( argc - 1 > num_of_args)
    {
        std::cerr << "ERROR: too many arguments!" << std::endl
             << "Only one argument is required, the name of an executable file." << std::endl;
        std::exit( EXIT_FAILURE);

    } else
    {
        std::cerr << "ERROR: too few arguments!" << std::endl
             << "One argument is required, the name of an executable file." << std::endl;
        std::exit( EXIT_FAILURE);
    }

    return 0;
}
