/**
 * register_file.h - Specification of register file (RF) model
 * Define classes and methods to operate with RF of simulated architecture
 * @author Dmitry Korobov, Dmitry Ustyugov
 * Copyright 2009 MDSP team
 */

#ifndef REGISTER_FILE_H
#define REGISTER_FILE_H

#include <cassert>

#include "types.h"
#include "memory.h"

/**
 * Class RegVal implements a object to interaction with RF 
 */
class RegVal: public ByteLine
{
  
public:
    /* Constructors and destructor */
    RegVal( unsigned int size_in_bytes): ByteLine( size_in_bytes){}

    RegVal( hostUInt8 hval8, OrderType type): ByteLine( hval8, type){}
    RegVal( hostUInt16 hval16, OrderType type): ByteLine( hval16, type){}
    RegVal( hostUInt32 hval32, OrderType type): ByteLine( hval32, type){}

    ~RegVal(){}

    /* The constant member function. Returns size of MemVal */
    unsigned int getSizeOfRegVal() const
    {
        return getSizeOfLine();
    }

    /* The constant member function. Returns entire ByteLine */
    ByteLine getByteLine() const;

    RegVal& operator =( RegVal& reg_val)
    {
        if ( this->getSizeOfLine() != reg_val.getSizeOfLine())
	    {
		    cout << "Error: you can write to RegVal with the equal length only!\n";
		    assert( 0);
        }
        int sz_of_ln = this->getSizeOfLine();

        for ( int i = 0; i < sz_of_ln; i++)
        {
            this->setByte( i, reg_val.getByte( i));
        }

        return ( *this);
    }
};

/**
 * class RegisterFileModel implements RF of simulated architecture and infrastructure to operate with it
 */

class RegisterFileModel 
{
    RegVal** reg_file;          // Array of RegVal*
    unsigned int num_of_reg;    // Number of registers in the register file

public:
	
    /* Constructors and destructor */
    RegisterFileModel( unsigned int number_of_registers, unsigned int size_of_register_in_bytes);
    ~RegisterFileModel();
     
    /* Read a logical set of bytes (RegVal) form physical register with number reg_num */
    RegVal* readReg( physRegNum reg_num);

    /* Returns hostUInt8, 16, 32. If that position is invalid, recalls exceptions */
    hostUInt8 read8( physRegNum reg_num);
    hostUInt16 read16( physRegNum reg_num);
    hostUInt32 read32( physRegNum reg_num);
     
    /* Write a logical set of bytes (RegVal) to physical register with number reg_num */
    void writeReg( physRegNum reg_num, RegVal& reg_value);

    /* Funcs storing the hostUInt8, 16, 32 at specified address */
    void write8( physRegNum reg_num, const hostUInt8 val);
    void write16( physRegNum reg_num, const hostUInt16 val);
    void write32( physRegNum reg_num, const hostUInt32 val);
};

#endif /* REGISTER_FILE_H */

