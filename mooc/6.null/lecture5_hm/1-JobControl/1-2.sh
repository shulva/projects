#!/bin/bash
# the & suffix in a command will run the command in the background
sleep 60 & 
jobs
wait $(pgrep sleep) 
ls
