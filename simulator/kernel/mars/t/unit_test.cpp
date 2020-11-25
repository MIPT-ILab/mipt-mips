/**
 * Unit tests for MARS kernel class
 * @author Vyacheslav Kompan kompan.vo@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#include "kernel/mars/mars_kernel.h"
#include <simulator.h>

#include <catch.hpp>

#include <cstdio>
#include <iostream>
#include <sstream>

static const uint8 v0 = 2;
static const uint8 a0 = 4;
static const uint8 a1 = 5;
static const uint8 a2 = 6;

TEST_CASE( "MARS: print integer") {
    std::ostringstream output;
    auto sim = Simulator::create_simulator( "mips64", true);
    auto mars_kernel = create_mars_kernel( std::cin, output, std::cerr);
    mars_kernel->set_simulator( sim);

    sim->write_cpu_register( v0, 1U); // print integer
    sim->write_cpu_register( a0, narrow_cast<uint64>( -1337));
    CHECK( mars_kernel->execute() == Trap::NO_TRAP);
    CHECK( output.str() == "-1337");
}

struct MARSSystem
{
    std::shared_ptr<Simulator> sim = nullptr;
    std::shared_ptr<Kernel> mars_kernel = nullptr;
    std::shared_ptr<FuncMemory> mem = nullptr;

    template<typename ... KernelArgs>
    explicit MARSSystem( KernelArgs&&... args)
        : sim( Simulator::create_simulator( "mips64", true))
        , mars_kernel( create_mars_kernel(std::forward<KernelArgs>(args)...))
        , mem( FuncMemory::create_default_hierarchied_memory())
    {
        mars_kernel->set_simulator( sim);
        sim->set_memory( mem);
        mars_kernel->connect_memory( mem);
    }
};

TEST_CASE( "MARS: print string")
{
    std::ostringstream output;
    std::ostringstream err;
    MARSSystem sys( std::cin, output, err);
    sys.mem->write_string( "Hello World!", 0x1000);

    sys.sim->write_cpu_register( v0, 4U); // print character
    sys.sim->write_cpu_register( a0, 0x1000U);
    CHECK( sys.mars_kernel->execute() == Trap::NO_TRAP);
    CHECK( output.str() == "Hello World!");
}

static uint64 read_integer( const std::string& str)
{
    std::istringstream input( str);
    std::ostringstream out;
    auto sim = Simulator::create_simulator( "mips64", true);
    auto mars_kernel = create_mars_kernel( input, out, out);
    mars_kernel->set_simulator( sim);

    sim->write_cpu_register( v0, 5U); // read integer
    mars_kernel->execute();
    return sim->read_cpu_register( v0);
}

TEST_CASE( "MARS: read integer")
{
    CHECK( read_integer("1337") == 1337);
    CHECK_THROWS_AS( read_integer("133q"), BadInputValue);
    CHECK_THROWS_AS( read_integer("q133"), BadInputValue);
    CHECK_THROWS_AS( read_integer("13333333333333333333333333333333333333333333333333333"), BadInputValue);
}

TEST_CASE( "MARS: read string")
{
    std::istringstream input( "Hello World\n");
    std::ostringstream output;
    MARSSystem sys( input, output, output);
    sys.sim->write_cpu_register( v0, 8U); // read string
    sys.sim->write_cpu_register( a0, 0x1000U);
    sys.sim->write_cpu_register( a1, 0x5);
    CHECK_NOTHROW( sys.mars_kernel->execute());
    CHECK( sys.mem->read_string(0x1000U) == "Hello");
}

TEST_CASE( "MARS: exit") {
    auto sim = Simulator::create_simulator ("mips64", true);
    auto mars_kernel = create_mars_kernel( std::cin, std::cout, std::cerr);
    mars_kernel->set_simulator (sim);

    sim->write_cpu_register( v0, 10U); // exit
    CHECK( mars_kernel->execute() == Trap::HALT);
    CHECK( mars_kernel->get_exit_code() == 0);
}

TEST_CASE( "MARS: print character")
{
    std::ostringstream output;
    std::ostringstream err;
    auto sim = Simulator::create_simulator( "mips64", true);
    auto mars_kernel = create_mars_kernel( std::cin, output, err);
    mars_kernel->set_simulator( sim);

    sim->write_cpu_register( v0, 11U); // print character
    sim->write_cpu_register( a0, uint64{ 'x'});
    CHECK( mars_kernel->execute() == Trap::NO_TRAP);
    CHECK( output.str() == "x");
}

TEST_CASE( "MARS: read character")
{
    std::istringstream input( "z\n");
    std::ostringstream out;
    auto sim = Simulator::create_simulator( "mips64", true);
    auto mars_kernel = create_mars_kernel( input, out, out);
    mars_kernel->set_simulator( sim);

    sim->write_cpu_register( v0, 12U); // read character
    CHECK( mars_kernel->execute() == Trap::NO_TRAP);
    CHECK( sim->read_cpu_register( v0) == 'z');
}

TEST_CASE( "MARS: read bad character")
{
    std::istringstream input( "zz\n");
    std::ostringstream out;
    auto sim = Simulator::create_simulator( "mips64", true);
    auto mars_kernel = create_mars_kernel( input, out, out);
    mars_kernel->set_simulator( sim);

    sim->write_cpu_register( v0, 12U); // read character
    CHECK_THROWS_AS( mars_kernel->execute(), BadInputValue);
}

TEST_CASE( "MARS: read from stdin")
{
    std::istringstream input( "Lorem Ipsum\n");
    std::ostringstream output;
    MARSSystem sys( input, output, output);

    sys.sim->write_cpu_register( v0, 14U); // read from file
    sys.sim->write_cpu_register( a0, 0);   // stdin
    sys.sim->write_cpu_register( a1, 0x1000);
    sys.sim->write_cpu_register( a2, 5);
    CHECK( sys.mars_kernel->execute() == Trap::NO_TRAP);
    CHECK( sys.mem->read_string(0x1000U) == "Lorem");
}

TEST_CASE( "MARS: read from stdout")
{
    std::istringstream input( "Lorem Ipsum\n");
    std::ostringstream output;
    MARSSystem sys( input, output, output);

    sys.sim->write_cpu_register( v0, 14U); // read from file
    sys.sim->write_cpu_register( a0, 1);   // stdin
    sys.sim->write_cpu_register( a1, 0x1000);
    sys.sim->write_cpu_register( a2, 5);
    CHECK( sys.mars_kernel->execute() == Trap::NO_TRAP);
    CHECK( sys.sim->read_cpu_register( v0) == all_ones<uint64>());
    CHECK( sys.mem->read_string(0x1000U).empty());
}

TEST_CASE( "MARS: read from stderr")
{
    std::istringstream input( "Lorem Ipsum\n");
    std::ostringstream output;
    MARSSystem sys( input, output, output);

    sys.sim->write_cpu_register( v0, 14U); // read from file
    sys.sim->write_cpu_register( a0, 2);   // stderr
    sys.sim->write_cpu_register( a1, 0x1000);
    sys.sim->write_cpu_register( a2, 5);
    CHECK( sys.mars_kernel->execute() == Trap::NO_TRAP);
    CHECK( sys.sim->read_cpu_register( v0) == all_ones<uint64>());
    CHECK( sys.mem->read_string(0x1000U).empty());
}

static void write_to_descriptor(MARSSystem* sys, int desc)
{
    sys->mem->write_string( "Lorem Ipsum", 0x1000);
    sys->sim->write_cpu_register( v0, 15U); // write to file
    sys->sim->write_cpu_register( a0, desc); // stdout
    sys->sim->write_cpu_register( a1, 0x1000U);
    sys->sim->write_cpu_register( a2, 7);
}

TEST_CASE( "MARS: write to stdout")
{
    std::ostringstream output;
    MARSSystem sys( std::cin, output, output);
    write_to_descriptor(&sys, 1);
    CHECK( sys.mars_kernel->execute() == Trap::NO_TRAP);
    CHECK( output.str() == "Lorem I");
}

TEST_CASE( "MARS: write to stderr")
{
    std::ostringstream output;
    MARSSystem sys( std::cin, output, output);
    write_to_descriptor(&sys, 2);
    CHECK( sys.mars_kernel->execute() == Trap::NO_TRAP);
    CHECK( output.str() == "Lorem I");
}

TEST_CASE( "MARS: write to stdin")
{
    std::ostringstream output;
    MARSSystem sys( std::cin, output, output);
    write_to_descriptor(&sys, 0);
    CHECK( sys.mars_kernel->execute() == Trap::NO_TRAP);
    CHECK( sys.sim->read_cpu_register( v0) == all_ones<uint64>());
}

TEST_CASE( "MARS: open non existing file")
{
    std::ostringstream output;
    std::ostringstream err;
    MARSSystem sys( std::cin, output, err);
    sys.mem->write_string( "/ksagklhfgldg/sgsfgdsfgadg", 0x1000);
    sys.sim->write_cpu_register( v0, 13U); // open file
    sys.sim->write_cpu_register( a0, 0x1000U);
    sys.sim->write_cpu_register( a1, 1); // write
    CHECK( sys.mars_kernel->execute() == Trap::NO_TRAP);
    CHECK( sys.sim->read_cpu_register( v0) == all_ones<uint64>());
}

static Trap write_buff_to_file( MARSSystem* sys, uint64 descr,
                                uint64 buff_ptr, uint64 chars_to_write)
{
    sys->sim->write_cpu_register( v0, 15U); //write to file
    sys->sim->write_cpu_register( a0, descr);
    sys->sim->write_cpu_register( a1, buff_ptr);
    sys->sim->write_cpu_register( a2, chars_to_write);
    return sys->mars_kernel->execute();
}

static Trap open_file( MARSSystem* sys, uint64 filename_ptr, uint64 flags)
{
    sys->sim->write_cpu_register( v0, 13U); // open file
    sys->sim->write_cpu_register( a0, filename_ptr);
    sys->sim->write_cpu_register( a1, flags);
    return sys->mars_kernel->execute();
}

static Trap read_from_file( MARSSystem* sys, uint64 descr,
                            uint64 buff_ptr, uint64 chars_to_read)
{
    sys->sim->write_cpu_register( v0, 14U); // read from file
    sys->sim->write_cpu_register( a0, descr);
    sys->sim->write_cpu_register( a1, buff_ptr);
    sys->sim->write_cpu_register( a2, chars_to_read);
    return sys->mars_kernel->execute();
}

static Trap close_file( MARSSystem* sys, uint64 descr)
{
    sys->sim->write_cpu_register( v0, 16U);
    sys->sim->write_cpu_register( a0, descr);
    return sys->mars_kernel->execute();
}

TEST_CASE( "MARS: open and close a file")
{
    std::string filename( "tempfile");
    std::ostringstream output;
    std::ostringstream err;
    MARSSystem sys( std::cin, output, err);
    sys.mem->write_string( filename, 0x1000);

    auto trap = open_file( &sys, 0x1000, 1); // WRONLY
    auto descr = sys.sim->read_cpu_register( v0);
    CHECK( trap == Trap::NO_TRAP);
    CHECK( descr >= 3);

    trap = close_file ( &sys, descr);
    CHECK( trap == Trap::NO_TRAP);
}

TEST_CASE( "MARS: open, write and close file")
{
    std::string filename( "tempfile");
    std::ostringstream output;
    std::ostringstream err;
    MARSSystem sys( std::cin, output, err);
    sys.mem->write_string( filename, 0x1000);
    sys.mem->write_string( "Lorem Ipsum\n", 0x2000);

    auto trap = open_file( &sys, 0x1000, 1); // WRONLY
    auto descr = sys.sim->read_cpu_register( v0);
    CHECK( trap == Trap::NO_TRAP);
    CHECK( descr >= 3);

    trap = write_buff_to_file( &sys, descr, 0x2000, 11);
    CHECK( trap == Trap::NO_TRAP); // FIXME(pkryukov): Can file be checked by read()

    trap = close_file ( &sys, descr);
    CHECK( trap == Trap::NO_TRAP);
}

TEST_CASE( "MARS: open, read and close a file")
{
    std::string filename("tempfile");
    std::ostringstream output;
    std::ostringstream err;
    MARSSystem sys( std::cin, output, err);
    sys.mem->write_string( filename, 0x1000);
    sys.mem->write_string( "Lorem Ipsum\n", 0x2000);

    auto trap = open_file( &sys, 0x1000, 1); // open WRONLY
    auto descriptor = sys.sim->read_cpu_register( v0);
    CHECK( trap == Trap::NO_TRAP);
    CHECK( descriptor >= 3);
    trap = write_buff_to_file( &sys, descriptor, 0x2000, 11);
    CHECK( trap == Trap::NO_TRAP);

    trap = close_file( &sys, descriptor);
    CHECK( trap == Trap::NO_TRAP);

    trap = open_file( &sys, 0x1000, 9); // open O_APPEND
    descriptor = sys.sim->read_cpu_register( v0);
    CHECK( trap == Trap::NO_TRAP);
    CHECK( descriptor >= 3);
    trap = write_buff_to_file( &sys, descriptor, 0x2000, 11);
    CHECK( trap == Trap::NO_TRAP);

    trap = close_file( &sys, descriptor);
    CHECK( trap == Trap::NO_TRAP);

    trap = open_file( &sys, 0x1000, 0); // read
    descriptor = sys.sim->read_cpu_register( v0);
    CHECK( trap == Trap::NO_TRAP);
    CHECK( descriptor >= 3);
    trap = read_from_file( &sys, descriptor, 0x3000, 22);
    CHECK( trap == Trap::NO_TRAP);

    CHECK( sys.mem->read_string( 0x3000) == "Lorem IpsumLorem Ipsum");

    trap = close_file( &sys, descriptor);
    CHECK( trap == Trap::NO_TRAP);
}

TEST_CASE( "MARS: open file with invalid mode")
{
    std::string filename("tempfile");
    std::ostringstream output;
    std::ostringstream err;
    MARSSystem sys( std::cin, output, err);
    sys.mem->write_string( filename, 0x1000);

    sys.sim->write_cpu_register( v0, 13U); // open file
    sys.sim->write_cpu_register( a0, 0x1000U);
    sys.sim->write_cpu_register( a1, 1); // write
    auto descriptor = sys.sim->read_cpu_register( v0);
    sys.mars_kernel->execute();

    sys.sim->write_cpu_register( v0, 16U); // close file
    sys.sim->write_cpu_register( a0, descriptor);
    sys.mars_kernel->execute();

    sys.sim->write_cpu_register( v0, 13U); // open file
    sys.sim->write_cpu_register( a0, 0x1000U);
    sys.sim->write_cpu_register( a1, 131); // INVALID MODE!

    CHECK( sys.mars_kernel->execute() == Trap::NO_TRAP);
    CHECK( sys.sim->read_cpu_register( v0) == all_ones<uint64>());
}

TEST_CASE( "MARS: close stdout")
{
    auto sim = Simulator::create_simulator( "mips64", true);
    auto mars_kernel = create_mars_kernel( std::cin, std::cout, std::cerr);
    mars_kernel->set_simulator( sim);
    sim->write_cpu_register( v0, 16U); // close file
    sim->write_cpu_register( a0, 0);
    CHECK( mars_kernel->execute() == Trap::NO_TRAP);
}

TEST_CASE( "MARS: close error")
{
    auto sim = Simulator::create_simulator( "mips64", true);
    auto mars_kernel = create_mars_kernel( std::cin, std::cout, std::cerr);
    mars_kernel->set_simulator( sim);
    sim->write_cpu_register( v0, 16U); // close file
    sim->write_cpu_register( a0, 111);
    CHECK( mars_kernel->execute() == Trap::NO_TRAP);
}

TEST_CASE( "MARS: exit with code")
{
    auto sim = Simulator::create_simulator ("mips64", true);
    auto mars_kernel = create_mars_kernel( std::cin, std::cout, std::cerr);
    mars_kernel->set_simulator(sim);

    sim->write_cpu_register( v0, 17U); // exit
    sim->write_cpu_register( a0, 21U); // exit code
    CHECK( mars_kernel->execute() == Trap::HALT);
    CHECK( mars_kernel->get_exit_code() == 21U);
}

TEST_CASE( "MARS: unsupported syscall")
{
    auto sim = Simulator::create_simulator ("mips64", true);
    auto mars_kernel = create_mars_kernel( std::cin, std::cout, std::cerr);
    mars_kernel->set_simulator(sim);

    sim->write_cpu_register( v0, 666U);
    CHECK( mars_kernel->execute() == Trap::UNSUPPORTED_SYSCALL);
}

TEST_CASE( "MARS: unsupported isa")
{
    auto sim = Simulator::create_simulator( "mips64be", true);
    auto mars_kernel = create_mars_kernel( std::cin, std::cout, std::cerr);
    mars_kernel->set_simulator( sim);
    CHECK_THROWS_AS( mars_kernel->connect_exception_handler(), UnsupportedISA);
}
