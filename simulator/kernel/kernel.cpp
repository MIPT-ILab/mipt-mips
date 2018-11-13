/*
 * kernel.cpp - base kernel class
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#include "kernel.h"
#include "mars/mars_kernel.h"
/* Simulator modules. */
#include <infra/config/config.h>

namespace config {
    static Switch use_mars = {"mars", "use MARS syscalls"};
}

std::shared_ptr<Kernel> Kernel::create_kernel( bool use_mars, std::istream& instream, std::ostream& outstream) {
    if (use_mars)
        return std::make_shared<MARSKernel>( instream, outstream);
    return std::make_shared<Kernel>();
}

std::shared_ptr<Kernel> Kernel::create_configured_kernel() {
    return create_kernel( config::use_mars);
}
