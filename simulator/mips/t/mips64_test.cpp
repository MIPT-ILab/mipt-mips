/* MIPS Instruction unit tests
 * @author: Pavel Kryukov, Vsevolod Pukhov, Egor Bova
 * Copyright (C) MIPT-MIPS 2017-2019
 */

#include <mips/mips.h>
#include <mips/mips_instr.h>

#include <catch.hpp>
#include <memory/memory.h>

class MIPS64Instr : public BaseMIPSInstr<uint64>
{
public:
    explicit MIPS64Instr( uint32 bytes) : BaseMIPSInstr<uint64>( MIPSVersion::v64, std::endian::little, bytes, 0) { }
    explicit MIPS64Instr( std::string_view str_opcode) : BaseMIPSInstr<uint64>( MIPSVersion::v64, str_opcode, std::endian::little, 0, 0xc000) { }
    MIPS64Instr( std::string_view str_opcode, uint32 immediate) : BaseMIPSInstr<uint64>( MIPSVersion::v64, str_opcode, std::endian::little, immediate, 0xc000) { }
};

class MIPS64BEInstr : public BaseMIPSInstr<uint64>
{
public:
    explicit MIPS64BEInstr( uint32 bytes) : BaseMIPSInstr<uint64>( MIPSVersion::v64, std::endian::big, bytes, 0) { }
    explicit MIPS64BEInstr( std::string_view str_opcode) : BaseMIPSInstr<uint64>( MIPSVersion::v64, str_opcode, std::endian::big, 0, 0xc000) { }
    MIPS64BEInstr( std::string_view str_opcode, uint32 immediate) : BaseMIPSInstr<uint64>( MIPSVersion::v64, str_opcode, std::endian::big, immediate, 0xc000) { }
};

