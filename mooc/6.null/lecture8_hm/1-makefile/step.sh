#!/bin/bash
make
ls -al
make clean
ls -al
#git ls-files -o | xargs rm -f is also fine
# this git command will show those untracked files, usually is the intermediary(middleware)
