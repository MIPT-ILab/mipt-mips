/**
* data_bypass_interface.h - Interface clases for bypassing units
*
* @author Denis Los
* Copyright 2018 MIPT-MIPS Project
*/

#ifndef DATA_BYPASS_INTERFACE_H
#define DATA_BYPASS_INTERFACE_H

#include <infra/types.h>
#include <infra/macro.h>

class RegisterStage
{
public:
    constexpr explicit RegisterStage(uint8 value) : value(value) { }

    auto operator==(const RegisterStage& rhs) const { return value == rhs.value; }
    explicit operator uint8() const { return value; }

    void inc() { ++value; }

    static constexpr const uint8 BYPASSING_STAGES_NUMBER = 3;
    static constexpr RegisterStage in_RF() { return RegisterStage(IN_RF_STAGE_VALUE); }

    auto is_writeback() const { return value == WRITEBACK_STAGE_VALUE; }

private:
    uint8 value = 0;  // distance from first execute stage

                      // EXECUTE   - 0  | Bypassing stage
                      // MEMORY    - 1  | Bypassing stage
                      // WRITEBACK - 2  | Bypassing stage
                      // IN_RF     - MAX_VAL8

    static constexpr const uint8 IN_RF_STAGE_VALUE = MAX_VAL8;
    static constexpr const uint8 WRITEBACK_STAGE_VALUE = 2;
};

// NOLINTNEXTLINE(google-runtime-int) https://bugs.llvm.org/show_bug.cgi?id=24840
inline auto operator""_RSG(unsigned long long int number)
{
    return RegisterStage(static_cast<uint8>(number));
}

template<typename Register>
class BypassCommand
{
public:
    BypassCommand(RegisterStage bypassing_stage, Register register_num)
        : bypassing_stage(bypassing_stage)
        , register_num(register_num)
    { }

    auto get_bypassing_stage() const { return bypassing_stage; }
    auto get_register_num() const { return register_num; }

    // returns an index of the port where bypassed data should be get from
    // in accordance with passed bypass command
    auto get_bypass_direction() const { return static_cast<uint8>( get_bypassing_stage()); }

    template <typename T>
    T adapt_bypassed_data( T data) const
    {
        // NOLINTNEXTLINE(misc-suspicious-semicolon) https://bugs.llvm.org/show_bug.cgi?id=35824
        if constexpr(bitwidth<T> > 64) {
            if ( register_num.is_mips_hi())
                data >>= 32u;
        }

        return data;
    }

private:
    const RegisterStage bypassing_stage;
    const Register register_num;
};

#endif // DATA_BYPASS_INTERFACE_H
