/*
 * perf_sim.cpp - the imlementation of mips pipeline simulator
 * @author Mikhail Lyubogoschev lyubogoshchev@phystech.edu
 * Copyright 2015 MIPT-MIPS 
 */

#include <iostream>
#include <perf_sim.h>


PerfMIPS::PerfMIPS():
    executed_instr(0),
    cycle(0),
    silent_mode(false),
	PC_is_valid(true),
    decode_data(0)
{
    rf = new RF();

    wp_fetch_2_decode = new WritePort<uint32>( "FETCH_2_DECODE", PORT_BW, PORT_FANOUT);
	rp_fetch_2_decode = new ReadPort<uint32>( "FETCH_2_DECODE", PORT_LATENCY);
    wp_decode_2_execute = new WritePort<FuncInstr>( "DECODE_2_EXECUTE", PORT_BW, PORT_FANOUT);
	rp_decode_2_execute = new ReadPort<FuncInstr>( "DECODE_2_EXECUTE", PORT_LATENCY);
    wp_execute_2_memory = new WritePort<FuncInstr>( "EXECUTE_2_MEMORY", PORT_BW, PORT_FANOUT);
	rp_execute_2_memory = new ReadPort<FuncInstr>( "EXECUTE_2_MEMORY", PORT_LATENCY);
    wp_memory_2_wback = new WritePort<FuncInstr>( "MEMORY_2_WBACK", PORT_BW, PORT_FANOUT);
	rp_memory_2_wback = new ReadPort<FuncInstr>( "MEMORY_2_WBACK", PORT_LATENCY);
   
    wp_wback_2_memory_stall = new WritePort<bool>( "WBACK_2_MEMORY_STALL", PORT_BW, PORT_FANOUT);
	rp_wback_2_memory_stall = new ReadPort<bool>( "WBACK_2_MEMORY_STALL", PORT_LATENCY);
    wp_memory_2_execute_stall = new WritePort<bool>( "MEMORY_2_EXECUTE_STALL", PORT_BW, PORT_FANOUT);
	rp_memory_2_execute_stall = new ReadPort<bool>( "MEMORY_2_EXECUTE_STALL", PORT_LATENCY);
    wp_execute_2_decode_stall = new WritePort<bool>( "EXECUTE_2_DECODE_STALL", PORT_BW, PORT_FANOUT);
	rp_execute_2_decode_stall = new ReadPort<bool>( "EXECUTE_2_DECODE_STALL", PORT_LATENCY);
    wp_decode_2_fetch_stall = new WritePort<bool>( "DECODE_2_FETCH_STALL", PORT_BW, PORT_FANOUT);
	rp_decode_2_fetch_stall = new ReadPort<bool>( "DECODE_2_FETCH_STALL", PORT_LATENCY);
	Port<uint32>::init();
	Port<FuncInstr>::init();
	Port<bool>::init();

}

void PerfMIPS::clock_fetch( uint64 cycle)
{
    bool is_stall = false;
    rp_decode_2_fetch_stall->read( &is_stall, cycle);
    if ( is_stall) 
    {
        if ( !silent_mode) cout << "fetch   cycle " << cycle <<":  " << "stall" << endl;
        return;
    }
    if ( PC_is_valid)
    {
        fetch_data = mem->read(PC);
        wp_fetch_2_decode->write( fetch_data, cycle); 
    }
    if ( !silent_mode) 
        cout << "fetch   cycle " << cycle <<":  " << hex << "0x" << fetch_data << dec << endl;
}


void PerfMIPS::clock_decode( uint64 cycle)
{
    bool is_stall = false;
    rp_execute_2_decode_stall->read( &is_stall, cycle);
    if ( is_stall)
    {
        if ( !silent_mode) cout << "decode  cycle " << cycle <<":  " << "stall" << endl;
        wp_decode_2_fetch_stall->write( true, cycle);
        return;
    }
    if ( !decode_failed) // decode_failed flag is set true, if the sources
    {                    // for the instruction aren't ready
        rp_fetch_2_decode->read( &decode_int, cycle); // In that case there is no need
        decode_data = FuncInstr( decode_int, PC);     // to read from port and call the constructor for FuncInstr
    }
    if ( !silent_mode) cout << "decode  cycle " << cycle <<":  " << decode_data << endl;
    if ( PC_is_valid)
    {   
        if ( !read_src( decode_data)) // If the sources aren't ready - we should 
        {                             // stall fetch and wait for sources
            rp_fetch_2_decode->read( &decode_int, cycle);
            decode_failed = true; 
            wp_decode_2_fetch_stall->write( true, cycle);
            return;
        }
        decode_failed = false; // After sources are ready - we should go on processing data
        rf->invalidate( decode_data.get_dst_num());
        PC_is_valid = decode_data.is_jump() ? false : true;
        if ( PC_is_valid)
        {
            PC += 4;
            wp_decode_2_execute->write( decode_data, cycle);
            wp_decode_2_fetch_stall->write( false, cycle);
        } else {
            rp_fetch_2_decode->read( &decode_int, cycle);
            wp_decode_2_execute->write( decode_data, cycle); 
        }
    } else 
        wp_decode_2_fetch_stall->write( true, cycle);
}

