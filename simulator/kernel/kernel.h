/*
 * kernel.h - base kernel class
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#ifndef KERNEL_H
#define KERNEL_H

#include <memory/memory.h>
#include <simulator.h>

#include <memory>

class Kernel {
    std::weak_ptr<Simulator> sim;
    std::shared_ptr<FuncMemory> mem;
public:
    static std::shared_ptr<Kernel> create_kernel() {
        return std::make_shared<Kernel>();
    }

    void set_simulator( std::shared_ptr<Simulator> s) { sim = std:move( s); }
    void set_memory( std::shared_ptr<FuncMemory> m) { mem = std::move( m); }
    /* Return false if simulator should be stopped, e.g. on 'exit' syscall */
    virtual bool execute() { return true; }

    Kernel() = default;
    virtual ~Kernel() = default;
    Kernel( const Kernel&) = delete;
    Kernel( Kernel&&) = delete;
    Kernel operator=( const Kernel&) = delete;
    Kernel operator=( Kernel&&) = delete;
};

#endif //KERNEL_H
