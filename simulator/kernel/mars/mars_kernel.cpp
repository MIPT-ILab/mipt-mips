/*
 * mars_kernel.cpp - MARS syscalls
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#include "mars_kernel.h"

#include <infra/macro.h>
#include <kernel/base_kernel.h>
#include <memory/elf/elf_loader.h>

#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class MARSKernel : public BaseKernel {
    void print_integer();
    void read_integer();
    void print_character();
    void read_character();
    void print_string();
    void read_string();
    void open_file();
    void read_from_file();
    void write_to_file();
    void close_file();

    std::fstream* find_user_file_by_descriptor( uint64 descriptor);
    std::istream* find_in_file_by_descriptor( uint64 descriptor);
    std::ostream* find_out_file_by_descriptor( uint64 descriptor);
    void io_failure();

    std::istream& instream;
    std::ostream& outstream;
    std::ostream& errstream;

    std::unordered_map<uint64, std::fstream> files;
    static const constexpr uint64 first_user_descriptor = 3;
    uint64 next_descriptor = first_user_descriptor;

    void connect_riscv_handler();
    void connect_mars_handler();

public:
    Trap execute() final;
    void connect_exception_handler() final;

    MARSKernel( std::istream& instream, std::ostream& outstream, std::ostream& errstream)
      : BaseKernel( errstream), instream( instream), outstream( outstream), errstream( errstream) {}
};

std::shared_ptr<Kernel> create_mars_kernel( std::istream& instream, std::ostream& outstream, std::ostream& errstream) {
    return std::make_shared<MARSKernel>( instream, outstream, errstream);
}

static const constexpr uint8 v0 = 2;
static const constexpr uint8 a0 = 4;
static const constexpr uint8 a1 = 5;
static const constexpr uint8 a2 = 6;

Trap MARSKernel::execute () {
    uint64 syscall_code = sim->read_cpu_register( v0);
    switch (syscall_code) {
        case 1: print_integer(); break;
        case 4: print_string (); break;
        case 5: read_integer (); break;
        case 8: read_string(); break;
        case 10: exit_code = 0; return Trap( Trap::HALT);
        case 11: print_character(); break;
        case 12: read_character(); break;
        case 13: open_file(); break;
        case 14: read_from_file(); break;
        case 15: write_to_file(); break;
        case 16: close_file(); break;
        case 17: exit_code = sim->read_cpu_register( a0); return Trap( Trap::HALT);
        default: return Trap( Trap::UNSUPPORTED_SYSCALL);
    }
    return Trap( Trap::NO_TRAP);
}

void MARSKernel::print_integer() {
    auto value = narrow_cast<int64>( sim->read_cpu_register( a0));
    outstream << value;
}

void MARSKernel::read_integer() {
    std::string input;
    instream >> input;
    uint64 value = 0;
    size_t pos = 0;

    try {
        value = narrow_cast<uint64>( std::stoll( input, &pos, 0));
    }
    catch ( const std::invalid_argument& e) {
        throw BadInputValue( std::string("No conversion could be performed: ") + e.what());
    }
    catch ( const std::out_of_range& e) {
        throw BadInputValue( std::string( "Out of range value: ") + e.what());
    }

    if ( pos != input.length())
        throw BadInputValue( "Unknown error.");
    sim->write_cpu_register( v0, value);
}

void MARSKernel::print_character() {
    outstream << narrow_cast<char>( sim->read_cpu_register( a0));
}

void MARSKernel::read_character() {
    std::string input;
    instream >> input;
    if (input.length() != 1)
        throw BadInputValue( "More than one character is entered");
    sim->write_cpu_register( v0, narrow_cast<uint64>( input.at(0)));
}

void MARSKernel::print_string() {
    outstream << mem->read_string( sim->read_cpu_register( a0));
}

void MARSKernel::read_string() {
    uint64 buffer_ptr = sim->read_cpu_register( a0);
    uint64 chars_to_read = sim->read_cpu_register( a1);

    std::string input;
    instream >> input;

    mem->write_string_limited( input, buffer_ptr, chars_to_read);
}

static auto get_openmode( uint64 value) {
    switch ( value) {
    case 0: return std::ios_base::in  | std::ios_base::binary;
    case 1: return std::ios_base::out | std::ios_base::binary;
    case 9: return std::ios_base::out | std::ios_base::binary | std::ios_base::app;
    default: return std::ios_base::openmode{};
    }
}

void MARSKernel::io_failure()
{
    sim->write_cpu_register( v0, all_ones<uint64>());
}

void MARSKernel::open_file() {
    uint64 filename_ptr = sim->read_cpu_register( a0);
    uint64 flags = sim->read_cpu_register( a1);
    auto filename = mem->read_string( filename_ptr);
    std::fstream file( filename, get_openmode( flags));
    if ( !file.is_open()) {
        io_failure();
        return;
    }

    files.emplace( next_descriptor, std::move( file));
    sim->write_cpu_register( v0, next_descriptor);
    ++next_descriptor;
}

void MARSKernel::close_file() {
    uint64 descriptor = sim->read_cpu_register( a0);
    if ( descriptor < first_user_descriptor)
        return;

    auto it = files.find( descriptor);
    if ( it == files.end())
        return;

    files.erase( it);
}

std::fstream* MARSKernel::find_user_file_by_descriptor(uint64 descriptor) {
    auto it = files.find( descriptor);
    return it == files.end() ? nullptr : &(it->second);
}

std::istream* MARSKernel::find_in_file_by_descriptor(uint64 descriptor) {
    switch ( descriptor) {
    case 0:  return &instream;
    case 1:  /* fallthrough */
    case 2:  return nullptr;
    default: return find_user_file_by_descriptor( descriptor);
    }
}

