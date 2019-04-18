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

#if defined (__SIZEOF_INT128__)
// For RISCV-128bit result of multiplication is 256 bit type,
// which is not defined in ABI.
// So, we have to use Karatsuba algorithm to get high register of 
// multiplication result.

template<typename T>
auto riscv_multiplication_high_unsigned(T x, T y) {
    uint8 halfwidth = bitwidth<T>/2;
    using UT = unsign_t<T>;
    //using ST = sign_t<T>;
    auto half_mask = narrow_cast<UT>( all_ones<UT>() >> halfwidth);
    auto x1 = narrow_cast<UT>( x >> halfwidth);
    auto x0 = narrow_cast<UT>( x & half_mask);
    auto y1 = narrow_cast<UT>( y >> halfwidth);
    auto y0 = narrow_cast<UT>( y & half_mask);
    auto low_part = ( ( ((x0 * y0) >> halfwidth) + (( x0 * y1) & half_mask) + (( x1 * y0) & half_mask) ) >> halfwidth);
    auto high_part = ( ( x1*y1) + ( ( x0*y1) >> halfwidth) + (( x1*y0) >> halfwidth) );
    auto hi = narrow_cast<UT>( high_part + low_part);
return hi;
}



#else 
// __SIZEOF_INT128__
// using boost types
printf("her\n");
template<typename T>
auto riscv_multiplication_high_unsigned(T x, T y) {
    using UT = unsign_t<T>;
    using T2 = doubled_t<T>;
    using UT2 = unsign_t<T2>;
    auto value = narrow_cast<UT2>(T2{ x} * T2{ y});
    auto hi = narrow_cast<UT>( value >> bitwidth<T>);
    return hi;
}
#endif





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



















