/**
 * trap_types.cpp - Trap type abstraction
 * @author Vsevolod Pukhov
 * Copyright 2019 MIPT-MIPS
 */

#include "trap.h"

#include <riscv.opcode.gen.h>
#include <unordered_map>

static constexpr const uint8 GDB_SIGNAL_0    = 0;
static constexpr const uint8 GDB_SIGNAL_INT  = 2;
static constexpr const uint8 GDB_SIGNAL_ILL  = 4;
static constexpr const uint8 GDB_SIGNAL_TRAP = 5;
static constexpr const uint8 GDB_SIGNAL_FPE  = 8;
static constexpr const uint8 GDB_SIGNAL_BUS  = 10;
static constexpr const uint8 GDB_SIGNAL_SYS  = 12;

/* Source: The MIPS64 and microMIPS64 Privileged Resource Architecture v5.04 */
static constexpr const uint8 MIPS_EXC_INT           =  0;
static constexpr const uint8 MIPS_EXC_MOD           =  1;
static constexpr const uint8 MIPS_EXC_TLBL          =  2;
static constexpr const uint8 MIPS_EXC_TLBS          =  3;
static constexpr const uint8 MIPS_EXC_ADEL          =  4; /* Loads; instruction fetch (UNALIGNED_FETCH and UNKNOWN_INSTRUCTION)*/
static constexpr const uint8 MIPS_EXC_ADES          =  5; /* Stores */
static constexpr const uint8 MIPS_EXC_IBE           =  6;
static constexpr const uint8 MIPS_EXC_DBE           =  7;
static constexpr const uint8 MIPS_EXC_SYS           =  8; /* Syscall */
static constexpr const uint8 MIPS_EXC_BP            =  9; /* Breakpoint */
static constexpr const uint8 MIPS_EXC_RI            = 10; /* Reserved instr */
static constexpr const uint8 MIPS_EXC_CPU           = 11; /* Coprocessor unusable */
static constexpr const uint8 MIPS_EXC_OV            = 12; /* Arithmetic overflow */
static constexpr const uint8 MIPS_EXC_TR            = 13; /* Explicit trap */
static constexpr const uint8 MIPS_EXC_MSAFPE        = 14;
static constexpr const uint8 MIPS_EXC_FPE           = 15; /* Division by zero */
static constexpr const uint8 MIPS_EXC_FPOVF         = 16; /* Free implementation; following MARS */
static constexpr const uint8 MIPS_EXC_FPUNDF        = 17; /* Free implementation; following MARS */
static constexpr const uint8 MIPS_EXC_C2E           = 18;
static constexpr const uint8 MIPS_EXC_TLBRI         = 19;
static constexpr const uint8 MIPS_EXC_TLBXI         = 20;
static constexpr const uint8 MIPS_EXC_MSADis        = 21;
static constexpr const uint8 MIPS_EXC_MDMX          = 22;
static constexpr const uint8 MIPS_EXC_WATCH         = 23;
static constexpr const uint8 MIPS_EXC_MCHECK        = 24;
static constexpr const uint8 MIPS_EXC_THREAD        = 25;
static constexpr const uint8 MIPS_EXC_DSPDIS        = 26;
static constexpr const uint8 MIPS_EXC_GE            = 27;
static constexpr const uint8 MIPS_EXC_CACHEERR      = 30;

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

std::array<std::string_view, Trap::MAX_TRAP_TYPE> Trap::TrapStrTable =
{{
    #define TRAP(name) #name ,
    #include "trap.def"
    #undef TRAP
}};

uint8 Trap::to_gdb_format()
{
    static const std::unordered_map<TrapType, uint8> to_gdb_conv =
    {
        { Trap::NO_TRAP,              GDB_SIGNAL_0    },
        { Trap::HALT,                 GDB_SIGNAL_0    },
        { Trap::EXPLICIT_TRAP,        GDB_SIGNAL_TRAP },
        { Trap::BREAKPOINT,           GDB_SIGNAL_TRAP },
        { Trap::SYSCALL,              GDB_SIGNAL_0    },
        { Trap::INTEGER_OVERFLOW,     GDB_SIGNAL_0    },
        { Trap::UNSUPPORTED_SYSCALL,  GDB_SIGNAL_SYS  },
        { Trap::UNKNOWN_INSTRUCTION,  GDB_SIGNAL_ILL  },
        { Trap::RESERVED_INSTRUCTION, GDB_SIGNAL_ILL  },
        { Trap::COPROCESSOR_UNUSABLE, GDB_SIGNAL_ILL  },
        { Trap::UNALIGNED_LOAD,       GDB_SIGNAL_BUS  },
        { Trap::UNALIGNED_STORE,      GDB_SIGNAL_BUS  },
        { Trap::UNALIGNED_FETCH,      GDB_SIGNAL_BUS  },
        { Trap::FP_OVERFLOW,          GDB_SIGNAL_FPE  },
        { Trap::FP_UNDERFLOW,         GDB_SIGNAL_FPE  },
        { Trap::FP_DIV_BY_ZERO,       GDB_SIGNAL_FPE  },
    };

    return to_gdb_conv.at( value);
}

