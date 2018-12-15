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

    std::istream& instream;
    std::ostream& outstream;
    std::ostream& errstream;

public:
    bool execute() final;

    MARSKernel( std::istream& instream, std::ostream& outstream, std::ostream& errstream)
      : instream( instream), outstream( outstream), errstream( errstream) {}
};

std::shared_ptr<Kernel> create_mars_kernel( std::istream& instream, std::ostream& outstream, std::ostream& errstream) {
    return std::make_shared<MARSKernel>( instream, outstream, errstream);
}

static const constexpr uint8 v0 = 2;
static const constexpr uint8 a0 = 4;

bool MARSKernel::execute () {
    (void)errstream; // w/a for Clang warning
    uint64 syscall_code = sim.lock()->read_cpu_register( v0);
    switch (syscall_code) {
        case 1: print_integer(); break;
        case 4: print_string (); break;
        case 5: read_integer (); break;
        case 10: return false; // exit
        case 11: print_character(); break;
        case 12: read_character(); break;
        default: break;
#if 0 // Temporarily, as some traces have unimplemented syscalls
            throw Exception( "Syscall " + std::to_string( syscall_code) + " not implemented yet");
#endif
    }
    return true;
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
