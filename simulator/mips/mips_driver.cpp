/**
 * driver.cpp - exception handler
 * @author Pavel Kryukov
 * Copyright 2019 MIPT-MIPS
 */

#include "mips.h"
#include "mips_register/mips_register.h"

#include <func_sim/driver/driver.h>
#include <simulator.h>

class DriverMIPS32 : public Driver
{
public:
    explicit DriverMIPS32( bool verbose, Simulator* sim) : Driver( verbose), cpu( sim) { }
    Trap handle_trap( const Operation& instr) const final
    {
        auto trap = instr.trap_type();
        if ( trap == Trap::NO_TRAP || trap == Trap::HALT)
            return trap;

        auto status = cpu->read_cpu_register( MIPSRegister::status().to_rf_index());
        auto cause  = cpu->read_cpu_register( MIPSRegister::cause().to_rf_index());
        status |= 0x2;
        cause = (cause & ~(bitmask<uint64>(4) << 2)) | ((trap.to_mips_format() & bitmask<uint64>(4)) << 2);
        cpu->write_cpu_register( MIPSRegister::status().to_rf_index(), status);
        cpu->write_cpu_register( MIPSRegister::cause().to_rf_index(), cause);
        cpu->write_cpu_register( MIPSRegister::epc().to_rf_index(), instr.get_PC());
        cpu->set_pc( 0x8'0000'0180);
        return Trap( Trap::NO_TRAP);
    }
    std::unique_ptr<Driver> clone() const final { return std::make_unique<DriverMIPS32>( verbose, cpu); }
private:
    Simulator* const cpu = nullptr;
    const bool verbose = false;
};

std::unique_ptr<Driver> create_mips32_driver( bool verbose, Simulator* sim)
{
    return std::make_unique<DriverMIPS32>( verbose, sim);
}
