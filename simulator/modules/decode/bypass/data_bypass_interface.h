/**
* data_bypass_interface.h - Interface clases for bypassing units
*
* @author Denis Los
* Copyright 2018 MIPT-MIPS Project
*/

#ifndef DATA_BYPASS_INTERFACE_H
#define DATA_BYPASS_INTERFACE_H

#include <infra/macro.h>
#include <infra/ports/timing.h>
#include <infra/types.h>

class RegisterStage
{
public:
    bool operator==(const RegisterStage& rhs) const { return is_same_stage( rhs.value); }

    void inc() { value = value + 1_lt; }

    static constexpr const uint8 BYPASSING_STAGES_NUMBER = 5;

    void set_to_stage( Latency v) { value = v; }
    void set_to_first_execution_stage() { set_to_stage( 0_lt); 
                                          bypass_direction = 0; }
    void set_to_mem_stage()    { set_to_stage( MEM_OR_BRANCH_STAGE); 
                                 bypass_direction = 2; }
    void set_to_writeback()    { set_to_stage( WB_STAGE); 
                                 bypass_direction = 3; }
    void set_to_branch_stage() { set_to_stage( MEM_OR_BRANCH_STAGE); 
                                 bypass_direction = 4; }
    void set_to_in_RF()        { set_to_stage( IN_RF_STAGE); }

    bool is_same_stage( Latency v) const  { return value == v; }
    bool is_first_execution_stage() const { return is_same_stage( 0_lt); }
    bool is_mem_or_branch_stage() const { return is_same_stage( MEM_OR_BRANCH_STAGE); }
    bool is_writeback() const { return is_same_stage( WB_STAGE); }
    bool is_in_RF() const     { return is_same_stage( IN_RF_STAGE); }

    auto get_bypass_direction_value() const{ return bypass_direction; }

private:
    uint8 bypass_direction = 0;
    Latency value = IN_RF_STAGE;
    
    // EXECUTE_0  - 0                              | Bypassing stage
    //  .......
    // EXECUTE_N  - last_execution_stage_value     | Bypassing stage
    // MEM        - MAX_VAL8 - 2                   | Bypassing stage
    // BRANCH     - MAX_VAL8 - 2                   | Bypassing stage
    // WRITEBACK  - MAX_VAL8 - 1                   | Bypassing stage
    // IN_RF      - MAX_VAL8

    static constexpr const Latency IN_RF_STAGE  = Latency( MAX_VAL8);
    static constexpr const Latency MEM_OR_BRANCH_STAGE    = IN_RF_STAGE - 2_lt;
    static constexpr const Latency WB_STAGE     = IN_RF_STAGE - 1_lt;
};


template<typename Register>
class BypassCommand
{
public:
    BypassCommand(RegisterStage bypassing_stage, Latency last_execution_stage)
        : bypassing_stage(bypassing_stage)
        , last_execution_stage(last_execution_stage)
    { }

    // returns an index of the port where bypassed data should be get from
    auto get_bypass_direction() const
    {
        uint8 bypass_direction = 0;

        if ( bypassing_stage.is_same_stage( last_execution_stage))
            bypass_direction = 1;
        else 
            bypass_direction = bypassing_stage.get_bypass_direction_value();
        
        return bypass_direction;
    }

private:
    const RegisterStage bypassing_stage;
    const Latency last_execution_stage;
};

#endif // DATA_BYPASS_INTERFACE_H
