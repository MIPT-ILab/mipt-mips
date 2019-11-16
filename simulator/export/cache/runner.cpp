/**
 * Standalone cache simulator
 * @author Pavel Kryukov
 */
 
#include "runner.h"

#include <infra/cache/cache_tag_array.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

std::ostream& operator<<( std::ostream& out, const CacheRunnerResults& rhs)
{
    return out << "total accesses: " << rhs.accesses << std::endl
        << "hit rate: "  << (rhs.get_hit_rate() * 100) << '%' << std::endl
        << "miss rate: " << (rhs.get_miss_rate() * 100) << '%' << std::endl;
}

static void run_single_access( CacheTagArray* cache, Addr addr, CacheRunnerResults* result)
{
    result->accesses++;
    if ( cache->lookup( addr))
        result->hits++;
    else
        cache->write( addr);
}     

CacheRunnerResults run_cache( CacheTagArray* cache, const std::string& filename)
{
    CacheRunnerResults result;
    boost::property_tree::ptree trace;
    read_json( filename, trace);

    for ( const auto& access : trace.get_child( "memory_trace"))
        run_single_access( cache, std::stoul( access.second.get_value<std::string>(), nullptr, 0), &result);
    
    return result;
}
