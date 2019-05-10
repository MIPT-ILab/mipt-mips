/**
 * driver.cpp - exception handler
 * @author Vsevolod Pukhov, Pavel Kryukov
 * Copyright 2019 MIPT-MIPS
 */

#include "driver.h"

#include <mips/mips_register/mips_register.h>
#include <simulator.h>

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

class DriverMIPS32 : public Driver
{
public:
    explicit DriverMIPS32( bool verbose, Simulator* sim) : Driver( verbose), cpu( sim) { }
    Trap handle_trap_impl( Trap trap, Addr pc) const final
    {
        if ( trap == Trap::NO_TRAP || trap == Trap::HALT)
            return trap;

        auto status = cpu->read_cpu_register( MIPSRegister::status().to_rf_index());
        auto cause  = cpu->read_cpu_register( MIPSRegister::cause().to_rf_index());
        status |= 0x2;
        cause = (cause & ~(bitmask<uint64>(4) << 2)) | ((trap.to_mips_format() & bitmask<uint64>(4)) << 2);
        cpu->write_cpu_register( MIPSRegister::status().to_rf_index(), status);
        cpu->write_cpu_register( MIPSRegister::cause().to_rf_index(), cause);
        cpu->write_cpu_register( MIPSRegister::epc().to_rf_index(), pc);
        cpu->set_pc( 0x8'0000'0180);
        return Trap( Trap::NO_TRAP);
    }
private:
    Simulator* cpu = nullptr;
};

std::unique_ptr<Driver> Driver::construct( const std::string& mode, Simulator* sim, bool log)
{
    if ( mode == "stop")
        return std::make_unique<DriverNoOp>( log);
    if ( mode == "stop_on_halt")
        return std::make_unique<DriverHaltOnly>( log);
    if ( mode == "ignore")
        return std::make_unique<DriverIgnore>( log);
    if ( mode == "critical")
        return std::make_unique<DriverCritical>( log);
    if ( mode == "mips32")
        return std::make_unique<DriverMIPS32>( log, sim);
    throw IncorrectDriver( mode);
}
