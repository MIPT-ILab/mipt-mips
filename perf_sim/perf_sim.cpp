#include <iostream> 

#include "perf_sim.h"

static const uint32 PORT_LATENCY = 1;
static const uint32 PORT_FANOUT = 1;
static const uint32 PORT_BW = 1;

PerfMIPS::PerfMIPS(bool log) : Log(log)
{
    executed_instrs = 0;

    wp_fetch_2_decode = new WritePort<uint32>("FETCH_2_DECODE", PORT_BW, PORT_FANOUT);
    rp_fetch_2_decode = new ReadPort<uint32>("FETCH_2_DECODE", PORT_LATENCY);
    wp_decode_2_fetch_stall = new WritePort<bool>("DECODE_2_FETCH_STALL", PORT_BW, PORT_FANOUT);
    rp_decode_2_fetch_stall = new ReadPort<bool>("DECODE_2_FETCH_STALL", PORT_LATENCY);
    
    wp_decode_2_execute = new WritePort<FuncInstr>("DECODE_2_EXECUTE", PORT_BW, PORT_FANOUT);
    rp_decode_2_execute = new ReadPort<FuncInstr>("DECODE_2_EXECUTE", PORT_LATENCY);
    wp_execute_2_decode_stall = new WritePort<bool>("EXECUTE_2_DECODE_STALL", PORT_BW, PORT_FANOUT);
    rp_execute_2_decode_stall = new ReadPort<bool>("EXECUTE_2_DECODE_STALL", PORT_LATENCY);
    
    wp_execute_2_memory = new WritePort<FuncInstr>("EXECUTE_2_MEMORY", PORT_BW, PORT_FANOUT);
    rp_execute_2_memory = new ReadPort<FuncInstr>("EXECUTE_2_MEMORY", PORT_LATENCY);
    wp_memory_2_execute_stall = new WritePort<bool>("MEMORY_2_EXECUTE_STALL", PORT_BW, PORT_FANOUT);
    rp_memory_2_execute_stall = new ReadPort<bool>("MEMORY_2_EXECUTE_STALL", PORT_LATENCY);
    
    wp_memory_2_writeback = new WritePort<FuncInstr>("MEMORY_2_WRITEBACK", PORT_BW, PORT_FANOUT);
    rp_memory_2_writeback = new ReadPort<FuncInstr>("MEMORY_2_WRITEBACK", PORT_LATENCY);
    wp_writeback_2_memory_stall = new WritePort<bool>("WRITEBACK_2_MEMORY_STALL", PORT_BW, PORT_FANOUT);
    rp_writeback_2_memory_stall = new ReadPort<bool>("WRITEBACK_2_MEMORY_STALL", PORT_LATENCY);

    Port<uint32>::init();
    Port<FuncInstr>::init();
    Port<bool>::init();
    
    rf = new RF();
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

PerfMIPS::~PerfMIPS() {
    delete rf;

    delete wp_fetch_2_decode;
    delete rp_fetch_2_decode;
    delete wp_decode_2_fetch_stall;
    delete rp_decode_2_fetch_stall;

    delete wp_decode_2_execute;
    delete rp_decode_2_execute;
    delete wp_execute_2_decode_stall;
    delete rp_execute_2_decode_stall;

    delete wp_execute_2_memory;
    delete rp_execute_2_memory;
    delete wp_memory_2_execute_stall;
    delete rp_memory_2_execute_stall;

    delete wp_memory_2_writeback;
    delete rp_memory_2_writeback;
    delete wp_writeback_2_memory_stall;
    delete rp_writeback_2_memory_stall;
}
