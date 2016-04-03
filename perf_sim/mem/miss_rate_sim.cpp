/**
 * miss_rate_sim.cpp
 * Program for checking performance of different cache configurations.
 * MIPT-MIPS Assignment 5.
 * Ladin Oleg.
 */

/* C libraries. */
#include <cstdlib>

/* C++ libraries. */
#include <iostream>
#include <fstream>

/* Simulator modules. */
#include <cache_tag_array.h>

using namespace std;

int main( int argc, char* argv[])
{
    /* Check arguments. */
    if ( argc != 3)
    {
        cerr << "ERROR: Wrong number of arguments! Required 2: name of file "
             << "with memory access addresses; name of output file with miss "
             << "rates." << endl;
        exit( EXIT_FAILURE);
    }
    if ( argv[ 1] == nullptr)
    {
        cerr << "ERROR: Wrong first argument! Required name of file with "
             << "memory access addresses." << endl;
        exit( EXIT_FAILURE);
    }
    if ( argv[ 2] == nullptr)
    {
        cerr << "ERROR: Wrong second argument! Required name of output file "
             << "with miss rates." << endl;
        exit( EXIT_FAILURE);
    }

    /* Open and check input and output files. */
    ifstream file_in;
    file_in.open( argv[ 1], ifstream::in);
    if ( !file_in.is_open())
    {
        cerr << "ERROR: Can't open the input file!" << endl;
        exit( EXIT_FAILURE);
    }
    ofstream file_out;
    file_out.open( argv[ 2], ofstream::out);
    if ( !file_out.is_open())
    {
        cerr << "ERROR: Can't open the output file!" << endl;
        exit( EXIT_FAILURE);
    }

    /* Cache parametres. */
    int associativity[ 5] = { 1, 2, 4, 8, 16 };
    int cache_size[ 11] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024 };
    uint32 addr; // storage for address

    for ( int i = 0; i < 5; ++i) // by associativity (excluding full-as.)
    {
        for ( int j = 0; j < 11; ++j) // by cache size
        {
            CacheTagArray cta( 1024 * cache_size[ j], associativity[ i]);
            uint64 hit = 0, miss = 0;
            while ( file_in >> hex >> addr) // while file contains addresses
            {
                if ( cta.read( addr)) // hit
                {
                    hit++;
                } else // miss
                {
                    miss++;
                    cta.write( addr); // load to the cache
                }
            }
            file_in.clear(); // reset "EOF" flag on file stream
            file_in.seekg( ifstream::beg); // set file pointer to the beginning
            double rate = ( double)miss / ( hit + miss);
            file_out << rate << ", ";
            file_out.flush(); // immidiate print to the file (to avoid delays)
        }
        file_out << endl;
    }
    /* Same as previous for full-associative cache. */
    for ( int i = 0; i < 11; ++i)
    {
        CacheTagArray cta( 1024 * cache_size[ i], 1024 * cache_size[ i] / 4);
        uint64 hit = 0, miss = 0;
        while ( file_in >> hex >> addr)
        {
            if ( cta.read( addr))
            {
                hit++;
            } else
            {
                miss++;
                cta.write( addr);
            }
        }
        file_in.clear();
        file_in.seekg( ifstream::beg);
        double rate = ( double)miss / ( hit + miss);
        file_out << rate << ", ";
        file_out.flush();
    }
    file_out << endl;

    /* Close files. */
    file_in.close();
    file_out.close();
    return 0;
}
