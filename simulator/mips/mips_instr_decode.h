/**
 * mips_instr_decode.h - instruction decoder for mips
 * @author Andrey Agrachev agrachev.af@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#ifndef MIPS_INSTR_DECODE_H
#define MIPS_INSTR_DECODE_H

#include <infra/types.h>
#include <infra/macro.h>

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

    static constexpr uint32 apply_mask(uint32 bytes, uint32 mask) noexcept
    {
        return ( bytes & mask) >> find_first_set( mask);
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
    { }
};

#endif // MIPS_INSTR_DECODE_H

