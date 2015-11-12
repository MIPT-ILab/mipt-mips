//protection from multi-include
#ifndef FUNC_INSTR__FUNC_INSTR_H
#define FUNC_INSTR__FUNC_INSTR_H
// Generic C
#include <cassert>
// Generic C++
#include <string>
#include <iostream>
#include <sstream>
// uArchSim modules
#include <types.h>
#include <elf_parser.h>
#include <func_memory.h>

enum Format
{
    R_FMT,
    I_FMT,
    J_FMT,
    INVALID_FMT
};

enum Type
{
    ADD, ADDU, SUB, SUBU, ADDI, ADDIU,
    MULT, MULTU, DIV, DIVU, MHFI, MTHI, MFLO, MTLO, 
    SLL, SRL, SRA, SLLV, SRLV, SRAV, LUI,
    SLT, SLTU, SLTI, SLTIU,
    AND, OR, XOR, NOR, ANDI, ORI, XORI,
    BEQ, BNE, BLEZ, BGTZ,
    J, JAL, JR, JALR,
    LB, LH, LW, LBU, LHU, LHWU,
    SB, SH, SW, 
    SYSCALL, BREAK, TRAP 
};

struct RegEntry
{
    const char* name;
    uint8 num;  
};

struct ISAEntry
{
    const char* name;
    uint8 opcode;
    uint8 func;
    Format format;
    Type type;
    bool rs;
    bool rt;
    bool rd;
    bool shamt;
};

class FuncInstr
{
    public:
        FuncInstr( uint32 bytes);
        ~FuncInstr();
        std::string Dump( std::string indent = " ");

    private:
        inline void processAsR();
        inline void processAsI();
        inline void processAsJ();
        
        string disasm_string;        
        const char* name;

        union
        {
            uint32 raw;
            struct
            {
                unsigned int func:   6;       
                unsigned int shamt:  5;
                unsigned int rd:     5;
                unsigned int rt:     5;
                unsigned int rs:     5;
                unsigned int opcode: 6;
            } asR;
            struct
            {
                unsigned int imm:   16;  
                unsigned int rt:     5;
                unsigned int rs:     5;
                unsigned int opcode: 6;
            } asI;
            struct
            {
                unsigned int addr:  26; 
                unsigned int opcode: 6;
            } asJ;
        } instr;

        Format format;
        Type type;

        const static ISAEntry isaTable[];
        const static RegEntry regTable[];
};
std::ostream& operator<< ( std::ostream& out, const FuncInstr& instr);

#endif // #ifndef FUNC_INSTR__FUNC_INSTR_H
