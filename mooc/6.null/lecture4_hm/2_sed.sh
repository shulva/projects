#!/bin/bash

#-v : 显示不包含匹配文本的所有行

echo "the number of words (in /usr/share/dict/words) that contain at least three a and don’t have a 's ending is:"
cat /usr/share/dict/words | tr "[:upper:]" "[:lower:]" | grep -v \'s$ | grep -e ".*a.*a.*a.*" | wc -l

echo "the three most common last two letters of those words is:"
cat /usr/share/dict/words | tr "[:upper:]" "[:lower:]"|  grep -v \'s$ | grep -e ".*a.*a.*a.*"| sed -E "s/.*(\w{2})/\1/" | sort | uniq -c | sort | tail -n3

echo "How many of those two-letter combinations are there? "
cat /usr/share/dict/words | tr "[:upper:]" "[:lower:]"|  grep -v \'s$ | grep -e ".*a.*a.*a.*"| sed -E "s/.*(\w{2})/\1/" | sort | uniq | wc -l

echo "challenge: which combinations do not occur?"
diff <(cat /usr/share/dict/words | tr "[:upper:]" "[:lower:]"|  grep -v \'s$ | grep -e ".*a.*a.*a.*"| sed -E "s/.*(\w{2})/\1/" | sort | uniq ) <(cat 2_combinations.txt) | grep -e ">.*" | awk '{print $2}' | paste -sd,



