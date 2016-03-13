/**
 * perf_sim.cpp
 * Implementation of scalar MIPS CPU simulator.
 * MIPT-MIPS Assignment 4.
 * Ladin Oleg.
 */

/* Generic C. */
#include <cstdlib>

/* Simulator modules. */
#include <perf_sim.h>

/* Ports constants. */
#define PORT_BW 1
#define PORT_FANOUT 1
#define PORT_LATENCY 1

PerfMIPS::PerfMIPS()
{
    /* Zero module storages. */
    uint32 fetch_data = 0;
    uint32 decode_data = 0;

    PC_is_valid = false; // PC unset
    decode_stall = true; // prevent generating "nop" on Decode
    /* Reset data dependency components. */
    source_stall_data = 0;
    source_stall = false;
    source_stall_end = false;

    rf = new RF; // create register file

    /* Create data ports. */
    wp_fetch_2_decode = new WritePort< uint32>( "FETCH_2_DECODE",
                                                PORT_BW,
                                                PORT_FANOUT);
    rp_fetch_2_decode = new ReadPort< uint32>( "FETCH_2_DECODE",
                                               PORT_LATENCY);
    wp_decode_2_execute = new WritePort< FuncInstr>( "DECODE_2_EXECUTE",
                                                  PORT_BW,
                                                  PORT_FANOUT);
    rp_decode_2_execute = new ReadPort< FuncInstr>( "DECODE_2_EXECUTE",
                                                 PORT_LATENCY);
    wp_execute_2_memory = new WritePort< FuncInstr>( "EXECUTE_2_MEMORY",
                                                  PORT_BW,
                                                  PORT_FANOUT);
    rp_execute_2_memory = new ReadPort< FuncInstr>( "EXECUTE_2_MEMORY",
                                                 PORT_LATENCY);
    wp_memory_2_writeback = new WritePort< FuncInstr>( "MEMORY_2_WRITEBACK",
                                                    PORT_BW,
                                                    PORT_FANOUT);
    rp_memory_2_writeback = new ReadPort< FuncInstr>( "MEMORY_2_WRITEBACK",
                                                   PORT_LATENCY);

    /* Create stall ports. */
    wp_decode_2_fetch_stall = new WritePort< bool>( "DECODE_2_FETCH_STALL",
                                                    PORT_BW,
                                                    PORT_FANOUT);
    rp_decode_2_fetch_stall = new ReadPort< bool>( "DECODE_2_FETCH_STALL",
                                                   PORT_LATENCY);
    wp_execute_2_decode_stall = new WritePort< bool>( "EXECUTE_2_DECODE_STALL",
                                                      PORT_BW,
                                                      PORT_FANOUT);
    rp_execute_2_decode_stall = new ReadPort< bool>( "EXECUTE_2_DECODE_STALL",
                                                     PORT_LATENCY);
    wp_memory_2_execute_stall = new WritePort< bool>( "MEMORY_2_EXECUTE_STALL",
                                                      PORT_BW,
                                                      PORT_FANOUT);
    rp_memory_2_execute_stall = new ReadPort< bool>( "MEMORY_2_EXECUTE_STALL",
                                                     PORT_LATENCY);
    wp_writeback_2_memory_stall = new WritePort< bool>( "WRITEBACK_2_MEMORY_STALL",
                                                        PORT_BW,
                                                        PORT_FANOUT);
    rp_writeback_2_memory_stall = new ReadPort< bool>( "WRITEBACK_2_MEMORY_STALL",
                                                       PORT_LATENCY);

    /* Initialize all types of ports. */
    Port< uint32>::init();
    Port< FuncInstr>::init();
    Port< bool>::init();
}

