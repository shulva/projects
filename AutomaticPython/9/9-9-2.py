#!/usr/bin/env python
# pdf-230 book-179

from pathlib import Path
import sys

replace_word = ["ADJECTIVE","NOUN","ADVERB","VERB"]

path = Path(str(sys.argv[1]))
File = open(Path.cwd() / path, 'r')
string = File.read()
File.close()

File = open(Path.cwd() / path, 'w')

for word in replace_word:
    while string.find(word) > -1:
        print("Enter a "+word.lower())
        string = string.replace(word,input(),1)

print(string)
File.write(string)
File.close()
