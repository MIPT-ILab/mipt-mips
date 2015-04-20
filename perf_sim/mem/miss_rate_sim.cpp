/*
 * miss_rate_sim.cpp - main function for testing caches
 *                     of different configurations
 * @author Mikhail Lyubogoschev lyubogoshchev@phystech.edu
 * Copyright 2015 MIPT-MIPS 
 */

#include <cache_tag_array.h>
#include <fstream>
#include <Cache_Prop_Table.h>
#include <vector>
using std::vector;

#define DIFF_SIZE 10
#define DIFF_WAYS 6

int main( int argc, char** argv)
{
    if ( argc != 3) 
    {
        cout << "2 arguments required: input file name and output file name\n";
        exit(EXIT_FAILURE);
    }
    std::ifstream input( argv[1]);
    std::ofstream output( argv[2]);
    if ( !input.is_open() || !output.is_open())
    {
        std::cerr << "Error in files open\n";
        return 1;
    }
    vector <uint32> in;
    input >> std::hex;
    uint64 curr_addr = 0;
    input >> curr_addr;
    while ( !input.eof())
    {
        in.push_back(curr_addr);
        input >> curr_addr;
    }
    CacheProperties** table;
    table = new CacheProperties*[DIFF_SIZE];
    uint64 size = 1 << 10;
    for ( int i = 0; i < DIFF_SIZE; i++)
    {
        table[i] = new CacheProperties[DIFF_WAYS];
        uint64 ways = 1;
        for ( int j = 0; j < DIFF_WAYS - 1; j++)
        {
            table[i][j].associativity = ways;
            ways *= 2;
            table[i][j].size = size;
            table[i][j].cache = new CacheTagArray( size, ways, table[i][j].block_size,
                              table[i][j].addr_size);
            
        }
        table[i][DIFF_WAYS - 1].associativity = size / table[i][DIFF_WAYS - 1].block_size;
        table[i][DIFF_WAYS - 1].size = size;
        table[i][DIFF_WAYS - 1].cache = new CacheTagArray( size, size / table[i][DIFF_WAYS-1].block_size, table[i][DIFF_WAYS-1].block_size,
                              table[i][DIFF_WAYS].addr_size);
        size = size << 1;
    }

//    input >> std::hex;
//    uint64 curr_addr = 0;
//    input >> curr_addr;
    uint64 instr_cnt = 0;
    //while ( !input.eof())
    for ( vector<uint32>::iterator it = in.begin(); it != in.end(); it++)
    {
        for ( int i = 0; i < DIFF_SIZE; i++)
            for ( int j = 0; j < DIFF_WAYS; j++)
//                if ( !table[i][j].cache->read( curr_addr))
                  if ( !table[i][j].cache->read( *it))
                    table[i][j].miss_num++;
        instr_cnt++;
//        input >> curr_addr;
    }
    for ( int i = 0; i < DIFF_WAYS; i++)
    {
        for ( int j = 0; j < DIFF_SIZE; j++)
        {
            table[j][i].miss_rate = ( double)table[j][i].miss_num / ( double)instr_cnt;
            output << table[j][i].miss_rate << ", ";
            cout << table[j][i].miss_rate << ", ";
        }
        output << std::endl;
        cout << std::endl;
    }
    input.close();
    output.close();
    getchar();
    return 0;
}


