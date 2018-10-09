/**
 * mips.h - all the aliases to MIPS ISA
 * @author Aleksandr Misevich
 * Copyright 2018 MIPT-MIPS
 */

#ifndef MIPS_H_
#define MIPS_H_

#include "mips_instr.h"

#include <infra/instrcache/instr_cache_memory.h>

template<MIPSVersion version>
struct MIPS
{
    using Register = MIPSRegister;
    using RegisterUInt = MIPSRegisterUInt<version>;
    using FuncInstr = MIPSInstr<version>;
    using Memory = InstrMemoryCached<FuncInstr>;
    static const auto& get_instr( uint32 bytes, Addr PC) {
        return FuncInstr( version, bytes, PC);
    }
};

// 32 bit MIPS
using MIPSI   = MIPS<MIPSVersion::I>;
using MIPSII  = MIPS<MIPSVersion::II>;
using MIPS32  = MIPS<MIPSVersion::v32>;

// 64 bit MIPS
using MIPSIII = MIPS<MIPSVersion::III>;
using MIPSIV  = MIPS<MIPSVersion::IV>;
using MIPS64  = MIPS<MIPSVersion::v64>;

static_assert( std::is_same_v<MIPS32Instr, MIPS32::FuncInstr>);
static_assert( std::is_same_v<MIPS64Instr, MIPS64::FuncInstr>);

#endif // MIPS_H_
