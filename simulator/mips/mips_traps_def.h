#ifdef DEF_MIPS_TRAP

DEF_MIPS_TRAP("no trap",             MIPS_NO_TRAP)
DEF_MIPS_TRAP("halt",                MIPS_HALT)
DEF_MIPS_TRAP("explicit trap",       MIPS_EXPLICIT_TRAP)
DEF_MIPS_TRAP("breakpoint",          MIPS_BREAKPOINT)
DEF_MIPS_TRAP("syscall",             MIPS_SYSCALL)
DEF_MIPS_TRAP("unsupported syscall", MIPS_UNSUPPORTED_SYSCALL)
DEF_MIPS_TRAP("unaligned adress",    MIPS_UNALIGNED_ADDRESS)
DEF_MIPS_TRAP("unknown instruction", MIPS_UNKNOWN_INSTRUCTION)

#endif

#ifndef MIPS_TRAP_ENCODINGS
#define MIPS_TRAP_ENCODINGS

#define MIPS_TRAP_NO_TRAP             0
#define MIPS_TRAP_HALT                1
#define MIPS_TRAP_EXPLICIT_TRAP       2
#define MIPS_TRAP_BREAKPOINT          3
#define MIPS_TRAP_SYSCALL             4
#define MIPS_TRAP_UNSUPPORTED_SYSCALL 5
#define MIPS_TRAP_UNALIGNED_ADRESS    6
#define MIPS_TRAP_UNKNOWN_INSTRUCTION 7

#endif