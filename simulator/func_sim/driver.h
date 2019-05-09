/**
 * driver.h - exception handler
 * @author Vsevolod Pukhov, Pavel Kryukov
 * Copyright 2019 MIPT-MIPS
 */

#ifndef DRIVER_H
#define DRIVER_H

#include "traps/trap.h"

#include <infra/log.h>

#include <memory>
#include <string>

class Simulator;

class Driver : public Log
{
public:
    explicit Driver( bool verbose) : Log( verbose) { }
    static std::unique_ptr<Driver> construct( const std::string& mode, Simulator* sim, bool verbose);
    virtual Trap handle_trap( Trap trap) const = 0;
};

#endif
