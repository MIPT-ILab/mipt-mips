/**
 * memory.h - Specification of memory model
 * Define classes and methods to operate with memory of simulated architecture
 * @author Dmitry Ustyugov
 * Copyright 2009 MDSP team
 */

#ifndef MEMORY_H
#define MEMORY_H

#include <vector>
#include <map>
#include <cassert>
#include <iostream>

#include "types.h"
#include "log.h"

#define DEFAULT_OUT HEX // default output form
#define DEFAULT_FLAG 0
#define DEFAULT_ENABLE 1

using namespace std;

class RegVal;

/**
 * class Byte implements
 */
class Byte: public log
{

    hostUInt8 byte_val;
    OutputFormat output;
    unsigned int flag;

public:
    /* Constructors */
    Byte( hostUInt8 val = 0):byte_val( val), output( DEFAULT_OUT), flag( DEFAULT_FLAG){}

    /* Copy constructors */
    Byte( const Byte& byte):byte_val( byte.getByteVal()), output( DEFAULT_OUT), flag( byte.getFlagEnable()){}

    /* Get/set methods */

    unsigned int getFlagEnable() const
    {
        return this->flag;
    }
    void setFlagEnable ( unsigned int temp)
    {
        this->flag = temp;
    }


    /* The constant member function. Returns the Byte value in dec form */
    hostUInt8 getByteVal() const
    {
        return this->byte_val;
    }

    /* Sets the Byte value in dec form */
    void setByteVal( hostUInt8 val)
    {
         this->byte_val = val;
    }

    OutputFormat getOutputFormat() const
    {
        return this->output;
    }
    /* Set methods for output options*/
    void setBinOut()
    {
        this->output = BIN;
    }

    void setDecOut()
    {
        this->output = DEC;
    }

    void setHexOut()
    {
        this->output = HEX;
    }
    /* Clear methods for output options*/
    void clrBinOut()
    {
        this->output = DEFAULT_OUT; // default output is hex
    }

    void clrDecOut()
    {
        this->output = DEFAULT_OUT;// default output is hex
    }

    void clrHexOut()
    {
        this->output = DEFAULT_OUT;// default output is hex
    }
    /* Overloaded. The member operator function
    returns true only if the Byte is the same */
    bool operator== ( const Byte& byte)
    {
        return this->getByteVal() == byte.getByteVal();
    }

    /* Overloaded. The member operator function returns
    true only if the Byte differs from the one */
    bool operator!= ( const Byte& byte)
    {
        return this->getByteVal() != byte.getByteVal();
    }

    /* 
     * Overloaded. Outputs the value of the
     * current Byte object in bin form to screen 
     */
    friend ostream& operator<< ( ostream& os, const Byte& byte);

    /* Overloaded. Returns Byte, shifting to the left upon count */
    friend Byte operator>> ( const Byte& byte, int count);

    /* Overloaded. Returns Byte, shifting to the right upon count */
    friend Byte operator<< ( const Byte& byte, int count);

    /* Overloaded. Returns Byte to be a result of bitwise addition */
    friend Byte operator& ( const Byte& byte, const Byte&);


};

/* Output operator due to output val, class Byte*/
inline ostream& operator<< ( ostream& os, const Byte& byte)
{   
    ostream::fmtflags old_outputFormat = os.flags();
    switch ( byte.getOutputFormat())
    {
case BIN:
    for ( short i = 7; i >= 0; i--) 
    { 
        os << ( ( 1 << i) & byte.getByteVal() ? '1' : '0'); 
    }
    break;
case DEC:
    os << dec << ( int ( byte.getByteVal()));
    break;
case HEX:
    os.setf( ostream::showbase);
    os << hex << ( int ( byte.getByteVal()));
    break;
    }
    os.flags( old_outputFormat);
    return os;
}

inline Byte operator>> ( const Byte& byte, int count)
{
    Byte temp;
    temp.setByteVal( byte.getByteVal() >> count);
    return temp;
}

