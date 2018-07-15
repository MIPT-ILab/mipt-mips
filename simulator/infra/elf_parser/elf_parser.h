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

struct InvalidElfFile : std::exception
{
    const std::string filename;
    const std::string message;
    InvalidElfFile(std::string name, std::string msg)
        : filename(std::move(name))
        , message(std::move(msg))
        {}
    virtual char const * what() const noexcept {
        using namespace std::literals::string_literals;
        return (filename + " is not a valid ELF file:" + message).c_str();
    }
};

class ElfSection
{
    const std::string name; // name of the elf section (e.g. ".text", ".data", etc)
    const size_t size; // size of the section in bytes
    const Addr start_addr; // the start address of the section
    std::unique_ptr<uint8[]> content; // the row data of the section
public:
    ElfSection( std::string name, Addr start_addr, Addr size, std::unique_ptr<uint8[]> ptr)
        : name( std::move( name)), size( size), start_addr( start_addr), content( std::move(ptr))
    { }
    virtual ~ElfSection() = default;

    // No assignment
    ElfSection& operator= ( const ElfSection&) = delete;
    ElfSection& operator= ( ElfSection&&) = delete;

    // copy and move ctors
    ElfSection( const ElfSection& that);
    ElfSection( ElfSection&& that) = default;

    // Use this function to extract all sections from the ELF binary file.
    static std::list<ElfSection> getAllElfSections( const std::string& elf_file_name);

    std::string dump( const std::string& indent) const;
    std::string strByBytes() const;
    std::string strByWords() const;

    const std::string& get_name() const { return name; }
    Addr  get_size()              const { return size; }
    Addr  get_start_addr()        const { return start_addr; }
    uint8 get_byte(size_t offset) const { return content.get()[offset]; }
};

#endif // #ifndef ELF_PARSER__ELF_PARSER_H
