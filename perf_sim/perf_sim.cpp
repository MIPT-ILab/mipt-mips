/*
 * perf_sim - mips simple performance simulator
 * @author Grigoriy Chirkov grigoriy.chirkov@phystech.edu
 * Copyright 2016 MIPT-MIPS
 */


#include <iostream>
#include <perf_sim.h>
#include <ports.h>
#include <string.h>
#include <sstream>
#include <iomanip>


#define PORT_BW 1
#define PORT_FANOUT 1
#define PORT_LATENCY 1
#define INSTR_SIZE 4

PerfMIPS::PerfMIPS() : is_PC_valid( true), executed_instrs( 0), cycle( 0)
{
    fetch_mod = new FetchModule( this);
    dec_mod = new DecodeModule( this);
    exec_mod = new ExecuteModule( this);
    mem_mod = new MemoryModule( this);
    wb_mod = new WritebackModule( this);
    Port<FuncInstr>::init();
    Port<uint32>::init();
    Port<bool>::init();
};


FetchModule::FetchModule( PerfMIPS* machine) :
Module( machine, "DEC_2_FETCH_STALL", NULL), instr_bytes( 0), instr_PC( 0)
{
    wp_fetch_2_dec = new WritePort<uint32>( string( "FETCH_2_DEC"),
                                            PORT_BW, PORT_FANOUT );

}

DecodeModule::DecodeModule( PerfMIPS* machine) :
Module( machine, "EXEC_2_DEC_STALL", "DEC_2_FETCH_STALL"),
cur_instr( 0), instr_bytes( 0), is_sent( true), instr_PC( 0)
{
    rp_fetch_2_dec = new ReadPort<uint32>( string( "FETCH_2_DEC"),
                                           PORT_LATENCY );
    wp_dec_2_exec = new WritePort<FuncInstr>( string( "DEC_2_EXEC"),
                                              PORT_BW, PORT_FANOUT );
}

ExecuteModule::ExecuteModule( PerfMIPS* machine) :
Module( machine, "MEM_2_EXEC_STALL", "EXEC_2_DEC_STALL"), cur_instr( 0)
{
    rp_dec_2_exec = new ReadPort<FuncInstr>( string( "DEC_2_EXEC"),
                                             PORT_LATENCY );
    wp_exec_2_mem = new WritePort<FuncInstr>( string( "EXEC_2_MEM"),
                                              PORT_BW, PORT_FANOUT );
}

MemoryModule::MemoryModule( PerfMIPS* machine) :
Module( machine, "WB_2_MEM_STALL", "MEM_2_EXEC_STALL"), cur_instr( 0)
{
    rp_exec_2_mem = new ReadPort<FuncInstr>( string( "EXEC_2_MEM"),
                                             PORT_LATENCY );
    wp_mem_2_wb = new WritePort<FuncInstr>( string( "MEM_2_WB"),
                                            PORT_BW, PORT_FANOUT );
}

