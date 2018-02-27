/**
 * data_bypass.cpp - Implementation of forwarding unit
 * 
 * @author Denis Los
 * Copyright 2018 MIPT-MIPS Project
 */


#include "data_bypass.h"


#include <mips/mips_instr.h>
#include <infra/ports/timing.h>



void DataBypass::trace_new_register( const MIPSInstr& instr, RegNum num)
{
    auto& entry = scoreboard.get_entry( num);

    entry.current_stage = 0_RSG;

    if ( instr.is_conditional_move())
        entry.ready_stage = RegisterStage::in_RF();
    else
        entry.ready_stage = instr.is_load() ? 1_RSG  // MEMORY 
                                            : 0_RSG; // EXECUTE

    if ( instr.get_dst_num() == REG_NUM_HI_LO)
    {
        is_HI_master_DIVMULT = true;
    }
    else if ( num == REG_NUM_HI)
    {
        is_HI_master_DIVMULT = false;
    }

    entry.is_bypassible = ( entry.current_stage == entry.ready_stage) ? true : false;

    traceable_registers.emplace( num);
}


void DataBypass::trace_new_instr( const MIPSInstr& instr)
{    
    const auto dst_reg_num = instr.get_dst_num();

    if ( dst_reg_num == REG_NUM_ZERO)
        return;

    if ( dst_reg_num == REG_NUM_HI_LO)
    {
        trace_new_register( instr, REG_NUM_LO);
        trace_new_register( instr, REG_NUM_HI);
        return;
    }

    trace_new_register( instr, dst_reg_num);
}


void DataBypass::update()
{
    for ( auto it = traceable_registers.begin(); it != traceable_registers.end();)
    {
        auto& entry = scoreboard.get_entry( *it);

        if ( entry.current_stage == RegisterStage::get_last_bypassing_stage())
        {
            entry.current_stage = RegisterStage::in_RF();
            entry.is_bypassible = false;
            it = traceable_registers.erase( it);
        }
        else
        {
            entry.current_stage.inc();

            if ( entry.current_stage == entry.ready_stage)
                entry.is_bypassible = true;

            ++it;
        }
    }
}


void DataBypass::cancel( const MIPSInstr& instr)
{
    auto dst_reg_num = instr.get_dst_num();

    if ( dst_reg_num == REG_NUM_ZERO)
        return;

    if ( dst_reg_num == REG_NUM_HI_LO)
    {
        untrace_register( REG_NUM_HI);
        untrace_register( REG_NUM_LO);
        return;
    }

    untrace_register( dst_reg_num);
}
