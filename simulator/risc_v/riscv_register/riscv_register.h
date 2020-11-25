/* riscv_register.h - RISCV register info class
 * @author Alexandr Misevich
 * Copyright 2018 MIPT-MIPS
 */

#ifndef RISCV_REG_H
#define RISCV_REG_H

#include <infra/macro.h>
#include <infra/types.h>

#include <array>
#include <cassert>
#include <iosfwd>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

class RISCVRegister {
    enum RegNum : size_t
    {
#define REGISTER(X) RISCV_REG_ ## X
#include "riscv_register.def"
#undef REGISTER
#define DECLARE_CSR(X, Y) , RISCV_ ## Y
#include <riscv.opcode.gen.h>
#undef DECLARE_CSR
        , MAX_VAL_RegNum
    };

    static RegNum get_csr_regnum( size_t val);
    static RegNum get_csr_regnum( std::string_view name);

public:
    static constexpr const size_t MAX_REG = MAX_VAL_RegNum;
    static const size_t popular_reg_shift = size_t( RISCV_REG_s0);

    friend std::ostream& operator<<( std::ostream& out, const RISCVRegister& rhs);

    std::string_view dump()        const { return regTable.at( value); }
    bool is_zero()                 const { return value == RISCV_REG_zero; }
    static constexpr bool is_mips_hi() { return false; }
    static constexpr bool is_mips_lo() { return false; }
    static auto from_cpu_index( size_t id) { return RISCVRegister( RegNum{ id}); }
    static auto from_gdb_index( size_t id) { return RISCVRegister( RegNum{ id}); }
    static auto from_csr_index( size_t id) { return RISCVRegister( get_csr_regnum( id)); }
    static auto from_csr_name( std::string_view name) { return RISCVRegister( get_csr_regnum( name)); }
    static auto from_cpu_popular_index( size_t id) {  id += popular_reg_shift; return RISCVRegister( RegNum{ id}); }
    static constexpr uint8 get_gdb_pc_index() { return 37; }
    size_t to_rf_index()           const { return value; }

    static constexpr RISCVRegister mips_hi() noexcept;
    static constexpr RISCVRegister mips_lo() noexcept;
    static constexpr RISCVRegister zero() noexcept;
    static constexpr RISCVRegister return_address() noexcept;
    static constexpr RISCVRegister cause() noexcept;

    bool operator==( const RISCVRegister& rhs) const { return value == rhs.value; }
    bool operator!=( const RISCVRegister& rhs) const { return !operator==(rhs); }
    bool is_valid() const { return value != MAX_VAL_RegNum; }

private:
    RegNum value = RISCV_REG_zero;
    static const std::array<std::string_view, MAX_REG> regTable;

    explicit constexpr RISCVRegister( RegNum id) noexcept : value( id) {}
    constexpr RISCVRegister() noexcept : RISCVRegister( RISCV_REG_zero) {}
    friend struct RISCVInstrDecoder;
};

constexpr inline RISCVRegister RISCVRegister::zero() noexcept { return RISCVRegister( RISCV_REG_zero); }
constexpr inline RISCVRegister RISCVRegister::return_address() noexcept { return RISCVRegister( RISCV_REG_ra); }
constexpr inline RISCVRegister RISCVRegister::mips_hi() noexcept { return RISCVRegister( MAX_VAL_RegNum); }
constexpr inline RISCVRegister RISCVRegister::mips_lo() noexcept { return RISCVRegister( MAX_VAL_RegNum); }
constexpr inline RISCVRegister RISCVRegister::cause() noexcept { return RISCVRegister( MAX_VAL_RegNum); }

#endif
