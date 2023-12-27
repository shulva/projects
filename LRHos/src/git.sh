#!/bin/bash

echo "message : $1"
make clean
git status
git add .
git commit -m $1
git push
git status
