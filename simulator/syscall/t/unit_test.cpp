/**
 * Unit tests for syscalls interface
 * @author Vyacheslav Kompan
 * Copyright 2018 MIPT-MIPS
 */

#include "../syscall.h"
/* ISA */
#include <mips/mips.h>
/* Catch2 */
#include <catch.hpp>

static const MIPSRegister v0 = MIPSRegister::from_cpu_index( 2);

TEST_CASE( "Syscalls: Ignore calls") {
    RF<MIPS32> rf;
    auto syscall_handler = Syscall<MIPS32>::get_handler( true, &rf);

    /* Check if syscalls are ignored */
    rf.write( v0, 10u); // exit
    CHECK_NOTHROW( syscall_handler->execute());
}

