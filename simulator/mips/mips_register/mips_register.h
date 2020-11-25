 /* mips_register.h - MIPS register info class
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2014-2018 MIPT-MIPS
 */

#ifndef MIPS_REG_H
#define MIPS_REG_H

#include <infra/macro.h>
#include <infra/types.h>

#include <array>
#include <cassert>
#include <iosfwd>
#include <string_view>
#include <utility>

class MIPSRegister {
    enum RegNum : size_t
    {
#define REGISTER(X) MIPS_REG_ ## X
#define CP0_REGISTER(X) MIPS_CP0_REG_ ## X
#define CP1_REGISTER(X) MIPS_CP1_REG_ ## X
#include "mips_register.def"
#undef REGISTER
#undef CP0_REGISTER
#undef CP1_REGISTER
        MAX_VAL_RegNum
    };

public:
    static constexpr const size_t MAX_REG = MAX_VAL_RegNum;

    auto dump() const
    {
        return regTable.at( value);
    }

    friend std::ostream& operator<<( std::ostream& out, const MIPSRegister& rhs);

    bool is_zero()       const { return value == MIPS_REG_zero; }
    bool is_mips_hi()    const { return value == MIPS_REG_hi; }
    bool is_mips_lo()    const { return value == MIPS_REG_lo; }

    static constexpr MIPSRegister from_cpu_index( size_t id) noexcept
    {
        return MIPSRegister( RegNum{ id});
    }

    static constexpr MIPSRegister from_cp0_index( size_t id) noexcept
    {
        return MIPSRegister( RegNum{ narrow_cast<size_t>( MIPS_CP0_REG_Context0 + id)});
    }

    static constexpr MIPSRegister from_cp1_index( uint8 id) noexcept
    {
        return MIPSRegister( RegNum{ narrow_cast<size_t>( MIPS_CP1_REG_f0 + id)});
    }

    static MIPSRegister from_gdb_index( size_t id)
    {
        if ( id < 32)
            return from_cpu_index( id);

        switch (id) {
            case 32: return MIPSRegister( MIPS_CP0_REG_SR);
            case 33: return MIPSRegister::mips_lo();
            case 34: return MIPSRegister::mips_hi();
            case 35: return MIPSRegister( MIPS_CP0_REG_BadVAddr);
            case 36: return MIPSRegister::cause();
            default: return MIPSRegister::zero();  // CP1 registers etc.
        }
    }
    static constexpr uint8 get_gdb_pc_index() { return 37; }
    static auto from_csr_name( std::string_view /* unused */) { return zero(); }

    constexpr size_t to_rf_index() const { return value; }

    static constexpr MIPSRegister mips_hi() noexcept;
    static constexpr MIPSRegister mips_lo() noexcept;
    static constexpr MIPSRegister mips_fcsr() noexcept;
    static constexpr MIPSRegister zero() noexcept;
    static constexpr MIPSRegister return_address() noexcept;
    static constexpr MIPSRegister cause() noexcept;
    static constexpr MIPSRegister status() noexcept;
    static constexpr MIPSRegister epc() noexcept;

    bool operator==( const MIPSRegister& rhs) const { return value == rhs.value; }
    bool operator!=( const MIPSRegister& rhs) const { return !operator==(rhs); }

private:
    RegNum value = MIPS_REG_zero;
    static const std::array<std::string_view, MAX_REG> regTable;

    explicit constexpr MIPSRegister( RegNum id) : value( id) { }
};

inline constexpr MIPSRegister MIPSRegister::mips_hi() noexcept { return MIPSRegister( MIPS_REG_hi); }
inline constexpr MIPSRegister MIPSRegister::mips_lo() noexcept { return MIPSRegister( MIPS_REG_lo); }
inline constexpr MIPSRegister MIPSRegister::mips_fcsr() noexcept { return MIPSRegister( MIPS_CP1_REG_f31); }
inline constexpr MIPSRegister MIPSRegister::zero() noexcept { return MIPSRegister( MIPS_REG_zero); }
inline constexpr MIPSRegister MIPSRegister::return_address() noexcept { return MIPSRegister( MIPS_REG_ra); }
inline constexpr MIPSRegister MIPSRegister::cause() noexcept { return MIPSRegister( MIPS_CP0_REG_Cause); }
inline constexpr MIPSRegister MIPSRegister::status() noexcept { return MIPSRegister( MIPS_CP0_REG_SR); }
inline constexpr MIPSRegister MIPSRegister::epc() noexcept { return MIPSRegister( MIPS_CP0_REG_EPC); }

#endif
