/**
 * Unit tests for MARS calls
 * @author Vyacheslav Kompan
 * Copyright 2018 MIPT-MIPS
 */

#include "../mars_calls.h"
/* ISA */
#include <mips/mips.h>
#include <risc_v/risc_v.h>
/* Catch2 */
#include <catch.hpp>
/* Generic C++ */
#include <sstream>

static const MIPSRegister v0 = MIPSRegister::from_cpu_index( 2);
static const MIPSRegister a0 = MIPSRegister::from_cpu_index( 4);

TEST_CASE( "MARSCalls: Disable RISCV") {
    RF<RISCV32> rf;
    auto syscall_handler = Syscall<RISCV32>::get_handler( false, &rf);

    /* Check if trying to do a MARS call does nothing for RISC-V */
    CHECK_NOTHROW( syscall_handler->execute ());
}

TEST_CASE( "MARSCalls: print_integer") {
    using RegisterUInt = MIPS32::RegisterUInt;
    RF<MIPS32> rf;
    std::ostringstream output;
    auto syscall_handler = Syscall<MIPS32>::get_handler( false, &rf, std::cin, output);

    rf.write( v0, 1u); // print_integer
    rf.write( a0, static_cast<RegisterUInt>(-1337));
    syscall_handler->execute();
    CHECK( output.str() == "-1337");
}

TEST_CASE( "MARSCalls: read_integer") {
    RF<MIPS32> rf;
    std::istringstream input( "1337\n");
    auto syscall_handler = Syscall<MIPS32>::get_handler( false, &rf, input);

    rf.write( v0, 5u); // read_integer
    CHECK_NOTHROW( syscall_handler->execute());
    CHECK( rf.read( v0) == 1337 );
}

TEST_CASE( "MARSCalls: read_integer_bad") {
    RF<MIPS32> rf;
    std::istringstream input( "1337q\n");
    auto syscall_handler = Syscall<MIPS32>::get_handler( false, &rf, input);

    rf.write( v0, 5u); // read_integer
    CHECK_THROWS_AS( syscall_handler->execute(), BadInputValue);
}

TEST_CASE( "MARSCalls: exit") {
    RF<MIPS32> rf;
    auto syscall_handler = Syscall<MIPS32>::get_handler( false, &rf);

    rf.write( v0, 10u); // exit
    CHECK_THROWS_AS( syscall_handler->execute(), TerminatedByExit);
}

TEST_CASE( "MARSCalls: print_character") {
    RF<MIPS32> rf;
    std::ostringstream output;
    auto syscall_handler = Syscall<MIPS32>::get_handler( false, &rf, std::cin, output);

    rf.write( v0, 11u); // print_character
    rf.write( a0, 'x');
    syscall_handler->execute();
    CHECK( output.str() == "x");
}

TEST_CASE( "MARSCalls: read_character") {
    RF<MIPS32> rf;
    std::istringstream input( "z\n");
    auto syscall_handler = Syscall<MIPS32>::get_handler( false, &rf, input);

    rf.write( v0, 12u); // read_character
    CHECK_NOTHROW( syscall_handler->execute());
    CHECK( rf.read( v0) == 'z');
}

TEST_CASE( "MARSCalls: read_character_bad") {
    RF<MIPS32> rf;
    std::istringstream input( "zz\n");
    auto syscall_handler = Syscall<MIPS32>::get_handler( false, &rf, input);

    rf.write( v0, 12u); // read_character
    CHECK_THROWS_AS( syscall_handler->execute(), BadInputValue);
}
