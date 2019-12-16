 /*
 * mips_version.h - information about MIPS versions
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#ifndef MIPS_VERSION_H
#define MIPS_VERSION_H

#include <infra/types.h>

enum class MIPSVersion : unsigned {
    I, II, III, IV, V,
    v32,
    v64,
    mars,
    mars64,
};

template<MIPSVersion V> struct MIPSRegisterUIntGen;
template<> struct MIPSRegisterUIntGen<MIPSVersion::I>   { using type = uint32; };
template<> struct MIPSRegisterUIntGen<MIPSVersion::II>  { using type = uint32; };
template<> struct MIPSRegisterUIntGen<MIPSVersion::III> { using type = uint64; };
template<> struct MIPSRegisterUIntGen<MIPSVersion::IV>  { using type = uint64; };
template<> struct MIPSRegisterUIntGen<MIPSVersion::V>   { using type = uint64; };
template<> struct MIPSRegisterUIntGen<MIPSVersion::v32> { using type = uint32; };
template<> struct MIPSRegisterUIntGen<MIPSVersion::v64> { using type = uint64; };
template<> struct MIPSRegisterUIntGen<MIPSVersion::mars>   { using type = uint32; };
template<> struct MIPSRegisterUIntGen<MIPSVersion::mars64> { using type = uint64; };

template<MIPSVersion V> using MIPSRegisterUInt = typename MIPSRegisterUIntGen<V>::type;

class MIPSVersionMask {
    uint64 mask = 0;
public:
    constexpr MIPSVersionMask() = default;
    constexpr bool is_supported( MIPSVersion version) const noexcept {
        return ((mask >> static_cast<unsigned>(version)) & 0x1U) != 0;
    }
    constexpr MIPSVersionMask operator|( MIPSVersion rhs) const noexcept {
        MIPSVersionMask result = *this;
        result.mask |= ( 1ULL << static_cast<unsigned>(rhs));
        return result;
    }
    constexpr MIPSVersionMask operator|( MIPSVersionMask rhs) const noexcept {
        MIPSVersionMask result = *this;
        result.mask |= rhs.mask;
        return result;
    }
};

static constexpr const MIPSVersionMask MIPS_64_Instr  = MIPSVersionMask() | MIPSVersion::v64 | MIPSVersion::mars64;
static constexpr const MIPSVersionMask MIPS_32_Instr  = MIPS_64_Instr | MIPSVersion::v32 | MIPSVersion::mars;
// MIPS64 is a superset of MIPSV
static constexpr const MIPSVersionMask MIPS_V_Instr   = MIPS_64_Instr | MIPSVersion::V;
// MIPSI-V are supersets of precedessors
static constexpr const MIPSVersionMask MIPS_IV_Instr  = MIPS_V_Instr   | MIPSVersion::IV;
static constexpr const MIPSVersionMask MIPS_III_Instr = MIPS_IV_Instr  | MIPSVersion::III;
// MIPS32 is a superset of MIPSII
static constexpr const MIPSVersionMask MIPS_II_Instr  = MIPS_III_Instr | MIPS_32_Instr | MIPSVersion::II;
static constexpr const MIPSVersionMask MIPS_I_Instr   = MIPS_II_Instr  | MIPSVersion::I;

#endif // MIPS_VERSION_H
