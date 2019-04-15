/**
 * CacheReplacement.h
 * Header for the Cache replacement algorithms module.
 * @author Oleg Ladin, Denis Los, Andrey Agrachev
 */

#ifndef CACHEREPLACEMENT_H
#define CACHEREPLACEMENT_H

#include <list>
#include <unordered_map>

enum replacement_polices { LRU, pseudo_LRU };

struct CacheReplacementInterface
{
    explicit CacheReplacementInterface() {}
    virtual ~CacheReplacementInterface() {}
    virtual void touch( std::size_t) = 0;
    virtual void set_to_erase( std::size_t) = 0;
    virtual void allocate( std::size_t) = 0;
    virtual std::size_t update() = 0;
    virtual std::size_t get_ways() const = 0;
    virtual std::size_t get_hash_size() const = 0;
};

class LRUCacheInfo : public CacheReplacementInterface
{
    public:
        explicit LRUCacheInfo( std::size_t ways);

        void touch( std::size_t way) override ;
        void set_to_erase( std::size_t way) override ;
        void allocate( std::size_t way) override ;
        std::size_t update() override ;
        std::size_t get_ways() const override { return ways; }
        std::size_t get_hash_size() const override { return lru_hash.size(); }


    private:
        std::list<std::size_t> lru_list{};
        std::unordered_map<std::size_t, decltype(lru_list.cbegin())> lru_hash{};
        const std::size_t ways;
        void erase_lru_element();
};


#endif // CACHEREPLACEMENT_H
