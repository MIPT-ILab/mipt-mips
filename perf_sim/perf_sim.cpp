# include <iostream>

# include <perf_sim.h>

# define PORT_BW      1
# define PORT_FANOUT  1
# define PORT_LATENCY 1

//==============================================================================
// RerfMipt Methods ============================================================
//==============================================================================
PerfMIPS :: PerfMIPS ()
{
    //Init PC valid bit
    PC_valid = true;

    //Init Register File
    rf = new RF();

    //Init Data Ports
    rp_fetch_2_decode = new ReadPort<uint32> ("FETCH_2_DECODE", PORT_LATENCY);
    wp_fetch_2_decode = new WritePort<uint32> ("FETCH_2_DECODE", PORT_BW, PORT_FANOUT);
    rp_decode_2_execute = new ReadPort<FuncInstr> ("DECODE_2_EXECUTE", PORT_LATENCY);
    wp_decode_2_execute = new WritePort<FuncInstr> ("DECODE_2_EXECUTE", PORT_BW, PORT_FANOUT);
    rp_execute_2_memory = new ReadPort<FuncInstr> ("EXECUTE_2_MEMORY", PORT_LATENCY);
    wp_execute_2_memory = new WritePort<FuncInstr> ("EXECUTE_2_MEMORY", PORT_BW, PORT_FANOUT);
    rp_memory_2_writeback = new ReadPort<FuncInstr> ("MEMORY_2_WRITEBACK", PORT_LATENCY);
    wp_memory_2_writeback = new WritePort<FuncInstr> ("MEMORY_2_WRITEBACK", PORT_BW, PORT_FANOUT);

    rp_fetch_2_decode->init ();
    wp_fetch_2_decode->init ();
    rp_decode_2_execute->init ();
    wp_decode_2_execute->init ();
    rp_execute_2_memory->init ();
    wp_execute_2_memory->init ();
    rp_memory_2_writeback->init ();
    wp_memory_2_writeback->init ();

    //Init Latency ports
    rp_decode_2_fetch_stall = new ReadPort<bool> ("DECODE_2_FETCH_STALL", PORT_LATENCY);
    wp_decode_2_fetch_stall = new WritePort<bool> ("DECODE_2_FETCH_STALL", PORT_BW, PORT_FANOUT);
    rp_execute_2_decode_stall = new ReadPort<bool> ("EXECUTE_2_DECODE_STALL", PORT_LATENCY);
    wp_execute_2_decode_stall = new WritePort<bool> ("EXECUTE_2_DECODE_STALL", PORT_BW, PORT_FANOUT);
    rp_memory_2_execute_stall = new ReadPort<bool> ("MEMORY_2_EXECUTE_STALL", PORT_LATENCY);
    wp_memory_2_execute_stall = new WritePort<bool> ("MEMORY_2_EXECUTE_STALL", PORT_BW, PORT_FANOUT);
    rp_writeback_2_memory_stall = new ReadPort<bool> ("WRITEBACK_2_MEMORY_STALL", PORT_LATENCY);
    wp_writeback_2_memory_stall = new WritePort<bool> ("WRITEBACK_2_MEMORY_STALL", PORT_BW, PORT_FANOUT);

    rp_decode_2_fetch_stall->init ();
    wp_decode_2_fetch_stall->init ();
    rp_execute_2_decode_stall->init ();
    wp_execute_2_decode_stall->init ();
    rp_memory_2_execute_stall->init ();
    wp_memory_2_execute_stall->init ();
    rp_writeback_2_memory_stall->init ();
    wp_writeback_2_memory_stall->init ();

    //Init functional parts
    fetch = new Fetch (this);
    decode = new Decode (this);
    execute = new Execute (this);
    memory = new Memory (this);
    writeback = new Writeback (this);

    executed_instrs = 0;
    silent = false;
}

PerfMIPS :: ~PerfMIPS ()
{
    delete rf;

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

    delete fetch;
    delete decode;
    delete execute;
    delete memory;
    delete writeback;
}

void PerfMIPS :: clock_fetch (int cycle)
{
    bool is_stall;

    dumpString ("fetch ===========================");

    rp_decode_2_fetch_stall->read( &is_stall, cycle);
    if (is_stall)
    {
        dumpString ("   fetch: decode is stall");
        dumpString ("end fetch =======================\n");

        return;
    }

    if (is_fetch_ok ())
    {
        if (cycle > 0 )
            PC += 4;

        fetch->run ();


        dumpString ("   fetch: fetch is ok ");
        //cheking on branch and jump
        if (fetch->is_jump_or_branch ())
            PC_valid = false;
        dump_fetch (cycle);
        wp_fetch_2_decode->write( fetch->getCurrentData (), cycle);
    }
    dumpString ("end fetch =======================\n");
}

