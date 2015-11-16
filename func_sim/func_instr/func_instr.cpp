# include <iostream>
# include <cassert>
# include <sstream>
# include "func_instr.h"

const FuncInstr::ISAEntry FuncInstr::isaTable [FuncInstr::ISA_SIZE] =
{
    // name    opcode  func  format   type      s  t  d  S  C  A
    { "add    ", 0x0 , 0x20, FORMAT_R, ADD    , 2, 3, 1, 0, 0, 0},
    { "addu   ", 0x0 , 0x21, FORMAT_R, ADD    , 2, 3, 1, 0, 0, 0}, 
    { "sub    ", 0x0 , 0x22, FORMAT_R, SUB    , 2, 3, 1, 0, 0, 0},
    { "subu   ", 0x0 , 0x23, FORMAT_R, SUB    , 2, 3, 1, 0, 0, 0},
    { "addi   ", 0x8 , 0x0 , FORMAT_I, ADD    , 2, 1, 0, 0, 3, 0},
    { "addiu  ", 0x9 , 0x0 , FORMAT_I, ADD    , 2, 1, 0, 0, 3, 0},
    
    { "mult   ", 0x0 , 0x18, FORMAT_R, MUL    , 1, 2, 0, 0, 0, 0},
    { "multu  ", 0x0 , 0x19, FORMAT_R, MUL    , 1, 2, 0, 0, 0, 0},
    { "div    ", 0x0 , 0x1A, FORMAT_R, DIV    , 1, 2, 0, 0, 0, 0},
    { "divu   ", 0x0 , 0x1B, FORMAT_R, DIV    , 1, 2, 0, 0, 0, 0},
    { "mfhi   ", 0x0 , 0x10, FORMAT_R, MOVE   , 0, 0, 1, 0, 0, 0},
    { "mthi   ", 0x0 , 0x11, FORMAT_R, MOVE   , 1, 0, 0, 0, 0, 0},
    { "mflo   ", 0x0 , 0x12, FORMAT_R, MOVE   , 0, 0, 1, 0, 0, 0},
    { "mtlo   ", 0x0 , 0x13, FORMAT_R, MOVE   , 1, 0, 0, 0, 0, 0},
    
    { "sll    ", 0x0 , 0x0 , FORMAT_R, SHIFT  , 0, 2, 1, 3, 0, 0},
    { "srl    ", 0x0 , 0x2 , FORMAT_R, SHIFT  , 0, 2, 1, 3, 0, 0},
    { "sra    ", 0x0 , 0x3 , FORMAT_R, SHIFT  , 0, 2, 1, 3, 0, 0},
    { "sllv   ", 0x0 , 0x4 , FORMAT_R, SHIFT  , 3, 2, 1, 0, 0, 0},
    { "srlv   ", 0x0 , 0x6 , FORMAT_R, SHIFT  , 3, 2, 1, 0, 0, 0},
    { "srav   ", 0x0 , 0x7 , FORMAT_R, SHIFT  , 3, 2, 1, 0, 0, 0},
    { "lui    ", 0xF , 0x0 , FORMAT_I, SHIFT  , 0, 1, 0, 0, 2, 0},
    
    { "slt    ", 0x0 , 0x2A, FORMAT_R, COMP   , 1, 2, 3, 0, 0, 0},
    { "sltu   ", 0x0 , 0x2B, FORMAT_R, COMP   , 1, 2, 3, 0, 0, 0},
    { "slti   ", 0xA , 0x0 , FORMAT_I, COMP   , 1, 2, 0, 0, 3, 0},
    { "sltiu  ", 0xB , 0x0 , FORMAT_I, COMP   , 1, 2, 0, 0, 3, 0},
    
    { "and    ", 0x0 , 0x24, FORMAT_R, LOGIC  , 3, 2, 1, 0, 0, 0},
    { "or     ", 0x0 , 0x25, FORMAT_R, LOGIC  , 3, 2, 1, 0, 0, 0},
    { "xor    ", 0x0 , 0x26, FORMAT_R, LOGIC  , 3, 2, 1, 0, 0, 0},
    { "nor    ", 0x0 , 0x27, FORMAT_R, LOGIC  , 3, 2, 1, 0, 0, 0},
    { "andi   ", 0xC , 0x0 , FORMAT_I, LOGIC  , 1, 2, 0, 0, 3, 0},
    { "ori    ", 0xD , 0x0 , FORMAT_I, LOGIC  , 1, 2, 0, 0, 3, 0},
    { "xori   ", 0xE , 0x0 , FORMAT_I, LOGIC  , 1, 2, 0, 0, 3, 0},
    
    { "beq    ", 0x4 , 0x0 , FORMAT_I, COND   , 1, 2, 0, 0, 3, 0},
    { "bne    ", 0x5 , 0x0 , FORMAT_I, COND   , 1, 2, 0, 0, 3, 0},
    { "blez   ", 0x6 , 0x0 , FORMAT_I, COND   , 1, 0, 0, 0, 2, 0},
    { "bdtz   ", 0x7 , 0x0 , FORMAT_I, COND   , 1, 0, 0, 0, 2, 0},
    
    { "j      ", 0x2 , 0x0 , FORMAT_J, UNCOND , 0, 0, 0, 0, 0, 1},
    { "jal    ", 0x3 , 0x0 , FORMAT_J, UNCOND , 0, 0, 0, 0, 0, 1},
    { "jr     ", 0x0 , 0x8 , FORMAT_R, UNCOND , 1, 0, 0, 0, 0, 0},
    { "jalr   ", 0x0 , 0x9 , FORMAT_R, UNCOND , 1, 0, 0, 0, 0, 0},
    
    { "lb     ", 0x20, 0x0 , FORMAT_I, LOAD   , 3, 1, 0, 2, 0, 0},
    { "lh     ", 0x21, 0x0 , FORMAT_I, LOAD   , 3, 1, 0, 2, 0, 0},
    { "lw     ", 0x23, 0x0 , FORMAT_I, LOAD   , 3, 1, 0, 2, 0, 0},
    { "lbu    ", 0x24, 0x0 , FORMAT_I, LOAD   , 3, 1, 0, 2, 0, 0},
    { "lhu    ", 0x25, 0x0 , FORMAT_I, LOAD   , 3, 1, 0, 2, 0, 0},
    
    { "sb     ", 0x28, 0x0 , FORMAT_I, STORE  , 3, 1, 0, 2, 0, 0},
    { "sh     ", 0x29, 0x0 , FORMAT_I, STORE  , 3, 1, 0, 2, 0, 0},
    { "sw     ", 0x2B, 0x0 , FORMAT_I, STORE  , 3, 1, 0, 2, 0, 0},
    
    { "syscall", 0x0 , 0xC , FORMAT_R, SPECIAL, 0, 0, 0, 0, 0, 0},
    { "break  ", 0x0 , 0xD , FORMAT_R, SPECIAL, 0, 0, 0, 0, 0, 0},
    { "trap   ", 0x1A, 0x0 , FORMAT_J, SPECIAL, 0, 0, 0, 0, 0, 1},
};


