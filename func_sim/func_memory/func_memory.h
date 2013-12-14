/**
 * func_memory.h - Header of module implementing the concept of 
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// protection from multi-include
#ifndef FUNC_MEMORY__FUNC_MEMORY_H
#define FUNC_MEMORY__FUNC_MEMORY_H

// Generic C++
#include <string>
#include <cassert>
#include <cstdlib>
#include <csignal>
#include <cstring>
#include <iostream>

// uArchSim modules
#include <types.h>
#include <elf_parser.h>

using namespace std;

// Класс elem_of_memory является массивом блоков страниц, блоком страниц или страницей, в зависимости от того, что мы запишем в size. Соответственно, элементы массива указывают на блоки страниц, страницы или на сами участки памяти (uint8)

class elem_of_memory
{
    public:
    elem_of_memory**  array_of_elem_of_memory;
    uint8** array_of_byte;
    uint64 size;

    elem_of_memory() {};
    elem_of_memory(uint64 size_of_elem_of_memory, uint64 size_of_page);
    ~elem_of_memory();
};


class FuncMemory
{
    // You could not create the object
    // using this default constructor
    FuncMemory() {};
    // Самый верхний уровень иерархии памяти. Содержит в себе указатели на блоки.
    elem_of_memory* array_of_sets; 
	
    // Адрес первого элемента области кода .text.
    uint64 start_addr;
  
    // Число бит, выделенное под адрес блока, адрес страницы и адрес байта.
    uint64 sets_bits;
    uint64 page_bits;
    uint64 offset_bits;
	 
public:

    FuncMemory ( const char* executable_file_name,
                 uint64 addr_size = 32,
                 uint64 page_num_size = 10,
                 uint64 offset_size = 12);
    
    virtual ~FuncMemory();
    
    uint64 read( uint64 addr, unsigned short num_of_bytes = 4) const;
    void   write( uint64 value, uint64 addr, unsigned short num_of_bytes = 4);
    
    uint64 startPC() const;
    
    string dump( string indent = "") const;

// Функция, используемая для рекурсивной очистки памяти.

    friend void clear(elem_of_memory* elem);
};

// Функция перевода размера адреса, используемого в элементе памяти в размер самого элемента памяти(кол-ва элементов массива). 

uint64 bit_to_size(uint64 bits);

#endif // #ifndef FUNC_MEMORY__FUNC_MEMORY_H
