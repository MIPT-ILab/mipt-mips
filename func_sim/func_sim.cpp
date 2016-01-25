/*
 * func_sim.cpp - MIPS singe-cycle simulator
 */

#include "func_sim.h"
#include <cstdio>
#include <iostream>
#include <sstream>

MIPS::MIPS()
{
    rf = new RF();
}

MIPS::~MIPS()
{
    delete rf;
}

RF::RF() 
{
    for ( int i = 0; i < MAX_REG; ++i)
    {
        reset(static_cast<RegNum>(i));
    }
}

RF::~RF() {}

uint32 RF::read( RegNum index) const
{
    return array[index];
}

void RF::write( RegNum index, uint32 data)
{
    if ( index != 0) 
    {
        array[index] = data;
    }
}

void RF::reset( RegNum index)
{
    array[index] = 0;
}

void MIPS::wb( FuncInstr& instr)
{
    //0 in dst means that instruction destination register is $0 or instruction needs no destination register
    if ( instr.get_dst_num_index() != 0)
    {
        rf->write( static_cast<RegNum>( instr.get_dst_num_index()), instr.v_dst);
    }
    hi = instr.hi;
    lo = instr.lo;
}

void MIPS::read_src( FuncInstr& instr)
{
    instr.hi = hi;
    instr.lo = lo;
    instr.v_src1 = rf->read( static_cast<RegNum>( instr.get_src1_num_index()));
    instr.v_src2 = rf->read( static_cast<RegNum>( instr.get_src2_num_index()));
}

uint32 MIPS::fetch() const 
{
    return mem->read( PC);
}

void MIPS::updatePC( const FuncInstr& instr)
{
    PC = instr.new_PC;
}

void MIPS::load( FuncInstr& instr) 
{
    instr.v_dst = mem->read( instr.mem_addr);
    switch ( instr.get_isaNum())
    {
        case 40: //LB
        case 43: //LBU
            instr.v_dst = ( instr.v_dst >> ( 8 * ( 3 - instr.byte_num))) & 0xFF;
            break;
        case 41: //LH
        case 44: //LHU
            instr.v_dst = ( instr.v_dst >> ( 16 * ( 1 - instr.hw_num))) & 0xFFFF;
            break;
        default:
            break;
    }
}

void MIPS::store( const FuncInstr& instr) 
{
    uint32 data = mem->read( instr.mem_addr);
    switch ( instr.get_isaNum())
    {
        case 45: //SB
            data = ( data & ~( 0xFF << ( 3 - instr.byte_num))) | ( instr.v_dst << ( 3 - instr.byte_num));
            break;
        case 46: //SH
            data = ( data & ~( 0xFFFF << ( 1 - instr.hw_num))) | ( instr.v_dst << ( 1 - instr.hw_num));
            break;
        default:
            data = instr.v_dst;
            break;
    }
    mem->write( instr.mem_addr, data);
}

void MIPS::ld_st( FuncInstr& instr)
{
    ostringstream oss;
    switch( instr.get_isaNum())
    {
        case 40 ... 44:
            load( instr);
            oss << std::hex << "[Loaded value is 0x" << static_cast<int>( instr.v_dst) \
                << " from 0x" <<  static_cast<int>( instr.mem_addr) << "]" << std::dec << endl;
            break;
        case 45 ... 47:
            store( instr);
            break;
        default:
            break;
    }
    mem_dump = oss.str();
}

void MIPS::run( const std::string& name, unsigned int instr_to_run)
{
    mem = new FuncMemory( name.c_str(), 32, 10, 12);
    PC = mem->startPC();
    printf( "startPC is 0x%x\n", PC);

    for ( unsigned int i = 0; i < instr_to_run; ++i)
    {

        //Fetch
        uint32 instr_bytes = fetch();
        //Decode
        FuncInstr instr( instr_bytes, PC);
        read_src( instr);
        //Execute
        instr.execute();
        //Memory
        ld_st( instr);
        //WriteBack
        wb( instr);
        updatePC( instr);
        //Dump 
        std::cout << instr.MIPSdump() << endl << mem_dump;
    }

    delete mem;
}
