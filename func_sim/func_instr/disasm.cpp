//Generic C
#include <cstdlib>
#include <cstring>
//Generic C++
#include <vector>
#include <string>
//uArchSim modules 
#include <elf_parser.h> 
#include <func_instr.h>

int main( int argc, char const *argv[])
{
    if ( argc != 3)
    {
        cerr << "ERROR: Invalid number of arguments.\nUsage: disasm <elf file> <section name>\n" << endl;
        return EXIT_FAILURE;
    }
        
    vector<ElfSection> sections_array;
    ElfSection::getAllElfSections( argv[1], sections_array);    
    
    bool correct_section = false;

    for ( unsigned int i = 0; i < sections_array.size(); ++i)
    {
        if ( strcmp( sections_array[i].name, argv[2]) == 0)
        {
            for ( unsigned int j = 0; j < sections_array[i].size / 4; ++j)
            {
                FuncInstr funcInstr( ( ( uint32 *)sections_array[i].content)[j]);
                cout << funcInstr.Dump("    ") << endl;
            }
            correct_section = true;
            break;
        }    
    }
    
    if ( !correct_section)
    {
        cerr << "ERROR: Invalid section name." << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
