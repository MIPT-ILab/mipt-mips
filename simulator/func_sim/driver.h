/**
 * driver.h - exception handler
 * @author Vsevolod Pukhov, Pavel Kryukov
 * Copyright 2019 MIPT-MIPS
 */

#ifndef DRIVER_H
#define DRIVER_H

#include "operation.h"
#include "traps/trap.h"

#include <infra/exception.h>
#include <infra/log.h>

#include <memory>
#include <string>

class Simulator;

struct IncorrectDriver : Exception
{
    explicit IncorrectDriver( const std::string& name)
        : Exception( "Incorrect driver configuration", name)
    { }
};

class Driver : public Log
{
public:
    explicit Driver( bool verbose) : Log( verbose) { }
    static std::unique_ptr<Driver> construct( const std::string& mode, Simulator* sim, bool verbose);
    Trap handle_trap( const Operation& instr) const
    {
        return handle_trap_impl( instr);
    }
private:
    virtual Trap handle_trap_impl( const Operation& instr) const = 0;
};

#endif
