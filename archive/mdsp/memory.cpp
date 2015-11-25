/**
 * memory.cpp - Implementation of memory model
 * Define classes and method to operate with memory of simulated architecture
 * @author Dmitry Ustyugov
 * Copyright 2009 MDSP team
 */

#include <cassert>
#include <cmath>

#include "memory.h"
#include "register_file.h"

/**
 * Implementation of class ByteLine
 */
ByteLine::ByteLine()
{
    byte_line = new vector<Byte>;
    output = DEFAULT_OUT;
}

ByteLine::ByteLine( unsigned int count)
{
    byte_line = new vector<Byte>;
    output = DEFAULT_OUT;
    try
    {
        (*byte_line).resize( count);
    }catch ( std::bad_alloc)
    {
        critical("Can not allocate memory for count constructor of ByteLine");
    }
}

ByteLine::ByteLine( const ByteLine& line)
{
    output = DEFAULT_OUT;
    try
    {
        byte_line = new vector<Byte>( line.getSizeOfLine());
    }catch ( std::bad_alloc)
    {
         critical("Can not allocate memory for line constructor of ByteLine");
    }
    for ( unsigned int i = 0 ; i < line.getSizeOfLine(); i++)
    {
        ( *byte_line).at( i).setByteVal( line.getByteVal( i));
        ( *byte_line).at( i).setFlagEnable( line.getByte( i).getFlagEnable());
    }
}

ByteLine::ByteLine( const Byte& byte)
{
    output = DEFAULT_OUT;
    try
    {
        byte_line = new vector<Byte>;
        ( *byte_line).push_back( byte);
    }catch ( std::bad_alloc)
    {
        critical("Can not allocate memory for byte constructor of ByteLine");
    }
}

/* Conversion functions ByteLine into hostUInt8, hostUInt16, hostUInt32 */
hostUInt8 ByteLine::getHostUInt8()
{
    return this->getByteVal( 0);
}

hostUInt16 ByteLine::getHostUInt16()
{
    hostUInt8 temp[] = { this->getByteVal( 1), this->getByteVal( 0)};
    hostUInt16 var = 0;
    var += temp[ 0];
    var = ( var << 8);
    var += temp[ 1];
    return var;
}

hostUInt32 ByteLine::getHostUInt32()
{
    hostUInt8 temp[] = { this->getByteVal( 3), this->getByteVal( 2), 
        this->getByteVal( 1), this->getByteVal( 0)};
    hostUInt32 var = 0;
    for ( int i = 0; i < 3; i++)
    {
        var += temp[ i];
        var = ( var << 8);
    }
    var += temp[3];
    return var;
}

/* Private functions converting hostUInt8,16,32 into vector<Byte> *byte_line */
void ByteLine::convert8( vector<Byte> *byte_line, hostUInt8 var)
{
    Byte byte( var);
    ( *byte_line).push_back( byte);
}

void ByteLine::convert16( vector<Byte> *byte_line, hostUInt16 var, OrderType type)
{
    hostUInt8 temp[2] = { 0, 0};
    for ( int i = 0; i < 2; i++)
    {
        for( int k = 0; k < 8; k++)
        {
            if ( var & 1)
                temp[ i] += ( 1 << k);
            var = ( var >> 1);
        }
    }
    Byte byte[] = { temp[ 0], temp[ 1]};
    if ( type == HIGH_FIRST)
    {
        for ( int i = 0; i < 2; i++)
            ( *byte_line).push_back( byte[ i]);
    } else
    {
        for ( int i = 2; i > 0; i--)
            ( *byte_line).push_back( byte[ i-1]);
    }
}

