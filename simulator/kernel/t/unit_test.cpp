/**
 * Unit tests for base kernel class
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#include "../kernel.h"
// Catch2
#include <catch.hpp>

TEST_CASE( "Kernel: Execute nothing")
{
    CHECK( Kernel::create_dummy_kernel()->execute().type == SyscallResult::SUCCESS);
}

TEST_CASE( "Kernel: Default kernel is empty")
{
    CHECK( Kernel::create_configured_kernel()->execute().type == SyscallResult::SUCCESS);
}
