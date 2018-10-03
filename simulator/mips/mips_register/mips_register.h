 /* mips_register.h - MIPS register info class
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2014-2018 MIPT-MIPS
 */

#ifndef MIPS_REG_H
#define MIPS_REG_H

// MIPT-MIPS modules
#include <infra/macro.h>
#include <infra/string_view.h>
#include <infra/types.h>

#include <array>
#include <iostream>
#include <utility>

class MIPSRegister {
    enum RegNum : uint8
    {
#define REGISTER(X) MIPS_REG_ ## X
#include "mips_register.def"
#undef REGISTER
        , MAX_VAL_RegNum
    };

public:
    static constexpr const size_t MAX_REG = MAX_VAL_RegNum;

    struct InvalidRegNum final : std::runtime_error
    {
        explicit InvalidRegNum( const uint8 num)
                : std::runtime_error( std::string( "Invalid MIPS register num ") + std::to_string( num) + '\n')
        { }
    };

    explicit MIPSRegister( uint8 id) : MIPSRegister( static_cast<RegNum>( id))
    {
        if( id >= 32u)
            throw InvalidRegNum( id);
    }

    friend std::ostream& operator<<( std::ostream& out, const MIPSRegister& rhs)
    {
        return out << regTable.at( rhs.value);
    }

    bool is_zero()       const { return value == MIPS_REG_zero; }
    bool is_mips_hi()    const { return value == MIPS_REG_hi; }
    bool is_mips_lo()    const { return value == MIPS_REG_lo; }
    bool is_pc()         const { return value == MIPS_REG_pc; }
    size_t to_size_t()   const { return value; }

    static const MIPSRegister mips_hi;
    static const MIPSRegister mips_lo;
    static const MIPSRegister zero;
    static const MIPSRegister return_address;
    static const MIPSRegister pc;

    bool operator==( const MIPSRegister& rhs) const { return value == rhs.value; }
    bool operator!=( const MIPSRegister& rhs) const { return !operator==(rhs); }
private:
    RegNum value = MIPS_REG_zero;
    static std::array<std::string_view, MAX_REG> regTable;

    explicit MIPSRegister( RegNum id) : value( id) { }
};

#endif
