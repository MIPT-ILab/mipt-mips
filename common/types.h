/**
 * types.h - Implementation of types that are independent of host system
 * The types are used in algorithms affected by size of a variable
 *
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// protection from multi-include
#ifndef COMMON__TYPES_H
#define COMMON__TYPES_H

// C++11 fixed width integer types
#include <cstdint>

/* Signed 8-bit integer type */
using int8 = int8_t;
/* Signed 16-bit integer type */
using int16 = int16_t;
/* Signed 32-bit integer type */
using int32 = int32_t;
/* Signed 64-bit integer type */
using int64 = int64_t;
/* Unsigned 8-bit integer type */
using uint8 = uint8_t;
/* Unsigned 16-bit integer type */
using uint16 = uint16_t;
/* Unsigned 32-bit integer type */
using uint32 = uint32_t;
/* Unsigned 64-bit integer type */
using uint64 = uint64_t;
/* Single precision 32-bit float type */
using float32 = float;
/* Double precision 64-bit float type */
using float64 = double;

// The following consts are used to initialize some variables
// as something that has invalid value.
// Using such values most likely will leads to segmenation fault,
// which is easy to debug than access by an uninitialize pointer
// or array index.
static const uint8  NO_VAL8  = 0xBA;
static const uint16 NO_VAL16 = 0xf00d;
static const uint32 NO_VAL32 = 0xdeadbeeful;
static const uint64 NO_VAL64 = 0xfeedfacecafebeafull;

static const uint8  MAX_VAL8  = UINT8_MAX;
static const uint16 MAX_VAL16 = UINT16_MAX;
static const uint32 MAX_VAL32 = UINT32_MAX;
static const uint64 MAX_VAL64 = UINT64_MAX;

// semantics
using Addr = uint32;
using Cycles = uint64;

#endif // #ifndef COMMON__TYPES_H
