/**
 * mips.h - all the aliases to MIPS ISA
 * @author Aleksandr Misevich
 * Copyright 2018 MIPT-MIPS
 */

#ifndef MIPS_H_
#define MIPS_H_

#include "mips_instr.h"

#include <infra/endian.h>

template<MIPSVersion version, Endian endian>
struct MIPS
{
    using Register = MIPSRegister;
    using RegisterUInt = MIPSRegisterUInt<version>;
    using FuncInstr = BaseMIPSInstr<RegisterUInt>;
    static constexpr const auto endianness = endian;
    static auto create_instr( uint32 bytes, Addr PC) {
        return FuncInstr( version, endian, bytes, PC);
    }
};

// 32 bit MIPS
using MIPSI_LE   = MIPS<MIPSVersion::I, Endian::little>;
using MIPSII_LE  = MIPS<MIPSVersion::II, Endian::little>;
using MIPS32_LE  = MIPS<MIPSVersion::v32, Endian::little>;
using MARS_LE    = MIPS<MIPSVersion::mars, Endian::little>;

using MIPSI_BE   = MIPS<MIPSVersion::I, Endian::big>;
using MIPSII_BE  = MIPS<MIPSVersion::II, Endian::big>;
using MIPS32_BE  = MIPS<MIPSVersion::v32, Endian::big>;
using MARS_BE    = MIPS<MIPSVersion::mars, Endian::big>;

// FIXME(pkryukov) Switch to BE since 2019
using MIPSI  = MIPSI_LE;
using MIPSII = MIPSII_LE;
using MIPS32 = MIPS32_LE;
using MARS   = MARS_LE;

// 64 bit MIPS
using MIPSIII_LE = MIPS<MIPSVersion::III, Endian::little>;
using MIPSIV_LE  = MIPS<MIPSVersion::IV, Endian::little>;
using MIPS64_LE  = MIPS<MIPSVersion::v64, Endian::little>;
using MARS64_LE  = MIPS<MIPSVersion::mars64, Endian::little>;

using MIPSIII_BE = MIPS<MIPSVersion::III, Endian::big>;
using MIPSIV_BE  = MIPS<MIPSVersion::IV, Endian::big>;
using MIPS64_BE  = MIPS<MIPSVersion::v64, Endian::big>;
using MARS64_BE  = MIPS<MIPSVersion::mars64, Endian::big>;

// FIXME(pkryukov) Switch to BE since 2019
using MIPSIII = MIPSIII_LE;
using MIPSIV  = MIPSIV_LE;
using MIPS64  = MIPS64_LE;
using MARS64  = MARS64_LE;

#endif // MIPS_H_
