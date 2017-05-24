cl /I. /EHsc libelf.lib /Femipt-mips ^
   /W4 /WX /wd4505 /wd4244 /wd4996 ^
   infra/memory/memory.cpp ^
   infra/config/config.cpp ^
   infra/ports/ports.cpp ^
   infra/cache/cache_tag_array.cpp ^
   mips/mips_instr.cpp ^
   func_sim/func_sim.cpp ^
   perf_sim.cpp main.cpp ^
   infra/elf_parser/elf_parser.cpp 
