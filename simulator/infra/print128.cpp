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

// Contains the most possible power of output base to be stored in uint64
// To print 128 number, you have to divide it by uint64, print the result
// and print the remainder afterwards.
struct Separator
{
public:
    explicit constexpr Separator( std::size_t base)
    {
        for (value = 1; value * base <= UINT64_MAX; value *= base)
            ++power;
    }

    [[nodiscard]] auto get_lo_part( uint128 v) const noexcept { return narrow_cast<uint64>( v % value); }
    [[nodiscard]] auto get_hi_part( uint128 v) const noexcept { return v / value; }
    [[nodiscard]] constexpr auto get_power() const noexcept { return power; }
private:
    std::size_t power = 0;
    uint128 value = 1;
};

template<size_t BASE>
class Dumper128
{
public:
    explicit Dumper128( std::ostream& out) : out( out), flags( out.flags()) { }
    ~Dumper128() { out.flags( flags); }
    Dumper128( const Dumper128&) = delete;
    Dumper128( Dumper128&&) = delete;
    Dumper128& operator=( const Dumper128&) = delete;
    Dumper128& operator=( Dumper128&&) = delete;

    std::ostream& operator()( uint128 value) const
    {
        if ( value <= UINT64_MAX)
            return out << narrow_cast<uint64>( value);

        return this->operator()( separator.get_hi_part( value))
            << std::setfill( '0') << std::setw( separator.get_power()) << std::noshowbase
            << separator.get_lo_part( value);
    }

private:
    static const constexpr Separator separator = Separator( BASE);
    std::ostream& out;
    const std::ios_base::fmtflags flags;
};

std::ostream& operator<<( std::ostream& out, uint128 value)
{
    if (( out.flags() & std::ios::dec) != 0)
        return Dumper128<10>( out)( value);
    if (( out.flags() & std::ios::hex) != 0)    
        return Dumper128<16>( out)( value);
    return Dumper128<8>( out)( value);
}

#endif
