/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C
#include <cstring>
#include <stdlib.h>

// Generic C++
#include <sstream>
#include <iostream>
#include <fstream>

// uArchSim modules
#include <func_memory.h>
#include <elf_parser.h>

// Allocating memory for name and content

uint64 FuncMemory::getDegree( uint64 degree)
{
    uint64 result = 1;
    for( uint64 i = 0; i < degree; i++)
    {
        result = result << 1;
    }

    return result;
}

void FuncMemory::memDelete()
{
    cout << "deleting memory" << endl;
    if(!memory)
        return;

    cout << "memory is allocated, deleting..." << endl;

    for( uint64 i = 0; i < set_num; i++)
    {
        if( memory[ i]!= NULL)
        {
            //cout << "no seg fault on [i]" << endl;
            for( uint64 j = 0; j < page_num; j++)
            {
                if( memory[ i][ j]!= NULL)
                {
                    //cout << "no seg fault on [j]" << endl;
                    delete [] memory[i][j];
                    //cout << "offset deleted" << endl;
                }
            }
        }
          
        delete [] memory[i];
        //cout << "page deleted" << endl;
    }
    
    delete [] memory;
    cout << "mem deleted" << endl;
}
using std::map;


FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
{
    // Check file exist
    FILE* file = fopen( executable_file_name, "r");
    if( !file)
    {
        fclose( file);
        cerr << "ERROR. Wrong file name!" << endl;
        exit( EXIT_FAILURE);
    }
    else
        fclose( file);
    
    /*ifstream file_t ( executable_file_name, ios::binary);
    if( !file_t.good()) 
    {    
        file_t.close();
        cerr << "Wrong file name!" << endl;
        exit( EXIT_FAILURE);
    } 
    else
        file_t.close();
    */
    // Set address parameters
    setSize = addr_size - page_bits - offset_bits;
    pageSize = page_bits;
    offsetSize = offset_bits;
    blindSize = 64 - addr_size;

    set_num = getDegree( setSize); 
    page_num = getDegree( pageSize);
    offset_num = getDegree( offsetSize);

    /*cout << "set_num = " << set_num << endl;
    cout << "page_num = " << page_num << endl;
    cout << "offset_num = " << offset_num << endl;
    cout << "-------------------------------------" << endl << endl;

    cout << "addr_size = " << addr_size << endl;
    cout << "setSize = " << setSize << endl;
    cout << "pageSize = " << pageSize << endl;
    cout << "offsetSize = " << offsetSize << endl;
    cout << "blindSize = " << blindSize << endl;
    cout << endl;*/

    // Parsing file
    vector<ElfSection> section_array;
    ElfSection::getAllElfSections( executable_file_name, section_array);
    cout << "parsed" << endl;
    
    //cout << "vector size " << section_array.size() << endl;
    // Init memory                   
    memory = new uint8 **[ set_num];

    //cout << "memory initializated" << endl;
    // test


    for( vector<ElfSection>::iterator it = section_array.begin(); it!= section_array.end(); it++)
    {
        //string b = (*it).name;
        //cout << "getting elfsection " << endl;
        // Getting set, page, offset
        uint64 set = (*it).start_addr >> ( pageSize + offsetSize);
        uint64 page = (*it).start_addr << ( blindSize + setSize) >> ( blindSize + pageSize + offsetSize);
        uint64 offset = (*it).start_addr << ( blindSize + setSize + pageSize) >> ( blindSize + setSize + pageSize);

        cout << "size = " << (*it).size << endl;
        cout << "start_addr = " << (*it).start_addr << endl;
        cout << "set = " << set << endl;
        cout << "page = " << page << endl;
        cout << "offset = " << offset << endl;

        string name = (*it).name;
        if( name == ".text")
            txt_addr = (*it).start_addr;
        cout << name << endl;

        // Allocating memory
        //cout << "*****************************"<<endl;
        
        if( memory[ set] == NULL)
        {
            memory[ set] = new uint8 *[ page_num];
            for( uint64 i; i < page_num; i++)
            {
                memory[ set][ i] = NULL;
            }
            //cout << "page allocated" << endl;
        }  
        if( memory[ set][ page] == NULL)
        {
            memory[ set][ page] = new uint8 [ offset_num];
            for( uint64 i = 0; i < offset_num; i++)
            {
                memory[ set][ page][ i] = 0;
            }
            //cout << "offset allocated" << endl;
        }

        /*cout << "memory allocated!" << endl;
        cout << "*****************************"<<endl;*/

        /*uint64 num = 0;
        uint64 a = (*it).content[num];
        cout << "ex value = " << a << "|";
        a = (*it).content[1];
        cout << a << "|";
        a = (*it).content[2];
        cout << a << "|";
        a = (*it).content[3];
        cout << a << endl;*/


        // Filling content
        for( uint64 i = 0; i < (*it).size; i++)
        {
            memory[ set][ page][ offset + i] = (*it).content[i];
            uint64 temp = memory[ set][ page][ offset + i];
            //cout << "wrote " << temp << " to " << offset+i << endl;
        }

        //cout << "section filled!" << endl;
    }
    
}

