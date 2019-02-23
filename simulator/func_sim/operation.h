/**
 * operation.h - list of basic operation types
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2019 MIPT-MIPS
 */
 
#ifndef OPERATION_H
#define OPERATION_H

#include <infra/types.h>

enum OperationType : uint8
{
    OUT_ARITHM,
    OUT_R_ACCUM,
    OUT_R_CONDM,
    OUT_R_JUMP,
    OUT_SYSCALL,
    OUT_BREAK,
    OUT_R_SUBTR,
    OUT_BRANCH,
    OUT_TRAP,
    OUT_LOAD,
    OUT_LOADU,
    OUT_PARTIAL_LOAD,
    OUT_STORE,
    OUT_J_JUMP,
    OUT_J_SPECIAL,
    OUT_UNKNOWN
};

#endif // OPERATION_H
 
