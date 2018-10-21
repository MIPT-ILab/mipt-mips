/*
 * mars_calls.cpp - implementation of MARS syscalls
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#include "mars_calls.h"
/* ISA */
#include <mips/mips.h>
/* Generic C & C++ */
#include <cassert>
#include <utility>

static const MIPSRegister v0 = MIPSRegister::from_cpu_index( 2);
static const MIPSRegister a0 = MIPSRegister::from_cpu_index( 4);

std::pair<bool, long long> strToInt( const std::string &str) {
    char *end;
    long long value = std::strtoll( str.c_str(), &end, 0);
    bool isInt = !(*end);
    return std::make_pair( isInt, value);
}

std::pair<bool, char> strToChar( const std::string &str) {
    bool isChar = (str.length() == 1);
    char value = isChar ? str[0] : ' ';
    return std::make_pair( isChar, value);
}

template <typename ISA>
void MARSCalls<ISA>::execute() {
    RegisterUInt syscall_code = rf->read( v0);
    switch (syscall_code) {
        case 1:  print_integer(); break;
        case 5:  read_integer(); break;
        case 10: exit(); break;
        case 11: print_character(); break;
        case 12: read_character(); break;
        default: break;
#if 0 // Temporarily, as some traces have unimplemented syscalls
            throw Exception( "Syscall " + std::to_string( syscall_code) + " not implemented yet");
#endif
    }
}

template <typename ISA>
void MARSCalls<ISA>::print_integer() {
    RegisterInt value = rf->read( a0);
    outstream << value;
}

template <typename ISA>
void MARSCalls<ISA>::read_integer() {
    std::string input;
    instream >> input;
    auto[isInt, value] = strToInt( input);
    if (!isInt)
        throw BadInputValue();
    rf->write( v0, value);
}

template <typename ISA>
void MARSCalls<ISA>::print_character() {
    char value = rf->read( a0);
    outstream << value;
}

template <typename ISA>
void MARSCalls<ISA>::read_character() {
    std::string input;
    instream >> input;
    auto[isChar, value] = strToChar( input);
    if (!isChar)
        throw BadInputValue();
    rf->write( v0, value);
}


template class MARSCalls<MIPSI>;
template class MARSCalls<MIPSII>;
template class MARSCalls<MIPSIII>;
template class MARSCalls<MIPSIV>;
template class MARSCalls<MIPS32>;
template class MARSCalls<MIPS64>;
/* Disable MARS syscalls for RISC-V */
#include <risc_v/risc_v.h>
template <> void MARSCalls<RISCV32>::execute() {}
template <> void MARSCalls<RISCV64>::execute() {}
template <> void MARSCalls<RISCV128>::execute() {}
