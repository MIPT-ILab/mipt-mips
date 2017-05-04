/*
 * rf.h - mips register file
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS
 */

#ifndef RF_H
#define RF_H

#include "mips_instr.h"

class RF
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
        } array[REG_NUM_MAX];

        Reg& get_entry( RegNum num) { return array[static_cast<size_t>( num)]; }
        const Reg& get_entry( RegNum num) const { return array[static_cast<size_t>( num)]; }

        void invalidate( RegNum num)
        {
            if ( num != REG_NUM_ZERO)
                get_entry( num).is_valid = false;
        }

        void validate( RegNum num)
        {
            if ( num != REG_NUM_ZERO)
                get_entry( num).is_valid = true;
        }

        bool check( RegNum num) const
        {
            return get_entry( num).is_valid;
        }

        uint32 read( RegNum num) const
        {
            return get_entry( num).value;
        }

        void write( RegNum num, uint32 val)
        {
            if ( num == REG_NUM_ZERO)
                return;
            auto& entry = get_entry(num);
            assert( !entry.is_valid);
            entry.is_valid = true;
            entry.value = val;
        }
    public:
        RF() { }

        inline void read_sources( FuncInstr& instr)
        {
            instr.set_v_src1( read(instr.get_src1_num()));
            instr.set_v_src2( read(instr.get_src2_num()));
            invalidate( instr.get_dst_num());
        }

        inline bool check_sources( const FuncInstr& instr) const
        {
            return check( instr.get_src1_num())
                && check( instr.get_src2_num())
                && check( instr.get_dst_num());
        }

        inline void write_dst( const FuncInstr& instr)
        {
            RegNum reg_num = instr.get_dst_num();
            if ( REG_NUM_ZERO != reg_num)
                write( reg_num, instr.get_v_dst());
        }

        inline void cancel( const FuncInstr& instr)
        {
            validate( instr.get_dst_num());
        }
};

#endif

