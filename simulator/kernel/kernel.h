/*
 * kernel.h - base kernel class
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#ifndef KERNEL_H
#define KERNEL_H

#include <func_sim/traps/trap.h>
#include <infra/exception.h>
#include <memory/memory.h>
#include <simulator.h>

#include <iostream>
#include <memory>

struct BadInputValue final : Exception {
    explicit BadInputValue( const std::string& msg) : Exception( "Bad input value", msg) {}
};

class Kernel {
public:
    static std::shared_ptr<Kernel> create_configured_kernel();
    static std::shared_ptr<Kernel> create_dummy_kernel();

    virtual void set_simulator( const std::shared_ptr<Simulator>& s) = 0;
    virtual void connect_memory( std::shared_ptr<FuncMemory> m) = 0;

    virtual Trap execute() = 0;

    Kernel() = default;
    virtual ~Kernel() = default;
    Kernel( const Kernel&) = delete;
    Kernel( Kernel&&) = delete;
    Kernel& operator=( const Kernel&) = delete;
    Kernel& operator=( Kernel&&) = delete;

    int get_exit_code() const { return exit_code; }
protected:
    int exit_code = 0;
};

#endif //KERNEL_H