void Trap::set_from_gdb_format(uint8 id)
{
    static const std::unordered_map<uint8, TrapType> from_gdb_conv =
    {
        { GDB_SIGNAL_0,    Trap::NO_TRAP             },
        { GDB_SIGNAL_INT,  Trap::BREAKPOINT          },
        { GDB_SIGNAL_TRAP, Trap::BREAKPOINT          },
        { GDB_SIGNAL_FPE,  Trap::FP_OVERFLOW         },
        { GDB_SIGNAL_BUS,  Trap::UNALIGNED_LOAD      },
        { GDB_SIGNAL_SYS,  Trap::UNSUPPORTED_SYSCALL },
        { GDB_SIGNAL_ILL,  Trap::UNKNOWN_INSTRUCTION },
    };

    auto it = from_gdb_conv.find( id);
    if ( it == from_gdb_conv.end())
        value = Trap::NO_TRAP;
    value = it->second;
}

uint8 Trap::to_riscv_format()
{
    static const std::unordered_map<TrapType, uint8> to_riscv_conv =
    {
        { Trap::NO_TRAP,              0                         },
        { Trap::HALT,                 0                         },
        { Trap::EXPLICIT_TRAP,        CAUSE_BREAKPOINT          },
        { Trap::BREAKPOINT,           CAUSE_BREAKPOINT          },
        { Trap::SYSCALL,              CAUSE_USER_ECALL          },
        { Trap::INTEGER_OVERFLOW,     0                         },
        { Trap::UNSUPPORTED_SYSCALL,  0                         },
        { Trap::UNKNOWN_INSTRUCTION,  CAUSE_ILLEGAL_INSTRUCTION },
        { Trap::RESERVED_INSTRUCTION, CAUSE_ILLEGAL_INSTRUCTION },
        { Trap::COPROCESSOR_UNUSABLE, CAUSE_ILLEGAL_INSTRUCTION },
        { Trap::UNALIGNED_LOAD,       CAUSE_MISALIGNED_LOAD     },
        { Trap::UNALIGNED_STORE,      CAUSE_MISALIGNED_STORE    },
        { Trap::UNALIGNED_FETCH,      CAUSE_MISALIGNED_FETCH    },
        { Trap::FP_OVERFLOW,          0                         },
        { Trap::FP_UNDERFLOW,         0                         },
        { Trap::FP_DIV_BY_ZERO,       0                         },
    };

    return to_riscv_conv.at( value);
}

void Trap::set_from_riscv_format(uint8 id)
{
    static const std::unordered_map<uint8, TrapType> from_riscv_conv =
    {
        { CAUSE_MISALIGNED_FETCH,    Trap::UNALIGNED_FETCH     },
        { CAUSE_FETCH_ACCESS,        Trap::NO_TRAP             },
        { CAUSE_ILLEGAL_INSTRUCTION, Trap::UNKNOWN_INSTRUCTION },
        { CAUSE_BREAKPOINT,          Trap::BREAKPOINT          },
        { CAUSE_MISALIGNED_LOAD,     Trap::UNALIGNED_LOAD      },
        { CAUSE_LOAD_ACCESS,         Trap::NO_TRAP             },
        { CAUSE_MISALIGNED_STORE,    Trap::UNALIGNED_STORE     },
        { CAUSE_STORE_ACCESS,        Trap::NO_TRAP             },
        { CAUSE_USER_ECALL,          Trap::SYSCALL             },
        { CAUSE_SUPERVISOR_ECALL,    Trap::SYSCALL             },
        { CAUSE_HYPERVISOR_ECALL,    Trap::SYSCALL             },
        { CAUSE_MACHINE_ECALL,       Trap::SYSCALL             },
        { CAUSE_FETCH_PAGE_FAULT,    Trap::NO_TRAP             },
        { CAUSE_LOAD_PAGE_FAULT,     Trap::NO_TRAP             },
        { CAUSE_STORE_PAGE_FAULT,    Trap::NO_TRAP             },
    };

    value = from_riscv_conv.at( id);
}