PerfMIPS::~PerfMIPS()
{
    delete rf;

    /** Delete all ports. */
    delete rp_fetch_2_decode;
    delete wp_fetch_2_decode;
    delete rp_decode_2_execute;
    delete wp_decode_2_execute;
    delete rp_execute_2_memory;
    delete wp_execute_2_memory;
    delete rp_memory_2_writeback;
    delete wp_memory_2_writeback;

    delete rp_decode_2_fetch_stall;
    delete wp_decode_2_fetch_stall;
    delete rp_execute_2_decode_stall;
    delete wp_execute_2_decode_stall;
    delete rp_memory_2_execute_stall;
    delete wp_memory_2_execute_stall;
    delete rp_writeback_2_memory_stall;
    delete wp_writeback_2_memory_stall;
}

void PerfMIPS::run( const string& tr, int instrs_to_run, bool is_silent)
{
    mem = new FuncMemory( tr.c_str()); // create functional memory
    PC = mem->startPC(); // get starting programm address
    PC_is_valid = true; // now PC is valid
    this->is_silent = is_silent; // set mode
    executed_instrs = 0;
    int cycle = 0;
    while ( executed_instrs < instrs_to_run) // main loop
    {
        clockFetch( cycle);
        clockDecode( cycle);
        clockExecute( cycle);
        clockMemory( cycle);
        clockWriteback( cycle);
        ++cycle;
        if ( !is_silent)
        {
            cout << "Executed instructions: " << executed_instrs << endl << endl;
        }
    }
}


void PerfMIPS::clockFetch( int cycle)
{
    bool is_stall = false;
    rp_decode_2_fetch_stall->read( &is_stall, cycle);
    if ( is_stall) // if stall
    {
        if ( !is_silent)
        {
            cout << "    fetch\tcycle " << cycle << ":  bubble" << endl;
        }
        return;
    }
    if ( !PC_is_valid) // if PC invalid just exit
    {
        if ( !is_silent)
        {
            cout << "    fetch\tcycle " << cycle << ":  bubble" << endl;
        }
        return;
    }
    /* Process data. */
    fetch_data = fetch();
    if ( isJump( fetch_data)) // if jump or branch - make stall
    {
        PC_is_valid = false;
    }
    wp_fetch_2_decode->write( fetch_data, cycle); // promote data
    if ( !is_silent)
    {
        cout << "    fetch\tcycle " << cycle << ":  0x" << hex << fetch_data
             << dec << endl;
    }
}

void PerfMIPS::clockDecode( int cycle)
{
    if ( decode_stall) // prevent generating "nop" on Decode
    {
        decode_stall = false;
        if ( PC_is_valid) // update PC
        {
            PC += 4;
        }
        if ( !is_silent)
        {
            cout << "    decode\tcycle " << cycle << ":  bubble" << endl;
        }
        return;
    }
    bool is_stall = false;
    rp_execute_2_decode_stall->read( &is_stall, cycle);
    if ( is_stall) // if stall
    {
        if ( !is_silent)
        {
            cout << "    decode\tcycle " << cycle << ":  bubble" << endl;
        }
        wp_decode_2_fetch_stall->write( true, cycle); // promote stall
        return;
    }
    bool read = true; // flag: will data read or not
    if ( source_stall) // if it's stall now, use additional storage
    {
        rp_fetch_2_decode->read( &source_stall_data, cycle);
    }
    if ( !source_stall_end) // normal read
    {
        read = rp_fetch_2_decode->read( &decode_data, cycle);
    } else // if it was stall, load data
    {
        decode_data = source_stall_data;
        source_stall_end = false;
    }
    /* Process data. */
    FuncInstr decode_data_( decode_data, PC);
    if ( !read && decode_data_.isJump()) // if it's control dependency
    {
        if ( PC_is_valid) // update PC
        {
            PC += 4;
        }
        if ( !is_silent)
        {
            cout << "    decode\tcycle " << cycle << ":  bubble" << endl;
        }
        return;
    }
    if ( !rf->check( decode_data_.get_src1_num()) || // check data dependencies
         !rf->check( decode_data_.get_src2_num()))
    {
        source_stall = true; // it's stall
        wp_decode_2_fetch_stall->write( true, cycle); // make stall
        if ( !is_silent)
        {
            cout << "    decode\tcycle " << cycle << ":  bubble" << endl;
        }
        return;
    }
    if ( source_stall) // if stall was ended, next time load data from storage
    {
        source_stall = false;
        source_stall_end = true;
    }
    read_src( decode_data_);
    rf->invalidate( decode_data_.get_dst_num());
    wp_decode_2_execute->write( decode_data_, cycle); // promote data
    PC += 4; // update PC
    if ( !is_silent)
    {
        cout << "    decode\tcycle " << cycle << ":  " << decode_data_ << endl;
    }
}

