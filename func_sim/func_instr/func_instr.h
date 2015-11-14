/**
 * func_instr.h - Header of module implementing the MIPS disassembler
 * @author Roman Zavodskikh <roman.zavodskikh@phystech.edu>
 * Copyright 2015 uArchSim iLab project
 */

//protection from multi-include
#ifndef FUNC_INSTR__FUNC_INSTR_H
#define FUNC_INSTR__FUNC_INSTR_H

// Generic C++
#include <string>

// Generic C

// uArchSim modules
#include <types.h>

using namespace std;

class FuncInstr
{
    private:
        enum FormatType
        {
            FORMAT_R,
            FORMAT_I,
            FORMAT_J,
            NO_FORMAT
        } format;

        enum Type
        {
            ADD,
            ADDU,
            SUB,
            SUBU,
            ADDI,
            ADDIU,
            MULT,
            MULTU,
            DIV,
            DIVU,
            MFHI,
            MTHI,
            MFLO,
            MTLO,
            SLL,
            SRA,
            SLLV,
            SRLV,
            SRAV,
            LUI,
            SLT,
            SLTU,
            SLTI,
            SLTIU,
            AND,
            OR,
            XOR,
            NOR,
            ANDI,
            ORI,
            XORI,
            BEQ,
            BNE,
            BLEZ,
            BGTZ,
            J,
            JAL,
            JR,
            JALR,
            LB,
            LH,
            LW,
            LBU,
            LHU,
            SB,
            SH,
            SW,
            SYSCALL,
            BREAK,
            TRAP,
            NO_TYPE
        } type;

        enum Register
        {
            ZERO = 0, AT,
            V0, V1,
            A0, A1, A2, A3,
            T0, T1, T2, T3, T4, T5, T6, T7,
            S0, S1, S2, S3, S4, S5, S6, S7,
            T8, T9, K0, K1, GP, SP, S8, RA, NUM_OF_REGS
        };

        struct ISAEntry
        {
            const char* name;

            uint8 opcode;
            uint8 func;

            FuncInstr::FormatType format;
            FuncInstr::Type type;
        };
        static const ISAEntry isaTable[];

        union
        {
            struct
            {
                unsigned imm:16;
                unsigned rt:5;
                unsigned rs:5;
                unsigned op:6;
            } asI;
            struct
            {
                unsigned funct:6;
                unsigned shamt:5;
                unsigned rd:5;
                unsigned rt:5;
                unsigned rs:5;
                unsigned op:6;
            } asR;
            struct
            {
                unsigned offset:26;
                unsigned op:6;
            } asJ;
            uint32 raw;
        } bytes;

        char* cmd_name;

        void initFormat( uint32 bytes);
        void parseR( uint32 bytes);
        void parseI( uint32 bytes);
        void parseJ( uint32 bytes);

        bool isLoadStore( Type type) const;
        bool isAluOp( Type type) const;

        string asReg( unsigned char reg) const;
        string asRegOff( unsigned char reg, uint16 offset) const;
    public:
        FuncInstr( uint32 bytes);
        ~FuncInstr();
        string Dump( std::string indent = " ") const;
};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);

