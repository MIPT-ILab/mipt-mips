/* trap_types.h - Trap types for MIPS and RISC-V
* @author Pavel Kryukov pavel.kryukov@phystech.edu, Vyacheslav Kompan
* Copyright 2014-2018 MIPT-MIPS
*/

#ifndef TRAP_TYPES_H
#define TRAP_TYPES_H

// MIPT-MIPS modules
#include <infra/types.h>
#include <riscv.opcode.gen.h>

#include <array>
#include <string_view>


class Trap {
    public:
        enum TrapType : uint8
        {
            //NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
            #define TRAP(name) name,
            #include <func_sim/trap_types.def>
            #undef TRAP
            MAX_TRAP_TYPE
        };

        explicit constexpr Trap( TrapType id) : value(id) { }

        Trap& operator=( TrapType id) { value = id; return *this; }

        bool operator==( Trap trap)     const { return value == trap.value; }
        bool operator==( TrapType trap) const { return value == trap; }
        bool operator!=( Trap trap)     const { return value != trap.value; }
        bool operator!=( TrapType trap) const { return value != trap; }

        std::size_t get_hash() const noexcept { return std::hash<std::uint8_t>{}(value); }


        void set_from_gdb_format(uint8 id);
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

namespace std
{
    template<> struct hash<Trap>
    {
        std::size_t operator()(const Trap& trap) const noexcept
        {
            return trap.get_hash();
        }
    };
} // namespace std


#endif // TRAP_TYPES_H
