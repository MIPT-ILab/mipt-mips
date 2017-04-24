/*
 * bpu.h - the branch prediction unit for MIPS
 * @author George Korepanov <gkorepanov.gk@gmail.com>
 * Copyright 2017 MIPT-MIPS
 */

 #ifndef BRANCH_PREDICTION_UNIT
 #define BRANCH_PREDICTION_UNIT

// C++ generic modules
#include <vector>

// MIPT_MIPS modules
#include <perf_sim/mem/cache_tag_array.h>
#include <common/log.h>
#include <common/types.h>

#include "bpentry.h"

/*
 *******************************************************************************
 *                           BRANCH PREDICTION UNIT                            *
 *******************************************************************************
 */
class BaseBP
{
public:
    virtual bool isTaken( Addr PC) = 0;
    virtual Addr getTarget( Addr PC) = 0;
    virtual void update( bool is_taken,
                         Addr branch_ip,
                         Addr target = NO_VAL32) = 0;

    virtual ~BaseBP() { }
};

template<typename T>
class BP : public BaseBP
{
    Addr set_mask;
    std::vector<std::vector<T>> data;
    CacheTagArray tags;


    /* acquire set number from address */
    inline unsigned int set( Addr addr)
    {
        return addr & set_mask;
    }

public:
    BP( unsigned int   size_in_entries,
        unsigned int   ways,
        unsigned int branch_ip_size_in_bits) :

        set_mask( ( size_in_entries / ways ) - 1),
        data( ways, std::vector<T>( size_in_entries / ways)),
        tags( size_in_entries,
              ways,
              1,
              branch_ip_size_in_bits)
        { }

    /* prediction */
    bool isTaken( Addr PC) final
    {
        unsigned int way;
        if ( tags.read_no_touch( PC, &way)) // hit
            return data[ way][ set(PC)].isTaken();

        return false;
    }
    Addr getTarget( Addr PC) final
    {
        unsigned int way;
        if ( tags.read_no_touch( PC, &way)) // hit
            return data[ way][ set(PC)].getTarget();
        else // miss
            return PC + 4;
    }

    /* update */
    void update( bool is_taken,
                 Addr branch_ip,
                 Addr target = NO_VAL32) final
    {
        unsigned int way;
        if ( !tags.read( branch_ip, &way)) // miss
            tags.write( branch_ip, &way); // add new entry to cache

        data[ way][ set( branch_ip)].update( is_taken, target);
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
        virtual std::unique_ptr<BaseBP> create(unsigned int   size_in_entries,
                                               unsigned int   ways,
                                               unsigned int branch_ip_size_in_bits) const = 0;
        virtual ~BaseBPCreator() { }
    };

    template<typename T>
    class BPCreator : public BaseBPCreator {
    public:
        virtual std::unique_ptr<BaseBP> create(unsigned int   size_in_entries,
                                               unsigned int   ways,
                                               unsigned int branch_ip_size_in_bits) const final
        {
            return std::unique_ptr<BaseBP>{ std::make_unique<BP<T>>( size_in_entries,
                                                                     ways,
                                                                     branch_ip_size_in_bits)};
        }
    };

    std::map<std::string, BaseBPCreator*> map;

public:
    /* TODO: create specialisations for static predictors which functions has different parameter list */
    BPFactory() :
        map({ //{ "static_always_taken",   new BPCreator<BPEntryAlwaysTaken>},
              //{ "static_backward_jumps", new BPCreator<BPEntryBackwardJumps>},
              { "dynamic_one_bit",       new BPCreator<BPEntryOneBit>},
              { "dynamic_two_bit",       new BPCreator<BPEntryTwoBit>},
              { "adaptive_two_level",    new BPCreator<BPEntryAdaptive>}})
    { }

    std::unique_ptr<BaseBP> create( const std::string& name,
                    unsigned int   size_in_entries,
                    unsigned int   ways,
                    unsigned int branch_ip_size_in_bits = 32) const
    {
        /* TODO: make this check user-friendly */
        assert( map.count(name));
        return map.at(name)->create( size_in_entries, ways, branch_ip_size_in_bits);
    }

    ~BPFactory()
    {
        for(auto& elem : map)
            delete elem.second;
    }
};

#endif
