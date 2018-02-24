/**
 * data_bypass.h - Implementation of DataBypass class
 * 
 * @author Denis Los
 * Copyright 2018 MIPT-MIPS Project
 */


#ifndef INFRA_DATA_BYPASS_H
#define INFRA_DATA_BYPASS_H


#include <array>

#include <mips/mips_instr.h>
#include <infra/ports/timing.h>



class DataBypass
{
    public:
        struct BypassCommand
        {
            enum class Direction
            {
                GET_FROM_EXECUTE,
                GET_FROM_MEMORY
            };

            enum class Specifier
            {
                GET_REGULAR,
                GET_HI,
                GET_LO
            };

            Direction direction;
            Specifier specifier;
        };

        enum class RegisterStage
        {
            EXECUTE,
            MEMORY,
            WRITEBACK,
            IN_RF
        };

    private:
        struct RegisterInfo
        {
            RegisterStage current_stage = RegisterStage::IN_RF;
            RegisterStage ready_stage = RegisterStage::IN_RF;
            bool is_bypassible = false;
            Cycle master_instr_hash_cycle = 0_Cl;
        };


        class Scoreboard
        {
            public:
                RegisterInfo& get_entry( RegNum num)
                {
                    return array.at( static_cast<std::size_t>( num));
                }

                const RegisterInfo& get_entry( RegNum num) const
                {
                    return array.at( static_cast<std::size_t>( num));
                }

            private:
                std::array<RegisterInfo, REG_NUM_MAX> array = {};
        };


        static auto eval_instr_hash_cycle( const Cycle& cycle, RegisterStage next_dst_stage)
        {
            Latency latency;

            switch( next_dst_stage)
            {
                case RegisterStage::EXECUTE:
                    latency = 0_Lt;
                    break;
                
                case RegisterStage::MEMORY:
                    latency = 1_Lt;
                    break;
                
                case RegisterStage::WRITEBACK:
                    latency = 2_Lt;
                    break;
                
                case RegisterStage::IN_RF:
                    latency = 3_Lt;
                    break;
            }

            return cycle - latency;
        }

        auto get_current_stage( RegNum num) const
        {
            return scoreboard.get_entry( num).current_stage;
        }

        void update_register_info( const MIPSInstr& instr, RegNum num,
                                   const Cycle& cycle,
                                   DataBypass::RegisterStage next_dst_stage);

        void set_initial_state_to_reg_info( RegNum num)
        {
            auto& entry = scoreboard.get_entry( num);
            entry.current_stage = RegisterStage::IN_RF;
            entry.is_bypassible = false; 
        }

        Scoreboard scoreboard = {};
        bool is_LO_master_DIVMULT = false;
        bool is_HI_master_DIVMULT = false;

    public:
        auto is_in_RF( RegNum num) const
        {
            return scoreboard.get_entry( num).current_stage == RegisterStage::IN_RF;
        }

        auto is_bypassible( RegNum num) const
        {
            return scoreboard.get_entry( num).is_bypassible;
        }

        auto get_bypass_command( RegNum num) const
        {
            const auto current_stage = get_current_stage( num);

            BypassCommand bypass_command;
            
            if ( current_stage == RegisterStage::EXECUTE)
                bypass_command.direction = BypassCommand::Direction::GET_FROM_EXECUTE;
            else if ( current_stage == RegisterStage::MEMORY)
                bypass_command.direction = BypassCommand::Direction::GET_FROM_MEMORY;

            if ( num == REG_NUM_HI && is_HI_master_DIVMULT)
                bypass_command.specifier = BypassCommand::Specifier::GET_HI;
            else if ( num == REG_NUM_LO && is_LO_master_DIVMULT)
                bypass_command.specifier = BypassCommand::Specifier::GET_LO;
            else 
                bypass_command.specifier = BypassCommand::Specifier::GET_REGULAR;
     
            return bypass_command;
        }

        void update( const MIPSInstr& instr, const Cycle& cycle, 
                     RegisterStage next_dst_stage);

        void cancel( const MIPSInstr& instr);
};



#endif // INFRA_DATA_BYPASS_H