

#include "virtual_memory.h"

//method to get mask ( 1 in pleces from first to last, including ends)
//from 0 to 63
uint64 getMask(int first, int last);//capter, implementation below...

VirtualMemory::VirtualMemory ( uint64 addr_size/* = 32*/,
                    uint64 page_num_size /*= 10*/,
                    uint64 offset_size /*= 12*/)
{
    this->addr_size = addr_size;
    this->sets_num_size = addr_size- page_num_size - offset_size;
    this->page_num_size = page_num_size;
    this->offset_size = offset_size;
    this->sets_num_mask = getMask( this->sets_num_size+
                                   page_num_size+
                                   offset_size -1,
                                   page_num_size+
                                   offset_size);
    this->page_num_mask = getMask( page_num_size+
                                   offset_size -1,
                                   offset_size);
    this->offset_mask = getMask( offset_size-1,
                                 0);
    this->sets=( uint8**) malloc( sizeof( uint64) *
                                ( 1 << this->sets_num_size) );
    for( int i = 0; i < ( 1 << this->sets_num_size) ; i++ )
    {
        this->sets[i] = NULL;
    }
}

uint64 VirtualMemory::getSetsNum( uint64 addr)
{
    return (addr & this->sets_num_mask) >> 
            (this->page_num_size + this->offset_size);
}

uint64 VirtualMemory::getPageNum( uint64 addr)
{
    return (addr & this->page_num_mask) >> 
            (this->offset_size);
}
uint64 VirtualMemory::getOffset( uint64 addr)
{
    return (addr & this->offset_mask);
}

uint8 VirtualMemory::getByteFromAddr( uint64 addr )
{
    uint64 sets_num = getSetsNum( addr );
    uint64 page_num = getPageNum( addr );
    uint64 offset = getOffset( addr );
    uint8* pages = this->sets[sets_num];
    
    assert( ( 1 << this->addr_size ) <= addr );
    assert( pages != NULL );

    return getByteFromPages( ( uint8**) pages, page_num, offset);
}

uint8 VirtualMemory::getByteFromPages( uint8** pages, uint64 page_num,
                            uint64 offset)
{
    assert( pages[page_num] != NULL );
    return pages[page_num][offset];
}

void VirtualMemory::setByteToAddr( uint64 addr, uint8 byte)
{
    uint64 sets_num = getSetsNum( addr );
    uint64 page_num = getPageNum( addr );
    uint64 offset = getOffset( addr );
    uint8* pages = this->sets[sets_num];
    if ( pages  == NULL )
        pages = allocateSet();
    setByteToPages( (uint8**) pages, page_num , offset, byte );
}

void VirtualMemory::setByteToPages( uint8** pages, uint64 page_num,
                          uint64 offset, uint8 byte)
{
    if ( pages[page_num] != NULL )
        pages[page_num] = allocatePage();
    pages[page_num][offset] = byte;
}

uint64 VirtualMemory::read( uint64 addr, unsigned short num_of_bytes /*= 4*/)
{
    uint64 addr_ = addr;
    uint8 temp;
    uint64 value = 0;
    for ( int i = 0 ; i < num_of_bytes ; i++ )
    {
        value = value << 8;
        temp = getByteFromAddr( addr_ );
        value += ( uint64) temp;
        addr_++;
    }
    return value;
}

void VirtualMemory::write( uint64 value, uint64 addr,
                                unsigned short num_of_bytes /*= 4*/)
{
    uint64 addr_ = addr;
    uint64 value_ = value;
    for ( int i = 0 ; i < num_of_bytes ; i++ )
    {
        setByteToAddr( addr_, (uint8) value_);
        value_ = value_ >> 8;
        addr_++;
    }
}

uint64 VirtualMemory::setStartPC( uint64 startPC )
{
    return this->startPC = startPC;
}

uint64 VirtualMemory::getStartPC()
{
    return this->startPC;
}

uint8* VirtualMemory::allocatePage()
{
    uint8* page = (uint8*) malloc(sizeof(void*) * 
            ( 1 << this->offset_size )  );
    return page;
}
uint8* VirtualMemory::allocateSet()
{
    uint8* pages = (uint8*) malloc(sizeof(void*) * 
            ( 1 << this->page_num_size )  );
    for( uint64 i = 0; i < (1<< this->page_num_size ); i++)
    {
        ((uint8**) pages)[i] = NULL;
    }
    return pages;
}

VirtualMemory::~VirtualMemory()
{
//Recursive freing of allocated memory;
    for ( uint64 i = 0 ; i < ( 1 << this->sets_num_size) ; i++)
    {
        if ( this->sets[i] == NULL )
            continue;
        else
        {
            for( uint64 j = 0; j < ( 1 << this->page_num_size); j++)
            {
                if ( (  (( uint8** )this->sets[i])[j] ) == NULL )
                    continue;
                else
                   free (  (( uint8** )this->sets[i])[j]);
            }
            free(this->sets[i]);
        }
    }
}



uint64 getMask(int first, int last)
{
    uint64 temp = (uint64) ( (int64) -1 );
    temp = ( temp >> last ) << last;
    temp = ( temp << (63 - first) ) >> (63 - first);
    return temp;
}



