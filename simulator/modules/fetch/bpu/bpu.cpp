/*
 * bpu.cc - the branch prediction unit for MIPS
 * @author George Korepanov <gkorepanov.gk@gmail.com>
 * Copyright 2017-2018 MIPT-MIPS
 */

#include "bpentry.h"
#include "bpu.h"

// MIPT_MIPS modules
#include <infra/cache/cache_tag_array.h>
#include <infra/config/config.h>

// C++ generic modules
#include <map>
#include <memory>
#include <sstream>
#include <vector>

namespace config {
    static const Value<std::string> bp_mode = { "bp-mode", "saturating_two_bits", "branch prediction mode"};
    static const Value<std::string> bp_lru = { "bp-lru", "pseudo-LRU", "branch prediction replacement policy"};
    static const Value<uint32> bp_size = { "bp-size", 128, "BTB size in entries"};
    static const Value<uint32> bp_ways = { "bp-ways", 16, "number of ways in BTB"};
} // namespace config

template<typename T>
class BP final: public BaseBP
{
    std::vector<std::vector<T>> directions;
    std::vector<std::vector<Addr>> targets;
    std::unique_ptr<CacheTagArray> tags = nullptr;

    bool is_way_taken( size_t way, Addr PC, Addr target) const
    {
        return directions[ way][ tags->set(PC)].is_taken( PC, target);
    }
public:
    BP( const std::string& lru, uint32 size_in_entries, uint32 ways, uint32 branch_ip_size_in_bits) try
        : directions( ways, std::vector<T>( size_in_entries / ways))
        , targets( ways, std::vector<Addr>( size_in_entries / ways))
    {
        // we're reusing existing CacheTagArray functionality,
        // but here we don't split memory in blocks, storing
        // IP's only, so hardcoding here the granularity of 4 bytes:
        tags = CacheTagArray::create( lru, size_in_entries, ways, 4, branch_ip_size_in_bits);
    }
    catch (const CacheTagArrayInvalidSizeException& e) {
        throw BPInvalidMode( e.what(), "");
    }

    /* prediction */
    bool is_taken( Addr PC) const final
    {
        // do not update LRU information on prediction,
        // so "no_touch" version of "tags->read" is used:
        const auto[ is_hit, way] = tags->read_no_touch( PC);
        return is_hit && is_way_taken( way, PC, targets[ way][ tags->set(PC)]);
    }

    bool is_hit( Addr PC) const final
    {
        // do not update LRU information on this check,
        // so "no_touch" version of "tags->read" is used:
        return tags->read_no_touch( PC).first;
    }

    Addr get_target( Addr PC) const final
    {
        // do not update LRU information on prediction,
        // so "no_touch" version of "tags->read" is used:
        const auto[ is_hit, way] = tags->read_no_touch( PC);

        // return saved target only in case it is predicted taken
        if ( is_hit && is_way_taken( way, PC, targets[ way][ tags->set(PC)]))
            return targets[ way][ tags->set(PC)];

        return PC + 4;
    }

    /* update */
    void update( const BPInterface& bp_upd) final
    {
        const auto set = tags->set( bp_upd.pc);
        auto[ is_hit, way] = tags->read( bp_upd.pc);

        if ( !is_hit) { // miss
            way = tags->write( bp_upd.pc); // add new entry to cache
            auto& entry = directions[ way][ set];
            entry.reset();
        }

        directions[ way][ set].update( bp_upd.is_taken);
        targets[ way][ set] = bp_upd.target;
    }
};

class BPFactory {
    struct BaseBPCreator {
        virtual std::unique_ptr<BaseBP> create( const std::string& lru, uint32 size_in_entries, uint32 ways,
                                               uint32 branch_ip_size_in_bits) const = 0;
        BaseBPCreator() = default;
        virtual ~BaseBPCreator() = default;
        BaseBPCreator( const BaseBPCreator&) = delete;
        BaseBPCreator( BaseBPCreator&&) = delete;
        BaseBPCreator& operator=( const BaseBPCreator&) = delete;
        BaseBPCreator& operator=( BaseBPCreator&&) = delete;
    };

    template<typename T>
    struct BPCreator : BaseBPCreator {
        std::unique_ptr<BaseBP> create( const std::string& lru, uint32 size_in_entries, uint32 ways,
                                       uint32 branch_ip_size_in_bits) const final
        {
            return std::make_unique<BP<T>>( lru, size_in_entries,
                                            ways, branch_ip_size_in_bits);
        }
        BPCreator() = default;
    };

    using Map = std::map<std::string, std::unique_ptr<BaseBPCreator>>;
    const Map map;

    // Use old-fashioned generation since initializer-lists don't work with unique_ptrs
    static Map generate_map() {
        Map my_map;
        my_map.emplace("always_taken", std::make_unique<BPCreator<BPEntryAlwaysTaken>>());
        my_map.emplace("always_not_taken", std::make_unique<BPCreator<BPEntryAlwaysNotTaken>>());
        my_map.emplace("backward_jumps", std::make_unique<BPCreator<BPEntryBackwardJumps>>());
        my_map.emplace("saturating_one_bit", std::make_unique<BPCreator<BPEntryOneBit>>());
        my_map.emplace("saturating_two_bits", std::make_unique<BPCreator<BPEntryTwoBit>>());
        my_map.emplace("adaptive_two_levels", std::make_unique<BPCreator<BPEntryAdaptive<2>>>());
        return my_map;
    }

    std::string print_map() const
    {
        std::ostringstream out;
        out << "Supported branch prediction modes:" << std::endl;
        for ( const auto& map_name : map)
            out << "\t" << map_name.first << std::endl;
        return std::move( out).str();
    }

public:
    BPFactory() : map( generate_map()) { }

    auto create( const std::string& name, const std::string& lru,
                 uint32 size_in_entries, uint32 ways,
                 uint32 branch_ip_size_in_bits) const
    {
        auto it = map.find( name);
        if ( it != map.end())
            return it->second->create( lru, size_in_entries, ways, branch_ip_size_in_bits);

        throw BPInvalidMode( name, print_map());
    }
};

std::unique_ptr<BaseBP> BaseBP::create_bp( const std::string& name, const std::string& lru, 
                                           uint32 size_in_entries, uint32 ways, uint32 branch_ip_size_in_bits)
{
    static const BPFactory factory;
    return factory.create(name, lru, size_in_entries, ways, branch_ip_size_in_bits);
}

std::unique_ptr<BaseBP> BaseBP::create_configured_bp()
{
    return create_bp( config::bp_mode, config::bp_lru, config::bp_size, config::bp_ways, 32);
}
