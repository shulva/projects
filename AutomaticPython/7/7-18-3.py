#!/usr/bin/env python
# pdf-186/228 book-146

import re

str1='  aaa  '
str2='aaabbbccc'

def regex_strip(str,arg=None):
    if arg==None:
        left=re.compile(r'^\s*')
        right=re.compile(r'\s*$')
        str=left.sub("",str)
        str=right.sub("",str)
    else:
        all=re.compile(arg)
        str=all.sub("",str)
    print(str)
    return str

regex_strip(str1)
regex_strip(str2,'a')
