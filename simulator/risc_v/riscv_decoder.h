/**
 * riscv_decoder.h - instruction decoder for risc-v
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * @author Rustem Yunusov
 * Copyright 2019 MIPT-MIPS
 */
 
#ifndef RISCV_DECODER_H
#define RISCV_DECODER_H

#include <infra/macro.h>
#include <infra/types.h>
#include <risc_v/riscv_register/riscv_register.h>

#include <cassert>

struct Reg
{
    enum Type : uint8 {
        RS1, RS2, RS2_CMP, RD,
        RS1_3BIT, RS2_3BIT, RD_3BIT,
        CSR, SEPC, MEPC,
        ZERO, SP, RA,
        MAX_REG
    };
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
    C_LD       = 40,
    C_LQ       = 41,
    C_I        = 42,
    C_J        = 43,
    C_S        = 45,
    C_B        = 46,
    C_ADDI4SPN = 47,
    C_ADDI16SP = 48
};

struct RISCVInstrDecoder
{
    std::array<RISCVRegister, Reg::MAX_REG> registers = {};
    const uint32 bytes;
    const uint32 sz;
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
    const uint32 CI_imm_1;   // 1st immediate of CI-format instruction
    const uint32 CI_imm_2;
    const uint32 CSS_imm;
    const uint32 CIW_imm;
    const uint32 CL_imm_1;
    const uint32 CL_imm_2;
    const uint32 CJ_imm;

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

    constexpr uint32 get_C_LWSP_immediate() const noexcept
    {
        return ( apply_mask( CI_imm_1, 0b1) << 5U)
             | ( apply_mask( CI_imm_2, 0b11100) << 2U)
             | ( apply_mask( CI_imm_2, 0b00011) << 6U);
    }

    constexpr uint32 get_C_LDSP_immediate() const noexcept
    {
        return ( apply_mask( CI_imm_1, 0b1) << 5U)
             | ( apply_mask( CI_imm_2, 0b11000) << 3U)
             | ( apply_mask( CI_imm_2, 0b00111) << 6U);
    }

    constexpr uint32 get_C_LQSP_immediate() const noexcept
    {
        return ( apply_mask( CI_imm_1, 0b1) << 5U)
             | ( apply_mask( CI_imm_2, 0b10000) << 4U)
             | ( apply_mask( CI_imm_2, 0b01111) << 6U);
    }

    constexpr uint32 get_C_SWSP_immediate() const noexcept
    {
        return ( apply_mask( CSS_imm, 0b111100) << 2U)
             | ( apply_mask( CSS_imm, 0b000011) << 6U);
    }

    constexpr uint32 get_C_SDSP_immediate() const noexcept
    {
        return ( apply_mask( CSS_imm, 0b111000) << 3U)
             | ( apply_mask( CSS_imm, 0b000111) << 6U);
    }

    constexpr uint32 get_C_SQSP_immediate() const noexcept
    {
        return ( apply_mask( CSS_imm, 0b110000) << 4U)
             | ( apply_mask( CSS_imm, 0b001111) << 6U);
    }

    constexpr uint32 get_C_LW_immediate() const noexcept
    {
        return ( apply_mask( CL_imm_2, 0b10) << 2U)
             | ( apply_mask( CL_imm_2, 0b01) << 6U)
             | ( apply_mask( CL_imm_1, 0b111) << 3U);
    }

    constexpr uint32 get_C_LD_immediate() const noexcept
    {
        return ( apply_mask( CL_imm_1, 0b111) << 3U)
             | ( apply_mask( CL_imm_2, 0b11) << 6U);
    }

    constexpr uint32 get_C_LQ_immediate() const noexcept
    {
        return ( apply_mask( CL_imm_1, 0b110) << 4U)
             | ( apply_mask( CL_imm_1, 0b001) << 8U)
             | ( apply_mask( CL_imm_2, 0b11) << 6U);
    }

    constexpr uint32 get_C_I_immediate() const noexcept
    {
        return ( apply_mask( CI_imm_1, 0b1) << 5U)
             | ( apply_mask( CI_imm_2, 0b11111));
    }