inline Byte operator<< ( const Byte& byte, int count)
{
    Byte temp;
    temp.setByteVal( byte.getByteVal() << count);
    return temp;
}

inline Byte operator& ( const Byte& left, const Byte& right)
{
    Byte temp;
    temp.setByteVal( left.getByteVal() & right.getByteVal());
    return temp;
}


/**
 * Class ByteLine implements a logical set of bytes
 */
class ByteLine: public log
{
    vector<Byte> *byte_line;
    OutputFormat output;
    /* Functions converting hostUInt8,16,32 into vector<Byte> *byte_line */
    void convert8( vector<Byte> * vc, hostUInt8 hu8);
    void convert16( vector<Byte> * vc, hostUInt16 hu16, OrderType type);
    void convert32( vector<Byte> * vc, hostUInt32 hu32, OrderType type);

public:
    /* Constructors */

    /* Creates empty object of Byteline class */
    ByteLine();

    /* Creates object of Byteline class
    with count Byte, initializing with null bytes */
    ByteLine( unsigned int count);

    /* Copy constructors */
    ByteLine( const ByteLine& bl);

    /* Conversion constructors Byte in ByteLine */
    ByteLine( const Byte& byte);

    /*
     * Conversion constructors hostUInt8, hostUInt16 and hostUInt32 
     * variables into ByteLine
     * OrderType variable manages how the bytes should be located in the ByteLine. 
     * OrderType = HIGH_FIRST recommended.
     */
    ByteLine( hostUInt8 hu8, OrderType type);
    ByteLine( hostUInt16 hu16, OrderType type);
    ByteLine( hostUInt32 hu32, OrderType type);
    
    /*
     * Conversion constructors hostUInt8, hostUInt16 and hostUInt32 
     * constants (HUINT8, HUINT16, HUINT32) into ByteLine 
     * OrderType variable manages how the bytes should be located 
     * in the ByteLine. OrderType = HIGH_FIRST recommended.
     */
    ByteLine( unsigned int num, ConversionType ctype, OrderType otype);

    /* Conversion functions ByteLine into hostUInt8, hostUInt16, hostUInt32 */
    hostUInt8 getHostUInt8();
    hostUInt16 getHostUInt16();
    hostUInt32 getHostUInt32();

    /* Destructor */
    virtual ~ByteLine()
    {
        delete byte_line;
    }

    /* Get/set methods */

    /* 
     * The constant member function. Returns the value of
     * the Byte at position pos in the ByteLine.If that
     * position is invalid, recalls exception 
     */
    hostUInt8 getByteVal( unsigned int num) const;

    /* 
     * The constant member function. Returns the
     * object of class Byte at position pos in the ByteLine.
     * If that position is invalid, recalls exception
     */
    Byte getByte( unsigned int num) const;

    /* 
     * Stores the object of class Byte at position pos in
     * the ByteLine.If that position is invalid,
     * recalls exception 
     */
    void setByte( unsigned int num, const Byte& byte);

    /* Adds object of Byte class to end of ByteLine */
    void addByte( const Byte& byte);

    /* 
     * Resize of ByteLine on count. New member
     * of ByteLine is null bytes 
     */
    void resizeByteLine( unsigned int count);

    /* The constant member function. Return size of Byteline */
    unsigned int getSizeOfLine() const
    {
        return (unsigned int)( *byte_line).size();
    }

    OutputFormat getOutputFormat() const
    {
        return this->output;
    }

    /* Set methods for output options*/
    void setBinOut()
    {
        this->output = BIN;
    }

    void setDecOut()
    {
        this->output = DEC;
    }

    void setHexOut()
    {
        this->output = HEX;
    }
    /* Clear methods for output options*/
    void clrBinOut()
    {
        this->output = DEFAULT_OUT; // default output is hex
    }

    void clrDecOut()
    {
        this->output = DEFAULT_OUT;// default output is hex
    }

    void clrHexOut()
    {
        this->output = DEFAULT_OUT;// default output is hex
    }

