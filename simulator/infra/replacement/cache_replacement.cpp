/**
 * CacheReplacement.cpp
 * Cache replacement algorithms module.
 * @author Oleg Ladin, Denis Los, Andrey Agrachev
 */

#include "infra/replacement/cache_replacement.h"
#include "infra/macro.h"

#include <list>
#include <vector>

class LRU : public CacheReplacement
{
    public:
        explicit LRU( std::size_t ways);

        void touch( std::size_t way) override ;
        void set_to_erase( std::size_t way) override ;
        std::size_t update() override ;
        std::size_t get_ways() const override { return lru_hash.size(); }

    private:
        std::list<std::size_t> lru_list{};
        std::vector<decltype(lru_list.cbegin())> lru_hash{};
};

LRU::LRU( std::size_t ways) : lru_hash( ways)
{
    for ( std::size_t i = 0; i < lru_hash.size(); i++)
        lru_hash[i] = lru_list.insert( lru_list.begin(), i);
}

void LRU::touch( std::size_t way)
{
    // Put the way to the head of the list
    lru_list.splice( lru_list.begin(), lru_list, lru_hash[way]);
}

void LRU::set_to_erase( std::size_t way)
{
    lru_list.splice( lru_list.end(), lru_list, lru_hash[way]);
}

std::size_t LRU::update()
{
    // remove the least recently used element from the tail
    std::size_t lru_elem = lru_list.back();
    touch( lru_elem);
    return lru_elem;
}

//////////////////////////////////////////////////////////////////

class PseudoLRU : public CacheReplacement
{
    public:
        explicit PseudoLRU( std::size_t ways);
        void touch( std::size_t way) override;
        void set_to_erase( std::size_t /* unused */) override;
        std::size_t update() override;
        std::size_t get_ways() const override { return ways; }

    private:
        enum Flags { Left = 0, Right = 1};

        /*
         *    0
         *   / \ 
         *  1   2  - nodes
         * / \ / \
         * 3 4 5 6 - leaves
         * 0 1 2 3 - ways
         */
        size_t get_next_node( size_t node) const { return node * 2 + ( nodes[node] == Left ? 1 : 2); }
        static Flags get_direction_to_prev_node( size_t node) { return node % 2 != 0 ? Left : Right; }
        void reverse_node( size_t node) { nodes[node] = ( nodes[node] == Left ? Right : Left); }

        std::vector<Flags> nodes;
        const std::size_t ways;
};

PseudoLRU::PseudoLRU( std::size_t ways) : nodes( ways - 1, Left), ways( ways)
{
    if (!is_power_of_two( ways))
        throw CacheReplacementException("Number of ways must be the power of 2!");
}

void PseudoLRU::touch( std::size_t way)
{
    auto node = way + nodes.size();
    while ( node != 0) {
        const auto parent = ( node - 1) / 2;
        if ( get_direction_to_prev_node( node) == nodes[parent])
            reverse_node( parent);
        node = parent;
    }
}

std::size_t PseudoLRU::update()
{
    size_t node = 0;
    while ( node < nodes.size())
        node = get_next_node(node);

    const auto way = node - nodes.size();
    touch( way);
    return way;
}

void PseudoLRU::set_to_erase( std::size_t /* way */)
{
    throw CacheReplacementException( "PLRU does not support inverted access");
}

////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<CacheReplacement> create_cache_replacement( const std::string& name, std::size_t ways)
{
    if (name == "LRU")
        return std::make_unique<LRU>( ways);

    if (name == "pseudo-LRU")
        return std::make_unique<PseudoLRU>( ways);

    throw CacheReplacementException("\"" + name + "\" replacement policy is not defined, supported polices are:\nLRU\npseudo-LRU\n");
}
