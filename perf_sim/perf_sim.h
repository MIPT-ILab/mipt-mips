/*
 * perf_sim - mips simple performance simulator
 * @author Grigoriy Chirkov grigoriy.chirkov@phystech.edu
 * Copyright 2016 MIPT-MIPS
 */

#ifndef PERF_SIM_H
#define PERF_SIM_H

#include <func_instr.h>
#include <func_memory.h>
#include <rf.h>
#include <ports.h>

class DecodeModule;
class FetchModule;
class ExecuteModule;
class MemoryModule;
class WritebackModule;
class Module;

class MIPS
{
protected:
    FuncMemory* mem;
    uint32 PC;
    RF* rf;
    uint32 fetch() const { return mem->read(PC); }
    void read_src( FuncInstr& instr) const
    {
        rf->read_src1( instr);
        rf->read_src2( instr);
    }

    void load( FuncInstr& instr) const
    {
        instr.set_v_dst( mem->read( instr.get_mem_addr(), instr.get_mem_size()));
    }

    void store( const FuncInstr& instr)
    {
        mem->write( instr.get_v_src2(), instr.get_mem_addr(), instr.get_mem_size());
    }

    void load_store( FuncInstr& instr)
    {
        if ( instr.is_load())
            load( instr);
        else if ( instr.is_store())
            store( instr);
    }

    void wb( const FuncInstr& instr)
    {
        rf->write_dst( instr);
    }

public:
    MIPS();
    void run( const std::string& tr, uint32 instrs_to_run);
    ~MIPS();
};


class PerfMIPS : public MIPS
{
private:
    DecodeModule* dec_mod;
    FetchModule* fetch_mod;
    ExecuteModule* exec_mod;
    MemoryModule* mem_mod;
    WritebackModule* wb_mod;

    bool is_PC_valid;
    bool check_PC() const { return is_PC_valid; };
    void invalidate_PC() { is_PC_valid = false; };
    void validate_PC() { is_PC_valid = true; };
    int executed_instrs;
    int cycle;

public:
    void run( const std::string& tr, int instr_to_run,
              bool silent = true);
    PerfMIPS();
    ~PerfMIPS();

    friend class FetchModule;
    friend class DecodeModule;
    friend class ExecuteModule;
    friend class MemoryModule;
    friend class WritebackModule;
    friend class Module;
};

class Module
{
    ReadPort<bool>* next_2_me_stall;
    WritePort<bool>* me_2_prev_stall;
    Module() {};
protected:
    bool check_stall( int cycle);
    void send_stall( int cycle);
    bool check_stall() { return check_stall( machine->cycle); }
    void send_stall()  { send_stall( machine->cycle); }
    PerfMIPS* machine;
public:
    Module( PerfMIPS* machine, const char* next_2_me_str,
                               const char* me_2_prev_str  );
    ~Module();
    virtual string dump( string indent = " ") { return indent; }
};

class DecodeModule : public Module
{
    ReadPort<uint32>*     rp_fetch_2_dec;
    WritePort<FuncInstr>* wp_dec_2_exec;
    FuncInstr cur_instr;
    uint32 instr_bytes;
    uint32 instr_PC;
    inline bool check_regs( RegNum reg1, RegNum reg2);
    inline bool check_regs();
    DecodeModule();
    bool is_sent;
public:
    DecodeModule( PerfMIPS* machine);
    ~DecodeModule();
    void clock();
    string dump( string indent);
};

class FetchModule : public Module
{
    WritePort<uint32>*    wp_fetch_2_dec;
    uint32 instr_bytes;
    FetchModule();
    uint32 instr_PC;
public:
    FetchModule( PerfMIPS* machine);
    ~FetchModule();
    void clock();
    string dump( string indent);
};

class ExecuteModule : public Module
{
    ReadPort<FuncInstr>*  rp_dec_2_exec;
    WritePort<FuncInstr>* wp_exec_2_mem;
    FuncInstr cur_instr;
    ExecuteModule();
public:
    ExecuteModule( PerfMIPS* machine);
    ~ExecuteModule();
    void clock();
    string dump( string indent);
};

class MemoryModule : public Module
{
    ReadPort<FuncInstr>*  rp_exec_2_mem;
    WritePort<FuncInstr>* wp_mem_2_wb;
    FuncInstr cur_instr;
    MemoryModule();
public:
    MemoryModule( PerfMIPS* machine);
    ~MemoryModule();
    void clock();
    string dump( string indent);
};

class WritebackModule : public Module
{
    ReadPort<FuncInstr>*  rp_mem_2_wb;
    FuncInstr cur_instr;
    WritebackModule();
public:
    WritebackModule( PerfMIPS* machine);
    ~WritebackModule();
    void clock();
    string dump( string indent);
    string silent_dump( string indent = " ");
};

#endif
