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
#include <cassert>
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

    friend std::ostream& operator<<( std::ostream& out, const MIPSRegister& rhs)
    {
        return out << regTable.at( rhs.value);
    }

    bool is_zero()       const { return value == MIPS_REG_zero; }
    bool is_mips_hi()    const { return value == MIPS_REG_hi; }
    bool is_mips_lo()    const { return value == MIPS_REG_lo; }

    static MIPSRegister from_cpu_index( uint8 id) {
        assert( id < 32u);
        return MIPSRegister( static_cast<RegNum>( id));
    }
    static MIPSRegister from_cp0_index( uint8 id) {
        /* To be implemented */
        (void) id; return MIPSRegister::zero;
    }
    static MIPSRegister from_gdb_index( uint8 id)
    {
        if (id < 32u)
            return from_cpu_index( id);
        else switch (id) {
            case 32: return from_cp0_index( 12); // SR
            case 33: return MIPSRegister::mips_lo;
            case 34: return MIPSRegister::mips_hi;
            case 35: return from_cp0_index( 8);  // Bad
            case 36: return from_cp0_index( 13); // Cause
            default: return MIPSRegister::zero;  // CP1 registers etc.
        }
    }
    size_t to_rf_index()  const { return value; }

    static const MIPSRegister mips_hi;
    static const MIPSRegister mips_lo;
    static const MIPSRegister zero;
    static const MIPSRegister return_address;

    bool operator==( const MIPSRegister& rhs) const { return value == rhs.value; }
    bool operator!=( const MIPSRegister& rhs) const { return !operator==(rhs); }
private:
    RegNum value = MIPS_REG_zero;
    static std::array<std::string_view, MAX_REG> regTable;

    explicit MIPSRegister( RegNum id) : value( id) { }
};

#endif
