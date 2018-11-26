/*
 * instr.h - instruction data structure
 * @author Pavel Kryukov
 * Copyright 2018 MIPT-MIPS
 */

#ifndef INSTR_H
#define INSTR_H
 
#include "trap_types.h"

#include <infra/types.h>
 
enum OperationType : uint8
{
    OUT_ARITHM,
    OUT_R_ACCUM,
    OUT_R_CONDM,
    OUT_R_JUMP,
    OUT_R_SPECIAL,
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

template<typename Register, typename R>
struct Instr
{
    using RegisterUInt = R;
    using RegisterSInt = sign_t<RegisterUInt>;
    
    MIPSRegister src1 = MIPSRegister::zero;
    MIPSRegister src2 = MIPSRegister::zero;
    MIPSRegister dst  = MIPSRegister::zero;
    MIPSRegister dst2 = MIPSRegister::zero;

    RegisterUInt v_src1 = NO_VAL<RegisterUInt>;
    RegisterUInt v_src2 = NO_VAL<RegisterUInt>;
    RegisterUInt v_dst  = NO_VAL<RegisterUInt>;
    RegisterUInt v_dst2 = NO_VAL<RegisterUInt>;
    RegisterUInt mask   = all_ones<RegisterUInt>();

    OperationType operation = OUT_UNKNOWN;
    Trap trap = Trap::NO_TRAP;
    
    Addr mem_addr = NO_VAL32;
    uint32 mem_size = NO_VAL32;

    uint32 v_imm = NO_VAL32;
    const Addr PC = NO_VAL32;
    Addr new_PC = NO_VAL32;

    bool complete   = false;
    bool _is_jump_taken = false; // actual result
    bool memory_complete = false;

    uint64 sequence_id = NO_VAL64;

    explicit Instr( Addr pc) : PC( pc) { }

    auto set_v_src( RegisterUInt value, size_t index)
    {
        if ( index == 0)
            v_src1 = value;
        else
            v_src2 = value;
    }

    bool is_jump() const
    {
        return operation == OUT_J_JUMP
            || operation == OUT_R_JUMP
            || operation == OUT_BRANCH;
    }

    bool is_partial_load() const
    {
        return operation == OUT_PARTIAL_LOAD;
    }

    bool is_load() const
    { 
        return operation == OUT_LOAD
            || operation == OUT_LOADU
            || is_partial_load();
    }

    int8 get_accumulation_type() const
    {
        return (operation == OUT_R_ACCUM) ? 1 : (operation == OUT_R_SUBTR) ? -1 : 0;
    }
};

#endif
