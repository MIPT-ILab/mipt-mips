/*
* multiplication.h - implementation of riscv multiplication
* @author Yauheni Sharamed SHaramed.EI@phystech.edu
* Copyright 2014-2019 MIPT-MIPS
*/



template<typename T>
auto riscv_multiplication_low(T x, T y) {
    using UT = unsign_t<T>;
    auto lo = narrow_cast<UT>( x * y & all_ones<UT>());
    return lo;
}





















