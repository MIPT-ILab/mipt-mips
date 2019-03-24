/**
 * mips_instr_decode.h - instruction decoder for mips
 * @author Andrey Agrachev agrachev.af@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#ifndef MIPS_INSTR_DECODE_H
#define MIPS_INSTR_DECODE_H

#include <infra/macro.h>
#include <infra/types.h>

#include "mips_register/mips_register.h"

enum class Reg : uint8
{
    RS, RT, RD,
    CP0_RT, CP0_RD,
    CP1_FT, CP1_FS,
    CP1_FD,
    ZERO, RA,
    HI, LO, HI_LO
};

using Src1 = Reg;
using Src2 = Reg;
using Dst  = Reg;

static inline bool is_explicit_register( Reg type)
{
    return type == Reg::RS
        || type == Reg::RT
        || type == Reg::RD
        || type == Reg::CP0_RT
        || type == Reg::CP0_RD
        || type == Reg::CP1_FT
        || type == Reg::CP1_FS
        || type == Reg::CP1_FD;
}

struct MIPSInstrDecoder
{
    const uint32 funct;
    const uint32 shamt;
    const uint32 rd;
    const uint32 rt;
    const uint32 rs;
    const uint32 opcode;
    const uint32 imm;
    const uint32 jump;
    const uint32 bytes;
    const uint32 fd;
    const uint32 fs;
    const uint32 ft;
    const uint32 fmt;

    static constexpr uint32 apply_mask(uint32 bytes, uint32 mask) noexcept
    {
        return ( bytes & mask) >> find_first_set( mask);
    }

    uint32 get_immediate_value( char type) const
    {
        switch ( type)
        {
        case 'N': return 0;
        case 'S': return shamt;
        case 'J': return jump;
        default:  return imm;
        }
    }

    template<typename R>
    static R get_immediate( char type, uint32 value) noexcept
    {
        switch ( type)
        {
        case 'N':
        case 'S':
        case 'J':
        case 'L': return value;
        default:  return sign_extension<16, R>( value);
        }
    }

    MIPSRegister get_register( Reg type) const
    {
        switch ( type) {
        case Reg::HI:     return MIPSRegister::mips_hi();
        case Reg::LO:     return MIPSRegister::mips_lo();
        case Reg::HI_LO:  return MIPSRegister::mips_lo();
        case Reg::ZERO:   return MIPSRegister::zero();
        case Reg::RA:     return MIPSRegister::return_address();
        case Reg::RS:     return MIPSRegister::from_cpu_index( rs);
        case Reg::RT:     return MIPSRegister::from_cpu_index( rt);
        case Reg::RD:     return MIPSRegister::from_cpu_index( rd);
        case Reg::CP0_RT: return MIPSRegister::from_cp0_index( rt);
        case Reg::CP0_RD: return MIPSRegister::from_cp0_index( rd);
        case Reg::CP1_FD: return MIPSRegister::from_cp1_index( fd);
        case Reg::CP1_FS: return MIPSRegister::from_cp1_index( fs);
        case Reg::CP1_FT: return MIPSRegister::from_cp1_index( ft);
        default: assert(0);  return MIPSRegister::zero();
        }
    }

    explicit constexpr MIPSInstrDecoder(uint32 raw) noexcept
        : funct  ( apply_mask( raw, 0b00000000'00000000'00000000'00111111))
        , shamt  ( apply_mask( raw, 0b00000000'00000000'00000111'11000000))
        , rd     ( apply_mask( raw, 0b00000000'00000000'11111000'00000000))
        , rt     ( apply_mask( raw, 0b00000000'00011111'00000000'00000000))
        , rs     ( apply_mask( raw, 0b00000011'11100000'00000000'00000000))
        , opcode ( apply_mask( raw, 0b11111100'00000000'00000000'00000000))
        , imm    ( apply_mask( raw, 0b00000000'00000000'11111111'11111111))
        , jump   ( apply_mask( raw, 0b00000011'11111111'11111111'11111111))
        , bytes  ( apply_mask( raw, 0b11111111'11111111'11111111'11111111))
        , fd     ( apply_mask( raw, 0b00000000'00000000'00000111'11000000)) 
        , fs     ( apply_mask( raw, 0b00000000'00000000'11111000'00000000))
        , ft     ( apply_mask( raw, 0b00000000'00011111'00000000'00000000))
        , fmt    ( apply_mask( raw, 0b00000011'11100000'00000000'00000000))
    { }
};

#endif // MIPS_INSTR_DECODE_H

