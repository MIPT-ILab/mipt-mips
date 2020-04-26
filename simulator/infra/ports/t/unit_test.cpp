/**
 * Ports unit test
 * @author Pavel Kryukov
 * Copyright 2019 MIPT-MIPS team
 */

#include <catch.hpp>
#include <infra/ports/module.h>

TEST_CASE( "Latency to string")
{
    CHECK( ( 5_cl).to_string() == "5");
    CHECK( ( 2_lt).to_string() == "2");
    CHECK( Port::LATENCY.to_string() == "1");
}

struct BaseTestRoot : public Root
{
    BaseTestRoot() : Root( "test-root") { }
};

TEST_CASE("Ports: no write port")
{
    struct TestRoot : public BaseTestRoot
    {
        TestRoot()
        {
            make_read_port<int>( "Key", Port::LATENCY);
            CHECK_THROWS_AS( init_portmap(), PortError);
        }
    } tr;
}

TEST_CASE("Ports: no read port")
{
    struct TestRoot : public BaseTestRoot
    {
        TestRoot()
        {
            make_write_port<int>( "Yek", Port::BW);
            CHECK_THROWS_AS( init_portmap(), PortError);
        }
    } tr;
}

TEST_CASE("Ports: two write ports")
{
    struct TestRoot : public BaseTestRoot
    {
        TestRoot()
        {
            make_read_port<int>( "Key", Port::LATENCY);
            make_write_port<int>( "Key", Port::BW);
            CHECK_THROWS_AS( make_write_port<int>( "Key", Port::BW), PortError);
        }
    } tr;
}

TEST_CASE("Ports: type mismatch")
{
    struct TestRoot : public BaseTestRoot
    {
        TestRoot()
        {
            make_read_port<int>( "Key", Port::LATENCY);
            make_write_port<std::string>( "Key", Port::BW);
            CHECK_THROWS_AS( init_portmap(), PortError);
        }
    } tr;
}

struct PairOfPorts : public BaseTestRoot
{
    ReadPort<int>* rp;
    WritePort<int>* wp;

    PairOfPorts()
    {
        rp = make_read_port<int>( "Key", Port::LATENCY);
        wp = make_write_port<int>( "Key", Port::BW);
        init_portmap();
    }
};

TEST_CASE("Ports: simple transmission")
{
    PairOfPorts pop;
    pop.wp->write( 11, 0_cl);
    CHECK( !pop.rp->is_ready( 0_cl));
    CHECK( pop.rp->is_ready( 1_cl));
    CHECK( pop.rp->read( 1_cl) == 11);
}

TEST_CASE("Ports: throw if not ready")
{
    PairOfPorts pop;
    pop.wp->write( 1, 0_cl);
    CHECK_THROWS_AS( pop.rp->read( 0_cl), PortError);
}

TEST_CASE("Ports: read once")
{
    PairOfPorts pop;
    pop.wp->write( 11, 0_cl);
    pop.rp->read( 1_cl);
    CHECK( !pop.rp->is_ready( 1_cl));
}

TEST_CASE("Ports: overload bandwidth")
{
    PairOfPorts pop;
    pop.wp->write( 11, 0_cl);
    CHECK_THROWS_AS( pop.wp->write( 12, 0_cl), PortError);
}

TEST_CASE("Ports: sequential read")
{
    PairOfPorts pop;

    pop.wp->write( 10, 0_cl);
    pop.wp->write( 11, 1_cl);
    CHECK( pop.rp->is_ready( 1_cl));
    CHECK( pop.rp->read( 1_cl) == 10);
    CHECK( !pop.rp->is_ready( 1_cl));
    CHECK( pop.rp->read( 2_cl) == 11);
}

TEST_CASE("Ports: non-regular read")
{
    PairOfPorts pop;

    pop.wp->write( 10, 0_cl);
    pop.wp->write( 11, 1_cl);
    CHECK( pop.rp->is_ready( 1_cl));
    CHECK( pop.rp->is_ready( 2_cl));
    CHECK( pop.rp->read( 2_cl) == 11);
}

struct SomeHiearchy : public BaseTestRoot
{
    struct DumpCheckingModule : public Module
    {
        bool check_if_dumps() const { return sout.enabled(); }
        using Module::Module;
    };

    std::unique_ptr<DumpCheckingModule> a, b, c, d, e, f;
    explicit SomeHiearchy( const std::string& dump_string)
    {
        /* Root
         * | \
         * A  D
         * |  | \
         * B  E  F
         * |
         * C
         */
        a = std::make_unique<DumpCheckingModule>( this, "A");
        b = std::make_unique<DumpCheckingModule>( a.get(), "B");
        c = std::make_unique<DumpCheckingModule>( b.get(), "C");
        d = std::make_unique<DumpCheckingModule>( this, "D");
        e = std::make_unique<DumpCheckingModule>( d.get(), "E");
        f = std::make_unique<DumpCheckingModule>( d.get(), "F");
        enable_logging( dump_string);
    }
};

TEST_CASE("Module: dump nothing")
{
    SomeHiearchy h( "");
    CHECK_FALSE( h.a->check_if_dumps());
    CHECK_FALSE( h.b->check_if_dumps());
    CHECK_FALSE( h.c->check_if_dumps());
    CHECK_FALSE( h.d->check_if_dumps());
    CHECK_FALSE( h.e->check_if_dumps());
    CHECK_FALSE( h.f->check_if_dumps());
}

TEST_CASE("Module: dump all")
{
    SomeHiearchy h( "test-root");
    CHECK( h.a->check_if_dumps());
    CHECK( h.b->check_if_dumps());
    CHECK( h.c->check_if_dumps());
    CHECK( h.d->check_if_dumps());
    CHECK( h.e->check_if_dumps());
    CHECK( h.f->check_if_dumps());
}

TEST_CASE("Module: dump subtree")
{
    SomeHiearchy h( "A");
    CHECK( h.a->check_if_dumps());
    CHECK( h.b->check_if_dumps());
    CHECK( h.c->check_if_dumps());
    CHECK_FALSE( h.d->check_if_dumps());
    CHECK_FALSE( h.e->check_if_dumps());
    CHECK_FALSE( h.f->check_if_dumps());
}

TEST_CASE("Module: dump subtrees")
{
    SomeHiearchy h( "B,E");
    CHECK_FALSE( h.a->check_if_dumps());
    CHECK( h.b->check_if_dumps());
    CHECK( h.c->check_if_dumps());
    CHECK_FALSE( h.d->check_if_dumps());
    CHECK( h.e->check_if_dumps());
    CHECK_FALSE( h.f->check_if_dumps());
}

TEST_CASE("Module: exclusion")
{
    SomeHiearchy h( "test-root,!D,!C,F");
    CHECK( h.a->check_if_dumps());
    CHECK( h.b->check_if_dumps());
    CHECK_FALSE( h.c->check_if_dumps());
    CHECK_FALSE( h.d->check_if_dumps());
    CHECK_FALSE( h.e->check_if_dumps());
    CHECK( h.f->check_if_dumps());
}