void PerfMIPS :: clock_decode (int cycle)
{
    bool is_stall;

    dumpString ("decode ==========================");

    rp_execute_2_decode_stall->read (&is_stall, cycle);
    if (is_stall)
    {
        wp_decode_2_fetch_stall->write (true, cycle);

        dumpString ("   decode : execute is stall");
        dumpString ("end decode ======================\n");
        return;
    }

    uint32 module_data = 0ull;

    bool check_data = rp_fetch_2_decode->read (&module_data, cycle);

    if (!decode->decode_queue.size () &&
        !check_data)
    {
        dumpString ("   decode : data is empty");
        dumpString ("end decode ======================\n");

        return;
    }
    if (check_data)
    {
        decode->run (module_data);
        decode->decode_queue.push_back (decode->getCurrentData ());
    }

    decode->setCurrentData (decode->decode_queue.front ());
    decode->decode_queue.pop_front ();
    //rp_fetch_2_decode->read (&module_data, cycle);

    dump_decode (cycle);

    if (is_decode_ok ())
    {
        dumpString ("   decode : decode is ok");
        wp_decode_2_execute->write (decode->getCurrentData (), cycle);

        rf->invalidate (decode->getCurrentData ().get_dst_num ());

        wp_decode_2_fetch_stall->write (false, cycle);
    }
    else
    {
        dumpString ("   decode : args isnt't prepeared");
        wp_decode_2_fetch_stall->write (true, cycle);

    }
    dumpString ("end decode ======================\n");
}

void PerfMIPS :: clock_execute (int cycle)
{
    bool is_stall;

    dumpString ("execute =========================");

    rp_memory_2_execute_stall->read (&is_stall, cycle);
    if (is_stall)
    {
        wp_execute_2_decode_stall->write (true, cycle);

        dumpString ("   execute : memory is stall");
        dumpString ("end execute =====================\n");

        return;
    }

    FuncInstr module_data (0ull);
    if (!rp_decode_2_execute->read (&module_data, cycle))
    {
        wp_execute_2_decode_stall->write (false, cycle);
        dumpString ("   execute : data is empty");
        dumpString ("end execute =====================\n");
        return;
    }
    execute->run (module_data);
    dump_execute (cycle);

    if (is_execute_ok ())
    {
        dumpString ("   execute : execute is ok");
        wp_execute_2_memory->write (execute->getCurrentData (), cycle);
        wp_execute_2_decode_stall->write (false, cycle);
    }
    else
    {
        dumpString ("   execute : execute isn't ok");
        wp_execute_2_decode_stall->write( true, cycle);
    }
    dumpString ("end execute =====================\n");
}

void PerfMIPS :: clock_memory (int cycle)
{
    bool is_stall;

    dumpString ("memory ==========================");

    rp_writeback_2_memory_stall->read (&is_stall, cycle);
    if (is_stall)
    {
        wp_memory_2_execute_stall->write (true, cycle);
        dumpString ("   memory : memory is stall");
        dumpString ("end memory ======================\n");
        return;
    }

    FuncInstr module_data (0ull);
    if (!rp_execute_2_memory->read (&module_data, cycle))
    {
        wp_memory_2_execute_stall->write (false, cycle);
        dumpString ("   memory : data is empty");
        dumpString ("end memory ======================\n");
        return;
    }

    memory->run (module_data);
    dump_memory (cycle);

    if (is_memory_ok ())
    {
        dumpString ("   memory : memory is ok");
        wp_memory_2_writeback->write (memory->getCurrentData (), cycle);
        wp_memory_2_execute_stall->write (false, cycle);
    }
    else
    {
        dumpString ("   memory : memory isn't ok");
        wp_memory_2_execute_stall->write (true, cycle);
    }
    dumpString ("end memory ======================\n");
}

void PerfMIPS :: clock_writeback (int cycle)
{
    bool is_stall;

    dumpString ("writeback =======================");

    FuncInstr module_data (0ull);
    if (!rp_memory_2_writeback->read (&module_data, cycle))
    {
        wp_writeback_2_memory_stall->write (false, cycle);
        dumpString ("   writeback : data is empty");
        dumpString ("end writeback ===================\n\n\n");
        return;
    }
    writeback->run (module_data);
    dump_writeback (cycle);

    if (is_writeback_ok ())
    {
        dumpString ("   writeback : writeback is ok");
        if (module_data.is_jump () || module_data.is_branch ())
        {
            PC_valid = true;
            PC = module_data.get_new_PC ();
        }
        rf->reset (module_data.get_src1_num ());
        rf->reset (module_data.get_src2_num ());
        wp_writeback_2_memory_stall->write (false, cycle);

        executed_instrs ++;

    }
    dumpString ("end writeback ===================\n\n\n");
}

bool PerfMIPS :: is_fetch_ok ()
{
    return PC_valid;
}

bool PerfMIPS :: is_decode_ok ()
{
    return decode->is_srcs_ready ();
}

bool PerfMIPS :: is_execute_ok ()
{
    return true;
}

bool PerfMIPS :: is_memory_ok ()
{
    return true;
}

bool PerfMIPS :: is_writeback_ok ()
{
    return true;
}

