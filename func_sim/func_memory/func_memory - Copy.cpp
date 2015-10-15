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
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream>


void MemoryCalculator(uint64* first, uint64* second, uint64* third, uint64 address, 
    uint8* content, uint64 total_size, uint64 middle_size, uint64 right_size)
{

    *first = address >> (middle_size + right_size);
    *second = (address - (*first << (middle_size + right_size))) >> right_size;
    *third = address - (*first << (middle_size + right_size)) - (*second << right_size);
    //int* addr = (int*)address;
//		cout << dec << *first << " " << *second <<" " << *third << " " << address << hex << " val:"<<  (uint32)*content << endl << endl;

}

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
{
    ElfSection::getAllElfSections(executable_file_name, sections_array );
    Memory = NULL;
    Addr_size = addr_size;
    Page_num_size = page_bits;
    Offset_size = offset_bits;
    addr_count = pow(2, Addr_size - Page_num_size - Offset_size);
    page_num_count = pow(2, Page_num_size);
    offset_count = pow(2, Offset_size);
    cout << Addr_size << " " << Page_num_size << " " << Offset_size << " " << endl;
    cout << addr_count << " " << page_num_count << " " << offset_count << " " << endl;

    Memory = new uint8**[addr_count];
    memset(Memory, 0, addr_count*sizeof(uint8**));
    assert(Memory);
    int i = 0;
    uint64 first, second, third;
    while (sections_array.size() > i) //sections_array[i].name != NULL
    {
        cout << "size" << sections_array[i].size;
        
        
        cout << "creating\n " << endl;
        for (int j = 0; j < sections_array[i].size; j++)
        {

            MemoryCalculator(&first, &second, &third, sections_array[i].start_addr + j * sizeof(uint8), sections_array[i].content + j * 1, Addr_size, Page_num_size, Offset_size);
            cout << "memorycalculated" << first << " " << sections_array[i].start_addr + j * sizeof(uint8) << " " << sections_array[i].name << endl;
            if (Memory[first] == NULL)
            {
                Memory[first] = new uint8*[page_num_count];
                memset(Memory[first], 0, page_num_count*sizeof(uint8*));

            }
            cout << "passedfirst\n" << endl;
            if (Memory[first][second] == NULL)
            {

                if (Offset_size < 32)
                {
                    Memory[first][second] = new uint8[offset_count];
                    memset(Memory[first][second], 0, offset_count*sizeof(uint8));

                    //cout << "lvl2";
                }
                else 
                {
                    Memory[first][second] = new uint8[1000];
                    memset(Memory[first][second], 0, 1000*sizeof(uint8));

                    cout << "lvl2";
                }
            }
            cout << "passedsecond\n" << endl;

        if (Offset_size < 32)	Memory[first][second][third] = (uint8)*(sections_array[i].content + j);

        }


        //if (Offset_size < 32)
        
        
            //cout << first << " " << second << " " << third << "-<afterfunct" << endl;
        i++;
    }

    cout << "created\n";
    //cout << "MEM:::::::" << (uint32)Memory[1][0][149];
}

FuncMemory::~FuncMemory()
{
    //if (Offset_size < 32) 
    {
        for (int i = 0; i < addr_count; i++)
            for (int j = 0; j < page_num_count; j++)
                if (Memory[i] != NULL)
                    if(Memory[i][j] != NULL)delete[] Memory[i][j];
        for (int i = 0; i < addr_count; i++)
            if(Memory[i] != NULL)
                delete[] Memory[i];
        

        if (Memory!= NULL)
        delete[] Memory;
    }
    cout << "bb\n";
    // put your code here
}

uint64 FuncMemory::startPC() const
{
    int i = 0;
    while (strcmp(".text", sections_array[i].name) != 0)i++;
    return sections_array[i].start_addr;
    // put your code here
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    cout << "startread";
    if (!num_of_bytes)abort();
    uint64 first, second, third;

    uint64 returnvalue = 0;
    for (int j = num_of_bytes - 1; j >= 0; j--)//(int j = 0; j < num_of_bytes; j++)
    {
    MemoryCalculator(&first, &second, &third, addr + j * sizeof(uint8), (uint8*)(addr + j * 1), Addr_size, Page_num_size, Offset_size);
    if (first >= addr_count || second >= page_num_count || third >= offset_count) abort();
    if (Memory[first]==NULL) abort();
    if (Memory[first][second] == NULL) abort();

    

    returnvalue = 256 * returnvalue + (uint32)Memory[first][second][third];
    
    }
    cout << "endread";
    return returnvalue;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    if (!num_of_bytes)abort();
    uint64 first, second, third;
    cout << "start";
    for (int j = 0; j < num_of_bytes; j++)//(int j = 0; j < num_of_bytes; j++)
    {
        
        MemoryCalculator(&first, &second, &third, addr + j * sizeof(uint8), (uint8*)(addr + j * 1), Addr_size, Page_num_size, Offset_size);
        if (first >= addr_count || second >= page_num_count || third >= offset_count || first < 0 || second < 0|| third < 0) abort();
        
        if (Memory[first] == NULL)
        {
            Memory[first] = new uint8*[page_num_count];
            memset(Memory[first], 0, page_num_count*sizeof(uint8*));

            cout << "fck";

        }
        if (Memory[first][second] == NULL)
            {
                Memory[first][second] = new uint8[offset_count];
                memset(Memory[first][second], 0, offset_count*sizeof(uint8));

            }
        
        if (Offset_size < 32)
            Memory[first][second][third] = (uint32)(value % 256);
        cout << "end";
            value /= 256;

    }

}

string FuncMemory::dump( string indent) const
{
    // put your code here
    return string("ERROR: You need to implement FuncMemory!");
}
