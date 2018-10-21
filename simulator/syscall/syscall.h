/*
 * syscall.h - interface for syscalls
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#ifndef SYSCALL_H
#define SYSCALL_H

/* Simulator modules */
#include <func_sim/rf/rf.h>
#include <infra/exception.h>
/* Generic C++ */
#include <iostream>
#include <memory>

struct TerminatedByExit final : Exception {
    TerminatedByExit() : Exception( "Terminated", "exit called") {}
};

struct BadInputValue final : Exception {
    BadInputValue() : Exception( "Runtime syscall error", "bad input value") {}
};

template <typename ISA>
struct Syscall {
    static std::unique_ptr<Syscall<ISA>> get_handler( bool ignore, RF<ISA> *rf, std::istream& instream = std::cin,
                                                      std::ostream& outstream = std::cout);
    virtual void execute() {};

    Syscall() = default;
    virtual ~Syscall() = default;
    Syscall( const Syscall&) = delete;
    Syscall( Syscall&&) = delete;
    Syscall& operator=( const Syscall&) = delete;
    Syscall& operator=( Syscall&&) = delete;
};

#endif //SYSCALL_H
