/**
 * data_bypass.cpp - Implementation of forwarding unit
 * 
 * @author Denis Los
 * Copyright 2018 MIPT-MIPS Project
 */


#include "data_bypass.h"


#include <mips/mips_instr.h>
#include <infra/ports/timing.h>


void DataBypass::update_register_info( const MIPSInstr& instr, RegNum num,
                                       const Cycle& cycle,
                                       DataBypass::RegisterStage new_dst_stage)
{
    auto& entry = scoreboard.get_entry( num);

    if ( new_dst_stage == 0_RSG)
    {
        entry.first_execute_stage_cycle = cycle;
        entry.is_bypassible = false;
        
        if ( instr.get_dst_num() == REG_NUM_HI_LO)
        {
            is_HI_master_DIVMULT = true;
        }
        else if ( num == REG_NUM_HI)
        {
            is_HI_master_DIVMULT = false;
        }

        if ( instr.is_conditional_move())
            entry.ready_stage = DataBypass::RegisterStage::in_RF();
        else
            entry.ready_stage = instr.is_load() ? 1_RSG  // MEMORY
                                                : 0_RSG; // EXECUTE
    }
    else if ( entry.first_execute_stage_cycle + new_dst_stage.get_latency_from_first_execute_stage() != cycle)
        return;

    entry.current_stage = new_dst_stage;

    if ( entry.current_stage == entry.ready_stage)
        entry.is_bypassible = true;
}



void DataBypass::update( const MIPSInstr& instr, const Cycle& cycle, 
                         DataBypass::RegisterStage new_dst_stage)
{
    auto dst_reg_num = instr.get_dst_num();

    if ( dst_reg_num == REG_NUM_ZERO)
        return;

    if ( dst_reg_num == REG_NUM_HI_LO)
    {
        update_register_info( instr, REG_NUM_LO, cycle, new_dst_stage);
        update_register_info( instr, REG_NUM_HI, cycle, new_dst_stage);
        return;
    }    
    
    update_register_info( instr, dst_reg_num, cycle, new_dst_stage);
}


void DataBypass::cancel( const MIPSInstr& instr)
{
    auto dst_reg_num = instr.get_dst_num();

    if ( dst_reg_num == REG_NUM_ZERO)
        return;

    if ( dst_reg_num == REG_NUM_HI_LO)
    {
        set_initial_state_to_reg_info( REG_NUM_HI);
        set_initial_state_to_reg_info( REG_NUM_LO);
        return;
    }

    set_initial_state_to_reg_info( dst_reg_num);
}
