/**
 * data_bypass.h - Implementation of forwarding unit
 * 
 * @author Denis Los
 * Copyright 2018 MIPT-MIPS Project
 */


#ifndef DATA_BYPASS_H
#define DATA_BYPASS_H


#include <array>

#include <mips/mips_instr.h>
#include <infra/ports/timing.h>



class DataBypass
{
    public:
        class RegisterStage
        {
            public:
                explicit RegisterStage( uint8 value) : value( value) { }

                auto operator==( const RegisterStage& rhs) const { return value == rhs.value; }
                explicit operator std::size_t() const { return static_cast<std::size_t>( value); }

                void inc() { ++value; }

                constexpr auto get_latency_from_first_execute_stage() const { return Latency( value); }

                static constexpr auto get_bypassing_stages_number() { return BYPASSING_STAGES_NUM; }
                static RegisterStage in_RF() { return RegisterStage( IN_RF_STAGE_VALUE); }

            private:
                uint8 value = 0;  // distance from first execute stage

                // EXECUTE   - 0  | Bypassing stage
                // MEMORY    - 1  | Bypassing stage
                // WRITEBACK - 2  | Bypassing stage
                // IN_RF     - 3

                static constexpr const uint8 IN_RF_STAGE_VALUE = 3;
                static constexpr const std::size_t BYPASSING_STAGES_NUM = 3;
        };

        class BypassCommand
        {
            public:
                BypassCommand( RegisterStage bypassing_stage, RegNum register_num)
                    : bypassing_stage( bypassing_stage)
                    , register_num( register_num)
                { }

                auto get_bypassing_stage() const { return bypassing_stage; }
                auto get_register_num() const { return register_num; }    

            private:
                const RegisterStage bypassing_stage;
                const RegNum register_num;
        };

    private:
        struct RegisterInfo
        {
            RegisterStage current_stage = RegisterStage::in_RF();
            RegisterStage ready_stage = RegisterStage::in_RF();
            bool is_bypassible = false;
            Cycle first_execute_stage_cycle = 0_Cl;
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

        auto get_current_stage( RegNum num) const
        {
            return scoreboard.get_entry( num).current_stage;
        }

        void update_register_info( const MIPSInstr& instr, RegNum num,
                                   const Cycle& cycle,
                                   RegisterStage new_dst_stage);

        void set_initial_state_to_reg_info( RegNum num)
        {
            auto& entry = scoreboard.get_entry( num);
            entry.current_stage = RegisterStage::in_RF();
            entry.is_bypassible = false; 
        }

        Scoreboard scoreboard = {};

        /* gives the information whether bypassed data should be transformed
           when bypassing is needed for HI register
        */ 
        bool is_HI_master_DIVMULT = false;

    public:
        auto is_in_RF( RegNum num) const
        {
            return scoreboard.get_entry( num).current_stage == RegisterStage::in_RF();
        }

        auto is_bypassible( RegNum num) const
        {
            return scoreboard.get_entry( num).is_bypassible;
        }

        auto is_stall( const MIPSInstr& instr) const
        {
            const auto src1_reg_num = instr.get_src1_num();
            const auto src2_reg_num = instr.get_src2_num();

            return ( (!is_in_RF( src1_reg_num) && !is_bypassible( src1_reg_num)) ||
                     (!is_in_RF( src2_reg_num) && !is_bypassible( src2_reg_num)) );
        }

        auto get_bypass_command( RegNum num) const
        {
            return BypassCommand( get_current_stage( num), num);
        }

        auto get_bypass_direction( const BypassCommand& bypass_command) const
        {
            const auto bypassing_stage = bypass_command.get_bypassing_stage();
            return static_cast<std::size_t>( bypassing_stage);
        }

        auto adapt_bypassed_data( const BypassCommand& bypass_command, uint64 data) const
        {
            const auto register_num = bypass_command.get_register_num();

            auto adapted_data = data;

            if ( register_num == REG_NUM_HI && is_HI_master_DIVMULT)
                adapted_data >>= 32;
            
            return adapted_data;
        }

        void update( const MIPSInstr& instr, const Cycle& cycle, 
                     RegisterStage new_dst_stage);

        void cancel( const MIPSInstr& instr);
};


inline auto operator""_RSG( unsigned long long int number) // NOLINT https://bugs.llvm.org/show_bug.cgi?id=24840
{
    return DataBypass::RegisterStage( static_cast<uint8>( number));
}


#endif // DATA_BYPASS_H