const FuncInstr::registers FuncInstr::regTable [FuncInstr::REG_SIZE] =
{
	{"zero", ZERO},
	{"at  ", AT  },
	{"v0  ", V0  },
	{"v1  ", V1  },
	{"a0  ", A0  },
	{"a1  ", A1  },
	{"a2  ", A2  },
	{"a3  ", A3  },
	{"t0  ", T0  },
	{"t1  ", T1  },
	{"t2  ", T2  },
	{"t3  ", T3  },
	{"t4  ", T4  },
	{"t5  ", T5  },
	{"t6  ", T6  },
	{"t7  ", T7  },
	{"s0  ", S0  },
	{"s1  ", S1  },
	{"s2  ", S2  },
	{"s3  ", S3  },
	{"s4  ", S4  },
	{"s5  ", S5  },
	{"s6  ", S6  },
	{"s7  ", S7  },
	{"t8  ", T8  },
	{"t9  ", T9  },
	{"k0  ", K0  },
	{"k1  ", K1  },
	{"gp  ", GP  },
	{"sp  ", SP  },
	{"s8  ", S8  },
	{"ra  ", RA  },
};

FuncInstr::FuncInstr (uint32 bytes)
{
	dumpPos = 1;

	std::cout << "start FuncInstr" << std::endl;
	std::cout << "bytes : " << std::hex << bytes << std::endl;

    this->initFormat(bytes);
    switch (this->format)
    {
        case FORMAT_R:
        {
            this->parseR(bytes);
            break;
        }
        case FORMAT_I:
        {
            this->parseI(bytes);
            break;
        }
        case FORMAT_J:
        {
            this->parseJ(bytes);
            break;
        }
        default:
        {
        	std::cout << "default" << std::endl;

            assert(0);
        }
    }
    // ...

}

