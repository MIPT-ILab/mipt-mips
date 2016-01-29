/*
 * func_instr.h - instruction parser for mips
 * @author Aleksandr Shashev <aleksandr.shashev.phystech.edu>
 * Copyright 2016 MIPT-MIPS
 */


#ifndef FUNC_INSTR_H
#define FUNC_INSTR_H

// Generic C++
#include <string>
#include <cassert>

// MIPT-MIPS modules
#include <types.h>
#include <elf_parser.h>

class FuncInstr
{
    public:
        uint32 v_src1  ; // v_src1 = rt in R_Type or v_src1 = imm in J_Type
        uint32 v_src2  ; // v_src2 = rs in R_Type or in in I_Type
        uint32 v_dst   ; 
        uint32 v_C     ; // c = shamt in R_Type or c = imm in I_Type
        uint32 v_HI    ;
        uint32 v_LO    ;
        uint32 mem_addr;
        uint32 new_PC  ;
    
        FuncInstr ( uint32 bytes);
        FuncInstr( uint32 bytes, uint32 PC) : instr( bytes), new_PC(PC) 
        {
            initFormat(); 
            switch ( format)
            {
                case FORMAT_R:
                    initR();
                    break;
                case FORMAT_I:
                    initI();
                    break;
                case FORMAT_J:
                    initJ();
                    break;
                case FORMAT_UNKNOWN:
                    initUnknown();
                    break;
             }
        }
        
        std::string Dump( std::string indent = " ") const;
        
        int get_src1_num_index () const;
        int get_src2_num_index () const;
        int get_dst_num_index  () const;
        int get_C () const;
        
        void execute () 
        { 
            (this->*isaTable [isaNum].func_pointer) (); 
       
            if (operation != OUT_J_JUMP &&
                operation != OUT_R_JUMP) 
                new_PC += 4; //this instruction size 
        }

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

        struct ISAEntry
        {
            std::string name;

            uint8 opcode;
            uint8 funct;

            Format format;
            OperationType operation;
            
            void (FuncInstr::* func_pointer)(void);
        };
        uint32 isaNum;

        static const ISAEntry isaTable[];
        static const uint32 isaTableSize;
        static const char *regTable[];
    
    private:  
        void add  () { v_dst = v_src1 + v_src2; }
        void addi () { v_dst = v_src1 + v_C   ; }
        void sub  () { v_dst = v_src1 - v_src2; }
        void subi () { v_dst = v_src1 - v_C   ; }
        
        void mult () { v_LO = (v_src2 * v_src1) &  0xFFFFFFFF; 
                       v_HI = (v_src2 * v_src1) >> 0x20      ; }
        void div  () { v_LO = v_src2 / v_src1; 
                       v_HI = v_src2 % v_src1; }
        void mfhi () { v_dst = v_HI  ; }
        void mthi () { v_HI  = v_src2; }
        void mflo () { v_dst = v_LO  ; }
        void mtlo () { v_LO  = v_src2; }
        
        void sll  () { v_dst = v_src1 <<  v_C;    }
        void srl  () { v_dst = v_src1 >>  v_C;    }
        void sra  () { v_dst = (int32) v_src1 >> (int32) v_C; }
        void sllv () { v_dst = v_src1 <<  v_src2; }
        void srlv () { v_dst = v_src1 >>  v_src2; }
        void srav () { v_dst = (int32) v_src1 >> (int32) v_src2; } 
        void lui  () { v_dst = v_C << 16;         }
        
        void slt  () { if (v_src2 < v_src1) v_dst = 1;
                       else                 v_dst = 0; }
        void slti () { if (v_src2 < v_C   ) v_dst = 1;
                       else                 v_dst = 0; }
                       
        void And  () { v_dst =    v_src2 & v_src1 ; }
        void andi () { v_dst =    v_src2 & v_C    ; }
        void Or   () { v_dst =    v_src2 | v_src1 ; }
        void ori  () { v_dst =    v_src2 | v_C    ; }
        void Xor  () { v_dst =    v_src2 ^ v_src1 ; }
        void xori () { v_dst =    v_src2 ^ v_C    ; }
        void nor  () { v_dst = ~ (v_src2 | v_src1); }
        
        void beq  () { if (v_src2 == v_src1) new_PC += (v_C << 2); }
        void bne  () { if (v_src2 != v_src1) new_PC += (v_C << 2); }
        void blez () { if (v_src2 <= 0     ) new_PC += (v_C << 2); }
        void bgtz () { if (v_src2 >  0     ) new_PC += (v_C << 2); }
        
        void j    () { new_PC = (new_PC & 0xf0000000) | (v_src1 << 2); }
        void jal  () { v_dst  = new_PC + 4                           ;
                       new_PC = (new_PC & 0xf0000000) | (v_src1 << 2); } 
        void jr   () { new_PC = v_src2                               ; }
        void jalr () { v_dst  = new_PC + 4;
                       new_PC = v_src2                               ; }
        
        void lb   () { mem_addr = (v_src2 + v_C); }
        void lh   () { mem_addr = (v_src2 + v_C); }
        void lw   () { mem_addr = (v_src2 + v_C); }
        void lbu  () { mem_addr = (v_src2 + v_C); }
        void lhu  () { mem_addr = (v_src2 + v_C); } 
        
        void sb   () { mem_addr = (v_src2 + v_C);
                       v_dst    = (v_src1 & 0xff  ); }
        void sh   () { mem_addr = (v_src2 + v_C);
                       v_dst    = (v_src1 & 0xffff); }
        void sw   () { mem_addr = (v_src2 + v_C);
                       v_dst    =  v_src1          ; }
        
        void Syscall () {}
        void Break   () {}    
        void Trap    () {}
    
        std::string disasm;
                                                               
        void initFormat();
        void initR();
        void initI();
        void initJ();
        void initUnknown();
};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);

#endif //FUNC_INSTR_H

