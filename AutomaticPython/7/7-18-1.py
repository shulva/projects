#!/usr/bin/env python
# pdf-186/228 book-146

import re
import sys

date_regex=re.compile(r'(\d\d)/(\d\d)/(\d\d\d\d)')
date_analyse=date_regex.search(str(sys.argv[1]))

flag=True
day,month,year='','',''

if date_analyse==None:
    print("date do not fit regex")
    sys.exit()
else:
    day,month,year=date_analyse.groups()

int_day=int(day)
int_month=int(month)
int_year=int(year)

if int_month not in [1,2,3,4,5,6,7,8,9,10,11,12]:
    flag=False
if int_month in [2,4,6,9,10]:
    if int_day>30:
        flag=False
else:
    if int_day>31:
        flag=False

if int_month==2 and int_day==29:
    if int_year%4==0 and int_year%100!=0:
        flag=True
    elif int_year%400==0:
        flag=True
    else:
        flag=False

if(not flag):
    print("date is not the real date")
