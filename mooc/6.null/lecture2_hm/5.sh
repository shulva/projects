#!/bin/bash 
# remember that is recursively 
# find -mtime +n 会寻找修改时间小于当前时间-(n+1)天的文件
# find -mtime  n 会寻找修改时间等于当前时间-(n+1)天的文件
# find -mtime -n 会寻找修改时间大于当前时间-(n+1)天的文件
# 仅为个人理解
echo " the most rencently modified file is $(find $1 -mtime +-1 -type f | xargs -d '\n' ls -at | head -1 ) "
echo "------------------------------------------"
find $1 -mtime +-1 -type f | xargs -d '\n' ls -alt 
