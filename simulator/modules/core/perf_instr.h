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

    // Returns address used to train branch target prediction
    auto get_bp_upd_address() const {
        return this->is_indirect_jump() ? this->get_new_PC() : this->get_decoded_target();
    }

public:
    PerfInstr( const FuncInstr& instr, const BPInterface& bp_info) : FuncInstr( instr), bp_data( bp_info) { }
    
    const auto& get_bp_data() const { return bp_data; }

    // Get targets for the next instruction, predicted and actual
    Target get_predicted_target() const {
        return Target( bp_data.target, this->get_sequence_id() + 1);
    }

    Target get_actual_target() const {
        return Target( this->get_new_PC(), this->get_sequence_id() + 1);
    }

    Target get_actual_decoded_target() const {
        return Target( this->get_decoded_target(), this->get_sequence_id() + 1);
    }

    BPInterface get_bp_upd() const {
        return BPInterface( this->get_PC(), this->is_taken(), this->get_bp_upd_address(), true);
    }

    bool is_bypassible() const { return !this->is_conditional_move() &&
                                        !this->is_partial_load()     &&
                                        this->get_accumulation_type() == 0; }

    auto is_long_arithmetic() const { return this->is_divmult(); }

    auto is_mem_stage_required() const { return this->is_load()  ||
                                                this->is_store() ||
                                                this->is_explicit_trap(); }

    auto is_branch_stage_required() const { return this->is_jump(); }
};

#endif // PERF_INSTR_H
