/**
 * argv.h - argument list helper functions
 * @author Pavel Kryukov
 * Copyright 2019 MIPT-MIPS
 */
 
#ifndef INFRA_ARGV_H
#define INFRA_ARGV_H

#include <cstddef>

static inline constexpr int count_argc( const char *const *argv)
{
    int argc = 0;
    while (argv[argc] != nullptr) // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        argc++;
    return argc;
}

// 'const char* const*' shall be the only allowed signature for argv
// For other cases, use `argv_cast'

template<std::size_t N> // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
constexpr const char* const* argv_cast( const char* (& array)[N])
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay, hicpp-no-array-decay)
    return array;
}

template<std::size_t N> // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
constexpr const char* const* argv_cast( const char* const (& array)[N])
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay, hicpp-no-array-decay)
    return array;
}

static inline constexpr const char* const* argv_cast( char** argv)
{
    return static_cast<const char* const*>( argv);
}

static inline constexpr const char* const* argv_cast( char* const* argv)
{
    return static_cast<const char* const*>( argv);
}

static inline constexpr const char* const* argv_cast( const char** argv)
{
    return static_cast<const char* const*>( argv);
}

#endif
