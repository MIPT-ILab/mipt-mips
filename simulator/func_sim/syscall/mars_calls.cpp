/*
 * mars_calls.cpp - implementation of MARS syscalls
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#include "mars_calls.h"

enum Regno : uint8 {
    v0 = 2,
    a0 = 4
};

template <typename ISA>
std::unique_ptr<Syscall<ISA>>
Syscall<ISA>::get_handler( bool ignore, RF<ISA> *rf, std::istream& cin, std::ostream& cout) {
    if (ignore)
        return std::make_unique<Syscall<ISA>>();
    return std::make_unique<MARSCalls<ISA>>( rf, cin, cout);
}

template <typename ISA>
void MARSCalls<ISA>::execute() {
    RegisterUInt syscall_code = rf->read( Register::from_cpu_index( Regno::v0));
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
    RegisterInt value = rf->read( Register::from_cpu_index( Regno::a0));
    outstream << value;
}

template <typename ISA>
void MARSCalls<ISA>::read_integer() {
    RegisterInt value = 0;
    instream >> value;
    rf->write( Register::from_cpu_index( Regno::v0), value);
}

template <typename ISA>
void MARSCalls<ISA>::print_character() {
    char value = rf->read( Register::from_cpu_index( Regno::a0));
    outstream << value;
}

template <typename ISA>
void MARSCalls<ISA>::read_character() {
    char value = 0;
    instream >> value;
    rf->write( Register::from_cpu_index( Regno::v0), value);
}


#include <mips/mips.h>
#include <risc_v/risc_v.h>

template struct Syscall<MIPSI>;
template struct Syscall<MIPSII>;
template struct Syscall<MIPSIII>;
template struct Syscall<MIPSIV>;
template struct Syscall<MIPS32>;
template struct Syscall<MIPS64>;
template struct Syscall<RISCV32>;
template struct Syscall<RISCV64>;
template struct Syscall<RISCV128>;

/* Disable MARS syscalls for RISC-V */
template <> void MARSCalls<RISCV32>::execute() {}
template <> void MARSCalls<RISCV64>::execute() {}
template <> void MARSCalls<RISCV128>::execute() {}
