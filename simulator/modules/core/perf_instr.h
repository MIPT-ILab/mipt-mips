/*
 * perf_instr.h - instruction class for performance simulation
 * Copyright 2018 MIPT-MIPS
 */

#ifndef PERF_INSTR_H
#define PERF_INSTR_H

#include <infra/types.h>
#include <modules/fetch/bpu/bp_interface.h>

#include <utility>

struct CheckerMismatch;

template <typename FuncInstr>
class PerfInstr : public FuncInstr
{
    /* info for branch misprediction unit */
    const BPInterface bp_data = {};
public:
    PerfInstr( const FuncInstr& instr, const BPInterface& bp_info) : FuncInstr( instr), bp_data( bp_info) { }

    auto get_dst_v() const { return std::make_pair( this->get_v_dst(), this->get_v_dst2()); }
    
    bool is_misprediction() const { return bp_data.is_taken != this->is_jump_taken() || bp_data.target != this->get_new_PC(); }

    // Get targets for the next instruction, predicted and actual
    Target get_predicted_target() const {
        return Target( bp_data.target, this->get_sequence_id() + 1);
    }

    Target get_actual_target() const {
        return Target( this->get_new_PC(), this->get_sequence_id() + 1);
    }
    
    BPInterface get_bp_upd() const { return BPInterface( this->get_PC(), this->is_jump_taken(), this->get_new_PC()); }

    bool is_bypassible() const { return !this->is_conditional_move() &&
                                        !this->is_partial_load()     &&
                                        this->get_accumulation_type() == 0; }

    auto is_long_arithmetic() const { return this->is_divmult(); }

    auto is_mem_stage_required() const { return this->is_load()  ||
                                                this->is_store() ||
                                                this->is_jump()  ||
                                                this->is_explicit_trap() ||
                                                this->is_special(); }
};

#endif // PERF_INSTR_H
