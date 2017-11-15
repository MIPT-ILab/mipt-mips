#!/bin/bash
./mipt-mips -b ./../traces/fib.out -n 1000000 | grep "sim freq"
