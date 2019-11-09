/**
 * Standalone cache simulator
 * @author Pavel Kryukov
 */
 
#ifndef CACHE_RUNNER_H
#define CACHE_RUNNER_H

#include <infra/types.h>
#include <iostream>

class CacheTagArray;

struct CacheRunnerResults
{
    uint64 accesses = 0;
    uint64 hits = 0;

    auto get_hit_rate() const noexcept  { return accesses == 0 ? 0 : double( hits) / accesses; }
    auto get_miss_rate() const noexcept { return 1 - get_hit_rate(); }

    friend std::ostream& operator<<( std::ostream& out, const CacheRunnerResults& rhs);
};

CacheRunnerResults run_cache( CacheTagArray* cache, const std::string& filename);

#endif
