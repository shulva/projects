#!/bin/bash
cat /dev/null > boot_time.txt
for i in {0..2}; do
   journalctl -b-$i | grep "Startup finished in" >> restart_info.txt
done

cat restart_info.txt | sed -E "s/.*kernel:\ (.*)/\1/" | uniq -c | sort | awk ' $1!=3 { print } '
