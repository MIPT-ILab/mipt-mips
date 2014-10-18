/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C
#include <string.h>

// Generic C++
#include <sstream>

// uArchSim modules
#include <func_memory.h>


FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
{
    assert( executable_file_name);

    SetBits = addr_size - page_bits - offset_bits;
    PageBits = page_bits;
    OffsetBits = offset_bits;

    SetSize = 1;
    PageSize = 1;
    OffsetSize = 1;

    for ( int i = 0; i < SetBits; i ++)
        SetSize *= 2;
    for ( int i = 0; i < OffsetBits; i ++)
        OffsetSize *= 2;
    for ( int i = 0; i < PageBits; i ++)
        PageSize *= 2;

    Memory = new SetMemory * [ SetSize];

    for ( int i = 0; i < SetSize; i ++)
        Memory[ i] = NULL;

    vector<ElfSection> sections_array;
    ElfSection::getAllElfSections( executable_file_name, sections_array);
    for ( int i = 0; i < sections_array.size(); i ++)
    {
        if ( !strcmp( ".text", sections_array[ i].name))
        {
            Start = sections_array[ i].start_addr;
        }
        ArrayWrite( sections_array[ i].content, sections_array[ i].start_addr, sections_array[ i].size);
    }
}

void FuncMemory::ArrayWrite( uint8* value, uint64 addr, uint64 size)
{
    assert( value);

    uint64 CurrentSet = ( ( SetSize - 1) & ( addr >> ( OffsetBits + PageBits)));
    uint64 CurrentPage = ( ( PageSize - 1) & ( addr >> OffsetBits));
    uint64 CurrentOffset = ( ( OffsetSize - 1) & addr);

    uint64 x = 0;
    bool flag = false;

    while ( size > 0)
    {
        if ( Memory[ CurrentSet])
        {
            if ( Memory[ CurrentSet]->page[ CurrentPage])
            {
                bool flag = false;

                while ( ( ( CurrentSet) || ( !flag)) && ( size))
                {
                    Memory[ CurrentSet]->page[ CurrentPage]->offset[ CurrentOffset] = value[ x];
                    addr ++;
                    x ++;
                    size --;
                    flag = true;
                    CurrentOffset = ( ( OffsetSize - 1) & addr);
                }
                CurrentSet = ( ( SetSize - 1) & ( addr >> ( OffsetBits + PageBits)));
                CurrentPage = ( ( PageSize - 1) & ( addr >> OffsetBits));
            }
            else
            {
                Memory[ CurrentSet]->page[ CurrentPage] = new PageMemory;

                Memory[ CurrentSet]->page[ CurrentPage]->offset = new OffsetMemory[ OffsetSize];
                flag = false;

                while ( ( ( CurrentSet) || ( !flag)) && ( size))
                {
                    Memory[ CurrentSet]->page[ CurrentPage]->offset[ CurrentOffset] = value[ x];
                    addr ++;
                    x ++;
                    size --;
                    flag = true;
                    CurrentOffset = ( ( OffsetSize - 1) & addr);
                }
                CurrentSet = ( ( SetSize - 1) & ( addr >> ( OffsetBits + PageBits)));
                CurrentPage = ( ( PageSize - 1) & ( addr >> OffsetBits));
            }
        }
        else
        {
            Memory[ CurrentSet] = new SetMemory;
            Memory[ CurrentSet]->page = new PageMemory * [ PageSize];

            for ( int i = 0; i < PageSize; i++)
            {
                Memory[ CurrentSet]->page[ i] = NULL;
            }

            Memory[ CurrentSet]->page[ CurrentPage] = new PageMemory;

            Memory[ CurrentSet]->page[ CurrentPage]->offset = new OffsetMemory[ OffsetSize];
            flag = false;

            while ( ( ( CurrentSet) || ( !flag)) && ( size))
            {
                Memory[ CurrentSet]->page[ CurrentPage]->offset[ CurrentOffset] = value[ x];
                addr ++;
                x ++;
                size --;
                flag = true;
                CurrentOffset = ( ( OffsetSize - 1) & addr);
            }
            CurrentSet = ( ( SetSize - 1) & ( addr >> ( OffsetBits + PageBits)));
            CurrentPage = ( ( PageSize - 1) & ( addr >> OffsetBits));
        }
    }
}

FuncMemory::~FuncMemory()
{
    delete [] Memory;
}

uint64 FuncMemory::startPC() const
{
    return Start;
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    assert( ( num_of_bytes <= 8) && ( num_of_bytes != 0));
    assert( addr != 0 );

    uint64 CurrentSet = ( ( SetSize - 1) & ( addr >> ( OffsetBits + PageBits)));
    uint64 CurrentPage = ( ( PageSize - 1) & ( addr >> OffsetBits));
    uint64 CurrentOffset = ( ( OffsetSize - 1) & addr);
    
    uint8 ArrayResult[ 8];
    uint64 result = 0;

    for ( int i = 0; i < num_of_bytes; i ++)
    {
        assert( Memory[ CurrentSet]);
        assert( Memory[ CurrentSet]->page[ CurrentPage]);
        
        ArrayResult[ i] = Memory[ CurrentSet]->page[ CurrentPage]->offset[ CurrentOffset];
        addr ++;
        CurrentSet = ( ( SetSize - 1) & ( addr >> ( OffsetBits + PageBits)));
        CurrentPage = ( ( PageSize - 1) & ( addr >> OffsetBits));
        CurrentOffset = ( ( OffsetSize - 1) & addr);
    }

    for ( int i = num_of_bytes - 1; i >= 0; i --)
    {
        result = result * LINK_CONST + ArrayResult[ i];
    }

    return result;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    assert( addr != 0);
    assert( num_of_bytes != 0 );

    uint8 result[ 8];
    for ( int i = 0; i < num_of_bytes; i ++)
    {
        result[ i] = uint8 ( ( value >> ( 8 * i)) & 255);
    }

    ArrayWrite( result, addr, num_of_bytes);
}

string FuncMemory::dump( string indent) const
{
    ostringstream oss;
    
    oss << indent << "Dump of memory by setcions:" << endl;
    oss << indent << "Start : " << Start << endl;

    uint64 value = 0;
    uint64 addr = 0;
    bool flag = false;

    for ( int i = 0; i < SetSize; i ++)
    {
        if ( this->Memory[ i])
        {
            for ( int j = 0; j < PageSize; j ++)
            {
                if ( Memory[ i]->page[ j])
                {
                    int last = 0;
                    for ( int h = 0; h < ( OffsetSize / 4); h ++)
                    {
                        addr = OffsetSize * PageSize * i + OffsetSize * j + 4 * h;
                        value = read( addr, 4);
                        if ( value == 0)
                        {
                            if ( !flag)
                                oss << indent << endl << indent << "          ....  " << endl << indent << endl;
                            flag = true;
                        }
                        else
                        {
                            flag = false;
                            oss << indent << "  " << addr << ":    " << value << endl;
                        }
                        last = h;
                    }
                    last++;
                    if ( OffsetSize % 4)
                    {
                        addr = OffsetSize * PageSize * i + OffsetSize * j + last * 4;
                        value = read( addr, OffsetSize % 4);
                        if ( value == 0)
                        {
                            if ( !flag)
                                oss << indent << endl << indent << "          ....  " << endl << indent << endl;
                            flag = true;
                        }
                        else
                        {
                            flag = false;
                            oss << indent << "  " << addr << ":    " << value << endl;
                        }
                    }
                }
            }
        }
    }

    oss << indent << "End of FuncMemory" << endl;
    return oss.str();
}
