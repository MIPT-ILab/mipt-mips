/**
 * Ports unit test
 * @author Pavel Kryukov
 * Copyright 2019 MIPT-MIPS team
 */

#include "../ports.h"
#include <catch.hpp>
#include <map>
 
TEST_CASE("Ports: no write port")
{
    auto pm = PortMap::create_port_map();
    ReadPort<int> input( pm, "Key", PORT_LATENCY);
    CHECK_THROWS_AS( pm->init(), PortError);
}

TEST_CASE("Ports: no read port")
{
    auto pm = PortMap::create_port_map();
    WritePort<int> output( pm, "Yek", PORT_BW, PORT_FANOUT);
    CHECK_THROWS_AS( pm->init(), PortError);
}

TEST_CASE("Ports: fanout overload")
{
    auto pm = PortMap::create_port_map();
    ReadPort<int> input( pm, "Key", PORT_LATENCY);
    WritePort<int> output( pm, "Key", PORT_BW, PORT_FANOUT);
    ReadPort<int> input2( pm, "Key", PORT_LATENCY);
    CHECK_THROWS_AS( pm->init(), PortError);
}

TEST_CASE("Ports: fanout underload")
{
    auto pm = PortMap::create_port_map();
    ReadPort<int> input( pm, "Key", PORT_LATENCY);
    WritePort<int> output( pm, "Key", PORT_BW, PORT_FANOUT * 2);
    CHECK_THROWS_AS( pm->init(), PortError);
}

using PairOfPorts = std::pair<std::unique_ptr<ReadPort<int>>, std::unique_ptr<WritePort<int>>>;

static PairOfPorts
get_pair_of_ports( const std::shared_ptr<PortMap>& pm, uint32 bw = PORT_BW, Latency lat = PORT_LATENCY)
{
    PairOfPorts pop;
    pop.first = std::make_unique<ReadPort<int>>( pm, "Key", lat);
    pop.second = std::make_unique<WritePort<int>>( pm, "Key", bw, PORT_FANOUT);
    pm->init();
    return pop;
}

TEST_CASE("Ports: simple transmission")
{
    auto pm = PortMap::create_port_map();
    const auto [rp, wp] = get_pair_of_ports( pm);
    wp->write( 11, 0_cl);
    CHECK( !rp->is_ready( 0_cl));
    pm->clean_up( 0_cl);
    CHECK( rp->is_ready( 1_cl));
    CHECK( rp->read( 1_cl) == 11);
}

TEST_CASE("Ports: throw if not ready")
{
    const auto [rp, wp] = get_pair_of_ports( PortMap::create_port_map());
    wp->write( 1, 0_cl);
    CHECK_THROWS_AS( rp->read( 0_cl), PortError);
}

TEST_CASE("Ports: read once")
{
    auto pm = PortMap::create_port_map();
    const auto [rp, wp] = get_pair_of_ports( pm);
    wp->write( 11, 0_cl);
    pm->clean_up( 0_cl);
    rp->read( 1_cl);
    CHECK( !rp->is_ready( 1_cl));
}

TEST_CASE("Ports: overload bandwidth")
{
    const auto [rp, wp] = get_pair_of_ports( PortMap::create_port_map());
    (void)rp;
    wp->write( 11, 0_cl);
    CHECK_THROWS_AS( wp->write( 12, 0_cl), PortError);
}

TEST_CASE("Ports: sequential read")
{
    auto pm = PortMap::create_port_map();
    const auto [rp, wp] = get_pair_of_ports( pm);

    wp->write( 10, 0_cl);
    pm->clean_up( 0_cl);
    
    wp->write( 11, 1_cl);
    CHECK( rp->is_ready( 1_cl));
    CHECK( rp->read( 1_cl) == 10);
    CHECK( !rp->is_ready( 1_cl));
    pm->clean_up( 1_cl);

    CHECK( rp->read( 2_cl) == 11);
}

TEST_CASE("Ports: non-regular read")
{
    auto pm = PortMap::create_port_map();
    const auto [rp, wp] = get_pair_of_ports( pm);
    wp->write( 10, 0_cl);
    pm->clean_up( 1_cl);

    wp->write( 11, 1_cl);
    CHECK( rp->is_ready( 1_cl));
    pm->clean_up( 2_cl);

    CHECK( rp->is_ready( 2_cl));
    CHECK( rp->read( 2_cl) == 11);
}