void FuncInstr::parseR (uint32 bytes)
{
	instr.raw = bytes;
	/*

	uint32 value = bytes;
	value >>= 26;
	instr.asR.opcode = bytes >> (32 - 6);

	instr.asR.s = (bytes >> (32 - 6 - 5)) & 0b11111;

	instr.asR.t = (bytes >> (32 - 6 - 5 - 5)) & 0b11111;

	instr.asR.d = (bytes >> (32 - 6 - 5 - 5)) & 0b11111;

	instr.asR.imm = (bytes >> (32 - 6 - 5 - 5 - 6)) & 0b111111;
	
	instr.asR.func = bytes & 0b111111;	
	*/

	std::cout << "type-R" << std::endl;
	std::cout << "opcode : " << std::hex << (uint32)instr.asR.opcode << std::endl;
	std::cout << "s      : " << std::hex << (uint32)instr.asR.s      << std::endl;
	std::cout << "t      : " << std::hex << (uint32)instr.asR.t      << std::endl;
	std::cout << "d      : " << std::hex << (uint32)instr.asR.d      << std::endl;
	std::cout << "imm    : " << std::hex << (uint32)instr.asR.imm    << std::endl;
	std::cout << "func   : " << std::hex << (uint32)instr.asR.func   << std::endl;
	std::cout << std::endl;
}


void FuncInstr::parseI (uint32 bytes)
{
	instr.raw = bytes;
	
	/*std::cout << "bytes is : " << std::hex << bytes << std::endl;

	instr.asI.opcode = bytes >> (32 - 6);

	instr.asI.s = (bytes >> (32 - 6 - 5)) & 0b11111;

	instr.asI.t = (bytes >> (32 - 6 - 5 - 5)) & 0b11111;
	
	instr.asI.imm = bytes  & (((uint32)1 << 16) - 1);
	
*/
	std::cout << "type-I" << std::endl;
	std::cout << "opcode : " << instr.asI.opcode << std::endl;
	std::cout << "s      : " << instr.asI.s      << std::endl;
	std::cout << "t      : " << instr.asI.t      << std::endl;
	std::cout << "imm    : " << instr.asI.imm    << std::endl;
	std::cout << std::endl;
}


void FuncInstr::parseJ (uint32 bytes)
{
	instr.raw = bytes;
	/*
	instr.asR.opcode = bytes >> 26;

	instr.asR.imm = bytes & (((uint32)1 << 26) - 1);
	*/

	std::cout << "type-J" << std::endl;
	std::cout << "opcode : " << instr.asR.opcode << std::endl;
	std::cout << "imm    : " << instr.asR.imm    << std::endl;	
	std::cout << std::endl;		
}


int FuncInstr::findInstr (uint32 bytes) const
{
	uint32 opcode = bytes >> 26;
	uint32 func   = bytes & (((uint32)1 << 6) - 1);

	std::cout << "opcode : " << std::hex << (int)opcode << std::endl;
	std::cout << "func   : " << std::hex << (int)func   << std::endl;
	std::cout << std::endl;

	
	for (int pos = 0; pos < ISA_SIZE; pos++)
	{
		if (isaTable [pos].opcode != 0x0)
		{
			//std::cout << "opcode != 0" << std::endl;
			if (opcode == isaTable [pos].opcode) 
			{
				std::cout << "pos = " << pos << " opcode = " << opcode << std::endl;
				return pos;
			}
		}
		else if (isaTable [pos].opcode == 0x0)
		{
			//std::cout << "opcode = 0" << std::endl;
			if (func   == (int)isaTable [pos].func  ) 
			{
				std::cout << "1) " << (int)isaTable [4].opcode << std::endl;
				std::cout << "2) " << (int)isaTable [4].func   << std::endl;


				std::cout << "pos = " << pos << 
							" func = " << (int)isaTable [pos].func << 
							" opcode = " <<(int) isaTable [pos].opcode << std::endl;
				return pos;
			}
		}
	}

	return -1;
}


