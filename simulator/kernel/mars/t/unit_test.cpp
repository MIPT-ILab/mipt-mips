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
    auto sim = Simulator::create_simulator( "mips64", true);
    auto mars_kernel = create_mars_kernel( std::cin, output);
    mars_kernel->set_simulator( sim);

    sim->write_cpu_register( v0, 1u); // print integer
    sim->write_cpu_register( a0, narrow_cast<uint64>( -1337));
    CHECK( mars_kernel->execute() == Trap::NO_TRAP);
    CHECK( output.str() == "-1337");
}

struct System
{
    std::shared_ptr<Simulator> sim = nullptr;
    std::shared_ptr<Kernel> mars_kernel = nullptr;
    std::shared_ptr<FuncMemory> mem = nullptr;

    template<typename ... KernelArgs>
    explicit System( KernelArgs&&... args)
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
    System sys( std::cin, output);
    sys.mem->write_string( "Hello World!", 0x1000);

    sys.sim->write_cpu_register( v0, 4u); // print character
    sys.sim->write_cpu_register( a0, 0x1000u);
    CHECK( sys.mars_kernel->execute() == Trap::NO_TRAP);
    CHECK( output.str() == "Hello World!");
}

static uint64 read_integer( const std::string& str)
{
    std::istringstream input( str);
    auto sim = Simulator::create_simulator( "mips64", true);
    auto mars_kernel = create_mars_kernel( input);
    mars_kernel->set_simulator( sim);

    sim->write_cpu_register( v0, 5u); // read integer
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

TEST_CASE( "MARS: read bad integer ang fix") {
    std::istringstream input( "133q\n133\n");
    auto sim = Simulator::create_simulator( "mips64", true);
    auto mars_kernel = create_mars_kernel( input);
    mars_kernel->set_simulator( sim);

    sim->write_cpu_register( v0, 5u); // read integer
    CHECK( mars_kernel->execute_interactive() == Trap::NO_TRAP);
    CHECK( sim->read_cpu_register( v0) == 133);
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
    auto sim = Simulator::create_simulator ("mips64", true);
    auto mars_kernel = create_mars_kernel( );
    mars_kernel->set_simulator (sim);

    sim->write_cpu_register( v0, 10u); // exit
    CHECK( mars_kernel->execute() == Trap::HALT);
    CHECK( mars_kernel->get_exit_code() == 0);
}

TEST_CASE( "MARS: print character") {
    std::ostringstream output;
    auto sim = Simulator::create_simulator( "mips64", true);
    auto mars_kernel = create_mars_kernel( std::cin, output);
    mars_kernel->set_simulator( sim);

    sim->write_cpu_register( v0, 11u); // print character
    sim->write_cpu_register( a0, uint64{ 'x'});
    CHECK( mars_kernel->execute() == Trap::NO_TRAP);
    CHECK( output.str() == "x");
}

TEST_CASE( "MARS: read character") {
    std::istringstream input( "z\n");
    auto sim = Simulator::create_simulator( "mips64", true);
    auto mars_kernel = create_mars_kernel( input);
    mars_kernel->set_simulator( sim);

    sim->write_cpu_register( v0, 12u); // read character
    CHECK( mars_kernel->execute() == Trap::NO_TRAP);
    CHECK( sim->read_cpu_register( v0) == 'z');
}

TEST_CASE( "MARS: read bad character") {
    std::istringstream input( "zz\n");
    auto sim = Simulator::create_simulator( "mips64", true);
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
    CHECK( sys.mars_kernel->execute() == Trap::NO_TRAP);
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
    CHECK( sys.mars_kernel->execute() == Trap::NO_TRAP);
    CHECK( sys.sim->read_cpu_register( v0) == all_ones<uint64>());
    CHECK( sys.mem->read_string(0x1000u).empty());
}

TEST_CASE( "MARS: read from stderr")
{
    std::istringstream input( "Lorem Ipsum\n");
    System sys( input);

    sys.sim->write_cpu_register( v0, 14u); // read from file
    sys.sim->write_cpu_register( a0, 2);   // stderr
    sys.sim->write_cpu_register( a1, 0x1000);
    sys.sim->write_cpu_register( a2, 5);
    CHECK( sys.mars_kernel->execute() == Trap::NO_TRAP);
    CHECK( sys.sim->read_cpu_register( v0) == all_ones<uint64>());
    CHECK( sys.mem->read_string(0x1000u).empty());
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
    CHECK( sys.mars_kernel->execute() == Trap::NO_TRAP);
    CHECK( output.str() == "Lorem I");
}

TEST_CASE( "MARS: write to stderr")
{
    std::ostringstream output;
    System sys( std::cin, output, output);
    write_to_descriptor(&sys, 2);
    CHECK( sys.mars_kernel->execute() == Trap::NO_TRAP);
    CHECK( output.str() == "Lorem I");
}

TEST_CASE( "MARS: write to stdin")
{
    System sys( std::cin);
    write_to_descriptor(&sys, 0);
    CHECK( sys.mars_kernel->execute() == Trap::NO_TRAP);
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
    CHECK( sys.mars_kernel->execute() == Trap::NO_TRAP);
    CHECK( sys.sim->read_cpu_register( v0) == all_ones<uint64>());
}

static Trap write_buff_to_file( System* sys, uint64 descr,
                                uint64 buff_ptr, uint64 chars_to_write)
{
    sys->sim->write_cpu_register( v0, 15u); //write to file
    sys->sim->write_cpu_register( a0, descr);
    sys->sim->write_cpu_register( a1, buff_ptr);
    sys->sim->write_cpu_register( a2, chars_to_write);
    return sys->mars_kernel->execute();
}

static Trap open_file( System* sys, uint64 filename_ptr, uint64 flags)
{
    sys->sim->write_cpu_register( v0, 13u); // open file
    sys->sim->write_cpu_register( a0, filename_ptr);
    sys->sim->write_cpu_register( a1, flags);
    return sys->mars_kernel->execute();
}

static Trap read_from_file( System* sys, uint64 descr,
                            uint64 buff_ptr, uint64 chars_to_read)
{
    sys->sim->write_cpu_register( v0, 14u); // read from file
    sys->sim->write_cpu_register( a0, descr);
    sys->sim->write_cpu_register( a1, buff_ptr);
    sys->sim->write_cpu_register( a2, chars_to_read);
    return sys->mars_kernel->execute();
}

static Trap close_file( System* sys, uint64 descr)
{
    sys->sim->write_cpu_register( v0, 16u);
    sys->sim->write_cpu_register( a0, descr);
    return sys->mars_kernel->execute();
}

TEST_CASE( "MARS: open and close a file")
{
    std::string filename( "tempfile");
    std::ostringstream output;
    System sys( std::cin, output);
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
    System sys( std::cin, output);
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
    System sys( std::cin, output);
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
    System sys( std::cin, output);
    sys.mem->write_string( filename, 0x1000);

    sys.sim->write_cpu_register( v0, 13u); // open file
    sys.sim->write_cpu_register( a0, 0x1000u);
    sys.sim->write_cpu_register( a1, 1); // write
    auto descriptor = sys.sim->read_cpu_register( v0);
    sys.mars_kernel->execute();

    sys.sim->write_cpu_register( v0, 16u); // close file
    sys.sim->write_cpu_register( a0, descriptor);
    sys.mars_kernel->execute();

    sys.sim->write_cpu_register( v0, 13u); // open file
    sys.sim->write_cpu_register( a0, 0x1000u);
    sys.sim->write_cpu_register( a1, 131); // INVALID MODE!

    CHECK( sys.mars_kernel->execute() == Trap::NO_TRAP);
    CHECK( sys.sim->read_cpu_register( v0) == all_ones<uint64>());
}

TEST_CASE( "MARS: close stdout")
{
    auto sim = Simulator::create_simulator( "mips64", true);
    auto mars_kernel = create_mars_kernel();
    mars_kernel->set_simulator( sim);
    sim->write_cpu_register( v0, 16u); // close file
    sim->write_cpu_register( a0, 0);
    CHECK( mars_kernel->execute() == Trap::NO_TRAP);
}

TEST_CASE( "MARS: close error")
{
    auto sim = Simulator::create_simulator( "mips64", true);
    auto mars_kernel = create_mars_kernel();
    mars_kernel->set_simulator( sim);
    sim->write_cpu_register( v0, 16u); // close file
    sim->write_cpu_register( a0, 111);
    CHECK( mars_kernel->execute() == Trap::NO_TRAP);
}

TEST_CASE( "MARS: exit with code")
{
    auto sim = Simulator::create_simulator ("mips64", true);
    auto mars_kernel = create_mars_kernel( );
    mars_kernel->set_simulator(sim);

    sim->write_cpu_register( v0, 17u); // exit
    sim->write_cpu_register( a0, 21u); // exit code
    CHECK( mars_kernel->execute() == Trap::HALT);
    CHECK( mars_kernel->get_exit_code() == 21u);
}

TEST_CASE( "MARS: unsupported syscall")
{
    auto sim = Simulator::create_simulator ("mips64", true);
    auto mars_kernel = create_mars_kernel( );
    mars_kernel->set_simulator(sim);

    sim->write_cpu_register( v0, 666u);
    CHECK( mars_kernel->execute() == Trap::UNSUPPORTED_SYSCALL);
}
