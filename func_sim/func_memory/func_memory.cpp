/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <cstdlib>
#include <cerrno>
#include <cassert>
#include <malloc.h>

// Generic C++
#include <iostream>
#include <sstream>

// uArchSim modules
#include <func_memory.h>

//my modules
#include <my_assert.h>
#include <size.h>

FuncMemory::FuncMemory()
{
    cerr << "ERROR: the constructor without parameters "
         << "of FuncMemory class is prohobited" << endl;
        exit(EXIT_FAILURE);
}

//--------------------------------------------------------------------------------------------------------------------------------------

FuncMemory::FuncMemory( const char* executable_file_name,                                                 //success
                        uint64 addr_size,
                        uint64 page_num_size,
                        uint64 offset_size)                //constructor for our memory
{
    if (executable_file_name == NULL)
    {
	cerr << "ERROR. You must choose executable file to parse" << endl;
        exit(EXIT_FAILURE);
    }

    int fd = open(executable_file_name, O_RDONLY);
    if (fd == -1)
    {
        cerr << "ERROR. File doesn't exist" << endl;
        exit(EXIT_FAILURE);
    }
    close(fd);

    this->set_num_size = addr_size - page_num_size - offset_size;
    this->page_num_size = page_num_size;
    this->offset_size = offset_size;

    // count page_size, pages_array_size, sets_array_size

    this->MAX_ADDR = SIZE_IN_ELEMENTS(addr_size);
    this->page_size = SIZE_IN_ELEMENTS(offset_size)+1;
    this->pages_array_size = SIZE_IN_ELEMENTS(page_num_size)+1;
    this->sets_array_size = SIZE_IN_ELEMENTS(set_num_size)+1;

    //inicialization of memory

    this->mem = (uint8***)calloc(this->sets_array_size, sizeof(uint8**));
    MY_ASSERT(this->mem != NULL, "You requaried a very big memory");

    // Get all sections from executable elf file (Elfsections will be saved in Private: sections_array)

    ElfSection::getAllElfSections(executable_file_name, this->sections_array);               
    Write_AllElfSections();                                                          // write elf sections in memory
}

//-------------------------------------------------------------------------------------------------------------------------------------

FuncMemory::~FuncMemory()
{
    destroy_mem();
}

//--------------------------------------------------------------------------------------------------------------------------------------

uint64 FuncMemory::startPC() const
{
    for(uint64 i=0; i < sections_array.size(); i++)
    {
        if (strcmp(sections_array[i].name, ".text") == 0)
        {
            return sections_array[i].start_addr;
        }
    }
    return 0;
}

//--------------------------------------------------------------------------------------------------------------------------------------

