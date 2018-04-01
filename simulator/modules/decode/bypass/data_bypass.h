/**
 * data_bypass.h - Implementation of bypassing unit
 * 
 * @author Denis Los
 * Copyright 2018 MIPT-MIPS Project
 */

#ifndef DATA_BYPASS_H
#define DATA_BYPASS_H

#include <array>

#include <modules/core/perf_instr.h>

#include "data_bypass_interface.h"

template <typename ISA>
class DataBypass
{
    using FuncInstr = typename ISA::FuncInstr;
    using Register  = typename ISA::Register;
    using Instr     = PerfInstr<FuncInstr>;
    using RegDstUInt = typename ISA::RegDstUInt;

    public:
        // checks whether the source register of passed instruction is in RF  
        auto is_in_RF( const Instr& instr, uint8 src_index) const
        {
            const auto reg_num = instr.get_src_num( src_index);
            return get_entry( reg_num).current_stage == RegisterStage::in_RF();
        }

        // checks whether the source register of passed instruction is bypassible
        auto is_bypassible( const Instr& instr, uint8 src_index) const
        {
            const auto reg_num = instr.get_src_num( src_index);
            return get_entry( reg_num).is_bypassible;
        }

        // checks if the stall needed for passed instruction
        auto is_stall( const Instr& instr) const
        {
            // add 1_Lt taking into account the fact that is_stall is invoked on the Decode stage
            const auto cycles_till_writeback = instr.get_cycles_till_writeback() + 1_Lt;

            return (( !is_in_RF( instr, 0) && !is_bypassible( instr, 0)) ||
                    ( !is_in_RF( instr, 1) && !is_bypassible( instr, 1)) ||
                    ( cycles_till_writeback <= writeback_stage_info.cycles_till_busy));
        }

        // returns bypass command for passed instruction and its source register
        // in accordance with current state of the scoreboard
        auto get_bypass_command( const Instr& instr, uint8 src_index) const
        {
            const auto reg_num = instr.get_src_num( src_index);
            return BypassCommand<Register>( get_current_stage( reg_num), reg_num);
        }

        // introduces new instruction to bypassing unit
        void trace_new_instr( const Instr& instr);

        // updates the scoreboard
        void update();

        // handle pipeline flush
        void handle_flush();
    
    private:
        struct RegisterInfo
        {
            private:
                class PipelineRoute
                {
                    public:
                        void set( const Instr& instr)
                        {
                            if ( instr.is_complex_arithmetic())
                                route_tag = 2; // COMPLEX_ARITHMETIC_ROUTE
                            else if ( instr.is_mem_stage_required())
                                route_tag = 1; // MEM_STAGE_ROUTE
                            else 
                                route_tag = 0; // SIMPLE_ROUTE
                        }

                        auto is_simple_route() const { return route_tag == 0; }
                        auto is_mem_stage_route() const { return route_tag == 1; }
                        auto is_complex_arithmetic_route() const { return route_tag == 2; }
                    
                    private:
                        uint8 route_tag;
                };

            public:
                RegisterStage current_stage = RegisterStage::in_RF();
                RegisterStage ready_stage = RegisterStage::in_RF();
                PipelineRoute pipeline_route;
                bool is_bypassible = false;
                bool is_traced = false;

                void set_pipeline_route( const Instr& instr) { pipeline_route.set( instr); }
                void change_current_stage()
                {
                    if ( current_stage == 0_RSG) // EXECUTE_0
                    {
                        if ( pipeline_route.is_complex_arithmetic_route())
                            current_stage = 1_RSG; // EXECUTE_1
                        else if ( pipeline_route.is_mem_stage_route())
                            current_stage = 3_RSG; // MEM
                        else 
                            current_stage = 4_RSG; // WRITEBACK
                    }
                    else if ( current_stage == 2_RSG) // EXECUTE_2
                    {
                        current_stage = 4_RSG; // WRITEBACK
                    }
                    else
                        current_stage.inc();
                }
        };

        struct FuncUnitInfo
        {
            Cycle cycles_till_busy = 0_Cl;
        };

        std::array<RegisterInfo, Register::MAX_REG> scoreboard = {};
        FuncUnitInfo writeback_stage_info = {};

        RegisterInfo& get_entry( Register num)
        {
            return scoreboard.at( num.to_size_t());
        }

        const RegisterInfo& get_entry( Register num) const
        {
            return scoreboard.at( num.to_size_t());
        }

        // returns current stage of passed register
        // in accordance with the current state of the scoreboard
        RegisterStage get_current_stage( Register num) const
        {
            return get_entry( num).current_stage;
        }

        // introduces a source register of a passed instruction to scoreboard 
        void trace_new_register( const Instr& instr, Register num);
};

template <typename ISA>
void DataBypass<ISA>::trace_new_register( const Instr& instr, Register num)
{
    auto& entry = get_entry( num);

    entry.current_stage = 0_RSG; // first execute stage

    entry.set_pipeline_route( instr);


    if ( !instr.is_bypassible()) {
        entry.ready_stage = RegisterStage::in_RF();
    }
    else if ( instr.is_complex_arithmetic()) {
        entry.ready_stage = 2_RSG;  // EXECUTE_2
    }
    else if ( instr.is_load()) {
        entry.ready_stage = 3_RSG;  // MEMORY
    }
    else
        entry.ready_stage = 0_RSG;  // EXECUTE_0


    entry.is_bypassible = ( entry.current_stage == entry.ready_stage);
    entry.is_traced = true;
}


template <typename ISA>
void DataBypass<ISA>::trace_new_instr( const Instr& instr)
{    
    const auto dst_reg_num = instr.get_dst_num();

    writeback_stage_info.cycles_till_busy = instr.get_cycles_till_writeback();

    if ( dst_reg_num.is_zero())
        return;
    
    if ( dst_reg_num.is_mips_hi_lo())
    {
        trace_new_register( instr, Register::mips_lo );
        trace_new_register( instr, Register::mips_hi );
        return;
    }

    trace_new_register( instr, dst_reg_num);
}


template <typename ISA>
void DataBypass<ISA>::update()
{
    for ( auto& entry:scoreboard)
    {
        if ( entry.is_traced)
        {
            if ( entry.current_stage.is_writeback())
            {
                entry.current_stage = RegisterStage::in_RF();
                entry.is_bypassible = false;
                entry.is_traced = false;
            }
            else
            {
                entry.change_current_stage();

                if ( entry.current_stage == entry.ready_stage)
                    entry.is_bypassible = true;   
            }
        }
    }

    writeback_stage_info.cycles_till_busy.dec();
}


template <typename ISA>
void DataBypass<ISA>::handle_flush()
{
    for ( auto& entry:scoreboard)
    {
        if ( entry.is_traced)
        {
            entry.current_stage = RegisterStage::in_RF();
            entry.is_bypassible = false;
            entry.is_traced = false;
        }
    }

    writeback_stage_info.cycles_till_busy = 0_Cl;
}

#endif // DATA_BYPASS_H
