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

template <typename Key, typename Value, size_t CAPACITY>
class LRUCache;

template<typename R>
class BaseMIPSInstr : public BaseInstruction<R, MIPSRegister>
{
    private:
        using MyDatapath = typename BaseInstruction<R, MIPSRegister>::MyDatapath;
        using DisasmCache = LRUCache<uint32, std::string, 8192>;

        const uint32 raw;
        const bool raw_valid = false;

        MIPSImm imm_type = MIPSImm::NO;

        void init( const MIPSTableEntry<typename BaseInstruction<R, MIPSRegister>::MyDatapath>& entry, MIPSVersion version);
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
            return this->PC == rhs.PC && is_same_bytes( rhs.raw);
        }

        bool is_same_checker( const BaseMIPSInstr& rhs) const {
            return is_same(rhs)
                && this->sequence_id == rhs.sequence_id
                && (this->dst.is_zero()  || this->v_dst == rhs.v_dst)
                && (this->dst2.is_zero() || this->v_dst2 == rhs.v_dst2);
        }

        std::string get_disasm() const;

        bool is_nop() const { return raw == 0x0u; }

        std::string string_dump() const;
        std::string bytes_dump() const;
        friend std::ostream& operator<<( std::ostream& out, const BaseMIPSInstr& instr)
        {
            return instr.dump_content( out, instr.get_disasm());
        }
};

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
