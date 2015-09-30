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

/* Signed 8-bit integer type */
typedef signed char int8;
/* Signed 16-bit integer type */
typedef signed short int16;
/* Signed 32-bit integer type */
typedef signed int int32;
/* Signed 64-bit integer type */
typedef signed long long int64;
/* Unsigned 8-bit integer type */
typedef unsigned char uint8;
/* Unsigned 16-bit integer type */
typedef unsigned short uint16;
/* Unsigned 32-bit integer type */
typedef unsigned int uint32;
/* Unsigned 64-bit integer type */
typedef unsigned long long uint64;
/* Single precision 32-bit float type */
typedef float float32;
/* Double precision 64-bit float type */
typedef double float64;

// The following consts are used to initialize some variables
// as something that has invalid value.
// Using such values most likely will leads to segmenation fault,
// which is easy to debug than access by an uninitialize pointer
// or array index.
static const uint8  NO_VAL8  = (uint8)  -1;
static const uint16 NO_VAL16 = (uint16) -1;
static const uint32 NO_VAL32 = (uint32) -1;
static const uint64 NO_VAL64 = (uint64) -1;

static const uint8  MAX_VAL8  = (uint8)  -1;
static const uint16 MAX_VAL16 = (uint16) -1;
static const uint32 MAX_VAL32 = (uint32) -1;
static const uint64 MAX_VAL64 = (uint64) -1;

#endif // #ifndef COMMON__TYPES_H 
