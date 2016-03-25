# include <list>
#include <iostream>
#include <cmath>

#include <cache_tag_array.h>

//==============================================================================
// TagsArray methods =======================================================
//==============================================================================
TagsArray :: TagsArray (uint64 vector_size)
    //_tag_value (tag_value)//,
    //_set (set)
{
    tag_values.resize (vector_size);
}

void TagsArray :: addTag (uint64 tag, uint32 pos)
{
    //uint32 pos = findFreeCell ();
    tag_values [pos] = tag;
}

void TagsArray :: dump (std::string indent)
{
    for (int i = 0; i < tag_values.size (); i++)
    {
        std::cout << indent
                  << indent
                  << "tag ["
                  << std::dec
                  << i
                  << "] = "
                  << std::hex
                  << tag_values [i] << std::endl;
    }
}

//==============================================================================
// CacheTagArray methods =======================================================
//==============================================================================

CacheTagArray :: CacheTagArray (uint32 size_in_bytes,
                                uint32 ways,
                                uint16 block_size_in_bytes,
                                uint16 addr_size_in_bits,
                                bool is_associative):
                                _addr_size_in_bits (addr_size_in_bits),
                                _is_associative (is_associative)
{
    if (!is_associative)
// multi ways cache ============================================================
{
        for (int i = 0; i < size_in_bytes / ways / block_size_in_bytes; i++)
        {
            std::list<uint32> curr_str;
            for (int curr_way = 0; curr_way < ways; curr_way++)
                curr_str.push_front (curr_way);
            _busy_cells.push_back (curr_str);
        }
        for (int curr_way = 0; curr_way < ways; curr_way++)
            _ways.push_back (TagsArray (size_in_bytes /
                                        ways          /
                                        block_size_in_bytes));
        _set_size = int(log2 ((double)(size_in_bytes /
                                       ways          /
                                       block_size_in_bytes)) + 0.5);
    }
    else
// full associative cache ======================================================
    {
        std::list<uint32> curr_str;
        for (int i = 0; i < size_in_bytes / block_size_in_bytes; i++)
            curr_str.push_front (i);
        _busy_cells.push_back (curr_str);
        _ways.push_back (TagsArray (size_in_bytes / block_size_in_bytes));
        _set_size = 0;

        //listDump (_busy_cells [0]);
    }
    _offset = int(log2 ((double)block_size_in_bytes) + 0.5);
    _tag_size = addr_size_in_bits - _offset - _set_size;

    /*std::cout << "Constr params: _tag = "
              << _tag_size
              << " set = "
              << _set_size
              << " offset = "
              << _offset
              << std::endl;*/
}

bool CacheTagArray :: read (uint64 addr)
{
    parseAddr (addr);
    if(!_is_associative)
    {
            for (int i = 0; i < _ways.size (); i++)
            {
                if (_ways [i].tag_values [_set] == _tag)
                    return true;
            }
    }
    else
    {
        for (int i = 0; i < _ways [0].tag_values.size (); i++)
            {
                if (_ways [0].tag_values [i] == _tag)
                    return true;
            }
    }
    write (addr);
    return false;
}

void CacheTagArray :: write (uint64 addr)
{
    //std::cout << "write value" << std::endl;
    //parseAddr (addr);

    //listDump (_busy_cells[_set]);

    uint32 num_of_free_cell = findFreeCell ();
    if (!_is_associative)
    {
        _ways [num_of_free_cell].addTag (_tag, _set);
        //vectorDump (_ways[num_of_free_cell].tag_values);

/*        std::cout << "==================Vector=======================" << std::endl;
        for (int i = 0; i < _ways_size; i++)
            std::cout << _ways [i].tag_values [_set] << std::endl;
        std::cout << std::endl;*/
    }
    else
    {
        _ways [0].addTag (_tag, num_of_free_cell);
        //vectorDump (_ways[0].tag_values);
    }
    _busy_cells [_set].push_front (num_of_free_cell);

    //listDump (_busy_cells[_set]);


}

void CacheTagArray :: listDump (std::list<uint32> list)
{
    std::cout << "==================List=========================" << std::endl;
    std::list<uint32> temp = list;
    for (;temp.size ();)
    {
        std::cout << temp.front () << std::endl;
        temp.pop_front ();
    }
    std::cout << std::endl;
}


void CacheTagArray :: vectorDump (std::vector<uint64> vec)
{
    std::cout << "==================Vector=======================" << std::endl;
    for (int i = 0; i < vec.size (); i++)
    {
        std::cout << vec [i] << std::endl;
    }
    std::cout << std::endl;
}


uint32 CacheTagArray :: findFreeCell ()
{
    uint32 buff = _busy_cells [_set].back ();
    _busy_cells [_set].pop_back ();

    //std::cout << "buff = " << buff << std::endl;

    return buff;
}


void CacheTagArray :: parseAddr (uint64 addr)
{
    _tag = (uint64)addr >> (_addr_size_in_bits - _tag_size);
    _set = (((uint64)1 << _set_size) - 1) & (uint64)(addr >> _offset);
/*
    uint64 val = addr >> _offset;
    uint64 mask = ((uint64)1 << _set_size) - 1;

    std::cout << "addr = "
              << std::hex
              << addr
              <<" addr >> _offset = "
              << val
              << " mask = "
              << std::dec
              << mask
              << std::endl;

    std::cout << "Parse : tag = "
              << std::hex
              << _tag
              << " set = "
              << _set
              << std::endl;*/
}
