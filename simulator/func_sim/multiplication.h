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


template<typename T>
//auto riscv128_multiplication_high(T x, T y) {
auto riscv_multiplication_high(T x, T y) {
#if defined (__SIZEOF_INT128__)
// For RISCV-128bit result of multiplication is 256 bit type,
// which is not defined in ABI.
// So, we have to use some algorithm to get high register of 
// multiplication result.

return x*y;

#else 
// __SIZEOF_INT128__
// using boost types
    using UT = unsign_t<T>;
    using T2 = doubled_t<T>;
    using UT2 = unsign_t<T2>;
    auto value = narrow_cast<UT2>(T2{ x} * T2{ y});
    auto hi = narrow_cast<UT>( value >> bitwidth<T>);
    return hi;
#endif
}
/*
template<typename T>
auto riscv_multiplication_high(T x, T y) {
    using UT = unsign_t<T>;
    using T2 = doubled_t<T>;
    using UT2 = unsign_t<T2>;
    auto value = narrow_cast<UT2>(T2{ x} * T2{ y});
    auto hi = narrow_cast<UT>( value >> bitwidth<T>);
    return hi;

}*/



















