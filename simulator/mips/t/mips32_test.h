/* MIPS Instruction unit tests
 * @author: Pavel Kryukov, Vsevolod Pukhov, Egor Bova
 * Copyright (C) MIPT-MIPS 2017-2019
 */

#include <mips/mips.h>
#include <mips/mips_instr.h>

class MIPS32Instr : public BaseMIPSInstr<uint32>
{
public:
    explicit MIPS32Instr( uint32 bytes) : BaseMIPSInstr<uint32>( MIPSVersion::v32, std::endian::little, bytes, 0) { }
    explicit MIPS32Instr( std::string_view str_opcode) : BaseMIPSInstr<uint32>( MIPSVersion::v32, str_opcode, std::endian::little, 0, 0xc000) { }
    MIPS32Instr( std::string_view str_opcode, uint32 immediate) : BaseMIPSInstr<uint32>( MIPSVersion::v32, str_opcode, std::endian::little, immediate, 0xc000) { }
};

class MIPS32BEInstr : public BaseMIPSInstr<uint32>
{
public:
    explicit MIPS32BEInstr( uint32 bytes) : BaseMIPSInstr<uint32>( MIPSVersion::v32, std::endian::big, bytes, 0) { }
    explicit MIPS32BEInstr( std::string_view str_opcode) : BaseMIPSInstr<uint32>( MIPSVersion::v32, str_opcode, std::endian::big, 0, 0xc000) { }
    MIPS32BEInstr( std::string_view str_opcode, uint32 immediate) : BaseMIPSInstr<uint32>( MIPSVersion::v32, str_opcode, std::endian::big, immediate, 0xc000) { }
};

static_assert( std::is_base_of_v<MIPS32::FuncInstr, MIPS32Instr>);
