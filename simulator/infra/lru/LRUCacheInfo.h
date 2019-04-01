/**
 * cache_tag_array.h
 * Header for the least recently used line replacement module.
 * @author Oleg Ladin, Denis Los
 */

#ifndef LRUCACHEINFO_H
#define LRUCACHEINFO_H

#include <list>
#include <unordered_map>

class LRUCacheInfo
{
    public:
        explicit LRUCacheInfo( std::size_t ways);

        void touch( std::size_t way);
        void erase( std::size_t way);
        std::size_t update();
        std::size_t get_ways() const { return ways; }


    private:
        std::list<std::size_t> lru_list{};
        std::unordered_map<std::size_t, decltype(lru_list.cbegin())> lru_hash{};
        const std::size_t ways;
};


#endif // LRUCACHEINFO_H
