/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C

// Generic C++

// uArchSim modules
#include <func_memory.h>
using namespace std;

uint8 invert(uint8 var)
{
	uint8 inv_var = 0;
	uint8 data_var1 = 0;
	
	inv_var = var << 7;
	for (int i = 6 ; i >= 0; i--)
	{
		data_var1 = ((var >> (7-i)) << 7) >> (7 - i);
		inv_var = data_var1 | inv_var;
	}	
return (inv_var);		
} 



uint64 maskfunc(uint64 addr_size_len, uint64 size_right, uint64 size)
{
	uint64 mask_data1 = 1;
	uint64 mask_data2 = 1;
	uint64 data_data  = 1;
	uint64 use_mask   = 0;

	
	for (int i = (addr_size_len - size_right); i > 0; i--)
		mask_data1 = data_data << 1;
	
	for(int j = 0; j < size; j++)
	{
		for (int i = (addr_size_len - size_right - j); i > 0; i--)
			mask_data2 = data_data<<1;
		
		use_mask = mask_data2 | mask_data1;
		mask_data1 = mask_data2;
	}
	return(use_mask);
}

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
{
	int num = 0;
	vector<ElfSection> SLT_vector;
	uint64 addr_size_tmp = addr_size;
	uint64 page_bits_tmp = page_bits;
	uint64 offset_bits_tmp = offset_bits;
	uint64 start_addres = 0;
		

	ElfSection::getAllElfSections(executable_file_name, SLT_vector);
	num = SLT_vector.size();


	for(int i = 1; i <= num; i++)
	{
		for(int j = 0; j < SLT_vector[i].size; j++)
			write ( SLT_vector[i].content[j], SLT_vector[i].start_addr + j, 1);

		if(SLT_vector[i].name == ".text")
			start_addres = SLT_vector[i].start_addr;	
	}
	
	 
	 
	


}

FuncMemory::~FuncMemory()
{
/**nothing**/		
}

uint64 FuncMemory::startPC() const
{
	return(start_addres);	
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
	uint64 mask_set_bits = 0;
	uint64 mask_page_bits = 0;
	uint64 mask_offset_bits = 0;
	uint64 set_bits = addr_size_tmp - page_bits_tmp - offset_bits_tmp;
	uint64 addr_set = 0;
	uint64 addr_page = 0;
	uint64 addr_offset = 0;
	uint64 value = 0;
	uint64 value_data1 = 0;
	uint64 value_data2 = 0;
	
	mask_set_bits = maskfunc( addr_size_tmp, 0, set_bits);
	mask_page_bits = maskfunc( addr_size_tmp, set_bits, page_bits_tmp);
	mask_offset_bits = maskfunc( addr_size_tmp, set_bits + page_bits_tmp, offset_bits_tmp);
	
	addr_set = (addr & mask_set_bits) >> (page_bits_tmp + offset_bits_tmp);
	addr_page = (addr & mask_page_bits) >> (offset_bits_tmp);
	addr_offset = addr & mask_offset_bits;
	

	if (memory.size() < addr_set)
		return(0);
	else{
		if (memory[addr_set].size() < addr_page)
			return(0);
		else{
			if (memory[addr_set][addr_page].size() < addr_offset)
				return(0);
			else
				value = memory[addr_set][addr_page][addr_offset];
		}
	}
	
	

	for (int i = 1; i < num_of_bytes; i++)
	{
		value_data2 = memory[addr_set][addr_page][addr_offset + i];
		value_data1 = value << 8;
		value = value_data1 | value_data2;	
	}
	
	return (value);
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
	uint64 mask_set_bits = 0;
	uint64 mask_page_bits = 0;
	uint64 mask_offset_bits = 0;
	uint64 set_bits = addr_size_tmp - page_bits_tmp - offset_bits_tmp;
	uint64 addr_set = 0;
	uint64 addr_page = 0;
	uint64 addr_offset = 0;
	uint64 mask_byte[8] = {};
	uint64 mask_data = 1;
	int k = 0;
	int j = 0;
	
	mask_set_bits = maskfunc( addr_size_tmp, 0, set_bits);
	mask_page_bits = maskfunc( addr_size_tmp, set_bits, page_bits_tmp);
	mask_offset_bits = maskfunc( addr_size_tmp, set_bits + page_bits_tmp, offset_bits_tmp);
	
	addr_set = (addr & mask_set_bits) >> (page_bits_tmp + offset_bits_tmp);
	addr_page = (addr & mask_page_bits) >> (offset_bits_tmp);
	addr_offset = addr & mask_offset_bits;
	
	for (int i = 0; i < 8; i++)
		mask_byte[i] = maskfunc(64, i*8, 8);


	for (int i = num_of_bytes - 1; i >= 0; i--)
	{	
		if (memory.size() > addr_set){
			if (memory[addr_set].size() > addr_page){
				if (memory[addr_set][addr_page].size() > addr_offset + i)
					memory[addr_set][addr_page][addr_offset + i] = invert ((value & mask_byte[8 - k]) >> (num_of_bytes - i - 1) * 8);
				else{
					memory[addr_set][addr_page].resize(addr_offset + i + 1);
					memory[addr_set][addr_page][addr_offset + i] = invert ((value & mask_byte[8 - k]) >> (num_of_bytes - i - 1) * 8);
				}
			}
			else{
				memory[addr_set].resize(addr_page + 1);
				if (memory [addr_set][addr_page].size() > addr_offset + i)
					memory[addr_set][addr_page][addr_offset + i] = invert ((value & mask_byte[8 - k]) >> (num_of_bytes - i - 1) * 8);
				else{
					memory[addr_set][addr_page].resize(addr_offset + i + 1);
					memory[addr_set][addr_page][addr_offset + i] = invert ((value & mask_byte[8 - k]) >> (num_of_bytes - i - 1) * 8);
				}
			}
		}
		else{
			memory.resize(addr_set + 1);
			if (memory[addr_set].size() > addr_page){
				if (memory[addr_set][addr_page].size() > addr_offset + i)
					memory[addr_set][addr_page][addr_offset + i] = invert ((value & mask_byte[8 - k]) >> (num_of_bytes - i - 1) * 8);
				else{
					memory[addr_set][addr_page].resize(addr_offset + i + 1);
					memory[addr_set][addr_page][addr_offset + i] = invert ((value & mask_byte[8 - k]) >> (num_of_bytes - i - 1) * 8);
				}
			}
			else{
				memory[addr_set].resize(addr_page + 1);
				if (memory [addr_set][addr_page].size() > addr_offset + i)
					memory[addr_set][addr_page][addr_offset + i] = invert ((value & mask_byte[8 - k]) >> (num_of_bytes - i - 1) * 8);
				else{
					memory[addr_set][addr_page].resize(addr_offset + i + 1);
					memory[addr_set][addr_page][addr_offset + i] = invert ((value & mask_byte[8 - k]) >> (num_of_bytes - i - 1) * 8);
				}
			}
		}	
		k++;
	}

}

string FuncMemory::dump( string indent) const
{
	ostringstream oss;
	int size_set = 0;
	int size_page = 0;
	int size_offset = 0;
	
	size_set = memory.size();
	size_page = memory[size_set].size();
	size_offset = memory[size_set][size_page].size();

	
	oss << indent << "Dump Functional memory " << endl;
	  
	for (int i = 1; i <= size_set; i++)
	{
		for (int j = 1; j <= size_page; j++)
		{
			for (int k = 1; k <=size_offset; k++)
			oss << indent << "  " << i << "  " << j << "  " << k << "  " << endl;
		} 
		
	}   
    
	
    return oss.str();
}
