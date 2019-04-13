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
    RS1_, RS2_, RD_,
    CSR, SEPC, MEPC,
    ZERO, RA, SP
};

enum ImmediateType
{
    I, B, S, U, J, C, NONE,
    C_LWSP, C_LDSP, C_LQSP,
    C_SWSP, C_SDSP, C_SQSP,
    C_LW, C_SW, C_ADDI
};

struct RISCVInstrDecoder
{
    const uint32 sz;
    const uint32 rd;
    const uint32 rs1;
    const uint32 rs2;
    const uint16 rd_;
    const uint16 rs1_;
    const uint16 rs2_;
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
    const uint16 Cx_imm1;   // 1-bit immediate for Compressed ISA subset
    const uint16 Cx_imm2;
    const uint16 Cx_imm3;
    const uint16 Cx_imm5;
    const uint16 Cx_imm6;

    constexpr uint32 get_B_immediate() const noexcept
    {
        return (B_imm4_1  << 1U)
            |  (B_imm10_5 << 5U)
            |  (B_imm11   << 11U)
            |  (B_imm12   << 12U);
    }

    constexpr uint32 get_J_immediate() const noexcept
    {
        return (J_imm10_1  << 1U)
            |  (J_imm11    << 11U)
            |  (J_imm19_12 << 12U)
            |  (J_imm20    << 20U);
    }

    uint32 get_immediate_value( ImmediateType subset) const noexcept
    {
        switch (subset) {
        case I: return I_imm;
        case B: return get_B_immediate();
        case S: return S_imm4_0 | (S_imm11_5 << 5U);
        case U: return U_imm;
        case J: return get_J_immediate();
        case C: return csr_imm;
        case NONE: return 0;
        default: return get_compressed_immediate_value( subset);
        }
    }

    uint16 get_compressed_immediate_value( ImmediateType type) const noexcept
    {
        switch (type) {
            case C_LWSP: return ( apply_mask( Cx_imm1, 0b1) << 5U)
                            | ( apply_mask( Cx_imm5, 0b11100) << 2U)
                            | ( apply_mask( Cx_imm5, 0b00011) << 6U);

            case C_LDSP: return ( apply_mask( Cx_imm1, 0b1) << 5U)
                            | ( apply_mask( Cx_imm5, 0b11000) << 3U)
                            | ( apply_mask( Cx_imm5, 0b00111) << 6U);

            case C_LQSP: return ( apply_mask( Cx_imm1, 0b1) << 5U)
                            | ( apply_mask( Cx_imm5, 0b10000) << 4U)
                            | ( apply_mask( Cx_imm5, 0b01111) << 6U);

            case C_SWSP: return ( apply_mask( Cx_imm6, 0b111100) << 2U)
                            | ( apply_mask( Cx_imm6, 0b000011) << 6U);

            case C_SDSP: return ( apply_mask( Cx_imm6, 0b111000) << 3U)
                            | ( apply_mask( Cx_imm6, 0b000111) << 6U);

            case C_SQSP: return ( apply_mask( Cx_imm6, 0b110000) << 4U)
                            | ( apply_mask( Cx_imm6, 0b001111) << 6U);

            case C_LW:   return ( apply_mask( Cx_imm2, 0b10) << 2U)
                            | ( apply_mask( Cx_imm2, 0b01) << 6U)
                            | ( apply_mask( Cx_imm3, 0b111) << 3U);

            case C_SW:   return ( apply_mask( Cx_imm2, 0b10) << 2U)
                            | ( apply_mask( Cx_imm2, 0b01) << 6U)
                            | ( apply_mask( Cx_imm3, 0b111) << 3U);

            case C_ADDI: return ( apply_mask( Cx_imm1, 0b1) << 5U)
                            | ( apply_mask( Cx_imm5, 0b11111));

            default:     assert(0); return 0;
        }
    }

    template<typename R>
    static R get_immediate( ImmediateType type, uint32 value) noexcept
    {
        switch (type) {
        case I:
        case B:
        case S: return sign_extension<12, R>( value);
        case U:
        case J: return sign_extension<20, R>( value);
        default:  return value;
        }
    }

    RISCVRegister get_register( Reg type) const noexcept
    {
        switch ( type) {
        case Reg::ZERO:   return RISCVRegister::zero();
        case Reg::RA:     return RISCVRegister::return_address();
        case Reg::SP:     return RISCVRegister::from_cpu_index( 2);
        case Reg::RS1:    return RISCVRegister::from_cpu_index( rs1);
        case Reg::RS2:    return RISCVRegister::from_cpu_index( rs2);
        case Reg::RD:     return RISCVRegister::from_cpu_index( rd);
        case Reg::RS1_:   return RISCVRegister::from_cpu_index( rs1_, RISCVRegister::separator::popular);
        case Reg::RS2_:   return RISCVRegister::from_cpu_index( rs2_, RISCVRegister::separator::popular);
        case Reg::RD_:    return RISCVRegister::from_cpu_index( rd_, RISCVRegister::separator::popular);
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
        , rs1       ( apply_mask( raw, 0b11111111'11111111'00000000'00000000)     // If instr is 16-bit,
                      ? apply_mask( raw, 0b00000000'00001111'10000000'00000000) // then rs1 is in other place
                      : apply_mask( raw, 0b00000000'00000000'00001111'10000000))
        , rs2       ( apply_mask( raw, 0b11111111'11111111'00000000'00000000)     // If instr is 16-bit,
                        ? apply_mask( raw, 0b00000001'11110000'00000000'00000000) // then rs2 is in other place
                        : apply_mask( raw, 0b00000000'00000000'00000000'01111100))
        , rd_       ( apply_mask( raw, 0b00000000'00000000'00000000'00011100))
        , rs1_      ( apply_mask( raw, 0b00000000'00000000'00000011'10000000))
        , rs2_      ( apply_mask( raw, 0b00000000'00000000'00000000'00011100))
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
        , Cx_imm1   ( apply_mask( raw, 0b00000000'00000000'00010000'00000000))
        , Cx_imm2   ( apply_mask( raw, 0b00000000'00000000'00000000'01100000))
        , Cx_imm3   ( apply_mask( raw, 0b00000000'00000000'00011100'00000000))
        , Cx_imm5   ( apply_mask( raw, 0b00000000'00000000'00000000'01111100))
        , Cx_imm6   ( apply_mask( raw, 0b00000000'00000000'00011111'10000000))
    { }
};

#endif // RISCV_DECODER_H
