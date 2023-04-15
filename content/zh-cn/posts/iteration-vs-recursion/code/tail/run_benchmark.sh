#!/usr/bin/env bash

for opt in -O0 -O1 -O2 -O3 -Ofast; do
  clang++ "${opt}" fact.cpp -lbenchmark -lpthread
  echo "=== Opt Level: ${opt} ==="
  ./a.out
done
