/**
 * Unit test for PortQueue
 * @author Pavel Kryukov
 * Copyright 2019 MIPT-MIPS team
 */

#include <catch.hpp>
#include <infra/ports/port_queue/port_queue.h>

TEST_CASE("PortQueue: empty queue")
{
    PortQueue<int> q;
    CHECK( q.empty() );
    CHECK( q.full() );
}

TEST_CASE("PortQueue: one entry queue")
{
    PortQueue<int> q;
    q.resize(1);
    CHECK( q.empty() );
    CHECK( !q.full() );
}

TEST_CASE("PortQueue: allocate one entry")
{
    PortQueue<int> q;
    q.resize(1);
    q.emplace(4);
    CHECK( !q.empty() );
    CHECK( q.front() == 4 );
    CHECK( q.full() );
}

TEST_CASE("PortQueue: allocate 5 entries in 3 places")
{
    PortQueue<int> q;
    q.resize(3);
    q.emplace(4);
    q.emplace(5);
    q.emplace(6);
    CHECK( q.front() == 4);
    q.pop();
    CHECK( q.front() == 5);
    q.emplace(7);
    CHECK( q.front() == 5);
    CHECK( q.full() );
    q.pop();
    CHECK( q.front() == 6);
    q.pop();
    CHECK( q.front() == 7);
    q.pop();
    CHECK( q.empty() );
}