WritebackModule::WritebackModule( PerfMIPS* machine) :
Module( machine, NULL, "WB_2_MEM_STALL"), cur_instr( 0)
{
    rp_mem_2_wb = new ReadPort<FuncInstr>( string( "MEM_2_WB"),
                                           PORT_LATENCY );
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

void FetchModule::clock()
{
    int cycle = machine->cycle;
    if ( check_stall())
    {
        return;
    }
    instr_PC = machine->PC;
    instr_bytes = machine->fetch();
    wp_fetch_2_dec->write( instr_bytes, cycle);
}

void DecodeModule::clock()
{
    bool is_stall_sent = false;
    int cycle = machine->cycle;
    if ( check_stall())
    {
        send_stall();
        is_stall_sent = true;
        return;
    }

    rp_fetch_2_dec->read( &instr_bytes, cycle);
    if ( is_sent)
        cur_instr = FuncInstr( instr_bytes, instr_PC);

    instr_PC = machine->PC;
    if ( !machine->check_PC())
    {
        if ( !is_stall_sent)
        {
            send_stall();
            is_stall_sent = true;
        }
    }

    if ( cur_instr.is_jump())
    {
        machine->invalidate_PC();
        if( !is_stall_sent)
        {
            is_stall_sent = true;
            send_stall();
        }
    }
    if ( check_regs())
    {
        machine->read_src( cur_instr);
        machine->rf->invalidate( cur_instr.get_dst_num());
        wp_dec_2_exec->write( cur_instr, cycle);
        machine->PC += INSTR_SIZE;
        is_sent = true;
    }
    else
    {
        if ( !is_stall_sent)
            send_stall();
        is_sent = false;
    }
}

inline bool DecodeModule::check_regs( RegNum reg1, RegNum reg2)
{
    return machine->rf->check( reg1) &&
           machine->rf->check( reg2) ;
}

inline bool DecodeModule::check_regs()
{
    return check_regs( cur_instr.get_src1_num(), cur_instr.get_src2_num());
}

void ExecuteModule::clock()
{
    int cycle = machine->cycle;
    if( check_stall())
    {
        send_stall();
        return;
    }
    if ( !rp_dec_2_exec->read( &cur_instr, cycle))
    {
        cur_instr = FuncInstr( 0);
        return;
    }
    cur_instr.execute();
    wp_exec_2_mem->write( cur_instr, cycle);
}

void MemoryModule::clock()
{
    int cycle = machine->cycle;
    if( check_stall())
    {
        send_stall();
        return;
    }
    if ( !rp_exec_2_mem->read( &cur_instr, cycle))
    {
        cur_instr = FuncInstr( 0);
        return;
    }
    machine->load_store( cur_instr);
    wp_mem_2_wb->write( cur_instr, cycle);
}

void WritebackModule::clock()
{
    int cycle = machine->cycle;
    if ( !rp_mem_2_wb->read( &cur_instr, cycle))
    {
        cur_instr = FuncInstr( 0);
        return;
    }
    machine->wb( cur_instr);
    if ( cur_instr.is_jump())
    {
        machine->PC = cur_instr.get_new_PC() - INSTR_SIZE;
        machine->validate_PC();
    }
    if ( !cur_instr.is_nop())
        machine->executed_instrs++;
}

string FetchModule::dump( string indent)
{
    ostringstream oss;
    oss << indent << "fetch:\t\t\t"
        << "0x" << hex << setfill( '0')
        << setw( 8) << instr_bytes << "\t["
        << "0x" << hex << setfill( '0')
        << setw( 8) << instr_PC << ']';
    return oss.str();
}

string DecodeModule::dump( string indent)
{
    ostringstream oss;
    oss << indent << "decode:\t\t"
        << cur_instr;
    return oss.str();
}

string ExecuteModule::dump( string indent)
{
    ostringstream oss;
    oss << indent << "execute:\t\t"
        << cur_instr;
    return oss.str();
}

string MemoryModule::dump( string indent)
{
    ostringstream oss;
    oss << indent << "memory:\t\t"
        << cur_instr;
    return oss.str();
}

string WritebackModule::dump( string indent)
{
    ostringstream oss;
    oss << indent << "writeback:\t\t"
        << cur_instr;
    return oss.str();
}

ostream& operator << ( ostream& stream, Module& module)
{
    stream << module.dump();
    return stream;
}

Module::Module( PerfMIPS* machine, const char* next_2_me_str,
                const char* me_2_prev_str) : machine( machine)
{
    assert( machine);
    if ( next_2_me_str)
        next_2_me_stall = new ReadPort<bool>( string( next_2_me_str),
                                              PORT_LATENCY );
    else
        next_2_me_stall = NULL;
    if ( me_2_prev_str)
        me_2_prev_stall = new WritePort<bool>( string( me_2_prev_str),
                                               PORT_BW, PORT_FANOUT );
    else
        me_2_prev_stall = NULL;
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
    assert( me_2_prev_stall);
    me_2_prev_stall->write( true, cycle);
}

Module::~Module()
{
    if ( next_2_me_stall)
        delete next_2_me_stall;
    if ( me_2_prev_stall)
        delete me_2_prev_stall;
}

void PerfMIPS::run( const std::string& tr, int instrs_to_run, bool silent)
{
    mem = new FuncMemory( tr.c_str());
    PC = mem->startPC();
    executed_instrs = 0;
    cycle = 0;
    while ( executed_instrs < instrs_to_run)
    {
        fetch_mod->clock();
        if ( !silent)
            cout << *fetch_mod << endl;
        dec_mod->clock();
        if ( !silent)
            cout << *dec_mod << endl;
        exec_mod->clock();
        if ( !silent)
            cout << *exec_mod << endl;
        mem_mod->clock();
        if ( !silent)
            cout << *mem_mod << endl;
        wb_mod->clock();
        if ( !silent)
            cout << *wb_mod << endl << endl;
        else
        {
            cout << wb_mod->silent_dump();
        }
        ++cycle;
    }
    if ( !silent)
        cout << "CPI = " << ( double) cycle/( double) instrs_to_run << endl;
    delete mem;
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

string WritebackModule::silent_dump( string indent)
{
    if( !cur_instr.is_nop())
        return cur_instr.Dump( indent) + "\n";
    else
        return "";
}

MIPS::~MIPS()
{
    delete rf;
}
