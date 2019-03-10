 /* riscv_register.cpp - RISCV register info class
  * @author Alexandr Misevich
  * Copyright 2018 MIPT-MIPS 
  */

#include "riscv_register.h"

std::array<std::string_view, RISCVRegister::MAX_REG> RISCVRegister::regTable =
{{
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define REGISTER(X) # X
#include "riscv_register.def"
#undef REGISTER
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define DECLARE_CSR(X, Y) , # X
#include <riscv.opcode.gen.h>
#undef DECLARE_CSR
}};

RISCVRegister::RegNum RISCVRegister::get_csr_regnum( uint16 val)
{
    switch (val) {
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define DECLARE_CSR(X, Y) case Y: return RegNum::RISCV_ ## Y;
#include <riscv.opcode.gen.h>
#undef DECLARE_CSR
    }
    return MAX_VAL_RegNum;
}

RISCVRegister::RegNum RISCVRegister::get_csr_regnum( std::string_view name)
{
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define DECLARE_CSR(X, Y) if ( name == # X) return RegNum::RISCV_ ## Y;
#include <riscv.opcode.gen.h>
#undef DECLARE_CSR
    return MAX_VAL_RegNum;
}
