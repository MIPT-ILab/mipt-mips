/**
 * riscv-driver.cpp - exception handler
 * @author Eric Konks
 * Copyright 2020 MIPT-MIPS
 */

#include "risc_v.h"
#include "riscv_register/riscv_register.h"

#include <func_sim/driver/driver.h>
#include <simulator.h>

const uint64 TRAP_VECTOR_BASE_ADDRESS = 0x8'000'0000;

class DriverRISCV32 : public Driver
{
public:
    explicit DriverRISCV32( Simulator* sim) : cpu( sim) 
    {
        cpu->write_csr_register( "mtvec", TRAP_VECTOR_BASE_ADDRESS);
    }
    Trap handle_trap( const Operation& instr) const final 
    {
        auto trap = instr.trap_type();
        if ( trap == Trap::NO_TRAP || trap == Trap::HALT)
            return trap;

        auto tvec = cpu->read_csr_register( "mtvec");
        tvec = (tvec >> 2U) & ~(bitmask<uint64>( 3));
        cpu->write_csr_register( "mcause", trap.to_riscv_format());
        cpu->write_csr_register( "mepc", instr.get_PC());
        cpu->set_pc( tvec);
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