    /* Overloaded. Assign the current object of ByteLine class to another */
    ByteLine& operator = ( const ByteLine& bl);

    /* 
     * Overloaded. The constant member function.The member function returns an object of
     * class reference. Returns the Byte at position pos in the ByteLine.
     * If that position is invalid, recalls exception 
     */
    Byte operator[] ( unsigned int num) const;

    /* Overloaded. Outputs the ByteLine in bin form to screen */
    friend ostream& operator<< ( ostream& os, const  ByteLine& bl);

    /* 
     * Overloaded. Returns the ByteLine to be a result of addition two
     * object of class reference 
     */
    friend ByteLine operator+ (  const ByteLine& bl1,  const ByteLine& bl2);

};

inline Byte ByteLine::operator []( unsigned int count) const
{
    if ( ( *byte_line).empty())
    {
        critical("Byte line is empty!");
        
    }
    if ( count > this->getSizeOfLine())
    {
        critical("Size of byte line is less than target byte number!");
    }
    return ( *byte_line).at( count);
}

inline ByteLine& ByteLine::operator = ( const ByteLine& line)
{
    if ( this != &line)
    {
        delete byte_line;
        byte_line = new vector<Byte>( line.getSizeOfLine());
        for ( unsigned int i = 0 ;i < line.getSizeOfLine() ; i++)
        {
        ( *byte_line).at( i).setByteVal( line.getByteVal( i));
        }
    }
    return *this;
}

/* Output operator due to output val, class Byteline*/
inline ostream& operator<< ( ostream& os, const ByteLine& line)
{   
    Byte a( 0);
    for ( unsigned int i = 0; i < line.getSizeOfLine(); i++)
    {
        switch ( line.getOutputFormat())
        {
    case BIN:
        a = ( line[ i]);
        a.setBinOut();
        os << a << " | ";
        break;
    case DEC:
        a = ( line[ i]);
        a.setDecOut();
        os << a << " | ";
        break;
    case HEX:
        a = ( line[ i]);
        a.setHexOut();
        os << a << " | ";
        break;
        }
    }
    return os;
}

inline ByteLine operator+ (  const ByteLine& a,  const ByteLine& b)
{
    ByteLine temp( a);

    for ( unsigned int i = 0 ; i < b.getSizeOfLine(); i++)
    {
        temp.addByte( b.getByte( i));;
    }
    return temp;
}
inline ByteLine operator<< ( const ByteLine& byteline, int count)
{   
    int length = byteline.getSizeOfLine();   // length of byteline
    int temp_byte_num = count/8;             // number of shifting byte
    int right_shift = 8-count%8;             
    int left_shift  = count%8;               
    hostUInt8 left_part, right_part;         //left and right part of byte in temp   
    ByteLine temp( byteline);                        
    int i;
    for ( i = 0; i < length; i++)   // nulling temp
    {
        temp.setByte( i, 0);
    }

    temp.setByte( temp_byte_num, (byteline.getByteVal( 0) << (count%8))); 
    for ( i = 1; i < ( length - count/8); i++)
    { 
        left_part  = byteline.getByteVal( i - 1) >> ( right_shift);
        right_part = byteline.getByteVal( i)     << ( left_shift);
        temp_byte_num = i + count/8;
        temp.setByte( temp_byte_num, left_part | right_part);  
    }     
return temp; 
}

inline ByteLine operator>> ( const ByteLine& byteline, int count)
{
    int length = byteline.getSizeOfLine();   // length of byteline 
    int temp_byte_num = length - count/8 - 1;// number of shifting byte
    int right_shift = 8-count%8;              
    int left_shift  = count%8;                
    hostUInt8 left_part, right_part;                  //left and right part of byte in temp 
    ByteLine temp( byteline);                        
    int i;
    for ( i = 0; i < length; i++)
    {
        temp.setByte( i, 0);
    }
    temp.setByte( temp_byte_num, (byteline.getByteVal( length-1) >> (count%8)));
    for ( i = 1; i < ( length - count/8); i++)
    { 
        left_part  = byteline.getByteVal( length - i) << ( right_shift);
        right_part = byteline.getByteVal( length - i - 1)  >> ( left_shift);
        temp_byte_num = length - count/8 - 1 - i;
        temp.setByte( temp_byte_num, left_part | right_part);  
    }     
return temp; 
}


