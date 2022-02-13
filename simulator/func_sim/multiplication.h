/*
* multiplication.h - implementation of riscv multiplication
* @author Yauheni Sharamed SHaramed.EI@phystech.edu
* Copyright 2014-2019 MIPT-MIPS
*/

#include <infra/macro.h>
#include <infra/types.h>

constexpr bool is_signed_division_overflow(Unsigned auto x, decltype(x))
{
    return false;
}

constexpr bool is_signed_division_overflow(Signed auto x, decltype(x) y)
{
    return y == -1 && x == sign_cast<decltype(x)>(msb_set<unsign_t<decltype(x)>>());
}

inline auto mips_multiplication(Unsigned auto x, decltype(x) y) {
    using T  = decltype(x);
    using T2 = doubled_t<T>;
    T2 value = T2{ x} * T2{ y};

    auto lo = narrow_cast<T>( value);
    auto hi = narrow_cast<T>( value >> bitwidth<T>);
    return std::pair{ lo, hi};
}

inline auto mips_multiplication(Signed auto x, decltype(x) y) {
    using T   = decltype(x);
    using UT  = unsign_t<T>;
    using UT2 = doubled_t<UT>;
    using T2  = sign_t<UT2>;
    auto value = narrow_cast<UT2>(T2{ x} * T2{ y});

    auto lo = narrow_cast<UT>( value);
    auto hi = narrow_cast<UT>( value >> bitwidth<T>);
    return std::pair{ lo, hi};
}

template<Integer T>
auto mips_division(T x, T y) {
    using ReturnType = std::pair<unsign_t<T>, unsign_t<T>>;
    if ( y == 0 || is_signed_division_overflow(x, y))
        return ReturnType{};

    return ReturnType(x / y, x % y);
}

template<Integer T>
auto riscv_multiplication_low(T x, T y) {
    using UT = unsign_t<T>;
    return narrow_cast<UT>( x * y);
}

// For RISCV-128bit result of multiplication is 256 bit type,
// which is not defined in ABI.
// So, we have to use Karatsuba algorithm to get high register of 
// multiplication result for unsigned*unsigned.
template<Integer T>
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

template<Integer T>
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

template<Integer T>
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

template<Integer T>
auto riscv_division(T x, T y) {
    using UT = unsign_t<T>;
    if ( y == 0)
        return all_ones<UT>();

    if ( is_signed_division_overflow( x, y))
        return msb_set<UT>();

    return narrow_cast<UT>( x / y);
}

template<Integer T>
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
