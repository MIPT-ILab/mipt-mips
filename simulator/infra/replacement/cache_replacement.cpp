/**
 * CacheReplacement.cpp
 * Cache replacement algorithms module.
 * @author Oleg Ladin, Denis Los, Andrey Agrachev
 */

#include <sparsehash/dense_hash_map.h>
#include "infra/replacement/cache_replacement.h"
#include "infra/macro.h"
#include <tree.h>

#include <list>
#include <cassert>
#include <cmath>
#include <bitset>

class LRUCacheInfo : public CacheReplacementInterface
{
    public:
        explicit LRUCacheInfo( std::size_t ways);

        void touch( std::size_t way) override ;
        void set_to_erase( std::size_t way) override ;
        std::size_t update() override ;
        std::size_t get_ways() const override { return ways; }

    private:
        std::list<std::size_t> lru_list{};
        const std::size_t ways;
        const std::size_t impossible_key = SIZE_MAX;
        google::dense_hash_map<std::size_t, decltype(lru_list.cbegin())> lru_hash{};
};

LRUCacheInfo::LRUCacheInfo( std::size_t ways)
    : ways( ways), lru_hash( ways)
{
    lru_hash.set_empty_key( impossible_key); //special dense_hash_map requirement
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
    lru_hash[ lru_elem] = ptr;

    return lru_elem;
}

//////////////////////////////////////////////////////////////////

struct LRU_tree_node
{
    explicit LRU_tree_node(int new_way_number = 0, std::size_t new_lru_branch_ptr = 0)
    : way_number( new_way_number), lru_branch_ptr( new_lru_branch_ptr) {}

    int way_number;
    std::size_t lru_branch_ptr;

    bool operator==(const LRU_tree_node& rhs) const { return way_number == rhs.way_number; }
    bool operator< (const LRU_tree_node &rhs) { return way_number < rhs.way_number; }
};

class Pseudo_LRUCacheInfo : public CacheReplacementInterface
{
    public:
        explicit Pseudo_LRUCacheInfo( std::size_t ways);
        void touch( std::size_t way) override;
        void set_to_erase( std::size_t ) override;
        std::size_t update() override;
        std::size_t get_ways() const override { return ways; }

    private:
        enum Flags { Left = 0, Right = 1};

        core::tree<LRU_tree_node> lru_tree;
        void construct_tree( core::tree<LRU_tree_node>::iterator LRU_tree_node_it, std::size_t max_depth);
        void construct_leaf_layer();
        std::size_t calculate_depth() const;
        std::size_t which_sibling( core::tree<LRU_tree_node>::iterator LRU_tree_node_it);
        std::size_t reverse_flag( enum Flags Flag);

        const std::size_t ways;
        int leaf_iterator = 0;
        int LRU_tree_node_iterator = INT8_MIN; //make shure it is less than 0
};

Pseudo_LRUCacheInfo::Pseudo_LRUCacheInfo( std::size_t ways)
    : ways( ways)
{
    lru_tree.data( LRU_tree_node(LRU_tree_node_iterator));
    construct_tree( lru_tree.get_tree_iterator(), calculate_depth());
}

std::size_t Pseudo_LRUCacheInfo::calculate_depth() const
{
    if (is_power_of_two( ways) ==  false)
        throw CacheReplacementException("Number of ways must be the power of 2!");
    return 32 - count_leading_zeroes<uint32>( narrow_cast<uint32>( ways)) - 1;
}

void Pseudo_LRUCacheInfo::construct_tree( core::tree<LRU_tree_node>::iterator LRU_tree_node_it, std::size_t max_depth)
{
    if ( LRU_tree_node_it.level() < max_depth)
    {
        if ( LRU_tree_node_it.level() != max_depth - 1)
        {
            auto left_it = LRU_tree_node_it.insert( LRU_tree_node( LRU_tree_node_iterator++, Left));
            auto right_it = LRU_tree_node_it.insert( LRU_tree_node( LRU_tree_node_iterator++, Left));
            construct_tree( left_it, max_depth);
            construct_tree( right_it, max_depth);
        }
        else
        {
            LRU_tree_node_it.insert( LRU_tree_node(leaf_iterator++));
            LRU_tree_node_it.insert( LRU_tree_node(leaf_iterator++));
        }
    }
}

std::size_t Pseudo_LRUCacheInfo::reverse_flag( enum Flags Flag) //more readable than multiplying on -1
{
    return Flag == Left ? Right : Left;
}

std::size_t Pseudo_LRUCacheInfo::which_sibling( core::tree<LRU_tree_node>::iterator LRU_tree_node_it) //tree container doesnt provide such an option
{
    return LRU_tree_node_it.data().way_number % 2 == 0 ? Left : Right;
}

void Pseudo_LRUCacheInfo::touch( std::size_t way)
{
    auto found_it = lru_tree.tree_find_depth( LRU_tree_node( static_cast<int>(way)));
    for ( auto i = found_it; i != lru_tree.get_tree_iterator(); i = i.out())
        if ( which_sibling( i) == i.out().data().lru_branch_ptr)
            i.out().data().lru_branch_ptr = reverse_flag(static_cast<enum Flags>(i.out().data().lru_branch_ptr));
}

std::size_t Pseudo_LRUCacheInfo::update()
{
    auto LRU_tree_node_it = lru_tree.get_tree_iterator();
    while ( LRU_tree_node_it.size() != 0)
    {
        if (LRU_tree_node_it.data().lru_branch_ptr == Left)
            LRU_tree_node_it = LRU_tree_node_it.begin();
        else
            LRU_tree_node_it = ++LRU_tree_node_it.begin();
    }
    touch( LRU_tree_node_it.data().way_number);
    return LRU_tree_node_it.data().way_number;
}

void Pseudo_LRUCacheInfo::set_to_erase( std::size_t ) { throw CacheReplacementException( "Set_to_erase method is not supposed to be used in perfomance simulation"); }

////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<CacheReplacementInterface> create_cache_replacement( const std::string& name, std::size_t ways)
{
    if (name == "LRU")
        return std::make_unique<LRUCacheInfo>( ways);

    if (name == "Pseudo-LRU")
        return std::make_unique<Pseudo_LRUCacheInfo>( ways);

    throw CacheReplacementException("\"" + name + "\" replacement policy is not defined, supported polices are:\nLRU\npseudo-LRU\n");
}
