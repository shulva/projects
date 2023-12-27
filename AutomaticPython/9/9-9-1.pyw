#!/usr/bin/env python
# pdf-230 book-179
# mcb.pyw - Saves and loads pieces of text to the clipboard.
# Usage: py.exe mcb.pyw save <keyword> - Saves clipboard to keyword.
#        py.exe mcb.pyw <keyword> - Loads keyword to clipboard.
#        py.exe mcb.pyw list - Loads all keywords to clipboard.
#        py.exe mcb.pyw delete <keyword> - delete keyword to clipboard.
#        py.exe mcb.pyw delete - delete all keywords to clipboard.

import shelve, pyperclip, sys

mcbShelf = shelve.open('mcb')

if len(sys.argv) == 3 and sys.argv[1].lower() == 'save':
    # Save clipboard content.
    mcbShelf[sys.argv[2]] = pyperclip.paste()

elif len(sys.argv) == 3 and sys.argv[1].lower() == 'delete':
    # delete clipboard content
    mcbShelf.pop(sys.argv[2],None)

elif len(sys.argv) == 2:

    # List keywords and load content.
    if sys.argv[1].lower() == 'list':
        pyperclip.copy(str(list(mcbShelf.keys())))
        for i in range(len(mcbShelf)):
            print(list(mcbShelf.keys())[i]+':'+list(mcbShelf.values())[i])

    elif sys.argv[1].lower() == 'delete':
        for i in range(len(mcbShelf)):
            mcbShelf.pop(list(mcbShelf.keys())[i],None)

    elif sys.argv[1] in mcbShelf:
        pyperclip.copy(mcbShelf[sys.argv[1]])

mcbShelf.close()
