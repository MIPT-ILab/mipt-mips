/*
 * perf_instr.h - instruction class for performance simulation
 * Copyright 2018 MIPT-MIPS
 */

#ifndef PERF_INSTR_H
#define PERF_INSTR_H

#include <infra/types.h>
#include <bpu/bp_interface.h>

template <typename FuncInstr>
class PerfInstr : public FuncInstr
{
    /* info for branch misprediction unit */
    const BPInterface bp_data = {};
public:
    PerfInstr( const FuncInstr& instr, const BPInterface& bp_info) : FuncInstr( instr), bp_data( bp_info) { }

    bool is_misprediction() const { return bp_data.is_taken != this->is_jump_taken() || bp_data.target != this->get_new_PC(); }
    auto get_predicted_target() const { return bp_data.target; }
    BPInterface get_bp_upd() const { return BPInterface( this->get_PC(), this->is_jump_taken(), this->get_new_PC()); }
};

#endif