/* MARS follows this */
uint8 Trap::to_mips_format()
{
    static const std::unordered_map<TrapType, uint8> to_mips_conv =
    {
        // Trap::NO_TRAP              not presented
        // Trap::HALT                 not presented
        { Trap::EXPLICIT_TRAP,        MIPS_EXC_TR     },
        { Trap::BREAKPOINT,           MIPS_EXC_BP     },
        { Trap::SYSCALL,              MIPS_EXC_SYS    },
        { Trap::INTEGER_OVERFLOW,     MIPS_EXC_OV     },
        // Trap::UNSUPPORTED_SYSCALL  handled by kernel
        { Trap::UNKNOWN_INSTRUCTION,  MIPS_EXC_ADEL   },
        { Trap::RESERVED_INSTRUCTION, MIPS_EXC_RI     },
        { Trap::COPROCESSOR_UNUSABLE, MIPS_EXC_CPU    },
        { Trap::UNALIGNED_LOAD,       MIPS_EXC_ADEL   },
        { Trap::UNALIGNED_STORE,      MIPS_EXC_ADES   },
        { Trap::UNALIGNED_FETCH,      MIPS_EXC_ADEL   },
        { Trap::FP_OVERFLOW,          MIPS_EXC_FPOVF  },
        { Trap::FP_UNDERFLOW,         MIPS_EXC_FPUNDF },
        { Trap::FP_DIV_BY_ZERO,       MIPS_EXC_FPE    },
    };

    return to_mips_conv.at( value);
}

/* MARS follows this */
void Trap::set_from_mips_format(uint8 id)
{
    static const std::unordered_map<uint8, TrapType> from_mips_conv =
    {
        { MIPS_EXC_INT,      Trap::NO_TRAP              },
        { MIPS_EXC_MOD,      Trap::NO_TRAP              },
        { MIPS_EXC_TLBL,     Trap::NO_TRAP              },
        { MIPS_EXC_TLBS,     Trap::NO_TRAP              },
        { MIPS_EXC_ADEL,     Trap::UNALIGNED_LOAD       },
        { MIPS_EXC_ADES,     Trap::UNALIGNED_STORE      },
        { MIPS_EXC_IBE,      Trap::NO_TRAP              },
        { MIPS_EXC_DBE,      Trap::NO_TRAP              },
        { MIPS_EXC_SYS,      Trap::SYSCALL              },
        { MIPS_EXC_BP,       Trap::BREAKPOINT           },
        { MIPS_EXC_RI,       Trap::RESERVED_INSTRUCTION },
        { MIPS_EXC_CPU,      Trap::COPROCESSOR_UNUSABLE },
        { MIPS_EXC_OV,       Trap::INTEGER_OVERFLOW     },
        { MIPS_EXC_TR,       Trap::EXPLICIT_TRAP        },
        { MIPS_EXC_MSAFPE,   Trap::NO_TRAP              },
        { MIPS_EXC_FPE,      Trap::FP_DIV_BY_ZERO       },
        { MIPS_EXC_FPOVF,    Trap::FP_OVERFLOW          },
        { MIPS_EXC_FPUNDF,   Trap::FP_UNDERFLOW         },
        { MIPS_EXC_C2E,      Trap::NO_TRAP              },
        { MIPS_EXC_TLBRI,    Trap::NO_TRAP              },
        { MIPS_EXC_TLBXI,    Trap::NO_TRAP              },
        { MIPS_EXC_MSADis,   Trap::NO_TRAP              },
        { MIPS_EXC_MDMX,     Trap::NO_TRAP              },
        { MIPS_EXC_WATCH,    Trap::NO_TRAP              },
        { MIPS_EXC_MCHECK,   Trap::NO_TRAP              },
        { MIPS_EXC_THREAD,   Trap::NO_TRAP              },
        { MIPS_EXC_DSPDIS,   Trap::NO_TRAP              },
        { MIPS_EXC_GE,       Trap::NO_TRAP              },
        { MIPS_EXC_CACHEERR, Trap::NO_TRAP              },
    };

    value = from_mips_conv.at( id);
}
