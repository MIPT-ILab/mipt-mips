/**
 * Exception handler unit tests
 * @author Pavel Kryukov
 * Copyright 2019 MIPT-MIPS
 */

#include <catch.hpp>
#include <func_sim/driver/driver.h>
#include <func_sim/operation.h>

static auto get_op_with_trap( Trap trap)
{
    Operation op( 0x100, 0x104);
    op.set_trap( trap);
    return op;
}

TEST_CASE( "Driver: dummy driver")
{
    auto drv = Driver::create_default_driver();
    CHECK( drv->handle_trap( get_op_with_trap( Trap( Trap::NO_TRAP))) == Trap::NO_TRAP);
    CHECK( drv->handle_trap( get_op_with_trap( Trap( Trap::HALT))) == Trap::HALT);
    CHECK( drv->handle_trap( get_op_with_trap( Trap( Trap::BREAKPOINT))) == Trap::NO_TRAP);
}

TEST_CASE( "Driver: dummy driver clone")
{
    auto drv = Driver::create_default_driver()->clone();
    CHECK( drv->handle_trap( get_op_with_trap( Trap( Trap::NO_TRAP))) == Trap::NO_TRAP);
    CHECK( drv->handle_trap( get_op_with_trap( Trap( Trap::HALT))) == Trap::HALT);
    CHECK( drv->handle_trap( get_op_with_trap( Trap( Trap::BREAKPOINT))) == Trap::NO_TRAP);
}

TEST_CASE( "Driver: hooked dummy driver")
{
    auto drv = Driver::create_hooked_driver( Driver::create_default_driver().get());
    CHECK( drv->handle_trap( get_op_with_trap( Trap( Trap::NO_TRAP))) == Trap::NO_TRAP);
    CHECK( drv->handle_trap( get_op_with_trap( Trap( Trap::HALT))) == Trap::HALT);
    CHECK( drv->handle_trap( get_op_with_trap( Trap( Trap::BREAKPOINT))) == Trap::BREAKPOINT);
}

TEST_CASE( "Driver: hooked dummy driver clone")
{
    auto drv = Driver::create_hooked_driver( Driver::create_default_driver().get())->clone();
    CHECK( drv->handle_trap( get_op_with_trap( Trap( Trap::NO_TRAP))) == Trap::NO_TRAP);
    CHECK( drv->handle_trap( get_op_with_trap( Trap( Trap::HALT))) == Trap::HALT);
    CHECK( drv->handle_trap( get_op_with_trap( Trap( Trap::BREAKPOINT))) == Trap::BREAKPOINT);
}
