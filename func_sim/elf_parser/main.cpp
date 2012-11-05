// Generci C
#include <string.h>
#include <stdlib.h>
#include <assert.h>

// Generic C++
#include <iostream>

// uArchSim modules
#include <elf_parser.h>

using namespace std;

int main (int argc, char* argv[])
{
    const int num_of_args = 2;
    
    if ( argc == num_of_args && strcmp(argv[1],"--help"))
    {
        // parse and dump .reginfo section
        ElfSection section( argv[1], ".reginfo");
        cout << section.dump() << endl;
 
    } else if ( argc != num_of_args)
    {
        cerr << "ERROR: wrong number of arguments!" << endl
             << "Type \"" << argv[0] << " --help\" for usage." << endl;
        exit( EXIT_FAILURE);

    } else
    {
        cout << "This program prints content of \".reginfo\" section" << endl
             << "of ELF binary file, which name is given as only parameter." << endl
             << endl
             << "Usage: \"" << argv[0] << " <ELF binary file>\"" << endl; 
    }

    return 0;
}
