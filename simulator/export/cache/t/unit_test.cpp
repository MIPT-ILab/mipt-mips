/**
 * Standalone cache runner unit test
 * @author Pavel Kryukov
 */

#include <export/cache/runner.h>
#include <infra/cache/cache_tag_array.h>

#include <catch.hpp>
#include <sstream>

TEST_CASE("CacheRunner: results zero dump")
{
    std::ostringstream oss;
    oss << CacheRunnerResults();
    CHECK( oss.str() == "total accesses: 0\nhit rate: 0%\nmiss rate: 100%\ncompulsory miss rate: 0%\n" );
}

TEST_CASE("CacheRunner: results any dump")
{
    CacheRunnerResults r;
    r.accesses = 10;
    r.hits = 7;
    r.compulsory_misses = 1;
    std::ostringstream oss;
    oss << r;
    CHECK( oss.str() == "total accesses: 10\nhit rate: 70%\nmiss rate: 30%\ncompulsory miss rate: 33.3333%\n" );
}

TEST_CASE("CacheRunner: run a test trace")
{
    auto cache = CacheTagArray::create( "LRU", 2048, 8, 64, 32);
    auto r = CacheRunner::create( cache.get())->run( TEST_PATH "/mem_trace.json");
    CHECK( r.accesses == 4);
    CHECK( r.hits == 1);
    CHECK( r.compulsory_misses == 3);
}

TEST_CASE("CacheRunner: invalid JSON")
{
    auto cache = CacheTagArray::create( "LRU", 2048, 8, 64, 32);
    CHECK_THROWS_AS( CacheRunner::create( cache.get())->run( TEST_PATH "/topology_root_test.json"), std::runtime_error);
}

TEST_CASE("CacheRunner: small cache")
{
    auto cache = CacheTagArray::create( "LRU", 64, 1, 64, 32);
    auto r = CacheRunner::create( cache.get())->run( TEST_PATH "/mem_trace.json");
    CHECK( r.accesses == 4);
    CHECK( r.hits == 0);
    CHECK( r.compulsory_misses == 3);
}
