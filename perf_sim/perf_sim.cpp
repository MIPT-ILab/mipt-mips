/*
 * perf_sim.cpp - mips pipeline simulator
 * @author Ilya Belikov ilya.i.belikov@gmail.com
 * Copyright 2015 MIPT-MIPS
 */

#include <iostream>
#include <perf_sim.h>
#include <cstring>

void PerfMIPS::clock_fetch( int cycle){

    bool is_stall = false;
    rp_decode_2_fetch_stall->read( &is_stall, cycle);

    /* If stall then jump out here */
    if ( is_stall) {
        if( !silent) cout << "fetch\tcycle\t" << cycle << ":\t" << "bubble" << endl;
        return;
    }

    // If PC is valid then succ_fetch = true
    bool succ_fetch;
    uint32 raw_instr = fetch( &succ_fetch);

    if( succ_fetch){
        if( !silent) cout << "fetch\tcycle\t" << cycle << ":\t"
                    << std::hex << "0x" << raw_instr << std::dec << endl;

        wp_fetch_2_decode->write( raw_instr, cycle);

        if ( executed_instrs == num_of_instructions) invalidate_PC();
        else if( PC_is_valid) PC += 4;

    } else {
        if( !silent) cout << "fetch\tcycle\t" << cycle << ":\t" << "bubble" << endl;
    }
}

void PerfMIPS::clock_decode( int cycle){

    uint32 decode_data;

    if ( !rp_fetch_2_decode->read( &decode_data, cycle)){
        if( !silent) cout << "decode\tcycle\t" << cycle << ":\t" << "bubble" << endl;
        return;
    }

    FuncInstr* decoding_instr = new FuncInstr( decode_data, PC);

    /*  If there is jump instruction then decrease PC to 4 to take
        previous pipeline state after end of stall */
    if( decoding_instr->is_jump()) {
        PC -= 4;

        // Flush port
        rp_fetch_2_decode->read( &decode_data, cycle+1);

        invalidate_PC();
    }

    // Here we process data.
    bool succ_read_src = read_src( *decoding_instr);

    if (succ_read_src) {
        // Invalidating the destination register
        rf->invalidate( decoding_instr->get_dst_num());

        // Sending data to pipeline
        wp_decode_2_execute->write( *decoding_instr, cycle);

        // Output
        if(!silent) cout << "decode\tcycle\t" << cycle << ":\t" << *decoding_instr << endl;

    } else {
        /*  If we have stall the pipeline we should to backtrace PC to 8
            and read latest data from port to flush it */
        PC -= 8;

        // Flushing port
        rp_fetch_2_decode->read( &decode_data, cycle+1);

        // Generating stall
        wp_decode_2_fetch_stall->write( true, cycle);

        // Output
        if(!silent) cout << "decode\tcycle\t" << cycle << ":\t" << *decoding_instr << "\t [   stallen]" << endl;
    }
}

void PerfMIPS::clock_execute(int cycle){

    FuncInstr* executing_instr = new FuncInstr();
    /* If nothing cames from previous stage
    execute, memory and writeback modules have to jump out here */
    if( !rp_decode_2_execute->read( executing_instr, cycle)){
        if( !silent)
            cout << "execute\tcycle\t" << cycle << ":\t" << "bubble" << endl;
        return;
    }

    // Here we process data.
    executing_instr->execute();
    wp_execute_2_memory->write( *executing_instr, cycle);
    if( executing_instr->is_jump())update_PC( executing_instr->get_new_PC());

    // Output
    if( !silent) cout << "execute\tcycle\t" << cycle << ":\t" << *executing_instr << endl;
}

void PerfMIPS::clock_memory(int cycle){

    FuncInstr* memory_instr = new FuncInstr;
    /* If nothing cames from previous stage
    execute, memory and writeback modules have to jump out here */
    if ( !rp_execute_2_memory->read( memory_instr, cycle)){
        if(!silent) cout << "memory\tcycle\t" << cycle << ":\t" << "bubble" << endl;
        return;
    }

    // Here we process data.
    load_store( *memory_instr);
    wp_memory_2_writeback->write( *memory_instr, cycle);

    // Output
    if( !silent) cout << "memory\tcycle\t" << cycle << ":\t" << *memory_instr << endl;
}

void PerfMIPS::clock_writeback(int cycle){

    FuncInstr* writeback_instr = new FuncInstr;
    /* If nothing cames from previous stage
    execute, memory and writeback modules have to jump out here */
    if ( !rp_memory_2_writeback->read( writeback_instr, cycle)){
        if(!silent) cout << "wr_back\tcycle\t" << cycle << ":\t" << "bubble" << endl << endl;
        return;
    }

    // Here we process data.
    wb(*writeback_instr);

    // Output
    if(!silent) cout << "wr_back\tcycle\t" << cycle << ":\t" << *writeback_instr << endl << endl;
    else cout << *writeback_instr << endl;

    executed_instrs++;
}

void PerfMIPS::run(const std::string& tr, int instrs_to_run, bool silent_mode) {
    Port<bool>::init();
    Port<FuncInstr>::init();
    Port<uint32>::init();

    silent = silent_mode;
    mem = new FuncMemory( tr.c_str());
    PC = mem->startPC();
    PC_is_valid = true;
    executed_instrs = 0; // this variable is stored inside PerfMIPS class
    cycle = 0;
    num_of_instructions = instrs_to_run;
    while (executed_instrs < instrs_to_run) {
        clock_fetch( cycle);
        clock_decode( cycle);
        clock_execute( cycle);
        clock_memory( cycle);
        clock_writeback( cycle); // each instruction writeback increases executed_instrs variable
        ++cycle;
    }
}

PerfMIPS::PerfMIPS() {
    // Ports initialization
    wp_fetch_2_decode = new WritePort<uint32>("FETCH_2_DECODE_", PORT_BW, PORT_FANOUT);
    wp_decode_2_execute = new WritePort<FuncInstr>("DECODE_2_EXECUTE_", PORT_BW, PORT_FANOUT);
    wp_execute_2_memory = new WritePort<FuncInstr>("EXECUTE_2_MEMORY_", PORT_BW, PORT_FANOUT);
    wp_memory_2_writeback = new WritePort<FuncInstr>("MEMORY_2_WRITEBACK_", PORT_BW, PORT_FANOUT);

    rp_fetch_2_decode = new ReadPort<uint32>("FETCH_2_DECODE_", PORT_LATENCY);
    rp_decode_2_execute = new ReadPort<FuncInstr>("DECODE_2_EXECUTE_", PORT_LATENCY);
    rp_execute_2_memory = new ReadPort<FuncInstr>("EXECUTE_2_MEMORY_", PORT_LATENCY);
    rp_memory_2_writeback = new ReadPort<FuncInstr>("MEMORY_2_WRITEBACK_", PORT_LATENCY);

    // Stall ports initialization
    wp_decode_2_fetch_stall = new WritePort<bool>("DECODE_2_FETCH_STALL", PORT_BW, PORT_FANOUT);
    rp_decode_2_fetch_stall = new ReadPort<bool>("DECODE_2_FETCH_STALL", PORT_LATENCY);

    rf = new RF();
}

PerfMIPS::~PerfMIPS(){
    delete rp_fetch_2_decode;
    delete rp_decode_2_execute;
    delete rp_execute_2_memory;
    delete rp_memory_2_writeback;

    delete wp_fetch_2_decode;
    delete wp_decode_2_execute;
    delete wp_execute_2_memory;
    delete wp_memory_2_writeback;

    delete rf;
}
