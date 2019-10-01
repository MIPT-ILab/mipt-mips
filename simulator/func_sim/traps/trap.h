/* trap.h - Trap types for MIPS and RISC-V
* @author Pavel Kryukov pavel.kryukov@phystech.edu, Vyacheslav Kompan
* Copyright 2014-2018 MIPT-MIPS
*/

#ifndef TRAP_TYPES_H
#define TRAP_TYPES_H

#include <infra/exception.h>
#include <infra/types.h>

struct InvalidTrapConversion : Exception
{
    explicit InvalidTrapConversion( const std::string& msg)
        : Exception("Invalid trap conversion", msg)
    { }
};

class Trap {
    public:
        enum TrapType : uint8
        {
            #define TRAP(name, gdb, riscv, mips) name ,
            #include "trap.def"
            #undef TRAP
            INVALID_TRAP
        };

        explicit constexpr Trap( TrapType id) : value(id) { }

        Trap& operator=( TrapType id) { value = id; return *this; }

        bool operator==( Trap trap)     const { return value == trap.value; }
        bool operator==( TrapType trap) const { return value == trap; }
        bool operator!=( Trap trap)     const { return value != trap.value; }
        bool operator!=( TrapType trap) const { return value != trap; }

        std::size_t get_hash() const noexcept { return std::hash<std::uint8_t>{}(value); }

        static Trap from_gdb_format(uint8 id);
        static Trap from_riscv_format(uint8 id);
        static Trap from_mips_format(uint8 id);

        uint8 to_gdb_format() const;
        uint8 to_riscv_format() const;
        uint8 to_mips_format() const;

        friend std::ostream& operator<<( std::ostream& out, const Trap& trap);

    private:
        TrapType value = Trap::NO_TRAP;
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
