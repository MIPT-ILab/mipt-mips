
#include <cstring>
#include <iostream>
#include <vector>
#include <elf_parser.h>
#include <func_memory.h>
#include <func_instr.h>

using namespace std;

int main(int argc, char* argv[])
{
  

    const int num_of_args = 2;

    if (argc > 3)
    {
        cerr << "ERROR. too many arguments!" << endl;
        return 1;
    }
    if (argc - 1 < num_of_args)
    {
        cerr << "ERROR. not enough arguments!" << endl;
        return 1;
    }
    char* file_name = argv[1];
    char* section_name = argv[2];
    std::vector<ElfSection> sections_array;
    ElfSection::getAllElfSections(file_name, sections_array);
    FuncMemory func_mem(file_name, 32, 10, 12);
    uint64 start_addr = 0, section_size = 0;
    for (std::vector<ElfSection>::iterator it = sections_array.begin(); it != sections_array.end(); ++it)
    {

        if (!strcmp(section_name, it->name))
        {
            start_addr = it->start_addr;
            section_size = it->size;
            break;
        }
    }


    if (start_addr == 0)
    {
        cerr << "ERROR. no such section" << endl;
        return 1;
    }

    //creating memory with loaded file
    

    //reading all 32-bit words
    for (int i = 0; i < section_size / 4; ++i)
    {
        uint32 word = func_mem.read(start_addr + i * 4, 4);
        FuncInstr instr(word);
        cout << instr.Dump("    ") << "\n";
    }
    return 0;
}

