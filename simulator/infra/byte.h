/**
 * byte.h - Implementation of Byte type
 *
 * @author Pavel Kryukov <pavel.kryukov@phystech.edu>
 * Copyright 2017-2019 MIPT-MIPS
 */

#ifndef INFRA_BYTE_H
#define INFRA_BYTE_H
 
#include "types.h"

#include <cstddef>
 
// Do not use std::byte as Apple Xcode does not have it
enum class Byte : uint8 { };

static inline Byte* byte_cast( char* b)
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast) Casting byte to byte is correct
    return reinterpret_cast<Byte*>( b);
}

static inline const Byte* byte_cast( const char* b)
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast) Casting byte to byte is correct
    return reinterpret_cast<const Byte*>( b);
}

static inline Byte* byte_cast( uint8* b)
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast) Casting byte to byte is correct
    return reinterpret_cast<Byte*>( b);
}

static inline const Byte* byte_cast( const uint8* b)
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast) Casting byte to byte is correct
    return reinterpret_cast<const Byte*>( b);
}

#endif // INFRA_BYTE_H
