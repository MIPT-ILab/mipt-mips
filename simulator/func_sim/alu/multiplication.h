/*
* multiplication.h - implementation of riscv multiplication
* @author Yauheni Sharamed SHaramed.EI@phystech.edu
* Copyright 2014-2019 MIPT-MIPS
*/

#include <func_sim/operation.h>
#include <infra/macro.h>
#include <infra/types.h>

template<Integer T1, Integer T2>
inline auto multiplication(T1 x, T2 y) requires (bitwidth<T1> == bitwidth<T2>) {
    using UT  = unsigned_integer_t<bitwidth<T1>>;
    using UTD = doubled_t<UT>;
    using TD  = sign_t<UTD>;
    auto value = narrow_cast<UTD>(TD{ x} * TD{ y});

    return std::pair{ narrow_cast<UT>( value), narrow_cast<UT>( value >> bitwidth<T1>)};
}

constexpr bool is_signed_division_overflow(Unsigned auto x, decltype(x) /*unused*/)
{
    return false;
}

constexpr bool is_signed_division_overflow(Signed auto x, decltype(x) y)
{
    return y == -1 && x == signify( msb_set<unsign_t<decltype(x)>>());
}

template<Integer T>
auto mips_division(T x, T y) {
    using Pair = std::pair<unsign_t<T>, unsign_t<T>>;
    if ( y == 0 || is_signed_division_overflow(x, y))
        return Pair{};

    return Pair( x / y, x % y);
}

template<Integer T>
auto riscv_division(T x, T y) {
    using UT   = unsigned_integer_t<bitwidth<T>>;
    using Pair = std::pair<UT, UT>;
    if ( y == 0)
        return Pair{ all_ones<UT>(), narrow_cast<UT>( x)};

    if ( is_signed_division_overflow( x, y))
        return Pair{ msb_set<UT>(), UT{}};

    return Pair( x / y, x % y);
}

struct RISCVMultALU
{
    static void mult_h_uu( Executable auto* instr) { instr->v_dst[0] = multiplication( instr->v_src[0], instr->v_src[1]).second; }
    static void mult_h_ss( Executable auto* instr) { instr->v_dst[0] = multiplication( signify( instr->v_src[0]), signify( instr->v_src[1])).second; }
    static void mult_h_su( Executable auto* instr) { instr->v_dst[0] = multiplication( signify( instr->v_src[0]), instr->v_src[1]).second; }
    static void mult_l( Executable auto* instr) { instr->v_dst[0] = instr->v_src[0] * instr->v_src[1]; }

    template<typename T> static void div( Executable auto* instr) { instr->v_dst[0] = riscv_division<T>(instr->v_src[0], instr->v_src[1]).first; }
    template<typename T> static void rem( Executable auto* instr) { instr->v_dst[0] = riscv_division<T>(instr->v_src[0], instr->v_src[1]).second; }
};

struct MIPSMultALU
{
    template<typename T> static void multiplication( Executable auto* instr)
    {
        using UInt = std::decay_t<decltype(instr->v_src[0])>;
        const auto& result = ::multiplication<T, T>( instr->v_src[0], instr->v_src[1]);
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