FuncMemory::~FuncMemory()
{
    //Cleaning memory
    memDelete();
    cout << "mem delete" << endl;
}

uint64 FuncMemory::startPC() const
{
    return txt_addr;
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    if( num_of_bytes > 8)
    {
        cerr << "ERROR. Wrong num_of_bytes!" << endl;
        exit( EXIT_FAILURE);
    }
    // Get set, page and offset
    uint64 value = 0;
    uint64 set = addr >> ( pageSize + offsetSize);
    uint64 page = addr << ( blindSize + setSize) >> ( blindSize + pageSize + offsetSize);
    uint64 offset = addr << ( blindSize + setSize + pageSize) >> ( blindSize + setSize + pageSize);
   
    // Check if address exists
    if( memory[ set] == NULL)
    {
        cerr << "ERROR. Reading wrong address! Abort." << endl;
        exit( EXIT_FAILURE);
    }
    if( memory[ set][ page] == NULL)
    {
        cerr << "ERROR. Reading wrong address! Abort." << endl;
        exit( EXIT_FAILURE);
    }
    // Reading value
    for( unsigned short i = 0; i < num_of_bytes; i++)
    {
        uint64 temp = memory[ set][ page][ offset + i];
        value = value << ( 8 * i);
        value += temp;
    }
 
    return value;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    // Check argument
    if( num_of_bytes > 8)
    {
        cerr << "ERROR. Wrong num_of_bytes argument!" << endl;
        exit( EXIT_FAILURE);
    }
    // Get set, page and offset
    uint64 set = addr >> ( pageSize + offsetSize);
    uint64 page = addr << ( blindSize + setSize) >> ( blindSize + pageSize + offsetSize);
    uint64 offset = addr << ( blindSize + setSize + pageSize) >> ( blindSize + setSize + pageSize);
 
    // Allocating memory if not
    if( memory[ set] == NULL)
    {
        memory[ set] = new uint8 *[ page_num];
        for( uint64 i = 0; i < page_num; i++)
        {
            memory[ set][ i] = NULL;
        }
        cout << "new set allocated" << endl;
    }
    if( memory[ set][ page] == NULL)
    {
        memory[ set][ page] = new uint8 [ offset_num];
        for( uint64 i = 0; i < offset_num; i++)
        {
            memory[ set][ page][ i] = 0;
        }
        cout << "new page allocated" << endl;
    }

    uint64 temp;
    for( unsigned short i = 0; i < num_of_bytes; i++)
    {
        temp = value >> ( ( num_of_bytes - i - 1) * 8); // Now 8 bits are at right
        temp << ( 7 * 8) >> ( 7 * 8);                   // Delete higher bits
        memory[ set][ page][ offset + i] = temp;
    }

}

string FuncMemory::dump( string indent) const
{
    ostringstream oss;
    cout << "dumping..." << endl << endl;

    for( uint64 i = 0; i < setSize; i++)
    {
        if( memory[ i]!= NULL)
            for( uint64 j = 0; j < page_num; j++)
            {
                if( memory[ i][ j]!= NULL)
                {
                    //cout << "SET = " << i << endl;
                    //cout << "PAGE = " << j << endl;
                    for( uint64 k = 0; k < offset_num - 4; k = k + 4)
                    {
                        uint64 value = 0;
                        uint64 temp = 0;
                        uint64 val[ 4];
                        for( uint64 delta = 0; delta < 4; delta++)
                        {
                            temp = memory[ i][ j][ k + delta];
                            value = value << ( 8 * delta);
                            value += temp;
                            val[ delta] = temp;
                        }

                        if( value!= 0)
                        {
                            uint64 address = i;
                            address = address << 8;
                            address += j;
                            address = address << 8;
                            address += k;

                            //cout << "addr = " << address << "  value = " << value << endl; 
                            oss << indent << "0x" << hex << address << " ";
                            for( uint64 val_i = 0; val_i < 4; val_i++)
                            {
                                if( val[ val_i] == 0)
                                {
                                    oss << indent << "00";
                                } else
                                if( val[ val_i] < 16)
                                {
                                    oss << indent << "0"
                                       << indent << hex << val[ val_i] << dec;
                                } else {
                                    oss << indent << hex << val[ val_i] << dec;
                                }
                                
                            }
                            oss << indent << endl;
                        }
                    }
                }
            }
    }
    return oss.str();;
}

