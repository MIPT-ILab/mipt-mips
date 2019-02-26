/* riscv_instr.h - instruction parser for risc_v
 * @author Alexandr Misevich
 * Copyright 2018 MIPT-MIPS
 */

#ifndef RISCV_INSTR_H
#define RISCV_INSTR_H

#include "riscv_register/riscv_register.h"

#include <func_sim/operation.h>
#include <func_sim/trap_types.h>
#include <infra/endian.h>
#include <infra/macro.h>
#include <infra/string_view.h>
#include <infra/types.h>

template <typename T>
class RISCVInstr : public Operation, public RFacade<RISCVRegister>
{
    public:
        using RegisterUInt = T;
    private:
        uint32 instr = NO_VAL32;

        RegisterUInt v_src1 = NO_VAL32;
        RegisterUInt v_src2 = NO_VAL32;
        RegisterUInt v_dst = NO_VAL32;

        char imm_type = ' ';
        char imm_print_type = ' ';

        std::string generate_disasm() const;
    public:
        static const constexpr Endian endian = Endian::little;

        RISCVInstr() = delete;
        explicit RISCVInstr( uint32 bytes, Addr PC = 0);

         bool is_same_bytes( uint32 bytes) const {
            return bytes == instr;
        }
        
        bool is_same( const RISCVInstr& rhs) const {
            return PC == rhs.PC && is_same_bytes( rhs.instr);
        }

        bool is_same_checker( const RISCVInstr& /* rhs */) const { return false; }

        constexpr bool is_nop() const { return false; }

        constexpr bool is_divmult() const { return false; }

        void set_v_src( const T& value, uint8 index)
        {
            if ( index == 0)
                v_src1 = value;
            else
                v_src2 = value;
        }

        auto get_v_dst() const { return v_dst; }
        auto get_v_dst2() const { return v_dst; }
        auto get_mask() const { return v_dst; }

        void set_v_dst( const T& value) { v_dst = value; } // for loads
        auto get_v_src2() const { return v_src2; } // for stores

        void execute() {};

        std::string get_disasm() const;
};

template <typename T>
static inline std::ostream& operator<<( std::ostream& out, const RISCVInstr<T>& /* rhs */)
{
    return out << "";
}

#endif //RISCV_INSTR_H
