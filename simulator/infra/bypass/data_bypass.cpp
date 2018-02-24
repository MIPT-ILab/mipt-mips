/**
 * data_bypass.cpp - Implementation of DataBypass class
 * 
 * @author Denis Los
 * Copyright 2018 MIPT-MIPS Project
 */


#include "data_bypass.h"


#include <mips/mips_instr.h>
#include <infra/ports/timing.h>

#include <iostream>
void DataBypass::update_register_info( const MIPSInstr& instr, RegNum num,
                                       const Cycle& cycle,
                                       DataBypass::RegisterStage next_dst_stage)
{
    auto& entry = scoreboard.get_entry( num);

    if ( next_dst_stage == DataBypass::RegisterStage::EXECUTE)
    {
        entry.master_instr_hash_cycle = cycle;
        entry.is_bypassible = false;
        
        if ( instr.is_div_mult())
        {
            is_HI_master_DIVMULT = true;
            is_LO_master_DIVMULT = true;
        }
        else if ( num == REG_NUM_HI)
            is_HI_master_DIVMULT = false;
        else if ( num == REG_NUM_LO)
            is_LO_master_DIVMULT = false;

        if ( instr.is_conditional_move())
            entry.ready_stage = DataBypass::RegisterStage::IN_RF;
        else
            entry.ready_stage = instr.is_load() ? DataBypass::RegisterStage::MEMORY
                                                : DataBypass::RegisterStage::EXECUTE;
    }
    else if ( entry.master_instr_hash_cycle != eval_instr_hash_cycle( cycle, next_dst_stage))
        return;

    entry.current_stage = next_dst_stage;

    if ( entry.current_stage == entry.ready_stage)
        entry.is_bypassible = true;    
}



void DataBypass::update( const MIPSInstr& instr, const Cycle& cycle, 
                         DataBypass::RegisterStage next_dst_stage)
{
    auto dst_reg_num = instr.get_dst_num();

    if ( dst_reg_num == REG_NUM_ZERO)
        return;

    if ( dst_reg_num == REG_NUM_HI_LO)
    {
        update_register_info( instr, REG_NUM_LO, cycle, next_dst_stage);
        update_register_info( instr, REG_NUM_HI, cycle, next_dst_stage);
        return;
    }    
    
    update_register_info( instr, dst_reg_num, cycle, next_dst_stage);
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