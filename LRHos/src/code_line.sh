#!/bin/bash
find . -name '*.h' | xargs wc -l
find . -name '*.sh' | xargs wc -l
find . -name '*.c' | xargs wc -l
find . -name '*.s' | xargs wc -l
