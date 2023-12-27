#!/usr/bin/env python3
# pdf-308 book-210
# https://inventwithpython.com/ or https://automatetheboringstuff.com/
import sys
import bs4
import requests
from pathlib import Path

str_arg = str(sys.argv[1])

res = requests.get(str_arg)
res.raise_for_status()

link_html = bs4.BeautifulSoup(res.text, 'html.parser')
hrefs = link_html.select('a[href]')

for i in range(len(hrefs)):
    print(hrefs[i].get('href'))
    path_html = hrefs[i].get('href')
    try:
        res = requests.get(path_html)
        res.raise_for_status()
        path_download = Path("12-11-4")
        name = "download"+str(i)+".txt"
        File = open(path_download/name, 'wb')
        for chunk in res.iter_content(100000):
            File.write(chunk)
    except Exception as err:
        print("404 not found: "+str(path_html))
