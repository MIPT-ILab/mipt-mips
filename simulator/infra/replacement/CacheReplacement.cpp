/**
 * CacheReplacement.cpp
 * Cache replacement algorithms module.
 * @author Oleg Ladin, Denis Los, Andrey Agrachev
 */

 #include <cassert>

 #include "infra/replacement/CacheReplacement.h"

LRUCacheInfo::LRUCacheInfo( std::size_t ways)
    : ways( ways)
{
    assert( ways != 0u);

    lru_hash.reserve( ways);

    for ( std::size_t i = 0; i < ways; i++)
    {
        lru_list.push_front( i);
        lru_hash.emplace( i, lru_list.begin());
    }
}

void LRUCacheInfo::touch( std::size_t way)
{
    const auto lru_it = lru_hash.find( way);
    assert( lru_it != lru_hash.end());
    // Put the way to the head of the list
    lru_list.splice( lru_list.begin(), lru_list, lru_it->second);
}

void LRUCacheInfo::set_to_erase( std::size_t way)
{
    const auto lru_it = lru_hash.find( way);
    assert( lru_it != lru_hash.end());
    lru_list.splice( lru_list.end(), lru_list, lru_it->second);
}

std::size_t LRUCacheInfo::update()
{
    // remove the least recently used element from the tail
    std::size_t lru_elem = lru_list.back();
    lru_list.pop_back();

    // put it to the head
    auto ptr = lru_list.insert( lru_list.begin(), lru_elem);
    lru_hash.insert_or_assign( lru_elem, ptr);

    return lru_elem;
}

void LRUCacheInfo::allocate( std::size_t way)
{
    if ( lru_hash.size() >= ways)
        erase_lru_element();
    lru_list.push_front( way);
    lru_hash.emplace( way, lru_list.begin());
}

void LRUCacheInfo::erase_lru_element() {
        std::size_t lru_elem = lru_list.back();
        lru_list.pop_back();
        lru_hash.erase( lru_elem);
}


