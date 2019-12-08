/**
 * Standalone cache simulator
 * @author Pavel Kryukov
 */
 
#include "runner.h"

#include <infra/cache/cache_tag_array.h>
#include <infra/macro.h>

// #include <sparsehash/dense_hash_map.h> FIXME(pikryuko) install

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <unordered_set>

static void dump_percentage( std::ostream& out, std::string_view name, double value)
{
    out << name << ": " << ( value * 100) << '%' << std::endl;
}

std::ostream& operator<<( std::ostream& out, const CacheRunnerResults& rhs)
{
    out << "total accesses: " << rhs.accesses << std::endl;
    dump_percentage( out, "hit rate", rhs.get_hit_rate());
    dump_percentage( out, "miss rate", rhs.get_miss_rate());
    dump_percentage( out, "compulsory miss rate", rhs.get_compulsory_miss_subrate());
    return out;
}

class CommonCacheRunner : public CacheRunner
{
public:
    explicit CommonCacheRunner( CacheTagArray* cache) : cache( cache)
    {
//        history.set_empty_key( all_ones<PhysAddr>());
    }

    CacheRunnerResults run( const std::string& filename) final;

private:
    void account_access( Addr addr, CacheRunnerResults* result);
    void account_miss( Addr addr, CacheRunnerResults* result);

//    google::dense_hash_set<Addr> history;
    std::unordered_set<Addr> history;
    CacheTagArray* cache;
};

void CommonCacheRunner::account_access( Addr addr, CacheRunnerResults* result)
{
    result->accesses++;
    if ( cache->lookup( addr))
        result->hits++;
    else
        account_miss( addr, result);
}

void CommonCacheRunner::account_miss( Addr addr, CacheRunnerResults* result)
{
    cache->write( addr);
    if ( history.count( addr) == 0) {
        ++result->compulsory_misses;
        history.insert( addr);
    }
}

CacheRunnerResults CommonCacheRunner::run( const std::string& filename)
{
    CacheRunnerResults result;
    boost::property_tree::ptree trace;
    read_json( filename, trace);

    for ( const auto& access : trace.get_child( "memory_trace"))
        account_access( std::stoul( access.second.get_value<std::string>(), nullptr, 0), &result);
    
    return result;
}

std::unique_ptr<CacheRunner> CacheRunner::create( CacheTagArray* cache)
{
    return std::make_unique<CommonCacheRunner>( cache);
}
