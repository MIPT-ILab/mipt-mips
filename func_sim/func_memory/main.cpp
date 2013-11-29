// Generci C
#include <stdlib.h>

// Generic C++
#include <iostream>

// uArchSim modules
#include <func_memory.h>

using namespace std;


int main (int argc, char* argv[])
{
    // Only one argumnt is required, the name of an executable file
    const int num_of_args = 1;

    if ( argc - 1 == num_of_args)
    {
        // set the name of the executable file
        const char * file_name = argv[1];

        // create the functiona memory
        FuncMemory func_mem( file_name, 32, 10, 12);

        // print content of the memory
        cout << func_mem.dump() << endl;

 // **********************************************************************************
 // ****************** Test section **************************************************
 // **********************************************************************************

    uint64 write_addr = 0x3FFFFE;

    // write 0x03020100 into the four bytes pointed by write_addr
    func_mem.write( 0x03020100, write_addr, sizeof( uint64));
    uint64 right_ret = 0x0100;
    assert( func_mem.read( write_addr, sizeof( uint16)) == right_ret);

    right_ret = 0x0201;
    assert( func_mem.read( write_addr + 1, sizeof( uint16)) == right_ret);

    right_ret = 0x0302;
    assert( func_mem.read( write_addr + 2, sizeof( uint16)) == right_ret);

 // **********************************************************************************
 // **********************************************************************************
 // **********************************************************************************


    } else if ( argc - 1 > num_of_args)
    {
        cerr << "ERROR: too many arguments!" << endl
             << "Only one argumnt is required, the name of an executable file." << endl;
        exit( EXIT_FAILURE);

    } else
    {
        cerr << "ERROR: too few arguments!" << endl
             << "One argument is required, the name of an executable file." << endl;
        exit( EXIT_FAILURE);
    }




    return 0;
}
