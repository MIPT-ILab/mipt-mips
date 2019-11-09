/**
 * Standalone cache runner unit test
 * @author Pavel Kryukov
 */

#include "../runner.h"

#include <infra/cache/cache_tag_array.h>

#include <catch.hpp>
#include <sstream>

TEST_CASE("CacheRunner: results zero dump")
{
    std::ostringstream oss;
    oss << CacheRunnerResults();
    CHECK( oss.str() == "total accesses: 0\nhit rate: 0%\nmiss rate: 100%\n" );
}

TEST_CASE("CacheRunner: results any dump")
{
    CacheRunnerResults r;
    r.accesses = 10;
    r.hits = 7;
    std::ostringstream oss;
    oss << r;
    CHECK( oss.str() == "total accesses: 10\nhit rate: 70%\nmiss rate: 30%\n" );
}

TEST_CASE("CacheRunner: run a test trace")
{
    auto cache = CacheTagArray::create( "LRU", 2048, 8, 64, 32);
    auto r = run_cache( cache.get(), TEST_PATH "/mem_trace.json");
    CHECK( r.accesses == 4);
    CHECK( r.hits == 1);
}

TEST_CASE("CacheRunner: invalid JSON")
{
    auto cache = CacheTagArray::create( "LRU", 2048, 8, 64, 32);
    CHECK_THROWS_AS( run_cache( cache.get(), TEST_PATH "/topology_root_test.json"), std::runtime_error);
}
