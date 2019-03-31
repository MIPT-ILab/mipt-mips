/* trap_types.h - Trap types for MIPS and RISC-V
* @author Pavel Kryukov pavel.kryukov@phystech.edu, Vyacheslav Kompan
* Copyright 2014-2018 MIPT-MIPS
*/

#ifndef TRAP_TYPES_H
#define TRAP_TYPES_H

// MIPT-MIPS modules
#include <infra/types.h>

#include <unordered_map>


// Now there are no such enumeration in mipt-mips
enum GDB_TrapType : uint8
{
    GDB_SIGNAL_0    = 0,
    GDB_SIGNAL_TRAP = 5,
    GDB_SIGNAL_SYS  = 12,
    GDB_SIGNAL_BUS  = 10,
    GDB_SIGNAL_ILL  = 4,
};


class Trap {
    public:
        enum TrapType : uint8
        {
            NO_TRAP,
            HALT,
            EXPLICIT_TRAP,
            BREAKPOINT,
            SYSCALL,
            INTEGER_OVERFLOW,
            UNSUPPORTED_SYSCALL,
            UNALIGNED_ADDRESS,
            UNKNOWN_INSTRUCTION
        };

        constexpr Trap(TrapType id) : value(id) { }

        bool operator==(Trap trap) const { return value == trap.value; }
        bool operator!=(Trap trap) const { return value != trap.value; }

        uint8 to_gdb_format()
        {
            static const std::unordered_map<TrapType, uint8> gdb_trap_conv =
            {
                { Trap::NO_TRAP,             GDB_SIGNAL_0    },  // New
                { Trap::EXPLICIT_TRAP,       GDB_SIGNAL_TRAP },
                { Trap::BREAKPOINT,          GDB_SIGNAL_TRAP },
                { Trap::UNSUPPORTED_SYSCALL, GDB_SIGNAL_SYS  },  // Not implemented in gdb_interface
                { Trap::UNALIGNED_ADDRESS,   GDB_SIGNAL_BUS  },
                { Trap::UNKNOWN_INSTRUCTION, GDB_SIGNAL_ILL  },  // Not implemented in gdb_interface
            };

            auto it = gdb_trap_conv.find( value);
            if ( it == gdb_trap_conv.end())
                return GDB_SIGNAL_0;
            return it->second;
        }

    private:
        TrapType value;
};

#endif //TRAP_TYPES_H
