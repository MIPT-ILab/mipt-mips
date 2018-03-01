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

class MIPSRegister {
    enum RegNum : uint8
    {
        MIPS_REG_ZERO = 0,
        MIPS_REG_AT,
        MIPS_REG_V0,
        MIPS_REG_V1,
        MIPS_REG_A0,
        MIPS_REG_A1,
        MIPS_REG_A2,
        MIPS_REG_A3,
        MIPS_REG_T0,
        MIPS_REG_T1,
        MIPS_REG_T2,
        MIPS_REG_T3,
        MIPS_REG_T4,
        MIPS_REG_T5,
        MIPS_REG_T6,
        MIPS_REG_T7,
        MIPS_REG_S0,
        MIPS_REG_S1,
        MIPS_REG_S2,
        MIPS_REG_S3,
        MIPS_REG_S4,
        MIPS_REG_S5,
        MIPS_REG_S6,
        MIPS_REG_S7,
        MIPS_REG_T8,
        MIPS_REG_T9,
        MIPS_REG_K0,
        MIPS_REG_K1,
        MIPS_REG_GP,
        MIPS_REG_SP,
        MIPS_REG_FP,
        MIPS_REG_RA,
        MIPS_REG_HI,
        MIPS_REG_LO,
        MIPS_REG_HI_LO,
        MIPS_REG_MAX
    };
    
public:
    static constexpr size_t MAX_REG = static_cast<size_t>( MIPS_REG_MAX);

    explicit MIPSRegister( uint32 id) : value( static_cast<RegNum>( id)) { }
    
    friend std::ostream& operator<<( std::ostream& out, const MIPSRegister& rhs)
    {
        return out << regTable[ rhs.value];
    }

    bool is_zero()       const { return value == MIPS_REG_ZERO; }
    bool is_mips_hi()    const { return value == MIPS_REG_HI; }
    bool is_mips_lo()    const { return value == MIPS_REG_LO; }
    bool is_mips_hi_lo() const { return value == MIPS_REG_HI_LO; }
    size_t to_size_t()   const { return value; }
    
    static const MIPSRegister mips_hi;
    static const MIPSRegister mips_lo;
    static const MIPSRegister mips_hi_lo;
    static const MIPSRegister zero;
    static const MIPSRegister return_address;

    bool operator==( const MIPSRegister& rhs) const { return value == rhs.value; }
private:
    RegNum value = MIPS_REG_ZERO;
    static std::array<std::string_view, MAX_REG> regTable;
};

namespace std {

template<> struct hash<MIPSRegister>
{
    auto operator()( const MIPSRegister& v) const noexcept { return hash<size_t>()( v.to_size_t()); }    
};
    
} // namespace std

#endif
