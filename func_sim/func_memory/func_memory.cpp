/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C

// Generic C++

// uArchSim modules
#include <func_memory.h>
#include <iostream>

using namespace std;
//------------------------------------------------------------------------------

// Конструктор класса elem_of_memory. 

elem_of_memory::elem_of_memory(uint64 size_of_elem_of_memory, uint64 size_of_page)  
{
// Присваиваем элементу памяти размер его массива(т.е. косвенно определяем, какой это элемент - массив блоков страниц, блок страниц или страница).
	size = size_of_elem_of_memory;
	uint64 counter = 0;
// Если этот элемент - страница, то она должна содержать в себе массив указателей на байты памяти(uint8). Поэтому мы выделяем необходимую для массива память и инициализируем все указатели на память в массиве как NULL
	if(size == size_of_page)
	{
		array_of_elem_of_memory = NULL;
		array_of_byte = new uint8*[size];
		assert(array_of_byte != NULL);
		while(counter < size)
		{
			array_of_byte[counter] = NULL;
			counter++;
		}
	}
// Если этот элемент не страница, то в её массиве содержатся указатели на другие элементы памяти(elem_of_memory). Поэтому мы делаем то же что и в предыдущем случае, только тип элементов массива заменяем на elem_of_memory.
	else
	{
		array_of_byte = NULL;
		array_of_elem_of_memory = new elem_of_memory*[size];
		assert(array_of_elem_of_memory != NULL);
		while(counter < size)
		{
			array_of_elem_of_memory[counter] = NULL;
			counter++;
		}
	}
}
//------------------------------------------------------------------------------

// Деструктор класса elem_of_memory. Он ничего не делает, так как всю работу за него выполняет функция clear.

elem_of_memory::~elem_of_memory()
{
	
}

//------------------------------------------------------------------------------

// Функция clear, используемая для рекурсивной очистки памяти.

void clear(elem_of_memory* elem)
{
	uint64 counter;
	if(elem -> array_of_byte == NULL)
	{
		for(counter = 0; counter < elem -> size; counter++)
		{
			if(elem -> array_of_elem_of_memory[counter] != NULL)
			{
				clear(elem -> array_of_elem_of_memory[counter]);
				delete elem -> array_of_elem_of_memory[counter];
			}
			assert(counter < elem -> size);		
		}
		delete elem -> array_of_elem_of_memory;
	}
	if(elem -> array_of_elem_of_memory == NULL)
	{
		for(counter = 0; counter < elem -> size; counter++)
		{
			if(elem -> array_of_byte[counter] != NULL)
			{
				delete elem -> array_of_byte[counter];
			}
			assert(counter < elem -> size);
		}
		delete elem -> array_of_byte;
	}
}

//------------------------------------------------------------------------------

