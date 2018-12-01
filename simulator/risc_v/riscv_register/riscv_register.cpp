 /* riscv_register.cpp - RISCV register info class
  * @author Alexandr Misevich
  * Copyright 2018 MIPT-MIPS 
  */

#include "riscv_register.h"

std::array<std::string_view, RISCVRegister::MAX_REG> RISCVRegister::regTable =
{{
#define REGISTER(X) # X
#include "riscv_register.def"
#undef REGISTER
}};
