/**
 * mips.h - all the aliases to MIPS ISA
 * @author Aleksandr Misevich
 * Copyright 2018 MIPT-MIPS
 */

#ifndef MIPS_H_
#define MIPS_H_

#include "mips_instr.h"

template<MIPSVersion version>
struct MIPS
{
    using Register = MIPSRegister;
    using RegisterUInt = MIPSRegisterUInt<version>;
    using FuncInstr = BaseMIPSInstr<RegisterUInt>;
    static auto create_instr( uint32 bytes, Endian endian, Addr PC) {
        return FuncInstr( version, endian, bytes, PC);
    }
};

// 32 bit MIPS
using MIPSI   = MIPS<MIPSVersion::I>;
using MIPSII  = MIPS<MIPSVersion::II>;
using MIPS32  = MIPS<MIPSVersion::v32>;
using MARS    = MIPS<MIPSVersion::mars>;

// 64 bit MIPS
using MIPSIII = MIPS<MIPSVersion::III>;
using MIPSIV  = MIPS<MIPSVersion::IV>;
using MIPS64  = MIPS<MIPSVersion::v64>;
using MARS64  = MIPS<MIPSVersion::mars64>;

#endif // MIPS_H_
