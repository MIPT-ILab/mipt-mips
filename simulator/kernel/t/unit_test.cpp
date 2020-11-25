/**
 * Unit tests for base kernel class
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#include <catch.hpp>
#include <kernel/kernel.h>

#include <iostream>
#include <sstream>

TEST_CASE( "Kernel: Execute nothing")
{
    CHECK( Kernel::create_kernel( false, std::cin, std::cout, std::cerr)->execute() == Trap::SYSCALL);
    CHECK( Kernel::create_configured_kernel()->execute() == Trap::SYSCALL);
}

static uint64 read_integer( const std::string& str)
{
    static const constexpr uint8 v0 = 2;

    std::istringstream input( str);
    std::ostringstream out;
    auto sim = Simulator::create_simulator( "mips64", true);
    auto mars_kernel = Kernel::create_kernel( true, input, out, out);
    mars_kernel->set_simulator( sim);

    sim->write_cpu_register( v0, 5U); // read integer
    mars_kernel->execute_interactive();
    return sim->read_cpu_register( v0);
}

TEST_CASE( "Kernel: Interactive call")
{
    CHECK( read_integer( "1337") == 1337);
    CHECK_THROWS_AS( read_integer( "133q"), BadInteraction);
    CHECK( read_integer( "133q\n133\n") == 133);
}
