/* riscv_register.h - RISCV register info class 
 * @author Alexandr Misevich
 * Copyright 2018 MIPT-MIPS
 */


#ifndef RISCV_REG_H
#define RISCV_REG_H

// MIPT-MIPS modules
#include <infra/macro.h>
#include <infra/string_view.h>
#include <infra/types.h>

#include <array>
#include <iostream>
#include <utility>

class RISCVRegister {
    enum RegNum : uint8
    {
#define REGISTER(X) RISCV_REG_ ## X
#include "riscv_register.def"
#undef REGISTER
        , MAX_VAL_RegNum
    };

public:
    static constexpr const size_t MAX_REG = MAX_VAL_RegNum;

    struct InvalidRegNum final : std::runtime_error
    {
        explicit InvalidRegNum( const uint8 num)
                : std::runtime_error( std::string( "Invalid RISC-V register num ") + std::to_string( num) + '\n')
        { }
    };

    explicit RISCVRegister( uint8 id) : RISCVRegister( static_cast<RegNum>( id))
    {
        if ( id >= 32u)
            throw InvalidRegNum( id);
    }

    friend std::ostream& operator<<( std::ostream& out, const RISCVRegister& rhs)
    {
        return out << regTable.at( rhs.value);
    }

    bool is_zero()                 const { return value == RISCV_REG_zero; }
    constexpr bool is_mips_hi()    const { return false; }
    constexpr bool is_mips_lo()    const { return false; }
    bool is_pc()                   const { return value == RISCV_REG_pc; }
    size_t to_size_t()             const { return value; }

    static const RISCVRegister zero;
    static const RISCVRegister return_address;
    static const RISCVRegister mips_hi;
    static const RISCVRegister mips_lo;
    static const RISCVRegister pc;

    bool operator==( const RISCVRegister& rhs) const { return value == rhs.value; }
    bool operator!=( const RISCVRegister& rhs) const { return !operator==(rhs); }
private:
    RegNum value = RISCV_REG_zero;
    static std::array<std::string_view, MAX_REG> regTable;

    explicit RISCVRegister( RegNum id) : value( id) {}

};

#endif
