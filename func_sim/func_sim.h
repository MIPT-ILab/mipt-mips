/*
 * func_sim.h - MIPS singe-cycle simulator
 */

#ifndef FUNC_SIM_H
#define FUNC_SIM_H

#include "func_memory/func_memory.h"
#include "func_instr/func_instr.h"

enum RegNum
{
    MAX_REG = 32
};

class RF 
{
private:
    uint32 array[MAX_REG];
public:
    RF();
    ~RF();
    uint32 read( RegNum index) const;
    void write( RegNum index, uint32 data);
    void reset( RegNum index);
};

class MIPS 
{
private:
    RF* rf;
    uint32 PC;
    uint32 hi;
    uint32 lo;
    FuncMemory* mem;
    std::string mem_dump;

    uint32 fetch() const;
    void read_src( FuncInstr& instr);
    void wb( FuncInstr& instr);
    void updatePC( const FuncInstr& instr);
    void load( FuncInstr& instr);
    void store( const FuncInstr& instr);
    void ld_st( FuncInstr& instr); 
public:
    MIPS();
    ~MIPS();
    void run( const std::string& name, unsigned int instr_to_run);
};  

#endif //FUNC_INSTR_H
