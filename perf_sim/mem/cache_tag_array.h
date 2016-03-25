# include <vector>
# include <list>
# include <string>

# include <types.h>


class TagsArray
{
public:
    TagsArray () {}
    TagsArray (uint64 vector_size);
    ~TagsArray () {}
    void addTag (uint64 tag, uint32 pos);
    void dump (std::string indent);

    //std::list<uint32> busy_cells;
    std::vector<uint64> tag_values;
    //uint32 _set;
};
//==============================================================================
// Class CacheTagArray =========================================================
//==============================================================================
class CacheTagArray
{
public:
    uint16 _addr_size_in_bits;
    bool _is_associative;

    std::vector<TagsArray> _ways;
    std::vector<std::list<uint32>> _busy_cells;
// variables to parse addr =====================================================
    uint32 _tag_size;
    uint32 _set_size;

    uint64 _tag;
    uint32 _set;
    uint32 _offset;



    CacheTagArray (uint32 size_in_bytes,
                   uint32 ways,
                   uint16 block_size_in_bytes,
                   uint16 addr_size_in_bits,
                   bool is_associative = true);
    bool read (uint64 addr);
    void write (uint64 addr);
    uint32 findFreeCell ();
    void parseAddr (uint64 addr);
    void listDump (std::list<uint32> list);
    void vectorDump (std::vector<uint64> vec);
};
