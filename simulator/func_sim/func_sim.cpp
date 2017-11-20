/*
* This is an open source non-commercial project. Dear PVS-Studio, please check it.
* PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
*/
#include <iostream>

#include <mips/mips_memory.h>
#include <mips/mips_rf.h>

#include "func_sim.h"

MIPS::MIPS( bool log) : Log( log), rf( new RF) { }

MIPS::~MIPS()
{
    delete mem;
}

std::string MIPS::step()
{
    // fetch instruction
    FuncInstr instr = mem->fetch_instr( PC);
        
    // read sources
    rf->read_sources( &instr);

    // execute
    instr.execute();

    // load/store
    mem->load_store( &instr);

    // writeback
    rf->write_dst( instr);

    // trap check
    instr.check_trap();

    // PC update
    PC = instr.get_new_PC();

    // dump
    return instr.Dump();
}

void MIPS::init( const std::string& tr)
{
    assert( mem == nullptr);
    mem = new MIPSMemory( tr);
    PC = mem->startPC();
}

void MIPS::run( const std::string& tr, uint32 instrs_to_run)
{
    init( tr);
    for ( uint32 i = 0; i < instrs_to_run; ++i)
        sout << step() << std::endl;
}

