/*
 * bpu.h - the branch prediction unit for MIPS
 * @author George Korepanov <gkorepanov.gk@gmail.com>
 * Copyright 2017 MIPT-MIPS
 */

 #ifndef BRANCH_PREDICTION_UNIT
 #define BRANCH_PREDICTION_UNIT

// C++ generic modules
#include <vector>
#include <memory>

// MIPT_MIPS modules
#include <infra/cache/cache_tag_array.h>
#include <infra/log.h>
#include <infra/types.h>

#include "bpentry.h"

/*
 *******************************************************************************
 *                           BRANCH PREDICTION UNIT                            *
 *******************************************************************************
 */
class BaseBP
{
public:
    virtual bool is_taken( Addr PC) = 0;
    virtual Addr get_target( Addr PC) = 0;
    virtual void update( bool is_taken,
                         Addr branch_ip,
                         Addr target) = 0;

    virtual ~BaseBP() = default;
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
    bool is_taken( Addr PC) final
    {
        uint32 way;
        bool is_hit;
        // do not update LRU information on prediction,
        // so "no_touch" version of "tags.read" is used:
        std::tie( is_hit, way) = tags.read_no_touch( PC);
        if ( is_hit) // hit
            return data[ way][ tags.set(PC)].is_taken( PC);

        return false;
    }

    Addr get_target( Addr PC) final
    {
        uint32 way;
        bool is_hit;
        // do not update LRU information on prediction,
        // so "no_touch" version of "tags.read" is used:
        std::tie( is_hit, way) = tags.read_no_touch( PC);

        // return saved target only in case it is predicted taken
        if ( is_hit && is_taken( PC))
            return data[ way][ tags.set(PC)].getTarget();

        return PC + 4;
    }

    /* update */
    void update( bool is_taken,
                 Addr branch_ip,
                 Addr target) final
    {
        uint32 set = tags.set( branch_ip);
        uint32 way;
        bool is_hit;
        std::tie( is_hit, way) = tags.read( branch_ip);

        if ( !is_hit) { // miss
            way = tags.write( branch_ip); // add new entry to cache
            T& entry = data[ way][ set];
            entry.reset();
            entry.update_target( target);
        }

        data[ way][ set].update( is_taken, target);
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
        virtual ~BaseBPCreator() = default;
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
    };

    const std::map<std::string, BaseBPCreator*> map;

public:
    BPFactory() :
        map({ { "static_always_taken",   new BPCreator<BPEntryAlwaysTaken>},
              { "static_backward_jumps", new BPCreator<BPEntryBackwardJumps>},
              { "dynamic_one_bit",       new BPCreator<BPEntryOneBit>},
              { "dynamic_two_bit",       new BPCreator<BPEntryTwoBit>},
              { "adaptive_two_level",    new BPCreator<BPEntryAdaptive<2>>}})
    { }

    std::unique_ptr<BaseBP> create( const std::string& name,
                    uint32 size_in_entries,
                    uint32 ways,
                    uint32 branch_ip_size_in_bits = 32) const
    {
        if ( map.find(name) == map.end())
        {
             std::cerr << "ERROR. Invalid branch prediction mode " << name << std::endl
                       << "Supported modes:" << std::endl;
             for ( const auto& map_name : map)
                 std::cerr << "\t" << map_name.first << std::endl;

             std::exit( EXIT_FAILURE);
        }

        return map.at( name)->create( size_in_entries, ways, branch_ip_size_in_bits);
    }

    ~BPFactory()
    {
        for ( auto& elem : map)
            delete elem.second;
    }
};

#endif
