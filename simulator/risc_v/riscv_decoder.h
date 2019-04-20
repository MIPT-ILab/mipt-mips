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
    RS1, RS2, RS2_COMPR, RD,
    RS1_3_BITS, RS2_3_BITS, RD_3_BITS,
    CSR, SEPC, MEPC,
    ZERO, RA, SP
};

enum ImmediateType
{
    I = 'I',
    B = 'B',
    S = 'S',
    U = 'U',
    J = 'J',
    C = 'C',
    NONE       = ' ', // ASCII code of ' ' is 32
    C_LWSP     = 33,  // ASCII code of the first letter is 65
    C_LDSP     = 34,
    C_LQSP     = 35,
    C_SWSP     = 36,
    C_SDSP     = 37,
    C_SQSP     = 38,
    C_LW       = 39,  // C_LW and C_SW are similar
    C_SW       = 39,
    C_LD       = 40,
    C_SD       = 40,
    C_ADDI     = 41,
    C_LI       = 41,
    C_ANDI     = 41,
    C_ADDIW    = 41,
    C_J        = 42,
    C_JAL      = 42,
    C_LUI      = 43,
    C_SRLI     = 44,
    C_SRAI     = 44,
    C_SLLI     = 44,
    C_BEQZ     = 45,
    C_BNEZ     = 45,
    C_ADDI4SPN = 46,
    C_ADDI16SP = 47
};

struct RISCVInstrDecoder
{
    const uint32 sz;
    const uint32 rd;
    const uint32 rs1;
    const uint32 rs2;
    const uint32 rs2_compr; // rs2 is in another place for 16-bit instrs
    const uint32 rd_3_bits;
    const uint32 rs1_3_bits;
    const uint32 rs2_3_bits;
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
    const uint32 Compr_imm12;   // 1-bit immediate for Compressed ISA subset
    const uint32 Compr_imm6_5;
    const uint32 Compr_imm12_10;
    const uint32 Compr_imm6_2;
    const uint32 Compr_imm12_7;
    const uint32 Compr_imm12_5;
    const uint32 Compr_imm12_2;

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

    uint32 get_immediate_value( char subset) const noexcept
    {
        switch (subset) {
        case 'I': return I_imm;
        case 'B': return get_B_immediate();
        case 'S': return S_imm4_0 | (S_imm11_5 << 5U);
        case 'U': return U_imm;
        case 'J': return get_J_immediate();
        case 'C': return csr_imm;
        case ' ': return 0;
        default: return get_compressed_immediate_value( subset);
        }
    }

