#!/usr/bin/env bash

for cc in clang++ g++; do
  for opt in -O0 -O1 -O2 -O3 -Ofast; do
    "${cc}" "${opt}" fib.cpp -lbenchmark -lpthread
    echo "=== Compiler: ${cc} / Opt Level: ${opt} ==="
    ./a.out
  done
done
