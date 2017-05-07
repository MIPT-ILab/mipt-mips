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
#include <list>

// uArchSim modules
#include <infra/types.h>

struct Elf;

class ElfSection
{
    // You cannot use this constructor to create an object.
    // Use the static function getAllElfSections.
    ElfSection() = delete;

    ElfSection& operator= (const ElfSection&) = delete;
public:
    const std::string name; // name of the elf section (e.g. ".text", ".data", etc)
    const uint32 size; // size of the section in bytes
    const Addr start_addr; // the start address of the section
    uint8* const content; // the row data of the section

    ElfSection( const  ElfSection& that);

    // Use this function to extract all sections from the ELF binary file.
    // Note that the 2nd parameter is used as output.
    static void getAllElfSections( const char* elf_file_name,
                                   std::list<ElfSection>& sections_array /*used as output*/);

    ElfSection( const char* name, Addr start_addr,
	    Addr size, const uint8* content_that);

    virtual ~ElfSection();

    std::string dump( std::string indent = "") const;
    std::string strByBytes() const;
    std::string strByWords() const;
};

#endif // #ifndef ELF_PARSER__ELF_PARSER_H
