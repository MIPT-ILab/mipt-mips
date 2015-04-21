/**
 * miss_rate_sim.cpp - testing miss rate for different cache models
 * @author Alexander Pronin <alexander.pronin.mipt@gmail.com>
 * Copyright 2015 MIPT-MIPS
 */

#include <iostream>
#include <stdio.h>
#include <cache_tag_array.h>
#include <types.h>
#include <vector>

using namespace std;

const unsigned short block_size_in_bytes = 4;
const unsigned short addr_size_in_bits = 32;

// cache sizes in bytes to be tested
const unsigned int size_table[] =
{
    1024,
    2 * 1024,
    4 * 1024,
    8 * 1024,
    16 * 1024,
    32 * 1024,
    64 * 1024,
    128 * 1024,
    256 * 1024,
    512 * 1024,
    1024 * 1024
};

const unsigned int num_of_size = sizeof( size_table) / sizeof( unsigned int);

// num of ways to be tested
// 0 means that cache is full associative
const unsigned int way_table[] =
{
    1,
    2,
    4,
    8,
    16,
    0
};

const unsigned int num_of_ways = sizeof( way_table) / sizeof( unsigned int);

int main(int argc, char* argv[])
{
    if ( argc != 3) {
        cout << "Error: wrong number of arguments. Program stopped\n";
        return -1;
    }

    
    freopen( argv[ 2], "w", stdout);
    freopen( argv[ 1], "r", stdin);
    vector<uint64> addr_seq;

    uint64 buffer;

    while ( cin >> hex >> buffer) {
        addr_seq.push_back( buffer);
    }

    fclose( stdin);

    // run test
    for ( int i = 0; i < num_of_ways; i++) {
        for ( int j = 0; j < num_of_size; j++) {
            CacheTagArray* cache = new CacheTagArray( size_table[ j], way_table[ i], block_size_in_bytes, addr_size_in_bits);
            uint64 miss_counter = 0;
            uint64 hit_counter = 0;
            for ( unsigned int k = 0; k < addr_seq.size(); k++)
                if ( !cache->read( addr_seq[ k])) {
                    cache->write( addr_seq[ k]);
                    miss_counter++;
                }
                else
                    hit_counter++;
            cout << (double) miss_counter / ( hit_counter + miss_counter)  << ", ";
            delete cache;
        }
        cout << endl;
    }
    fclose( stdout);
    return 0;
}
