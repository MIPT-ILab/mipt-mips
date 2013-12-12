/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */


// Generic C
#include <libelf.h>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <gelf.h>
#include <cstdlib>
#include <cerrno>
#include <cassert>

// Generic C++
#include <sstream>

// uArchSim modules
#include "func_memory.h"
#include "elf_parser.h"

Memory::~Memory()
{
	for (int i = 0; i < this->MemSize; i++)
	{
		for (int j = 0; j < this->SegSize; j++)
		{
			if (this->ExistPage(i, j))
			{
				delete[] this->data[i][j];
			}
		}
		delete[] this->data[i];
	}
	delete[] this->data;
}


Memory::Memory(uint64 memsize, uint64 segsize, uint64 pagesize)
{
	this->MemSize = memsize;
	this->SegSize = segsize;
	this->PageSize = pagesize;

	this->data = new uint8**[this->MemSize];

	for (int i = 0; i < this->MemSize; i++)
	{
		this->data[i] = new uint8*[this->SegSize];
		
#ifdef ZEROING_ENABLE	
	
		for (int j = 0; j < this->SegSize; j++)
		{
			this->data[i][j] = NULL;
		}
		
#endif	//ZEROING_ENABLE

	}

}


bool Memory::ExistPage(uint64 segnum, uint64 pagenum)
{
	if (segnum >= this->MemSize) return false;
	if (pagenum >= this->PageSize) return false;
	return (this->data[segnum][pagenum]);
}


bool Memory::CreatePage(uint64 segnum, uint64 pagenum)
{
	if (!(this->ExistPage(segnum, pagenum)))
	{
		this->data[segnum][pagenum] = new uint8[this->PageSize];
		
#ifdef ZEROING_ENABLE		

		for (int i = 0; i < this->PageSize; i++)
		{
			this->data[segnum][pagenum][i] = 0;
		}
		
#endif	//ZEROING_ENABLE
	
		return true;
	}
	cerr << "CreatePage: Page " << segnum << ":" << pagenum << "already exists!" << endl;
	return false;
}


bool Memory::WriteData(uint64 segnum, uint64 pagenum, uint64 bytenum, uint8 value)
{
	if (!(this->ExistPage(segnum, pagenum)))
	{
		cerr << "WriteData: No page " << segnum << ":" << pagenum << endl;
		return false;
	}
	this->data[segnum][pagenum][bytenum] = value;
	return true;
}


void  Memory::WriteDataHard(uint64 segnum, uint64 pagenum, uint64 bytenum, uint8 value)
{
	if (!(this->ExistPage(segnum, pagenum)))
	{
		this->CreatePage(segnum, pagenum);
	}
	this->WriteData(segnum, pagenum, bytenum, value);
}


uint8 Memory::ReadData(uint64 segnum, uint64 pagenum, uint64 bytenum)
{
	if (!(this->ExistPage(segnum, pagenum)))
	{
		cerr << "ReadData: No page " << segnum << ":" << pagenum << endl;
		return 0;
	}
	return this->data[segnum][pagenum][bytenum];
}
/////////////////////////////////////////////////////////



FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
{

	AddrSize = addr_size;
	PageBits = page_bits;
	OffsetBits = offset_bits;

	uint64 memsize = pow(2, addr_size - page_bits - offset_bits);
	uint64 segsize = pow(2, page_bits);
	uint64 pagesize = pow(2, offset_bits);

	this->memory = new Memory(memsize, segsize, pagesize);
	
	vector<ElfSection> sections_array;
    ElfSection::getAllElfSections( executable_file_name, sections_array);

    for ( uint64 i = 0; i < sections_array.size(); ++i)
    {
		if (!strcmp(sections_array[i].name,".text"))
		{
			this -> StartAdrr =  sections_array[i].start_addr;
		}

		for (uint64 j = 0; j < sections_array[i].size; j++)
		{
			this->write(sections_array[i].content[j], sections_array[i].start_addr+j, 1);
		}
	}
    
}

FuncMemory::~FuncMemory()
{
    //
}

uint64 FuncMemory::startPC() const
{
	return this->StartAdrr;
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
	uint64 segnum;
	uint64 pagenum;
	uint64 bytenum;

	intdata data;
	data.data64 = 0;
	
	if ((num_of_bytes<=0)||(num_of_bytes>8))
	{
			exit(EXIT_FAILURE);
	}
	
	for (uint64 i = 0; i < num_of_bytes; i++)
	{
		bytenum = addr%((uint64)(pow(2, this->OffsetBits)));
		pagenum = (addr / ((uint64)(pow(2, this->OffsetBits))))%((uint64)(pow(2, this->PageBits)));
		segnum = addr / ((uint64)(pow(2, (this->OffsetBits + this->PageBits))));
		if (!(this->memory->ExistPage(segnum,pagenum)))
		{
			cerr << "read: Page doesn`t exist" << endl;
			exit(EXIT_FAILURE);
		}
		data.data8[i] = this->memory->ReadData(segnum, pagenum, bytenum);
		addr++;
	}

	return data.data64;
}

void FuncMemory::write(uint64 value, uint64 addr, unsigned short num_of_bytes)
{
	uint64 segnum;
	uint64 pagenum;
	uint64 bytenum;

	intdata data;
	data.data64 = value;
	
	if ((num_of_bytes<=0)||(num_of_bytes>8))
	{
		cerr << "write: Num_of_bytes = " << num_of_bytes << ". Invalid value." << endl;
		exit(EXIT_FAILURE);
	}
	
	for (uint64 i = 0; i < num_of_bytes; i++)
	{
		bytenum = addr%((uint64)(pow(2, this->OffsetBits)));
		pagenum = (addr / ((uint64)(pow(2, this->OffsetBits))))%((uint64)(pow(2, this->PageBits)));
		segnum = addr / ((uint64)(pow(2, (this->OffsetBits + this->PageBits))));

		this->memory->WriteDataHard(segnum, pagenum, bytenum, data.data8[i]);
		addr++;
	}
}

uint64 FuncMemory::getAddr(uint64 segnum, uint64 pagenum, uint64 bytenum) const
{
	uint64 addr = 0;
	addr = addr + segnum * pow(2,(this->PageBits+this->OffsetBits));
	addr = addr + pagenum * pow(2, (this->OffsetBits));
	addr = addr + bytenum;
	return addr;
}

string FuncMemory::dump( string indent) const
{
	ostringstream oss;
	intdata data;
	data.data64 = 0;

    oss << indent << "Dump memory: "<< endl << "Size: "; 
	oss	<< this->memory->_MemSize() << ":";
	oss	<< this->memory->_SegSize() << ":";
	oss	<< this->memory->_PageSize() << ":";
	oss	<< endl;
    oss << indent << "  Content:" << endl;

	for (uint64 i = 0; i < this->memory->_MemSize(); i++)
	{
		for (uint64 j = 0; j < this->memory->_SegSize(); j++)
		{
			if (this->memory->ExistPage(i, j))
			{
				for (uint64 k = 0; k < this->memory->_PageSize(); k += 4)
				{
					data.data8[3] = this->memory->ReadData(i, j, k);
					data.data8[2] = this->memory->ReadData(i, j, k+1);
					data.data8[1] = this->memory->ReadData(i, j, k+2);
					data.data8[0] = this->memory->ReadData(i, j, k+3);
					uint64 addr = this-> getAddr(i, j, k);
					if (data.data64 != 0) oss <<"0x"<<hex<<addr<<dec<<" :\t" <<hex<<data.data64<<dec<< endl;
				}
			}
		}
	}
	return oss.str();
}
