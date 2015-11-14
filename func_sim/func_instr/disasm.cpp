//Generic C
#include <cstring>

//Generic C++
#include <iostream>
#include <vector>

//uArchSim modules
#include <elf_parser.h>
#include <func_memory.h>
#include <func_instr.h>

using namespace std;

int main ( int argc, char* argv[])
{
    const int num_of_args = 2;

    if ( argc - 1 > num_of_args)
    {
        cerr << "ERROR: too many arguments!" << endl
             << "One argument is required, the name of an exe"
             << "cutable file." << endl;
    } else if ( argc - 1 < num_of_args)
    {
        cerr << "ERROR: too few arguments!" << endl
             << "Only two arguments are required, the name of an exe"
             << "cutable file and name of section." << endl;
    }

    //getting all sections of file
    const char* file_name = argv[1];
    std::vector<ElfSection> sections_array;
    ElfSection::getAllElfSections( file_name, sections_array);
    uint64 start_addr = 0, section_size = 0;
    for ( auto it = sections_array.begin(); it != sections_array.end(); ++it)
    {
        const char* section_name = argv[2];
        if ( !strcmp( section_name, it->name))
        {
            start_addr = it->start_addr;
            section_size = it->size;
            break;
        }
    }

    if ( start_addr == 0)
    {
        cerr << "ERROR: section with this name doesn\'t exist." << endl;
    }

    //creating memory with loaded file
    FuncMemory func_mem( file_name, 32, 10, 12);

    //reading all 32-bit words
    for ( unsigned long i = 0; i < section_size/4; ++i)
    {
        uint32 cur_word = func_mem.read( start_addr + i*4, 4);
        FuncInstr cur_instr( cur_word);
        cout << cur_instr.Dump("    ") << endl;
    }
    return 0;
}
