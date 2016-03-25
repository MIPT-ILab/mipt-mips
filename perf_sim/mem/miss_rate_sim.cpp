#include <iostream>
#include <fstream>

#include <cache_tag_array.h>

int readFunc (char file_input [],
               std::ofstream& file_out,
               int32 size_in_bytes,
               uint32 ways,
               uint16 block_size_in_bytes,
               uint16 addr_size_in_bits,
               bool is_associative)
{
    uint64 addr = 0ull;
    std::ifstream file_in (file_input);

    CacheTagArray cache (size_in_bytes,
                         ways,
                         block_size_in_bytes,
                         addr_size_in_bits,
                         is_associative);

    if (!file_in.is_open ())
    {
        std::cerr << "ERROR. File doesn't exist" << std::endl;
        return (EXIT_FAILURE);
    }

    uint64 miss = 0;
    uint64 count = 0;
    for (; !file_in.eof (); count ++)
    {
        file_in >> std::hex >> addr;
        miss += (int)(!cache.read (addr));
    }

    double answ = (double)miss / (double)count;
    file_out << (double)answ << ",";

    file_in.close ();
}

int main (int argc, char* argv [])
{

	switch( argc)
    {
        case 3:
            if ( ( argv[ 1] == nullptr) || ( ( argv[ 2] == nullptr)) )
            {
                std::cerr << "ERROR: Wrong arguments!\n";
                exit( EXIT_FAILURE);
            }
            break;
        default: // wrong number of arguments
            std::cerr << "ERROR: Wrong number of arguments!\n";
            exit( EXIT_FAILURE);
    }

    std::ofstream file_out (argv[2], std::ios_base::out | std::ios_base::app);

    file_out << ",1KB,2KB,4KB,8KB,16KB,32KB,64KB,128KB,256KB,512KB,1024KB"
             << std::endl;

    file_out << "full associative,";
    for (int i = 1; i <= 1024; i *= 2)
    {
        bool is_input_file_existed = readFunc (argv [1], file_out, i*1024, 1, 4, 32, true);
        if (!is_input_file_existed)
        	exit( EXIT_FAILURE);
    }
    file_out << std::endl;

    for (int ways_count = 1; ways_count <= 16; ways_count *= 2)
    {
        file_out << std::dec << ways_count <<" way(s),";
        for (int i = 1; i <= 1024; i *= 2)
        {
            readFunc (argv [1], file_out, i*1024, ways_count, 4, 32, false);
        }
        file_out << std::endl;
    }
    file_out.close ();

    return 0;
}
