/*
 * perf_instr.h - instruction class for performance simulation
 * Copyright 2018 MIPT-MIPS
 */

#ifndef PERF_INSTR_H
#define PERF_INSTR_H

#include <infra/types.h>
#include <modules/fetch/bpu/bp_interface.h>
#include <modules/decode/bypass/data_bypass_interface.h>

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

    auto is_bypassible() const { return !this->is_conditional_move() && !this->is_accumulating_instr(); }
    
    auto is_complex_arithmetic() const { return this->is_divmult(); }
    auto is_mem_stage_required() const { return this->is_load()  ||
                                                this->is_store() ||
                                                this->is_jump()  ||
                                                this->is_explicit_trap()  ||
                                                this->is_special(); }
    
    auto get_instruction_latency() const
    {
        if ( is_mem_stage_required())
            return 2_Lt;

        if ( is_complex_arithmetic())
            return 1_Lt + RegisterStage::get_last_execution_stage_latency();
        
        return 1_Lt;
    }
};

#endif // PERF_INSTR_H
