/* riscv_instr.h - instruction parser for risc_v
 * @author Alexandr Misevich
 * Copyright 2018 MIPT-MIPS
 */

#ifndef RISCV_INSTR_H
#define RISCV_INSTR_H

#include "risc_v.h"
#include "riscv_register/riscv_register.h"

#include <func_sim/instr.h>
#include <infra/endian.h>
#include <infra/types.h>

template <typename T>
class RISCVInstr : public Instr<RISCVRegister, T>
{
        using RegisterUInt = T;
    private:
        std::string disasm = {};
        const uint32 instr;

    public:
        static const constexpr Endian endian = Endian::little;
        RISCVInstr() = delete;

        explicit
        RISCVInstr( uint32 bytes, Addr PC = 0) : Instr<RISCVRegister, T>( PC), instr( bytes)
        {
            this->new_PC = PC + 4;
        };

        bool is_same_bytes( uint32 bytes) const {
            return bytes == instr;
        }
        
        bool is_same( const RISCVInstr& rhs) const {
            return this->PC == rhs.PC && is_same_bytes( rhs.instr);
        }

        bool is_same_checker( const RISCVInstr& /* rhs */) const { return false; }

        void load_value(RegisterUInt value);

        void execute() {};
};

template <typename T>
static inline std::ostream& operator<<( std::ostream& out, const RISCVInstr<T>& /* rhs */)
{
    return out << "";
}

#endif //RISCV_INSTR_H
