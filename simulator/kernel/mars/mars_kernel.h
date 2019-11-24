/*
 * mars_kernel.h - MARS syscalls
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#ifndef MARS_KERNEL_H
#define MARS_KERNEL_H

#include <kernel/kernel.h>

std::shared_ptr<Kernel> create_mars_kernel( std::istream& instream = std::cin, std::ostream& outstream = std::cout, std::ostream& errstream = std::cerr);

#endif //MARS_KERNEL_H
