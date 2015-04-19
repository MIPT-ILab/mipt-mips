/*
 * Simple cache model
 * @author Semyon Abramov semyon.abramov.mipt@gmail.com
 * iLab project MIPT-MIPS 2015
 */

#include<cache_tag_array.h>

using namespace std;


CacheTagArray::CacheTagArray( unsigned int size_in_bytes,
                              unsigned int ways,
                              unsigned short block_size_in_bytes, 
                              unsigned short addr_size_in_bits)
{
	this->num_of_blocks = size_in_bytes / block_size_in_bytes;          // number of lines in cache
	this->offset_size_in_bits = log( block_size_in_bytes) / log( 2);    // offset's size inside the address
	this->ways = ways;
	this->blocks_in_way = num_of_blocks / ways;                         // number of lines in each way
	this->set_size_in_bits = log( blocks_in_way) / log( 2);
	this->clock = 1;                                                    // clock is needed for LRU methods
	Cache_array = new Cache[num_of_blocks];
}

CacheTagArray::~CacheTagArray()
{
	delete Cache_array;
}

unsigned int CacheTagArray::getSet( uint64 addr)
{
	unsigned int set = 0;
	unsigned int temp1 = 0;
	unsigned int temp2 = 0;	
	
	temp1 = addr >> ( offset_size_in_bits + set_size_in_bits);
	temp2 = temp1 << ( offset_size_in_bits + set_size_in_bits);
	set = (addr ^ temp2);
	set = set >> offset_size_in_bits;
	
	return set;
}

unsigned int CacheTagArray::getTag( uint64 addr)
{
	return addr >> ( offset_size_in_bits + set_size_in_bits);
}

/* Next function returns the index of LRU cache line */  

int CacheTagArray::lruIndex( uint64 addr)
{
	unsigned int set  = getSet( addr);
	int temp_age;
	unsigned int index_of_lru_element;
	int i = 0;
	
	temp_age = Cache_array[ i * blocks_in_way + set].age;
	index_of_lru_element = i * blocks_in_way + set;
	
	while ( i < ways)
	{
		if ( Cache_array[ i * blocks_in_way + set].age < temp_age)
		{
			temp_age = Cache_array[ i * blocks_in_way + set].age;
			index_of_lru_element = i * blocks_in_way + set;
		}
		
		++i;
	}
	
	return index_of_lru_element;
	
}

void CacheTagArray::write( uint64 addr)
{
	int index = lruIndex( addr);
	unsigned int tag = getTag( addr);
	
	Cache_array[ index].value = tag;
	Cache_array[ index].age = clock;
	Cache_array[ index].is_empty = false;
	updateClock();
}


bool CacheTagArray::read( uint64 addr)
{
	unsigned int set =  getSet( addr);
	unsigned int tag = getTag( addr);
	int i = 0;
	
	while ( i < ways)
	{
		if ( Cache_array[ i * blocks_in_way + set].value == tag 
			&& Cache_array[ i * blocks_in_way + set].is_empty == false )
		{
			updateLRU( Cache_array[ i * blocks_in_way + set]);           // LRU update
			return true;
		}
		
		++i;
	}
	
	write( addr);
	return false;
}	
	
	
	
	
