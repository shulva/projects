#!/usr/bin/env python
# pdf-285 book-196

from pathlib import Path
import re,os,shutil

# for i in range(100):
# filename = "test"+str(i).rjust(3,'0')+".txt"
# open( Path.cwd() / "10-8-3" / filename,'w') 
# 批量生成testxxx.txt文件
# 要测试的话自己手动删除几个即可

regex_prefix = re.compile(r'test(\d{3})\.txt')

i,num=0,0
path = Path.cwd() / "10-8-3"

for filename in os.listdir(path):
    final = regex_prefix.search(filename)
    if final:
        num=int(final.group(1))
        if num-i>0:
            new_name="test"+str(i).rjust(3,'0')+".txt"
            shutil.move(path/filename,path/new_name)
    i=i+1
