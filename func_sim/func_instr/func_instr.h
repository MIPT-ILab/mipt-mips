/*
 * func_instr.h - instruction parser for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2014 MIPT-MIPS
 */


#ifndef FUNC_INSTR_H
#define FUNC_INSTR_H

// Generic C++
#include <string>
#include <cassert>
#include <cstdio>

// MIPT-MIPS modules
#include <types.h>
#include <elf_parser.h>

class FuncInstr
{
    public:
        FuncInstr( uint32 bytes, uint32 PC = 0);
        std::string Dump( std::string indent = " ") const;
        std::string MIPSdump( std::string indent = " ") const;

        uint32 get_isaNum() const { return isaNum; };

        uint32 v_src1;
        uint32 v_src2;
        uint32 v_dst;
        uint32 mem_addr;
        unsigned short int byte_num;
        unsigned short int hw_num;
        uint32 new_PC;
        uint32 hi;
        uint32 lo;
        const uint32 PC;

        int get_src1_num_index() const;
        int get_src2_num_index() const;
        int get_dst_num_index() const;

        void add()      { v_dst = v_src1 + v_src2; };
        void addu()     { v_dst = v_src1 + v_src2; };                               
        void sub()      { v_dst = v_src1 - v_src2; };                                
        void subu()     { v_dst = v_src1 - v_src2; };                               
        void addi()     { v_dst = v_src2 + instr.asI.imm; };                               
        void addiu()    { v_dst = v_src2 + instr.asI.imm; };
        void mult()     { uint64 mult_res = v_src1 * v_src2; lo = mult_res & 0xFFFFFFFF; hi = mult_res >> 0x20; };
        void multu()    { uint64 mult_res = v_src1 * v_src2; lo = mult_res & 0xFFFFFFFF; hi = mult_res >> 0x20; };            
        void div()      { lo = v_src2 / v_src1; hi = v_src2 % v_src1; };
        void divu()     { lo = v_src2 / v_src1; hi = v_src2 % v_src1; };   
        void mfhi()     { v_dst = hi; };   
        void mthi()     { hi = v_src2; };   
        void mflo()     { v_dst = lo; };   
        void mtlo()     { lo = v_src2;};   
        void sll()      { v_dst = v_src1 << instr.asR.shamt; };    
        void srl()      { v_dst = v_src1 >> instr.asR.shamt; };    
        void sra()      { v_dst = v_src1 >> instr.asR.shamt; };    
        void sllv()     { v_dst = v_src1 << v_src2; };   
        void srlv()     { v_dst = v_src1 >> v_src2; };   
        void srav()     { v_dst = v_src1 >> v_src2; };   
        void lui()      { v_dst = instr.asI.imm << 16; };    
        void slt()      { v_dst = v_src2 < v_src1; };    
        void sltu()     { v_dst = v_src2 < v_src1; };   
        void slti()     { v_dst = v_src2 < instr.asI.imm; };   
        void sltiu()    { v_dst = v_src2 < instr.asI.imm; };   
        void _and()     { v_dst =   v_src2 & v_src1; };   
        void _or()      { v_dst =   v_src2 | v_src1; };    
        void _xor()     { v_dst =   v_src2 ^ v_src1; };   
        void nor()      { v_dst = ~(v_src2 | v_src1); };    
        void andi()     { v_dst = v_src2 & instr.asI.imm; };   
        void ori()      { v_dst = v_src2 | instr.asI.imm; };  
        void xori()     { v_dst = v_src2 ^ instr.asI.imm; };  
        void beq()      { if (v_src1 == v_src2) new_PC += (instr.asI.imm << 2); };    
        void bne()      { if (v_src1 != v_src2) new_PC += (instr.asI.imm << 2); };     
        void blez()     { if (v_src1 <= 0)      new_PC += (instr.asI.imm << 2); }; 
        void bgtz()     { if (v_src1 <= v_src2) new_PC += (instr.asI.imm << 2); }; 
        void jal()      { v_dst = new_PC; new_PC = (PC & 0xF0000000) | (instr.asJ.imm << 2); };    
        void j()        {                 new_PC = (PC & 0xF0000000) | (instr.asJ.imm << 2); };
        void jr()       {                 new_PC = v_src2; };     
        void jalr()     { v_dst = new_PC; new_PC = v_src2; };   
        void lb()       { mem_addr = (v_src2 + instr.asI.imm) / 4; byte_num = (v_src2 + instr.asI.imm) % 4; };     
        void lh()       { mem_addr = (v_src2 + instr.asI.imm) / 2; hw_num   = (v_src2 + instr.asI.imm) % 2; };     
        void lw()       { mem_addr =  v_src2 + instr.asI.imm; };                                             
        void lbu()      { mem_addr = (v_src2 + instr.asI.imm) / 4; byte_num = (v_src2 + instr.asI.imm) % 4; };     
        void lhu()      { mem_addr = (v_src2 + instr.asI.imm) / 2; hw_num   = (v_src2 + instr.asI.imm) % 2; };   
        void sb()       
        { 
            v_dst = v_src1 & 0xFF;   
            mem_addr = (v_src2 + instr.asI.imm) / 4; 
            byte_num = (v_src2 + instr.asI.imm) % 4; 
        }
        void sh()       
        { 
            v_dst = v_src1 & 0xFFFF; 
            mem_addr = (v_src2 + instr.asI.imm) / 2; 
            hw_num   = (v_src2 + instr.asI.imm) % 2; 
        };
        void sw()       { v_dst = v_src1; mem_addr =  v_src2 + instr.asI.imm; };          
        void syscall()  {};
        void _break()   {}; 
        void trap()     {};   

        void execute();

    private:
        void makeMIPSdump();

        union _instr
        {
            struct
            {
                unsigned funct  :6;
                unsigned shamt  :5;
                unsigned rd     :5;
                unsigned rt     :5;
                unsigned rs     :5;
                unsigned opcode :6;
            } asR;
            struct
            {
                unsigned imm    :16;
                unsigned rt     :5;
                unsigned rs     :5;
                unsigned opcode :6;
            } asI;
            struct
            {
                unsigned imm    :26;
                unsigned opcode :6;
            } asJ;
            uint32 raw;

            _instr(uint32 bytes) {
                 raw = bytes;
            }
        } instr;
        
        enum Format
        {
            FORMAT_R,
            FORMAT_I,
            FORMAT_J,
            FORMAT_UNKNOWN
        } format;

        enum OperationType
        {  
            OUT_R_ARITHM,
            OUT_R_SHAMT,
            OUT_R_JUMP,
            OUT_R_SPECIAL,
            OUT_I_ARITHM,
            OUT_I_BRANCH,
            OUT_I_LOAD,
            OUT_I_STORE,
            OUT_J_JUMP,
            OUT_J_SPECIAL
        } operation;

        struct ISAEntry
        {
            std::string name;

            uint8 opcode;
            uint8 funct;

            Format format;
            OperationType operation;

            void (FuncInstr::*exec)();
        };
        uint32 isaNum;

        static const ISAEntry isaTable[];
        static const uint32 isaTableSize;
        static const char *regTable[];

        std::string disasm;
        std::string MIPSdisasm;

        void initFormat();
        void initR();
        void initI();
        void initJ();
        void initUnknown();
};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);

#endif //FUNC_INSTR_H

