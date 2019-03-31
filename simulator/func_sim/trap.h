#ifndef TRAP_H
#define TRAP_H

#include <infra/types.h>
#include <string>

#include <riscv-opcodes/riscv.opcode.gen.h>
#include <mips/mips_traps_def.h>

class Trap
{
    private:
        enum TrapType : uint16
        {
            TRAP_TYPE_NO_TRAP,

            TRAP_TYPE_MIPS_BEGIN,
            #define DEF_MIPS_TRAP(str_decr, const_name) TRAP_TYPE_##const_name,
            #include <mips/mips_traps_def.h>
            #undef DEF_MIPS_TRAP

            TRAP_TYPE_RISCV_BEGIN,
            #define DECLARE_CAUSE(str_descr, const_name) TRAP_TYPE_##const_name,
            #include <riscv-opcodes/riscv.opcode.gen.h>
            #undef DECLARE_CAUSE
        };

        TrapType value;

        explicit constexpr Trap( TrapType id) : value( id) { }

    public:

        static constexpr Trap from_mips_index( uint8 id) noexcept
        {
            return Trap( TrapType{ narrow_cast<uint8>( TRAP_TYPE_MIPS_BEGIN + 1 + id)});
        }

        static constexpr Trap from_riscv_index( uint8 id) noexcept
        {
            return Trap( TrapType{ narrow_cast<uint8>( TRAP_TYPE_RISCV_BEGIN + 1 + id)});
        }

        uint8 get_value() { return value; }

        std::string get_str() const {
            std::string str;

            switch ( this->value) {
                #define DEF_MIPS_TRAP(str_decr, const_name) \
                    case TRAP_TYPE_##const_name: str = str_descr;
                #include <mips/mips_traps_def.h>
                #undef DEF_MIPS_TRAP

                #define DECLARE_CAUSE(str_decr, const_name) \
                    case TRAP_TYPE_##const_name: str = str_descr;
                #include <riscv-opcodes/riscv.opcode.gen.h>
                #undef DECLARE_CAUSE

                default:
                    str = "unknown trap";
            }
            return str;
        }
};

#endif // TRAP_H
