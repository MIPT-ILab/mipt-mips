/**
 * timing.h - Implementation of safe timing primitive types
 *
 * @author Denis Los
 * Copyright 2018 MIPT-MIPS project
 */

#ifndef INFRA_TIMING_H
#define INFRA_TIMING_H

#include <iostream>
#include <boost/operators.hpp>

#include <infra/types.h>

class Cycle;
class Latency;

class Cycle : public boost::totally_ordered<Cycle>
{
    public:
        constexpr explicit Cycle( uint64 value = NO_VAL64) : value( value) { }

        constexpr auto operator==( const Cycle& rhs) const noexcept { return value == rhs.value; }
        constexpr auto operator<( const Cycle& rhs) const noexcept { return value < rhs.value; }

        constexpr void inc() noexcept { ++value; }
        constexpr explicit operator double() const noexcept { return static_cast<double>( value); }

        constexpr uint64 operator%( uint64 number) const noexcept { return value % number; }

        constexpr Cycle   operator+( const Latency& latency) const noexcept;
        constexpr Cycle   operator-( const Latency& latency) const noexcept;
        constexpr Latency operator-( const Cycle& cycle) const noexcept;

        friend std::ostream& operator<<( std::ostream& os, const Cycle& cycle)
        {
            return os << cycle.value;
        }
        friend std::istream& operator>>( std::istream& is, Cycle& cycle)
        {
            return is >> cycle.value;
        }

    private:
        uint64 value;
};

// NOLINTNEXTLINE(google-runtime-int) https://bugs.llvm.org/show_bug.cgi?id=24840
constexpr inline auto operator""_Cl( unsigned long long int number) noexcept
{
    return Cycle( static_cast<uint64>( number));
}

class Latency : public boost::totally_ordered<Latency>
{
    public:
        constexpr explicit Latency( int64 value = 0) noexcept : value( value) { }

        constexpr auto operator==( const Latency& rhs) const { return value == rhs.value; }
        constexpr auto operator<( const Latency& rhs) const { return value < rhs.value; }
        constexpr auto operator+( const Latency& rhs) const { return Latency( value + rhs.value); }
        constexpr auto operator-( const Latency& rhs) const { return Latency( value - rhs.value); }
        constexpr auto operator/( int64 number) const { return Latency( value / number); }
        constexpr auto operator*( int64 number) const { return Latency( value * number); }

        friend std::ostream& operator<<( std::ostream& os, const Latency& latency)
        {
            return os << latency.value;
        }
        friend std::istream& operator>>( std::istream& is, Latency& latency)
        {
            return is >> latency.value;
        }

        friend constexpr inline Cycle Cycle::operator+( const Latency& latency) const noexcept;
        friend constexpr inline Cycle Cycle::operator-( const Latency& latency) const noexcept;

    private:
        int64 value;
};

// NOLINTNEXTLINE(google-runtime-int) https://bugs.llvm.org/show_bug.cgi?id=24840
constexpr inline auto operator""_Lt( unsigned long long int number) noexcept
{
    return Latency( static_cast<int64>( number));
}

constexpr inline auto operator*( int64 number, const Latency& latency) noexcept { return latency * number; }

constexpr Cycle   Cycle::operator+( const Latency& latency) const noexcept { return Cycle( value + latency.value); }
constexpr Cycle   Cycle::operator-( const Latency& latency) const noexcept { return Cycle( value - latency.value); }
constexpr Latency Cycle::operator-( const Cycle& cycle) const noexcept { return Latency( value - cycle.value); }

#endif // INFRA_TIMING_H

