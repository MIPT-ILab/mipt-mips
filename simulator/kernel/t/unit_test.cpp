/**
 * Unit tests for base kernel class
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#include <catch.hpp>
#include <kernel/kernel.h>

TEST_CASE( "Kernel: Execute nothing")
{
    CHECK( Kernel::create_dummy_kernel()->execute() == Trap::SYSCALL);
    CHECK( Kernel::create_configured_kernel()->execute() == Trap::SYSCALL);
}

