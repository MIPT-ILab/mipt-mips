 /* mips_instr.h - instruction parser for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2014-2017 MIPT-MIPS
 */

/** Edited by Ladin Oleg. */

#ifndef MIPS_INSTR_H
#define MIPS_INSTR_H

#include "mips_register/mips_register.h"
#include "mips_version.h"

#include <func_sim/operation.h>
#include <func_sim/trap_types.h>
#include <infra/endian.h>
#include <infra/exception.h>
#include <infra/string_view.h>
#include <infra/types.h>

#include <sstream>

enum class MIPSImm : uint8
{
    NO, SHIFT,                // R type
    LOGIC, ARITH, TRAP, ADDR, // I type
    JUMP                      // J type
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
class BaseMIPSInstr : public Operation, public RFacade<MIPSRegister>
{
    public:
        using RegisterUInt = R;
        using RegisterSInt = sign_t<RegisterUInt>;
    private:
        friend struct ALU;
        using Execute = void (*)(BaseMIPSInstr*);
        using DisasmCache = LRUCache<uint32, std::string, 8192>;

        const uint32 raw;
        const bool raw_valid = false;

        MIPSImm imm_type = MIPSImm::NO;

        RegisterUInt v_src1 = NO_VAL<RegisterUInt>;
        RegisterUInt v_src2 = NO_VAL<RegisterUInt>;
        RegisterUInt v_dst  = NO_VAL<RegisterUInt>;
        RegisterUInt v_dst2 = NO_VAL<RegisterUInt>;
        RegisterUInt mask   = all_ones<RegisterUInt>();

        Execute executor = unknown_mips_instruction;

        void init( const MIPSTableEntry<BaseMIPSInstr>& entry, MIPSVersion version);
        std::string generate_disasm() const;

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

        std::string get_disasm() const;

        bool is_nop() const { return raw == 0x0u; }
        bool is_divmult() const { return get_dst_num().is_mips_lo() && get_dst2_num().is_mips_hi(); }

        void set_v_dst(RegisterUInt value); // for loads
        void set_v_src( RegisterUInt value, uint8 index)
        {
            if ( index == 0)
                v_src1 = value;
            else
                v_src2 = value;
        }

        auto get_v_dst()  const { return v_dst; }
        auto get_v_dst2() const { return v_dst2; }
        auto get_v_src2() const { return v_src2; } // for stores
        auto get_mask()  const { return mask;  }

        void execute();

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
