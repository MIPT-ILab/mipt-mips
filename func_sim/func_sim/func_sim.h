/*
 * funcsim.h - MIPS single-cycle implementation
 * @author Aleksandr Shashev <aleksandr.shashev.phystech.edu>
 * Copyright 2016 MIPT-MIPS
 */

// MIPT-MIPS modules
#include <types.h>
#include <func_memory.h>
#include <func_instr.h>

class RF;

class MIPS 
{
    // storages of internal state
    RF* rf;
    uint32 PC;
    uint32 HI;
    uint32 LO;
    FuncMemory* mem;
     
    uint32 fetch () const { return mem->read( PC); }
    void read_src ( FuncInstr& instr); 
    void wb (FuncInstr& instr);
    
    void load ( FuncInstr& instr);
    void store ( const FuncInstr& instr);
    
    void updatePC ( const FuncInstr& instr) 
    { 
        PC = instr.new_PC; 
    }
    
    public:   
         MIPS ();
        ~MIPS ();
        void run ( const string&, uint instr_to_run);
};


enum RegNum 
{
    ZERO = 0,
    AT,
    V0, v1,
    A0, A1, A2, A3,
    T0, T1, T2, T3, T4, T5, T6, T7,
    S0, S1, S2, S3, S4, S5, S6, S7,
    T8, T9, 
    K0, K1,
    GP,
    SP,
    S8,
    RA,
    MAX_REG = 32
};
//typedef int RegNum;

class RF 
{
    static const int MAX_REG = 32;

    uint32 array[MAX_REG];
    
    public:
        RF ()
        {
            for (int i = 0; i < MAX_REG; i++)
                //reset( (RegNum) i);
                array [static_cast< RegNum>(i)] = 0; 
        }
        ~RF ()
        {
            std::cout << "RF deleted" << std::endl; 
        }
        
        uint32 read( RegNum index) const
        {
            return array [index];
        }
        void write( RegNum index, uint32 data)
        {
            array [index] = data;
        }
        void reset( RegNum index) // clears register to 0 value
        {   
            array [index] = 0;
        }
        
        void Dump ()
        {
            std::cout << "Dump" << std::endl;
        
            for (int i = 0; i < MAX_REG; i++)
                std::cout << "reg[ " << FuncInstr::regTable [i] << "] = " << array [static_cast< RegNum>(i)] << std::endl;
            std::cout << "end_dump" << std::endl;
        }
        
};
