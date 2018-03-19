 /* mips_register.cpp - MIPS register info class
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2014-2018 MIPT-MIPS
 */

#include "mips_register.h"

const MIPSRegister MIPSRegister::mips_hi = MIPSRegister( MIPS_REG_hi);
const MIPSRegister MIPSRegister::mips_lo = MIPSRegister( MIPS_REG_lo);
const MIPSRegister MIPSRegister::mips_hi_lo = MIPSRegister( MIPS_REG_hi_lo);
const MIPSRegister MIPSRegister::mips_acc = MIPSRegister( MIPS_REG_acc);
const MIPSRegister MIPSRegister::zero = MIPSRegister( MIPS_REG_zero);
const MIPSRegister MIPSRegister::return_address = MIPSRegister( MIPS_REG_ra);

std::array<std::string_view, MIPSRegister::MAX_REG> MIPSRegister::regTable =
{{
#define REGISTER(X) # X
#include "mips_register.def"
#undef REGISTER
}};
