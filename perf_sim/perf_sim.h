#include <types.h>
#include <func_instr.h>
#include <func_memory.h>
#include <rf.h>
#include <ports.h>
# include <list>

class Fetch;
class Decode;
class Execute;
class Memory;
class Writeback;

//==============================================================================
// RefMIPS class ===============================================================
//==============================================================================
class PerfMIPS
{
    //Simulator objects
    RF* rf;
    uint32 PC;
    bool PC_valid;
    FuncMemory* mem;

    //data ports
    ReadPort<uint32>* rp_fetch_2_decode;
    WritePort<uint32>* wp_fetch_2_decode;
    ReadPort<FuncInstr>* rp_decode_2_execute;
    WritePort<FuncInstr>* wp_decode_2_execute;
    ReadPort<FuncInstr>* rp_execute_2_memory;
    WritePort<FuncInstr>* wp_execute_2_memory;
    ReadPort<FuncInstr>* rp_memory_2_writeback;
    WritePort<FuncInstr>* wp_memory_2_writeback;

    //latency ports
    ReadPort<bool>* rp_decode_2_fetch_stall;
    WritePort<bool>* wp_decode_2_fetch_stall;
    ReadPort<bool>* rp_execute_2_decode_stall;
    WritePort<bool>* wp_execute_2_decode_stall;
    ReadPort<bool>* rp_memory_2_execute_stall;
    WritePort<bool>* wp_memory_2_execute_stall;
    ReadPort<bool>* rp_writeback_2_memory_stall;
    WritePort<bool>* wp_writeback_2_memory_stall;

    //clock modules
    void clock_fetch (int cycle);
    void clock_decode (int cycle);
    void clock_execute (int cycle);
    void clock_memory (int cycle);
    void clock_writeback (int cycle);

    //dump methods
    void dump_fetch (int cycle);
    void dump_decode (int cycle);
    void dump_execute (int cycle);
    void dump_memory (int cycle);
    void dump_writeback (int cycle);

    //modules checking methods
    bool is_fetch_ok ();
    bool is_decode_ok ();
    bool is_execute_ok ();
    bool is_memory_ok ();
    bool is_writeback_ok ();

    void dumpString (std::string data);

    Fetch *fetch;
    Decode *decode;
    Execute *execute;
    Memory *memory;
    Writeback *writeback;

    int executed_instrs;
    bool silent;

public:
    PerfMIPS ();
    ~PerfMIPS ();

    void run (const std::string& tr, int instrs_to_run, bool silent);

    RF& getRF () const;
    FuncMemory& getMem () const;
    uint32& getPC ();
};

//==============================================================================
// Fetch class =================================================================
//==============================================================================
class Fetch
{
    PerfMIPS *_perf_mips;
    uint32 _current_data;

    bool _is_jump_or_branch;
    bool _is_branch;
    bool _is_undo;
public:
    Fetch (PerfMIPS *perf_mips);
    ~Fetch ();

    uint32 getCurrentData () {return _current_data;}
    void   setCurrentData (uint32 data) {_current_data = data;}

    bool getUndo () {return _is_undo;}
    void setUndo (bool undo) {_is_undo = undo;}

    bool is_jump_or_branch ();

    void run ();
};

//==============================================================================
// Decode class ================================================================
//==============================================================================
class Decode
{
    PerfMIPS *_perf_mips;

    FuncInstr _current_data;
public:
    Decode (PerfMIPS *perf_mips);
    ~Decode ();

    FuncInstr getCurrentData () {return _current_data;}
    void   setCurrentData (FuncInstr data) {_current_data = data;}

    bool is_srcs_ready ();

    void run (uint32 data);

    std::list<FuncInstr> decode_queue;
};

//==============================================================================
// Execute class ===============================================================
//==============================================================================
class Execute
{
    PerfMIPS *_perf_mips;

    FuncInstr _current_data;

public:
    Execute (PerfMIPS *perf_mips);
    ~Execute ();

    FuncInstr getCurrentData () {return _current_data;}
    void   setCurrentData (FuncInstr data) {_current_data = data;}

    void run (FuncInstr& instr);
};

//==============================================================================
// Memory class ================================================================
//==============================================================================
class Memory
{
    PerfMIPS *_perf_mips;

    FuncInstr _current_data;

    void load(FuncInstr& instr) const ;
    void store(const FuncInstr& instr);
    void load_store(FuncInstr& instr);

public:
    Memory (PerfMIPS *perf_mips);
    ~Memory ();

    FuncInstr& getCurrentData () {return _current_data;}
    void   setCurrentData (FuncInstr data) {_current_data = data;}

    void run (FuncInstr& instr);
};

//==============================================================================
// Writeback class =============================================================
//==============================================================================
class Writeback
{
    PerfMIPS *_perf_mips;

    FuncInstr _current_data;

public:
    Writeback (PerfMIPS *perf_mips);
    ~Writeback ();

    FuncInstr& getCurrentData () {return _current_data;}
    void   setCurrentData (FuncInstr data) {_current_data = data;}

    void run (FuncInstr& instr);
};
