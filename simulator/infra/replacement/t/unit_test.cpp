/**
 * Tests for the least recently used line replacement module.
 * @author Andrey Agrachev
 */

#include "../cache_replacement.h"

#include <catch.hpp>

TEST_CASE( "Check_bad_string_pass_to_factory_method")
{
    CHECK_THROWS_AS( create_cache_replacement( "BAD STRING", 3), UndefinedCacheReplacementPolicyName);
}

TEST_CASE( "Check_get_ways_method")
{
    auto test_lru_module = create_cache_replacement( "LRU", 3);
    CHECK(test_lru_module->get_ways() == 3);
}

TEST_CASE( "Check_simple_touch_method")
{
    auto test_lru_module = create_cache_replacement( "LRU", 3);
    test_lru_module->touch( 0);
    test_lru_module->touch( 1);
    test_lru_module->touch( 2);
    CHECK( test_lru_module->update() == 0);
}

TEST_CASE( "Check_another_touch_method")
{
    auto test_lru_module = create_cache_replacement( "LRU", 3);
    test_lru_module->touch( 0);
    test_lru_module->touch( 1);
    test_lru_module->touch( 2);
    test_lru_module->touch( 0);
    CHECK( test_lru_module->update() == 1);
}

TEST_CASE( "Check_set_to_erase_method")
{
    auto test_lru_module = create_cache_replacement( "LRU", 3);
    test_lru_module->touch( 0);
    test_lru_module->touch( 1);
    test_lru_module->touch( 2);
    test_lru_module->set_to_erase( 2);
    CHECK( test_lru_module->update() == 2);
}

TEST_CASE( "Check_Untouched_way")
{
    auto test_lru_module = create_cache_replacement( "LRU", 3);
    test_lru_module->touch( 0);
    test_lru_module->touch( 2);
    CHECK( test_lru_module->update() == 1);
}
