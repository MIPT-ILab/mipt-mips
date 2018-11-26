/* riscv_instr.h - instruction parser for risc_v
 * @author Alexandr Misevich
 * Copyright 2018 MIPT-MIPS
 */

#ifndef RISCV_INSTR_H
#define RISCV_INSTR_H

#include "risc_v.h"
#include "riscv_register/riscv_register.h"

// MIPT-MIPS modules
#include <func_sim/trap_types.h>
#include <infra/endian.h>
#include <infra/macro.h>
#include <infra/string_view.h>
#include <infra/types.h>

// Generic C++
#include <array>
#include <unordered_map>

template <typename T>
class RISCVInstr
{
    using RegisterUInt = T;
    private:
        uint32 instr = NO_VAL32;

        RISCVRegister src1 = RISCVRegister::zero;
        RISCVRegister src2 = RISCVRegister::zero;
        RISCVRegister dst = RISCVRegister::zero;

        RegisterUInt v_src1 = NO_VAL32;
        RegisterUInt v_src2 = NO_VAL32;
        RegisterUInt v_dst = NO_VAL32;

        Addr mem_addr = NO_VAL32;
        uint32 mem_size = NO_VAL32;

        Addr PC = NO_VAL32;
        Addr new_PC = NO_VAL32;

        uint64 sequence_id = NO_VAL64;

        std::string disasm = {};

    public:
        static const constexpr Endian endian = Endian::little;
        RISCVInstr() = delete;

        explicit
        RISCVInstr( uint32 bytes, Addr PC = 0) : instr( bytes), PC( PC), new_PC( PC + 4) {};

         bool is_same_bytes( uint32 bytes) const {
            return bytes == instr;
        }
        
        bool is_same( const RISCVInstr& rhs) const {
            return PC == rhs.PC && is_same_bytes( rhs.instr);
        }

        bool is_same_checker( const RISCVInstr& /* rhs */) const { return false; }

        RISCVRegister get_src_num( uint8 index) const { return ( index == 0) ? src1 : src2; }
        RISCVRegister get_dst_num()  const { return dst; }
        RISCVRegister get_dst2_num() const { return dst; }

        /* Checks if instruction can change PC in unusual way. */
        constexpr bool is_jump() const { return false; }

        constexpr bool is_jump_taken() const { return false; }

        constexpr bool is_load()  const { return false; }

        constexpr bool is_store() const { return false; }

        constexpr bool is_nop() const { return false; }

        constexpr bool is_halt() const { return false; }

        constexpr bool is_conditional_move() const { return false; }

        constexpr bool is_divmult() const { return false; }

        constexpr bool is_explicit_trap() const { return false; }
        
        constexpr bool is_special() const { return false; }

        constexpr bool has_trap() const { return false; }

        Trap trap_type() const { return Trap::NO_TRAP; }

        constexpr bool get_writes_dst() const { return false; }

        constexpr bool is_bubble() const { return false; }

        constexpr int8 get_accumulation_type() const { return 0; }

        constexpr bool is_partial_load() const { return false; }

        constexpr bool is_mthi() const { return false; }

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

        Addr get_mem_addr() const { return mem_addr; }
        uint32 get_mem_size() const { return mem_size; }
        Addr get_new_PC() const { return new_PC; }
        Addr get_PC() const { return PC; }

        void set_v_dst( const T& value) { v_dst = value; } // for loads
        auto get_v_src2() const { return v_src2; } // for stores

        RegisterUInt get_bypassing_data() const
        {
            return v_dst;
        }

        void execute() {};
        void check_trap() {};

        void set_sequence_id( uint64 id) { sequence_id = id; }
        auto get_sequence_id() const { return sequence_id; }
};

template <typename T>
static inline std::ostream& operator<<( std::ostream& out, const RISCVInstr<T>& /* rhs */)
{
    return out << "";
}

#endif //RISCV_INSTR_H
