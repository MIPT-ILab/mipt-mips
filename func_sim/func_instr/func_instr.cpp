#include <func_instr.h>
#include <stdlib.h>

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

    //getting  format
    uint32 opcode = argument >> 26;
    cout << "opcode" << opcode;
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
    switch (this->format)
    {
    case FORMAT_I: 
        this->ParseI(bytes.raw);
        break;
    case FORMAT_J:
        this->ParseJ(bytes.raw);
        break;
    case FORMAT_R:
        this->ParseR(bytes.raw);
        break;
    default:
        cout << "WRONG FORMAT";
        abort();
        break;
    }
    
    //creating disassembled string
    this->disassembled << "NAME:" << this->name;

    switch (this->format)
    {
    case FORMAT_J:
        this->disassembled << " :" << hex << this->bytes.asJ.offset << "\n";
        break;

    case FORMAT_I:
        if (TwoRegsOneOffset(this->type))
        {
            disassembled << RegToStr(this->bytes.asI.rt) << ", "
                         << RegAndOffsetToStr(this->bytes.asI.rs, this->bytes.asI.imm);
        }
        else if (TwoRegsOneImm(this->type))
        {
            disassembled << RegToStr(this->bytes.asI.rt) << ", "
                         << RegToStr(this->bytes.asI.rs) << ", "
                         << "0x" << hex << this->bytes.asI.imm << dec;
        }
        else if (OneRegOneImm(this->type))
        {
            disassembled << RegToStr(this->bytes.asR.rt) << ", "
                         << "0x" << hex << this->bytes.asR.rs << dec;
        }
        else
        {
            cout << "Can't disassembly!";
            abort();
        }
        break;

    case FORMAT_R:
        if (ThreeRegs(this->type))
        {
            disassembled << RegToStr(this->bytes.asR.rd) << ", "
                         << RegToStr(this->bytes.asR.rs) << ", "
                         << RegToStr(this->bytes.asR.rt);
        }
        else if (TwoRegs(this->type))
        {
            disassembled << RegToStr(this->bytes.asR.rd) << ", "
                         << RegToStr(this->bytes.asR.rs);
        }
        else if (TwoRegsOneImm(this->type))
        {
            disassembled << RegToStr(this->bytes.asR.rd) << ", "
                         << RegToStr(this->bytes.asR.rs) << ", "
                         << "0x" << hex << this->bytes.asR.rt << dec;
        }
        else if (OneReg(this->type))
        {
            disassembled << RegToStr(this->bytes.asR.rt);
        }
        else if (OneRegOneImm(this->type))
        {
            disassembled << RegToStr(this->bytes.asR.rt) << ", "
                         << "0x" << hex << this->bytes.asR.rs << dec;
        }
        else
        {
            cout << "Can't disassembly!";
            abort();
        }
        break;

    }

    cout << this->disassembled.str();

}
FuncInstr::~FuncInstr()
{
    cout << this->name << "died\n";
}

void FuncInstr::ParseI(uint32 bytes)
{
    cout << "parseI\n";
    cout << "op" << this->bytes.asI.op;
    for (int i = 0; i < sizeof(isaTable) / sizeof(isaTable[0]); i++)
    {
        cout << isaTable[i].name << "<-tried this\n";
        if (this->bytes.asI.op == isaTable[i].opcode
            && isaTable[i].format == FORMAT_I
            && this->format == FORMAT_I)
        {
            cout << "applying" << isaTable[i].name;
            this->type = isaTable[i].type;
            this->name = isaTable[i].name;
            return;
        }

    }
    cout << "NO SUCH I COMMAND";
    abort();
}

void FuncInstr::ParseJ(uint32 bytes)
{
    cout << "parseJ\n";
    for (int i = 0; i < sizeof(isaTable) / sizeof(isaTable[0]); i++)
    {
        cout << isaTable[i].name << "<-tried this\n";
        if (this->bytes.asJ.op == isaTable[i].opcode
            && isaTable[i].format == FORMAT_J
            && this->format == FORMAT_J)
        {
            cout << "applying" << isaTable[i].name;
            this->type = isaTable[i].type;
            this->name = isaTable[i].name;
            return;
        }

    }
    cout << "NO SUCH J COMMAND";
    abort();
}

