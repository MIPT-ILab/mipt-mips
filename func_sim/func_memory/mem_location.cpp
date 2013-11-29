/**
 * mem_location.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Anton Mitrokhin <anton.v.mitrokhin@gmail.com>
 * Copyright 2013 uArchSim iLab project
 */

// Generic C

// Generic C++

// uArchSim modules
#include <func_memory.h>




MemLocation::MemLocation( uint64 addr,
                          uint64 addr_size,
                          uint64 page_num_size,
                          uint64 offset_size)
// addr = set/page/offset
{
    uint64 set_num_size = addr_size - ( page_num_size + offset_size);

    uint64 set_num_mask  = ~( (uint64)(-1) << set_num_size ) << ( offset_size + page_num_size);
    uint64 page_num_mask = ~( (uint64)(-1) << page_num_size) << offset_size;
    uint64 byte_num_mask = ~( (uint64)(-1) << offset_size  );

    this->set_num  = ( addr & set_num_mask ) >> ( offset_size + page_num_size);
    this->page_num = ( addr & page_num_mask) >> offset_size;
    this->byte_num = ( addr & byte_num_mask);
}


MemLocation& MemLocation::operator=(const MemLocation& that)
{
    this->set_num  = that.set_num;
    this->page_num = that.page_num;
    this->byte_num = that.byte_num;

    return *this;
}
