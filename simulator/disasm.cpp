/*
 * disasm.cpp - mips disassembler implemented using elf_parser
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS
 */

#include <iostream>
#include <iomanip>

#include <infra/elf_parser/elf_parser.h>
#include <mips/mips_instr.h>

int main( int argc, char* argv[])
{
    if ( argc != 3)
    {
        std::cout << "2 arguments required: mips_exe filename and section name" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    const std::string section_name( argv[2]);
    std::list<ElfSection> sections;
    ElfSection::getAllElfSections( argv[1], sections);
    for ( const auto section : sections) {
        if ( section.name != section_name)
            continue;

        bool skip_mode = false;
        size_t j = 0;
        do
        {
            uint32 content = reinterpret_cast<uint32*>(section.content)[j];
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

            FuncInstr instr(content);
            std::cout << std::hex << std::setfill( '0')
                      << "0x" << std::setw( 8)
                      << ( section.start_addr + ( j * 4))
                      << '\t' << instr << std::dec << std::endl;
            ++j;
       } while (j < section.size / 4);
       return 0;
    }

    std::cerr << "Section was not found" << std::endl;
    std::exit(EXIT_FAILURE);

    return -1;
}