/**
 * class MemVal implements a object to interaction with memory
 */

class MemVal: public ByteLine
{
    unsigned int size_of_segmentation;

public:

    /* Constructors and destructor */

    /* Copy constructors */
    MemVal( const MemVal &mem_val):ByteLine( mem_val.getByteLine()),
                    size_of_segmentation( mem_val.getSizeOfSegment()){};

    /* Creates empty object of MemVal class */
    MemVal():ByteLine(), size_of_segmentation( 1){};

    /* Creates object of MemVl class ,
    ByteLine, initializing with null bytes */
    MemVal( unsigned int size):ByteLine( size), size_of_segmentation( 1){};

    /* Conversion constructor ByteLine into MemVal */
    MemVal( const ByteLine& line):ByteLine( line),
                         size_of_segmentation( 1){};

    /* Conversion constructor RegVal into MemVal */
    MemVal( const RegVal& reg_val, unsigned int size_of_segm);

    /* Get/set methods */

    /*
     * If size_of_segmentation different from 1,
     * adds null bytes to ByteLine
     * multiple of size_of_segmentation 
     */
    void recountLenght();

    /* Set size_of_segmentation */
    void setSizeOfSegment( unsigned int size)
    {
        size_of_segmentation = size;
        recountLenght();
    }

    /* 
     * The constant member function. Returns ByteLine with specified
     * length and first element with index.If that position is invalid,
     * recalls exception 
     */
    ByteLine getByteLine( unsigned int index, unsigned int len) const;

    /* The constant member function. Returns entire ByteLine */
    ByteLine getByteLine() const;

    /* 
     * Stores the object of class ByteLine at position pos in
     * the MemVal.If that position is invalid,
     * recalls exception 
     */
    void writeByteLine( const ByteLine& bl, unsigned int pos);
    void writeByteLine( const ByteLine& pos);

    /* 
     * Resizes of MemVal on count. New member
     * of ByteLine is null bytes 
     */
    void resizeMemVal( unsigned int size)
    {
        resizeByteLine( size);
        recountLenght();
    }

    /* The constant member function. Returns size of segmentation */
    unsigned int getSizeOfSegment() const
    {
        return size_of_segmentation;
    }

    /* The constant member function. Returns size of MemVal */
    unsigned int getSizeOfMemVal() const
    {
        return getSizeOfLine();
    }

    /* 
     * Overloaded. Returns the MemVal to be a result of addition two
     * object of class reference 
     */
    friend MemVal operator+ ( const MemVal& mv1, const MemVal& mv2);

   /* Overloaded. Assign the current object of MemVal class to another */
    MemVal& operator= ( const MemVal& mv);

};
inline MemVal operator+ ( const MemVal& a, const MemVal& b)
{
    MemVal temp ( a);
    temp.resizeMemVal( a.getSizeOfMemVal() + b.getSizeOfMemVal());
    temp.writeByteLine( b.getByteLine(), a.getSizeOfMemVal());
    return temp;
}
inline MemVal& MemVal::operator= ( const MemVal& mem_val)
{
    if ( this != &mem_val)
    {
        ByteLine temp( mem_val.getByteLine());
        resizeMemVal( mem_val.getSizeOfMemVal());
        setSizeOfSegment( mem_val.getSizeOfSegment());
        this->writeByteLine( temp);
    }
    return *this;
}

/**
 * class MemoryModel implements memory of simulated architecture and infrastructure to operate with it
 */

typedef map< mathAddr, MemVal, std::less< mathAddr> > memMap;
class MemoryModel : public log
{

