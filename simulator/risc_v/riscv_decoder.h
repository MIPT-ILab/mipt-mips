/**
 * riscv_decoder.h - instruction decoder for risc-v
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2019 MIPT-MIPS
 */
 
#ifndef RISCV_DECODER_H
#define RISCV_DECODER_H

#include <infra/macro.h>
#include <infra/types.h>
#include <risc_v/riscv_register/riscv_register.h>

#include <cassert>

enum class Reg : uint8
{
    RS1, RS2, RD,
    CSR, SEPC, MEPC,
    ZERO, RA
};

struct RISCVInstrDecoder
{
    const uint32 sz;
    const uint32 rd;
    const uint32 rs1;
    const uint32 rs2;
    const uint32 I_imm;
    const uint32 S_imm4_0;
    const uint32 S_imm11_5;
    const uint32 B_imm11;
    const uint32 B_imm4_1;
    const uint32 B_imm10_5;
    const uint32 B_imm12;
    const uint32 U_imm;
    const uint32 J_imm19_12;
    const uint32 J_imm11;
    const uint32 J_imm10_1;
    const uint32 J_imm20;
    const uint32 csr_imm;
    const uint32 csr;
    const uint32 bytes;

    constexpr uint32 get_B_immediate() const noexcept
    {
        return (B_imm4_1  << 1U)
            |  (B_imm10_5 << 5U)
            |  (B_imm11   << 11U)
            |  (B_imm12 == 1 ? all_ones<uint32>() << 12U : 0);
    }

    constexpr uint32 get_J_immediate() const noexcept
    {
        return (J_imm10_1  << 1U)
            |  (J_imm11    << 11U)
            |  (J_imm19_12 << 12U)
            |  (J_imm20 == 1 ? all_ones<uint32>() << 20U : 0);
    }

    uint32 get_immediate(char subset) const noexcept
    {
        switch (subset) {
        case 'I': return I_imm;
        case 'B': return get_B_immediate();
        case 'S': return S_imm4_0 | (S_imm11_5 << 5);
        case 'U': return U_imm;
        case 'J': return get_J_immediate();
        case 'C': return csr_imm;
        case ' ': return 0;
        default: assert(0); return 0;
        }
    }

    RISCVRegister get_register( Reg type) const noexcept
    {
        switch ( type) {
        case Reg::ZERO:   return RISCVRegister::zero();
        case Reg::RA:     return RISCVRegister::return_address();
        case Reg::RS1:    return RISCVRegister::from_cpu_index( rs1);
        case Reg::RS2:    return RISCVRegister::from_cpu_index( rs2);
        case Reg::RD:     return RISCVRegister::from_cpu_index( rd);
        case Reg::CSR:    return RISCVRegister::from_csr_index( csr);
        case Reg::SEPC:   return RISCVRegister::from_csr_index( 0x141);
        case Reg::MEPC:   return RISCVRegister::from_csr_index( 0x341);
        default: assert(0);  return RISCVRegister::zero();
        }
    }

    static constexpr uint32 apply_mask(uint32 bytes, uint32 mask) noexcept
    {
        return ( bytes & mask) >> find_first_set( mask);
    }

    explicit constexpr RISCVInstrDecoder(uint32 raw) noexcept
        : sz        ( apply_mask( raw, 0b00000000'00000000'00000000'00000011))
        , rd        ( apply_mask( raw, 0b00000000'00000000'00001111'10000000))
        , rs1       ( apply_mask( raw, 0b00000000'00001111'10000000'00000000))
        , rs2       ( apply_mask( raw, 0b00000001'11110000'00000000'00000000))
        , I_imm     ( apply_mask( raw, 0b11111111'11110000'00000000'00000000))
        , S_imm4_0  ( apply_mask( raw, 0b00000000'00000000'00001111'10000000))
        , S_imm11_5 ( apply_mask( raw, 0b11111110'00000000'00000000'00000000))
        , B_imm11   ( apply_mask( raw, 0b00000000'00000000'00000000'10000000))
        , B_imm4_1  ( apply_mask( raw, 0b00000000'00000000'00001111'00000000))
        , B_imm10_5 ( apply_mask( raw, 0b01111110'00000000'00000000'00000000))
        , B_imm12   ( apply_mask( raw, 0b10000000'00000000'00000000'00000000))
        , U_imm     ( apply_mask( raw, 0b11111111'11111111'11110000'00000000))
        , J_imm19_12( apply_mask( raw, 0b00000000'00001111'11110000'00000000))
        , J_imm11   ( apply_mask( raw, 0b00000000'00010000'00000000'00000000))
        , J_imm10_1 ( apply_mask( raw, 0b01111111'11100000'00000000'00000000))
        , J_imm20   ( apply_mask( raw, 0b10000000'00000000'00000000'00000000))
        , csr_imm   ( apply_mask( raw, 0b00000000'00001111'10000000'00000000))
        , csr       ( apply_mask( raw, 0b11111111'11110000'00000000'00000000))
        , bytes     ( apply_mask( raw, 0b11111111'11111111'11111111'11111111))
    { }
};

#endif // RISCV_DECODER_H
