/*
 * mars_kernel.cpp - MARS syscalls
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#include "mars_kernel.h"

#include <fstream>
#include <string>

class MARSKernel : public Kernel {
    void print_integer();
    void read_integer();
    void print_character();
    void read_character();
    void print_string();
    void read_string();

    std::istream& instream;
    std::ostream& outstream;
    std::ostream& errstream;

public:
    SyscallResult execute() final;

    MARSKernel( std::istream& instream, std::ostream& outstream, std::ostream& errstream)
      : instream( instream), outstream( outstream), errstream( errstream) {}
};

std::shared_ptr<Kernel> create_mars_kernel( std::istream& instream, std::ostream& outstream, std::ostream& errstream) {
    return std::make_shared<MARSKernel>( instream, outstream, errstream);
}

static const constexpr uint8 v0 = 2;
static const constexpr uint8 a0 = 4;
static const constexpr uint8 a1 = 5;

SyscallResult MARSKernel::execute () {
    (void)errstream; // w/a for Clang warning
    auto cpu = sim.lock();
    uint64 syscall_code = sim.lock()->read_cpu_register( v0);
    switch (syscall_code) {
        case 1: print_integer(); break;
        case 4: print_string (); break;
        case 5: read_integer (); break;
        case 8: read_string(); break;
        case 10: return { SyscallResult::HALT, 0};
        case 11: print_character(); break;
        case 12: read_character(); break;
        case 17: return { SyscallResult::HALT, cpu->read_cpu_register( a0)};
        default: return { SyscallResult::UNSUPPORTED, 0};
    }
    return { SyscallResult::SUCCESS, 0};
}

void MARSKernel::print_integer() {
    auto value = narrow_cast<int64>( sim.lock()->read_cpu_register( a0));
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
    catch (...) {
        throw BadInputValue();
    }
    if (pos != input.length())
        throw BadInputValue();
    sim.lock()->write_cpu_register( v0, value);
}

void MARSKernel::print_character() {
    auto value = static_cast<char>( sim.lock()->read_cpu_register( a0));
    outstream << value;
}

void MARSKernel::read_character() {
    std::string input;
    instream >> input;
    if (input.length() != 1)
        throw BadInputValue();
    sim.lock()->write_cpu_register( v0, narrow_cast<uint64>( input.at(0)));
}

void MARSKernel::print_string() {
    outstream << mem->read_string( sim.lock()->read_cpu_register( a0));
}

void MARSKernel::read_string() {
    auto cpu = sim.lock();
    uint64 buffer_ptr = cpu->read_cpu_register( a0);
    uint64 chars_to_read = cpu->read_cpu_register( a1);

    std::string input;
    instream >> input;

    mem->write_string_limited( input, buffer_ptr, chars_to_read);
}