void ByteLine::convert32( vector<Byte> *byte_line, hostUInt32 var, OrderType type)
{
    hostUInt8 temp[4] = { 0, 0, 0, 0};
    for ( int i = 0; i < 4; i++)
    {
        for( int k = 0; k < 8; k++)
        {
            if ( var & 1)
                temp[ i] += ( 1 << k);
            var = ( var >> 1);
        }
    }
    Byte byte[] = { temp[ 0], temp[ 1], temp[ 2], temp[ 3]};
    if ( type == HIGH_FIRST)
    {
        for ( int i = 0; i < 4; i++)
            ( *byte_line).push_back( byte[ i]);
    } else
    {
        for ( int i = 4; i > 0; i--)
            ( *byte_line).push_back( byte[ i-1]);
    }
}

/* Conversion constructors hostUInt8, hostUInt16 and hostUInt32 in Byteline */
ByteLine::ByteLine( hostUInt8 var, OrderType type)
{
    output = DEFAULT_OUT;
    try
    {
        byte_line = new vector<Byte>;
        convert8( byte_line, var);
    }catch ( std::bad_alloc)
    {
        critical("Can not allocate memory for hostUInt8 constructor of ByteLine");
    }
}

ByteLine::ByteLine( hostUInt16 var, OrderType type)
{
    output = DEFAULT_OUT;
    try
    {
        byte_line = new vector<Byte>;
        convert16( byte_line, var, type);
    }catch ( std::bad_alloc)
    {
        critical("Can not allocate memory for hostUInt16 constructor of ByteLine");
    }
}

ByteLine::ByteLine( hostUInt32 var, OrderType type)
{
    output = DEFAULT_OUT;
    try
    {
        byte_line = new vector<Byte>;
        convert32( byte_line, var, type);
    }catch ( std::bad_alloc)
    {
        critical("Can not allocate memory for hostUInt32 constructor of ByteLine");
    }
}

ByteLine::ByteLine(unsigned int var, ConversionType ctype, OrderType type)
{
    output = DEFAULT_OUT;
    switch( ctype)
    {
    case HUINT8:
        try
        {
           byte_line = new vector<Byte>;
           convert8( byte_line, var);
        }catch ( std::bad_alloc)
        {
            critical("Can not allocate memory for HUINT8 constructor of ByteLine");
        }
        break;
    case HUINT16:
        try
        {
            byte_line = new vector<Byte>;
            convert16( byte_line, var, type);
        }catch ( std::bad_alloc)
        {
            critical("Can not allocate memory for HUINT16 constructor of ByteLine");
        }
        break;
    case HUINT32:
        try
        {
            byte_line = new vector<Byte>;
            convert32( byte_line, var, type);
        }catch ( std::bad_alloc)
        {
            critical("Can not allocate memory for HUINT32 constructor of ByteLine");
        }
        break;
    }
}
        
hostUInt8 ByteLine::getByteVal( unsigned int byte_num) const
{
    if ( byte_num > this->getSizeOfLine())
    {
        critical("In method getByteVal of class ByteLine\nSize of byte line is less than target byte number");
    }
    if ( ( *byte_line).empty())
    {
        critical("In method getByteVal of class ByteLine\nByte line is empty");
    }
    return ( *byte_line).at( byte_num).getByteVal();
}

Byte ByteLine::getByte( unsigned int byte_num) const
{
    if ( byte_num > this->getSizeOfLine())
    {
        critical("In method getByte of class ByteLine\nSize of byte line is less than target byte number");
    }
    if ( ( *byte_line).empty())
    {
        critical("In method getByteVal of class ByteLine\nByte line is empty");
    }
    return ( *byte_line).at( byte_num);
}

void ByteLine::setByte( unsigned int byte_num, const Byte& byte)
{
    if ( byte_num > this->getSizeOfLine())
    {
        critical("In method setByte of class ByteLine\nSize of byte line is less than target byte number");
    }
    if ( ( *byte_line).empty())
    {
         critical("In method setByte of class ByteLine\nByte line is empty");
    }
    ( *byte_line).at( byte_num) = byte;
}


void ByteLine::addByte( const Byte& byte)
{
    try
    {
        ( *byte_line).push_back( byte);
    }catch ( std::bad_alloc)
    {
        critical("Can not allocate memory in method addByte of class ByteLine");
    }
}

