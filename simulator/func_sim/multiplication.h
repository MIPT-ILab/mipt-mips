/*
* multiplication.h - implementation of riscv multiplication
* @author Yauheni Sharamed SHaramed.EI@phystech.edu
* Copyright 2014-2019 MIPT-MIPS
*/

#include <infra/macro.h>
#include <infra/types.h>

template<typename T>
constexpr bool is_signed_division_overflow(T x, T y)
{
    if constexpr (std::is_same<T, unsign_t<T>>()) {
        (void)x; (void)y;
        return false;
    }
    else {
        return y == -1 && x == narrow_cast<T>(msb_set<unsign_t<T>>());
    }
}

template<typename T>
auto mips_multiplication(T x, T y) {
    using UT = unsign_t<T>;
    using T2 = doubled_t<T>;
    using UT2 = unsign_t<T2>;
    auto value = narrow_cast<UT2>(T2{ x} * T2{ y});
    // With Boost < 1.68.0, result of narrowing cast of uint128 is undefined
    // if the value does not fit to the built-in target type (e.g. uint64)
    // To workaround that, we mask the value with full-ones mask first.
    auto lo = narrow_cast<UT>( value & all_ones<UT>());
    auto hi = narrow_cast<UT>( value >> bitwidth<T>);
    return std::pair{ lo, hi};
}

template<typename T>
auto mips_division(T x, T y) {
    using ReturnType = std::pair<unsign_t<T>, unsign_t<T>>;
    if ( y == 0 || is_signed_division_overflow(x, y))
        return ReturnType{};

    return ReturnType(x / y, x % y);
}

template<typename T>
auto riscv_multiplication_low(T x, T y) {
    using UT = unsign_t<T>;
    return narrow_cast<UT>( x * y & all_ones<UT>());
}

// For RISCV-128bit result of multiplication is 256 bit type,
// which is not defined in ABI.
// So, we have to use Karatsuba algorithm to get high register of 
// multiplication result for unsigned*unsigned.
template<typename T>
auto riscv_multiplication_high_uu(T x, T y) {
    uint8 halfwidth = bitwidth<T>/2;
    using UT = unsign_t<T>;
    auto half_mask = narrow_cast<UT>( all_ones<UT>() >> halfwidth);
    auto x1 = narrow_cast<UT>( x >> halfwidth);
    auto x0 = narrow_cast<UT>( x & half_mask);
    auto y1 = narrow_cast<UT>( y >> halfwidth);
    auto y0 = narrow_cast<UT>( y & half_mask);
    auto low_part = ( ( ((x0 * y0) >> halfwidth) + (( x0 * y1) & half_mask) + (( x1 * y0) & half_mask) ) >> halfwidth);
    auto high_part = ( ( x1*y1) + (( x0*y1) >> halfwidth) + (( x1*y0) >> halfwidth) );
    auto hi = narrow_cast<UT>( high_part + low_part);
    return hi;
}

template<typename T>
auto riscv_multiplication_high_ss(T x, T y) {
    using UT = unsign_t<T>;
    auto x_is_neg = x >> (bitwidth<T> - 1);
    auto y_is_neg = y >> (bitwidth<T> - 1);
    auto result_is_neg = x_is_neg ^ y_is_neg;
    auto x_abs = ( x_is_neg) 
                 ? ~( UT{ x} - 1)
                 : UT{x};
    auto y_abs = ( y_is_neg) 
                 ? ~( UT{ y} - 1)
                 : UT{y};
    auto hi_abs = riscv_multiplication_high_uu( UT{ x_abs}, UT{ y_abs});
    auto lo_abs = riscv_multiplication_low( UT{ x_abs}, UT{ y_abs});
    auto result = UT{ hi_abs}; 
    if( result_is_neg)
        result = ( lo_abs == 0)
                 ? narrow_cast<UT>( ~result + 1)
                 : narrow_cast<UT>( ~result);
    else
        result = narrow_cast<UT>( hi_abs);
        
    return result;
}

template<typename T>
auto riscv_multiplication_high_su(T x, T y) {
    using UT = unsign_t<T>;
    auto x_is_neg = x >> (bitwidth<T> - 1);
    auto x_abs = ( x_is_neg) 
                 ? ~( UT{ x} - 1)
                 : UT{x};
    auto hi_abs = riscv_multiplication_high_uu( UT{ x_abs}, UT{ y});
    auto lo_abs = riscv_multiplication_low( UT{ x_abs}, UT{ y});
    const auto& result = UT{ hi_abs}; 
    if ( x_is_neg)
        return ( lo_abs == 0)
                 ? narrow_cast<UT>( ~result + 1)
                 : narrow_cast<UT>( ~result);

    return narrow_cast<UT>( hi_abs);
}

template<typename T>
auto riscv_division(T x, T y) {
    using UT = unsign_t<T>;
    if ( y == 0)
        return narrow_cast<UT>( all_ones<UT>());

    if ( is_signed_division_overflow(x, y))
        return narrow_cast<UT>( msb_set<UT>());

    return narrow_cast<UT>( x / y);
}

template<typename T>
auto riscv_remainder(T x, T y) {
    using UT = unsign_t<T>;
    if ( y == 0)
        return narrow_cast<UT>( x);

    if ( is_signed_division_overflow(x, y))
        return narrow_cast<UT>( 0);

    return narrow_cast<UT>( x % y);
}

template <typename Instr>
struct RISCVMultALU
{
    template<typename T> static void mult_h_uu( Instr* instr) { instr->v_dst[0] = riscv_multiplication_high_uu<T>(instr->v_src[0], instr->v_src[1]); }
    template<typename T> static void mult_h_ss( Instr* instr) { instr->v_dst[0] = riscv_multiplication_high_ss<T>(instr->v_src[0], instr->v_src[1]); }
    template<typename T> static void mult_h_su( Instr* instr) { instr->v_dst[0] = riscv_multiplication_high_su<T>(instr->v_src[0], instr->v_src[1]); }
    template<typename T> static void mult_l( Instr* instr) { instr->v_dst[0] = riscv_multiplication_low<T>(instr->v_src[0], instr->v_src[1]); }
    template<typename T> static void div( Instr* instr) { instr->v_dst[0] = riscv_division<T>(instr->v_src[0], instr->v_src[1]); }
    template<typename T> static void rem( Instr* instr) { instr->v_dst[0] = riscv_remainder<T>(instr->v_src[0], instr->v_src[1]); }
};

template <typename Instr>
struct MIPSMultALU
{
    template<typename T> static void multiplication( Instr* instr)
    {
        const auto& result = mips_multiplication<T>( instr->v_src[0], instr->v_src[1]);
        instr->v_dst[0]  = narrow_cast<typename Instr::RegisterUInt>( result.first);
        instr->v_dst[1] = narrow_cast<typename Instr::RegisterUInt>( result.second);
    }

    template<typename T> static void division( Instr* instr)
    {
        const auto& result = mips_division<T>( instr->v_src[0], instr->v_src[1]);
        instr->v_dst[0]  = narrow_cast<typename Instr::RegisterUInt>( result.first);
        instr->v_dst[1] = narrow_cast<typename Instr::RegisterUInt>( result.second);
    } 
};
