/**
 * Unit tests for MARS kernel class
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#include "kernel/mars/mars_kernel.h"
#include <simulator.h>

#include <catch.hpp>

#include <cstdio>
#include <sstream>

static const uint8 v0 = 2;
static const uint8 a0 = 4;
static const uint8 a1 = 5;
static const uint8 a2 = 6;

TEST_CASE( "MARS: construct kernel") {
    CHECK_NOTHROW( create_mars_kernel());
}

TEST_CASE( "MARS: print integer") {
    std::ostringstream output;
    auto sim = Simulator::create_simulator( "mips64", true, false);
    auto mars_kernel = create_mars_kernel( std::cin, output);
    mars_kernel->set_simulator( sim);

    sim->write_cpu_register( v0, 1u); // print integer
    sim->write_cpu_register( a0, narrow_cast<uint64>( -1337));
    CHECK( mars_kernel->execute().type == SyscallResult::SUCCESS);
    CHECK( output.str() == "-1337");
}

struct System
{
    std::shared_ptr<Simulator> sim = nullptr;
    std::shared_ptr<Kernel> mars_kernel = nullptr;
    std::shared_ptr<FuncMemory> mem = nullptr;
    
    template<typename ... KernelArgs>
    System( KernelArgs&&... args)
        : sim( Simulator::create_simulator( "mips64", true, false))
        , mars_kernel( create_mars_kernel(std::forward<KernelArgs>(args)...))
        , mem( FuncMemory::create_plain_memory( 24))
    {
        mars_kernel->set_simulator( sim);
        sim->set_memory( mem);
        mars_kernel->set_memory( mem);
    }
};

TEST_CASE( "MARS: print string")
{
    std::ostringstream output;
    System sys( std::cin, output);
    sys.mem->write_string( "Hello World!", 0x1000);

    sys.sim->write_cpu_register( v0, 4u); // print character
    sys.sim->write_cpu_register( a0, 0x1000u);
    CHECK( sys.mars_kernel->execute().type == SyscallResult::SUCCESS);
    CHECK( output.str() == "Hello World!");
}

TEST_CASE( "MARS: read integer") {
    std::istringstream input( "1337\n");
    auto sim = Simulator::create_simulator( "mips64", true, false);
    auto mars_kernel = create_mars_kernel( input);
    mars_kernel->set_simulator( sim);

    sim->write_cpu_register( v0, 5u); // read integer
    CHECK( mars_kernel->execute().type == SyscallResult::SUCCESS);
    CHECK( sim->read_cpu_register( v0) == 1337);
}

TEST_CASE( "MARS: read bad integer") {
    std::istringstream input( "133q\n");
    auto sim = Simulator::create_simulator( "mips64", true, false);
    auto mars_kernel = create_mars_kernel( input);
    mars_kernel->set_simulator( sim);

    sim->write_cpu_register( v0, 5u); // read integer
    CHECK_THROWS_AS( mars_kernel->execute(), BadInputValue);
}

TEST_CASE( "MARS: read string")
{
    std::istringstream input( "Hello World\n");
    System sys( input);
    sys.sim->write_cpu_register( v0, 8u); // read string
    sys.sim->write_cpu_register( a0, 0x1000u);
    sys.sim->write_cpu_register( a1, 0x5);
    CHECK_NOTHROW( sys.mars_kernel->execute());
    CHECK( sys.mem->read_string(0x1000u) == "Hello");
}

TEST_CASE( "MARS: exit") {
    auto sim = Simulator::create_simulator ("mips64", true, false);
    auto mars_kernel = create_mars_kernel( );
    mars_kernel->set_simulator (sim);

    sim->write_cpu_register( v0, 10u); // exit
    CHECK( mars_kernel->execute().type == SyscallResult::HALT);
    CHECK( mars_kernel->execute().code == 0);
}

TEST_CASE( "MARS: print character") {
    std::ostringstream output;
    auto sim = Simulator::create_simulator( "mips64", true, false);
    auto mars_kernel = create_mars_kernel( std::cin, output);
    mars_kernel->set_simulator( sim);

    sim->write_cpu_register( v0, 11u); // print character
    sim->write_cpu_register( a0, uint64{ 'x'});
    CHECK( mars_kernel->execute().type == SyscallResult::SUCCESS);
    CHECK( output.str() == "x");
}

TEST_CASE( "MARS: read character") {
    std::istringstream input( "z\n");
    auto sim = Simulator::create_simulator( "mips64", true, false);
    auto mars_kernel = create_mars_kernel( input);
    mars_kernel->set_simulator( sim);

    sim->write_cpu_register( v0, 12u); // read character
    CHECK( mars_kernel->execute().type == SyscallResult::SUCCESS);
    CHECK( sim->read_cpu_register( v0) == 'z');
}

TEST_CASE( "MARS: read bad character") {
    std::istringstream input( "zz\n");
    auto sim = Simulator::create_simulator( "mips64", true, false);
    auto mars_kernel = create_mars_kernel( input);
    mars_kernel->set_simulator( sim);

    sim->write_cpu_register( v0, 12u); // read character
    CHECK_THROWS_AS( mars_kernel->execute(), BadInputValue);
}

TEST_CASE( "MARS: read from stdin")
{
    std::istringstream input( "Lorem Ipsum\n");
    System sys( input);

    sys.sim->write_cpu_register( v0, 14u); // read from file
    sys.sim->write_cpu_register( a0, 0);   // stdin
    sys.sim->write_cpu_register( a1, 0x1000);
    sys.sim->write_cpu_register( a2, 5);
    CHECK( sys.mars_kernel->execute().type == SyscallResult::SUCCESS);
    CHECK( sys.mem->read_string(0x1000u) == "Lorem");
}

TEST_CASE( "MARS: read from stdout")
{
    std::istringstream input( "Lorem Ipsum\n");
    System sys( input);

    sys.sim->write_cpu_register( v0, 14u); // read from file
    sys.sim->write_cpu_register( a0, 1);   // stdin
    sys.sim->write_cpu_register( a1, 0x1000);
    sys.sim->write_cpu_register( a2, 5);
    CHECK( sys.mars_kernel->execute().type == SyscallResult::SUCCESS);
    CHECK( sys.sim->read_cpu_register( v0) == all_ones<uint64>());
    CHECK( sys.mem->read_string(0x1000u) == "");
}

TEST_CASE( "MARS: read from stderr")
{
    std::istringstream input( "Lorem Ipsum\n");
    System sys( input);

    sys.sim->write_cpu_register( v0, 14u); // read from file
    sys.sim->write_cpu_register( a0, 2);   // stderr
    sys.sim->write_cpu_register( a1, 0x1000);
    sys.sim->write_cpu_register( a2, 5);
    CHECK( sys.mars_kernel->execute().type == SyscallResult::SUCCESS);
    CHECK( sys.sim->read_cpu_register( v0) == all_ones<uint64>());
    CHECK( sys.mem->read_string(0x1000u) == "");
}

static void write_to_descriptor(System* sys, int desc)
{
    sys->mem->write_string( "Lorem Ipsum", 0x1000);
    sys->sim->write_cpu_register( v0, 15u); // write to file
    sys->sim->write_cpu_register( a0, desc); // stdout
    sys->sim->write_cpu_register( a1, 0x1000u);
    sys->sim->write_cpu_register( a2, 7);
}

TEST_CASE( "MARS: write to stdout")
{
    std::ostringstream output;
    System sys( std::cin, output, output);
    write_to_descriptor(&sys, 1);
    CHECK( sys.mars_kernel->execute().type == SyscallResult::SUCCESS);
    CHECK( output.str() == "Lorem I");
}

TEST_CASE( "MARS: write to stderr")
{
    std::ostringstream output;
    System sys( std::cin, output, output);
    write_to_descriptor(&sys, 2);
    CHECK( sys.mars_kernel->execute().type == SyscallResult::SUCCESS);
    CHECK( output.str() == "Lorem I");
}

TEST_CASE( "MARS: write to stdin")
{
    System sys( std::cin);
    write_to_descriptor(&sys, 0);
    CHECK( sys.mars_kernel->execute().type == SyscallResult::SUCCESS);
    CHECK( sys.sim->read_cpu_register( v0) == all_ones<uint64>());
}

TEST_CASE( "MARS: open non existing file")
{
    std::ostringstream output;
    System sys( std::cin, output);
    sys.mem->write_string( "/ksagklhfgldg/sgsfgdsfgadg", 0x1000);
    sys.sim->write_cpu_register( v0, 13u); // open file
    sys.sim->write_cpu_register( a0, 0x1000u);
    sys.sim->write_cpu_register( a1, 1); // write
    CHECK( sys.mars_kernel->execute().type == SyscallResult::SUCCESS);
    CHECK( sys.sim->read_cpu_register( v0) == all_ones<uint64>());
}

TEST_CASE( "MARS: open, write, read and close a file")
{
    std::string filename("tempfile");
    std::ostringstream output;
    System sys( std::cin, output);
    sys.mem->write_string( filename, 0x1000);
    sys.mem->write_string( "Lorem Ipsum\n", 0x2000);

    sys.sim->write_cpu_register( v0, 13u); // open file
    sys.sim->write_cpu_register( a0, 0x1000u);
    sys.sim->write_cpu_register( a1, 1); // write
    CHECK( sys.mars_kernel->execute().type == SyscallResult::SUCCESS);
    auto descriptor = sys.sim->read_cpu_register( v0);
    CHECK( descriptor >= 3);

    sys.sim->write_cpu_register( v0, 15u); // write to file
    sys.sim->write_cpu_register( a0, descriptor);
    sys.sim->write_cpu_register( a1, 0x2000);
    sys.sim->write_cpu_register( a2, 11);
    CHECK( sys.mars_kernel->execute().type == SyscallResult::SUCCESS);

    sys.sim->write_cpu_register( v0, 16u); // close file
    sys.sim->write_cpu_register( a0, descriptor);
    CHECK( sys.mars_kernel->execute().type == SyscallResult::SUCCESS);

    sys.sim->write_cpu_register( v0, 13u); // open file
    sys.sim->write_cpu_register( a0, 0x1000u);
    sys.sim->write_cpu_register( a1, 9); // append
    CHECK( sys.mars_kernel->execute().type == SyscallResult::SUCCESS);
    descriptor = sys.sim->read_cpu_register( v0);
    CHECK( descriptor >= 3);

    sys.sim->write_cpu_register( v0, 15u); // write to file
    sys.sim->write_cpu_register( a0, descriptor);
    sys.sim->write_cpu_register( a1, 0x2000);
    sys.sim->write_cpu_register( a2, 11);
    CHECK( sys.mars_kernel->execute().type == SyscallResult::SUCCESS);

    sys.sim->write_cpu_register( v0, 16u); // close file
    sys.sim->write_cpu_register( a0, descriptor);
    CHECK( sys.mars_kernel->execute().type == SyscallResult::SUCCESS);

    sys.sim->write_cpu_register( v0, 13u); // open file
    sys.sim->write_cpu_register( a0, 0x1000u);
    sys.sim->write_cpu_register( a1, 0); // read
    CHECK( sys.mars_kernel->execute().type == SyscallResult::SUCCESS);
    descriptor = sys.sim->read_cpu_register( v0);
    CHECK( descriptor >= 3);

    sys.sim->write_cpu_register( v0, 14u); // read from file
    sys.sim->write_cpu_register( a0, descriptor);
    sys.sim->write_cpu_register( a1, 0x3000);
    sys.sim->write_cpu_register( a2, 22);
    CHECK( sys.mars_kernel->execute().type == SyscallResult::SUCCESS);
    CHECK( sys.mem->read_string( 0x3000) == "Lorem IpsumLorem Ipsum");

    sys.sim->write_cpu_register( v0, 16u); // close file
    sys.sim->write_cpu_register( a0, descriptor);
    CHECK( sys.mars_kernel->execute().type == SyscallResult::SUCCESS);
}

TEST_CASE( "MARS: close stdout")
{
    auto sim = Simulator::create_simulator( "mips64", true, false);
    auto mars_kernel = create_mars_kernel();
    mars_kernel->set_simulator( sim);
    sim->write_cpu_register( v0, 16u); // close file
    sim->write_cpu_register( a0, 0);
    CHECK( mars_kernel->execute().type == SyscallResult::SUCCESS);
}

TEST_CASE( "MARS: close error")
{
    auto sim = Simulator::create_simulator( "mips64", true, false);
    auto mars_kernel = create_mars_kernel();
    mars_kernel->set_simulator( sim);
    sim->write_cpu_register( v0, 16u); // close file
    sim->write_cpu_register( a0, 111);
    CHECK( mars_kernel->execute().type == SyscallResult::SUCCESS);
}


TEST_CASE( "MARS: exit with code") 
{
    auto sim = Simulator::create_simulator ("mips64", true, false);
    auto mars_kernel = create_mars_kernel( );
    mars_kernel->set_simulator(sim);

    sim->write_cpu_register( v0, 17u); // exit
    sim->write_cpu_register( a0, 21u); // exit code
    CHECK( mars_kernel->execute().type == SyscallResult::HALT);
    CHECK( mars_kernel->execute().code == 21u);
}
