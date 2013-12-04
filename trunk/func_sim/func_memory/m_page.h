#ifndef _VK_PAGE_T_
#define _VK_PAGE_T_
//uArch lib
#include <types.h>

//C++ lib

using namespace std;

class Page 
{
private:
	Page ();
public:
	uint64 size;
	uint64 start_addr;
	uint8 *data_page;
	~Page ();
	Page ( uint64 p_bits );
	Page ( Page &pg );
	 
	//fil_page ( uint64 addr, string *data ); 
};




#endif //_VK_PAGE_T_
