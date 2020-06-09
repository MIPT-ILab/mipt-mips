/**
 * Simple test to check whether two memory instances are equal
 * @author Pavel I. Kryukov kryukov@frtk.ru
 * Copyright 2018 MIPT-MIPS iLab project
 */

#include <catch.hpp>
#include <memory/memory.h>

void check_coherency(FuncMemory* mem1, FuncMemory* mem2, Addr address)
{
    CHECK( mem1->read<uint32, std::endian::little>( address) == mem2->read<uint32, std::endian::little>( address));
    CHECK( mem1->read<uint32, std::endian::little>( address + 1, 0xFFFFFFULL) == mem2->read<uint32, std::endian::little>( address + 1, 0xFFFFFFULL));
    CHECK( mem1->read<uint32, std::endian::little>( address + 2, 0xFFFFULL) == mem2->read<uint32, std::endian::little>( address + 2, 0xFFFFULL));
    CHECK( mem1->read<uint16, std::endian::little>( address + 2) == mem2->read<uint16, std::endian::little>( address + 2));
    CHECK( mem1->read<uint8, std::endian::little>( address + 3) == mem2->read<uint8, std::endian::little>( address + 3));
    CHECK( mem1->read<uint8, std::endian::little>( 0x7777) == mem2->read<uint8, std::endian::little>( 0x7777));

    mem1->write<uint8, std::endian::little>( 1, address);
    CHECK( mem1->read<uint32, std::endian::little>( address) != mem2->read<uint32, std::endian::little>( address));

    mem2->write<uint8, std::endian::little>( 1, address);
    CHECK( mem1->read<uint32, std::endian::little>( address) == mem2->read<uint32, std::endian::little>( address));

    mem1->write<uint16, std::endian::little>( 0x7777, address + 1);
    CHECK( mem1->read<uint32, std::endian::little>( address) != mem2->read<uint32, std::endian::little>( address));

    mem2->write<uint16, std::endian::little>( 0x7777, address + 1);
    CHECK( mem1->read<uint32, std::endian::little>( address) == mem2->read<uint32, std::endian::little>( address));

    mem1->write<uint32, std::endian::little>( 0x00000000, address);
    mem2->write<uint32, std::endian::little>( 0x00000000, address);

    CHECK( mem1->read<uint32, std::endian::little>( address) == mem1->read<uint32, std::endian::little>( address));
}
