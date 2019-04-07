/* trap_types.h - Trap types for MIPS and RISC-V
* @author Pavel Kryukov pavel.kryukov@phystech.edu, Vyacheslav Kompan
* Copyright 2014-2018 MIPT-MIPS
*/

#ifndef TRAP_TYPES_H
#define TRAP_TYPES_H

// MIPT-MIPS modules
#include <infra/types.h>
#include "riscv.opcode.gen.h"

#include <array>
#include <string_view>

// Temporary enumeration
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

            #define TRAP(name) name,
            #include <func_sim/trap_types.def>
            #undef TRAP
            MAX_TRAP_TYPE
        };

        constexpr Trap(TrapType id) : value(id) { }

        bool operator==(Trap trap) const { return value == trap.value; }
        bool operator!=(Trap trap) const { return value != trap.value; }
        
        void set_from_gdb_format(GDB_TrapType id);
        uint8 to_gdb_format();
        
        void set_from_riscv_format(uint8 id);
        uint8 to_riscv_format();
        
        friend std::ostream& operator<<( std::ostream& out, const Trap& trap)
        {
            return out << TrapStrTable.at( trap.value);
        }

    private:
        TrapType value = Trap::NO_TRAP;
        
        static std::array<std::string_view, MAX_TRAP_TYPE> TrapStrTable;
};

#endif // TRAP_TYPES_H
