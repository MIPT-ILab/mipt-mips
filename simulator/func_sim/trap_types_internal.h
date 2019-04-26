#ifndef TRAP_TYPES_INTERNAL_H
#define TRAP_TYPES_INTERNAL_H

enum GDB_TrapType : uint8
{
    GDB_SIGNAL_0    = 0,
    GDB_SIGNAL_TRAP = 5,
    GDB_SIGNAL_SYS  = 12,
    GDB_SIGNAL_BUS  = 10,
    GDB_SIGNAL_ILL  = 4,
};

/* Source: The MIPS64 and microMIPS64 Privileged Resource Architecture v5.04 */
enum MIPS_TrapType : uint8
{                                /* Implementation comments */
    MIPS_EXC_INT           =  0,
    MIPS_EXC_MOD           =  1,
    MIPS_EXC_TLBL          =  2,
    MIPS_EXC_TLBS          =  3,
    MIPS_EXC_ADEL          =  4, /* Loads, instruction fetch */
    MIPS_EXC_ADES          =  5, /* Stores */
    MIPS_EXC_IBE           =  6,
    MIPS_EXC_DBE           =  7,
    MIPS_EXC_SYS           =  8, /* Syscall */
    MIPS_EXC_BP            =  9, /* Breakpoint */
    MIPS_EXC_RI            = 10, /* Reserved instr */
    MIPS_EXC_CPU           = 11, /* Coprocessor unusable */
    MIPS_EXC_OV            = 12, /* Arithmetic overflow */
    MIPS_EXC_TR            = 13, /* Explicit trap */
    MIPS_EXC_MSAFPE        = 14,
    MIPS_EXC_FPE           = 15,
    MIPS_EXC_FPOVF         = 16, /* Free implementation, following MARS */
    MIPS_EXC_FPUNDF        = 17, /* Free implementation, following MARS */
    MIPS_EXC_C2E           = 18,
    MIPS_EXC_TLBRI         = 19,
    MIPS_EXC_TLBXI         = 20,
    MIPS_EXC_MSADis        = 21,
    MIPS_EXC_MDMX          = 22,
    MIPS_EXC_WATCH         = 23,
    MIPS_EXC_MCHECK        = 24,
    MIPS_EXC_THREAD        = 25,
    MIPS_EXC_DSPDIS        = 26,
    MIPS_EXC_GE            = 27,
    MIPS_EXC_RESERVED28    = 28,
    MIPS_EXC_RESERVED29    = 29,
    MIPS_EXC_CACHEERR      = 30,
    MIPS_EXC_RESERVED31    = 31,
};

enum MARS_TrapType : uint8
{
    ADDRESS_EXCEPTION_LOAD         =  4,
    ADDRESS_EXCEPTION_STORE        =  5,
    SYSCALL_EXCEPTION              =  8,
    BREAKPOINT_EXCEPTION           =  9,
    RESERVED_INSTRUCTION_EXCEPTION = 10,
    ARITHMETIC_OVERFLOW_EXCEPTION  = 12,
    TRAP_EXCEPTION                 = 13,
    DIVIDE_BY_ZERO_EXCEPTION       = 15, /* Only FP-div */
    FLOATING_POINT_OVERFLOW        = 16,
    FLOATING_POINT_UNDERFLOW       = 17,
};

#endif // TRAP_TYPES_INTERNAL_H