void PerfMIPS::clockExecute( int cycle)
{
    bool is_stall = false;
    rp_memory_2_execute_stall->read( &is_stall, cycle);
    if ( is_stall) // if stall
    {
        if ( !is_silent)
        {
            cout << "    execute\tcycle " << cycle << ":  bubble" << endl;
        }
        wp_execute_2_decode_stall->write( true, cycle); // promote stall
        return;
    }
    if ( !rp_decode_2_execute->read( &execute_data, cycle)) // nothing to read
    {
        if ( !is_silent)
        {
            cout << "    execute\tcycle " << cycle << ":  bubble" << endl;
        }
        return;
    }
    /* Process data. */
    execute_data.execute();
    wp_execute_2_memory->write( execute_data, cycle); // promote data
    if ( execute_data.isJump()) // update PC
    {
        PC = execute_data.get_new_PC();
        PC_is_valid = true;
    }
    if ( !is_silent)
    {
        cout << "    execute\tcycle " << cycle << ":  " << execute_data << endl;
    }
}

void PerfMIPS::clockMemory( int cycle)
{
    bool is_stall = false;
    rp_writeback_2_memory_stall->read( &is_stall, cycle);
    if ( is_stall) // if stall
    {
        if ( !is_silent)
        {
            cout << "    memory\tcycle " << cycle << ":  bubble" << endl;
        }
        wp_memory_2_execute_stall->write( true, cycle); // promote stall
        return;
    }
    if ( !rp_execute_2_memory->read( &memory_data, cycle)) // nothing to read
    {
        if ( !is_silent)
        {
            cout << "    memory\tcycle " << cycle << ":  bubble" << endl;
        }
        return;
    }
    /* Process data. */
    load_store( memory_data);
    wp_memory_2_writeback->write( memory_data, cycle); // promote data
    if ( !is_silent)
    {
        cout << "    memory\tcycle " << cycle << ":  " << memory_data << endl;
    }
}

void PerfMIPS::clockWriteback( int cycle)
{
    if ( !rp_memory_2_writeback->read( &writeback_data, cycle)) // nothing to read
    {
        if ( !is_silent)
        {
            cout << "    writeback\tcycle " << cycle << ":  bubble" << endl;
        }
        return;
    }
    /* Process data. */
    wb( writeback_data);
    executed_instrs++; // increase number of executed instructions
    if ( !is_silent)
    {
        cout << "    writeback\tcycle " << cycle << ":  " << writeback_data << endl;
    } else
    {
        cout << writeback_data << endl;
    }
}


bool PerfMIPS::isJump( uint32 data)
{
    union // storage for data
    {
        struct
        {
            unsigned funct : 6;
            unsigned other_bits : 20;
            unsigned opcode : 6;
        } bit; // by bits
        uint32 raw; // as integer
    } instr;
    instr.raw = data;
    switch( instr.bit.opcode)
    {
        case 0x2: // j
        case 0x4: // beq
        case 0x5: // bne
            return true;
        case 0x0: // R-type
            if ( instr.bit.funct == 0x8) // jr
            {
                return true;
            }
        default: // if not jump or branch
            return false;
    }
}
