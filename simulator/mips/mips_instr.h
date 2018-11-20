 /* mips_instr.h - instruction parser for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2014-2017 MIPT-MIPS
 */

/** Edited by Ladin Oleg. */

#ifndef MIPS_INSTR_H
#define MIPS_INSTR_H

#include "mips_register/mips_register.h"
#include "mips_version.h"

#include <func_sim/trap_types.h>
#include <infra/endian.h>
#include <infra/exception.h>
#include <infra/string_view.h>
#include <infra/types.h>
#include <kryucow_string.h>

#include <sstream>

enum OperationType : uint8
{
    OUT_R_ARITHM,
    OUT_R_ACCUM,
    OUT_R_CONDM,
    OUT_R_SHAMT,
    OUT_R_JUMP,
    OUT_R_SPECIAL,
    OUT_R_SUBTR,
    OUT_R_TRAP,
    OUT_I_ARITHM,
    OUT_I_BRANCH,
    OUT_RI_BRANCH_0,
    OUT_RI_TRAP,
    OUT_I_LOAD,
    OUT_I_LOADU,
    OUT_I_PARTIAL_LOAD,
    OUT_I_CONST,
    OUT_I_STORE,
    OUT_J_JUMP,
    OUT_J_SPECIAL,
    OUT_UNKNOWN
};

template<typename R>
struct MIPSTableEntry;

struct UnknownMIPSInstruction final : Exception
{
    explicit UnknownMIPSInstruction(const std::string& msg)
        : Exception("Unknown MIPS instruction is an unhandled trap", msg)
    { }
};

template<typename I>
void unknown_mips_instruction( I* instr)
{
    std::ostringstream oss;
    oss << *instr;
    throw UnknownMIPSInstruction( oss.str());
}

template<typename R>
class BaseMIPSInstr
{
    private:
        friend struct ALU;
        using RegisterUInt = R;
        using RegisterSInt = sign_t<RegisterUInt>;

        OperationType operation = OUT_UNKNOWN;
        Trap trap = Trap::NO_TRAP;

        const uint32 raw;

        MIPSRegister src1 = MIPSRegister::zero;
        MIPSRegister src2 = MIPSRegister::zero;
        MIPSRegister dst  = MIPSRegister::zero;
        MIPSRegister dst2 = MIPSRegister::zero;

        uint32 v_imm = NO_VAL32;

        RegisterUInt v_src1 = NO_VAL<RegisterUInt>;
        RegisterUInt v_src2 = NO_VAL<RegisterUInt>;
        RegisterUInt v_dst  = NO_VAL<RegisterUInt>;
        RegisterUInt v_dst2 = NO_VAL<RegisterUInt>;
        RegisterUInt mask   = all_ones<RegisterUInt>();

        uint16 shamt = NO_VAL16;
        Addr mem_addr = NO_VAL32;
        uint32 mem_size = NO_VAL32;

        // convert this to bitset
        bool complete   = false;
        bool _is_jump_taken = false; // actual result
        bool memory_complete = false;

        Addr new_PC = NO_VAL32;

        const Addr PC = NO_VAL32;

        uint64 sequence_id = NO_VAL64;

        KryuCowString disasm = {};

        void init( const MIPSTableEntry<RegisterUInt>& entry, MIPSVersion version);

        using Execute = void (*)(BaseMIPSInstr*);
        Execute execute_function = unknown_mips_instruction;
    protected:
        BaseMIPSInstr( MIPSVersion version, uint32 bytes, Addr PC);
        BaseMIPSInstr( MIPSVersion version, std::string_view str_opcode, Addr PC);
    public:
        static const constexpr Endian endian = Endian::little;

        BaseMIPSInstr() = delete;

        bool is_same_bytes( uint32 bytes) const {
            return raw == bytes;
        }

        bool is_same( const BaseMIPSInstr& rhs) const {
            return PC == rhs.PC && is_same_bytes( rhs.raw);
        }

