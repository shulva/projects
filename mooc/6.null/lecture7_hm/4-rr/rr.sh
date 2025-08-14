#!/bin/bash
gcc -g demo.c -o demo
./demo
rr record ./demo
rr replay
