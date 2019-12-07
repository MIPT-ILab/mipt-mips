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
        BaseMIPSInstr( MIPSVersion version, Endian endian, uint32 bytes, Addr PC);
        BaseMIPSInstr( MIPSVersion version, std::string_view str_opcode, Endian endian, uint32 immediate, Addr PC );
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
                && (this->dst.is_zero()  || this->v_dst == rhs.v_dst)
                && (this->dst2.is_zero() || this->v_dst2 == rhs.v_dst2);
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
        const Endian endian;

        void init( const MIPSTableEntry<typename BaseInstruction<R, MIPSRegister>::MyDatapath>& entry, MIPSVersion version);
        void init_target();
        static DisasmCache& get_disasm_cache();
        
        void set_src( size_t index, MIPSRegister reg) {
            switch (index) {
            case 0: this->src1 = reg; break;
            case 1: this->src2 = reg; break;
            case 2: this->src3 = reg; break;
            default: assert(0);
            }
        }
};

#endif //MIPS_INSTR_H
