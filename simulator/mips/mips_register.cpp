#include "mips_register.h"

const MIPSRegister MIPSRegister::mips_hi = MIPSRegister( MIPS_REG_HI);
const MIPSRegister MIPSRegister::mips_lo = MIPSRegister( MIPS_REG_LO);
const MIPSRegister MIPSRegister::mips_hi_lo = MIPSRegister( MIPS_REG_HI_LO);
const MIPSRegister MIPSRegister::zero = MIPSRegister( MIPS_REG_ZERO);
const MIPSRegister MIPSRegister::return_address = MIPSRegister( MIPS_REG_RA);

std::array<std::string_view, MIPSRegister::MAX_REG> MIPSRegister::regTable =
{{
    "zero",
    "at",
    "v0", "v1",
    "a0", "a1", "a2", "a3",
    "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
    "t8", "t9",
    "k0", "k1",
    "gp",
    "sp",
    "fp",
    "ra",
    "hi",  "lo", "hi~lo"
}};