    uint32 get_compressed_immediate_value( char subset) const noexcept
    {
        switch (subset) {
            case C_LWSP: return ( apply_mask( Compr_imm12, 0b1) << 5U)
                              | ( apply_mask( Compr_imm6_2, 0b11100) << 2U)
                              | ( apply_mask( Compr_imm6_2, 0b00011) << 6U);

            case C_LDSP: return ( apply_mask( Compr_imm12, 0b1) << 5U)
                              | ( apply_mask( Compr_imm6_2, 0b11000) << 3U)
                              | ( apply_mask( Compr_imm6_2, 0b00111) << 6U);

            case C_LQSP: return ( apply_mask( Compr_imm12, 0b1) << 5U)
                              | ( apply_mask( Compr_imm6_2, 0b10000) << 4U)
                              | ( apply_mask( Compr_imm6_2, 0b01111) << 6U);

            case C_SWSP: return ( apply_mask( Compr_imm12_7, 0b111100) << 2U)
                              | ( apply_mask( Compr_imm12_7, 0b000011) << 6U);

            case C_SDSP: return ( apply_mask( Compr_imm12_7, 0b111000) << 3U)
                              | ( apply_mask( Compr_imm12_7, 0b000111) << 6U);

            case C_SQSP: return ( apply_mask( Compr_imm12_7, 0b110000) << 4U)
                              | ( apply_mask( Compr_imm12_7, 0b001111) << 6U);

            case C_LW:   return ( apply_mask( Compr_imm6_5, 0b10) << 2U)
                              | ( apply_mask( Compr_imm6_5, 0b01) << 6U)
                              | ( apply_mask( Compr_imm12_10, 0b111) << 3U);

            case C_LD:   return ( apply_mask( Compr_imm12_10, 0b111) << 3U)
                              | ( apply_mask( Compr_imm6_5, 0b11) << 6U);

            case C_ADDI: return sign_extension<6>( ( apply_mask( Compr_imm12, 0b1) << 5U)
                                                 | ( apply_mask( Compr_imm6_2, 0b11111)));

            case C_J:    return sign_extension<12>( ( apply_mask( Compr_imm12_2, 0b10000000000) << 11U)
                                                  | ( apply_mask( Compr_imm12_2, 0b01000000000) << 4U)
                                                  | ( apply_mask( Compr_imm12_2, 0b00110000000) << 8U)
                                                  | ( apply_mask( Compr_imm12_2, 0b00001000000) << 10U)
                                                  | ( apply_mask( Compr_imm12_2, 0b00000100000) << 6U)
                                                  | ( apply_mask( Compr_imm12_2, 0b00000010000) << 7U)
                                                  | ( apply_mask( Compr_imm12_2, 0b00000001110) << 1U)
                                                  | ( apply_mask( Compr_imm12_2, 0b00000000001) << 5U));

            case C_LUI:  return ( apply_mask( Compr_imm12, 0b1) * 0xfffe0)
                                | apply_mask( Compr_imm6_2, 0b11111);

            case C_SRLI: return ( apply_mask( Compr_imm12, 0b1) << 5U)
                              | ( apply_mask( Compr_imm6_2, 0b11111));

            case C_BEQZ: return sign_extension<9>( ( apply_mask( Compr_imm12_10, 0b100) << 8U)
                                                 | ( apply_mask( Compr_imm12_10, 0b011) << 3U)
                                                 | ( apply_mask( Compr_imm6_2, 0b11000) << 6U)
                                                 | ( apply_mask( Compr_imm6_2, 0b00110) << 1U)
                                                 | ( apply_mask( Compr_imm6_2, 0b00001) << 5U));

            case C_ADDI4SPN: return ( apply_mask( Compr_imm12_5, 0b11000000) << 4U)
                                  | ( apply_mask( Compr_imm12_5, 0b00111100) << 6U)
                                  | ( apply_mask( Compr_imm12_5, 0b00000010) << 2U)
                                  | ( apply_mask( Compr_imm12_5, 0b00000001) << 3U);

            case C_ADDI16SP: return sign_extension<10>( ( apply_mask( Compr_imm12, 0b1) << 9U)
                                                      | ( apply_mask( Compr_imm6_2, 0b10000) << 4U)
                                                      | ( apply_mask( Compr_imm6_2, 0b01000) << 6U)
                                                      | ( apply_mask( Compr_imm6_2, 0b00110) << 7U)
                                                      | ( apply_mask( Compr_imm6_2, 0b00001) << 5U));

            default:     assert(0); return 0;
        }
    }

    template<typename R>
    static R get_immediate( char subset, uint32 value) noexcept
    {
        switch (subset) {
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
        case Reg::ZERO:           return RISCVRegister::zero();
        case Reg::RA:             return RISCVRegister::return_address();
        case Reg::SP:             return RISCVRegister::from_cpu_index( 2);
        case Reg::RS1:            return RISCVRegister::from_cpu_index( rs1);
        case Reg::RS2:            return RISCVRegister::from_cpu_index( rs2);
        case Reg::RS2_COMPR:      return RISCVRegister::from_cpu_index( rs2_compr);
        case Reg::RD:             return RISCVRegister::from_cpu_index( rd);
        case Reg::RS1_3_BITS:     return RISCVRegister::from_cpu_popular_index( rs1_3_bits);
        case Reg::RS2_3_BITS:     return RISCVRegister::from_cpu_popular_index( rs2_3_bits);
        case Reg::RD_3_BITS:      return RISCVRegister::from_cpu_popular_index( rd_3_bits);
        case Reg::CSR:            return RISCVRegister::from_csr_index( csr);
        case Reg::SEPC:           return RISCVRegister::from_csr_index( 0x141);
        case Reg::MEPC:           return RISCVRegister::from_csr_index( 0x341);
        default:       assert(0); return RISCVRegister::zero();
        }
    }

