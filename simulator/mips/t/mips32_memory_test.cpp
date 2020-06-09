/* MIPS Instruction unit tests
 * @author: Pavel Kryukov, Vsevolod Pukhov, Egor Bova
 * Copyright (C) MIPT-MIPS 2017-2019
 */

#include "mips32_test.h"

#include <catch.hpp>
#include <memory/memory.h>


static auto get_plain_memory_with_data()
{
    auto memory = FuncMemory::create_plain_memory(15);
    memory->write<uint32, std::endian::little>( 0xABCD'1234, 0x1000);
    memory->write<uint32, std::endian::little>( 0xBADC'5678, 0x1004);
    return memory;
}

TEST_CASE( "MIPS32_instr: lb (most significant bit is 0)")
{
    CHECK(MIPS32Instr(0x813104d2).get_disasm() == "lb $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0x8131fb2e).get_disasm() == "lb $s1, 0xfb2e($t1)");

    MIPS32Instr instr( "lb", 0x0fff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst( 0) == 0x34);
}

TEST_CASE( "MIPS32_instr: lb (most significant bit is 1)")
{
    MIPS32Instr instr( "lb", 0x0fff);
    instr.set_v_src( 4, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1003);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst( 0) == 0xffffffab);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: lbu (most significant bit is 0)")
{
    CHECK(MIPS32Instr(0x913104d2).get_disasm() == "lbu $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0x9131fb2e).get_disasm() == "lbu $s1, 0xfb2e($t1)");

    MIPS32Instr instr( "lbu", 0x0fff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst( 0) == 0x34);
}

TEST_CASE( "MIPS32_instr: lbu (most significant bit is 1)")
{
    MIPS32Instr instr( "lbu", 0x0fff);
    instr.set_v_src( 4, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1003);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst( 0) == 0xab);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: lh (most significant bit is 0)")
{
    CHECK(MIPS32Instr(0x853104d2).get_disasm() == "lh $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0x8531fb2e).get_disasm() == "lh $s1, 0xfb2e($t1)");

    MIPS32Instr instr( "lh", 0x0fff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst( 0) == 0x1234);
}

TEST_CASE( "MIPS32_instr: lh (most significant bit is 1)")
{
    MIPS32Instr instr( "lh", 0x0fff);
    instr.set_v_src( 3, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1002);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst( 0) == 0xffff'ABCD);
}

TEST_CASE( "MIPS32_instr: lh unaligned address trap")
{
    MIPS32Instr instr( "lh", 0);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 1);
    CHECK( instr.trap_type() == Trap::UNALIGNED_LOAD);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: lhu (most significant bit is 0)")
{
    CHECK(MIPS32Instr(0x953104d2).get_disasm() == "lhu $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0x9531fb2e).get_disasm() == "lhu $s1, 0xfb2e($t1)");

    MIPS32Instr instr( "lhu", 0x0fff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst( 0) == 0x1234);
}

TEST_CASE( "MIPS32_instr: lhu (most significant bit is 1)")
{
    MIPS32Instr instr( "lhu", 0x0fff);
    instr.set_v_src( 3, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1002);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst( 0) == 0xABCD);
}

TEST_CASE( "MIPS32_instr: lhu unaligned address trap")
{
    MIPS32Instr instr( "lhu", 0);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 1);
    CHECK( instr.trap_type() == Trap::UNALIGNED_LOAD);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: lwl (instr->mem_addr % 4 = 0)")
{
    CHECK(MIPS32Instr(0x893104d2).get_disasm() == "lwl $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0x8931fb2e).get_disasm() == "lwl $s1, 0xfb2e($t1)");

    MIPS32Instr instr( "lwl", 0x0fff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x0ffd);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst( 0) == 0x3400'0000);
}

TEST_CASE( "MIPS32_instr: lwl (instr->mem_addr % 4 = 3)")
{
    MIPS32Instr instr( "lwl", 0x0fff);
    instr.set_v_src( 4, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst( 0) == 0xABCD'1234);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: lwr (instr->mem_addr % 4 = 0)")
{
    CHECK(MIPS32Instr(0x993104d2).get_disasm() == "lwr $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0x9931fb2e).get_disasm() == "lwr $s1, 0xfb2e($t1)");

    MIPS32Instr instr( "lwr", 0x0fff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst( 0) == 0xABCD'1234);
}

TEST_CASE( "MIPS32_instr: lwr (instr->mem_addr % 4 = 3)")
{
    MIPS32Instr instr( "lwr", 0x0fff);
    instr.set_v_src( 4, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1003);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst( 0) == 0xDC56'78AB);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: ll (most significant bit is 0)")
{
    CHECK(MIPS32Instr(0xc13104d2).get_disasm() == "ll $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0xc131fb2e).get_disasm() == "ll $s1, 0xfb2e($t1)");

    MIPS32Instr instr( "ll", 0x0fff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst( 0) == 0x1234);
}

TEST_CASE( "MIPS32_instr: ll (most significant bit is 1)")
{
    MIPS32Instr instr( "ll", 0x0fff);
    instr.set_v_src( 3, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1002);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst( 0) == 0xffff'ABCD);
}

////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: lw le")
{
    CHECK(MIPS32Instr(0x8d3104d2).get_disasm() == "lw $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0x8d31fb2e).get_disasm() == "lw $s1, 0xfb2e($t1)");

    MIPS32Instr instr( "lw", 0x0fff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst( 0) == 0xABCD'1234);
}

TEST_CASE( "MIPS32_instr: lw be")
{
    CHECK(MIPS32BEInstr(0x8d3104d2).get_disasm() == "lw $s1, 0x4d2($t1)");
    CHECK(MIPS32BEInstr(0x8d31fb2e).get_disasm() == "lw $s1, 0xfb2e($t1)");

    MIPS32BEInstr instr( "lw", 0x0fff);
    instr.set_v_src( 1, 0);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.get_v_dst( 0) == 0x3412'CDAB);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: sb 0x12")
{
    CHECK(MIPS32Instr(0xa13104d2).get_disasm() == "sb $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0xa131fb2e).get_disasm() == "sb $s1, 0xfb2e($t1)");

    MIPS32Instr instr( "sb", 0x1000);
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0x12, 1);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    auto memory = get_plain_memory_with_data();
    memory->load_store( &instr);
    auto value = memory->read<uint8, std::endian::little>( 0x1000);
    CHECK( value == 0x12);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: sh 0xdead")
{
    CHECK(MIPS32Instr(0xa53104d2).get_disasm() == "sh $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0xa531fb2e).get_disasm() == "sh $s1, 0xfb2e($t1)");

    MIPS32Instr instr( "sh", 0x1000);
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0xdead, 1);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    auto memory = get_plain_memory_with_data();
    memory->load_store( &instr);
    auto value = memory->read<uint16, std::endian::little>( 0x1000);
    CHECK( value == 0xdead);
}

TEST_CASE( "MIPS32_instr: sh be 0xdead")
{
    MIPS32BEInstr instr( "sh", 0x1000);
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0xdead, 1);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    auto memory = get_plain_memory_with_data();
    memory->load_store( &instr);
    auto value = memory->read<uint16, std::endian::big>( 0x1000);
    CHECK( value == 0xdead);
}

TEST_CASE( "MIPS32_instr: sw 0xfee1'dead")
{
    CHECK(MIPS32Instr(0xad3104d2).get_disasm() == "sw $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0xad31fb2e).get_disasm() == "sw $s1, 0xfb2e($t1)");

    MIPS32Instr instr( "sw", 0x1000);
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0xfee1'dead, 1);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    auto memory = get_plain_memory_with_data();
    memory->load_store( &instr);
    auto value = memory->read<uint32, std::endian::little>( 0x1000);
    CHECK( value == 0xfee1'dead);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: sw be 0xfee1'dead")
{
    CHECK(MIPS32Instr(0xad3104d2).get_disasm() == "sw $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0xad31fb2e).get_disasm() == "sw $s1, 0xfb2e($t1)");

    MIPS32BEInstr instr( "sw", 0x1000);
    instr.set_v_src( 0, 0);
    instr.set_v_src( 0xfee1'dead, 1);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1000);

    auto memory = get_plain_memory_with_data();
    memory->load_store( &instr);
    auto value = memory->read<uint32, std::endian::big>( 0x1000);
    CHECK( value == 0xfee1'dead);
}

TEST_CASE( "MIPS32_instr: swl (instr->mem_addr % 4 = 2)")
{
    CHECK(MIPS32Instr(0xa93104d2).get_disasm() == "swl $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0xa931fb2e).get_disasm() == "swl $s1, 0xfb2e($t1)");

    MIPS32Instr instr( "swl", 0x1003);
    instr.set_v_src( 2, 0);
    instr.set_v_src( 0xfee1'dead, 1);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1002);

    auto memory = get_plain_memory_with_data();
    memory->load_store( &instr);
    auto value = memory->read<uint32, std::endian::little>( 0x1002);
    CHECK( value == 0xfee1'abcd);
}
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "MIPS32_instr: swr (instr->mem_addr % 4 = 2)")
{
    CHECK(MIPS32Instr(0xb93104d2).get_disasm() == "swr $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0xb931fb2e).get_disasm() == "swr $s1, 0xfb2e($t1)");

    MIPS32Instr instr( "swr", 0x1000);
    instr.set_v_src( 2, 0);
    instr.set_v_src( 0xfee1'dead, 1);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1002);

    auto memory = get_plain_memory_with_data();
    memory->load_store( &instr);
    auto value = memory->read<uint32, std::endian::little>( 0x1002);
    CHECK( value == 0x5678'dead);
}
////////////////////////////////////////////////////////////////////////////////

//Data cache model is not implemeted so sc is tested like sh
TEST_CASE( "MIPS32_instr: sc 0xdead")
{
    CHECK(MIPS32Instr(0xe13104d2).get_disasm() == "sc $s1, 0x4d2($t1)");
    CHECK(MIPS32Instr(0xe131fb2e).get_disasm() == "sc $s1, 0xfb2e($t1)");

    MIPS32Instr instr( "sc", 0x1000);
    instr.set_v_src( 1, 0);
    instr.set_v_src( 0xdead, 1);
    instr.execute();
    CHECK( instr.get_mem_addr() == 0x1001);

    auto memory = get_plain_memory_with_data();
    memory->load_store( &instr);
    auto value = memory->read<uint32, std::endian::little>( 0x1001);
    CHECK( value == 0x78ab'dead);
}

TEST_CASE( "MIPS32_instr: load dump")
{
    MIPS32Instr instr(0x8d310ff0);
    CHECK( instr.get_disasm() == "lw $s1, 0xff0($t1)");
    CHECK( instr.bytes_dump() == "Bytes: 0xf0 0x0f 0x31 0x8d");

    instr.set_v_src( 0x10, 0);
    instr.set_sequence_id( 0);
    instr.execute();
    CHECK( instr.string_dump() == "{0}\tlw $s1, 0xff0($t1)\t [ $ma = 0x1000 ]");

    get_plain_memory_with_data()->load_store( &instr);
    CHECK( instr.string_dump() == "{0}\tlw $s1, 0xff0($t1)\t [ $ma = 0x1000, $s1 = 0xabcd1234 ]" );
}

TEST_CASE( "MIPS32_instr: load dump with trap")
{
    MIPS32Instr instr(0x8d3104d0);
    CHECK( instr.get_disasm() == "lw $s1, 0x4d0($t1)");

    instr.set_v_src( 0x1, 0);
    instr.set_sequence_id( 0);
    instr.execute();
    CHECK( instr.string_dump() == "{0}\tlw $s1, 0x4d0($t1)\t [ $ma = 0x4d1 ]\t UNALIGNED_LOAD");
}
