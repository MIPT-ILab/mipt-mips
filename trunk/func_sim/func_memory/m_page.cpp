//C++ lib
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

//uArch lib
#include <types.h>

//My_Lib
#include <m_page.h>

using namespace std;

///////////////////////////////////////////////////////////////////////

Page :: Page ()
		{
		}

///////////////////////////////////////////////////////////////////////

Page :: Page ( uint64 p_bits )
		{	
			size = pow ( 2, p_bits );
			data_page = new uint8[size];
			//memset ( data_page, 0, size * sizeof ( uint8 ) );			
		}

//////////////////////////////////////////////////////////////////////

Page :: Page ( Page &pg )
		{
			size = pg.size;
			data_page = new uint8[size];

			memcpy ( data_page, pg.data_page, size * sizeof ( uint8 ) );
		}

//////////////////////////////////////////////////////////////////////

Page :: ~Page ()
		{
			delete [] data_page;		
		}

//////////////////////////////////////////////////////////////////////
