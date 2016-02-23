/*
 * disasm.cpp - mips disassembler implemented using elf_parser
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS 
 */


#include <cstring>
#include <cstdlib>

#include <iostream>
#include <iomanip>

#include <func_instr.h>
#include <elf_parser.h>

int main( int argc, char* argv[])
{
    if ( argc != 3)
    {
        std::cout << "2 arguments required: mips_exe filename and section name" << endl;
        std::exit(EXIT_FAILURE);
    }

    std::vector<ElfSection> section;
    ElfSection::getAllElfSections( argv[1], section);
    size_t i;
    for ( i = 0; i < section.size(); i++)
        if ( !strcmp( section[i].name, argv[2]))
            break;

    if ( i == section.size())
    {
        std::cout << "Section was not found" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    bool skip_mode = false;
    size_t j = 0;
    do
    {
        uint32 content = ((uint32*) section[i].content)[j];
        if (content == 0x0) {
            ++j;
            if (!skip_mode){
                std::cout << "        ..." << std::endl;
                skip_mode = true;
            }
            continue;
        }
        else {
            skip_mode = false;
        }

        FuncInstr instr((( uint32*) section[i].content)[j]);
        std::cout << std::hex << std::setfill( '0')
                  << "0x" << std::setw( 8)
		  << ( section[i].start_addr + ( j * 4))
	          << '\t' << instr << std::dec << std::endl;
	    ++j;
    } while (j < section[i].size / 4);

    return 0;
}