std::ostream* MARSKernel::find_out_file_by_descriptor(uint64 descriptor) {
    switch ( descriptor) {
    case 0:  return nullptr;
    case 1:  return &outstream;
    case 2:  return &errstream;
    default: return find_user_file_by_descriptor( descriptor);
    }
}

void MARSKernel::write_to_file() {
    uint64 descriptor = sim->read_cpu_register( a0);
    uint64 buffer_ptr = sim->read_cpu_register( a1);
    uint64 chars_to_write = sim->read_cpu_register( a2);
    auto* file = find_out_file_by_descriptor( descriptor);
    if (file == nullptr) {
        io_failure();
        return;
    }

    auto data = mem->read_string_limited( buffer_ptr, chars_to_write);
    auto current_pos = file->tellp();
    file->write( data.c_str(), data.size());
    assert( !file->bad()); // FIXME(pikryukov): How to test the opposite?
    sim->write_cpu_register( v0, file->tellp() - current_pos);
}

void MARSKernel::read_from_file() {
    uint64 descriptor = sim->read_cpu_register( a0);
    uint64 buffer_ptr = sim->read_cpu_register( a1);
    uint64 chars_to_read = sim->read_cpu_register( a2);
    auto* file = find_in_file_by_descriptor( descriptor);
    if (file == nullptr) {
        io_failure();
        return;
    }

    std::vector<char> buffer( chars_to_read);
    auto current_pos = file->tellg();
    file->read( buffer.data(), chars_to_read);
    assert( !file->bad()); // FIXME(pikryukov): How to test the opposite?
    auto chars_read = file->tellg() - current_pos;
    sim->write_cpu_register( v0, chars_read);
    mem->memcpy_host_to_guest( buffer_ptr, byte_cast( buffer.data()), chars_to_read);
}

void MARSKernel::connect_riscv_handler()
{
    constexpr Addr TRAP_VECTOR = 0x8'000'0000;
    sim->write_csr_register( "stvec", TRAP_VECTOR);
    auto pc = trap_vector_address<Addr>( TRAP_VECTOR);
    ElfLoader elf_loader( KERNEL_IMAGES "riscv32.bin");
    elf_loader.load_to( mem.get(), pc - elf_loader.get_text_section_addr());
}

void MARSKernel::connect_mars_handler()
{
    ElfLoader elf_loader( KERNEL_IMAGES "mars32_le.bin");
    elf_loader.load_to( mem.get(), 0x8'0000'0180 - elf_loader.get_text_section_addr());
}

static bool is_mips_le( std::string_view isa)
{
    static std::unordered_set<std::string_view> isas =
        { "mars", "mars64", "mips32le", "mips32", "mips64", "mips64le" };
    return isas.count( isa) > 0;
}

void MARSKernel::connect_exception_handler()
{
    auto isa = sim->get_isa();
    if ( isa == "riscv32" || isa == "riscv64")
        connect_riscv_handler();
    else if ( is_mips_le( isa))
        connect_mars_handler();
    else 
        throw UnsupportedISA( isa);
}
