#!/bin/bash

# don't forget delete @profile above insertsort and quicksort function before using cProfile 
python3 -m cProfile -s time sorts.py | grep sorts.py
echo "-------------------------------------------------------------------------------"
# don't forget add @profile above insertsort and quicksort function before using line_profiler 
kernprof -l -v sorts.py
# 很明显快排要优于插入排序,插入排序的瓶颈在于while循环,快排的瓶颈在于left与right的赋值

# don't forget add @profile above insertsort and quicksort function before using memory_profiler 
echo "-------------------------------------------------------------------------------"
python3 -m memory_profiler sorts.py
echo "-------------------------------------------------------------------------------"
sudo perf stat -e cycles,cache-references,cache-misses python3 sorts.py
