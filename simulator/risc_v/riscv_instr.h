/* riscv_instr.h - instruction parser for risc_v
 * @author Alexandr Misevich
 * Copyright 2018 MIPT-MIPS
 */

#ifndef RISCV_INSTR_H
#define RISCV_INSTR_H

#include "riscv_register/riscv_register.h"

#include <func_sim/operation.h>
#include <infra/endian.h>
#include <infra/macro.h>
#include <infra/types.h>

template<typename I>
struct RISCVTableEntry;

template <typename T>
class RISCVInstr : public BaseInstruction<T, RISCVRegister>
{
    private:
        using MyDatapath = typename BaseInstruction<T, RISCVRegister>::MyDatapath;
        uint32 instr = NO_VAL32;
        void init( const RISCVTableEntry<MyDatapath>& entry);
        void init_target();

    public:
        static constexpr auto get_endian() { return std::endian::little; }

        RISCVInstr() = delete;
        RISCVInstr( uint32 bytes, Addr PC);
        RISCVInstr( std::string_view name, uint32 immediate, Addr PC);

        explicit RISCVInstr( uint32 bytes) : RISCVInstr( bytes, 0) { }
        RISCVInstr( std::string_view name, uint32 immediate) : RISCVInstr( name, immediate, 0)  { }

        bool is_same_bytes( uint32 bytes) const {
            return bytes == instr;
        }

        bool is_same( const RISCVInstr& rhs) const {
            return this->PC == rhs.PC && is_same_bytes( rhs.instr);
        }

        bool is_same_checker( const RISCVInstr& rhs) const {
            return is_same( rhs)
                && this->get_sequence_id() == rhs.get_sequence_id()
                && (this->get_dst( 0).is_zero() || this->get_v_dst( 0) == rhs.get_v_dst( 0))
                && (this->get_dst( 1).is_zero() || this->get_v_dst( 1) == rhs.get_v_dst( 1));
        }

        constexpr bool is_nop() const { return instr == 0x0U; }

        std::string get_disasm() const;
        std::string string_dump() const;
        std::string bytes_dump() const;
};

template <typename T>
static inline std::ostream& operator<<( std::ostream& out, const RISCVInstr<T>& rhs)
{
    return out << rhs.string_dump();
}

#endif //RISCV_INSTR_H
