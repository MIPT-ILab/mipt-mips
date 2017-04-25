/**
 * macro.h - Implementation of useful inline functions
 *
 * @author Pavel Kryukov <pavel.kryukov@phystech.edu>
 * Copyright 2017 MIPT-MIPS
 */

// protection from multi-include
#ifndef COMMON__MACRO_H
#define COMMON__MACRO_H

/* Returns size of a static array */
template<typename T, size_t N>
constexpr size_t countof( const T (&)[N]) noexcept { return N; }

/* Checks if values is power of two */
template<typename T>
constexpr bool is_power_of_two( const T& n) noexcept { return (n & (n - 1)) == 0; }

// It should be replaced with std::string_view
class StringView
{
    const char* value = nullptr;
public:
    StringView( std::nullptr_t) : value( nullptr) { }
    StringView( const char* v) : value( v) { }

    // The pointer is guaranteed to point to static string
    // so it is OK to copy it by value
    StringView& operator=( const StringView& v)
    {
        this->value = v.value;
        return *this;
    }

    operator const char* const &() const { return value; }
};

#endif