void PerfMIPS :: dump_fetch (int cycle)
{
    std::cout << "fetch      : cycle "
              << cycle  << " data "
              << std::hex
              << fetch->getCurrentData ()
              << std::endl;
}

void PerfMIPS :: dump_decode (int cycle)
{
    std::cout << "decode     : cycle "
              << cycle  << " data "
              << decode->getCurrentData ().Dump ()
              << std::endl;
}

void PerfMIPS :: dump_execute (int cycle)
{
    std::cout << "execute    : cycle "
              << cycle   << " data "
              << execute->getCurrentData ().Dump ()
              << std::endl;
}

void PerfMIPS :: dump_memory (int cycle)
{
    std::cout << "memory     : cycle "
              << cycle  << " data "
              << memory->getCurrentData ().Dump ()
              << std::endl;
}

void PerfMIPS :: dump_writeback (int cycle)
{
    std::cout << "writeback  : cycle "
              << cycle     << " data "
              << writeback->getCurrentData ().Dump ()
              << std::endl;
}

void PerfMIPS :: dumpString (std::string data)
{
    if (!silent)
        std::cout << data << std::endl;
}

void PerfMIPS :: run (const std::string& tr, int instrs_to_run, bool silent)
{
    mem = new FuncMemory(tr.c_str());
    PC = mem->startPC();

    this->silent = silent;
    this->executed_instrs = 0;
    int cycle = 0;
    int i = 0;
    while (executed_instrs < instrs_to_run)
    {
        clock_fetch(cycle);
        clock_decode(cycle);
        clock_execute(cycle);
        clock_memory(cycle);
        clock_writeback(cycle);
        ++cycle;
        i++;
    }
}

RF& PerfMIPS :: getRF () const
{
    return *rf;
}
FuncMemory& PerfMIPS :: getMem () const
{
    return *mem;
}
uint32& PerfMIPS :: getPC ()
{
    return PC;
}

//==============================================================================
// Fetch methods ===============================================================
//==============================================================================
Fetch :: Fetch (PerfMIPS *perf_mips) :
    _perf_mips (perf_mips),
    _current_data (0ull),
    _is_jump_or_branch (false),
    _is_undo (false)
{
}

Fetch :: ~Fetch ()
{
}

bool Fetch :: is_jump_or_branch ()
{
    return _is_jump_or_branch;
}

void Fetch :: run ()
{
    _current_data = _perf_mips->getMem ().read(_perf_mips->getPC ());

    FuncInstr current_instr (_current_data);

    if (current_instr.is_jump () || current_instr.is_branch ())
    {
        _is_jump_or_branch = true;
    }
    else
        _is_jump_or_branch = false;
}

//==============================================================================
// Decode methods ==============================================================
//==============================================================================
Decode :: Decode (PerfMIPS *perf_mips) :
    _perf_mips (perf_mips),
    _current_data (0ull)
{
}

Decode :: ~Decode ()
{
}

bool Decode :: is_srcs_ready ()
{
    return _perf_mips-> getRF ().check (_current_data.get_src1_num()) &&
           _perf_mips-> getRF ().check (_current_data.get_src2_num());

}

void Decode :: run (uint32 data)
{
    _current_data = FuncInstr (data);

    _perf_mips->getRF ().read_src1(_current_data);
    _perf_mips->getRF ().read_src2(_current_data);

    //_current_data = instr;
}

//==============================================================================
// Execute methods =============================================================
//==============================================================================
Execute :: Execute (PerfMIPS *perf_mips) :
    _perf_mips (perf_mips),
    _current_data (0ull)
{
}

Execute :: ~Execute ()
{
}

void Execute :: run (FuncInstr&  instr)
{
    instr.execute ();
    _current_data = instr;
}

//==============================================================================
// Memory methods ==============================================================
//==============================================================================
Memory :: Memory (PerfMIPS *perf_mips) :
    _perf_mips (perf_mips),
    _current_data (0ull)
{
}

Memory :: ~Memory ()
{
}

void Memory :: load (FuncInstr& instr) const
{
    instr.set_v_dst (_perf_mips->getMem ().read (instr.get_mem_addr (),
                                                 instr.get_mem_size ()));
}

void Memory :: store (const FuncInstr& instr)
{
    _perf_mips->getMem ().write (instr.get_v_src2   (),
                                 instr.get_mem_addr (),
                                 instr.get_mem_size ());
}

void Memory :: load_store (FuncInstr& instr)
{
    if (instr.is_load ())
        load (instr);
    else if (instr.is_store ())
        store (instr);
}

void Memory :: run (FuncInstr&  instr)
{
    load_store (instr);
    _current_data = instr;
}

//==============================================================================
// Writeback methods ===========================================================
//==============================================================================
Writeback :: Writeback (PerfMIPS *perf_mips) :
    _perf_mips (perf_mips),
    _current_data (0ull)
{
}

Writeback :: ~Writeback ()
{
}

void Writeback :: run (FuncInstr&  instr)
{
    _perf_mips->getRF ().write_dst (instr);
    _current_data = instr;
}
