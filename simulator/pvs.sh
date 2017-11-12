#!/bin/bash
make clean
pvs-studio-analyzer trace -- make mipt-mips && pvs-studio-analyzer analyze -o ./project.log --compiler $CXX && plog-converter -a GA:1,2 -t tasklist -o ./project.tasks ./project.log
cat ./project.tasks