// Конструктор класса FuncMemory.

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
{
// Создаем вектор sections_array и загружаем в него секции памяти из elf файла.
	vector<ElfSection> sections_array;
	ElfSection::getAllElfSections(executable_file_name, sections_array);
// Считаем число бит, уходящее на адрес блока в массиве блоков(верхний уровень иерархии памяти).
	uint64 sets_bits = addr_size - page_bits - offset_bits;
// Считаем размер каждого типа элементов памяти(число элементов массива в элементе памяти).
	uint64 size_of_array_of_sets = bit_to_size(sets_bits);
	uint64 size_of_set = bit_to_size(page_bits);
	uint64 size_of_page = bit_to_size(offset_bits);
	
// Инициализируем массив блоков, входящий в класс FuncMemory(верхний уровень иерархии памяти).
	array_of_sets = new elem_of_memory(size_of_array_of_sets, size_of_page);
// Запихиваем секции данных в созданную нами память.	
	uint64 sections_counter;
	for(sections_counter = 0; sections_counter < sections_array.size(); sections_counter++)
	{
// Выделяем одну секцию. Назначение этих переменных будет понятно далее.
		uint32 this_addr_32 = 0;
		uint32 this_addr_in_array_of_sets_32 = 0;
		uint32 this_addr_in_set_32 = 0;
		uint32 this_addr_in_page_32 = 0;

		uint64 this_addr_64 = 0;
                uint64 this_addr_in_array_of_sets_64 = 0;
                uint64 this_addr_in_set_64 = 0;
                uint64 this_addr_in_page_64 = 0;

		uint64 this_addr = 0;
                uint64 this_addr_in_array_of_sets = 0;
                uint64 this_addr_in_set = 0;
                uint64 this_addr_in_page = 0;
	
		
		ElfSection this_section = sections_array[sections_counter];
		uint64 byte_counter;
		for(byte_counter = 0; byte_counter < this_section.size; byte_counter++)
		{
// Для каждого байта из секции находем номер его блока, страницы и непосредственно местоположения. Учитываем, что адреса могут быть как 32 бита, так и 64 бита.

// Для 32 битных адресов: 

		        this_addr_32 = (uint32)this_section.start_addr + (uint32)byte_counter;
			this_addr_in_array_of_sets_32 = this_addr_32 >> (offset_bits + page_bits);
			this_addr_in_set_32 = (this_addr_32 << sets_bits) >> (sets_bits + offset_bits);
			this_addr_in_page_32 = (this_addr_32 << (sets_bits + page_bits)) >> (sets_bits + page_bits);
			
// Для 64 битных адресов:

			this_addr_64 = this_section.start_addr + byte_counter;
                        this_addr_in_array_of_sets_64 = this_addr_64 >> (offset_bits + page_bits);
                        this_addr_in_set_64 = (this_addr_64 << sets_bits) >> (sets_bits + offset_bits);
                        
			this_addr_in_page_64 = (this_addr_64 << (sets_bits + page_bits)) >> (sets_bits + page_bits);
			
			this_addr_in_array_of_sets = 0;
			this_addr_in_set = 0;
			this_addr_in_page = 0;
			
			if(addr_size == 32)
			{
				this_addr_in_array_of_sets = this_addr_in_array_of_sets_32;
				this_addr_in_set = this_addr_in_set_32;
				this_addr_in_page = this_addr_in_page_32;
			}
			if(addr_size == 64)
			{
				this_addr_in_array_of_sets = this_addr_in_array_of_sets_64;             
                                this_addr_in_set = this_addr_in_set_64;
                                this_addr_in_page = this_addr_in_page_64;
			}
			
// Проверяем, выделена ли память под блок, если нет - выделяем.
			if(array_of_sets -> array_of_elem_of_memory[this_addr_in_array_of_sets] == NULL)
			{
				array_of_sets -> array_of_elem_of_memory[this_addr_in_array_of_sets] = new elem_of_memory(size_of_set, size_of_page);	
			} 
// Проверяем, выделена ли память под страницу, если нет, выделяем.
			if(array_of_sets -> array_of_elem_of_memory[this_addr_in_array_of_sets] -> array_of_elem_of_memory[this_addr_in_set] == NULL)
			{
				array_of_sets -> array_of_elem_of_memory[this_addr_in_array_of_sets] -> array_of_elem_of_memory[this_addr_in_set] = new elem_of_memory(size_of_page, size_of_page);
			}
// Записываем данные по нашему адресу, предварительно выделив память.
			array_of_sets -> array_of_elem_of_memory[this_addr_in_array_of_sets] -> array_of_elem_of_memory[this_addr_in_set] -> array_of_byte[this_addr_in_page] = new uint8(this_section.content[byte_counter]);
		}
	}
}

//------------------------------------------------------------------------------

// Функция перевода размера адреса, используемого в элементе памяти в размер самого элемента памяти(кол-ва элементов массива).

uint64 bit_to_size(uint64 bits)
{
	uint64 size = 1;
	while(bits > 0) 
	{
		size = size * 2;
		bits--;
	}
	return size;
}

//-----------------------------------------------------------------------------

// Деструктор класса FuncMemory. Он вызывает функцию clear для верхнего уровня памяти, содержащего в себе указатели на блоки. Эта функция рекурсивно очищает память.

FuncMemory::~FuncMemory()
{
    clear(array_of_sets);
    delete array_of_sets;
}

//-----------------------------------------------------------------------------

uint64 FuncMemory::startPC() const
{
    // put your code here
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    // put your code here

    return 0;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    // put your code here
}

string FuncMemory::dump( string indent) const
{
    // put your code here
    return string("ERROR: You need to implement FuncMemory!");
}
