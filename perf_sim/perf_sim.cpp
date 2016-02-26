/*
 * perf_sim - mips simple performance simulator
 * @author Grigoriy Chirkov grigoriy.chirkov@phystech.edu
 * Copyright 2016 MIPT-MIPS
 */


#include <iostream>
#include <perf_sim.h>
#include <ports.h>

#define PORT_BW 1
#define PORT_FANOUT 1
#define PORT_LATENCY 1

PerfMIPS::PerfMIPS() : is_PC_valid( true), executes_instrs( 0)
{
    dec_mod = new DecodeModule( this);
    fetch_mod = new fetch_mod( this);
    exec_mod = new exec_mod( this);
    mem_mod = new mem_mod( this);
    wb_mod = new wb_mod( this);
};


FetchModule::FetchModule( PerfMIPS* machine) :
Module( machine, "DEC_2_FETCH_STALL", NULL)
{
    wp_fetch_2_dec = new WritePort<uint32>( "FETCH_2_DEC", PORT_LATENCY);
}

DecodeModule::DecodeModule( PerfMIPS* machine) :
Module( machine, "EXEC_2_DEC_STALL", "DEC_2_FETCH_STALL")
{
    rp_fetch_2_dec = new ReadPort<uint32>( "FETCH_2_DEC", PORT_BW, PORT_FANOUT);
    wp_dec_2_exec = new WritePort<FuncInstr>( "DEC_2_EXEC", PORT_LATENCY);
}

ExecuteModule::ExecuteModule( PerfMIPS* machine) :
Module( machine, "MEM_2_EXEC_STALL", "EXEC_2_DEC_STALL")
{
    rp_dec_2_exec = new ReadPort<FuncInstr>( "DEC_2_EXEC", PORT_BW, PORT_FANOUT);
    wp_exec_2_mem = new WritePort<FuncInstr>( "EXEC_2_MEM", PORT_LATENCY);
}

MemoryModule::MemoryModule( PerfMIPS* machine) :
Module( machine, "WB_2_MEM_STALL", "MEM_2_EXEC_STALL")
{
    rp_exec_2_mem = new ReadPort<FuncInstr>( "EXEC_2_MEM", PORT_BW, PORT_FANOUT);
    wp_mem_2_wb = new WritePort<FuncInstr>( "MEM_2_WB", PORT_LATENCY);
}

WritebackModule::WritebackModule( PerfMIPS* machine) :
Module( machine, NULL, "WB_2_MEM_STALL")
{
    rp_mem_2_wb = new ReadPort<FuncInstr>( "MEM_2_WB", PORT_BW, PORT_FANOUT);
}

PerfMIPS::~PerfMIPS()
{
    delete dec_mod;
    delete fetch_mod;
    delete exec_mod;
    delete mem_mod;
    delete wb_mod;
}

FetchModule::~FetchModule()
{
    delete wp_fetch_2_dec;
}

DecodeModule::~DecodeModule()
{
    delete rp_fetch_2_dec;
    delete wp_dec_2_exec;
}

ExecuteModule::~ExecuteModule()
{
    delete rp_dec_2_exec;
    delete wp_exec_2_mem;
}

MemoryModule::~MemoryModule()
{
    delete rp_exec_2_mem;
    delete wp_mem_2_wb;
}

WritebackModule::~WritebackModule()
{
    delete rp_mem_2_wb;
}

void FetchModule::clock( int cycle)
{
    if ( check_stall( cycle))
        return;
    cur_instr = machine->fetch();
    
    wp_fetch_2_dec->write( cur_instr, cycle);
}

void DecodeModule::clock( int cycle)
{
    if ( check_stall( cycle))
    {
        send_stall( cycle);
        return;
    }
    
    uint32 instr_bytes = 0;
    if ( rp_fetch_2_dec->read( &instr_bytes, cycle) && machine->is_PC_valid())
        cur_instr = FuncInstr( instr_bytes, PC);

    if ( cur_instr.is_jump())
        machine->invalidate_PC();
    
    if( check_regs( instr.get_src1_num(), instr.get_src2_num()))
    {
        machine->read_src( cur_instr);
        machine->rf->invalidate( cur_instr.get_dst_num());
        machine->PC = cur_instr.get_new_PC();
        wp_dec_2_exec->write( cur_instr, cycle);
    } else
    {
        send_stall( cycle);
    }
}

bool DecodeModule::check_regs( RegNum reg1, RegNum reg2)
{
    return machine->rf->is_valid( reg1) &&
           machine->rf->is_valid( reg2) ;
}

void ExecuteModule::clock( int cycle)
{
    if( check_stall( cycle))
    {
        send_stall( cycle);
        return;
    }
    
    if ( !rp_dec_2_exec->read( &cur_instr, cycle))
        return;
    
    cur_instr.execute();
    
    wp_exec_2_mem->write( cur_instr, cycle);
}

void MemoryModule::clock( int cycle)
{
    if( check_stall( cycle))
    {
        send_stall( cycle);
        return;
    }

    if ( !rp_exec_2_mem->read( &cur_instr, cycle))
        return;
    
    machine->load_store( cur_instr);
    
    wp_mem_2_wb->write( instr, cycle);
}

void WritebackModule::clock( int cycle)
{
    if ( !rp_mem_2_wb->write( &cur_instr, cycle))
        return;
    
    machine->wb( instr);
    machine->PC = cur_instr.get_new_PC();
    machine->validate_PC();
    machine->executed_instrs++;
}

Module::Module( PerfMIPS* machine, const char* next_2_me_str,
                const char* me_2_prev_str) : machine( machine)
{
    assert( machine);
    if ( next_2_me)
        next_2_me_stall = new ReadPort<bool>( next_2_me_str, PORT_LATENCY);
    else
        next_2_me_stall = NULL;
    if ( me_2_prev)
        me_2_prev_stall = new WritePort<bool>( me_2_prev_str, PORT_BW, PORT_FANOUT);
    else
        prev_2_me_stall = NULL;
}

bool Module::check_stall( int cycle)
{
    assert( next_2_me_stall);
    bool is_stall = false;
    next_2_me_stall->read( &is_stall, cycle);
    return is_stall;
}

void Module::send_stall( int cycle)
{
    assert( prev_2_me_stall);
    me_2_prev_stall->write( true, cycle);
}

Module::~Module
{
    if ( next_2_me_stall)
        delete next_2_me_stall;
    if ( me_2_prev_stall)
        delete me_2_prev-stall;
}

MIPS::MIPS()
{
    rf = new RF();
}

void MIPS::run( const std::string& tr, uint32 instrs_to_run)
{
    mem = new FuncMemory( tr.c_str());
    PC = mem->startPC();
    for ( uint32 i = 0; i < instrs_to_run; ++i)
    {
        // fetch
        uint32 instr_bytes = fetch();
   
        // decode
        FuncInstr instr( instr_bytes, PC);

        // read sources
        read_src( instr);

        // execute
        instr.execute();

        // load/store
        load_store( instr);

        // writeback
        wb( instr);
        
        // PC update
        PC = instr.get_new_PC();
        
        // dump
        std::cout << instr << std::endl;
    }
    delete mem;
}

MIPS::~MIPS()
{
    delete rf;
}
