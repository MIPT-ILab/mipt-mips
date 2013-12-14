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
                        uint64 page_num_size,
                        uint64 offset_size)
{
// Создаем вектор sections_array и загружаем в него секции памяти из elf файла.
	vector<ElfSection> sections_array;
	ElfSection::getAllElfSections(executable_file_name, sections_array);

// Считаем число бит, уходящее на адрес блока, адрес страницы и адрес байта.
	sets_bits = addr_size - page_num_size - offset_size;
	page_bits = page_num_size;
	offset_bits = offset_size;

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
// Выделяем одну секцию. Смотрим её название, если это секция кода(.text), то записываем её начальный адрес.
	
		if(strcmp(sections_array[sections_counter].name, ".text") == 0)
		{
			start_addr = sections_array[sections_counter].start_addr;
		}

// Назначение этих переменных будет понятно далее.
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

// Функция, возвращающая начальное значение Program Counter.

uint64 FuncMemory::startPC() const
{
	return start_addr;
}

//-----------------------------------------------------------------------------

// Функция, возвращающая указанное кол-во байт из памяти начиная с указанного адреса.

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
// Прверяем, не равно ли нулю число считываемых байт. Если равно - ошибка.
	assert(num_of_bytes != 0);
	
// Считываем байты из памяти и записываем в возвращаемое значение.
	uint64 data = 0;

	for(uint64 counter = 0; counter < num_of_bytes; counter++)
	{
// Для каждого считываемого байта:

	// Записываем его адрес.

		addr = addr + counter;
	
	// Считаем адрес в каждой секции. Если адрес 32-битный, то зануляем первые 32 бита addr.
		uint64 addr_in_array_of_sets = addr >> (offset_bits + page_bits);
        	uint64 addr_in_set = addr << sets_bits;
		if(offset_bits + page_bits + sets_bits == 32)
		{
			addr_in_set = addr_in_set & 0x00000000ffffffff;
		}
		addr_in_set = addr_in_set >> (sets_bits + offset_bits);
        	uint64 addr_in_page = addr << (sets_bits + page_bits);
		if(offset_bits + page_bits + sets_bits == 32)
		{
			addr_in_page = addr_in_page & 0x00000000ffffffff;
		}
		addr_in_page = addr_in_page >> (sets_bits + page_bits);

	// Проверяем, велась ли вообще запись по данному адресу.
		assert(array_of_sets -> array_of_elem_of_memory[addr_in_array_of_sets] != NULL);
		assert(array_of_sets -> array_of_elem_of_memory[addr_in_array_of_sets] -> array_of_elem_of_memory[addr_in_set] != NULL);
		assert(array_of_sets -> array_of_elem_of_memory[addr_in_array_of_sets] -> array_of_elem_of_memory[addr_in_set] -> array_of_byte[addr_in_page] != NULL);
	
	// Считываем байт. 
		uint64 byte = *(array_of_sets -> array_of_elem_of_memory[addr_in_array_of_sets] -> array_of_elem_of_memory[addr_in_set] -> array_of_byte[addr_in_page]);
		
	// Записываем байт в возвращаемый параметр.
		byte = byte << (8 * counter);
		data = data + byte;
	
	// Восстанавливаем значение начального адреса, чтобы мы могли получить адрес следующего быйта для чтения.
		addr = addr - counter;
	}	
	return data;
}

//-----------------------------------------------------------------------------

// Функция, записывающая указанное значение в память, начиная с указанного адреса.

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
// Прверяем, не равно ли нулю число записываемых байт. Если равно - ошибка.
        assert(num_of_bytes != 0);
       
// Также считаем размер масивов указателей для каждого уровня памяти. Это понадобится в случае, если мы будем записывать данные в ранее не использовавшийся участок памяти, и нам понадобится выделять для него память.
        uint64 size_of_array_of_sets = bit_to_size(sets_bits);
        uint64 size_of_set = bit_to_size(page_bits);
        uint64 size_of_page = bit_to_size(offset_bits);

