/*
 * mars_kernel.h - MARS syscalls
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#ifndef MARS_KERNEL_H
#define MARS_KERNEL_H

#include <kernel/kernel.h>

class MARSKernel : public Kernel {
    void print_integer();
    void read_integer();
    void print_character();
    void read_character();
    void print_string();

    std::istream& instream;
    std::ostream& outstream;

public:
    bool execute() final;

    MARSKernel( std::istream& instream, std::ostream& outstream)
      : instream( instream), outstream( outstream) {}
};

#endif //MARS_KERNEL_H
