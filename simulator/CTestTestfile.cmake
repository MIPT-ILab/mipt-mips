# CMake generated Testfile for 
# Source directory: /home/alex/mipt-mips/simulator
# Build directory: /home/alex/mipt-mips/simulator
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(infra_cache_test "/home/alex/mipt-mips/simulator/infra_cache_test")
add_test(infra_elf_parser_test "/home/alex/mipt-mips/simulator/infra_elf_parser_test")
add_test(infra_memory_test "/home/alex/mipt-mips/simulator/infra_memory_test")
add_test(infra_config_test "/home/alex/mipt-mips/simulator/infra_config_test")
add_test(infra_instrcache_test "/home/alex/mipt-mips/simulator/infra_instrcache_test")
add_test(infra_ports_test "/home/alex/mipt-mips/simulator/infra_ports_test")
add_test(infra_string_test "/home/alex/mipt-mips/simulator/infra_string_test")
add_test(bpu_test "/home/alex/mipt-mips/simulator/bpu_test")
add_test(func_sim_test "/home/alex/mipt-mips/simulator/func_sim_test")
add_test(core_test "/home/alex/mipt-mips/simulator/core_test")
add_test(mips_mips_register_test "/home/alex/mipt-mips/simulator/mips_mips_register_test")
subdirs(build)
