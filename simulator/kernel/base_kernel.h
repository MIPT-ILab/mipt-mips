/*
 * base_kernel.h - base kernel class
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2019 MIPT-MIPS
 */
 
#ifndef BASE_KERNEL_H
#define BASE_KERNEL_H

#include "kernel.h"
#include "replicant.h"

#include <vector>

class BaseKernel : public Kernel
{
public:
    explicit BaseKernel( std::ostream& cerr) : Kernel( cerr) { }
    void set_simulator( const std::shared_ptr<CPUModel>& s) override { sim = std::make_unique<CPUReplicant>( s); }
    void connect_memory( std::shared_ptr<FuncMemory> m) override { mem = std::make_unique<FuncMemoryReplicant>( m); }
    void connect_exception_handler() override { }
    void add_replica_simulator( const std::shared_ptr<CPUModel>& s) override { sim->add_replica( s); }
    void add_replica_memory( const std::shared_ptr<FuncMemory>& s) override { mem->add_replica( s); }
    void load_file( const std::string& name) override;
protected:
    std::unique_ptr<CPUReplicant> sim;
    std::unique_ptr<FuncMemoryReplicant> mem;
};

#endif // BASE_KERNEL_H
