#!/usr/bin/zsh
# " data come from https://use-land-property-data.service.gov.uk/datasets/rfi/download"
# 先把字符串转成数字
# NF: number of field (column)


echo "the max of the column is"
cat 6-Customer.csv | sed -E 's/"//g' | awk -F ,  '$2 ~ /[0-9]+/ {print($2);}' | sort -n | tail -n1


echo "the min of the column is"
cat 6-Customer.csv | sed -E 's/"//g' | awk -F ,  '$2 ~ /[0-9]+/ {print($2);}' | sort -n | head -n1

echo "and the difference of the sum of each column in another is"
cat 6-Customer.csv | sed -E 's/"//g' | awk -F , ' { for(i=2; i<=NF; i++) a[i]+=$i } END { for(j in a) print "the difference between column " j-1 " and " j " is " a[j-1]-a[j] }'