void ByteLine::resizeByteLine( unsigned int count)
{
    try
    {
        ( *byte_line).resize( count);
    }catch ( std::bad_alloc)
    {
        critical("Can not allocate memory in method resizeByteLine of class ByteLine");
    }
}


/**
 * Implementation of class MemVal
 */

MemVal::MemVal( const RegVal& rv, unsigned int size_of_segm)
{
    ByteLine( rv.getByteLine());
    size_of_segmentation = size_of_segm;
}

void MemVal::recountLenght()
{
    unsigned int temp = getSizeOfMemVal() % size_of_segmentation;
    if ( temp != 0 && size_of_segmentation != 1)
    {
        temp = size_of_segmentation - temp;
        temp += getSizeOfMemVal();
        resizeByteLine( temp);
    }
}

ByteLine MemVal::getByteLine( unsigned int index, unsigned int count) const
{
    if ( getSizeOfMemVal() < index + count)
    {
        critical("In method getByteLine of MemVal\n Size of MemVal is less than target byte number");
    }
    ByteLine temp( count);
    for ( unsigned int i = 0; i < count ; i++)
    {
		temp.setByte( i, getByte( i + index));
        /*if ( temp.getByte( i).getFlagEnable() != DEFAULT_ENABLE)
        {
            cout << "ERROR: Out of Memory!\n";
            assert( 0);
        }*/
        
    }
    return temp;
}

ByteLine MemVal::getByteLine() const
{
    ByteLine temp( getSizeOfMemVal());
    for ( unsigned int i = 0; i < getSizeOfMemVal(); i++)
    {
		temp.setByte( i, getByte( i));
        /*if ( temp.getByte( i).getFlagEnable() != DEFAULT_ENABLE)
        {
            cout << "ERROR: Out of Memory!\n";
            assert( 0);
        }*/
        
    }
    return temp;
}

void MemVal::writeByteLine( const ByteLine& line, unsigned int index)
{
    if ( getSizeOfMemVal() < index + line.getSizeOfLine())
    {
        critical("In method writeByteLine of MemVal\nSize of byte line is less than target byte number");
    }
    Byte temp;
    for ( unsigned int i = 0; i < line.getSizeOfLine(); i++)
    {
        temp = line.getByte( i);
        temp.setFlagEnable( DEFAULT_ENABLE);
        setByte( i + index, temp);
    }
}

void MemVal::writeByteLine( const ByteLine & line)
{
    if ( getSizeOfMemVal() < line.getSizeOfLine())
    {
        critical("In method writeByteLine of MemVal\nSize of byte line is less than target byte number");
    }
    Byte temp;
    for ( unsigned int i = 0; i < line.getSizeOfLine(); i++)
    {
        temp = line.getByte( i);
        temp.setFlagEnable( line.getByte( i).getFlagEnable());
        setByte( i, temp);
    }
}

/**
 * Implementation of Memory Model
 */

MemoryModel::MemoryModel( unsigned int size)
{
    
    mem_model = new memMap;
    size_of_segmentation = size;

}

ByteLine MemoryModel::readBL(  mathAddr read_ptr, unsigned int num_of_bytes)
{
    if ( ( *mem_model).empty())
    {
        critical("In method readBL of MemoryModel\nMemoryModel is empty");
    }

    memMap::iterator pos, start, end;
    start = find( read_ptr);
    mathAddr temp_addr = start->first;
    end = find( read_ptr + num_of_bytes - 1);
    if ( start == ( *mem_model).end() || end == ( *mem_model).end())
    {
        critical("In method readBL of MemoryModel\nMemoryModel is empty");
    }
    MemVal memval = start->second;
    for ( pos = start; pos != end; ++pos)
    {
        if ( countDistance( pos) > 0)
        {
            critical("In method readBL of MemoryModel\nImpaired memory segmentation\n");
        }
        //mergeMemVal( pos, &memval);
        memval = mergeMemVal( pos);

    }
    ( *mem_model).erase( start, end);
    ( *mem_model).erase( end);
    ( *mem_model)[ temp_addr] = memval;
	ByteLine temp( memval.getByteLine( read_ptr - temp_addr, num_of_bytes));
    for ( unsigned int i = 0; i < temp.getSizeOfLine(); i++)
    {
		if ( temp.getByte( i).getFlagEnable() != DEFAULT_ENABLE)
        {
            critical("Attempt to read from the cell without data");
        }
        
    }
    return temp;
}

