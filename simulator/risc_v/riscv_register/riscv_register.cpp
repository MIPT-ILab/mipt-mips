 /* riscv_register.cpp - RISCV register info class
  * @author Alexandr Misevich
  * Copyright 2018 MIPT-MIPS 
  */

#include "riscv_register.h"

const RISCVRegister RISCVRegister::zero = RISCVRegister( RISCV_REG_zero);
const RISCVRegister RISCVRegister::return_address = RISCVRegister( RISCV_REG_rs);
const RISCVRegister RISCVRegister::mips_hi = RISCVRegister( MAX_VAL_RegNum);
const RISCVRegister RISCVRegister::mips_lo = RISCVRegister( MAX_VAL_RegNum);

std::array<std::string_view, RISCVRegister::MAX_REG> RISCVRegister::regTable =
{{
#define REGISTER(X) # X
#include "riscv_register.def"
#undef REGISTER
}};
