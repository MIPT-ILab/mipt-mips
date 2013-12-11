#ifndef _M_MEMORY_
#define _M_MEMORY_
//C++ lib
#include <vector>

//uArch lib
#include <types.h>

//My_Lib
#include <m_segment.h>

using namespace std;

class Memory
{
private:
	Memory ();	 
public:
	vector<Segment*> mem;
	uint64 mem_size;	
	uint64 num_seg;	
	uint64 addr_size;
	uint64 page_bits;
	uint64 offset_bits;
	
	Memory ( uint64 addr_size, uint64 page_bits,
		 uint64 offset_bits  );
	~Memory ();
	Memory ( Memory &m );

	int filling ( uint64 addr, uint8 data );  
};


#endif //_M_MEMORY_
