/* trap_types.h - Trap types for MIPS and RISC-V
* @author Pavel Kryukov pavel.kryukov@phystech.edu, Vyacheslav Kompan
* Copyright 2014-2018 MIPT-MIPS
*/

#ifndef TRAP_TYPES_H
#define TRAP_TYPES_H

// MIPT-MIPS modules
#include <infra/types.h>

enum class Trap : uint8
{
    NO_TRAP,
    HALT,
    EXPLICIT_TRAP,
    BREAKPOINT,
    SYSCALL,
    INTEGER_OVERFLOW,
    UNSUPPORTED_SYSCALL,
    UNALIGNED_ADDRESS,
    UNKNOWN_INSTRUCTION
};

#endif //TRAP_TYPES_H
