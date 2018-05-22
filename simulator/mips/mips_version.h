 /*
 * mips_version.h - information about MIPS versions
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2018 MIPT-MIPS
 */

#ifndef MIPS_VERSION_H
#define MIPS_VERSION_H

#include <infra/types.h>

enum MIPSVersion {
    MIPS_I   = 1 << 1,
    MIPS_II  = 1 << 2,
    MIPS_III = 1 << 3,
    MIPS_IV  = 1 << 4,
    MIPS_V   = 1 << 5,
    MIPS_32  = 1 << 6,
    MIPS_64  = 1 << 7,
    // MIPS64 is a superset of MIPSV
    MIPS_64_Instr  = MIPS_64,
    MIPS_V_Instr   = MIPS_V   | MIPS_64,
    // MIPSI-V are supersets of precedessors
    MIPS_IV_Instr  = MIPS_IV  | MIPS_V_Instr,
    MIPS_III_Instr = MIPS_III | MIPS_IV_Instr,
    MIPS_II_Instr  = MIPS_II  | MIPS_III_Instr,
    MIPS_I_Instr   = MIPS_I   | MIPS_II_Instr,
};

constexpr static inline bool is_supported( MIPSVersion mask, MIPSVersion version)
{
    return (mask & version) != 0;
}

template<MIPSVersion V> struct MIPSRegisterUIntGen;
template<> struct MIPSRegisterUIntGen<MIPS_I>   { using type = uint32; };
template<> struct MIPSRegisterUIntGen<MIPS_II>  { using type = uint32; };
template<> struct MIPSRegisterUIntGen<MIPS_III> { using type = uint64; };
template<> struct MIPSRegisterUIntGen<MIPS_IV>  { using type = uint64; };
template<> struct MIPSRegisterUIntGen<MIPS_V>   { using type = uint64; };
template<> struct MIPSRegisterUIntGen<MIPS_32>  { using type = uint32; };
template<> struct MIPSRegisterUIntGen<MIPS_64>  { using type = uint64; };

template<MIPSVersion V> using MIPSRegisterUInt = typename MIPSRegisterUIntGen<V>::type;

#endif // MIPS_VERSION_H