    constexpr uint32 get_C_J_immediate() const noexcept
    {
        return ( apply_mask( CJ_imm, 0b10000000000) << 11U)
             | ( apply_mask( CJ_imm, 0b01000000000) << 4U)
             | ( apply_mask( CJ_imm, 0b00110000000) << 8U)
             | ( apply_mask( CJ_imm, 0b00001000000) << 10U)
             | ( apply_mask( CJ_imm, 0b00000100000) << 6U)
             | ( apply_mask( CJ_imm, 0b00000010000) << 7U)
             | ( apply_mask( CJ_imm, 0b00000001110) << 1U)
             | ( apply_mask( CJ_imm, 0b00000000001) << 5U);
    }

    constexpr uint32 get_C_B_immediate() const noexcept
    {
        return ( apply_mask( CL_imm_1, 0b100) << 8U)
             | ( apply_mask( CL_imm_1, 0b011) << 3U)
             | ( apply_mask( CI_imm_2, 0b11000) << 6U)
             | ( apply_mask( CI_imm_2, 0b00110) << 1U)
             | ( apply_mask( CI_imm_2, 0b00001) << 5U);
    }

    constexpr uint32 get_C_ADDI4SPN_immediate() const noexcept
    {
        return ( apply_mask( CIW_imm, 0b11000000) << 4U)
             | ( apply_mask( CIW_imm, 0b00111100) << 6U)
             | ( apply_mask( CIW_imm, 0b00000010) << 2U)
             | ( apply_mask( CIW_imm, 0b00000001) << 3U);
    }

    constexpr uint32 get_C_ADDI16SP_immediate() const noexcept
    {
        return ( apply_mask( CI_imm_1, 0b1) << 9U)
             | ( apply_mask( CI_imm_2, 0b10000) << 4U)
             | ( apply_mask( CI_imm_2, 0b01000) << 6U)
             | ( apply_mask( CI_imm_2, 0b00110) << 7U)
             | ( apply_mask( CI_imm_2, 0b00001) << 5U);
    }

    uint32 get_immediate_value( char subset) const noexcept
    {
        switch (subset) {
        case 'I': return I_imm;
        case '5': return I_imm & bitmask<uint32>( 5);
        case '6': return I_imm & bitmask<uint32>( 6);
        case '7': return I_imm & bitmask<uint32>( 7);
        case 'B': return get_B_immediate();
        case 'S': return S_imm4_0 | (S_imm11_5 << 5U);
        case 'U': return U_imm;
        case 'J': return get_J_immediate();
        case 'C': return csr_imm;
        case ' ': return 0;
        default:  return get_compressed_immediate_value( subset);
        }
    }

    uint32 get_compressed_immediate_value( char subset) const noexcept
    {
        switch (subset) {
        case C_LWSP: return get_C_LWSP_immediate();
        case C_LDSP: return get_C_LDSP_immediate();
        case C_LQSP: return get_C_LQSP_immediate();
        case C_SWSP: return get_C_SWSP_immediate();
        case C_SDSP: return get_C_SDSP_immediate();
        case C_SQSP: return get_C_SQSP_immediate();
        case C_LW:   return get_C_LW_immediate();
        case C_LD:   return get_C_LD_immediate();
        case C_LQ:   return get_C_LQ_immediate();
        case C_J:    return get_C_J_immediate();
        case C_B:    return get_C_B_immediate();
        case C_ADDI4SPN: return get_C_ADDI4SPN_immediate();
        case C_ADDI16SP: return get_C_ADDI16SP_immediate();
        case C_S:
        case C_I:    break;
        default:     assert( false);
        }
        return get_C_I_immediate();
    }

    template<typename R>
    static R get_immediate( char subset, uint32 value) noexcept
    {
        switch (subset) {
        case '5':
        case '6':
        case '7':        return value & bitmask<R>( log_bitwidth<R>);
        case 'I':
        case 'B':
        case 'S':        return sign_extension<12, R>( value);
        case 'U':
        case 'J':        return sign_extension<20, R>( value);
        case C_I:        return sign_extension<6, R>( value);
        case C_J:        return sign_extension<12, R>( value);
        case C_B:        return sign_extension<9, R>( value);
        case C_ADDI16SP: return sign_extension<10, R>( value);
        default:         return value;
        }
    }

