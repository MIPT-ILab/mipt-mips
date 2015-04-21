#include <cache_tag_array.h>
#include <math.h>
#include <iostream>


uint64 CacheTagArray::get_tag(uint64 addr){
    return addr & tag_mask;
}

bool CacheTagArray::read(uint64 addr){
    uint64 tag = get_tag(addr);
    uint64 line_index = get_line_index(addr);

    for(uint64 i = 0; i < ways_num; i++)
        if(tag_array[ line_index*ways_num + i] == tag){
            lru_update(line_index, i);
            return true;
        }

    write(addr);
    return false;
}

CacheTagArray::CacheTagArray( unsigned int size_in_bytes,
                              unsigned int ways,
                              unsigned short block_size_in_bytes,
                              unsigned short addr_size_in_bits)
{
    tag_array = (uint64 *) calloc( size_in_bytes, sizeof( uint64));

    ways_num = ways;

    cache_size = size_in_bytes;

    way_size = size_in_bytes / ways;
    line_index_bits = ( uint64) ceil( log2( way_size / block_size_in_bytes));

    block_bits = ( uint64) ceil( log2( block_size_in_bytes));
    addr_bits = addr_size_in_bits;
    tag_bits = addr_bits - line_index_bits - block_bits;

    line_index_mask = ( ( 1 << line_index_bits) - 1) << block_bits;
    tag_mask = ( ( 1 << tag_bits) - 1);
    tag_mask = tag_mask << ( line_index_bits + block_bits);

    lru_init();
}


void CacheTagArray::write(uint64 addr){
    uint64 tag = get_tag(addr);
    uint64 line_index = get_line_index(addr);

    uint64 i = lru_get(line_index);
    tag_array[ line_index * ways_num + i] = tag;
}

uint64 CacheTagArray::lru_get(uint64 line_index){
    uint64 i = 0;
    for(uint64 j = 0; j < ways_num; j++)
        if( lru_array[line_index*ways_num + j] < lru_array[line_index*ways_num + i])
            i = j;
    lru_update(line_index, i);
    return i;
}

void CacheTagArray::lru_update(uint64 line_index, uint64 most_recent_used){
    for(uint64 i = 0; i < ways_num; i++)
        if( lru_array[line_index*ways_num + i] > lru_array[line_index * ways_num + most_recent_used])
            lru_array[line_index*ways_num + i] --;
    lru_array[line_index * ways_num + most_recent_used] = ways_num-1;
}

void CacheTagArray::lru_print(uint64 line_index){
    for(uint64 j = 0; j < ways_num; j++)
        std::cout << lru_array[line_index * ways_num + j] << " ";
    std::cout << std::endl;
}

void CacheTagArray::lru_init(){
    lru_array = (uint64 *) calloc(cache_size, sizeof(uint64));
    for(uint64 i = 0; i < cache_size; i++)
        lru_array[i] = i % ways_num;
}

uint64 CacheTagArray::get_line_index(uint64 addr){
    return (addr & line_index_mask) >> block_bits;
}