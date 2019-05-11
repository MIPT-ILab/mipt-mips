/**
 * risc_v.h - all the aliases to RISC-V ISA
 * @author Pavel Kryukov
 * Copyright 2018 MIPT-MIPS
 */ 

#ifndef RISC_V_H_
#define RISC_V_H_

#include "riscv_instr.h"

#include <infra/types.h>

class Driver;
class Simulator;

template <typename T>
struct RISCV
{
    using FuncInstr = RISCVInstr<T>;
    using Register = RISCVRegister;
    using RegisterUInt = T;
    static auto create_instr( uint32 bytes, Endian /* little */, Addr PC) {
        return FuncInstr( bytes, PC);
    }
    static std::unique_ptr<Driver> create_driver( bool /* verbose */, Simulator* /* sim */) {
        return nullptr;
    }
};

using RISCV32 = RISCV<uint32>;
using RISCV64 = RISCV<uint64>;
using RISCV128 = RISCV<uint128>;

#endif // RISC_V_H_
