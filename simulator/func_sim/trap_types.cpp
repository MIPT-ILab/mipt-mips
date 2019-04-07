#include "trap_types.h"

#include <unordered_map>

std::array<std::string_view, Trap::MAX_TRAP_TYPE> Trap::TrapStrTable =
{{

    #define TRAP(name) #name
    #include "trap_types.def"
    #undef TRAP
}};

uint8 Trap::to_gdb_format()
{
    static const std::unordered_map<TrapType, GDB_TrapType> to_gdb_conv =
    {
        { Trap::NO_TRAP,             GDB_SIGNAL_0    },
        { Trap::HALT,                GDB_SIGNAL_0    },
        { Trap::EXPLICIT_TRAP,       GDB_SIGNAL_TRAP },
        { Trap::BREAKPOINT,          GDB_SIGNAL_TRAP },
        { Trap::SYSCALL,             GDB_SIGNAL_0    },
        { Trap::INTEGER_OVERFLOW,    GDB_SIGNAL_0    },
        { Trap::UNSUPPORTED_SYSCALL, GDB_SIGNAL_SYS  },  // Not implemented in gdb_interface
        { Trap::UNALIGNED_ADDRESS,   GDB_SIGNAL_BUS  },
        { Trap::UNKNOWN_INSTRUCTION, GDB_SIGNAL_ILL  },  // Not implemented in gdb_interface
    };

    auto it = to_gdb_conv.find( value);
    if ( it == to_gdb_conv.end())
        return GDB_SIGNAL_0;
    return it->second;
}

uint8 Trap::to_riscv_format()
{
    static const std::unordered_map<TrapType, uint8> to_riscv_conv =
    {
        { Trap::NO_TRAP,             0                         },
        { Trap::HALT,                0                         },
        { Trap::EXPLICIT_TRAP,       CAUSE_BREAKPOINT          },
        { Trap::BREAKPOINT,          CAUSE_BREAKPOINT          },
        { Trap::SYSCALL,             CAUSE_USER_ECALL          },
        { Trap::INTEGER_OVERFLOW,    0                         },
        { Trap::UNSUPPORTED_SYSCALL, 0                         },
        { Trap::UNALIGNED_ADDRESS,   CAUSE_MISALIGNED_LOAD     }, // _STORE, _FETCH
        { Trap::UNKNOWN_INSTRUCTION, CAUSE_ILLEGAL_INSTRUCTION },
    };

    auto it = to_riscv_conv.find( value);
    if ( it == to_riscv_conv.end())
        return 0;
    return it->second;
}

void Trap::set_from_gdb_format(GDB_TrapType id)
{
    static const std::unordered_map<GDB_TrapType, TrapType> from_gdb_conv =
    {
        { GDB_SIGNAL_0,    Trap::NO_TRAP             },
        { GDB_SIGNAL_TRAP, Trap::BREAKPOINT          },
        { GDB_SIGNAL_SYS,  Trap::UNSUPPORTED_SYSCALL },  // Not implemented in gdb_interface
        { GDB_SIGNAL_BUS,  Trap::UNALIGNED_ADDRESS   },
        { GDB_SIGNAL_ILL,  Trap::UNKNOWN_INSTRUCTION },  // Not implemented in gdb_interface
    };

    auto it = from_gdb_conv.find( id);
    if ( it == from_gdb_conv.end())
        value = Trap::NO_TRAP;
    value = it->second;
}

void Trap::set_from_riscv_format(uint8_t id)
{
    static const std::unordered_map<uint8, TrapType> from_riscv_conv =
    {
        { CAUSE_MISALIGNED_FETCH,    Trap::UNALIGNED_ADDRESS   },
        { CAUSE_FETCH_ACCESS,        Trap::NO_TRAP             },
        { CAUSE_ILLEGAL_INSTRUCTION, Trap::UNKNOWN_INSTRUCTION },
        { CAUSE_BREAKPOINT,          Trap::BREAKPOINT          },
        { CAUSE_MISALIGNED_LOAD,     Trap::UNALIGNED_ADDRESS   },
        { CAUSE_LOAD_ACCESS,         Trap::NO_TRAP             },
        { CAUSE_MISALIGNED_STORE,    Trap::UNALIGNED_ADDRESS   },
        { CAUSE_STORE_ACCESS,        Trap::NO_TRAP             },
        { CAUSE_USER_ECALL,          Trap::SYSCALL             },
        { CAUSE_SUPERVISOR_ECALL,    Trap::SYSCALL             },
        { CAUSE_HYPERVISOR_ECALL,    Trap::SYSCALL             },
        { CAUSE_MACHINE_ECALL,       Trap::SYSCALL             },
        { CAUSE_FETCH_PAGE_FAULT,    Trap::NO_TRAP             },
        { CAUSE_LOAD_PAGE_FAULT,     Trap::NO_TRAP             },
        { CAUSE_STORE_PAGE_FAULT,    Trap::NO_TRAP             },
    };

    auto it = from_riscv_conv.find( id);
    if ( it == from_riscv_conv.end())
        value = Trap::NO_TRAP;
    value = it->second;
}
