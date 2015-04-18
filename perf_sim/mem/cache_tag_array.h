#ifndef CACHE_TAG_ARRAY_H
#define CACHE_TAG_ARRAY_H

#include <stdlib.h>
#include <types.h>

class CacheTagArray
{

public:
    /**
     * Constructor params:
     *
     * 1) size_in_bytes is a number of data bytes that can be stored in the cache,
     *    i.e. if the block size is 16 Bytes then the number of data blocks in the cache is size_in_bytes/16.
     *
     * 2) ways is a number of associative ways in a set, i.e. how many blocks are referred by the same index.
     *
     * 3) block_size_in_bytes is a number of Bytes in a data block
     *
     * 4) addr_size_in_bit is a number of bits in the physical address.
     */
    CacheTagArray( unsigned int size_in_bytes,
                   unsigned int ways,
                   unsigned short block_size_in_bytes,
                   unsigned short addr_size_in_bits);
    /**
     * Return true if the byte with the given address is stored in the cache,
     * otherwise, return false.
     *
     * Note that his method updates the LRU information.
     */
    bool read( uint64 addr);

    /**
     * Mark that the block containing the byte with the given address
     * is stored in the cache.
     *
     * Note: in order to put the given address inside the tags it is needed
     * to select a way where it will be written in.
     * This selection is being done according to LRU (Least Recently Used)
     * policy.
     */
    void write( uint64 addr);

private:
    uint64 * tagArray;
    unsigned int * LRU_array;

    int ways_num;
    int way_size;
    int line_index_bits;
    int block_bits;
    int addr_bits;
    int tag_bits;
    int cache_size;

    uint64 line_index_mask;
    uint64 tag_mask;
    uint64 offset_mask;

    uint64 get_tag(uint64 addr);
    uint64 get_line_index(uint64 addr);

    int LRU_get(uint64 line_index);
    void LRU_refresh(uint64 line_index, uint64 mostRecentUsed);
    void init_LRU();
    void LRU_print(uint64 line_index);
};

#endif
