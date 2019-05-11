/*
 * base_kernel.h - base kernel class
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2019 MIPT-MIPS
 */

#include "kernel.h"
#include "replicant.h"

#include <vector>

class BaseKernel : public Kernel
{
protected:
    std::unique_ptr<CPUModel> sim;
    std::shared_ptr<FuncMemory> mem;
public:
    void set_simulator( const std::shared_ptr<Simulator>& s) { sim = std::make_unique<CPUReplicant>(s); }
    void connect_memory( std::shared_ptr<FuncMemory> m) { mem = std::move( m); }
};
