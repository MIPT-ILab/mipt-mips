/**
 * CacheReplacement.cpp
 * Cache replacement algorithms module.
 * @author Oleg Ladin, Denis Los, Andrey Agrachev
 */

#include "infra/replacement/cache_replacement.h"

#include <list>
#include <unordered_map>
#include <cassert>
#include <cmath>
#include "tree.hh"

class LRUCacheInfo : public CacheReplacementInterface
{
    public:
        explicit LRUCacheInfo( std::size_t ways);

        void touch( std::size_t way) override ;
        void set_to_erase( std::size_t way) override ;
        void allocate( std::size_t way) override ;
        std::size_t update() override ;
        std::size_t get_ways() const override { return ways; }

    private:
        std::list<std::size_t> lru_list{};
        std::unordered_map<std::size_t, decltype(lru_list.cbegin())> lru_hash{};
        const std::size_t ways;
        void erase_lru_element();
};

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

class Pseudo_LRUCacheInfo : public CacheReplacementInterface
{
    public:
        explicit Pseudo_LRUCacheInfo( std::size_t ways);
        void touch( std::size_t way) override;
        std::size_t update() override;
        std::size_t get_ways() const override { return ways; }

        void set_to_erase( std::size_t ) override;
        void allocate( std::size_t ) override;

    private:
        enum flags { LEFT, RIGHT};

        tree<std::size_t> lru_tree;
        void construct_tree( tree<std::size_t>::iterator node_it, int depth);
        void construct_leaf_layer();
        int calculate_depth() const;
        std::size_t reverse_flag( std::size_t flag);

        const std::size_t ways;
        size_t leaf_iterator = 0;
};

Pseudo_LRUCacheInfo::Pseudo_LRUCacheInfo( std::size_t ways)
    : ways( ways)
{
    lru_tree.insert( lru_tree.begin(), LEFT);
    construct_tree( lru_tree.begin(), calculate_depth());
}

int Pseudo_LRUCacheInfo::calculate_depth() const
{
    int i = 0;
    for (; pow(2, i) < ways; ++i);
    return i;
}

void Pseudo_LRUCacheInfo::construct_tree( tree<std::size_t>::iterator node_it, int depth)
{
    if (lru_tree.depth( node_it) < depth)
    {
        if ( lru_tree.depth( node_it) != depth - 1)
        {
            auto left_it = lru_tree.append_child( node_it, LEFT);
            auto right_it = lru_tree.append_child( node_it, LEFT);
            construct_tree( left_it, depth);
            construct_tree( right_it, depth);
        }
        else
        {
            if ( leaf_iterator != ways)
                lru_tree.append_child(node_it, leaf_iterator++);
            if ( leaf_iterator != ways)
                lru_tree.append_child(node_it, leaf_iterator++);
        }
    }
}

std::size_t Pseudo_LRUCacheInfo::reverse_flag( std::size_t flag)
{
    if ( flag == LEFT)
        return RIGHT;
    else
        return LEFT;
}

void Pseudo_LRUCacheInfo::touch( std::size_t way)
{
    if ( way >= ways)
        throw CacheReplacementException( "Requsted way number ( " + std::to_string( way) + ") is out of the capacity (" + std::to_string( ways) +")\n");
    auto found_it = std::find(lru_tree.begin(), lru_tree.end(), way);
    for ( auto i = found_it; i != lru_tree.begin(); i = lru_tree.parent( i))
        if ( lru_tree.index( i) == *lru_tree.parent( i))
            lru_tree.replace( lru_tree.parent( i), reverse_flag( *lru_tree.parent( i)));
}

std::size_t Pseudo_LRUCacheInfo::update()
{
    auto node_it = lru_tree.begin();
    while ( lru_tree.number_of_children( node_it) != 0)
    {
        if (*node_it == RIGHT && lru_tree.number_of_children( node_it) == 2)
            node_it = lru_tree.child( node_it, 1); //go to the right child if it exists
        else
            node_it = lru_tree.child( node_it, 0); //go tot he left child
    }
    return *node_it;
}

void Pseudo_LRUCacheInfo::set_to_erase( std::size_t ) { throw CacheReplacementException( "Set_to_erase method is not supposed to be used in perfomance simulation"); }
void Pseudo_LRUCacheInfo::allocate( std::size_t ) { throw CacheReplacementException( "Allocate method is not supposed to be used in perfomance simulation"); }


////////////////////////////////////////////////////////////////////////////////////


std::unique_ptr<CacheReplacementInterface> create_cache_replacement( const std::string& name, std::size_t ways)
{
    if (name == "LRU")
        return std::make_unique<LRUCacheInfo>( ways);

    if (name == "Pseudo-LRU")
        return std::make_unique<Pseudo_LRUCacheInfo>( ways);
    else
        throw CacheReplacementException("\"" + name + "\" replacement policy is not defined, supported polices are:\nLRU\npseudo-LRU\n");
}
    //if (name == "pseudo-LRU")
    throw UndefinedCacheReplacementPolicyName("\"" + name + "\" replacement policy is not defined, supported polices are:\nLRU\npseudo-LRU\n");
}