#!/bin/bash
# kill -0 does not send a signal but will give a nonzero exit status if the process does not exist
pidwait()
{
	while (kill -0 $1)
	do
	sleep 1
	done
	ls
}
