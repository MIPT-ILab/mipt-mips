 /* mips_register.cpp - MIPS register info class
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2014-2018 MIPT-MIPS
 */

#include "mips_register.h"

#include <iostream>

std::ostream& operator<<( std::ostream& out, const MIPSRegister& rhs)
{
    return out << rhs.dump();
}

const std::array<std::string_view, MIPSRegister::MAX_REG> MIPSRegister::regTable =
{{
#define REGISTER(X) # X
#define CP0_REGISTER(X) # X
#define CP1_REGISTER(X) # X
#include "mips_register.def"
#undef CP1_REGISTER
#undef CP0_REGISTER
#undef REGISTER
}};
