#include <func_memory.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
using namespace std;
class FuncInstr
{
public:
    FuncInstr(uint32 bytes);
    ~FuncInstr();
    string Dump(std::string indent = " ") const;

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
        SRL,
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


    enum FormatType
    {
        FORMAT_I,
        FORMAT_J,
        FORMAT_R
    } format;

    enum Registers
    {
        ZERO = 0,
        AT,
        V0, V1,
        A0, A1, A2, A3,
        T0, T1, T2, T3, T4, T5, T6, T7,
        S0, S1, S2, S3, S4, S5, S6, S7,
        T8, T9,
        K0, K1,
        GP,
        SP,
        S8,
        RA,
        REG_COUNT
    };

    union
    {
        struct
        {
            unsigned imm : 16;
            unsigned rt : 5;
            unsigned rs : 5;
            unsigned op : 6;
        }asI;
        struct
        {
            unsigned offset : 26;
            unsigned op : 6;
        }asJ;
        struct
        {
            unsigned funct : 6;
            unsigned shamt : 5;
            unsigned rd : 5;
            unsigned rt : 5;
            unsigned rs : 5;
            unsigned op : 6;
        }asR;
        uint32 raw;

    } bytes;

    struct ISAEntry
    {
        const char* name;

        uint8 opcode;
        uint8 func;

        FuncInstr::FormatType format;
        FuncInstr::Type type;
    };

    const char* name;
    ostringstream disassembled;

    static const ISAEntry isaTable[];




    void ParseI(uint32 bytes);
    void ParseJ(uint32 bytes);
    void ParseR(uint32 bytes);

};
