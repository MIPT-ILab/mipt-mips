#include<cache_tag_array.h>
#include<fstream>
#include<iomanip>
#include<cstdlib>
#include<string.h>

#define NUMBER_OF_BLOCKS 4
#define ADDR_SIZE_IN_BITS 32 

using namespace std;

int main( int argc , char* argv[])
{
	
	if ( argc != 3)
	{
		cout << "INVALID NUMBER OF ARGUMENTS" << endl;
		return 0;
	}
	
	if ( ( strcmp( argv[ 1],"mem_trace.txt" ) != 0 ) && ( strcmp( argv[ 2],"miss_rate.csv" ) == 0 ))
	{
		cout << "INVALID NAME OF THE FIRST ( INPUT) FILE" << endl;
		return 0;
	}
	
	if ( ( strcmp( argv[ 1],"mem_trace.txt" ) == 0 ) && ( strcmp( argv[ 2],"miss_rate.csv" ) != 0 ))
	{
		cout << "INVALID NAME OF THE SECOND ( OUTPUT) FILE" << endl;	
		return 0;
	}
		
	if ( ( strcmp( argv[ 1],"mem_trace.txt" ) != 0 ) && ( strcmp( argv[ 2],"miss_rate.csv" ) != 0 ))
	{
		cout << "INVALID NAMES OF BOTH FILES" << endl;
		return 0;
	}
	
	bool is_fully = false;
	int number_of_ways = 1;
	
	ofstream out;  
	out.open( argv[ 2]);
	
	for(;;)
	{
		for ( unsigned int size_in_bytes = 1024; size_in_bytes <= 1024 * 1024; size_in_bytes = size_in_bytes * 2 )
		{
			
			float hit_number = 0;
			float miss_number = 0;
			float miss_rate = 0;
			
			if ( is_fully == true)
			{
				number_of_ways = size_in_bytes / NUMBER_OF_BLOCKS;
			}
			
			CacheTagArray* cache = new CacheTagArray( size_in_bytes, number_of_ways, NUMBER_OF_BLOCKS, ADDR_SIZE_IN_BITS);
			ifstream in;
			in.open( argv[ 1]);
			uint32 address;
	
			while ( !in.eof())
			{
				bool is_hit = false;
				
				in >> hex >> address;
				is_hit = cache->read( address);
			
				if ( is_hit)
				{
					hit_number+=1;
				}
				else
				{
					miss_number+=1;
				}
			}
			
			in.close();
			miss_rate = miss_number / ( hit_number + miss_number);
			out << miss_rate << ", ";
			delete cache;
		}
		
		if ( is_fully == true)
		{
			break;
		}
		
		if ( number_of_ways < 16)
		{
			number_of_ways = number_of_ways * 2;
		}
		else
		{
			is_fully = true;
		}
		
		out << "\n" << endl;
	}
	
	out.close();
	return 0;
}