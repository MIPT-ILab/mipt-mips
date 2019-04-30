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
    std::size_t power = 0;
    uint128 value = 1;
    explicit constexpr Separator(std::size_t base)
    {
        for (value = 1; value * base <= UINT64_MAX; value *= base)
            ++power;
    }
};

// RAII wrapper to restore std::ostream flags
class FlagsKeeper
{
public:
    explicit FlagsKeeper( std::ostream& out) : out( out), flags( out.flags()) { }
    ~FlagsKeeper() { out.flags( flags); }
private:
    std::ostream& out;
    const std::ios_base::fmtflags flags;
};

template<size_t BASE>
static std::ostream& dump(std::ostream& out, uint128 value)
{
    static const constexpr auto separator = Separator(BASE);
    if (value <= UINT64_MAX)
        return out << narrow_cast<uint64>( value);

    FlagsKeeper flags_keeper( out);
    dump<BASE>(out, value / separator.value);
    return out << std::setfill('0') << std::setw(separator.power) << std::noshowbase << (value % separator.value);
}

std::ostream& operator<<( std::ostream& out, uint128 value)
{
    if (( out.flags() & std::ios::dec) != 0)
        return dump<10>( out, value);
    if (( out.flags() & std::ios::hex) != 0)    
        return dump<16>( out, value);
    return dump<8>( out, value);
}

#endif
