/*
 * rf.h - mips register file
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS
 */

#ifndef RF_H
#define RF_H

#include <func_sim/func_instr/func_instr.h>

class RF
{
        struct Reg {
            uint32 value;

            /* The register is called "valid" when it's value is updated with
             * result of calculation of the latest fetched writing instruction.
             * For instance, when there is
             *    add $t0, $s1, $s2
             * instruction decoded, the register $t0 is marked "not valid".
             * After writeback it becomes valid again, and till that moment no
             * instruction with WAW or RAW dependency from register $t0 can be
             * executed. The WAW dependency is just a workaround for #45 issue.
             */
            bool is_valid;
        } array[REG_NUM_MAX];

        Reg& get_entry( RegNum num) { return array[static_cast<size_t>( num)]; }
        const Reg& get_entry( RegNum num) const { return array[static_cast<size_t>( num)]; }

    public:
        inline void read_src1( FuncInstr& instr)
        {
           instr.set_v_src1( read(instr.get_src1_num()));
        }

        inline void read_src2( FuncInstr& instr)
        {
           instr.set_v_src2( read(instr.get_src2_num()));
        }

        inline void write_dst( const FuncInstr& instr)
        {
            RegNum reg_num = instr.get_dst_num();
            if ( REG_NUM_ZERO != reg_num)
                write( reg_num, instr.get_v_dst());
        }

        RF()
        {
            for ( size_t i = 0; i < REG_NUM_MAX; ++i)
            {
                array[i].is_valid = true;
                array[i].value = 0;
            }
        }

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

        uint32 read( RegNum num)
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
};

#endif
