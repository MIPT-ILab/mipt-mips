/**
 * riscv-driver.cpp - exception handler
 * @author Eric Konks
 * Copyright 2020 MIPT-MIPS
 */

#include "risc_v.h"
#include "riscv_register/riscv_register.h"

#include <func_sim/driver/driver.h>
#include <simulator.h>

class DriverRISCV32 : public Driver
{
public:
    explicit DriverRISCV32( Simulator* sim) : cpu( sim) 
    {
        cpu->write_csr_register( "stvec", 0);
    }
    Trap handle_trap( const Operation& instr) const final 
    {
        auto trap = instr.trap_type();
        cpu->write_csr_register( "scause", trap.to_riscv_format());
        if ( trap == Trap::NO_TRAP || trap == Trap::HALT)
            return trap;
        auto tvec = cpu->read_csr_register( "stvec");
        auto pc = trap_vector_address<Addr>( cpu->read_csr_register( "stvec")); 
        cpu->write_csr_register( "sepc", instr.get_PC());
        cpu->set_pc( pc);
        if ( tvec == 0)
            return Trap( Trap::HALT);
        return Trap( Trap::NO_TRAP);
    }
    std::unique_ptr<Driver> clone() const final { return std::make_unique<DriverRISCV32>( cpu); }
private:
    Simulator* const cpu;
};

std::unique_ptr<Driver> create_riscv32_driver( Simulator* sim)
{
    return std::make_unique<DriverRISCV32>( sim);
}
