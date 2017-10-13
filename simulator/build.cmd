@echo off
rem Batch file to build MIPT-MIPS with Visual Studio
rem Copyright 2017 (C) MIPT-MIPS
rem Pavel Kryukov

rem Clean up
del *.obj

rem Build object files
cl /I. /EHsc /c /nologo /MD ^
   /D__LIBELF_INTERNAL__=1 ^
   /D_HAS_AUTO_PTR_ETC=1 ^
   /W4 /WX /wd4505 /wd4244 /wd4996 /wd4267 ^
   /std:c++17 ^
   infra/elf_parser/elf_parser.cpp ^
   infra/memory/memory.cpp ^
   infra/config/config.cpp ^
   infra/ports/ports.cpp ^
   infra/cache/cache_tag_array.cpp ^
   mips/mips_instr.cpp ^
   func_sim/func_sim.cpp ^
   core/perf_sim.cpp || exit /b

rem Build GoogleTest
cl /EHsc /c /nologo /MD ^
   ..\googletest\googletest\src\gtest-all.cc ^
   /I ..\googletest\googletest\ /I ..\googletest\googletest\include\ || exit /b

set TRUNK=%cd%
set TRUNKX=%TRUNK:\=\\%

rem Build and run all the tests
for %%G in (infra\elf_parser infra\memory mips func_sim bpu core) do (
    echo Testing %%G
    cd %%G\t
    cl /nologo unit_test.cpp %TRUNK%\*.obj %TRUNK%\..\libelf\lib\libelf.lib ^
       /EHsc /I %TRUNK%\..\googletest\googletest\include\ /I %TRUNK% /Fetest ^
       /D_HAS_AUTO_PTR_ETC=1
       /std:c++17 ^
       /DTEST_PATH=\"%TRUNKX%\\..\\traces\\tt.core.out\" /MD || exit /b
    .\test.exe || exit /b
    cd %TRUNK%
)

rem Build main.cpp
cl /I. /EHsc /c /nologo /MD ^
   /D__LIBELF_INTERNAL__=1 ^
   /D_HAS_AUTO_PTR_ETC=1 ^
   /W4 /WX /wd4505 /wd4244 /wd4996 /wd4267 ^
   /std:c++17 ^
   main.cpp || exit /b

rem Build MIPT-MIPS
cl ..\libelf\lib\libelf.lib *.obj /Femipt-mips /nologo /MD || exit /b
