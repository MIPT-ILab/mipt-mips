/* trap_types.h - Trap types for MIPS and RISC-V
* @author Pavel Kryukov pavel.kryukov@phystech.edu, Vyacheslav Kompan
* Copyright 2014-2018 MIPT-MIPS
*/

#ifndef TRAP_TYPES_H
#define TRAP_TYPES_H

// MIPT-MIPS modules
#include <infra/types.h>

enum class TrapType : uint8
{
    NO_TRAP,
    EXPLICIT_TRAP,
    BREAKPOINT,
    SYSCALL,
    UNALIGNED_ADDRESS,
};

#endif //TRAP_TYPES_H
