/*
 * rf.h - mips register file
 * @author Grigoriy Chirkov grigoriy.chirkov@phystech.edu
 * Copyright 2016 MIPT-MIPS
 */

#ifndef RF_H
#define RF_H

#include <func_instr.h>

class RF
{
    //uint32 array[REG_NUM_MAX];
    struct Reg
    {
        uint32 value;
        bool is_valid;
        Reg() : value( 0ull), is_valid( true) {}
    } array[REG_NUM_MAX];
public:
    uint32 read( Reg_Num);
    bool check( Reg_Num num) const { return array[( size_t)num].is_valid; }
    void invalidate( Reg_Num num)
    {
        if ( num == REG_NUM_ZERO)
            return;
        array[( size_t) num].is_valid = false;
    }
    void write( Reg_Num num, uint32 val)
    {
        if ( REG_NUM_ZERO == num)
            return;
        array[( size_t)num].value = val;
        assert( check( num) == false);
        array[( size_t) num].is_valid = true;
    }
    
    
    inline void read_src1( FuncInstr& instr) const
    {
        size_t reg_num = instr.get_src1_num();
        instr.set_v_src1( read( reg_num));
    }
    
    inline void read_src2( FuncInstr& instr) const
    {
        size_t reg_num = instr.get_src2_num();
        instr.set_v_src2( read( reg_num));
    }
    
    inline void write_dst( const FuncInstr& instr)
    {
        size_t reg_num = instr.get_dst_num();
        write( reg num, instr.get_v_dst());
    }

    inline void reset( RegNum reg)
    {
        array[ reg].value = 0;
        array[ reg].is_valid = true;
    }
 
    /* RF()
    {
        for ( size_t i = 0; i < REG_NUM_MAX; ++i)
            reset((RegNum)i);
    } */
};
          
#endif
 
