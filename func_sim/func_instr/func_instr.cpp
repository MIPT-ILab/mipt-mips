#include <func_instr.h>
#include <sstream>
#include <iostream>
#include <fstream>

const FuncInstr::ISAEntry FuncInstr::isaTable[] =
{
    // name       opcode  func    format               type
    { "add ",     0x0,    0x20,   FuncInstr::FORMAT_R, FuncInstr::ADD },
    { "addu ",    0x0,    0x21,   FuncInstr::FORMAT_R, FuncInstr::ADDU },
    { "sub ",     0x0,    0x22,   FuncInstr::FORMAT_R, FuncInstr::SUB },
    { "subu ",    0x0,    0x23,   FuncInstr::FORMAT_R, FuncInstr::SUBU },
    { "addi ",    0x8,    0x0,    FuncInstr::FORMAT_I, FuncInstr::ADDI },
    { "addiu ",   0x9,    0x0,    FuncInstr::FORMAT_I, FuncInstr::ADDIU },
    { "mult ",    0x0,    0x18,   FuncInstr::FORMAT_R, FuncInstr::MULT },
    { "multu ",   0x0,    0x19,   FuncInstr::FORMAT_R, FuncInstr::MULTU },
    { "div ",     0x0,    0x1A,   FuncInstr::FORMAT_R, FuncInstr::DIV },
    { "divu ",    0x0,    0x1B,   FuncInstr::FORMAT_R, FuncInstr::DIVU },
    { "mfhi ",    0x0,    0x10,   FuncInstr::FORMAT_R, FuncInstr::MFHI },
    { "mthi ",    0x0,    0x11,   FuncInstr::FORMAT_R, FuncInstr::MTHI },
    { "mflo ",    0x0,    0x12,   FuncInstr::FORMAT_R, FuncInstr::MFLO },
    { "mtlo ",    0x0,    0x13,   FuncInstr::FORMAT_R, FuncInstr::MTLO },
    { "sll ",     0x0,    0x0,    FuncInstr::FORMAT_R, FuncInstr::SLL },
    { "srl ",     0x0,    0x2,    FuncInstr::FORMAT_R, FuncInstr::SRL },
    { "sra ",     0x0,    0x3,    FuncInstr::FORMAT_R, FuncInstr::SRA },
    { "sllv ",    0x0,    0x4,    FuncInstr::FORMAT_R, FuncInstr::SLLV },
    { "srlv ",    0x0,    0x6,    FuncInstr::FORMAT_R, FuncInstr::SRLV },
    { "srav ",    0x0,    0x7,    FuncInstr::FORMAT_R, FuncInstr::SRAV },
    { "lui ",     0xF,    0x0,    FuncInstr::FORMAT_I, FuncInstr::LUI },
    { "slt ",     0x0,    0x2A,   FuncInstr::FORMAT_R, FuncInstr::SLT },
    { "sltu ",    0x0,    0x2B,   FuncInstr::FORMAT_R, FuncInstr::SLTU },
    { "slti ",    0xA,    0x0,    FuncInstr::FORMAT_I, FuncInstr::SLTI },
    { "sltiu ",   0xB,    0x0,    FuncInstr::FORMAT_I, FuncInstr::SLTIU },
    { "and ",     0x0,    0x24,   FuncInstr::FORMAT_R, FuncInstr::AND },
    { "or ",      0x0,    0x25,   FuncInstr::FORMAT_R, FuncInstr::OR },
    { "xor ",     0x0,    0x26,   FuncInstr::FORMAT_R, FuncInstr::XOR },
    { "nor ",     0x0,    0x27,   FuncInstr::FORMAT_R, FuncInstr::NOR },
    { "andi ",    0xC,    0x0,    FuncInstr::FORMAT_I, FuncInstr::ANDI },
    { "ori ",     0xD,    0x0,    FuncInstr::FORMAT_I, FuncInstr::ORI },
    { "xori ",    0xE,    0x0,    FuncInstr::FORMAT_I, FuncInstr::XORI },
    { "beq ",     0x4,    0x0,    FuncInstr::FORMAT_I, FuncInstr::BEQ },
    { "bne ",     0x5,    0x0,    FuncInstr::FORMAT_I, FuncInstr::BNE },
    { "blez ",    0x6,    0x0,    FuncInstr::FORMAT_I, FuncInstr::BLEZ },
    { "bgtz ",    0x7,    0x0,    FuncInstr::FORMAT_I, FuncInstr::BGTZ },
    { "j ",       0x2,    0x0,    FuncInstr::FORMAT_J, FuncInstr::J },
    { "jal ",     0x3,    0x0,    FuncInstr::FORMAT_J, FuncInstr::JAL },
    { "jr ",      0x0,    0x8,    FuncInstr::FORMAT_R, FuncInstr::JR },
    { "jalr ",    0x0,    0x9,    FuncInstr::FORMAT_R, FuncInstr::JALR },
    { "lb ",      0x20,   0x0,    FuncInstr::FORMAT_I, FuncInstr::LB },
    { "lh ",      0x21,   0x0,    FuncInstr::FORMAT_I, FuncInstr::LH },
    { "lw ",      0x23,   0x0,    FuncInstr::FORMAT_I, FuncInstr::LW },
    { "lbu ",     0x24,   0x0,    FuncInstr::FORMAT_I, FuncInstr::LBU },
    { "lhu ",     0x25,   0x0,    FuncInstr::FORMAT_I, FuncInstr::LHU },
    { "sb ",      0x28,   0x0,    FuncInstr::FORMAT_I, FuncInstr::SB },
    { "sh ",      0x29,   0x0,    FuncInstr::FORMAT_I, FuncInstr::SH },
    { "sw ",      0x2B,   0x0,    FuncInstr::FORMAT_I, FuncInstr::SW },
    { "syscall ", 0x0,    0xC,    FuncInstr::FORMAT_R, FuncInstr::SYSCALL },
    { "break ",   0x0,    0xD,    FuncInstr::FORMAT_R, FuncInstr::BREAK },
    { "trap ",    0x0,    0x1A,   FuncInstr::FORMAT_J, FuncInstr::TRAP }
};

FuncInstr::FuncInstr(uint32 argument)
{
    cout << "created\n" << argument;
    this->bytes.raw = argument;
    uint32 opcode = argument >> 26;
    if (opcode == 0)
    {
        this->format = FORMAT_R;
    }
    else if (opcode == 2 || opcode == 3 || opcode == 0x1A)
    {
        this->format = FORMAT_J;
    }
    else 
    {
        this->format = FORMAT_I;
    }

    cout << "format:" << this->format;
    ParseR(bytes.raw);


    //getting  format

}
FuncInstr::~FuncInstr()
{
    cout << "died\n";
}

void FuncInstr::ParseR(uint32 bytes)
{
    cout << "parseR\n";
    for (int i = 0; i < sizeof(isaTable) / sizeof(isaTable[0]); i++)
    {
        cout << isaTable[i].name << "<-tried this\n";


    }

}

int main()
{
    FuncInstr lol(123);
    //cout << "blabla";
}