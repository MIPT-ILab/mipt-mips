/**
 * mips.h - all the aliases to MIPS ISA
 * @author Aleksandr Misevich
 * Copyright 2018 MIPT-MIPS
 */

#ifndef MIPS_H_
#define MIPS_H_

#include <infra/instrcache/instr_cache_memory.h>
#include "mips_instr.h"

template<MIPSVersion VERSION>
struct MIPS
{
    using Register = MIPSRegister;
    using RegisterUInt = MIPSRegisterUInt<VERSION>;
    using FuncInstr = MIPSInstr<VERSION>;
    using Memory = InstrMemory<FuncInstr>;
    static const auto& get_instr( uint32 bytes, Addr PC) {
        return FuncInstr( VERSION, bytes, PC);
    }
};

// 32 bit MIPS
using MIPSI   = MIPS<MIPS_I>;
using MIPSII  = MIPS<MIPS_II>;
using MIPS32  = MIPS<MIPS_32>;

// 64 bit MIPS
using MIPSIII = MIPS<MIPS_III>;
using MIPSIV  = MIPS<MIPS_IV>;
using MIPS64  = MIPS<MIPS_64>;

static_assert( std::is_same_v<MIPS32Instr, MIPS32::FuncInstr>);
static_assert( std::is_same_v<MIPS64Instr, MIPS64::FuncInstr>);

#endif // MIPS_H_