    RISCVRegister get_register( Reg::Type type) const noexcept
    {
        return registers.at( type);
    }

    static constexpr uint32 apply_mask(uint32 bytes, uint32 mask) noexcept
    {
        // NOLINTNEXTLINE(hicpp-signed-bitwise) rhs must be positive
        return ( bytes & mask) >> std::countr_zero( mask);
    }

    explicit RISCVInstrDecoder(uint32 raw) noexcept
        : bytes      ( apply_mask( raw, 0b11111111'11111111'11111111'11111111))
        , sz         ( apply_mask( raw, 0b00000000'00000000'00000000'00000011))
        , I_imm      ( apply_mask( raw, 0b11111111'11110000'00000000'00000000))
        , S_imm4_0   ( apply_mask( raw, 0b00000000'00000000'00001111'10000000))
        , S_imm11_5  ( apply_mask( raw, 0b11111110'00000000'00000000'00000000))
        , B_imm11    ( apply_mask( raw, 0b00000000'00000000'00000000'10000000))
        , B_imm4_1   ( apply_mask( raw, 0b00000000'00000000'00001111'00000000))
        , B_imm10_5  ( apply_mask( raw, 0b01111110'00000000'00000000'00000000))
        , B_imm12    ( apply_mask( raw, 0b10000000'00000000'00000000'00000000))
        , U_imm      ( apply_mask( raw, 0b11111111'11111111'11110000'00000000))
        , J_imm19_12 ( apply_mask( raw, 0b00000000'00001111'11110000'00000000))
        , J_imm11    ( apply_mask( raw, 0b00000000'00010000'00000000'00000000))
        , J_imm10_1  ( apply_mask( raw, 0b01111111'11100000'00000000'00000000))
        , J_imm20    ( apply_mask( raw, 0b10000000'00000000'00000000'00000000))
        , csr_imm    ( apply_mask( raw, 0b00000000'00001111'10000000'00000000))
        , CI_imm_1   ( apply_mask( raw, 0b00010000'00000000))
        , CI_imm_2   ( apply_mask( raw, 0b00000000'01111100))
        , CSS_imm    ( apply_mask( raw, 0b00011111'10000000))
        , CIW_imm    ( apply_mask( raw, 0b00011111'11100000))
        , CL_imm_1   ( apply_mask( raw, 0b00011100'00000000))
        , CL_imm_2   ( apply_mask( raw, 0b00000000'01100000))
        , CJ_imm     ( apply_mask( raw, 0b00011111'11111100))
    {
        registers[Reg::ZERO] = RISCVRegister::zero();
        registers[Reg::SP]   = RISCVRegister::from_cpu_index( 2);
        registers[Reg::RA]   = RISCVRegister::from_cpu_index( 1);
        registers[Reg::SEPC] = RISCVRegister::from_csr_index( 0x141);
        registers[Reg::MEPC] = RISCVRegister::from_csr_index( 0x341);
        registers[Reg::RD]   = RISCVRegister::from_cpu_index( apply_mask( raw, 0b00000000'00000000'00001111'10000000));
        registers[Reg::RS1]  = RISCVRegister::from_cpu_index( apply_mask( raw, 0b00000000'00001111'10000000'00000000));
        registers[Reg::RS2]  = RISCVRegister::from_cpu_index( apply_mask( raw, 0b00000001'11110000'00000000'00000000));
        registers[Reg::CSR]  = RISCVRegister::from_csr_index( apply_mask( raw, 0b11111111'11110000'00000000'00000000));
        registers[Reg::RS2_CMP]  = RISCVRegister::from_cpu_index(      apply_mask( raw, 0b00000000'01111100));
        registers[Reg::RD_3BIT]  = RISCVRegister::from_cpu_popular_index( apply_mask( raw, 0b00000000'00011100));
        registers[Reg::RS1_3BIT] = RISCVRegister::from_cpu_popular_index( apply_mask( raw, 0b00000011'10000000));
        registers[Reg::RS2_3BIT] = RISCVRegister::from_cpu_popular_index( apply_mask( raw, 0b00000000'00011100));
    }
};

#endif // RISCV_DECODER_H