    static constexpr uint32 apply_mask(uint32 bytes, uint32 mask) noexcept
    {
        return ( bytes & mask) >> find_first_set( mask);
    }

    explicit constexpr RISCVInstrDecoder(uint32 raw) noexcept
        : sz             ( apply_mask( raw, 0b00000000'00000000'00000000'00000011))
        , rd             ( apply_mask( raw, 0b00000000'00000000'00001111'10000000))
        , rs1            ( apply_mask( raw, 0b00000000'00001111'10000000'00000000))
        , rs2            ( apply_mask( raw, 0b00000001'11110000'00000000'00000000))
        , rs2_compr      ( apply_mask( raw, 0b00000000'00000000'00000000'01111100))
        , rd_3_bits      ( apply_mask( raw, 0b00000000'00000000'00000000'00011100))
        , rs1_3_bits     ( apply_mask( raw, 0b00000000'00000000'00000011'10000000))
        , rs2_3_bits     ( apply_mask( raw, 0b00000000'00000000'00000000'00011100))
        , I_imm          ( apply_mask( raw, 0b11111111'11110000'00000000'00000000))
        , S_imm4_0       ( apply_mask( raw, 0b00000000'00000000'00001111'10000000))
        , S_imm11_5      ( apply_mask( raw, 0b11111110'00000000'00000000'00000000))
        , B_imm11        ( apply_mask( raw, 0b00000000'00000000'00000000'10000000))
        , B_imm4_1       ( apply_mask( raw, 0b00000000'00000000'00001111'00000000))
        , B_imm10_5      ( apply_mask( raw, 0b01111110'00000000'00000000'00000000))
        , B_imm12        ( apply_mask( raw, 0b10000000'00000000'00000000'00000000))
        , U_imm          ( apply_mask( raw, 0b11111111'11111111'11110000'00000000))
        , J_imm19_12     ( apply_mask( raw, 0b00000000'00001111'11110000'00000000))
        , J_imm11        ( apply_mask( raw, 0b00000000'00010000'00000000'00000000))
        , J_imm10_1      ( apply_mask( raw, 0b01111111'11100000'00000000'00000000))
        , J_imm20        ( apply_mask( raw, 0b10000000'00000000'00000000'00000000))
        , csr_imm        ( apply_mask( raw, 0b00000000'00001111'10000000'00000000))
        , csr            ( apply_mask( raw, 0b11111111'11110000'00000000'00000000))
        , bytes          ( apply_mask( raw, 0b11111111'11111111'11111111'11111111))
        , Compr_imm12    ( apply_mask( raw, 0b00000000'00000000'00010000'00000000))
        , Compr_imm6_5   ( apply_mask( raw, 0b00000000'00000000'00000000'01100000))
        , Compr_imm12_10 ( apply_mask( raw, 0b00000000'00000000'00011100'00000000))
        , Compr_imm6_2   ( apply_mask( raw, 0b00000000'00000000'00000000'01111100))
        , Compr_imm12_7  ( apply_mask( raw, 0b00000000'00000000'00011111'10000000))
        , Compr_imm12_5  ( apply_mask( raw, 0b00000000'00000000'00011111'11100000))
        , Compr_imm12_2  ( apply_mask( raw, 0b00000000'00000000'00011111'11111100))
    { }
};

#endif // RISCV_DECODER_H
