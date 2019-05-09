/**
 * driver.cpp - exception handler
 * @author Vsevolod Pukhov, Pavel Kryukov
 * Copyright 2019 MIPT-MIPS
 */

#include "driver.h"

class DriverNoOp : public Driver
{
public:
    explicit DriverNoOp( bool verbose) : Driver( verbose) { }
    Trap handle_trap_impl( Trap trap, Addr /* pc */) const final { return trap; }
};

class DriverHaltOnly : public Driver
{
public:
    explicit DriverHaltOnly( bool verbose) : Driver( verbose) { }
    Trap handle_trap_impl( Trap trap, Addr /* pc */) const final { return trap == Trap::HALT ? trap : Trap(Trap::NO_TRAP); }
};

class DriverIgnore : public Driver
{
public:
    explicit DriverIgnore( bool verbose) : Driver( verbose) { }
    Trap handle_trap_impl( Trap /* trap */, Addr /* pc */) const final { return Trap(Trap::NO_TRAP); }
};

class DriverCritical : public Driver
{
public:
    explicit DriverCritical( bool verbose) : Driver( verbose) { }
    Trap handle_trap_impl( Trap trap, Addr /* pc */) const final
    {
        if ( trap != Trap::NO_TRAP)
            throw std::runtime_error( "critical trap");
        return trap;
    }
};

std::unique_ptr<Driver> Driver::construct( const std::string& mode, Simulator* /* sim */, bool log)
{
    if ( mode == "stop")
        return std::make_unique<DriverNoOp>( log);
    if ( mode == "stop_on_halt")
        return std::make_unique<DriverHaltOnly>( log);
    if ( mode == "ignore")
        return std::make_unique<DriverIgnore>( log);
    if ( mode == "critical")
        return std::make_unique<DriverCritical>( log);
    throw IncorrectDriver( mode);
}
