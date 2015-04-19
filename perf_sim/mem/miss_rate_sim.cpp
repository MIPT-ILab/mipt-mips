
// Generci C
#include <cstring>

// Generic C++
#include <fstream>
#include <iostream>

// uArchSim modules
#include "types.h"
#include "cache_tag_array.h"


double  count_miss_rate( CacheTagArray* abstract_cache, std::ifstream& fin);
bool is_type( char* str, const char* type);


int main( int argc, char* argv[])
{
    if  ( argc != 3) {
        std::cerr << "ERROR: wrong amount of arguments!" << std::endl
                  << "Only two argumnts is required, the names of an input " 
                  << "and of an output files." << std::endl;
        exit( EXIT_FAILURE); 
    }

    if  ( !is_type( argv[1], ".txt")) {
        std::cerr << "ERROR: wrong format of argument one!" << std::endl
                  << "It should be .txt format" << std::endl;
        exit( EXIT_FAILURE); 
    }
    
    if  ( !is_type( argv[2], ".csv")) {
        std::cerr << "ERROR: wrong format of argument two!" << std::endl
                  << "It should be .csv format" << std::endl;
        exit( EXIT_FAILURE); 
    }

    std::ifstream fin( argv[1]);
    std::ofstream fout( argv[2]);
    if  ( !fin.is_open()) {
        std::cout << "Can't open file for input\n";
        exit( EXIT_FAILURE);
    }
    if  ( !fout.is_open()) {
        std::cout << "Can't open filefor output\n";
        exit( EXIT_FAILURE);
    }

    for ( int size = 1024, i = 1; size < 1048580; size = size << 1, i *= 2) {
        CacheTagArray* abstract_cache = new FullAssosiativeCache( size);
        double miss_rate = count_miss_rate( abstract_cache, fin);
        fout << miss_rate << ", ";
        delete abstract_cache;
    }
    fout << std::endl;

    for ( int size = 1024, i = 1; size < 1048580; size = size << 1, i *= 2) {
        CacheTagArray* abstract_cache = new DirectMappedCache( size);
        double miss_rate = count_miss_rate( abstract_cache, fin);
        fout << miss_rate << ", ";
        delete abstract_cache;
    }
    fout << std::endl;

    for ( int ways = 2; ways <= 8; ways = ways << 1) {
        for ( int size = 1024, i = 1; size < 1048580; size = size << 1, i *= 2) {
            CacheTagArray* abstract_cache = new MultiWayCache( size, ways);
            double miss_rate = count_miss_rate( abstract_cache, fin);
            fout << miss_rate << ", ";
            delete abstract_cache;
        }
        fout << std::endl;
    }

    fin.close();
    fout.close();
}

double  count_miss_rate( CacheTagArray* abstract_cache, std::ifstream& fin)
{
    fin.seekg( 0, fin.beg);
    uint64 address;
    uint64 addr_cntr = 0;
    uint64 miss_cntr = 0;
    fin.setf( std::ios::hex, std::ios::basefield);
    bool is_read = false;

    while ( !fin.eof()) {
        fin >> address;
        addr_cntr++;
        is_read = abstract_cache->read( address);
        if  ( is_read == false) {
            miss_cntr++;
            abstract_cache->write( address);
        }
    }
    fin.unsetf( std::ios::hex);

    double miss_rate = miss_cntr;
    miss_rate /= addr_cntr;
    return miss_rate;
}

bool is_type( char* str, const char* type)
{
    char * pch = NULL;
    pch=strrchr(str,'.');
    if  ( pch != NULL) {
        int flag = strcmp( pch, type);
        if  ( flag == 0)
            return true;
        else
            return false;
    } else 
        return false;
}