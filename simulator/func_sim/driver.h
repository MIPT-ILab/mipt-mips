/**
 * driver.h - exception handler
 * @author Vsevolod Pukhov, Pavel Kryukov
 * Copyright 2019 MIPT-MIPS
 */

#ifndef DRIVER_H
#define DRIVER_H

#include "traps/trap.h"

#include <memory>
#include <string>

class Simulator;

class Driver
{
public:
    static std::unique_ptr<Driver> construct( const std::string& mode, Simulator* sim);
    virtual Trap handle_trap( Trap trap) const = 0;

    Driver() = default;
    virtual ~Driver() = default;
    Driver( const Driver&) = delete;
    Driver( Driver&&) = delete;
    Driver& operator=( const Driver&) = delete;
    Driver& operator=( Driver&&) = delete;
};

#endif
