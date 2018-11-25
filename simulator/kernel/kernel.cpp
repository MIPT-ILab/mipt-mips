/*
 * kernel.cpp - base kernel class
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#include "kernel.h"
#include "mars/mars_kernel.h"

#include <infra/config/config.h>

namespace config {
    static Switch use_mars = {"mars", "use MARS syscalls"};
} // namespace config

class DummyKernel : public Kernel
{
public:
    bool execute() final { return true; }
};

std::shared_ptr<Kernel> Kernel::create_dummy_kernel() {
    return std::make_shared<DummyKernel>();
}

std::shared_ptr<Kernel> Kernel::create_configured_kernel() {
    return config::use_mars ? create_mars_kernel() : Kernel::create_dummy_kernel();
}
