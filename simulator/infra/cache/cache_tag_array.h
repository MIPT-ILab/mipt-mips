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
    virtual uint32 set( Addr addr) const = 0; // get set number
    virtual Addr tag( Addr addr) const = 0; // get tag

    /**
     * Mark that the block containing the byte with the given address
     * is stored in the cache.
     *
     * Note: in order to put the given address inside the tags it is needed
     * to select a way where it will be written in.
     * This selection is performed according to LRU (Least Recently Used)
     * policy.
     *
     * Returns # of updated way
     */
    virtual int32 write( Addr addr) = 0;

    /**
     * Returns true and way if the byte with the given address is stored in the cache,
     * otherwise, returns false and -1.
     *
     * This method updates the LRU information.
     */
    virtual std::pair<bool, int32> read( Addr addr) = 0;

    /**
     * Returns true and way if the byte with the given address is stored in the cache,
     * otherwise, returns false and -1.
     *
     * This method DOES NOT update the LRU information.
     */        
    virtual std::pair<bool, int32> read_no_touch( Addr addr) const = 0;

    bool lookup( Addr addr) { return read( addr).first; }; // hit or not

    /**
     * Constructor params:
     *
     * type is a replacement policy and/or ideal cache models
     *
     * size_in_bytes is a number of data bytes that can be stored in the cache,
     *    i.e. if the block size is 16 Bytes then the number of data blocks in the cache is size_in_bytes/16.
     *
     * ways is a number of associative ways in a set, i.e. how many blocks are referred by the same index.
     *
     * block_size_in_bytes is a number of Bytes in a data block
     *
     * addr_size_in_bit is a number of bits in the physical address.
     */
    static std::unique_ptr<CacheTagArray> create(
        const std::string& type,
        uint32 size_in_bytes,
        uint32 ways,
        uint32 line_size,
        uint32 addr_size_in_bits);
};

#endif // CACHE_TAG_ARRAY_H
