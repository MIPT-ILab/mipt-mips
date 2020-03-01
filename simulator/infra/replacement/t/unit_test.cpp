/**
 * Tests for the line replacement modules.
 * @author Andrey Agrachev
 */

#include <infra/replacement/cache_replacement.h>

#include <catch.hpp>

TEST_CASE( "Check_bad_string_pass_to_factory_method")
{
    CHECK_THROWS_AS( create_cache_replacement( "BAD STRING", 3), CacheReplacementException);
}

TEST_CASE( "Check_get_ways_method")
{
    auto test_lru_module = create_cache_replacement( "LRU", 3);
    CHECK(test_lru_module->get_ways() == 3);
}

TEST_CASE( "LRU: Check_simple_touch_method")
{
    auto test_lru_module = create_cache_replacement( "LRU", 4);
    CHECK(test_lru_module->get_ways() == 4);
}

TEST_CASE( "LRU: Check_touch_method")
{
    auto test_lru_module = create_cache_replacement( "LRU", 4);
    test_lru_module->touch( 0);
    test_lru_module->touch( 1);
    test_lru_module->touch( 2);
    test_lru_module->touch( 3);
    CHECK( test_lru_module->update() == 0);
}

TEST_CASE( "LRU: Check_touch_method_2")
{
    auto test_lru_module = create_cache_replacement( "LRU", 4);
    test_lru_module->touch( 0);
    test_lru_module->touch( 1);
    test_lru_module->touch( 2);
    test_lru_module->touch( 3);
    test_lru_module->touch( 0);
    CHECK( test_lru_module->update() == 1);
}

TEST_CASE( "LRU: Check_set_to_erase_method")
{
    auto test_lru_module = create_cache_replacement( "LRU", 4);
    test_lru_module->touch( 0);
    test_lru_module->touch( 1);
    test_lru_module->touch( 2);
    test_lru_module->set_to_erase( 2);
    CHECK( test_lru_module->update() == 2);
}


TEST_CASE( "LRU: Check_Untouched_way")
{
    auto test_lru_module = create_cache_replacement( "LRU", 4);
    test_lru_module->touch( 0);
    test_lru_module->touch( 2);
    CHECK( test_lru_module->update() == 1);
}

TEST_CASE( "Pseudo-LRU: Check_get_ways_method")
{
    auto test_pseudo_lru_module = create_cache_replacement( "pseudo-LRU", 4);
    CHECK(test_pseudo_lru_module->get_ways() == 4);
}

TEST_CASE( "Pseudo-LRU: Check_forbidden_to_call_methods")
{
    auto test_pseudo_lru_module = create_cache_replacement( "pseudo-LRU", 4);
    CHECK_THROWS_AS( test_pseudo_lru_module->set_to_erase( 0), CacheReplacementException);
}

TEST_CASE( "Pseudo-LRU: Check_bad_way_number_in_touch_method")
{
    CHECK_THROWS_AS( create_cache_replacement( "pseudo-LRU", 3), CacheReplacementException);
}

TEST_CASE( "Pseudo-LRU: Check_update_method")
{
    auto test_lru_module = create_cache_replacement( "pseudo-LRU", 4);
    CHECK( test_lru_module->update() == 0);
}

TEST_CASE( "Pseudo-LRU: Check_touch_method")
{
    auto test_lru_module = create_cache_replacement( "pseudo-LRU", 4);
    test_lru_module->touch( 1);
    CHECK( test_lru_module->update() == 2);
}

TEST_CASE( "Pseudo-LRU: Check_update_method_to_initiate_a_touch_method")
{
    auto test_lru_module = create_cache_replacement( "pseudo-LRU", 4);
    test_lru_module->touch( 2);
    test_lru_module->touch( 1);
    CHECK( test_lru_module->update() == 3);
    CHECK( test_lru_module->update() == 0);
}

TEST_CASE( "Pseudo-LRU: Check_touch_method_2")
{
    auto test_lru_module = create_cache_replacement( "pseudo-LRU", 4);
    test_lru_module->touch( 0);
    test_lru_module->touch( 1);
    test_lru_module->touch( 2);
    test_lru_module->touch( 3);
    CHECK( test_lru_module->update() == 0);
}

TEST_CASE( "Pseudo-LRU: Check_touch_method_3")
{
    auto test_lru_module = create_cache_replacement( "pseudo-LRU", 4);
    test_lru_module->touch( 0);
    test_lru_module->touch( 1);
    test_lru_module->touch( 2);
    test_lru_module->touch( 3);
    test_lru_module->touch( 0);
    CHECK( test_lru_module->update() == 2);
}

TEST_CASE ( "Pseudo-LRU: Check_touch_method_with_big_number_of elements")
{
    auto test_lru_module = create_cache_replacement( "pseudo-LRU", 2048);
    test_lru_module->touch( 0);
    CHECK( test_lru_module->update() == 1024);
    CHECK( test_lru_module->update() == 512);
}