// Записываем данные в память.
	for(uint32 counter = 0; counter < num_of_bytes; counter++)
        {
// Для каждого записываемого байта:

	// Считаем его полный адрес:
		addr = addr + counter;
	// Считаем адрес в каждой секции. Если адрес 32-битный, то зануляем первые 32 бита addr.
                uint64 addr_in_array_of_sets = addr >> (offset_bits + page_bits);
                uint64 addr_in_set = addr << sets_bits;
                if(offset_bits + page_bits + sets_bits == 32)
                {
                        addr_in_set = addr_in_set & 0x00000000ffffffff;
                }
                addr_in_set = addr_in_set >> (sets_bits + offset_bits);
                uint64 addr_in_page = addr << (sets_bits + page_bits);
                if(offset_bits + page_bits + sets_bits == 32)
                {
                        addr_in_page = addr_in_page & 0x00000000ffffffff;
                }
                addr_in_page = addr_in_page >> (sets_bits + page_bits);

	// Выделяем записываемый байт из всех данных.
		uint8 write_byte =(uint8)((value << (8 * (sizeof(uint64) - 1 - counter))) >> (8 * (sizeof(uint64) - 1)));

	// Проверяем, выделена ли память под блок, если нет - выделяем.
                if(array_of_sets -> array_of_elem_of_memory[addr_in_array_of_sets] == NULL)
                {
                        array_of_sets -> array_of_elem_of_memory[addr_in_array_of_sets] = new elem_of_memory(size_of_set, size_of_page);
                }

	// Проверяем, выделена ли память под страницу, если нет, выделяем.
                if(array_of_sets -> array_of_elem_of_memory[addr_in_array_of_sets] -> array_of_elem_of_memory[addr_in_set] == NULL)
                {
                        array_of_sets -> array_of_elem_of_memory[addr_in_array_of_sets] -> array_of_elem_of_memory[addr_in_set] = new elem_of_memory(size_of_page, size_of_page);
                }

	// Проверяем, выделена ли память под байт, если нет - выделяем. Затем записываем в байт данные.
	
                if(array_of_sets -> array_of_elem_of_memory[addr_in_array_of_sets] -> array_of_elem_of_memory[addr_in_set] -> array_of_byte[addr_in_page] == NULL)
		{
			array_of_sets -> array_of_elem_of_memory[addr_in_array_of_sets] -> array_of_elem_of_memory[addr_in_set] -> array_of_byte[addr_in_page] = new uint8(write_byte);
		}
		else
		{
			*(array_of_sets -> array_of_elem_of_memory[addr_in_array_of_sets] -> array_of_elem_of_memory[addr_in_set] -> array_of_byte[addr_in_page]) = write_byte;
		}

		// Возращаем адрес записи в начальное значение, иначе мы не сможем получить нужный адрес для следующего байта.
		addr = addr - counter;
	}
}

//------------------------------------------------------------------------------

// Функция, распечатывающая содержимое нашей памяти.

string FuncMemory::dump( string indent) const
{
	for(uint64 sets_counter = 0; sets_counter < array_of_sets -> size; sets_counter++)
	{
		if(array_of_sets -> array_of_elem_of_memory[sets_counter] != NULL)
		{
			for(uint64 page_counter = 0; page_counter < array_of_sets -> array_of_elem_of_memory[sets_counter] -> size; page_counter++)
			{
				if(array_of_sets -> array_of_elem_of_memory[sets_counter] -> array_of_elem_of_memory[page_counter] != NULL)
				{
					for(uint64 byte_counter = 0; byte_counter < array_of_sets -> array_of_elem_of_memory[sets_counter] -> array_of_elem_of_memory[page_counter] -> size; byte_counter++)
					{
						if(array_of_sets -> array_of_elem_of_memory[sets_counter] -> array_of_elem_of_memory[page_counter] -> array_of_byte[byte_counter] != NULL)
						{
							uint32 byte_addr = byte_counter + (page_counter << offset_bits) + (sets_counter << (offset_bits + page_bits));
							cout.fill('0');
							cout.width(8);
							cout << hex << byte_addr << "   ";
							cout.fill('0');
                                                        cout.width(2);
							cout << (uint32)(*(array_of_sets -> array_of_elem_of_memory[sets_counter] -> array_of_elem_of_memory[page_counter] -> array_of_byte[byte_counter])) << "\n";
						}
					}
				}
			}
		}
	}
	return string("Done");
}
