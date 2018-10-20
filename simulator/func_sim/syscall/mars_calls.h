/*
 * mars_syscalls.h - implementation of MARS syscalls
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#ifndef MIPS_SYSCALL_H
#define MIPS_SYSCALL_H

/* Simulator modules */
#include "func_sim/rf/rf.h"
#include <infra/exception.h>
#include "infra/types.h"
/* Generic C & C++ */
#include <cassert>
#include <map>
#include <memory>
#include <iostream>


struct TerminatedByExit final : Exception {
    TerminatedByExit() : Exception( "Terminated", "exit called") {}
};

template <typename ISA>
struct Syscall {
    static std::unique_ptr<Syscall<ISA>> get_handler( bool ignore, RF<ISA> *rf, std::istream& cin = std::cin,
                                                      std::ostream& cout = std::cout);
    virtual void execute() {};

    Syscall() = default;
    virtual ~Syscall() = default;
    Syscall( const Syscall&) = delete;
    Syscall( Syscall&&) = delete;
    Syscall& operator=( const Syscall&) = delete;
    Syscall& operator=( Syscall&&) = delete;
};


template <typename ISA>
class MARSCalls : public Syscall<ISA> {
    using Register = typename ISA::Register;
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

#endif //MIPS_SYSCALL_H
