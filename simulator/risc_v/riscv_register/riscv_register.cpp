 /* riscv_register.cpp - RISCV register info class
  * @author Alexandr Misevich
  * Copyright 2018 MIPT-MIPS 
  */

#include "riscv_register.h"

#include <iostream>

std::ostream& operator<<( std::ostream& out, const RISCVRegister& rhs)
{
    return out << rhs.dump();
}

const std::array<std::string_view, RISCVRegister::MAX_REG> RISCVRegister::regTable =
{{
#define REGISTER(X) # X
#include "riscv_register.def"
#undef REGISTER
#define DECLARE_CSR(X, Y) , # X
#include <riscv.opcode.gen.h>
#undef DECLARE_CSR
}};

template<typename Map>
static auto try_read( const Map& map, typename Map::key_type key, typename Map::mapped_type bad) try
{
    return map.at( key);
}
catch ( const std::out_of_range&)
{
    return bad;
}   

RISCVRegister::RegNum RISCVRegister::get_csr_regnum( size_t val)
{
    static const std::unordered_map<size_t, RegNum> csr_values =
    {
#define DECLARE_CSR(X, Y) { Y, RegNum::RISCV_ ## Y},
#include <riscv.opcode.gen.h>
#undef DECLARE_CSR
    };
    return try_read( csr_values, val, MAX_VAL_RegNum);
}

RISCVRegister::RegNum RISCVRegister::get_csr_regnum( std::string_view name)
{
    static const std::unordered_map<std::string_view, RegNum> csr_names = 
    {
#define DECLARE_CSR(X, Y) { # X, RegNum::RISCV_ ## Y},
#include <riscv.opcode.gen.h>
#undef DECLARE_CSR
    };
    return try_read( csr_names, name, MAX_VAL_RegNum);
}
    