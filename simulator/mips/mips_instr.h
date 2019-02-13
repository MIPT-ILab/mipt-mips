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

#include <sstream>

enum OperationType : uint8
{
    OUT_ARITHM,
    OUT_R_ACCUM,
    OUT_R_CONDM,
    OUT_R_JUMP,
    OUT_SYSCALL,
    OUT_BREAK,
    OUT_R_SUBTR,
    OUT_BRANCH,
    OUT_TRAP,
    OUT_LOAD,
    OUT_LOADU,
    OUT_PARTIAL_LOAD,
    OUT_STORE,
    OUT_J_JUMP,
    OUT_J_SPECIAL,
    OUT_UNKNOWN
};

template<typename I>
struct MIPSTableEntry;

struct UnknownMIPSInstruction final : Exception
{
    explicit UnknownMIPSInstruction(const std::string& msg)
        : Exception("Unknown MIPS instruction is an unhandled trap", msg)
    { }
};

template<typename I>
void unknown_mips_instruction( I* i)
{
    throw UnknownMIPSInstruction( i->string_dump() + ' ' + i->bytes_dump());
}

template <typename Key, typename Value, size_t CAPACITY>
class LRUCache;

template<typename R>
class alignas(64) BaseMIPSInstr
{
    public:
        using Register = MIPSRegister;
        using RegisterUInt = R;
        using RegisterSInt = sign_t<RegisterUInt>;
    private:
        friend struct ALU;
        using Execute = void (*)(BaseMIPSInstr*);
        using DisasmCache = LRUCache<uint32, std::string, 8192>;

        OperationType operation = OUT_UNKNOWN;
        Trap trap = Trap::NO_TRAP;

        const uint32 raw;

        MIPSRegister src1 = MIPSRegister::zero();
        MIPSRegister src2 = MIPSRegister::zero();
        MIPSRegister dst  = MIPSRegister::zero();
        MIPSRegister dst2 = MIPSRegister::zero();

        uint32 v_imm = NO_VAL32;

        RegisterUInt v_src1 = NO_VAL<RegisterUInt>;
        RegisterUInt v_src2 = NO_VAL<RegisterUInt>;
        RegisterUInt v_dst  = NO_VAL<RegisterUInt>;
        RegisterUInt v_dst2 = NO_VAL<RegisterUInt>;
        RegisterUInt mask   = all_ones<RegisterUInt>();

        Addr mem_addr = NO_VAL32;
        uint32 mem_size = NO_VAL32;

        // convert this to bitset
        bool complete   = false;
        bool _is_jump_taken = false; // actual result
        bool memory_complete = false;

        Addr new_PC = NO_VAL32;

        const Addr PC = NO_VAL32;

        uint64 sequence_id = NO_VAL64;

        Execute executor = unknown_mips_instruction;

        void init( const MIPSTableEntry<BaseMIPSInstr>& entry, MIPSVersion version);
        std::string generate_disasm( const MIPSTableEntry<BaseMIPSInstr>& entry) const;

        static DisasmCache& get_disasm_cache();
    public:
        BaseMIPSInstr( MIPSVersion version, uint32 bytes, Addr PC);
        BaseMIPSInstr( MIPSVersion version, std::string_view str_opcode, Addr PC);

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
        std::string get_disasm() const;

	/* Checks if instruction can change PC in unusual way. */	
	
	//target is known at ID stage and always taken
	bool is_direct_jump() const { return operation == OUT_J_JUMP; }

	//target is known at ID stage but if branch is taken or not is known only at EXE stage
	bool is_direct_branch() const { return operation == OUT_BRANCH; }

	// target is known only at EXE stage
	bool is_indirect_branch() const { return operation == OUT_R_JUMP; }

	bool is_jump() const { return this->is_direct_jump()     ||
				      this->is_direct_branch()   ||
				      this->is_indirect_branch(); }

        bool is_jump_taken() const { return  _is_jump_taken; }

        bool is_partial_load() const
        {
            return operation == OUT_PARTIAL_LOAD;
        }

        bool is_load() const { return operation == OUT_LOAD ||
                                       operation == OUT_LOADU ||
                                       is_partial_load(); }

        int8 get_accumulation_type() const
        {
            return (operation == OUT_R_ACCUM) ? 1 : (operation == OUT_R_SUBTR) ? -1 : 0;
        }
        Trap trap_type() const { return trap; }

        bool is_store() const { return operation == OUT_STORE; }
        bool is_nop() const { return raw == 0x0u; }
        bool is_halt() const { return trap_type() == Trap::HALT; }
        bool is_conditional_move() const { return operation == OUT_R_CONDM; }
        bool is_divmult() const { return get_dst_num().is_mips_lo() && get_dst2_num().is_mips_hi(); }
        bool is_explicit_trap() const { return operation == OUT_TRAP; }
        bool is_syscall() const { return operation == OUT_SYSCALL; }
        bool has_trap() const { return trap_type() != Trap::NO_TRAP; }

        void set_v_dst(RegisterUInt value); // for loads
        void set_v_imm( uint32 value) { v_imm = value; }
        void set_v_src( RegisterUInt value, uint8 index)
        {
            if ( index == 0)
                v_src1 = value;
            else
                v_src2 = value;
        }

        auto get_v_dst()  const { return v_dst; }
        auto get_v_dst2() const { return v_dst2; }
        auto get_mask()  const { return mask;  }
        auto get_mem_addr() const { return mem_addr; }
        auto get_mem_size() const { return mem_size; }
        auto get_new_PC() const { return new_PC; }
        auto get_PC() const { return PC; }
        auto get_v_src2() const { return v_src2; } // for stores

        void execute();
        void check_trap() { };

        void set_sequence_id( uint64 id) { sequence_id = id; }
        auto get_sequence_id() const { return sequence_id; }

        std::ostream& dump( std::ostream& out) const;
        std::string string_dump() const;
        std::string bytes_dump() const;
};

template<typename RegisterUInt>
std::ostream& operator<<( std::ostream& out, const BaseMIPSInstr<RegisterUInt>& instr)
{
    return instr.dump( out);
}

class MIPS32Instr : public BaseMIPSInstr<uint32>
{
public:
    explicit MIPS32Instr( uint32 bytes, Addr pc = 0x0) : BaseMIPSInstr<uint32>( MIPSVersion::v32, bytes, pc) { }
    explicit MIPS32Instr( std::string_view str_opcode, Addr pc = 0x0) : BaseMIPSInstr<uint32>( MIPSVersion::v32, str_opcode, pc) { }
};

class MIPS64Instr : public BaseMIPSInstr<uint64>
{
public:
    explicit MIPS64Instr( uint32 bytes, Addr pc = 0x0) : BaseMIPSInstr<uint64>( MIPSVersion::v64, bytes, pc) { }
    explicit MIPS64Instr( std::string_view str_opcode, Addr pc = 0x0) : BaseMIPSInstr<uint64>( MIPSVersion::v64, str_opcode, pc) { }
};

#endif //MIPS_INSTR_H
