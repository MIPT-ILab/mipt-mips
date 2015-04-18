#include <cache_tag_array.h>
#include <math.h>
#include <iostream>

CacheTagArray::CacheTagArray( unsigned int size_in_bytes,
                   unsigned int ways,
                   unsigned short block_size_in_bytes,
                   unsigned short addr_size_in_bits)
{
    tagArray = (uint64*)calloc( size_in_bytes, sizeof( uint64));

    ways_num = ways;

    cache_size = size_in_bytes;

    way_size = size_in_bytes / ways;
    line_index_bits = log2( way_size / block_size_in_bytes);

    block_bits = log2( block_size_in_bytes);
    addr_bits = addr_size_in_bits;
    tag_bits = addr_bits - line_index_bits - block_bits;

    line_index_mask = ( ( 1 << line_index_bits) - 1) << block_bits;
    tag_mask = ( ( 1 << tag_bits) - 1);
    tag_mask = tag_mask << ( line_index_bits + block_bits);

    init_LRU();
}

uint64 CacheTagArray::get_tag(uint64 addr){
    return addr & tag_mask;
}

uint64 CacheTagArray::get_line_index(uint64 addr){
    return (addr & line_index_mask) >> block_bits;
}

bool CacheTagArray::read(uint64 addr){
    uint64 tag = get_tag(addr);
    uint64 line_index = get_line_index(addr);

    for(int i = 0; i < ways_num; i++)
        if(tagArray[ line_index*ways_num + i] == tag){
            LRU_refresh(line_index, i);
            //LRU_print(line_index);
            return true;
        }

    write(addr);
    //LRU_print(line_index);
    return false;
}

void CacheTagArray::write(uint64 addr){
    uint64 tag = get_tag(addr);
    uint64 line_index = get_line_index(addr);

    int i = LRU_get(line_index);
    tagArray[ line_index*ways_num + i] = tag;
}

int CacheTagArray::LRU_get(uint64 line_index){
    int i = 0;
    for(int j = 0; j < ways_num; j ++)
        if( LRU_array[line_index*ways_num + j] < LRU_array[line_index*ways_num + i])
            i = j;
    LRU_refresh(line_index, i);
    return i;
}

void CacheTagArray::LRU_refresh(uint64 line_index, uint64 mostRecentUsed){
    for(int i = 0; i < ways_num; i ++)
        if( LRU_array[line_index*ways_num + i] > LRU_array[line_index*ways_num + mostRecentUsed])
            LRU_array[line_index*ways_num + i] --;
    LRU_array[line_index*ways_num + mostRecentUsed] = ways_num-1;
}

void CacheTagArray::LRU_print(uint64 line_index){
    for(int j = 0; j < ways_num; j ++)
        std::cout << LRU_array[line_index*ways_num + j] << " ";
    std::cout << std::endl;
}

/***********************************/

void CacheTagArray::init_LRU(){
    LRU_array = (unsigned int *)calloc(cache_size, sizeof(unsigned int));
    for(int i = 0; i < cache_size; i ++)
        LRU_array[i] = i%ways_num;
}
