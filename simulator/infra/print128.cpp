/**
 * types.h - Implementation of types that are independent of host system
 * The types are used in algorithms affected by size of a variable
 *
 * @author Andrey Agrachev, Pavel Kryukov
 * Copyright 2019 MIPT-MIPS project
 */

#include <infra/macro.h>
#include <infra/types.h>

#include <iomanip>

#ifdef USE_GNUC_INT128

static inline std::ostream& decimal_dump_of_uint128(std::ostream& out, uint128 value)
{
    if (value <= UINT64_MAX)
        return out << narrow_cast<uint64>( value);

    static const uint128 separator{ 10'000'000'000'000'000'000u};
    const uint64 trailing = value % separator;
    return decimal_dump_of_uint128(out, value / separator) << trailing;
}

static inline std::ostream& hexadecimal_dump_of_uint128(std::ostream& out, uint128 value)
{
    if ( value <= UINT64_MAX)
        return out << narrow_cast<uint64>( value);

    std::ios_base::fmtflags flags( out.flags() );
    out << narrow_cast<uint64>( value >> 64ULL) << std::setfill( '0') << std::setw( 16) << narrow_cast<uint64>( value);
    out.flags( flags);
    return out;
}

static inline std::ostream& octal_dump_of_uint128(std::ostream& out, uint128 value)
{
    if ( value <= bitmask<uint64>( 63))
        return out << narrow_cast<uint64>( value);

    std::ios_base::fmtflags flags( out.flags() );
    octal_dump_of_uint128( out, value >> 63ULL) << std::setfill( '0') << std::setw( 21) << (value & bitmask<uint64>( 63));
    out.flags( flags);
    return out;
}

std::ostream& operator<<( std::ostream& out, uint128 value)
{
    if (( out.flags() & std::ios::dec) != 0)
        return decimal_dump_of_uint128( out, value);
    if (( out.flags() & std::ios::hex) != 0)    
        return hexadecimal_dump_of_uint128( out, value);
    return octal_dump_of_uint128( out, value);
}

#endif
