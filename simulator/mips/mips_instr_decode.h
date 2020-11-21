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

enum class MIPSReg : uint8
{
    RS, RT, RD,
    CP0_RD, SR, EPC,
    FR, FT, FS, FD, FCSR,
    ZERO, RA,
    HI, LO, HI_LO
};

using Dst = MIPSReg;
using Src = MIPSReg;

static inline bool is_explicit_register( MIPSReg type)
{
    return type == MIPSReg::RS
        || type == MIPSReg::RT
        || type == MIPSReg::RD
        || type == MIPSReg::CP0_RD
        || type == MIPSReg::FR
        || type == MIPSReg::FT
        || type == MIPSReg::FS
        || type == MIPSReg::FD
        || type == MIPSReg::FCSR;
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
        // NOLINTNEXTLINE(hicpp-signed-bitwise) rhs must be positive
        return ( bytes & mask) >> std::countr_zero( mask);
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

    MIPSRegister get_register( MIPSReg type) const
    {
        switch ( type) {
        case MIPSReg::ZERO:   break;
        case MIPSReg::HI:     return MIPSRegister::mips_hi();
        case MIPSReg::LO:     return MIPSRegister::mips_lo();
        case MIPSReg::RA:     return MIPSRegister::return_address();
        case MIPSReg::RS:     return MIPSRegister::from_cpu_index( rs);
        case MIPSReg::RT:     return MIPSRegister::from_cpu_index( rt);
        case MIPSReg::RD:     return MIPSRegister::from_cpu_index( rd);
        case MIPSReg::CP0_RD: return MIPSRegister::from_cp0_index( rd);
        case MIPSReg::SR:     return MIPSRegister::status();
        case MIPSReg::EPC:    return MIPSRegister::epc();
        case MIPSReg::FD:     return MIPSRegister::from_cp1_index( fd);
        case MIPSReg::FS:     return MIPSRegister::from_cp1_index( fs);
        case MIPSReg::FT:     return MIPSRegister::from_cp1_index( ft);
        case MIPSReg::FR:     return MIPSRegister::from_cp1_index( fmt);
        case MIPSReg::FCSR:   return MIPSRegister::mips_fcsr();
        default: assert(0);
        }
        return MIPSRegister::zero();
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

