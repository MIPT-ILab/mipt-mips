//C++ lib
#include <iostream>
#include <cmath>
#include <cstring>
#include <cstdlib>

//uArch lib
#include <types.h>

//My_Lib
#include <m_memory.h>
#include <m_segment.h>
#include <m_page.h>

using namespace std;

///////////////////////////////////////////////////////////////////////

Memory :: Memory ()
		{
			cout << "I need addr_size, page_bits and offset_bits\n";
			exit ( EXIT_FAILURE );		
		}

///////////////////////////////////////////////////////////////////////

Memory :: Memory ( uint64 addrSize, uint64 pageBits, uint64 offsetBits )
		{
			//start check
			if ( ( addrSize > 64 ) || ( pageBits > 64 ) || ( offsetBits > 64 ) )
			{
				cout << "ERROR : Size of memomy, page and offset mustn't be more than 64!\n" 
				<< "addr_size = " << addrSize
				<< "\npage_bits = " << pageBits
				<< "\noffset_bits = " << offsetBits 
				<< endl;
				exit ( EXIT_FAILURE );
			}
			addr_size = addrSize;
			page_bits = pageBits;
			offset_bits = offsetBits;
			
			num_seg = pow ( 2, addr_size - page_bits - offset_bits );
			if ( addr_size != 64 ) 
				mem_size = pow ( 2, addr_size );
			else 
				mem_size = -1;

			mem.resize(num_seg, NULL);						
		}

///////////////////////////////////////////////////////////////////////

Memory :: Memory ( Memory &m )
		{
			addr_size = m.addr_size;
			page_bits = m.page_bits;
			offset_bits = m.offset_bits;
			num_seg = m.num_seg;
	
			mem.resize(num_seg, NULL);
			mem = m.mem;  
		}

///////////////////////////////////////////////////////////////////////

int Memory :: filling ( uint64 addr, uint8  data )
		{
			//start check addr
			if ( addr > mem_size )
			{
				cout << "ERROR : Addr 0x" << hex << addr << dec << mem_size 
				<< " is to big\n";
				return -1;
			}
			uint64 offset = 0;
			uint64 num_page = 0;
			uint64 num_seg = 0;
	
			//get offset
			offset = ( addr << ( (sizeof(addr)*8) - offset_bits )) >>  ( (sizeof(addr)*8) - offset_bits );  
			//get num_page
			num_page = ( addr << ( (sizeof(addr)*8)- offset_bits - page_bits) ) >> ( (sizeof(addr)*8) - page_bits );
			//get num_seg
			num_seg = addr >> (offset_bits + page_bits);
									
			//create new segment and new page
			if ( !(this -> mem[num_seg]) )
			{			
				Segment *new_seg = new Segment( page_bits, offset_bits );
				if ( new_seg <= 0 )
				{
					cout << "ERROR: memory isn't allocate\n";
					return -1;
				}	
				this -> mem[num_seg] = new_seg;
			}
			
			if ( !(this -> mem[num_seg] -> Seg[num_page]) )
			{
				Page *new_page = new Page ( offset_bits );
				if ( new_page <= 0 )
				{
					cout << "ERROR: memory isn't allocate\n";
					return -1;
				}	
				
				this -> mem[num_seg] -> Seg[num_page] = new_page;			
				this -> mem[num_seg] -> Seg[num_page] -> start_addr = ( addr >> offset_bits ) << offset_bits;
			
			}
			
			//fill new page
			this -> mem[num_seg] -> Seg[num_page] -> data_page[offset] = data;
			return 0;
		}

//////////////////////////////////////////////////////////////////////

Memory :: ~Memory ()
		{
			for ( unsigned int i = 0; i < num_seg; ++i )
			{
				if ( mem[i] )
					delete mem[i];
			}
		}

//////////////////////////////////////////////////////////////////////











