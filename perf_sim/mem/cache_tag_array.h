/*
 * header for cache_tag_array.cpp (simple cache model)
 * @author Semyon Abramov semyon.abramov.mipt@gmail.com
 * iLab project MIPT-MIPS 2015
 */

#ifndef CACHE_TAG_ARRAY
#define CACHE_TAG_ARRAY

// Generic elf library
#include<types.h>

// Generic C++
#include<iostream>
#include<string>
#include<fstream>
#include<cassert>

// Generic C
#include<math.h>

using namespace std;


struct Cache
{
	uint32 value;
	int age;                                                // age is needed for LRU methods
	bool is_empty;                                          // this bit is needed to calculate compulsory misses
	Cache():value( 0ull), age ( 0), is_empty( true) { } 
};


class CacheTagArray
{
	private:
		unsigned int num_of_blocks;
		int ways;
		int blocks_in_way;
		unsigned long long clock;                
		int set_size_in_bits;
		int offset_size_in_bits;
		Cache* Cache_array;
	
	public:
		CacheTagArray( unsigned int size_in_bytes,
					   unsigned int ways,
					   unsigned short block_size_in_bytes = 4, 
					   unsigned short addr_size_in_bits = 32);
		
		~CacheTagArray();			   
		
		bool read ( uint64 addr);
		void write ( uint64 addr);
		unsigned int getSet( uint64 addr);
		unsigned int getTag( uint64 addr);
		
		/* === LRU methods === */
		
		/*	
		 *  Next method updates LRU information
		 *  making block's age equal to current clock.
		 *  So block with least clock is the first 
		 *  candidate for eviction	 
		 */
		
		inline void updateLRU( Cache cell)
		{
			cell.age = clock;
			updateClock();
		}
		inline void updateClock() { clock = clock + 1; }
		int lruIndex( uint64 addr);
};




#endif  // CACHE_TAG_ARRAY