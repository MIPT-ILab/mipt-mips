/**
 * driver.cpp - exception handler
 * @author Vsevolod Pukhov, Pavel Kryukov
 * Copyright 2019 MIPT-MIPS
 */

#include "driver.h"
#include <func_sim/operation.h>

class DriverNoOp : public Driver
{
public:
    Trap handle_trap( const Operation& instr) const final
    {
        return instr.trap_type() == Trap::HALT ? instr.trap_type() : Trap( Trap::NO_TRAP);
    }

    std::unique_ptr<Driver> clone() const final { return std::make_unique<DriverNoOp>(); }
};

std::unique_ptr<Driver> Driver::create_default_driver()
{
    return std::make_unique<DriverNoOp>();
}

class HookedDriver : public Driver
{
public:
    explicit HookedDriver( const Driver* drv) : drv( drv->clone()) { }
    Trap handle_trap( const Operation& instr) const final
    {
        auto orig_trap = instr.trap_type();
        drv->handle_trap( instr);
        return orig_trap;
    }
    std::unique_ptr<Driver> clone() const final { return std::make_unique<HookedDriver>( drv->clone().get()); }
private:
    std::unique_ptr<Driver> drv = nullptr;
};

std::unique_ptr<Driver> Driver::create_hooked_driver( const Driver* drv)
{
    return std::make_unique<HookedDriver>( drv);
}
