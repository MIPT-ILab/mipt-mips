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
        uint32 array[REG_NUM_MAX];
    public:
        inline void read_src1( FuncInstr& instr) const
        {
           size_t reg_num = instr.get_src1_num();
           instr.set_v_src1( array[reg_num]);
        }

        inline void read_src2( FuncInstr& instr) const
        {
           size_t reg_num = instr.get_src2_num();
           instr.set_v_src2( array[reg_num]);
        }

        inline void write_dst( const FuncInstr& instr)
        {
            size_t reg_num = instr.get_dst_num();
            if ( REG_NUM_ZERO != reg_num)
                array[reg_num] = instr.get_v_dst();
        }

        inline void reset( RegNum reg)
        {
            array[reg] = 0;
        }
 
        RF()
        {
            for ( size_t i = 0; i < REG_NUM_MAX; ++i)
                reset((RegNum)i);
        }
};
          
#endif
 
