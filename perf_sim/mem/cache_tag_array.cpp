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
        if(tagArray[ line_index + i*way_size] == tag) return true;

    write(addr);
    LRU_print(line_index);
    return false;
}

void CacheTagArray::write(uint64 addr){
    uint64 tag = get_tag(addr);
    uint64 line_index = get_line_index(addr);

    int i = LRU(line_index);
    tagArray[ line_index + i*way_size] = tag;
}

int CacheTagArray::LRU(uint64 line_index){
    int i = 0;
    for(int j = 0; j < ways_num; j ++)
        if( LRU_array[line_index + j*way_size] < LRU_array[line_index + i*way_size])
            i = j;
    for(int j = 0; j < ways_num; j ++)
        if( LRU_array[line_index + j*way_size] > 0)
            LRU_array[line_index + j*way_size]-- ;
    LRU_array[line_index + i*way_size] = ways_num - 1;
    return i;
}

void CacheTagArray::LRU_print(uint64 line_index){
    for(int j = 0; j < ways_num; j ++)
        std::cout << LRU_array[line_index + j*way_size] << " ";
    std::cout << std::endl;
}

void CacheTagArray::init_LRU(){
    LRU_array = (unsigned short *)calloc(ways_num*way_size, sizeof(unsigned short));
    for(int i = 0; i < way_size; i ++)
        for(int j = 0; j < ways_num; j ++)LRU_array[i + j*way_size] = j;
}
