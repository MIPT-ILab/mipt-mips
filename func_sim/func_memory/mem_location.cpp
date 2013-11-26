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
						  uint64 addr_size = 32,
                          uint64 page_num_size = 10,
                          uint64 offset_size = 12);
// addr = set/page/offset
{
	uint64 set_num_size = addr_size - ( page_num_size + offset_size);

	uint64 set_num_mask  = ^( (uint64)(-1) << set_num_size ) << ( offset_size + page_num_size);
	uint64 page_num_mask = ^( (uint64)(-1) << page_num_size) << offset_size;
	uint64 byte_num_mask = ^( (uint64)(-1) << offset_size  );

	set_num  = ( addr & set_num_mask ) >> ( offset_size + page_num_size);
    page_num = ( addr & page_num_mask) >> offset_size;
    byte_num = ( addr & byte_num_mask);
}
