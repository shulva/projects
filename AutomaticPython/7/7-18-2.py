#!/usr/bin/env python
# pdf-186/228 book-146

import sys
import re

password=str(sys.argv[1])
password_regex=re.compile(r'''
                          ^
                          (?=.*[A-Z])
                          (?=.*[a-z])
                          (?=.*[0-9])
                          .{8,}
                          $
                          ''',re.VERBOSE)

if password_regex.search(password)!=None:
    print("right password")
else:
    print("wrong password")
