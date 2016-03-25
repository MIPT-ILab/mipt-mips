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
        std::cout << "ERROR. File doesn't exist" << std::endl;
        return -1;
    }

    uint64 miss = 0;
    uint64 count = 0;
    for (; !file_in.eof (); count ++)
    {
        file_in >> std::hex >> addr;
        miss += (int)(!cache.read (addr));
        if (count % 500000 == 0)
            std::cout << std::dec << count << " " << std::hex << addr << std::endl;
    }

    std::cout << std::dec
              << "size = "
              << size_in_bytes
              <<" miss = "
              << miss
              << " count = "
              << count << std::endl;

    double answ = (double)miss / (double)count;
    file_out << (double)answ << ",";

    std::cout << "=======================================" << std::endl;

    file_in.close ();
}

int main (int argc, char* argv [])
{

    std::ofstream file_out (argv[2], std::ios_base::out | std::ios_base::app);

    file_out << ",1KB,2KB,4KB,8KB,16KB,32KB,64KB,128KB,256KB,512KB,1024KB"
             << std::endl;

    file_out << "full associative,";
    for (int i = 1; i <= 1024; i *= 2)
    {
        readFunc (argv [1], file_out, i*1024, 1, 4, 32, true);
        std::cout << "======================================" << std::endl;
    }
    file_out << std::endl;

    for (int ways_count = 1; ways_count <= 16; ways_count *= 2)
    {
        file_out << std::dec << ways_count <<" way(s),";
        for (int i = 1; i <= 1024; i *= 2)
        {
            readFunc (argv [1], file_out, i*1024, 1, 4, 32, false);
            std::cout << "======================================" << std::endl;
        }
        file_out << std::endl;
    }
    file_out.close ();


/*    CacheTagArray array (24, 3, 4, 32, false);
    array.read (0b00000000000000000000000000011000);
    array.read (0b00000000000000000000000000111000);
                //12345678123456781234567812345678
    array.read (0b00000000000000000000000001111000);
    array.read (0b00000000000000000000000011111000);

    std::cout << array.read (0b00000000000000000000000000011000) << std::endl;
*/
/*    CacheTagArray array2 (16, 3, 4, 32, true);
    array2.read (0b00000000000000000000000000011000);
    array2.read (0b00000000000000000000000000111000);
    array2.read (0b00000000000000000000000001111000);
    array2.read (0b00000000000000000000000011111000);
    array2.read (0b00000000000000000000000111111000);

    std::cout << array2.read (0b00000000000000000000000011111000) << std::endl;
*/
    return 0;
}
