/**
 * elf_parser.h - Header of ELF binary perser
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// protection from multi-include
#ifndef ELF_PARSER__ELF_PARSER_H
#define ELF_PARSER__ELF_PARSER_H

// Generic C++
#include <string>
class Elf;

// uArchSim modules
#include <types.h>

using namespace std;

class ElfSection
{
    char* name; // name of the elf section (e.g. ".text", ".data", etc)
    uint8* content; // the row data of the section
    uint64 size; // size of the section in bytes
    uint64 start_addr; // the start address of the section
    
    static void extractSectionParams( Elf* elf, const char* section_name,
                                      uint64& offset, uint64& size,
                                      uint64& start_addr);
    
    // You could not create the object
    // using this default constructor
    ElfSection(){}

public:

    ElfSection( const char* elf_file_name, const char* section_name);
    virtual ~ElfSection();
    
    
    uint64 read( uint64 addr, short num_of_bytes = 4) const;
    bool   isInside( uint64 addr, short num_of_bytes = 1) const;
    uint64 startAddr() const;
    
    string dump( string indent = "") const;
    string strByBytes() const;
    string strByWords() const;
};

#endif // #ifndef ELF_PARSER__ELF_PARSER_H
