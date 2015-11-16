# include "../../common/types.h"
# include <iostream>

class FuncInstr
{
	enum FormatType
    {
        FORMAT_R,
        FORMAT_I,
        FORMAT_J
    } format;
    enum Reg
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
    	RA
    };
    enum Type
    {
    	ADD = 0,
    	SUB,
    	MUL,
    	DIV,
    	MOVE,
    	SHIFT,
    	COMP,
    	LOGIC,
    	COND,
    	UNCOND,
    	LOAD,
    	STORE,
    	SPECIAL
    } type;

    union
    {
        struct
        {
            unsigned imm:16;
            unsigned t:5;
            unsigned s:5;
            unsigned opcode:6;
        } asI;
        struct
        {
        	unsigned func:6;
            unsigned imm:5;
            unsigned d:5;
            unsigned t:5;
            unsigned s:5;
            unsigned opcode:6; 
        } asR;
        struct
        {
            unsigned imm:26;
            unsigned opcode:6;
        } asJ;
        uint32 raw;
    } instr;//bytes;

    //bytes instr;

    struct ISAEntry
    {
        const char name [10];

        uint8 opcode;
        uint8 func;

        FuncInstr::FormatType format;
        FuncInstr::Type type;

        uint8 s;
        uint8 t;
        uint8 d;
        uint8 S;
        uint8 C;
        uint8 A;
    };

    struct registers
    {
    	const char name [5];
    	uint8 reg_num;
    };

    static const int ISA_SIZE = 51;
    static const ISAEntry isaTable [ISA_SIZE];

    static const int REG_SIZE = 32;
    static const registers regTable [REG_SIZE];

    mutable int dumpPos;

	//FuncInstr (){}

	void parseR (uint32 bytes);
	void parseJ (uint32 bytes);
	void parseI (uint32 bytes);

	int findInstr (uint32 bytes) const;

	int initFormat (uint32 bytes);

	enum opType
	{
		s_OP = 0,
		t_OP,
		d_OP,
		S_OP,
		C_OP,
		A_OP
	} optype;

	int    nextOperandType (int isaPos     ) const;
    uint32 nextOperand     (int operandType) const;
    
    public:
        FuncInstr( uint32 bytes);
        std::string Dump( std::string indent = " ") const;


};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);