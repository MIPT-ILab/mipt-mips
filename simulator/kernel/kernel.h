/*
 * kernel.h - base kernel class
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#ifndef KERNEL_H
#define KERNEL_H

/* Simulator modules */
#include <memory/memory.h>
#include <simulator.h>
/* Generic C++ */
#include <memory>

class Kernel {
    std::weak_ptr<Simulator> sim;
    std::shared_ptr<FuncMemory> mem;
public:
    static std::shared_ptr<Kernel> create_kernel() {
        return std::make_shared<Kernel>();
    }

    void set_simulator( std::shared_ptr<Simulator> sim) { this->sim = sim; }
    void set_memory( std::shared_ptr<FuncMemory> mem) { this->mem = std::move( mem); }
    virtual bool execute() { return true; }

    Kernel() = default;
    Kernel( const Kernel&) = delete;
    Kernel( Kernel&&) = delete;
    Kernel operator=( const Kernel&) = delete;
    Kernel operator=( Kernel&&) = delete;
};

#endif //KERNEL_H
