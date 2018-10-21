/*
 * mars_calls.cpp - implementation of MARS syscalls
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#include "mars_calls.h"
#include <mips/mips.h>

static const MIPSRegister v0 = MIPSRegister::from_cpu_index( 2);
static const MIPSRegister a0 = MIPSRegister::from_cpu_index( 4);

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
    RegisterInt value = 0;
    instream >> value;
    rf->write( v0, value);
}

template <typename ISA>
void MARSCalls<ISA>::print_character() {
    char value = rf->read( a0);
    outstream << value;
}

template <typename ISA>
void MARSCalls<ISA>::read_character() {
    char value = 0;
    instream >> value;
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
