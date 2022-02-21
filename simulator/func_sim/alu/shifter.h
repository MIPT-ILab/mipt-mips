/*
 * shifter.h - implementation of shifter unit
 * Copyright 2014-2022 MIPT-MIPS
 */
 
#ifndef SHIFTER_H
#define SHIFTER_H

#include <func_sim/operation.h>
#include <infra/uint128.h>

/*
 * Performs an arithmetic right shift, i.e. shift with progapating
 * the most significant bit.
 * 0xF0 sra 2 -> 0xFC
 */
template<Unsigned T> constexpr T arithmetic_rs( T value, size_t shamt) { return signify( value) >> shamt; }
template<Unsigned T> constexpr T circ_ls( T x, size_t shamt) { return std::rotl( x, sign_cast<int>( shamt)); }
template<Unsigned T> constexpr T circ_rs( T x, size_t shamt) { return std::rotr( x, sign_cast<int>( shamt)); }

// Boost.Multiprecision does not define right shift for signed values.
inline uint128 arithmetic_rs( uint128 value, size_t shamt)
{
    return signify( value) > 0 ? value >> shamt : ones_rs( value, shamt);
}

inline uint128 circ_ls( uint128 x, size_t shamt)
{
    if ( shamt == 0 || shamt == bitwidth<uint128>)
        return x;
    return ( x << shamt) | ( x >> ( bitwidth<uint128> - shamt));
}

inline uint128 circ_rs( uint128 x, size_t shamt)
{
    return circ_ls( std::move( x), bitwidth<uint128> - shamt);
}

struct Shifter
{
private:
    static inline auto gen_reverse( Unsigned auto value, size_t shamt)
    {
        using T = decltype(value);
        for ( size_t i = 0; i < log_bitwidth<T>; ++i) {
            const auto shift = 1U << i;
            if ( ( shamt & shift) != 0)
                value = ( ( value & interleaved_mask<T>( i)) << shift) | ( ( value & ~interleaved_mask<T>( i)) >> shift);
        }

        return value;
    }

    static inline auto gen_or_combine( Unsigned auto value, size_t shamt)
    {
        using T = decltype(value);
        for ( size_t i = 0; i < log_bitwidth<T>; ++i) {
            const auto shift = 1U << i;
            if ( ( shamt & shift) != 0)
                value |= ( ( value & interleaved_mask<T>( i)) << shift) | ( (value & ~interleaved_mask<T>( i)) >> shift);
        }

        return value;
    }

    // Shift value extractors
    static size_t shamt_imm( const Executable auto* instr) { return narrow_cast<size_t>( instr->v_imm); }
    static size_t shamt_imm_32( const Executable auto* instr) { return narrow_cast<size_t>( instr->v_imm) + 32U; }

    template<Executable I, Unsigned T = typename I::RegisterUInt> static
    size_t shamt_v_src2( const I* instr)
    {
        return narrow_cast<size_t>( instr->v_src[1] & bitmask<size_t>(log_bitwidth<T>));
    }

    static auto shamt_v_src2_mask( const Executable auto* instr)
    {
        using UInt = std::decay_t<decltype(instr->v_src[0])>;
        return UInt{1} << shamt_v_src2( instr);
    }

    static auto shamt_imm_mask( const Executable auto* instr)
    {
        using UInt = std::decay_t<decltype(instr->v_src[0])>;
        auto index = shamt_imm( instr) & ( bitwidth<UInt> - 1);
        return UInt{1} << index;
    }

public:
    // Logical shifts
    template<Executable I, Unsigned T = typename I::RegisterUInt>
    static void sll( I* instr)  { instr->v_dst[0] = sign_extension<bitwidth<T>>( ( instr->v_src[0] & all_ones<T>()) << shamt_imm( instr)); }

    template<Executable I, Unsigned T = typename I::RegisterUInt>
    static void srl( I* instr)  { instr->v_dst[0] = sign_extension<bitwidth<T>>( ( instr->v_src[0] & all_ones<T>()) >> shamt_imm( instr)); }

