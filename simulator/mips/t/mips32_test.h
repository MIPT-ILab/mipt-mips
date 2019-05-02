/* MIPS Instruction unit tests
 * @author: Pavel Kryukov, Vsevolod Pukhov, Egor Bova
 * Copyright (C) MIPT-MIPS 2017-2019
 */

#include "../mips.h"
#include "../mips_instr.h"

class MIPS32Instr : public BaseMIPSInstr<uint32>
{
public:
    explicit MIPS32Instr( uint32 bytes, Addr pc = 0x0) : BaseMIPSInstr<uint32>( MIPSVersion::v32, Endian::little, bytes, pc) { }
    explicit MIPS32Instr( std::string_view str_opcode, uint32 immediate = 0) : BaseMIPSInstr<uint32>( MIPSVersion::v32, str_opcode, Endian::little, immediate, 0xc000) { }
};

class MIPS32BEInstr : public BaseMIPSInstr<uint32>
{
public:
    explicit MIPS32BEInstr( uint32 bytes, Addr pc = 0x0) : BaseMIPSInstr<uint32>( MIPSVersion::v32, Endian::big, bytes, pc) { }
    explicit MIPS32BEInstr( std::string_view str_opcode, uint32 immediate = 0) : BaseMIPSInstr<uint32>( MIPSVersion::v32, str_opcode, Endian::big, immediate, 0xc000) { }
};

static_assert( std::is_base_of_v<MIPS32::FuncInstr, MIPS32Instr>);