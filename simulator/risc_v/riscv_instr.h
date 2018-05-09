/* riscv_instr.h - instruction parser for risc_v
 * @author Alexandr Misevich
 * Copyright 2018 MIPT-MIPS
 */

#ifndef RISCV_INSTR_H
#define RISCV_INSTR_H

// Generic C++
#include <cassert>
#include <array>
#include <unordered_map>

// MIPT-MIPS modules
#include <infra/types.h>
#include <infra/macro.h>
#include <infra/string/cow_string.h>

#include "riscv_register/riscv_register.h"
#include "risc_v.h"

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

#if 0
        std::string disasm = {};
#else
        CowString disasm = {};
#endif

    public:
        RISCVInstr() = delete;

        explicit
        RISCVInstr( uint32 bytes, Addr PC = 0) : instr( bytes), PC( PC),
                                                 new_PC( PC + 4)
                                                 {};

        bool is_same( const RISCVInstr& rhs) const {
            return PC == rhs.PC && instr == rhs.instr;
        }

        const std::string_view Dump() const { return static_cast<std::string_view>(disasm); }

        RISCVRegister get_src_num( uint8 index) const { return ( index == 0) ? src1 : src2; }
        RISCVRegister get_dst_num()  const { return dst; }

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

        constexpr bool get_writes_dst() const { return false; }

        constexpr bool is_bubble() const { return false; }

        constexpr int8 get_accumulation_type() const { return 0; }

        constexpr bool is_partial_load() const { return false; }

        constexpr bool is_partial_store() const { return false; }

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
};

template <typename T>
static inline std::ostream& operator<<( std::ostream& out, const RISCVInstr<T>& instr)
{
        return out << instr.Dump();
}

#endif //RISCV_INSTR_H
