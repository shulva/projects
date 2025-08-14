#!/bin/bash
count=0

while [ ture ]; do
	((count++))
	./3.sh 2>> 3_log.log

	if [[ $? -eq 1 ]]; then
		echo "fatal !"
		echo "loop $count times"
		exit 1
	fi

done
