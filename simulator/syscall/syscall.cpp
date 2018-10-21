/*
 * syscall.cpp - interface for syscalls
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#include "syscall.h"
/* Syscall implementations */
#include <func_sim/syscall/mars_calls.h>

template <typename ISA>
std::unique_ptr<Syscall<ISA>>
Syscall<ISA>::get_handler( bool ignore, RF<ISA> *rf, std::istream& instream, std::ostream& outstream) {
    if (ignore)
        return std::make_unique<Syscall<ISA>>();
    return std::make_unique<MARSCalls<ISA>>( rf, instream, outstream);
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
