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

template<Integer T>
inline auto mips_multiplication(T x, T y) {
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

// For RISCV-128bit result of multiplication is 256 bit type,
// which is not defined in ABI.
// So, we have to use Karatsuba algorithm to get high register of 
// multiplication result for unsigned*unsigned.
auto riscv_multiplication_high_uu(Unsigned auto x, decltype(x) y) {
    using T = decltype(x);
    uint8 halfwidth = bitwidth<T>/2;
    auto half_mask = narrow_cast<T>( all_ones<T>() >> halfwidth);
    auto x1 = narrow_cast<T>( x >> halfwidth);
    auto x0 = narrow_cast<T>( x & half_mask);
    auto y1 = narrow_cast<T>( y >> halfwidth);
    auto y0 = narrow_cast<T>( y & half_mask);
    auto low_part = ( ( ((x0 * y0) >> halfwidth) + (( x0 * y1) & half_mask) + (( x1 * y0) & half_mask) ) >> halfwidth);
    auto high_part = ( ( x1*y1) + (( x0*y1) >> halfwidth) + (( x1*y0) >> halfwidth) );
    return high_part + low_part;
}

auto riscv_multiplication_high_ss(Unsigned auto x, decltype(x) y) {
    bool x_is_neg = signify(x) < 0;
    bool y_is_neg = signify(y) < 0;
    auto result_is_neg = x_is_neg ^ y_is_neg;
    auto x_abs = ( x_is_neg) 
                 ? ~(x - 1)
                 : x;
    auto y_abs = ( y_is_neg) 
                 ? ~(y - 1)
                 : y;
    auto hi_abs = riscv_multiplication_high_uu( x_abs, y_abs);
    auto lo_abs = x_abs * y_abs;
    if ( result_is_neg)
        return lo_abs == 0 ? ~hi_abs + 1 : ~hi_abs;

    return hi_abs;
}

auto riscv_multiplication_high_su(Unsigned auto x, decltype(x) y) {
    bool x_is_neg = signify(x) < 0;
    auto x_abs = ( x_is_neg) 
                 ? ~( x - 1)
                 : x;
    auto hi_abs = riscv_multiplication_high_uu( x_abs, y);
    auto lo_abs = x_abs * y; 
    if ( x_is_neg)
        return lo_abs == 0 ? ~hi_abs + 1 : ~hi_abs;

    return hi_abs;
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

struct RISCVMultALU
{
    static void mult_h_uu( Executable auto* instr) { instr->v_dst[0] = riscv_multiplication_high_uu(instr->v_src[0], instr->v_src[1]); }
    static void mult_h_ss( Executable auto* instr) { instr->v_dst[0] = riscv_multiplication_high_ss(instr->v_src[0], instr->v_src[1]); }
    static void mult_h_su( Executable auto* instr) { instr->v_dst[0] = riscv_multiplication_high_su(instr->v_src[0], instr->v_src[1]); }
    static void mult_l( Executable auto* instr) { instr->v_dst[0] = instr->v_src[0] * instr->v_src[1]; }
    template<typename T> static void div( Executable auto* instr) { instr->v_dst[0] = riscv_division<T>(instr->v_src[0], instr->v_src[1]); }
    template<typename T> static void rem( Executable auto* instr) { instr->v_dst[0] = riscv_remainder<T>(instr->v_src[0], instr->v_src[1]); }
};

struct MIPSMultALU
{
    template<typename T> static void multiplication( Executable auto* instr)
    {
        using UInt = std::decay_t<decltype(instr->v_src[0])>;
        const auto& result = mips_multiplication<T>( instr->v_src[0], instr->v_src[1]);
        instr->v_dst[0] = narrow_cast<UInt>( result.first);
        instr->v_dst[1] = narrow_cast<UInt>( result.second);
    }

    template<typename T> static void division( Executable auto* instr)
    {
        using UInt = std::decay_t<decltype(instr->v_src[0])>;
        const auto& result = mips_division<T>( instr->v_src[0], instr->v_src[1]);
        instr->v_dst[0] = narrow_cast<UInt>( result.first);
        instr->v_dst[1] = narrow_cast<UInt>( result.second);
    } 
};
