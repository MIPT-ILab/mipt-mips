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
    uint32 read( RegNum num) const
    {
        assert( num != REG_NUM_MAX);
        return array[num].value;
    }
    bool check( RegNum num) const
    {
        assert( num != REG_NUM_MAX);
        return array[num].is_valid;
    }
    void invalidate( RegNum num)
    {
        assert( num != REG_NUM_MAX);
        if ( num == REG_NUM_ZERO)
            return;
        array[num].is_valid = false;
    }
    void write( RegNum num, uint32 val)
    {
        assert( num != REG_NUM_MAX);
        if ( REG_NUM_ZERO == num)
            return;
        array[num].value = val;
        assert( check( num) == false);
        array[num].is_valid = true;
    }


    inline void read_src1( FuncInstr& instr) const
    {
        RegNum reg_num = instr.get_src1_num();
        instr.set_v_src1( read( reg_num));
    }

    inline void read_src2( FuncInstr& instr) const
    {
        RegNum reg_num = instr.get_src2_num();
        instr.set_v_src2( read( reg_num));
    }

    inline void write_dst( const FuncInstr& instr)
    {
        RegNum reg_num = instr.get_dst_num();
        write( reg_num, instr.get_v_dst());
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
