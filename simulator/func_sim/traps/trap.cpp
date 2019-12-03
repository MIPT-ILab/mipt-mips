/**
 * trap_types.cpp - Trap type abstraction
 * @author Vsevolod Pukhov
 * Copyright 2019 MIPT-MIPS
 */

#include "trap.h"

#include <infra/exception.h>
#include <riscv.opcode.gen.h>

#include <cassert>
#include <vector>

static constexpr const uint8 GDB_SIGNAL_0    = 0;
// static constexpr const uint8 GDB_SIGNAL_INT  = 2;
static constexpr const uint8 GDB_SIGNAL_ILL  = 4;
static constexpr const uint8 GDB_SIGNAL_TRAP = 5;
// static constexpr const uint8 GDB_SIGNAL_FPE  = 8;
static constexpr const uint8 GDB_SIGNAL_BUS  = 10;
static constexpr const uint8 GDB_SIGNAL_SYS  = 12;

/* Source: The MIPS64 and microMIPS64 Privileged Resource Architecture v5.04 */
// static constexpr const uint8 MIPS_EXC_INT           =  0;
// static constexpr const uint8 MIPS_EXC_MOD           =  1;
// static constexpr const uint8 MIPS_EXC_TLBL          =  2;
// static constexpr const uint8 MIPS_EXC_TLBS          =  3;
static constexpr const uint8 MIPS_EXC_ADEL          =  4; /* Loads; instruction fetch (UNALIGNED_FETCH and UNKNOWN_INSTRUCTION)*/
static constexpr const uint8 MIPS_EXC_ADES          =  5; /* Stores */
// static constexpr const uint8 MIPS_EXC_IBE           =  6;
// static constexpr const uint8 MIPS_EXC_DBE           =  7;
static constexpr const uint8 MIPS_EXC_SYS           =  8; /* Syscall */
static constexpr const uint8 MIPS_EXC_BP            =  9; /* Breakpoint */
static constexpr const uint8 MIPS_EXC_RI            = 10; /* Reserved instr */
static constexpr const uint8 MIPS_EXC_CPU           = 11; /* Coprocessor unusable */
static constexpr const uint8 MIPS_EXC_OV            = 12; /* Arithmetic overflow */
static constexpr const uint8 MIPS_EXC_TR            = 13; /* Explicit trap */
// static constexpr const uint8 MIPS_EXC_MSAFPE        = 14;
static constexpr const uint8 MIPS_EXC_FPE           = 15; /* Division by zero */
static constexpr const uint8 MIPS_EXC_FPOVF         = 16; /* Free implementation; following MARS */
static constexpr const uint8 MIPS_EXC_FPUNDF        = 17; /* Free implementation; following MARS */
// static constexpr const uint8 MIPS_EXC_C2E           = 18;
// static constexpr const uint8 MIPS_EXC_TLBRI         = 19;
// static constexpr const uint8 MIPS_EXC_TLBXI         = 20;
// static constexpr const uint8 MIPS_EXC_MSADis        = 21;
// static constexpr const uint8 MIPS_EXC_MDMX          = 22;
// static constexpr const uint8 MIPS_EXC_WATCH         = 23;
// static constexpr const uint8 MIPS_EXC_MCHECK        = 24;
// static constexpr const uint8 MIPS_EXC_THREAD        = 25;
// static constexpr const uint8 MIPS_EXC_DSPDIS        = 26;
// static constexpr const uint8 MIPS_EXC_GE            = 27;
// static constexpr const uint8 MIPS_EXC_CACHEERR      = 30;

#if 0
static constexpr const uint8 ADDRESS_EXCEPTION_LOAD         =  4;
static constexpr const uint8 ADDRESS_EXCEPTION_STORE        =  5;
static constexpr const uint8 SYSCALL_EXCEPTION              =  8;
static constexpr const uint8 BREAKPOINT_EXCEPTION           =  9;
static constexpr const uint8 RESERVED_INSTRUCTION_EXCEPTION = 10;
static constexpr const uint8 ARITHMETIC_OVERFLOW_EXCEPTION  = 12;
static constexpr const uint8 TRAP_EXCEPTION                 = 13;
static constexpr const uint8 DIVIDE_BY_ZERO_EXCEPTION       = 15; /* Only FP-div */
static constexpr const uint8 FLOATING_POINT_OVERFLOW        = 16;
static constexpr const uint8 FLOATING_POINT_UNDERFLOW       = 17;
#endif

std::ostream& operator<<( std::ostream& out, const Trap& trap)
{
    static const std::vector<std::string_view> table =
    {{
        #define TRAP(name, gdb, riscv, mips) #name ,
        #include "trap.def"
        #undef TRAP
        "INVALID_TRAP"
    }};
    return out << table.at( trap.value).data();
}

uint8 Trap::to_gdb_format() const
{
    switch ( value) {
    #define TRAP(name, gdb, riscv, mips) case name: return gdb;
    #include "trap.def"
    #undef TRAP
    default: throw InvalidTrapConversion("Invalid trap");
    }
}

Trap Trap::from_gdb_format(uint8 id)
{
    #define TRAP(name, gdb, riscv, mips) if ( id == ( gdb)) return Trap(name);
    #include "trap.def"
    #undef TRAP
    throw InvalidTrapConversion("Unsupported GDB trap code (" + std::to_string(id) + ")");
}

uint8 Trap::to_riscv_format() const
{
    switch ( value) {
    #define TRAP(name, gdb, riscv, mips) case name: return riscv;
    #include "trap.def"
    #undef TRAP
    default: throw InvalidTrapConversion("Invalid trap");
    }
}

Trap Trap::from_riscv_format(uint8 id)
{
    #define TRAP(name, gdb, riscv, mips) if ( id == ( riscv)) return Trap(name);
    #include "trap.def"
    #undef TRAP
    throw InvalidTrapConversion("Unsupported RISC-V trap code (" + std::to_string(id) + ")");
}

uint8 Trap::to_mips_format() const
{
    switch ( value) {
    #define TRAP(name, gdb, riscv, mips) case name: return mips;
    #include "trap.def"
    #undef TRAP
    default: throw InvalidTrapConversion("Invalid trap");
    }
}

Trap Trap::from_mips_format(uint8 id)
{
    #define TRAP(name, gdb, riscv, mips) if ( id == ( mips)) return Trap(name);
    #include "trap.def"
    #undef TRAP
    throw InvalidTrapConversion("Unsupported MIPS trap code (" + std::to_string(id) + ")");
}
