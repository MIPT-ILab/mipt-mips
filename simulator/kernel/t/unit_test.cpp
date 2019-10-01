/**
 * Unit tests for base kernel class
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#include "../kernel.h"
#include <catch.hpp>

TEST_CASE( "Kernel: Execute nothing")
{
    CHECK( Kernel::create_dummy_kernel()->execute() == Trap::SYSCALL);
    CHECK( Kernel::create_configured_kernel()->execute() == Trap::SYSCALL);
}

TEST_CASE( "Kernel: Dummy kernel")
{
    auto k =  Kernel::create_dummy_kernel();
    CHECK_NOTHROW( k->set_simulator( nullptr));
    CHECK_NOTHROW( k->connect_memory( nullptr));
    CHECK_NOTHROW( k->add_replica_simulator( nullptr));
    CHECK_NOTHROW( k->add_replica_memory( nullptr));
}
