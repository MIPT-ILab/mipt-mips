 /* mips_instr.h - instruction parser for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2014-2017 MIPT-MIPS
 */

/** Edited by Ladin Oleg. */

#ifndef MIPS_INSTR_H
#define MIPS_INSTR_H

#include "mips_register/mips_register.h"
#include "mips_version.h"

#include <func_sim/instr.h>
#include <infra/endian.h>
#include <infra/exception.h>
#include <infra/string_view.h>
#include <infra/types.h>
#include <kryucow_string.h>

#include <sstream>

template<typename I>
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
class BaseMIPSInstr : public Instr<MIPSRegister, R>
{
    private:
        KryuCowString disasm = {};

        void init( const MIPSTableEntry<BaseMIPSInstr>& entry, MIPSVersion version);
        std::string generate_disasm( const MIPSTableEntry<BaseMIPSInstr>& entry) const;

        using Execute = void (*)(BaseMIPSInstr*);
        Execute executor = unknown_mips_instruction;
    protected:
        BaseMIPSInstr( MIPSVersion version, uint32 bytes, Addr PC);
        BaseMIPSInstr( MIPSVersion version, std::string_view str_opcode, Addr PC);
    public:
        static const constexpr Endian endian = Endian::little;

        BaseMIPSInstr() = delete;

        const uint32 raw;

        bool is_same_bytes( uint32 bytes) const {
            return raw == bytes;
        }

        bool is_same( const BaseMIPSInstr& rhs) const {
            return this->PC == rhs.PC && is_same_bytes( rhs.raw);
        }

        bool is_same_checker( const BaseMIPSInstr& rhs) const {
            return is_same(rhs)
                && this->sequence_id == rhs.sequence_id
                && (this->dst.is_zero()  || this->v_dst == rhs.v_dst)
                && (this->dst2.is_zero() || this->v_dst2 == rhs.v_dst2);
        }

        std::string_view get_disasm() const { return static_cast<std::string_view>( disasm); }

        bool is_nop() const { return raw == 0x0u; }

        void load_value(RegisterUInt value);

        void execute();

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
