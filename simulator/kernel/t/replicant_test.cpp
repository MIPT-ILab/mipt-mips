/*
 * Unit test for adapter for several CPU models under single interface
 * @author Pavel Kryukov
 * Copyright 2019 MIPT-MIPS
 */

#include <catch.hpp>
#include <kernel/replicant.h>

#include <iostream>

TEST_CASE( "Replicant: set_target + get_pc")
{
    auto sim1 = Simulator::create_functional_simulator( "mips32");
    auto sim2 = Simulator::create_functional_simulator( "mips32");
    CPUReplicant sim12( sim1);
    sim12.add_replica( sim2);

    sim12.set_target( Target{ 0x100500, 0});

    CHECK( sim1->get_pc() == 0x100500);
    CHECK( sim2->get_pc() == 0x100500);
    CHECK( sim12.get_pc() == 0x100500);
    
    CHECK( sim12.sizeof_register() == 4);
    CHECK( sim12.max_cpu_register() >= 32);
}

TEST_CASE( "Replicant: cpu register")
{
    auto sim1 = Simulator::create_functional_simulator( "mips32");
    auto sim2 = Simulator::create_functional_simulator( "mips32");
    CPUReplicant sim12( sim1);
    sim12.add_replica( sim2);

    sim12.write_cpu_register( 4, 1000);

    CHECK( sim1->read_cpu_register( 4) == 1000);
    CHECK( sim2->read_cpu_register( 4) == 1000);
    CHECK( sim12.read_cpu_register( 4) == 1000);
}

TEST_CASE( "Replicant: gdb register")
{
    auto sim1 = Simulator::create_functional_simulator( "mips32");
    auto sim2 = Simulator::create_functional_simulator( "mips32");
    CPUReplicant sim12( sim1);
    sim12.add_replica( sim2);

    sim12.write_gdb_register( 4, 1000);

    CHECK( sim1->read_gdb_register( 4) == 1000);
    CHECK( sim2->read_gdb_register( 4) == 1000);
    CHECK( sim12.read_gdb_register( 4) == 1000);
}

TEST_CASE( "Replicant: csr register")
{
    auto sim1 = Simulator::create_functional_simulator( "riscv32");
    auto sim2 = Simulator::create_functional_simulator( "riscv32");
    CPUReplicant sim12( sim1);
    sim12.add_replica( sim2);

    sim12.write_csr_register( "mscratch", 1000);

    CHECK( sim1->read_csr_register( "mscratch") == 1000);
    CHECK( sim2->read_csr_register( "mscratch") == 1000);
    CHECK( sim12.read_csr_register( "mscratch") == 1000);
}
