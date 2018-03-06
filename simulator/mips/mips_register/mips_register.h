 /* mips_register.h - MIPS register info class
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2014-2018 MIPT-MIPS
 */

#ifndef MIPS_REG_H
#define MIPS_REG_H

#include <array>
#include <iostream>
#include <utility>

// MIPT-MIPS modules
#include <infra/types.h>
#include <infra/macro.h>
#include <infra/string/string_view.h>

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

    explicit MIPSRegister( uint32 id) : value( static_cast<RegNum>( id)) { }
    
    friend inline std::ostream& operator<<( std::ostream& out, const MIPSRegister& rhs)
    {
        switch( rhs.value)
        {
            case 0: return out << "zero";
            case 1: return out << "at";
            case 2: return out << "v0";
            case 3: return out << "v1";
            case 4: return out << "a0";
            case 5: return out << "a1";
            case 6: return out << "a2";
            case 7: return out << "a3";
            case 8: return out << "t0";
            case 9: return out << "t1";
            case 10: return out << "t2";
            case 11: return out << "t3";
            case 12: return out << "t4";
            case 13: return out << "t5";
            case 14: return out << "t6";
            case 15: return out << "t7";
            case 16: return out << "s0";
            case 17: return out << "s1";
            case 18: return out << "s2";
            case 19: return out << "s3";
            case 20: return out << "s4";
            case 21: return out << "s5";
            case 22: return out << "s6";
            case 23: return out << "s7";
            case 24: return out << "t8";
            case 25: return out << "t9";
            case 26: return out << "k0";
            case 27: return out << "k1";
            case 28: return out << "gp";
            case 29: return out << "sp";
            case 30: return out << "fp";
            case 31: return out << "ra";
            case 32: return out << "hi";
            case 33: return out << "lo";
            case 34: return out << "hi_lo";
            default: return out << "Null";
        }

    }

    bool is_zero()       const { return value == MIPS_REG_zero; }
    bool is_mips_hi()    const { return value == MIPS_REG_hi; }
    bool is_mips_lo()    const { return value == MIPS_REG_lo; }
    bool is_mips_hi_lo() const { return value == MIPS_REG_hi_lo; }
    bool is_mips_ra()    const { return value == MIPS_REG_ra; }
    size_t to_size_t()   const { return value; }
    
    static const MIPSRegister mips_hi;
    static const MIPSRegister mips_lo;
    static const MIPSRegister mips_hi_lo;
    static const MIPSRegister zero;
    static const MIPSRegister return_address;

    bool operator==( const MIPSRegister& rhs) const { return value == rhs.value; }
private:
    RegNum value = MIPS_REG_zero;
    static std::array<std::string_view, MAX_REG> regTable;
};

namespace std {

template<> struct hash<MIPSRegister>
{
    auto operator()( const MIPSRegister& v) const noexcept { return hash<size_t>()( v.to_size_t()); }    
};
    
} // namespace std

#endif