uint64 FuncMemory::read(uint64 addr, unsigned short num_of_bytes) const
{
    MY_ASSERT(addr <= this->MAX_ADDR, "Invalid address for reading");
    MY_ASSERT((num_of_bytes <= sizeof(uint64)) && (num_of_bytes > 0), "Number_of_bytes for writing can't be more then sizeof(uint64) or less then 1");
    MY_ASSERT(num_of_bytes+addr-1 <= this->MAX_ADDR, "Attempt to read beyond memory");

    uint64 set_number;
    uint64 page_number;
    uint64 offset;

    pars_addr(addr, set_number, page_number, offset);

    if (this->mem[set_number] != NULL)
    {
        if (this->mem[set_number][page_number] != NULL)
        {
            uint64 res_value = 0;
            if (offset + num_of_bytes <= this->page_size)           // if we will read from one page
            {
                for(uint64 i=0; i <= num_of_bytes-1; i++)
                {
                    res_value = res_value << ((sizeof(uint8)*8));
                    res_value = res_value + mem[set_number][page_number][offset+num_of_bytes-1-i];
                }
                return res_value;

            }
            else                                                      // if we will read from several pages
            {
                uint64 part2 = read(addr+(this->page_size-offset), num_of_bytes-(this->page_size-offset));   //get the part of number which                                                                                                             is situated on the next page
                part2 = part2 << ((this->page_size-offset)*8);

                uint64 part1 = read(addr, this->page_size-offset);    //get the part of number which is situated on this page

                res_value = part2+part1;
                return res_value;

            }
        }
        else
        {
            MY_ASSERT(NULL, "Attempt to read from unallocated memory");
        }
    }
    else
    {
        MY_ASSERT(NULL, "Attempt to read from unallocated memory");
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------

void FuncMemory::write(uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    MY_ASSERT(addr <= this->MAX_ADDR, "Invalid address for writing");
    MY_ASSERT((num_of_bytes <= sizeof(uint64)) && (num_of_bytes > 0), "Number_of_bytes for writing can't be more then sizeof(uint64) or less then 1");
    MY_ASSERT(num_of_bytes+addr-1 <= this->MAX_ADDR, "Attempt to write beyond memory");

    uint64 set_number;
    uint64 page_number;
    uint64 offset;

    pars_addr(addr, set_number, page_number, offset);


    if (offset + num_of_bytes <= this->page_size)                      // if we will write on one page
    {
        if (this->mem[set_number] == NULL)                              // if address doesn't exist then create it
        {
            this->mem[set_number] = (uint8**)calloc(this->pages_array_size, sizeof(uint8*));
            MY_ASSERT(this->mem[set_number] != NULL, "You requaried a very big memory");

            this->mem[set_number][page_number] = (uint8*)calloc(this->page_size, sizeof(uint8));
            MY_ASSERT(this->mem[set_number][page_number] != NULL, "You requaried a very big memory");
        }
        if (this->mem[set_number][page_number] == NULL)
        {
            this->mem[set_number][page_number] = (uint8*)calloc(this->page_size, sizeof(uint8));
            MY_ASSERT(this->mem[set_number][page_number] != NULL, "You requaried a very big memory");
        }

        uint64 temp = 0;                               // temp variable

        for(uint64 i=0; i <= num_of_bytes-1; i++)            //writing value in memory
        {
            temp = value << ((sizeof(uint64)-i-1)*8);
            temp = temp >> ((sizeof(uint64)-1)*8);
            this->mem[set_number][page_number][offset+i] = temp;
        }
    }
    else                                                     // if we will write on several pages
    {
        write(value, addr, (this->page_size-offset));
        uint64 value_next = value >> ((this->page_size-offset)*8);

        write(value_next, addr+(this->page_size-offset), num_of_bytes-(this->page_size-offset));

    }
}

//------------------------------------------------------------------------------------------------------------------------------------

string FuncMemory::dump(string indent) const
{
    ostringstream oss;

    if (this->mem == NULL)
    {
        oss << indent << "Memory is not initialized" << endl;
        return oss.str();
    }

    oss << indent << "Dump for allocated memory:" << endl
        << indent << "Memory size = " << MAX_ADDR/1024/1024/1024 + 1 << " gigabytes" << endl
        << indent << "Set size = " << this->pages_array_size << " pages" << endl
        << indent << "Page size = " << this->page_size << " bytes" << endl
        << indent << endl;

    for(uint64 set_number = 0; set_number < this->sets_array_size; set_number++)             //research the sets_array
    {
        if (this->mem[set_number] != NULL)          // if set exists
        {
            for(uint64 page_number = 0; page_number < this->pages_array_size; page_number++)     //resaearch pages_array
            {
                if (this->mem[set_number][page_number] != NULL)          //if page exists
                {
                    oss << indent << endl;
                    oss << indent << hex << "In set number " << set_number << " Page number " << page_number << endl;
                    oss << indent << endl;
                    oss << indent << page_dump(set_number, page_number);                //invoke dump for page
                }
            }
        }
    }

    return oss.str();
}

//------------------------------------------------------------------------------------------------------------------------------------

string FuncMemory::page_dump(uint64 set_number, uint64 page_number) const
{
     string indent;
     ostringstream oss;

     uint64 addr = (set_number << (this->offset_size+this->page_num_size)) + (page_number << this->offset_size);
     uint64 offset = 0;

     oss << hex;

     while (offset < this->page_size-1)
     {
         uint64 val = read(addr+offset, sizeof(uint32));
         if (val != NULL)
         {
             oss << indent << "0x" << addr+offset << ": " ;
             uint64 temp = 0;
             uint64 mask = MAX_VAL8;

             for(uint64 i = 0; i <= sizeof(uint32)-1; i++)
             {
                 temp = val >> i*8;
                 temp = mask & temp;
                 oss.width(2);               //two hex simbols to print a byte
                 oss.fill('0');              //1 will be printed as 01
                 oss << (uint16) temp;
             }
             
             oss << indent << endl;
         }
         offset = offset + sizeof(uint32);
     }

     return oss.str();
}

//------------------------------------------------------------------------------------------------------------------------------------

// Private functions

void FuncMemory::pars_addr(uint64 addr, uint64 &set_number, uint64 &page_number, uint64 &offset) const
{
    uint64 temp = 0;

    // get the set_number
    set_number = addr >> (this->page_num_size+this->offset_size);

    // get the page_number

    temp = addr - (set_number << (this->page_num_size+this->offset_size));
    page_number = temp >> this->offset_size;

    // get the offset

    temp = addr - (set_number << (this->page_num_size+this->offset_size)) - (page_number << this->offset_size);
    offset = temp;
}

//--------------------------------------------------------------------------------------------------------------------------------------

void FuncMemory::destroy_mem()                   // destruct memory after using
{
    if (this->mem == NULL)                 //if memory is not initialized
    {
        return;
    }

    for(uint64 i=0; i < this->sets_array_size; i++)       //destroy sets
    {
        if (this->mem[i] != NULL)
        {
            for(uint64 j=0; j < this->pages_array_size; j++)   //destroy pages
            {
                if (this->mem[i][j] != NULL)
                {
                    free(this->mem[i][j]);
                }
            }
            free(this->mem[i]);
        }
    }
    free(this->mem);          //destroy head pointer
}

//--------------------------------------------------------------------------------------------------------------------------------------

void FuncMemory::Write_AllElfSections()
{
    for(uint64 sect_number = 0; sect_number < sections_array.size(); sect_number++)
    {
        Write_Section(sect_number);
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------

void FuncMemory::Write_Section(uint64 sect_number)
{
    char* name = sections_array[sect_number].name;                            // name of the elf section (e.g. ".text", ".data", etc)
    uint64 size = sections_array[sect_number].size;                           // size of the section in bytes
    uint64 start_addr = sections_array[sect_number].start_addr;               // the start address of the section
    uint8* content = sections_array[sect_number].content;                     // the row data of the section

    uint64 addr_to_write = start_addr;                                        // address of memory to write
    uint64 value_to_write = 0;                                                // variable for generation value for writing
    uint64 cont_pointer = 0;                                                 //current position in content

    uint64 number_of_records = (size - size % sizeof(uint64))/sizeof(uint64);
    uint64 modulo = size % sizeof(uint64);

    for(uint64 i = 1; i <= number_of_records; i++)
    {
        value_to_write = 0;

        for(uint64 j = 0; j <= sizeof(uint64)-1; j++)        // extract 8 bytes of content
        {
            value_to_write = value_to_write << (sizeof(uint8)*8);
            value_to_write = value_to_write + content[cont_pointer+sizeof(uint64)-1-j];
        }

        write(value_to_write, addr_to_write, sizeof(uint64));

        addr_to_write = addr_to_write + sizeof(uint64);              //increase current values of pointers
        cont_pointer = cont_pointer + sizeof(uint64);

    }

    value_to_write = 0;

    if (modulo != 0)
    {

        for(uint64 j = 0; j <= modulo-1; j++)
        {
            value_to_write = value_to_write << (sizeof(uint8)*8);
            value_to_write = value_to_write + content[cont_pointer+modulo-1-j];
        }

        write(value_to_write, addr_to_write, modulo);
    }
}
