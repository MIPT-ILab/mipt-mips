/**
 * elf_parser.h - Header of ELF binary perser
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2013 MIPT-MIPS iLab project
 */

// protection from multi-include
#ifndef ELF_PARSER__ELF_PARSER_H
#define ELF_PARSER__ELF_PARSER_H

// Generic C++
#include <string>
#include <vector>
class Elf;

// uArchSim modules
#include <types.h>

using namespace std;

class ElfSection
{
    // You cannot use this constructor to create an object.
    // Use the static function getAllElfSections.
    ElfSection(); 
    ElfSection( const char* name, uint64 start_addr,
                uint64 size, const uint8* content);

public:
    char* name; // name of the elf section (e.g. ".text", ".data", etc)
    uint64 size; // size of the section in bytes
    uint64 start_addr; // the start address of the section
    uint8* content; // the row data of the section

    ElfSection( const  ElfSection& old);
    ElfSection& operator=( const ElfSection& that);
    
    // Use this function to extract all sections from the ELF binary file.
    // Note that the 2nd parameter is used as output.
    static void getAllElfSections( const char* elf_file_name,
                                   vector<ElfSection>& sections_array /*used as output*/);
    
    virtual ~ElfSection();
    
    string dump( string indent = "") const;
    string strByBytes() const;
    string strByWords() const;
};

#endif // #ifndef ELF_PARSER__ELF_PARSER_H