    template<Executable I, Unsigned T = typename I::RegisterUInt>
    static void sllv( I* instr) { instr->v_dst[0] = sign_extension<bitwidth<T>>( ( instr->v_src[0] & all_ones<T>()) << shamt_v_src2<I, T>( instr)); }

    template<Executable I, Unsigned T = typename I::RegisterUInt>
    static void srlv( I* instr) { instr->v_dst[0] = sign_extension<bitwidth<T>>( ( instr->v_src[0] & all_ones<T>()) >> shamt_v_src2<I, T>( instr)); }

    // Arithmetic shifts
    template<Executable I, Unsigned T = typename I::RegisterUInt>
    static void sra( I* instr)  { instr->v_dst[0] = arithmetic_rs( sign_extension<bitwidth<T>>( instr->v_src[0]), shamt_imm( instr)); }

    template<Executable I, Unsigned T = typename I::RegisterUInt>
    static void srav( I* instr) { instr->v_dst[0] = arithmetic_rs( sign_extension<bitwidth<T>>( instr->v_src[0]), shamt_v_src2<I, T>( instr)); }

    // MIPS extra shifts
    static void dsll32( Executable auto* instr) { instr->v_dst[0] = instr->v_src[0] << shamt_imm_32( instr); }
    static void dsrl32( Executable auto* instr) { instr->v_dst[0] = instr->v_src[0] >> shamt_imm_32( instr); }
    static void dsra32( Executable auto* instr) { instr->v_dst[0] = arithmetic_rs( instr->v_src[0], shamt_imm_32( instr)); }

    // Circular shifts
    static void rol( Executable auto* instr)  { instr->v_dst[0] = circ_ls( instr->v_src[0], shamt_v_src2( instr)); }
    static void ror( Executable auto* instr)  { instr->v_dst[0] = circ_rs( instr->v_src[0], shamt_v_src2( instr)); }
    static void rori( Executable auto* instr) { instr->v_dst[0] = circ_rs( instr->v_src[0], shamt_imm( instr)); }

    // Ones shifts
    static void slo( Executable auto* instr)  { instr->v_dst[0] = ones_ls( instr->v_src[0], shamt_v_src2( instr)); }
    static void sloi( Executable auto* instr) { instr->v_dst[0] = ones_ls( instr->v_src[0], shamt_imm( instr)); }
    static void sro( Executable auto* instr)  { instr->v_dst[0] = ones_rs( instr->v_src[0], shamt_v_src2( instr)); }
    static void sroi( Executable auto* instr) { instr->v_dst[0] = ones_rs( instr->v_src[0], shamt_imm( instr)); }

    // Generalized OR-Combine and reverse
    static void gorc( Executable auto* instr) { instr->v_dst[0] = gen_or_combine( instr->v_src[0], shamt_v_src2( instr)); }
    static void grev( Executable auto* instr) { instr->v_dst[0] = gen_reverse( instr->v_src[0], shamt_v_src2( instr)); }

    static void orc_b( Executable auto* instr )
    {
        static constexpr size_t gorci_orc_b_shamt = 4 | 2 | 1;
        instr->v_dst[0] = gen_or_combine( instr->v_src[0], gorci_orc_b_shamt);
    }

    // Bit clear, set, invert, extract
    static void bclr( Executable auto* instr)  { instr->v_dst[0] = instr->v_src[0] & ~shamt_v_src2_mask(instr); }
    static void bclri( Executable auto* instr) { instr->v_dst[0] = instr->v_src[0] & ~shamt_imm_mask(instr); }
    static void bset( Executable auto* instr)  { instr->v_dst[0] = instr->v_src[0] | shamt_v_src2_mask(instr); }
    static void bseti( Executable auto* instr) { instr->v_dst[0] = instr->v_src[0] | shamt_imm_mask(instr); }
    static void binv( Executable auto* instr)  { instr->v_dst[0] = instr->v_src[0] ^ shamt_v_src2_mask(instr); }
    static void bext( Executable auto* instr)  { instr->v_dst[0] = (instr->v_src[0] & shamt_v_src2_mask(instr)) == 0 ? 0 : 1; }
};

#endif
