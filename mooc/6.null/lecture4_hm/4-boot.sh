#!/bin/bash
cat /dev/null > boot_time.txt
for i in {0..9}; do
   journalctl -b-$i | grep "Startup finished in" >> boot_time.txt
done
# n min y s的形式也可处理
echo "单位为秒"
echo -n "max is:"
cat boot_time.txt | grep "systemd\[1\]" | sed -E "s/.*= (.*\..*)s/\1/" | sed -E "s/(.*)min (.*)/\2 \1/" | awk '{print $2*60+$1}' | sort | tail -n1

echo -n "min is:"
cat boot_time.txt | grep "systemd\[1\]" | sed -E "s/.*= (.*\..*)s/\1/" | sed -E "s/(.*)min (.*)/\2 \1/" | awk '{print $2*60+$1}' | sort -r | tail -n1

echo -n "average is:"
cat boot_time.txt | grep "systemd\[1\]" | sed -E "s/.*= (.*\..*)s/\1/" | sed -E "s/(.*)min (.*)/\2 \1/" | awk '{print $2*60+$1}' |paste -sd+ | bc -l | awk '{print $1/10}'

echo -n "median is:"
cat boot_time.txt | grep "systemd\[1\]" | sed -E "s/.*= (.*\..*)s/\1/" | sed -E "s/(.*)min (.*)/\2 \1/" | awk '{print $2*60+$1}' | sort | paste -sd\ | awk '{print ($5+$6)/2}'
