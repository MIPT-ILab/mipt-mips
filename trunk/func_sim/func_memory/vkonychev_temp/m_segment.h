#ifndef _M_SEGMENT_
#define _M_SEGMENT_
//uArch lib
#include <types.h>

//My_Lib
#include <m_page.h>

//C++ lib
#include <vector>

using namespace std;

class Segment
{
private:
	Segment ();
public:
	vector<Page*> Seg;
	uint64 seg_size;
	uint64 page_size;

	Segment ( uint64 seg_bits, uint64 page_bits );
	Segment ( Segment &seg );
	~Segment ();

	//fil_seg ( uint64 addr, string *data );
};

#endif //_M_SEGMENT_
