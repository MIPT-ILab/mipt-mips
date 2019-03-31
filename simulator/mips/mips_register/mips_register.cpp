 /* mips_register.cpp - MIPS register info class
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2014-2018 MIPT-MIPS
 */

#include "mips_register.h"

std::array<std::string_view, MIPSRegister::MAX_REG> MIPSRegister::regTable =
{{
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define REGISTER(X) # X
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CP0_REGISTER(X) # X
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CP1_REGISTER(X) # X
#include "mips_register.def"
#undef CP1_REGISTER
#undef CP0_REGISTER
#undef REGISTER
}};