        bool is_same_checker( const BaseMIPSInstr& rhs) const {
            return is_same(rhs)
                && sequence_id == rhs.sequence_id
                && (dst.is_zero()  || v_dst == rhs.v_dst)
                && (dst2.is_zero() || v_dst2 == rhs.v_dst2);
        }

        MIPSRegister get_src_num( uint8 index) const { return ( index == 0) ? src1 : src2; }
        MIPSRegister get_dst_num()  const { return dst;  }
        MIPSRegister get_dst2_num() const { return dst2; }
        std::string_view get_disasm() const { return static_cast<std::string_view>( disasm); }

        /* Checks if instruction can change PC in unusual way. */
        bool is_jump() const { return operation == OUT_J_JUMP      ||
                                      operation == OUT_RI_BRANCH_0 ||
                                      operation == OUT_R_JUMP      ||
                                      operation == OUT_I_BRANCH;}
        bool is_jump_taken() const { return  _is_jump_taken; }

        bool is_partial_load() const
        {
            return operation == OUT_I_PARTIAL_LOAD;
        }

        bool is_load() const { return operation == OUT_I_LOAD ||
                                       operation == OUT_I_LOADU ||
                                       is_partial_load(); }

        int8 get_accumulation_type() const
        {
            return (operation == OUT_R_ACCUM) ? 1 : (operation == OUT_R_SUBTR) ? -1 : 0;
        }

        bool is_store() const { return operation == OUT_I_STORE; }

        bool is_nop() const { return raw == 0x0u; }
        bool is_halt() const { return trap_type() == Trap::HALT; }

        bool is_conditional_move() const { return operation == OUT_R_CONDM; }

        bool is_divmult() const { return get_dst_num().is_mips_lo() && get_dst2_num().is_mips_hi(); }

        bool is_explicit_trap() const { return operation == OUT_R_TRAP ||
                                               operation == OUT_RI_TRAP; }

        bool is_special() const { return operation == OUT_R_SPECIAL; }

        bool has_trap() const { return trap_type() != Trap::NO_TRAP; }

        Trap trap_type() const { return trap; }

        bool is_bubble() const { return is_nop() && PC == 0; }

        void set_v_imm( uint32 value) { v_imm = value; }
        auto get_v_imm() { return v_imm; }

        void set_v_src( RegisterUInt value, uint8 index)
        {
            if ( index == 0)
                v_src1 = value;
            else if ( index == 1)
                v_src2 = value;
            else
                assert( false);
        }

        void set_new_PC( Addr value) { new_PC = value; }

        void set_shamt(uint16 val) { shamt = val; }

        auto get_v_dst()  const { return v_dst; }
        auto get_v_dst2() const { return v_dst2; }
        auto get_mask()  const { return mask;  }

        auto get_mem_addr() const { return mem_addr; }
        auto get_mem_size() const { return mem_size; }
        auto get_new_PC() const { return new_PC; }
        auto get_PC() const { return PC; }

        void set_v_dst(RegisterUInt value); // for loads
        auto get_v_src2() const { return v_src2; } // for stores

        void execute();
        void check_trap() { };

        void set_sequence_id( uint64 id) { sequence_id = id; }
        auto get_sequence_id() const { return sequence_id; }

        std::ostream& dump( std::ostream& out) const;
        std::string string_dump() const;
};

template<typename RegisterUInt>
std::ostream& operator<<( std::ostream& out, const BaseMIPSInstr<RegisterUInt>& instr)
{
    return instr.dump( out);
}

template<MIPSVersion V>
class MIPSInstr : public BaseMIPSInstr<MIPSRegisterUInt<V>>
{
    using Base = BaseMIPSInstr<MIPSRegisterUInt<V>>;
public:
    explicit MIPSInstr( uint32 bytes, Addr PC = 0)
        : Base( V, bytes, PC) { }
    explicit MIPSInstr( std::string_view str_opcode, Addr PC = 0)
        : Base( V, str_opcode, PC) { }
};


using MIPS32Instr = MIPSInstr<MIPSVersion::v32>;
using MIPS64Instr = MIPSInstr<MIPSVersion::v64>;

#endif //MIPS_INSTR_H
