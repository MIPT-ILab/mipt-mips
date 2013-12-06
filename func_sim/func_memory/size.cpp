#include <size.h>

uint64 SIZE_IN_ELEMENTS(uint64 num_of_bits)
{
     uint64 res = 1;

     for(int i = 0; i < num_of_bits; i++)
     {
          res = res*2;
     }
     return res-1;
}
