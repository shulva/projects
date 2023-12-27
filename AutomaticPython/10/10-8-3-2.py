#!/usr/bin/env python
# pdf-285 book-196

from pathlib import Path
import os,random

# for i in range(100):
# filename = "test"+str(i).rjust(3,'0')+".txt"
# open( Path.cwd() / "10-8-3-2" / filename,'w') 
# 批量生成testxxx.txt文件

path = Path.cwd() / "10-8-3-2"

for i in range(10):
    num=random.randint(0,99)
    filename="test"+str(num).rjust(3,'0')+".txt"
    os.unlink(path/filename)