int FuncInstr::initFormat (uint32 bytes)
{
	int instruction_pos = findInstr (bytes);

	std::cout << "instruction_pos : " << instruction_pos << std::endl;
	std::cout << "format    : " << isaTable [instruction_pos].format << std::endl;

	//return isaTable [instruction_pos].format;
	format = isaTable [instruction_pos].format;
}


int FuncInstr::nextOperandType (int isaPos) const
{
	/*switch (dumpPos)
	{
		case (uint32)isaTable [isaPos].s :
			return s_OP;
		case (uint32)isaTable [isaPos].t :
			return t_OP;
		case (uint32)isaTable [isaPos].d :
			return d_OP;
		case (uint32)isaTable [isaPos].S :
			return S_OP;
		case (uint32)isaTable [isaPos].C :
			return C_OP;
		case (uint32)isaTable [isaPos].A :
			return A_OP;
		default : 
			return -1;
	};*/

	dumpPos++;

	if      (dumpPos - 1 == (uint32)isaTable [isaPos].s) return s_OP;
	else if (dumpPos - 1 == (uint32)isaTable [isaPos].t) return t_OP;
	else if (dumpPos - 1 == (uint32)isaTable [isaPos].d) return d_OP;
	else if (dumpPos - 1 == (uint32)isaTable [isaPos].S) return S_OP;
	else if (dumpPos - 1 == (uint32)isaTable [isaPos].C) return C_OP;
	else if (dumpPos - 1 == (uint32)isaTable [isaPos].A) return A_OP;
	else 												 return -1  ;
}


uint32 FuncInstr::nextOperand (int operandType) const
{
	/*
	switch (operandType)
	{
		case s_OP :
			return instr.asR.s;
		case t_OP :
			return instr.asR.t;
		case d_OP :
			return instr.asR.d;
		case S_OP :
		{
			if (format == FORMAT_R)
				return instr.asR.S;
			else
				return instr.asI.imm;
		}
		case C_OP :
			return instr.asI.C;
		case A_OP :
			return instr.asJ.imm;
		default :
			return -1;			
	};*/
	if      (operandType == s_OP) return instr.asR.s;
	else if (operandType == t_OP) return instr.asR.t;
	else if (operandType == d_OP) return instr.asR.d;
	else if (operandType == S_OP) 
	{
		if (format == FORMAT_R)
			return instr.asR.d;
		else 
			return instr.asI.imm;
	}
	else if (operandType == C_OP) return instr.asI.imm;
	else if (operandType == A_OP) return instr.asJ.imm;
	else 						  return -1			;
}


std::string FuncInstr::Dump (std::string indent) const
{
	dumpPos = 1;

	std::ostringstream oss;

	uint32 instructionPos = findInstr (instr.raw);

	std::cout << instructionPos << std::endl;

	oss << isaTable [instructionPos].name << " ";
	std::cout << "command : " << isaTable [instructionPos].name << std::endl;

	if (format == FORMAT_R && isaTable [instructionPos].type == SPECIAL)
	{
		oss << std::endl;
		return oss.str ();
	}	

	int currOpType = -1;

	do
	{
		int currOpType = nextOperandType (instructionPos);

		std::cout << "currOpType : " << currOpType << std::endl;

		if (currOpType == -1) std::terminate ();

		if (currOpType == s_OP || currOpType == t_OP ||
			(currOpType == d_OP && format == FORMAT_R))
		{
			oss << "$" << regTable [nextOperand (currOpType)].name << " ";
			//std::cout << "$" << regTable [nextOperand (currOpType)].name << " ";
		}

		else if (currOpType != -1)
		{
			oss << "0x" << std::hex << nextOperand (currOpType) << " ";
			//std::cout<< "0x" << std::hex << nextOperand (currOpType) << " ";
		}

	} while (currOpType != -1);

	oss << std::endl;

	return oss.str ();
}
//==============================================================================
std::ostream& operator<< ( std::ostream& out, const FuncInstr& instr)
{
     out << instr.Dump("");

     return out;
}