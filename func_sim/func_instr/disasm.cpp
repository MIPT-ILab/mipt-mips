# include "func_instr.h"
# include "func_memory.h"
# include <iostream>


void disassembler (std::string file_name, std::string section_name);

int main (int argc, char* argv [])
{
	if (argc != 3)
	{
		std::cout << "Wrong parametrs number" << std::endl;
		std::terminate ();
	}

	disassembler (std::string (argv [1]), std::string (argv [2]));

	return 0;
}

void disassembler (std::string file_name, std::string section_name)
{
	FuncMemory func_mem ( file_name.c_str (), 32, 10, 12);
	std::vector<ElfSection> sections_array;

	ElfSection::getAllElfSections (file_name.c_str (), sections_array);

	uint64 sec_addr = 0ull;
	uint64 sec_size = 0ull;

	for (int num_sec = 0; num_sec < sections_array.size (); num_sec++)
	{
		if (std::string (sections_array [num_sec].name) == section_name)
		{
			sec_addr = sections_array [num_sec].start_addr;
			sec_size = sections_array [num_sec].size      ;
			break;
		}
	}

	std::cout << sec_addr << std::endl;
	std::cout << sec_size << std::endl;

	if (sec_addr == 0ull) std::terminate ();

	std::cout << section_name << std::endl;

	for (int offset = 0; offset < sec_size; offset += sizeof (uint32))
	{
		uint64 bytes = func_mem.read (sec_addr + offset, sizeof (uint32));
		//std::cout << "0x"<< std::hex << bytes << std::endl;

		FuncInstr instr (bytes);

		std::cout << instr.Dump ("    ") << std::endl;
	}

}