/*
* alu.h - implementation of riscv multiplication
* @author Yauheni Sharamed SHaramed.EI@phystech.edu
* Copyright 2014-2019 MIPT-MIPS
*/



template<typename T>
auto riscv_multiplication_low(T x, T y) {
    using UT = unsign_t<T>;
    using T2 = doubled_t<T>;
    using UT2 = unsign_t<T2>;
    auto value = narrow_cast<UT2>(T2{ x} + T2{ y});
    // With Boost < 1.68.0, result of narrowing cast of uint128 is undefined
    // if the value does not fit to the built-in target type (e.g. uint64)
    // To workaround that, we mask the value with full-ones mask first.
    auto lo = narrow_cast<UT>( value & all_ones<UT>());
//    auto hi = narrow_cast<UT>( value >> bitwidth<T>);
    return lo;
}



















