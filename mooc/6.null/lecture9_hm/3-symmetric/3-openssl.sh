#!/bin/bash
echo "hello" >2.txt
openssl aes-256-cbc -salt -in 2.txt -out 2-entropy

openssl aes-256-cbc -d -in 2-entropy -out 2-decrypt
cmp 2.txt 2-decrypt
if [[ $? == 0 ]]; then
	echo "文件内容相同"
fi
