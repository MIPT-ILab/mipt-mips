/*
 * mars_kernel.h - MARS syscalls
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#ifndef MARS_KERNEL_H
#define MARS_KERNEL_H

#include <infra/macro.h>
#include <kernel/kernel.h>

struct UnsupportedISA final : Exception
{
    explicit UnsupportedISA( std::string_view isa)
        : Exception("MARSKernel unsupported isa", std::string(isa))
    { }
};

std::shared_ptr<Kernel> create_mars_kernel( std::istream& instream, std::ostream& outstream, std::ostream& errstream);

#endif //MARS_KERNEL_H
