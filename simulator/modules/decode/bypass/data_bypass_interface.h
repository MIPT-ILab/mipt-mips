/**
* data_bypass_interface.h - Interface clases for bypassing units
*
* @author Denis Los
* Copyright 2018 MIPT-MIPS Project
*/

#ifndef DATA_BYPASS_INTERFACE_H
#define DATA_BYPASS_INTERFACE_H

#include <infra/types.h>
#include <infra/ports/timing.h>
#include <infra/macro.h>

class RegisterStage
{
public:
    auto operator==(const RegisterStage& rhs) const { return value == rhs.value; }

    void inc() { ++value; }

    static constexpr const uint8 BYPASSING_STAGES_NUMBER = 4;

    void set_to_first_execution_stage() { value = 0; }
    void set_to_mem_stage() { value = IN_RF_STAGE_VALUE - 2; }
    void set_to_writeback() { value = IN_RF_STAGE_VALUE - 1; }
    void set_to_in_RF() { value = IN_RF_STAGE_VALUE; }

    auto is_first_execution_stage() const { return value == 0; }
    auto is_mem_stage() const { return value == IN_RF_STAGE_VALUE - 2; }
    auto is_writeback() const { return value == IN_RF_STAGE_VALUE - 1; }
    auto is_in_RF() const { return value == IN_RF_STAGE_VALUE; }

    void set_to_last_execution_stage( uint8 last_execution_stage_value) { value = last_execution_stage_value; }
    auto is_last_execution_stage( uint8 last_execution_stage_value) const 
    { 
        return value == last_execution_stage_value; 
    }

private:
    uint8 value = IN_RF_STAGE_VALUE;

    // EXECUTE_0  - 0                              | Bypassing stage
    //  .......
    // EXECUTE_N  - last_execution_stage_value     | Bypassing stage
    // MEM        - MAX_VAL8 - 2                   | Bypassing stage
    // WRITEBACK  - MAX_VAL8 - 1                   | Bypassing stage
    // IN_RF      - MAX_VAL8

    static constexpr const uint8 IN_RF_STAGE_VALUE = MAX_VAL8;
};


template<typename Register>
class BypassCommand
{
public:
    BypassCommand(RegisterStage bypassing_stage, Register register_num, uint8 last_execution_stage_value)
        : bypassing_stage(bypassing_stage)
        , register_num(register_num)
        , last_execution_stage_value(last_execution_stage_value)
    { }

    auto get_bypassing_stage() const { return bypassing_stage; }
    auto get_register_num() const { return register_num; }

    // returns an index of the port where bypassed data should be get from
    auto get_bypass_direction() const
    {
        uint8 bypass_direction = 0;

        if ( bypassing_stage.is_first_execution_stage())
            bypass_direction = 0;

        if ( bypassing_stage.is_last_execution_stage( last_execution_stage_value))
            bypass_direction = 1;

        if ( bypassing_stage.is_mem_stage())
            bypass_direction = 2;

        if ( bypassing_stage.is_writeback())
            bypass_direction = 3;
        
        return bypass_direction;
    }

    template <typename T>
    T adapt_bypassed_data( T data) const
    {
        // NOLINTNEXTLINE(misc-suspicious-semicolon) https://bugs.llvm.org/show_bug.cgi?id=35824
        if constexpr(bitwidth<T> > 32) {
            if ( register_num.is_mips_hi())
                data >>= 32u;
        }

        return data;
    }

private:
    const RegisterStage bypassing_stage;
    const Register register_num;
    const uint8 last_execution_stage_value;
};

#endif // DATA_BYPASS_INTERFACE_H
