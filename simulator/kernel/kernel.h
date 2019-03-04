/*
 * kernel.h - base kernel class
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#ifndef KERNEL_H
#define KERNEL_H

/* Simulator modules */
#include <infra/exception.h>
#include <memory/memory.h>
#include <simulator.h>
/* Generic C++ */
#include <iostream>
#include <memory>

struct BadInputValue final : Exception {
    explicit BadInputValue( const std::string& msg) : Exception( "Bad input value", msg) {}
};

struct SyscallResult {
    enum {
        HALT,
        UNSUPPORTED,
        SUCCESS,
        IGNORED,
    } type;
    uint64 code;
};

class Kernel {
protected:
    std::weak_ptr<Simulator> sim;
    std::shared_ptr<FuncMemory> mem;
public:
    static std::shared_ptr<Kernel> create_configured_kernel();
    static std::shared_ptr<Kernel> create_dummy_kernel();

    void set_simulator( const std::shared_ptr<Simulator>& s) { sim = s; }
    void set_memory( std::shared_ptr<FuncMemory> m) { mem = std::move( m); }

    virtual SyscallResult execute() = 0;

    Kernel() = default;
    virtual ~Kernel() = default;
    Kernel( const Kernel&) = delete;
    Kernel( Kernel&&) = delete;
    Kernel& operator=( const Kernel&) = delete;
    Kernel& operator=( Kernel&&) = delete;
};

#endif //KERNEL_H
