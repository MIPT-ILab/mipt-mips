/**
 * cache_tag_array.h
 * Header for the cache tag array model.
 * @author Oleg Ladin, Denis Los
 */

#ifndef CACHE_TAG_ARRAY_H
#define CACHE_TAG_ARRAY_H

#include <infra/exception.h>
#include <infra/log.h>
#include <infra/types.h>

#include <memory>

struct CacheTagArrayInvalidSizeException final : Exception
{
    explicit CacheTagArrayInvalidSizeException(const std::string& msg)
        : Exception("Invalid cache size", msg)
    { }
};

class CacheTagArray : public Log
{
    public:
        virtual uint32 set( Addr addr) const = 0;
        virtual Addr tag( Addr addr) const = 0;
        virtual int32 write( Addr addr) = 0;   // create new entry in cache
        virtual std::pair<bool, int32> read( Addr addr) = 0;    // lookup the cache and update LRU info
        virtual std::pair<bool, int32> read_no_touch( Addr addr) const = 0; // find in the cache but do not update LRU info
        bool lookup( Addr addr) { return read( addr).first; }; // hit or not

        static std::unique_ptr<CacheTagArray> create(
            const std::string& type,
            uint32 size_in_bytes,
            uint32 ways,
            uint32 line_size,
            uint32 addr_size_in_bits);
};

#endif // CACHE_TAG_ARRAY_H
