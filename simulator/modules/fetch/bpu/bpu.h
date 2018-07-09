/*
 * bpu.h - the branch prediction unit for MIPS
 * @author George Korepanov <gkorepanov.gk@gmail.com>
 * Copyright 2017-2018 MIPT-MIPS
 */

 #ifndef BRANCH_PREDICTION_UNIT
 #define BRANCH_PREDICTION_UNIT

// C++ generic modules
#include <vector>
#include <memory>
#include <map>

// MIPT_MIPS modules
#include <infra/cache/cache_tag_array.h>
#include <infra/log.h>
#include <infra/types.h>

#include "bp_interface.h"
#include "bpentry.h"

/*
 *******************************************************************************
 *                           BRANCH PREDICTION UNIT                            *
 *******************************************************************************
 */
class BaseBP
{
public:
    virtual bool is_taken( Addr PC) const = 0;
    virtual Addr get_target( Addr PC) const = 0;
    virtual BPInterface get_bp_info( Addr PC) const = 0;
    virtual void update( const BPInterface& bp_upd) = 0;

    BaseBP() = default;
    virtual ~BaseBP() = default;
    BaseBP( const BaseBP&) = default;
    BaseBP( BaseBP&&) = default;
    BaseBP& operator=( const BaseBP&) = default;
    BaseBP& operator=( BaseBP&&) = default;
};


template<typename T>
class BP final: public BaseBP
{
    std::vector<std::vector<T>> data;
    CacheTagArray tags;

public:
    BP( uint32 size_in_entries,
        uint32 ways,
        uint32 branch_ip_size_in_bits) :

        data( ways, std::vector<T>( size_in_entries / ways)),
        tags( size_in_entries,
              ways,
              // we're reusing existing CacheTagArray functionality,
              // but here we don't split memory in blocks, storing
              // IP's only, so hardcoding here the granularity of 4 bytes:
              4,
              branch_ip_size_in_bits)
        { }

    /* prediction */
    bool is_taken( Addr PC) const final
    {
        // do not update LRU information on prediction,
        // so "no_touch" version of "tags.read" is used:
        const auto[ is_hit, way] = tags.read_no_touch( PC);

        return is_hit && data[ way][ tags.set(PC)].is_taken( PC);
    }

    Addr get_target( Addr PC) const final
    {
        // do not update LRU information on prediction,
        // so "no_touch" version of "tags.read" is used:
        const auto[ is_hit, way] = tags.read_no_touch( PC);

        // return saved target only in case it is predicted taken
        if ( is_hit && is_taken( PC))
            return data[ way][ tags.set(PC)].getTarget();

        return PC + 4;
    }

    /* update */
    void update( const BPInterface& bp_upd) final
    {
        const auto set = tags.set( bp_upd.pc);
        auto[ is_hit, way] = tags.read( bp_upd.pc);

        if ( !is_hit) { // miss
            way = tags.write( bp_upd.pc); // add new entry to cache
            auto& entry = data[ way][ set];
            entry.reset();
            entry.update_target( bp_upd.target);
        }

        data[ way][ set].update( bp_upd.is_taken, bp_upd.target);
    }

    BPInterface get_bp_info( Addr PC) const final
    {
        return BPInterface( PC, is_taken( PC), get_target( PC)); 
    }
};


/*
 *******************************************************************************
 *                                FACTORY CLASS                                *
 *******************************************************************************
 */
class BPFactory {
    class BaseBPCreator {
    public:
        virtual std::unique_ptr<BaseBP> create(uint32 size_in_entries,
                                               uint32 ways,
                                               uint32 branch_ip_size_in_bits) const = 0;
        BaseBPCreator() = default;
        virtual ~BaseBPCreator() = default;
        BaseBPCreator( const BaseBPCreator&) = delete;
        BaseBPCreator( BaseBPCreator&&) = delete;
        BaseBPCreator& operator=( const BaseBPCreator&) = delete;
        BaseBPCreator& operator=( BaseBPCreator&&) = delete;
    };

    template<typename T>
    class BPCreator : public BaseBPCreator {
    public:
        std::unique_ptr<BaseBP> create(uint32 size_in_entries,
                                       uint32 ways,
                                       uint32 branch_ip_size_in_bits) const final
        {
            return std::make_unique<BP<T>>( size_in_entries,
                                            ways,
                                            branch_ip_size_in_bits);
        }
        BPCreator() = default;
    };

    using Map = std::map<std::string, std::unique_ptr<BaseBPCreator>>;
    const Map map;

    // Use old-fashioned generation since initializer-lists don't work with unique_ptrs
    static Map generate_map() {
        Map my_map;
        my_map.emplace("static_always_taken",   std::make_unique<BPCreator<BPEntryAlwaysTaken>>());
        my_map.emplace("static_backward_jumps", std::make_unique<BPCreator<BPEntryBackwardJumps>>());
        my_map.emplace("dynamic_one_bit",       std::make_unique<BPCreator<BPEntryOneBit>>());
        my_map.emplace("dynamic_two_bit",       std::make_unique<BPCreator<BPEntryTwoBit>>());
        my_map.emplace("adaptive_two_level",    std::make_unique<BPCreator<BPEntryAdaptive<2>>>());
        return my_map;
    }

public:
    BPFactory() : map( generate_map()) { }

    auto create( const std::string& name,
                 uint32 size_in_entries,
                 uint32 ways,
                 uint32 branch_ip_size_in_bits = 32) const
    {
        auto it = map.find( name);
        if ( it == map.end())
        {
             std::cerr << "ERROR. Invalid branch prediction mode " << name << std::endl
                       << "Supported modes:" << std::endl;
             for ( const auto& map_name : map)
                 std::cerr << "\t" << map_name.first << std::endl;

             std::exit( EXIT_FAILURE);
        }

        return it->second->create( size_in_entries, ways, branch_ip_size_in_bits);
    }
};

#endif
