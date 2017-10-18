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
#include <memory>

// uArchSim modules
#include <infra/types.h>

class ElfSection
{
    // You cannot use this constructor to create an object.
    // Use the static function getAllElfSections.
    ElfSection() = delete;
    ElfSection& operator= ( const ElfSection&) = delete;
    ElfSection& operator= ( ElfSection&&) = delete;

    const std::string name; // name of the elf section (e.g. ".text", ".data", etc)
    const size_t size; // size of the section in bytes
    const Addr start_addr; // the start address of the section
    std::unique_ptr<uint8[]> content; // the row data of the section
public:
    ElfSection( const std::string& name, Addr start_addr, Addr size, std::unique_ptr<uint8[]>&& ptr)
        : name( name), size( size)
        , start_addr( start_addr)
        , content( ptr) { }
    
    virtual ~ElfSection() = default;
    
    // copy and move ctors
    ElfSection( const ElfSection& that);
    ElfSection( ElfSection&& that) = default;

    // Use this function to extract all sections from the ELF binary file.
    static std::list<ElfSection> getAllElfSections( const std::string& elf_file_name);

    std::string dump( const std::string& indent) const;
    std::string strByBytes() const;
    std::string strByWords() const;

    const std::string& get_name()   const { return name; }
    const Addr   get_size()         const { return size; }
    const Addr   get_start_addr()   const { return addr; }
    uint8 operator[](size_t offset) const { return content.get()[offset]; }
};

#endif // #ifndef ELF_PARSER__ELF_PARSER_H
