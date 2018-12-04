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
#include <cassert>
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

    friend std::ostream& operator<<( std::ostream& out, const RISCVRegister& rhs)
    {
        return out << regTable.at( rhs.value);
    }

    bool is_zero()                 const { return value == RISCV_REG_zero; }
    constexpr bool is_mips_hi()    const { return false; }
    constexpr bool is_mips_lo()    const { return false; }
    static RISCVRegister from_cpu_index( uint8 id) { return RISCVRegister( RegNum{ id}); }
    size_t to_rf_index()           const { return value; }

    static constexpr RISCVRegister mips_hi() noexcept;
    static constexpr RISCVRegister mips_lo() noexcept;
    static constexpr RISCVRegister zero() noexcept;
    static constexpr RISCVRegister return_address() noexcept;
    static constexpr RISCVRegister cause() noexcept;

    bool operator==( const RISCVRegister& rhs) const { return value == rhs.value; }
    bool operator!=( const RISCVRegister& rhs) const { return !operator==(rhs); }

private:
    RegNum value = RISCV_REG_zero;
    static std::array<std::string_view, MAX_REG> regTable;

    explicit constexpr RISCVRegister( RegNum id) noexcept : value( id) {}
};

constexpr inline RISCVRegister RISCVRegister::zero() noexcept { return RISCVRegister( RISCV_REG_zero); }
constexpr inline RISCVRegister RISCVRegister::return_address() noexcept { return RISCVRegister( RISCV_REG_rs); }
constexpr inline RISCVRegister RISCVRegister::mips_hi() noexcept { return RISCVRegister( MAX_VAL_RegNum); }
constexpr inline RISCVRegister RISCVRegister::mips_lo() noexcept { return RISCVRegister( MAX_VAL_RegNum); }
constexpr inline RISCVRegister RISCVRegister::cause() noexcept { return RISCVRegister( MAX_VAL_RegNum); }

#endif
