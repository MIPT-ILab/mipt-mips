 /* mips_register.h - MIPS register info class
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2014-2018 MIPT-MIPS
 */

#ifndef MIPS_REG_H
#define MIPS_REG_H

#include <cassert>

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

    explicit MIPSRegister( uint8 id) : MIPSRegister( static_cast<RegNum>( id))
    {
        if ( id >= 32u) {
            std::cerr << "ERROR: Invalid MIPS register id = " << id;
            exit( EXIT_FAILURE);
        }
    }

    friend std::ostream& operator<<( std::ostream& out, const MIPSRegister& rhs)
    {
        return out << regTable[ rhs.value];
    }

    bool is_zero()       const { return value == MIPS_REG_zero; }
    bool is_mips_hi()    const { return value == MIPS_REG_hi; }
    bool is_mips_lo()    const { return value == MIPS_REG_lo; }
    bool is_mips_hi_lo() const { return value == MIPS_REG_hi_lo; }
    size_t to_size_t()   const { return value; }

    static const MIPSRegister mips_hi;
    static const MIPSRegister mips_lo;
    static const MIPSRegister mips_hi_lo;
    static const MIPSRegister zero;
    static const MIPSRegister return_address;

    bool operator==( const MIPSRegister& rhs) const { return value == rhs.value; }
    bool operator!=( const MIPSRegister& rhs) const { return !operator==(rhs); }
private:
    RegNum value = MIPS_REG_zero;
    static std::array<std::string_view, MAX_REG> regTable;

    explicit MIPSRegister( RegNum id) : value( id) { }
};

namespace std {

template<> struct hash<MIPSRegister>
{
    auto operator()( const MIPSRegister& v) const noexcept { return hash<size_t>()( v.to_size_t()); }    
};
    
} // namespace std

#endif
