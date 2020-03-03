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
    [[nodiscard]] auto get_bp_upd_address() const {
        return this->is_indirect_jump() ? this->get_new_PC() : this->get_decoded_target();
    }

public:
    PerfInstr( const FuncInstr& instr, const BPInterface& bp_info) : FuncInstr( instr), bp_data( bp_info) { }

    [[nodiscard]] auto get_dst_v() const { return std::pair{ this->get_v_dst(), this->get_v_dst2()}; }
    
    [[nodiscard]] const auto& get_bp_data() const { return bp_data; }

    // Get targets for the next instruction, predicted and actual
    [[nodiscard]] Target get_predicted_target() const {
        return Target( bp_data.target, this->get_sequence_id() + 1);
    }

    [[nodiscard]] Target get_actual_target() const {
        return Target( this->get_new_PC(), this->get_sequence_id() + 1);
    }

    [[nodiscard]] Target get_actual_decoded_target() const {
        return Target( this->get_decoded_target(), this->get_sequence_id() + 1);
    }

    [[nodiscard]] BPInterface get_bp_upd() const {
        return BPInterface( this->get_PC(), this->is_taken(), this->get_bp_upd_address(), true);
    }

    [[nodiscard]] bool is_bypassible() const { return !this->is_conditional_move() &&
                                        !this->is_partial_load()     &&
                                        this->get_accumulation_type() == 0; }

    [[nodiscard]] auto is_long_arithmetic() const { return this->is_divmult(); }

    [[nodiscard]] auto is_mem_stage_required() const { return this->is_load()  ||
                                                this->is_store() ||
                                                this->is_explicit_trap(); }

    [[nodiscard]] auto is_branch_stage_required() const { return this->is_jump(); }
};

#endif // PERF_INSTR_H