void FuncInstr::ParseR(uint32 bytes)
{
    cout << "parseR\n";
    cout << "func" << this->bytes.asR.funct;
    for (int i = 0; i < sizeof(isaTable) / sizeof(isaTable[0]); i++)
    {
        cout << isaTable[i].name << "<-tried this\n";
        if (this->bytes.asR.funct == isaTable[i].func
            && isaTable[i].format == FORMAT_R 
            && this->format == FORMAT_R)
        {
            cout << "applying" << isaTable[i].name;
            this->type = isaTable[i].type;
            this->name = isaTable[i].name;
            return;
        }

    }
    cout << "NO SUCH R COMMAND";
    abort();
}

bool FuncInstr::ThreeRegs(Type type)
{
    return (type == ADD || type == ADDU || type == SUB || type == SUBU
        || type == SLLV || type == SRLV || type == SRAV|| type == SLT
        || type == SLTU || type == AND  || type == OR  || type == XOR 
        || type == NOR);
}

bool FuncInstr::TwoRegsOneOffset(Type type)
{
    return (type == LB || type == LH || type == LW || type == LBU 
        || type == LHU || type == SB || type == SH || type == SW);
}

bool FuncInstr::TwoRegsOneImm(Type type) 
{
    return (type == ADDI || type == ADDIU || type == SLL  || type == SRL
        || type == SRA   || type == SLTI  || type == SLTIU|| type == ANDI 
        || type == ORI   || type == XORI  || type == BEQ  || type == BNE);
}

bool FuncInstr::TwoRegs(Type type)
{
    return (type == MULT || type == MULTU || type == DIV || type == DIVU);
}

bool FuncInstr::OneReg(Type type)
{
    return (type == MFHI || type == MTHI || type == MFLO || type == MTLO
        || type == JR    || type == JALR);
}

bool FuncInstr::OneRegOneImm(Type type)
{
    return (type == LUI || type == BLEZ || type == BGTZ);
}

string FuncInstr::RegToStr(unsigned char reg)
{
    switch (reg)
    {
    case ZERO: return "$zero"; break;
    case AT: return "$at"; break;
    case V0: return "$v0"; break;
    case V1: return "$v1"; break;
    case A0: return "$a0"; break;
    case A1: return "$a1"; break;
    case A2: return "$a2"; break;
    case A3: return "$a3"; break;
    case T0: return "$t0"; break;
    case T1: return "$t1"; break;
    case T2: return "$t2"; break;
    case T3: return "$t3"; break;
    case T4: return "$t4"; break;
    case T5: return "$t5"; break;
    case T6: return "$t6"; break;
    case T7: return "$t7"; break;
    case S0: return "$s0"; break;
    case S1: return "$s1"; break;
    case S2: return "$s2"; break;
    case S3: return "$s3"; break;
    case S4: return "$s4"; break;
    case S5: return "$s5"; break;
    case S6: return "$s6"; break;
    case S7: return "$s7"; break;
    case T8: return "$t8"; break;
    case T9: return "$t9"; break;
    case K0: return "$k0"; break;
    case K1: return "$k1"; break;
    case GP: return "$gp"; break;
    case SP: return "$sp"; break;
    case S8: return "$s8"; break;
    case RA: return "$ra"; break;
    default: abort(); break;
    }
}

string FuncInstr::RegAndOffsetToStr(unsigned char reg, uint32 offset)
{
    ostringstream temp;
    temp << "0x" << hex << offset  << dec 
         << "(" << RegToStr(reg) << ")";
}


int main()
{
    FuncInstr lol(00010203);
  //  FuncInstr lol(‭536870912‬);
    //cout << "blabla";
}