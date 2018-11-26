/**
 * Unit tests for MARS kernel class
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#include "kernel/mars/mars_kernel.h"
#include <simulator.h>
/* Catch2 */
#include <catch.hpp>
/* Generic C++ */
#include <sstream>

static const uint8 v0 = 2;
static const uint8 a0 = 4;

TEST_CASE( "MARS: construct kernel") {
    CHECK_NOTHROW( Kernel::create_kernel( true));
}

TEST_CASE( "MARS: print integer") {
    std::ostringstream output;
    auto sim = Simulator::create_simulator( "mips64", true, false);
    auto mars_kernel = Kernel::create_kernel( true, std::cin, output);
    mars_kernel->set_simulator( sim);

    sim->write_cpu_register( v0, 1u); // print integer
    sim->write_cpu_register( a0, narrow_cast<uint64>( -1337));
    CHECK_NOTHROW( mars_kernel->execute());
    CHECK( output.str() == "-1337");
}

TEST_CASE( "MARS: print string")
{
    std::ostringstream output;
    auto sim = Simulator::create_simulator( "mips64", true, false);
    auto mars_kernel = Kernel::create_kernel( true, std::cin, output);
    mars_kernel->set_simulator( sim);
    auto mem = FuncMemory::create_plain_memory( 24);
    sim->set_memory( mem);
    mars_kernel->set_memory( mem);
    mem->write_string( "Hello World!", 0x1000);

    sim->write_cpu_register( v0, 4u); // print character
    sim->write_cpu_register( a0, 0x1000u);
    CHECK_NOTHROW( mars_kernel->execute());
    CHECK( output.str() == "Hello World!");
}

TEST_CASE( "MARS: read integer") {
    std::istringstream input( "1337\n");
    auto sim = Simulator::create_simulator( "mips64", true, false);
    auto mars_kernel = Kernel::create_kernel( true, input);
    mars_kernel->set_simulator( sim);

    sim->write_cpu_register( v0, 5u); // read integer
    CHECK_NOTHROW( mars_kernel->execute());
    CHECK( sim->read_cpu_register( v0) == 1337);
}

TEST_CASE( "MARS: read bad integer") {
    std::istringstream input( "133q\n");
    auto sim = Simulator::create_simulator( "mips64", true, false);
    auto mars_kernel = Kernel::create_kernel( true, input);
    mars_kernel->set_simulator( sim);

    sim->write_cpu_register( v0, 5u); // read integer
    CHECK_THROWS_AS( mars_kernel->execute(), BadInputValue);
}

TEST_CASE( "MARS: exit") {
    auto sim = Simulator::create_simulator ("mips64", true, false);
    auto mars_kernel = Kernel::create_kernel (true);
    mars_kernel->set_simulator (sim);

    sim->write_cpu_register( v0, 10u); // exit
    CHECK_FALSE( mars_kernel->execute());
}

TEST_CASE( "MARS: print character") {
    std::ostringstream output;
    auto sim = Simulator::create_simulator( "mips64", true, false);
    auto mars_kernel = Kernel::create_kernel( true, std::cin, output);
    mars_kernel->set_simulator( sim);

    sim->write_cpu_register( v0, 11u); // print character
    sim->write_cpu_register( a0, uint64{ 'x'});
    CHECK_NOTHROW( mars_kernel->execute());
    CHECK( output.str() == "x");
}

TEST_CASE( "MARS: read character") {
    std::istringstream input( "z\n");
    auto sim = Simulator::create_simulator( "mips64", true, false);
    auto mars_kernel = Kernel::create_kernel( true, input);
    mars_kernel->set_simulator( sim);

    sim->write_cpu_register( v0, 12u); // read character
    CHECK_NOTHROW( mars_kernel->execute());
    CHECK( sim->read_cpu_register( v0) == 'z');
}

TEST_CASE( "MARS: read bad character") {
    std::istringstream input( "zz\n");
    auto sim = Simulator::create_simulator( "mips64", true, false);
    auto mars_kernel = Kernel::create_kernel( true, input);
    mars_kernel->set_simulator( sim);

    sim->write_cpu_register( v0, 12u); // read character
    CHECK_THROWS( mars_kernel->execute());
}
