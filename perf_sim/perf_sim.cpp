#include <iostream> 

#include "perf_sim.h"

static const uint32 PORT_LATENCY = 1;
static const uint32 PORT_FANOUT = 1;
static const uint32 PORT_BW = 1;

PerfMIPS::PerfMIPS(bool log) : Log(log)
{
    executed_instrs = 0;

    wp_fetch_2_decode = std::make_unique<WritePort<uint32>>("FETCH_2_DECODE", PORT_BW, PORT_FANOUT);
    rp_fetch_2_decode = std::make_unique<ReadPort<uint32>>("FETCH_2_DECODE", PORT_LATENCY);
    wp_decode_2_fetch_stall = std::make_unique<WritePort<bool>>("DECODE_2_FETCH_STALL", PORT_BW, PORT_FANOUT);
    rp_decode_2_fetch_stall = std::make_unique<ReadPort<bool>>("DECODE_2_FETCH_STALL", PORT_LATENCY);
    
    wp_decode_2_execute = std::make_unique<WritePort<FuncInstr>>("DECODE_2_EXECUTE", PORT_BW, PORT_FANOUT);
    rp_decode_2_execute = std::make_unique<ReadPort<FuncInstr>>("DECODE_2_EXECUTE", PORT_LATENCY);
    wp_execute_2_decode_stall = std::make_unique<WritePort<bool>>("EXECUTE_2_DECODE_STALL", PORT_BW, PORT_FANOUT);
    rp_execute_2_decode_stall = std::make_unique<ReadPort<bool>>("EXECUTE_2_DECODE_STALL", PORT_LATENCY);
    
    wp_execute_2_memory = std::make_unique<WritePort<FuncInstr>>("EXECUTE_2_MEMORY", PORT_BW, PORT_FANOUT);
    rp_execute_2_memory = std::make_unique<ReadPort<FuncInstr>>("EXECUTE_2_MEMORY", PORT_LATENCY);
    wp_memory_2_execute_stall = std::make_unique<WritePort<bool>>("MEMORY_2_EXECUTE_STALL", PORT_BW, PORT_FANOUT);
    rp_memory_2_execute_stall = std::make_unique<ReadPort<bool>>("MEMORY_2_EXECUTE_STALL", PORT_LATENCY);
    
    wp_memory_2_writeback = std::make_unique<WritePort<FuncInstr>>("MEMORY_2_WRITEBACK", PORT_BW, PORT_FANOUT);
    rp_memory_2_writeback = std::make_unique<ReadPort<FuncInstr>>("MEMORY_2_WRITEBACK", PORT_LATENCY);
    wp_writeback_2_memory_stall = std::make_unique<WritePort<bool>>("WRITEBACK_2_MEMORY_STALL", PORT_BW, PORT_FANOUT);
    rp_writeback_2_memory_stall = std::make_unique<ReadPort<bool>>("WRITEBACK_2_MEMORY_STALL", PORT_LATENCY);

    Port<uint32>::init();
    Port<FuncInstr>::init();
    Port<bool>::init();
}

void PerfMIPS::run( const std::string& tr, uint32 instrs_to_run)
{ 
    int cycle = 0;

    decode_next_time = false;

    mem = new FuncMemory( tr.c_str());

    PC = mem->startPC();
    PC_is_valid = true;

    while (executed_instrs < instrs_to_run)
    {
        clock_writeback( cycle);
        clock_decode( cycle);
        clock_fetch( cycle);
        clock_execute( cycle);
        clock_memory( cycle);
        ++cycle;

        if ( cycle - last_writeback_cycle >= 1000)
            serr << "Deadlock was detected. The process will be aborted.\n\n" << critical;
        sout << "Executed instructions: " << executed_instrs << std::endl << std::endl;
    }

    delete mem;
}

void PerfMIPS::clock_fetch( int cycle) {
    sout << "fetch   cycle " << std::dec << cycle << ":";

    bool is_stall = false;
    rp_decode_2_fetch_stall->read( &is_stall, cycle);
    if ( is_stall) 
    {
        sout << "bubble\n";
        return;
    }
   
    if (PC_is_valid)
    {
        uint32 module_data = mem->read(PC);
        wp_fetch_2_decode->write( module_data, cycle);
        
        sout << std::hex << "0x" << module_data << std::endl;
    }
    else
    {
        sout << "bubble\n";
    }
}

void PerfMIPS::clock_decode( int cycle) {
    sout << "decode  cycle " << std::dec << cycle << ":";
    
    bool is_stall = false;
    rp_execute_2_decode_stall->read( &is_stall, cycle);
    if ( is_stall) {
        wp_decode_2_fetch_stall->write( true, cycle);
   
        sout << "bubble\n";
        return;
    }

    bool is_anything_from_fetch = rp_fetch_2_decode->read( &decode_data, cycle);
    
    FuncInstr instr( decode_data, PC);
   
    if ( instr.isJump() && is_anything_from_fetch)
        PC_is_valid = false;

    if ( !is_anything_from_fetch && !decode_next_time)
    {
        sout << "bubble\n";
        return;
    }

    if ( rf.check( instr.get_src1_num()) && rf.check( instr.get_src2_num()))
    {
        rf.read_src1( instr);
        rf.read_src2( instr);
        rf.invalidate( instr.get_dst_num());
        wp_decode_2_execute->write( instr, cycle);
        
        decode_next_time = false;
        
        if (!instr.isJump())
            PC += 4;

        sout << instr << std::endl;
    }   
    else
    {
        wp_decode_2_fetch_stall->write( true, cycle);
        decode_next_time = true;
        sout << "bubble\n";
    }
}

void PerfMIPS::clock_execute( int cycle)
{
    std::ostringstream oss;
    sout << "execute cycle " << std::dec << cycle << ":";

    bool is_stall = false;
    rp_memory_2_execute_stall->read( &is_stall, cycle);
    if ( is_stall)
    {
        wp_execute_2_decode_stall->write( true, cycle);
        
        sout << "bubble\n";
        return;
    }

    FuncInstr instr;
    if ( !rp_decode_2_execute->read( &instr, cycle))
    {
        sout << "bubble\n";
        return;
    }

    instr.execute();
    wp_execute_2_memory->write( instr, cycle);

    sout << instr << std::endl;
}

void PerfMIPS::clock_memory( int cycle)
{
    sout << "memory  cycle " << std::dec << cycle << ":"; 

    bool is_stall = false;
    rp_writeback_2_memory_stall->read( &is_stall, cycle);
    if ( is_stall)
    {
        wp_memory_2_execute_stall->write( true, cycle);
        sout << "bubble\n";
        return;
    }

    FuncInstr instr;
    if ( !rp_execute_2_memory->read( &instr, cycle))
    {
        sout << "bubble\n";
        return;
    }

    load_store(instr);
    wp_memory_2_writeback->write( instr, cycle);

    sout << instr << std::endl;
}

void PerfMIPS::clock_writeback( int cycle)
{
    sout << "wb      cycle " << std::dec << cycle << ":"; 

    FuncInstr instr;
    if ( !rp_memory_2_writeback->read( &instr, cycle))
    {
        sout << "bubble\n";
        return;
    }

    if ( instr.isJump())
    {
        PC_is_valid = true;
        PC = instr.get_new_PC();
    }

    rf.write_dst( instr);

    sout << instr << std::endl;
    std::cout << instr << std::endl;

    ++executed_instrs;
    last_writeback_cycle = cycle;
}

PerfMIPS::~PerfMIPS() {

}
