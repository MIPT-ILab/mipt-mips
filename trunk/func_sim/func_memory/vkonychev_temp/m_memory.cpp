//C++ lib
#include <iostream>
#include <cmath>
#include <cstring>

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
		}

///////////////////////////////////////////////////////////////////////

Memory :: Memory ( uint64 addrSize, uint64 pageBits, uint64 offsetBits )
		{
			addr_size = addrSize;
			page_bits = pageBits;
			offset_bits = offsetBits;
			num_seg = pow ( 2, addr_size - page_bits - offset_bits );
			
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
			uint64 offset = 0;
			uint64 num_page = 0;
			uint64 num_seg = 0;
		
			int i = 0;
			int k = 1;
			//take offset from addr
			for ( i = 0; i < ( this -> offset_bits ); ++i )
			{
				offset += ( ( addr >> i ) &  1  ) * k;
				k *= 2;
			}
			
			k = 1;
			//take num_page from addr
			for ( ; i < ( ( this -> offset_bits ) + ( this -> page_bits ) ); ++i ) 
			{
				num_page += ( ( addr >> i ) & 1 ) * k;
				k *= 2; 
			}

			k = 1;
			//take num_seg from addr
			for ( ; i < ( this -> addr_size ); ++i )
			{
				num_seg += ( ( addr >> i ) & 1 ) * k;
				k *= 2;
			}
			
			//create new segment and new page
			if ( !(this -> mem[num_seg]) )
			{			
				Segment *new_seg = new Segment( page_bits, offset_bits );	
				this -> mem[num_seg] = new_seg;
			}

			if ( !(this -> mem[num_seg] -> Seg[num_page]) )
			{
				Page *new_page = new Page ( offset_bits );	
				this -> mem[num_seg] -> Seg[num_page] = new_page;
			}
			//fill new page
			this -> mem[num_seg] -> Seg[num_page] -> data_page[offset] = data;
			return 0;
		}

//////////////////////////////////////////////////////////////////////

Memory :: ~Memory ()
		{
		}














