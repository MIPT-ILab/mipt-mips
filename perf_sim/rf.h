/*
 * rf.h - mips register file
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS 
 */

#ifndef RF_H
#define RF_H

#include <func_instr.h>

class RF
{
        struct Reg {
            uint32 value;
            bool is_valid;
        } array[REG_NUM_MAX];

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
                array[(size_t)num].is_valid = false;
        }

        bool check( RegNum num) const
        {
            return array[(size_t)num].is_valid;
        }

        uint32 read( RegNum num)
        {
            return array[(size_t)num].value;
        }

        void write( RegNum num, uint32 val)
        {
            if ( num == REG_NUM_ZERO)
                return;
            assert( array[(size_t)num].is_valid == false);
            array[(size_t)num].is_valid = true;
            array[(size_t)num].value = val;
        }
};
          
#endif
 