void PerfMIPS::clock_execute( uint64 cycle)
{
    bool is_stall = false;
    rp_memory_2_execute_stall->read(&is_stall, cycle);
    if ( is_stall)
    {
        if ( !silent_mode) cout << "execute cycle " << cycle <<":  " << "stall" << endl;
        wp_execute_2_decode_stall->write( true, cycle);
        return;
    }
    if ( !rp_decode_2_execute->read( &execute_data, cycle)) // We should return from the function
    {                                                       // in case there is nothing to proceed
        if ( !silent_mode) cout << "execute cycle " << cycle <<":  " << "stall - no read" << endl;
        return;
    }
    execute_data.execute();
    if ( !silent_mode) cout << "execute cycle " << cycle <<":  " << execute_data << endl;
    wp_execute_2_memory->write( execute_data, cycle);
    wp_execute_2_decode_stall->write( false, cycle);
}


void PerfMIPS::clock_memory( uint64 cycle)
{
    bool is_stall = false;
    rp_wback_2_memory_stall->read( &is_stall, cycle);
    if ( is_stall)
    {
        if ( !silent_mode) cout << "memory  cycle " << cycle <<":  " << "stall" << endl;
        wp_memory_2_execute_stall->write( true, cycle);
        return;
    }
    if ( !rp_execute_2_memory->read( &memory_data, cycle))
    {
        if ( !silent_mode) cout << "memory  cycle " << cycle <<":  " << "stall - no read" << endl;
        return;
    }
    load_store( memory_data);
    if ( !silent_mode) cout << "memory  cycle " << cycle <<":  " << memory_data << endl;
    wp_memory_2_wback->write( memory_data, cycle);
    wp_memory_2_execute_stall->write( false, cycle);
}

void PerfMIPS::clock_wback( uint64 cycle)
{
    FuncInstr instr;
    if ( !rp_memory_2_wback->read( &instr, cycle))
    {
        if ( !silent_mode) cout << "wback   cycle " << cycle <<":  " << "stall - no read" << endl;
        return;
    }
    wb( instr);
    if ( !PC_is_valid && instr.is_jump()) // In case current instruction is jump-type 
    {                                     // program updates the PC and deblocks the
        PC_is_valid = true;               // next instructions, as control hazard is solved
        this->PC = instr.get_new_PC() - 4;
    }
    executed_instr++;
    if ( !silent_mode) cout << "wback   cycle " << cycle <<":  ";
    if ( !silent_mode || instr.get_raw_bytes() != 0)    // There is no need to see NOPs in the output
        cout << instr << endl;
    wp_wback_2_memory_stall->write( false, cycle);
}

void PerfMIPS::run(const std::string& tr, uint32 instrs_to_run, bool is_silent)
{
    mem = new FuncMemory(tr.c_str());
    PC = mem->startPC();
    executed_instr = 0;
    silent_mode = is_silent;
    while ( executed_instr < instrs_to_run )
    {
        clock_fetch(cycle);
        clock_decode(cycle);
        clock_execute(cycle);
        clock_memory(cycle);
        clock_wback(cycle);
        ++cycle;
    }
}


PerfMIPS::~PerfMIPS()
{
    delete rf;

    delete rp_fetch_2_decode;
    delete wp_fetch_2_decode;
    delete rp_decode_2_execute;
    delete wp_decode_2_execute;
    delete rp_execute_2_memory;
    delete wp_execute_2_memory;
    delete rp_memory_2_wback;
    delete wp_memory_2_wback;
    
    delete rp_wback_2_memory_stall;
    delete wp_wback_2_memory_stall;
    delete rp_memory_2_execute_stall;
    delete wp_memory_2_execute_stall;
    delete rp_execute_2_decode_stall;
    delete wp_execute_2_decode_stall;
    delete rp_decode_2_fetch_stall;
    delete wp_decode_2_fetch_stall;


}