MemVal MemoryModel::read(  mathAddr read_ptr, unsigned int num_of_bytes)
{
    MemVal mv( readBL( read_ptr, num_of_bytes));
    return mv;
}

/* Reading funcs returning hostUInt8, 16, 32 */
hostUInt8 MemoryModel::read8( mathAddr addr)
{
    ByteLine bl = readBL( addr, 1);
    return bl.getHostUInt8();
}

hostUInt16 MemoryModel::read16( mathAddr addr)
{
    ByteLine bl = readBL( addr, 2);
    return bl.getHostUInt16();
}

hostUInt32 MemoryModel::read32( mathAddr addr)
{
    ByteLine bl = readBL( addr, 4);
    return bl.getHostUInt32();
}

MemVal MemoryModel::mergeMemVal( memMap::iterator pos)
{
    MemVal mem_val = pos->second;
    if ( countDistance( pos) > 0)
    {
        mem_val.resizeMemVal( ( pos + 1)->first - pos->first);
    }
    mem_val = mem_val + ( pos + 1)->second;
    return mem_val;
}

void MemoryModel::write( mathAddr write_ptr, const ByteLine& line)
{
    memMap::iterator pos, start, end ;
    start = findOrInit( write_ptr);
    mathAddr temp_addr = start->first;
    end = findOrInit( write_ptr + line.getSizeOfLine() - 1);

    MemVal memval = start->second;
    for ( pos = start; pos != end; ++pos)
    {
        memval = mergeMemVal( pos);
    }
    ( *mem_model).erase( start, end);
    ( *mem_model).erase( end);

    memval.writeByteLine( line, write_ptr - temp_addr);
    ( *mem_model)[ temp_addr] = memval;
}

/* Writing hostUInt8, 16, 32 funcs */
void MemoryModel::write8( mathAddr write_ptr, const hostUInt8 val)
{
    ByteLine bl( val, HIGH_FIRST);
    write( write_ptr, bl);
}

void MemoryModel::write16( mathAddr write_ptr, const hostUInt16 val)
{
    ByteLine bl( val, HIGH_FIRST);
    write( write_ptr, bl);
}

void MemoryModel::write32( mathAddr write_ptr, const hostUInt32 val)
{
    ByteLine bl( val, HIGH_FIRST);
    write( write_ptr, bl);
}

memMap::iterator MemoryModel::findOrInit( mathAddr ptr)
{
    memMap::iterator pos;
    MemVal temp( size_of_segmentation);
    mathAddr addr = ptr - ( ptr % size_of_segmentation);

    for ( pos = ( *mem_model).begin(); pos != ( *mem_model).end(); ++pos)
    {
        if ( pos == ptr)
        {
            return pos;
        }
        if ( ( pos->first) > ptr)
        {
            ( *mem_model)[ addr] = temp;
            pos = ( *mem_model).find( addr);
            return pos;

        }
    }
    ( *mem_model)[ addr] = temp;
    pos = ( *mem_model).find( addr);
    return pos;

}

memMap::iterator MemoryModel::find( mathAddr ptr)
{
    memMap::iterator pos;
    mathAddr adrr;
    adrr = ptr - ( ptr % size_of_segmentation);
    for ( pos = ( *mem_model).begin(); pos != ( *mem_model).end(); ++pos)
    {
        if ( pos == ptr)
        {
            return pos;
        }
    }
    return pos = ( *mem_model).end();
}

unsigned int MemoryModel::countDistance( const memMap::iterator pos)
{
    return ( pos + 1)->first - ( pos->first + ( pos->second).getSizeOfMemVal());
}