static auto get_plain_memory_with_data()
{
    auto memory = FuncMemory::create_plain_memory(15);
    memory->write<uint32, std::endian::little>( 0xABCD'1234, 0x1000);
    memory->write<uint32, std::endian::little>( 0xBADC'5678, 0x1004);
    return memory;
}

static_assert( std::is_base_of_v<MIPS64::FuncInstr, MIPS64Instr>);

TEST_CASE( "MIPS64_instr: addiu two zeroes")
{
    CHECK(MIPS64Instr(0x253104d2).get_disasm() == "addiu $s1, $t1, 1234");
    CHECK(MIPS64Instr(0x2531fb2e).get_disasm() == "addiu $s1, $t1, -1234");
    
    MIPS64Instr instr( "addiu");
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS64_instr: addiu 0 and 1")
{
    MIPS64Instr instr( "addiu", 1);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS64_instr: addiu 1 and -1")
{
    MIPS64Instr instr( "addiu", 0xffff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}
    
TEST_CASE( "MIPS64_instr: addiu overflow")
{
    MIPS64Instr instr( "addiu", 2);
    instr.set_v_src( 0x7fffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x80000001);
    CHECK( instr.has_trap() == false);
}

////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: dclo zero")
{
    CHECK(MIPS64Instr(0x71208825).get_disasm() == "dclo $s1, $t1");
    
    MIPS64Instr instr( "dclo");
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS64_instr: dclo -1")
{
    MIPS64Instr instr( "dclo");
    instr.set_v_src( 0xffffffffffffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 64);
}

TEST_CASE( "MIPS64_instr: dclo ffe002200011000a")
{
    MIPS64Instr instr( "dclo");
    instr.set_v_src( 0xffe002200011000a, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 11);
}

////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: dclz zero")
{
    CHECK(MIPS64Instr(0x71208824).get_disasm() == "dclz $s1, $t1");
    
    MIPS64Instr instr( "dclz");
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 64);
}

TEST_CASE( "MIPS64_instr: dclz -1")
{
    MIPS64Instr instr( "dclz");
    instr.set_v_src( 0xffffffffffffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS64_instr: dclz 0x02ffaa00cc720000")
{
    MIPS64Instr instr( "dclz");
    instr.set_v_src( 0x02ffaa00cc720000, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 6);
}

////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: dsllv 0xaaaaaaaafee1dead by 0")
{
    CHECK(MIPS64Instr(0x03298814).get_disasm() == "dsllv $s1, $t1, $t9");
    
    MIPS64Instr instr( "dsllv");
    instr.set_v_src( 0xaaaaaaaafee1dead, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xaaaaaaaafee1dead);
}

TEST_CASE( "MIPS64_instr: dsllv 2 by 1")
{
    MIPS64Instr instr( "dsllv");

    instr.set_v_src( 2, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 4);
}

TEST_CASE( "MIPS64_instr: dsllv 1 by 32")
{
    MIPS64Instr instr( "dsllv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 32, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x100000000);
}

TEST_CASE( "MIPS64_instr: dsllv 1 by 64 (shift-variable overflow)")
{
    MIPS64Instr instr( "dsllv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 64, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS64_instr: dsllv 1 by 128 (shift-variable overflow)")
{
    MIPS64Instr instr( "dsllv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 128, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS64_instr: dsrav 0xfeedabcd by 0")
{
    CHECK(MIPS64Instr(0x03298817).get_disasm() == "dsrav $s1, $t1, $t9");
    
    MIPS64Instr instr( "dsrav");
    instr.set_v_src( 0xfeedabcd, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xfeedabcd);
}

TEST_CASE( "MIPS64_instr: dsrav 0xab by 0xff")
{
    MIPS64Instr instr( "dsrav");
    instr.set_v_src( 0xab, 0);
    instr.set_v_src( 0xff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS64_instr: dsrav 0x123400000000 by 4")
{
    MIPS64Instr instr( "dsrav");
    instr.set_v_src( 0x123400000000, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x012340000000);
}

TEST_CASE( "MIPS64_instr: dsrav 0xffab000000000000 by 4")
{
    MIPS64Instr instr( "dsrav");
    instr.set_v_src( 0xffab000000000000, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xfffab00000000000);
}

TEST_CASE( "MIPS64_instr: dsrav 1 by 64 (shift-variable overflow)")
{
    MIPS64Instr instr( "dsrav");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 64, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS64_instr: dsrlv 0xdeadbeef by 0")
{
    CHECK(MIPS64Instr(0x03298816).get_disasm() == "dsrlv $s1, $t1, $t9");
    
    MIPS64Instr instr( "dsrlv");
    instr.set_v_src( 0xdeadbeef, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xdeadbeef);
}

TEST_CASE( "MIPS64_instr: dsrlv 1 by 1")
{
    MIPS64Instr instr( "dsrlv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS64_instr: dsrlv 0x01a00000 by 8")
{
    MIPS64Instr instr( "dsrlv");
    instr.set_v_src( 0x01a00000, 0);
    instr.set_v_src( 8, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x0001a000);
}

TEST_CASE( "MIPS64_instr: dsrlv 0x8765432000000011 by 16")
{
    MIPS64Instr instr( "dsrlv");
    instr.set_v_src( 0x8765432000000011, 0);
    instr.set_v_src( 16, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x0000876543200000);
}

TEST_CASE( "MIPS64_instr: dsrlv 1 by 64 (shift-variable overflow)")
{
    MIPS64Instr instr( "dsrlv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 64, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: ld")
{
    CHECK(MIPS64Instr(0xdd3104d2).get_disasm() == "ld $s1, 0x4d2($t1)");
    CHECK(MIPS64Instr(0xdd31fb2e).get_disasm() == "ld $s1, 0xfb2e($t1)");

    MIPS64Instr instr( "ld", 0x0fff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst( 0) == 0xBADC'5678'ABCD'1234);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: ldl")
{
    CHECK(MIPS64Instr(0x693104d2).get_disasm() == "ldl $s1, 0x4d2($t1)");
    CHECK(MIPS64Instr(0x6931fb2e).get_disasm() == "ldl $s1, 0xfb2e($t1)");

    MIPS64Instr instr( "ldl", 0x0ffd);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x0ffe);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst( 0) == 0x5678'ABCD'1234'0000);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: ldr")
{
    CHECK(MIPS64Instr(0x6d3104d2).get_disasm() == "ldr $s1, 0x4d2($t1)");
    CHECK(MIPS64Instr(0x6d31fb2e).get_disasm() == "ldr $s1, 0xfb2e($t1)");

    MIPS64Instr instr( "ldr", 0x0ffd);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x0ffe);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst( 0) == 0x5678'ABCD'1234'0000);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: sd 0xdead'beef'fee1'dead")
{
    CHECK(MIPS64Instr(0xfd3104d2).get_disasm() == "sd $s1, 0x4d2($t1)");
    CHECK(MIPS64Instr(0xfd31fb2e).get_disasm() == "sd $s1, 0xfb2e($t1)");

    MIPS64Instr instr( "sd", 0x1000);
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0xdead'beef'fee1'dead, 1);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    auto memory = get_plain_memory_with_data();
    memory->load_store( &instr);
    auto value = memory->read<uint64, std::endian::little>( 0x1000);
    CHECK( value == 0xdead'beef'fee1'dead);
}
////////////////////////////////////////////////////////////////////////////////

//Instructions sdl and sdr are not implemented
TEST_CASE( "MIPS64_instr: sdl 0xdead'beef'fee1'dead")
{
    CHECK(MIPS64Instr(0xb13104d2).get_disasm() == "sdl $s1, 0x4d2($t1)");
    CHECK(MIPS64Instr(0xb131fb2e).get_disasm() == "sdl $s1, 0xfb2e($t1)");

    MIPS64Instr instr( "sdl", 0x1000);
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0xdead'beef'fee1'dead, 1);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    auto memory = get_plain_memory_with_data();
    memory->load_store( &instr);
    auto value = memory->read<uint64, std::endian::little>( 0x1000);
    CHECK( value == 0xdead'beef'fee1'dead);
}

////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: sdr 0xdead'beef'fee1'dead")
{
    CHECK(MIPS64Instr(0xb53104d2).get_disasm() == "sdr $s1, 0x4d2($t1)");
    CHECK(MIPS64Instr(0xb531fb2e).get_disasm() == "sdr $s1, 0xfb2e($t1)");

    MIPS64Instr instr( "sdr", 0x1000);
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0xdead'beef'fee1'dead, 1);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    auto memory = get_plain_memory_with_data();
    memory->load_store( &instr);
    auto value = memory->read<uint64, std::endian::little>( 0x1000);
    CHECK( value == 0xdead'beef'fee1'dead);
}
TEST_CASE( "MIPS64_instr: nor 0 and 0")
{
    CHECK(MIPS64Instr(0x01398827).get_disasm() == "nor $s1, $t1, $t9");

    MIPS64Instr instr( "nor");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xffffffffffffffff);
}

TEST_CASE( "MIPS64_instr: nor 1 and 1")
{
    MIPS64Instr instr( "nor");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xfffffffffffffffe);
}

TEST_CASE( "MIPS64_instr: nor 1 and -1")
{
    MIPS64Instr instr( "nor");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffffffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}
////////////////////////////////////////////////////////////////////////////////


TEST_CASE ( "MIPS64_instr: sll 100 by 0")
{
    CHECK(MIPS64Instr(0x00098cc0).get_disasm() == "sll $s1, $t1, 19");

    MIPS64Instr instr( "sll", 0);
    instr.set_v_src( 100, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 100);
}

TEST_CASE ( "MIPS64_instr: sll 3 by 2")
{
    MIPS64Instr instr( "sll", 2);
    instr.set_v_src( 3, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 12);
}

TEST_CASE ( "MIPS64_instr: sll 1 by 16")
{
    MIPS64Instr instr( "sll", 16);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x10000);
}

TEST_CASE ( "MIPS64_instr: sll 1 by 31")
{
    MIPS64Instr instr( "sll", 31);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xffff'ffff'8000'0000);
}

////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: sllv by 64 (shift-variable ovreflow)")
{
    MIPS64Instr instr( "sllv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 64, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: sra 0xdeadc0dde by 0")
{
    CHECK(MIPS64Instr(0x00098cc3).get_disasm() == "sra $s1, $t1, 19");

    MIPS64Instr instr( "sra", 0);
    instr.set_v_src( 0xdeadc0de, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xffff'ffff'dead'c0de);
}

TEST_CASE( "MIPS64_instr: sra 0x0fffffff by 2")
{
    MIPS64Instr instr( "sra", 2);
    instr.set_v_src( 0x0fffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x03ffffff);
}

TEST_CASE( "MIPS64_instr: sra 0xdead by 4")
{
    MIPS64Instr instr( "sra", 4);
    instr.set_v_src( 0xdead, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x0dea);
}

TEST_CASE( "MIPS64_instr: sra 0xf1234567 by 16")
{
    MIPS64Instr instr( "sra", 16);
    instr.set_v_src( 0xf1234567, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xffff'ffff'ffff'f123);
}

////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: srav 24 by 0")
{
    CHECK(MIPS64Instr(0x03298807).get_disasm() == "srav $s1, $t1, $t9");
    
    MIPS64Instr instr( "srav");
    instr.set_v_src( 24, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 24);
}

TEST_CASE( "MIPS64_instr: srav 10 by 1")
{
    MIPS64Instr instr( "srav");
    instr.set_v_src( 10, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 5);
}

TEST_CASE( "MIPS64_instr: srav 0x000a by 4")
{
    MIPS64Instr instr( "srav");
    instr.set_v_src( 0x000a, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS64_instr: srav 0xff000000 by 4")
{
    MIPS64Instr instr( "srav");
    instr.set_v_src( 0xff000000, 0);
    instr.set_v_src( 4, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xffff'ffff'fff0'0000);
}

TEST_CASE( "MIPS64_instr: srav 0xffff0000 by 32 (shift-variable overflow)")
{
    MIPS64Instr instr( "srav");
    instr.set_v_src( 0xffff0000, 0);
    instr.set_v_src( 32, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xffff'ffff'ffff'0000);
}

TEST_CASE( "MIPS64_instr: srlv 0x11 by 0x00000a00 (shift-variable overflow)")
{
    MIPS64Instr instr( "srlv");
    instr.set_v_src( 0x11, 0);
    instr.set_v_src( 0x00000a00, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x11);
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//-------------------------MIPS64Instr unit-tests-----------------------------//

static bool not_a_mips32_instruction( std::string_view name)
{
    class MIPS32Instr : public BaseMIPSInstr<uint32>
    {
    public:
        explicit MIPS32Instr( std::string_view str_opcode) : BaseMIPSInstr<uint32>( MIPSVersion::v32, str_opcode, std::endian::little, 0, 0xc000) { }
    };
    MIPS32Instr instr( name);
    instr.execute(); 
    return instr.trap_type() == Trap::UNKNOWN_INSTRUCTION;
}

TEST_CASE ( "MIPS64_instr: dadd two zeroes")
{
    CHECK(MIPS64Instr(0x0139882C).get_disasm() == "dadd $s1, $t1, $t9");
    CHECK( not_a_mips32_instruction("dadd"));
    
    MIPS64Instr instr( "dadd");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE ( "MIPS64_instr: dadd 0 and 1")
{
    MIPS64Instr instr( "dadd");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE ( "MIPS64_instr: dadd 1 and -1")
{
    MIPS64Instr instr( "dadd");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffffffffffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}


TEST_CASE ( "MIPS64_instr: dadd overflow")
{
    MIPS64Instr instr( "dadd");
    instr.set_v_src( 0x7fffffffffffffff, 0);
    instr.set_v_src( 0x7fffffffffffffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == NO_VAL32);
    CHECK( instr.trap_type() != Trap::NO_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: daddi two zeroes")
{
    CHECK(MIPS64Instr(0x613104d2).get_disasm() == "daddi $s1, $t1, 1234");
    CHECK(MIPS64Instr(0x6131fb2e).get_disasm() == "daddi $s1, $t1, -1234");
    CHECK( not_a_mips32_instruction("daddi"));

    MIPS64Instr instr( "daddi", 0);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS64_instr: daddi 0 and 1")
{
    MIPS64Instr instr( "daddi", 1);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS64_instr: daddi 1 and -1")
{
    MIPS64Instr instr( "daddi", 0xffff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}
    

TEST_CASE( "MIPS64_instr: daddi overflow")
{
    MIPS64Instr instr( "daddi", 1);
    instr.set_v_src( 0x7fffffffffffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == NO_VAL32);
    CHECK( instr.trap_type() != Trap::NO_TRAP);
}
////////////////////////////////////////////////////////////////////////////////
    
TEST_CASE( "MIPS64_instr: daddiu two zeroes")
{
    CHECK(MIPS64Instr(0x653104d2).get_disasm() == "daddiu $s1, $t1, 1234");
    CHECK(MIPS64Instr(0x6531fb2e).get_disasm() == "daddiu $s1, $t1, -1234");
    CHECK( not_a_mips32_instruction("daddiu"));

    MIPS64Instr instr( "daddiu", 0);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS64_instr: daddiu 0 and 1")
{
    MIPS64Instr instr( "daddiu", 1);
    instr.set_v_src( 0, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS64_instr: daddiu 1 and -1")
{
    MIPS64Instr instr( "daddiu", 0xffff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}


TEST_CASE( "MIPS64_instr: daddiu overflow")
{
    MIPS64Instr instr( "daddiu", 1);
    instr.set_v_src( 0x7fffffffffffffff, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x8000000000000000);
    CHECK( instr.trap_type() == Trap::NO_TRAP);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE ( "MIPS64_instr: daddu two zeroes")
{
    CHECK(MIPS64Instr(0x0139882D).get_disasm() == "daddu $s1, $t1, $t9");
    CHECK( not_a_mips32_instruction("daddu"));

    MIPS64Instr instr( "daddu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE ( "MIPS64_instr: daddu 0 and 1")
{
    MIPS64Instr instr( "daddu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE ( "MIPS64_instr: daddu 1 and -1")
{
    MIPS64Instr instr( "daddu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0);
}

TEST_CASE ( "MIPS64_instr: daddu overflow")
{
    MIPS64Instr instr( "daddu");
    instr.set_v_src( 0x7fff'ffff'ffff'ffff, 0);
    instr.set_v_src( 0x7fff'ffff'ffff'ffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xffff'ffff'ffff'fffe);
    CHECK( instr.has_trap() == false);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: ddiv 1 by 1")
{
    CHECK(MIPS64Instr(0x0229001e).get_disasm() == "ddiv $s1, $t1");
    CHECK(MIPS64Instr(0x0229001e).is_divmult());
    CHECK(MIPS64Instr( "ddiv").is_divmult());
    CHECK( not_a_mips32_instruction( "ddiv"));

    MIPS64Instr instr( "ddiv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS64_instr: ddiv -1 by 1")
{
    MIPS64Instr instr( "ddiv");
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 0xffff'ffff'ffff'ffff);
}

TEST_CASE( "MIPS64_instr: ddiv -1 by -1")
{
    MIPS64Instr instr( "ddiv");
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 0);
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 1);
}

TEST_CASE( "MIPS64_instr: ddiv 1 by -1")
{
    MIPS64Instr instr( "ddiv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 0xffff'ffff'ffff'ffff);
}

TEST_CASE( "MIPS64_instr: ddiv 0 by 1")
{
    MIPS64Instr instr( "ddiv");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 0);
}

TEST_CASE( "MIPS64_instr: ddiv 1 by 0")
{
    MIPS64Instr instr( "ddiv");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 0);
}

TEST_CASE( "MIPS64_instr: ddiv 0x8000'0000'0000'0000 by -1")
{
    MIPS64Instr instr( "ddiv");
    instr.set_v_src( 0x8000'0000'0000'0000, 0);
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 0);
}

TEST_CASE( "MIPS64_instr: ddiv 0x4c4b'4000'0000'0000 by 0x1dcd'6500'0000'0000")
{
    MIPS64Instr instr( "ddiv");
    instr.set_v_src( 0x4c4b'4000'0000'0000, 0);
    instr.set_v_src( 0x1dcd'6500'0000'0000, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0x10b0'7600'0000'0000);
    CHECK( instr.get_v_dst( 0)  == 2);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: ddivu 1 by 1")
{
    CHECK(MIPS64Instr(0x0229001f).get_disasm() == "ddivu $s1, $t1");
    CHECK(MIPS64Instr(0x0229001f).is_divmult());
    CHECK(MIPS64Instr( "ddivu").is_divmult());
    CHECK( not_a_mips32_instruction( "ddivu"));

    MIPS64Instr instr( "ddivu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 1);
}

TEST_CASE( "MIPS64_instr: ddivu -1 by 1")
{
    MIPS64Instr instr( "ddivu");
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 0xffff'ffff'ffff'ffff);
}

TEST_CASE( "MIPS64_instr: ddivu -1 by -1")
{
    MIPS64Instr instr( "ddivu");
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 0);
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 1);
}

TEST_CASE( "MIPS64_instr: ddivu 1 by -1")
{
    MIPS64Instr instr( "ddivu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 1);
    CHECK( instr.get_v_dst( 0)  == 0);
}

TEST_CASE( "MIPS64_instr: ddivu 0 by 1")
{
    MIPS64Instr instr( "ddivu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 0);
}

TEST_CASE( "MIPS64_instr: ddivu 1 by 0")
{
    MIPS64Instr instr( "ddivu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 0);
}

TEST_CASE( "MIPS64_instr: ddivu 0x8000'0000'0000'0000 by -1")
{
    MIPS64Instr instr( "ddivu");
    instr.set_v_src( 0x8000'0000'0000'0000, 0);
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0x8000'0000'0000'0000);
    CHECK( instr.get_v_dst( 0)  == 0);
}

TEST_CASE( "MIPS64_instr: ddivu 0x4c4b'4000'0000'0000 by 0x1dcd'6500'0000'0000")
{
    MIPS64Instr instr( "ddivu");
    instr.set_v_src( 0x4c4b'4000'0000'0000, 0);
    instr.set_v_src( 0x1dcd'6500'0000'0000, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0x10b0'7600'0000'0000);
    CHECK( instr.get_v_dst( 0)  == 2);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: dmult 0 by 0")
{
    CHECK(MIPS64Instr(0x0229001c).get_disasm() == "dmult $s1, $t1");
    CHECK(MIPS64Instr(0x0229001c).is_divmult());
    CHECK(MIPS64Instr("dmult").is_divmult());
    CHECK( not_a_mips32_instruction("dmult"));

    MIPS64Instr instr( "dmult");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 0);
}

TEST_CASE( "MIPS64_instr: dmult 1 by 1")
{
    MIPS64Instr instr( "dmult");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 1);
}

TEST_CASE( "MIPS64_instr: dmult -1 by -1")
{
    MIPS64Instr instr( "dmult");
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 0);
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 1);
}

TEST_CASE( "MIPS64_instr: dmult -1 by 1")
{
    MIPS64Instr instr( "dmult");
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0xffff'ffff'ffff'ffff);
    CHECK( instr.get_v_dst( 0)  == 0xffff'ffff'ffff'ffff);
}

TEST_CASE( "MIPS64_instr: dmult 0x100000000 by 0x100000000")
{
    MIPS64Instr instr( "dmult");
    instr.set_v_src( 0x100000000, 0);
    instr.set_v_src( 0x100000000, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 1);
    CHECK( instr.get_v_dst( 0)  == 0);
}

TEST_CASE( "MIPS64_instr: dmult 0x8000'0000'0000'0000 by 0x8000'0000'0000'0000")
{
    MIPS64Instr instr( "dmult");
    instr.set_v_src( 0x8000'0000'0000'0000, 0);
    instr.set_v_src( 0x8000'0000'0000'0000, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0x4000'0000'0000'0000);
    CHECK( instr.get_v_dst( 0)  == 0);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: dmultu 0 by 0")
{
    CHECK(MIPS64Instr(0x0229001d).get_disasm() == "dmultu $s1, $t1");
    CHECK(MIPS64Instr(0x0229001d).is_divmult());
    CHECK(MIPS64Instr("dmultu").is_divmult());
    CHECK( not_a_mips32_instruction("dmultu"));

    MIPS64Instr instr( "dmultu");
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 0);
}

TEST_CASE( "MIPS64_instr: dmultu 1 by 1")
{
    MIPS64Instr instr( "dmultu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 1);
}

TEST_CASE( "MIPS64_instr: dmultu -1 by -1")
{
    MIPS64Instr instr( "dmultu");
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 0);
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0xffff'ffff'ffff'fffe);
    CHECK( instr.get_v_dst( 0)  == 1);
}

TEST_CASE( "MIPS64_instr: dmultu -1 by 0")
{
    MIPS64Instr instr( "dmultu");
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 0);
}

TEST_CASE( "MIPS64_instr: dmultu -1 by 1")
{
    MIPS64Instr instr( "dmultu");
    instr.set_v_src( 0xffff'ffff'ffff'ffff, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0);
    CHECK( instr.get_v_dst( 0)  == 0xffff'ffff'ffff'ffff);
}

TEST_CASE( "MIPS64_instr: dmultu 0x100000000 by 0x100000000")
{
    MIPS64Instr instr( "dmultu");
    instr.set_v_src( 0x100000000, 0);
    instr.set_v_src( 0x100000000, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 1);
    CHECK( instr.get_v_dst( 0)  == 0);
}

TEST_CASE( "MIPS64_instr: dmultu 0x8000'0000'0000'0000 by 0x8000'0000'0000'0000")
{
    MIPS64Instr instr( "dmultu");
    instr.set_v_src( 0x8000'0000'0000'0000, 0);
    instr.set_v_src( 0x8000'0000'0000'0000, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0x4000'0000'0000'0000);
    CHECK( instr.get_v_dst( 0)  == 0);
}

TEST_CASE( "MIPS64_instr: dmultu 0xcecb'8f27'0000'0000 by 0xfd87'b5f2'0000'0000")
{
    MIPS64Instr instr( "dmultu");
    instr.set_v_src( 0xcecb'8f27'0000'0000, 0);
    instr.set_v_src( 0xfd87'b5f2'0000'0000, 1);
    instr.execute();
    CHECK( instr.get_v_dst( 1) == 0xcccc'cccb'7134'e5de);
    CHECK( instr.get_v_dst( 0)  == 0);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE ( "MIPS64_instr: dsll 0xaaaa'aaaa'0009'8cc0 by 0")
{
    CHECK(MIPS64Instr(0x00098cf8).get_disasm() == "dsll $s1, $t1, 19");
    CHECK( not_a_mips32_instruction("dsll"));

    MIPS64Instr instr( "dsll", 0);
    instr.set_v_src( 0xaaaa'aaaa'0009'8cc0, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xaaaa'aaaa'0009'8cc0);
}

TEST_CASE ( "MIPS64_instr: dsll 51 by 1")
{
    MIPS64Instr instr( "dsll", 1);
    instr.set_v_src( 51, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 102);
}

TEST_CASE ( "MIPS64_instr: dsll 0x8899'aabb'ccdd'eeff by 8")
{
    MIPS64Instr instr( "dsll", 8);
    instr.set_v_src( 0x8899'aabb'ccdd'eeff, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x99aa'bbcc'ddee'ff00);
}

TEST_CASE ( "MIPS64_instr: dsll 1 by 63")
{
    MIPS64Instr instr( "dsll", 63);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x8000'0000'0000'0000);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: dsll32 0xaaaa'aaaa'0009'8ccf by 0")
{
    CHECK(MIPS64Instr(0x00098cfc).get_disasm() == "dsll32 $s1, $t1, 19");
    CHECK( not_a_mips32_instruction("dsll32"));

    MIPS64Instr instr( "dsll32", 0);
    instr.set_v_src( 0xaaaa'aaaa'0009'8ccf, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x0009'8ccf'0000'0000);
}

TEST_CASE ( "MIPS64_instr: dsll32 0x8899'aabb'ccdd'eeff by 8")
{
    MIPS64Instr instr( "dsll32", 8);
    instr.set_v_src( 0x8899'aabb'ccdd'eeff, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xddee'ff00'0000'0000);
}

TEST_CASE( "MIPS64_instr: dsll32 1 by 31")
{
    MIPS64Instr instr( "dsll32", 31);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x8000'0000'0000'0000);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: dsra 0xabcd'1234'abcd'1234 by 0")
{
    CHECK(MIPS64Instr(0x00098cfb).get_disasm() == "dsra $s1, $t1, 19");
    CHECK( not_a_mips32_instruction("dsra"));
    
    MIPS64Instr instr( "dsra", 0);
    instr.set_v_src( 0xabcd'1234'abcd'1234, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xabcd'1234'abcd'1234);
}

TEST_CASE( "MIPS64_instr: dsra 49 by 1")
{
    MIPS64Instr instr( "dsra", 1);
    instr.set_v_src( 49, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 24);
}

TEST_CASE( "MIPS64_instr: dsra 0x1000 by 4")
{
    MIPS64Instr instr( "dsra", 4);
    instr.set_v_src( 0x1000, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x100);
}

TEST_CASE( "MIPS64_instr: dsra 0xffa0'0000'0000'0000 by 16")
{
    MIPS64Instr instr( "dsra", 16);
    instr.set_v_src( 0xffa0'0000'0000'0000, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xffff'ffa0'0000'0000);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: dsra32 0xabcd'1234'abcd'1234 by 0")
{
    CHECK(MIPS64Instr(0x00098cff).get_disasm() == "dsra32 $s1, $t1, 19");
    CHECK( not_a_mips32_instruction("dsra32"));
    
    MIPS64Instr instr( "dsra32", 0);
    instr.set_v_src( 0xabcd'1234'abcd'1234, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xffff'ffff'abcd'1234);
}

TEST_CASE( "MIPS64_instr: dsra32 0x1000'0000'0000 by 4")
{
    MIPS64Instr instr( "dsra32", 4);
    instr.set_v_src( 0x1000'0000'0000, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x100);
}

TEST_CASE( "MIPS64_instr: dsra32 0xffa0'0000'0000'0000 by 16")
{
    MIPS64Instr instr( "dsra32", 16);
    instr.set_v_src( 0xffa0'0000'0000'0000, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xffff'ffff'ffff'ffa0);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: dsrl 0xabcd'1234'abcd'1234 by 0")
{
    CHECK(MIPS64Instr(0x00098cfa).get_disasm() == "dsrl $s1, $t1, 19");
    CHECK( not_a_mips32_instruction("dsrl"));
    
    MIPS64Instr instr( "dsrl", 0);
    instr.set_v_src( 0xabcd'1234'abcd'1234, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xabcd'1234'abcd'1234);
}

TEST_CASE( "MIPS64_instr: dsrl 49 by 1")
{
    MIPS64Instr instr( "dsrl", 1);
    instr.set_v_src( 49, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 24);
}

TEST_CASE( "MIPS64_instr: dsrl 0x1000 by 4")
{
    MIPS64Instr instr( "dsrl", 4);
    instr.set_v_src( 0x1000, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x100);
}

TEST_CASE( "MIPS64_instr: dsrl 0xffa0'0000'0000'0000 by 16")
{
    MIPS64Instr instr( "dsrl", 16);
    instr.set_v_src( 0xffa0'0000'0000'0000, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xffa0'0000'0000);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: dsrl32 0xabcd'1234'abcd'1234 by 0")
{
    CHECK(MIPS64Instr(0x00098cfe).get_disasm() == "dsrl32 $s1, $t1, 19");
    CHECK( not_a_mips32_instruction("dsrl32"));
    
    MIPS64Instr instr( "dsrl32", 0);
    instr.set_v_src( 0xabcd'1234'abcd'1234, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xabcd1234);
}

TEST_CASE( "MIPS64_instr: dsrl32 0x1000'0000'0000 by 4")
{
    MIPS64Instr instr( "dsrl32", 4);
    instr.set_v_src( 0x1000'0000'0000, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0x100);
}

TEST_CASE( "MIPS64_instr: dsrl32 0xffa0'0000'0000'0000 by 16")
{
    MIPS64Instr instr( "dsrl32", 16);
    instr.set_v_src( 0xffa0'0000'0000'0000, 0);
    instr.execute();
    CHECK( instr.get_v_dst( 0) == 0xffa0);
}

////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: dsub 1 from 1")
{
    CHECK(MIPS64Instr(0x0139882e).get_disasm() == "dsub $s1, $t1, $t9");
    CHECK( not_a_mips32_instruction("dsub"));

    MIPS64Instr instr( "dsub");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK(instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS64_instr: dsub 1 from 10")
{
    MIPS64Instr instr( "dsub");
    instr.set_v_src( 10, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK(instr.get_v_dst( 0) == 9);
}

TEST_CASE( "MIPS64_instr: dsub 0 from 1")
{
    MIPS64Instr instr( "dsub");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK(instr.get_v_dst( 0) == 1);
}

TEST_CASE( "MIPS64_instr: dsub overflow")
{
    MIPS64Instr instr( "dsub");
    instr.set_v_src( 0x8000000000000000, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK(instr.get_v_dst( 0) == NO_VAL32);
    CHECK(instr.trap_type() == Trap::INTEGER_OVERFLOW);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS64_instr: dsubu 1 from 1")
{
    CHECK(MIPS64Instr(0x0139882f).get_disasm() == "dsubu $s1, $t1, $t9");
    CHECK( not_a_mips32_instruction("dsubu"));

    MIPS64Instr instr( "dsubu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK(instr.get_v_dst( 0) == 0);
}

TEST_CASE( "MIPS64_instr: dsubu 1 from 10")
{ 
    MIPS64Instr instr( "dsubu");
    instr.set_v_src( 10, 0);
    instr.set_v_src( 1, 1);
    instr.execute();
    CHECK(instr.get_v_dst( 0) == 9);
}

TEST_CASE( "MIPS64_instr: dsubu 0 from 1")
{   
    MIPS64Instr instr( "dsubu");
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0, 1);
    instr.execute();
    CHECK(instr.get_v_dst( 0) == 1);
}
////////////////////////////////////////////////////////////////////////////////
