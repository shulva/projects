#! /usr/bin/env python
# bullet_point_adder.py--Adds wikipedia bullet points to the start of each line of text on the clipboard
# you need intstall pyperclip module
import pyperclip

text = pyperclip.paste()

# TODO: Separate lines and add stars.
lines=text.split("\n")
for i in range(len(lines)):
    lines[i]='*'+lines[i]
text="\n".join(lines)

pyperclip.copy(text)
