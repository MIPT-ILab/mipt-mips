/*
 * mars_calls.h - implementation of MARS syscalls
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#ifndef MARS_CALLS_H
#define MARS_CALLS_H

/* Simulator modules */
#include <func_sim/rf/rf.h>
#include <infra/types.h>
#include <syscall/syscall.h>
/* Generic C & C++ */
#include <cassert>

template <typename ISA>
class MARSCalls : public Syscall<ISA> {
    using RegisterUInt = typename ISA::RegisterUInt;
    using RegisterInt = sign_t<RegisterUInt>;

    RF<ISA> *rf;

    std::istream& instream;
    std::ostream& outstream;

    void print_integer();
    void read_integer();
    void exit() { throw TerminatedByExit(); }
    void print_character();
    void read_character();

public:
    MARSCalls( RF<ISA> *rf, std::istream& instream, std::ostream& outstream)
        : rf( rf), instream( instream), outstream( outstream) { assert( rf); };

    void execute() final;
};

#endif //MARS_CALLS_H
