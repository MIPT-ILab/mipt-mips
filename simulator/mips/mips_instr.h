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
#include <infra/endian.h>
#include <infra/exception.h>
#include <infra/types.h>

#include <sstream>
#include <string_view>

template<typename I>
struct MIPSTableEntry;

template <typename Key, typename Value, size_t CAPACITY, Key INVALID_KEY, Key DELETED_KEY>
class InstrCache;

template<typename R>
class BaseMIPSInstr : public BaseInstruction<R, MIPSRegister>
{
    public:
        BaseMIPSInstr( MIPSVersion version, std::endian endian, uint32 bytes, Addr PC);
        BaseMIPSInstr( MIPSVersion version, std::string_view str_opcode, std::endian endian, uint32 immediate, Addr PC );
        BaseMIPSInstr() = delete;

        auto get_endian() const { return endian; }

        bool is_same_bytes( uint32 bytes) const {
            return raw == bytes;
        }

        bool is_same( const BaseMIPSInstr& rhs) const {
            return this->PC == rhs.PC && is_same_bytes( rhs.raw);
        }

        bool is_same_checker( const BaseMIPSInstr& rhs) const {
            return is_same(rhs)
                && this->get_sequence_id() == rhs.get_sequence_id()
                && (this->get_dst( 0).is_zero() || this->get_v_dst( 0) == rhs.get_v_dst( 0))
                && (this->get_dst( 1).is_zero() || this->get_v_dst( 1) == rhs.get_v_dst( 1));
        }

        std::string get_disasm() const;

        bool is_nop() const { return raw == 0x0U; }

        std::string string_dump() const;
        std::string bytes_dump() const;
        friend std::ostream& operator<<( std::ostream& out, const BaseMIPSInstr& instr) {
            return instr.dump_content( out, instr.get_disasm());
        }
    private:
        using MyDatapath = typename BaseInstruction<R, MIPSRegister>::MyDatapath;
        using DisasmCache = InstrCache<uint32, std::string, 8192, all_ones<uint32>(), all_ones<uint32>() - 1>;

        const uint32 raw;
        const bool raw_valid = false;
        const std::endian endian;

        void init( const MIPSTableEntry<typename BaseInstruction<R, MIPSRegister>::MyDatapath>& entry, MIPSVersion version);
        void init_target();
        static DisasmCache& get_disasm_cache();
};

#endif //MIPS_INSTR_H
