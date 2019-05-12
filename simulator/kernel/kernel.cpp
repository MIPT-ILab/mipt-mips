/*
 * kernel.cpp - base kernel class
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#include "base_kernel.h"
#include "mars/mars_kernel.h"

#include <infra/config/config.h>

namespace config {
    static Switch use_mars = {"mars", "use MARS syscalls"};
} // namespace config

class DummyKernel : public Kernel
{
public:
    SyscallResult execute() final { return {SyscallResult::IGNORED, 0}; }
    void set_simulator( const std::shared_ptr<CPUModel>&) final { };
    void connect_memory( std::shared_ptr<FuncMemory>) final { };
    void add_replica_simulator( const std::shared_ptr<CPUModel>&) final { };
    void add_replica_memory( const std::shared_ptr<FuncMemory>&) final { };
};

std::shared_ptr<Kernel> Kernel::create_dummy_kernel() {
    return std::make_shared<DummyKernel>();
}

std::shared_ptr<Kernel> Kernel::create_configured_kernel() {
    return config::use_mars ? create_mars_kernel() : Kernel::create_dummy_kernel();
}
