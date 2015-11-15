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
            ADD = 0,
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
            JAL, JR,
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
        string full_cmd_name;

        void initFormat( uint32 bytes);
        void parseR( uint32 bytes);
        void parseI( uint32 bytes);
        void parseJ( uint32 bytes);

        bool isLoadStore( Type type) const;
        bool isAluOp( Type type) const;

        string asReg( unsigned char reg) const;
        string asRegOff( unsigned char reg, uint16 offset) const;
        string strFormat( FormatType format) const;
    public:
        FuncInstr( uint32 bytes);
        ~FuncInstr();
        string Dump( std::string indent = " ") const;
};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);

#endif //FUNC_INSTR__FUNC_INSTR_H
