#!/bin/bash
#alias dc=cd 但是dc名被另一个命令占用了
history | awk '{$1="";print substr($0,2)}' | sort | uniq -c | sort -n | tail -n 10
