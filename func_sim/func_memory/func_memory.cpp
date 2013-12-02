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

//------------------------------------------------------------------------------

// Конструктор класса elem_of_memory. 

elem_of_memory::elem_of_memory(uint64 size_of_elem_of_memory, uint64 size_of_page)  
{
// Присваиваем элементу памяти размер его массива(т.е. косвенно определяем, какой это элемент - массив блоков страниц, блок страниц или страница).
	elem_of_memory.size = size_of_elem_of_memory;

	uint64 counter = 0;
// Если этот элемент - страница, то она должна содержать в себе массив указателей на байты памяти(uint8). Поэтому мы выделяем необходимую для массива память и инициализируем все указатели на память в массиве как NULL
	if(elem_of_memory.size == size_of_page)
	{
		(uint8*)elem_of_memory.array = new uint8*[elem_of_memory.size];
		while(counter < elem_of_memory.size)
		{
			elem_of_memory.array[counter] = NULL;
			counter++;
		}
	}
// Если этот элемент не страница, то в её массиве содержатся указатели на другие элементы памяти(elem_of_memory). Поэтому мы делаем то же что и в предыдущем случае, только тип элементов массива заменяем на elem_of_memory.
	else
	{
		(elem_of_memory*)elem_of_memory.array = new elem_of_memory*[elem_of_memory.size];
		while(counter < elem_of_memory.size)
		{
			elem_of_memory.array[counter] = NULL;
		}	
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
	uint64 array_of_sets_bits = addr_size - page_bits - offset_bits;
// Считаем размер каждого типа элементов памяти(число элементов массива в элементе памяти).
	uint64 size_of_array_of_sets = bit_to_size(array_of_sets_bits);
	uint64 size_of_set = bit_to_size(offset_bits);
	uint64 size_of_page = bit_to_size(page_bits);
// Создаем элемент памяти, отвечающий за верхний уровень иерархии(массив блоков). Он содержит массив указателей на элементы среднего уровня иерархии памяти(блоки). При создании все указатели в массиве равны NULL.
	elem_of_memory array_of_sets(size_of_array_of_sets, size_of_page);
// Запихиваем секции данных в созданную нами память.	
	uint64 sections_counter;
	for(sections_counter = 0, sections_counter < sections_array.size(), sections_counter++)
	{
// Выделяем одну секцию.
		ElfSection this_section = sections_array[sections_counter];
		uint64 byte_counter;
		for(byte_counter = 0, byte_counter < this_section.size, byte_counter++)
		{
// Для каждого байта из секции находем номер его блока, страницы и непосредственно местоположения. 
			this_addr = this_section.start_addr + byte_counter;
			this_addr_in_array_of_sets = this_addr >> (offset_bits + page_bits);
			this_addr_in_set = (this_addr << array_of_sets_bits) >> (array_of_sets_bits + page_bits);
			this_addr_in_page = (this_addr << (array_of_sets_bits + offset_bits)) >> (array_of_sets_bits + offset_bits);
// Проверяем, выделена ли память под блок и страницу, если нет - выделяем.
			if(array_of_sets.array[this_addr_in_array_of_sets] == NULL)
			{
				elem_of_memory set(size_of_set, size_of_page);
				*(array_of_sets.array[this_addr_in_array_of_sets]) = set;	
			} 
			else
			{
				*(array_of_sets.array[this_addr_in_array_of_sets]) = set;
			}
// Проверяем, выделена ли память под страницу, если нет, выделяем.
			if(array_of_sets.array[this_addr_in_array_of_sets] -> array[this_addr_in_set] == NULL)
			{
				elem_of_memory page(size_of_page,  size_of_page);
				*(array_of_sets.array[this_addr_in_array_of_sets] -> array[this_addr_in_set]) = page;
			}		
			else
			{
				*(array_of_sets.array[this_addr_in_array_of_sets] -> array[this_addr_in_set]) = page;
			}
// Записываем данные по нашему адресу, предварительно выделив память.
			array_of_sets.array[this_addr_in_array_of_sets] -> array[this_addr_in_set] -> array[this_addr_in_page] = new uint8;
			*(array_of_sets.array[this_addr_in_array_of_sets] -> array[this_addr_in_set] -> array[this_addr_in_page]) = this_section.content[byte_counter];
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

// Деструктор класса FuncMemory.

FuncMemory::~FuncMemory()
{
    // put your code here
}

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
