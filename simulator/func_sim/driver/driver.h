/**
 * driver.h - exception handler
 * @author Vsevolod Pukhov, Pavel Kryukov
 * Copyright 2019 MIPT-MIPS
 */

#ifndef DRIVER_H
#define DRIVER_H

#include <func_sim/traps/trap.h>
#include <infra/log.h>

#include <memory>
#include <string>

class Operation;
class Simulator;

class Driver : public Log
{
public:
    static std::unique_ptr<Driver> create_default_driver();
    static std::unique_ptr<Driver> create_hooked_driver( const Driver* drv);
    virtual Trap handle_trap( const Operation& instr) const = 0;
    virtual std::unique_ptr<Driver> clone() const = 0;
};

#endif