    memMap *mem_model;
    unsigned int size_of_segmentation;

    /* Closed copy constructors */
    MemoryModel( const MemoryModel& mm);


public:

    /* Constructors and destructor */

    /* 
     * Creates empty object of MemoryModel class
     * with specified size of segmentation 
     */
    MemoryModel( unsigned int size);

    virtual ~MemoryModel()
    {
        delete mem_model;
    }


    /* 
     * Returns ByteLine with specified length from specified
     * address. If that position is invalid, recalls exception 
     */
    ByteLine readBL(  mathAddr addr, unsigned int len);
    
    /* 
     * Returns MemVal with specified length from specified
     * address. If that position is invalid, recalls exception 
     */
    MemVal read( mathAddr addr, unsigned int len);

    /* Returns hostUInt8, 16, 32. If that position is invalid, recalls exceptions */
    hostUInt8 read8( mathAddr addr);
    hostUInt16 read16( mathAddr addr);
    hostUInt32 read32( mathAddr addr);

    /* 
     * Returns pointer to object of MemVal class if will be
     * found in MemoryModel.If not returns poiter to end of MemoryModel.
     * Used in read member function 
     */
    memMap::iterator find( mathAddr addr);

    /* 
     * Returns pointer to object of MemVal class if will be
     * found in MemoryModel.If not create MemVal with specified
     * address. Used in write member function 
     */
    memMap::iterator findOrInit( mathAddr addr);

    /*
     * The constant member function.Returns pointer to first
     * object of MemVal class in MemoryModel 
     */
    memMap::iterator getFirstIter() const
    {
        memMap::iterator pos;
        return pos = ( *mem_model).begin();
    }
    /* 
     * The constant member function. Returns pointer to last
     * object of MemVal class in MemoryModel 
     */
    memMap::iterator getLastIter() const
    {
        memMap::iterator pos;
        return pos = ( *mem_model).end();
    }


    /* Merges two object of MemVal class in MemoryModel*/
    MemVal mergeMemVal( memMap::iterator);


    /* Counts amount of empty address between two object of MemVal class*/
    unsigned int countDistance( const memMap::iterator pos);


    /* Stores the object of class ByteLine at specified address */
    void write( mathAddr write_ptr, const ByteLine& bl);

    /* Stores the object of class MemVal at specified address */
    void write( mathAddr write_ptr, MemVal mem_value);

    /* Funcs storing the hostUInt8, 16, 32 at specified address */
    void write8( mathAddr write_ptr, const hostUInt8 val);
    void write16( mathAddr write_ptr, const hostUInt16 val);
    void write32( mathAddr write_ptr, const hostUInt32 val);

     /* Overloaded. Checks of belonging specified address to object of class MemVal*/
    friend bool operator== ( memMap::iterator pos, mathAddr addr);

    /* Overloaded. Outputs the contents of MemoryModel to screen */
    friend ostream& operator<< ( ostream& os,  MemoryModel& mm);

    /* Overloaded. Returns pointer to the next object of class MemVal*/
    friend memMap::iterator operator+ ( const memMap::iterator pos, int count);

};

inline bool operator ==( memMap::iterator p, mathAddr adrr)
{

    if ( ( adrr >= p->first ) && ( ( ( p->second).getSizeOfMemVal()+ p->first - 1) >=  adrr))
    {
        return true;
    }
    return false;
}
inline ostream& operator<< ( ostream& os,  MemoryModel& model)
{
    memMap::iterator pos;
    for ( pos = model.getFirstIter(); pos != model.getLastIter(); ++pos)
    {
        os<<"address: "<< pos->first<<'\t'<< pos->second<<endl;
    }
    return os;

}
inline memMap::iterator operator+ ( const memMap::iterator pos,  int count)
{
    memMap::iterator temp = pos;
    for ( int i = 0; i < count; i++)
    {
        ++temp;
    }
    return temp;
}



#endif /* MEMORY_H */
