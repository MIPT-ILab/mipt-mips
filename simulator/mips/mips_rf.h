/*
 * rf.h - mips register file
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS
 */

#ifndef RF_H
#define RF_H

#include <array>

#include "mips_instr.h"

class MIPSRF
{
        struct Reg {
            uint32 value = 0;

            /* The register is called "valid" when it's value is updated with
             * result of calculation of the latest fetched writing instruction.
             * For instance, when there is
             *    add $t0, $s1, $s2
             * instruction decoded, the register $t0 is marked "not valid".
             * After writeback it becomes valid again, and till that moment no
             * instruction with WAW or RAW dependency from register $t0 can be
             * executed. The WAW dependency is just a workaround for #45 issue.
             */
            bool is_valid = true;
        };
        std::array<Reg, REG_NUM_MAX> array = {};

        Reg& get_entry( RegNum num) { return array.at( static_cast<size_t>( num)); }
        const Reg& get_entry( RegNum num) const { return array.at( static_cast<size_t>( num)); }

        void invalidate( RegNum num)
        {
            if ( num == REG_NUM_HI_LO) {
                invalidate( REG_NUM_HI);
                invalidate( REG_NUM_LO);
            }
            else if ( num != REG_NUM_ZERO) {
                get_entry( num).is_valid = false;
            }
        }

        void validate( RegNum num)
        {
            if ( num == REG_NUM_HI_LO) {
                validate( REG_NUM_HI);
                validate( REG_NUM_LO);
            }
            else if ( num != REG_NUM_ZERO) {
                get_entry( num).is_valid = false;
            }
        }

        bool check( RegNum num) const
        {
            if ( num == REG_NUM_HI_LO) {
                return check( REG_NUM_HI) && check( REG_NUM_LO);
            }
            return get_entry( num).is_valid;
        }

        uint32 read( RegNum num) const
        {
            assert( num != REG_NUM_HI_LO);
            return get_entry( num).value;
        }

        void write( RegNum num, uint64 val)
        {
            if ( num == REG_NUM_ZERO)
                return;
            if ( num == REG_NUM_HI_LO) {
                write( REG_NUM_HI, val >> 32);
                write( REG_NUM_LO, val);
            }            

            auto& entry = get_entry(num);
            assert( !entry.is_valid);
            entry.is_valid = true;
            entry.value = val;
        }
    public:
        MIPSRF() = default;

        inline void read_sources( MIPSInstr* instr)
        {
            instr->set_v_src1( read(instr->get_src1_num()));
            instr->set_v_src2( read(instr->get_src2_num()));
            invalidate( instr->get_dst_num());
        }

        inline bool check_sources( const MIPSInstr& instr) const
        {
            return check( instr.get_src1_num())
                && check( instr.get_src2_num())
                && check( instr.get_dst_num());
        }

        inline void write_dst( const MIPSInstr& instr)
        {
            RegNum reg_num  = instr.get_dst_num();
            bool writes_dst = instr.get_writes_dst();
            if ( REG_NUM_ZERO != reg_num && writes_dst)
                write( reg_num, instr.get_v_dst());
            else
                write( reg_num, read(reg_num));
        }

        inline void cancel( const MIPSInstr& instr)
        {
            validate( instr.get_dst_num());
        }
};

#endif

