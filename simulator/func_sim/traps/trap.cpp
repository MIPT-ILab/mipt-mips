/**
 * trap_types.cpp - Trap type abstraction
 * @author Vsevolod Pukhov
 * Copyright 2019 MIPT-MIPS
 */

#include "trap.h"

#include <infra/exception.h>
#include <riscv.opcode.gen.h>

#include <cassert>
#include <sstream>
#include <unordered_map>

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

struct Entry
{
    std::string_view str;
    uint8 gdb;
    uint8 riscv;
    uint8 mips;
};

static const auto& converter()
{
    static const std::unordered_map<Trap, Entry> instance =
    {
#define TRAP(name, gdb, riscv, mips) { Trap( Trap:: name), Entry{ #name, gdb, riscv, mips } },
#include "trap.def"
#undef TRAP
    };
    return instance;
}

template<typename T, T Entry::*ptr>
T convert_to( const Trap& trap) try
{
    return converter().at( trap).*ptr;
}
catch ( const std::out_of_range&)
{
    throw InvalidTrapConversion("Invalid trap");
}

template<typename T, T Entry::*ptr>
Trap convert_from( const T& id, std::string_view name)
{
    for ( const auto& [trap, e] : converter())
        if ( e.*ptr == id)
            return trap;

    std::ostringstream oss;
    oss << "Unsupported " << name << " trap code (" << id << ")";
    throw InvalidTrapConversion( oss.str());
}

std::ostream& operator<<( std::ostream& out, const Trap& trap)
{
    return out << convert_to<std::string_view, &Entry::str>( trap);
}

uint8 Trap::to_gdb_format()   const { return convert_to<uint8, &Entry::gdb>  ( *this); }
uint8 Trap::to_riscv_format() const { return convert_to<uint8, &Entry::riscv>( *this); }
uint8 Trap::to_mips_format()  const { return convert_to<uint8, &Entry::mips> ( *this); }

Trap Trap::from_gdb_format(uint8 id)   { return convert_from<uint8, &Entry::gdb>( id, "GDB"); }
Trap Trap::from_mips_format(uint8 id)  { return convert_from<uint8, &Entry::mips>( id, "MIPS"); }
Trap Trap::from_riscv_format(uint8 id) { return convert_from<uint8, &Entry::riscv>( id, "RISC-V"); }
