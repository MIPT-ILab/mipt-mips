/**
 * macro.h - Implementation of useful inline functions
 *
 * @author Pavel Kryukov <pavel.kryukov@phystech.edu>
 * Copyright 2017-2019 MIPT-MIPS
 */

#ifndef COMMON_MACRO_H
#define COMMON_MACRO_H

#include <infra/types.h>

#include <array>
#include <bit>
#include <climits>
#include <limits>
#include <type_traits>

#ifdef _MSC_FULL_VER
static_assert(_MSC_FULL_VER >= 192328105, "Update Visual Studio because of the bug: "
              "https://developercommunity.visualstudio.com/content/problem/833637/wrong-compilation-for-ones-right-shift.html");
#endif

#ifdef  _LIBCPP_VERSION
template<typename T> static constexpr inline auto is_power_of_two( T n) noexcept       { return std::ispow2( n); }
#else
template<typename T> static constexpr inline auto is_power_of_two( T n) noexcept       { return std::has_single_bit( n); }
#endif

template<typename T> void ignored( const T& /* unused */) noexcept { }

/* Bit widths of integer types */
template<typename T> static constexpr size_t bitwidth = std::numeric_limits<T>::digits + std::numeric_limits<T>::is_signed;
template<typename T> static constexpr size_t bytewidth = bitwidth<T> / CHAR_BIT;
template<typename T> static constexpr size_t half_bitwidth = bitwidth<T> >> 1;
template<typename T> static constexpr size_t log_bitwidth = std::countr_zero(bitwidth<T>);

/* Bit masks */
template<typename T> static constexpr inline auto lsb_set()  noexcept { return T{ 1U}; }
template<typename T> static constexpr inline auto msb_set()  noexcept { return (lsb_set<T>() << (bitwidth<T> - 1)); }
template<typename T> static constexpr inline auto all_ones() noexcept { return (msb_set<T>() - 1U) | msb_set<T>(); }

template<typename T> static constexpr T ones_ls( const T& value, size_t shamt) { return ~( ~value << shamt); }
template<typename T> static constexpr T ones_rs( const T& value, size_t shamt) { return ~( ~value >> shamt); }

/* Returns a bitmask with desired amount of LSB set to '1'
 * Examples: bitmask<uint32>(0)  -> 0x0
 *           bitmask<uint32>(5)  -> 0x1F
 *           bitmask<uint32>(32) -> 0xFFFF'FFFF
 */

template<typename T> static constexpr T bitmask( size_t onecount) noexcept
{
    return onecount == bitwidth<T> ? all_ones<T>() : ones_ls( T{}, onecount);
}

template<typename T>
T sign_extension( T value, size_t bits)
{
    if ( bits < bitwidth<T>) {
        const T msb = bits == 0 ? 0 : T{ 1} << ( bits - 1);
        value = ( ( value & bitmask<T>(bits)) ^ msb) - msb;
    }
    return value;
}

template<size_t N, typename T>
T sign_extension( T value)
{
    return sign_extension(value, N);
}

template<typename To, typename From>
static constexpr inline auto unpack_to( From src) noexcept
{
    std::array<To, bytewidth<From> / bytewidth<To>> result{};
    const constexpr size_t offset = bitwidth<To>;
    size_t shift = 0;
    for ( auto& v : result) {
        v = narrow_cast<To>( ( src >> shift) & bitmask<From>( offset));
        shift += offset;
    }

    return result;
}

template<typename From, size_t N>
static constexpr inline auto pack_from( std::array<From, N> src) noexcept
{
    using To = packed_t<N, From>;
    To result{};
    size_t shift = 0;
    for ( const auto& value : src) {
        result |= To{ value} << shift;
        shift += bitwidth<From>;
    }
    return result;
}        

template<typename T>
static inline T trap_vector_address( T value)
{
    return value >> 2U & ~(bitmask<T>( 3));
}

/*
 * Templated no-value (non-trivial data of given size)
 */
template<typename T> static constexpr T NO_VAL = all_ones<T>();
template<> inline constexpr uint8  NO_VAL<uint8>  = NO_VAL8;
template<> inline constexpr uint16 NO_VAL<uint16> = NO_VAL16;
template<> inline constexpr uint32 NO_VAL<uint32> = NO_VAL32;
template<> inline constexpr uint64 NO_VAL<uint64> = NO_VAL64;

#endif
