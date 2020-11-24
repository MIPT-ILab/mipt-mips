/**
 * risc_v.h - all the aliases to RISC-V ISA
 * @author Pavel Kryukov
 * Copyright 2018 MIPT-MIPS
 */ 

#ifndef RISC_V_H_
#define RISC_V_H_

#include "riscv_instr.h"

#include <func_sim/driver/driver.h>
#include <infra/types.h>
#include <infra/uint128.h>

class Simulator;

std::unique_ptr<Driver> create_riscv32_driver( Simulator* sim);

template <typename T>
struct RISCV
{
    using FuncInstr = RISCVInstr<T>;
    using Register = RISCVRegister;
    using RegisterUInt = T;
    static auto create_instr( uint32 bytes, std::endian /* little */, Addr PC) {
        return FuncInstr( bytes, PC);
    }
    static auto create_driver( Simulator* sim) {
        return create_riscv32_driver( sim);
    }
};

using RISCV32 = RISCV<uint32>;
using RISCV64 = RISCV<uint64>;
using RISCV128 = RISCV<uint128>;

#endif // RISC_V_H_