const FuncInstr::ISAEntry FuncInstr::isaTable[] =
{
    // name       opcode  func    format               type
    { "add ",     0x0,    0x20,   FuncInstr::FORMAT_R, FuncInstr::ADD},
    { "addu ",    0x0,    0x21,   FuncInstr::FORMAT_R, FuncInstr::ADDU},
    { "sub ",     0x0,    0x22,   FuncInstr::FORMAT_R, FuncInstr::SUB},
    { "subu ",    0x0,    0x23,   FuncInstr::FORMAT_R, FuncInstr::SUBU},
    { "addi ",    0x8,    0x0,    FuncInstr::FORMAT_I, FuncInstr::ADDI},
    { "addiu ",   0x9,    0x0,    FuncInstr::FORMAT_I, FuncInstr::ADDIU},
    { "mult ",    0x0,    0x18,   FuncInstr::FORMAT_R, FuncInstr::MULT},
    { "multu ",   0x0,    0x19,   FuncInstr::FORMAT_R, FuncInstr::MULTU},
    { "div ",     0x0,    0x1A,   FuncInstr::FORMAT_R, FuncInstr::DIV},
    { "divu ",    0x0,    0x1B,   FuncInstr::FORMAT_R, FuncInstr::DIVU},
    { "mfhi ",    0x0,    0x10,   FuncInstr::FORMAT_R, FuncInstr::MFHI},
    { "mthi ",    0x0,    0x11,   FuncInstr::FORMAT_R, FuncInstr::MTHI},
    { "mflo ",    0x0,    0x12,   FuncInstr::FORMAT_R, FuncInstr::MFLO},
    { "mtlo ",    0x0,    0x13,   FuncInstr::FORMAT_R, FuncInstr::MTLO},
    { "sll ",     0x0,    0x0,    FuncInstr::FORMAT_R, FuncInstr::SLL},
    { "sra ",     0x0,    0x3,    FuncInstr::FORMAT_R, FuncInstr::SRA},
    { "sllv ",    0x0,    0x4,    FuncInstr::FORMAT_R, FuncInstr::SLLV},
    { "srlv ",    0x0,    0x6,    FuncInstr::FORMAT_R, FuncInstr::SRLV},
    { "srav ",    0x0,    0x7,    FuncInstr::FORMAT_R, FuncInstr::SRAV},
    { "lui ",     0xF,    0x0,    FuncInstr::FORMAT_I, FuncInstr::LUI},
    { "slt ",     0x0,    0x2A,   FuncInstr::FORMAT_R, FuncInstr::SLT},
    { "sltu ",    0x0,    0x2B,   FuncInstr::FORMAT_R, FuncInstr::SLTU},
    { "slti ",    0xA,    0x0,    FuncInstr::FORMAT_I, FuncInstr::SLTI},
    { "sltiu ",   0xB,    0x0,    FuncInstr::FORMAT_I, FuncInstr::SLTIU},
    { "and ",     0x0,    0x24,   FuncInstr::FORMAT_R, FuncInstr::AND},
    { "or ",      0x0,    0x25,   FuncInstr::FORMAT_R, FuncInstr::OR},
    { "xor ",     0x0,    0x26,   FuncInstr::FORMAT_R, FuncInstr::XOR},
    { "nor ",     0x0,    0x27,   FuncInstr::FORMAT_R, FuncInstr::NOR},
    { "andi ",    0xC,    0x0,    FuncInstr::FORMAT_I, FuncInstr::ANDI},
    { "ori ",     0xD,    0x0,    FuncInstr::FORMAT_I, FuncInstr::ORI},
    { "xori ",    0xE,    0x0,    FuncInstr::FORMAT_I, FuncInstr::XORI},
    { "beq ",     0x4,    0x0,    FuncInstr::FORMAT_I, FuncInstr::BEQ},
    { "bne ",     0x5,    0x0,    FuncInstr::FORMAT_I, FuncInstr::BNE},
    { "blez ",    0x6,    0x0,    FuncInstr::FORMAT_I, FuncInstr::BLEZ},
    { "bgtz ",    0x7,    0x0,    FuncInstr::FORMAT_I, FuncInstr::BGTZ},
    { "j ",       0x2,    0x0,    FuncInstr::FORMAT_J, FuncInstr::J},
    { "jal ",     0x3,    0x0,    FuncInstr::FORMAT_J, FuncInstr::JAL},
    { "jr ",      0x0,    0x8,    FuncInstr::FORMAT_R, FuncInstr::JR},
    { "jalr ",    0x0,    0x9,    FuncInstr::FORMAT_R, FuncInstr::JALR},
    { "lb ",      0x20,   0x0,    FuncInstr::FORMAT_I, FuncInstr::LB},
    { "lh ",      0x21,   0x0,    FuncInstr::FORMAT_I, FuncInstr::LH},
    { "lw ",      0x23,   0x0,    FuncInstr::FORMAT_I, FuncInstr::LW},
    { "lbu ",     0x24,   0x0,    FuncInstr::FORMAT_I, FuncInstr::LBU},
    { "lhu ",     0x25,   0x0,    FuncInstr::FORMAT_I, FuncInstr::LHU},
    { "sb ",      0x28,   0x0,    FuncInstr::FORMAT_I, FuncInstr::SB},
    { "sh ",      0x29,   0x0,    FuncInstr::FORMAT_I, FuncInstr::SH},
    { "sw ",      0x2B,   0x0,    FuncInstr::FORMAT_I, FuncInstr::SW},
    { "syscall ", 0x0,    0xC,    FuncInstr::FORMAT_R, FuncInstr::SYSCALL},
    { "break ",   0x0,    0xD,    FuncInstr::FORMAT_R, FuncInstr::BREAK},
    { "trap ",    0x0,    0x1A,   FuncInstr::FORMAT_J, FuncInstr::TRAP}
};

#endif //FUNC_INST__FUNC_INSTR_H
