/**
 * types.h - Implementation of types that are independent of host system
 * The types are used in algorithms affected by size of a variable
 *
 * @author Denis Los
 * Copyright 2018 MIPT-MIPS project
 */

#ifndef INFRA_TIMING_H
#define INFRA_TIMING_H

#include <iostream>

class Cycle;
class Latency;

class Cycle
{
    public:
        constexpr explicit Cycle( uint64 value = NO_VAL64) : value( value) { }

        constexpr auto operator< ( const Cycle& cycle) const { return value < cycle.value; }
        constexpr auto operator> ( const Cycle& cycle) const { return value > cycle.value; }
        constexpr auto operator<=( const Cycle& cycle) const { return value <= cycle.value; }
        constexpr auto operator>=( const Cycle& cycle) const { return value >= cycle.value; }
        constexpr auto operator==( const Cycle& cycle) const { return value == cycle.value; }
        constexpr auto operator!=( const Cycle& cycle) const { return value != cycle.value; }

        constexpr void inc() { ++value; }
        constexpr explicit operator double() const { return static_cast<double>( value); }

        constexpr uint64 operator%( uint64 number) const { return value % number; }

        constexpr Cycle   operator+( const Latency& latency) const;
        constexpr Cycle   operator-( const Latency& latency) const;
        constexpr Latency operator-( const Cycle& cycle) const;

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

constexpr inline auto operator""_Cl( unsigned long long int number)
{
    return Cycle( static_cast<uint64>( number));
}

class Latency
{
    public:
        constexpr explicit Latency( uint64 value = NO_VAL64) : value( value) { }

        constexpr auto operator+( const Latency& latency) const { return Latency( value + latency.value); }
        constexpr auto operator-( const Latency& latency) const { return Latency( value - latency.value); }
        constexpr auto operator/( uint64 number) const { return Latency( value / number); }
        constexpr auto operator*( uint64 number) const { return Latency( value * number); }

        friend std::ostream& operator<<( std::ostream& os, const Latency& latency)
        {
            return os << latency.value;
        }
        friend std::istream& operator>>( std::istream& is, Latency& latency)
        {
            return is >> latency.value;
        }

        friend constexpr inline Cycle Cycle::operator+( const Latency& latency) const;
        friend constexpr inline Cycle Cycle::operator-( const Latency& latency) const;
        
    private:
        uint64 value;
};

constexpr inline auto operator""_Lt( unsigned long long int number)
{
    return Latency( static_cast<uint64>( number));
}

constexpr inline auto operator*( uint64 number, const Latency& latency) { return latency * number; }

constexpr Cycle   Cycle::operator+( const Latency& latency) const { return Cycle( value + latency.value); }
constexpr Cycle   Cycle::operator-( const Latency& latency) const { return Cycle( value - latency.value); }
constexpr Latency Cycle::operator-( const Cycle& cycle) const { return Latency( value - cycle.value); }

#endif // INFRA_TIMING_H

