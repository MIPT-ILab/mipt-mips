// Generci C
#include <string.h>
#include <stdlib.h>
#include <assert.h>

// Generic C++
#include <iostream>

// uArchSim modules
#include <elf_parser.h>

using namespace std;

int main ( int argc, char* argv[])
{
    const int num_of_args = 2;
    
    if ( argc == num_of_args && strcmp( argv[ 1],"--help"))
    {
        // extract all ELF sections into the section_array variable
        vector<ElfSection> sections_array;
        ElfSection::getAllElfSections( argv[1], sections_array);
        
        // print the information about each section
        for ( int i = 0; i < sections_array.size(); ++i)
	        cout << sections_array[ i].dump() << endl;
 
    } else if ( argc != num_of_args)
    {
        cerr << "ERROR: wrong number of arguments!" << endl
             << "Type \"" << argv[ 0] << " --help\" for usage." << endl;
        exit( EXIT_FAILURE);

    } else
    {
        cout << "This program prints content of all the sections" << endl
             << "of the ELF binary file, which name is given as only parameter." << endl
             << endl
             << "Usage: \"" << argv[ 0] << " <ELF binary file>\"" << endl; 
    }

    return 0;
}
