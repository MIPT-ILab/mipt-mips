/**
 * Tests for the least recently used line replacement module.
 * @author Andrey Agrachev
 */

#include "../cache_replacement.h"

#include <catch.hpp>

TEST_CASE( "Check_simple_touch_method")
{
    std::unique_ptr<CacheReplacementInterface> test_lru_module = create_cache_replacement( "LRU", 3);
    test_lru_module->touch( 0);
    test_lru_module->touch( 1);
    test_lru_module->touch( 2);
    CHECK( test_lru_module->update() == 0);
}

TEST_CASE( "Check_another_touch_method")
{
    std::unique_ptr<CacheReplacementInterface> test_lru_module = create_cache_replacement( "LRU", 3);
    test_lru_module->touch( 0);
    test_lru_module->touch( 1);
    test_lru_module->touch( 2);
    test_lru_module->touch( 0);
    CHECK( test_lru_module->update() == 1);
}

TEST_CASE( "Check_set_to_erase_method")
{
    std::unique_ptr<CacheReplacementInterface> test_lru_module = create_cache_replacement( "LRU", 3);
    test_lru_module->touch( 0);
    test_lru_module->touch( 1);
    test_lru_module->touch( 2);
    test_lru_module->set_to_erase( 2);
    CHECK( test_lru_module->update() == 2);
}

TEST_CASE( "Check_Untouched_way")
{
    std::unique_ptr<CacheReplacementInterface> test_lru_module = create_cache_replacement( "LRU", 3);
    test_lru_module->touch( 0);
    test_lru_module->touch( 2);
    CHECK( test_lru_module->update() == 1);
}


TEST_CASE( "Check_allocate_method")
{
    std::unique_ptr<CacheReplacementInterface> test_lru_module = create_cache_replacement( "LRU", 3);
    CHECK_NOTHROW( test_lru_module->allocate( 0));
}

TEST_CASE( "Check_erase_lru_element_method")
{
    std::unique_ptr<CacheReplacementInterface> test_lru_module = create_cache_replacement( "LRU", 3);
    CHECK_NOTHROW(test_lru_module->allocate(1));
}




