#!/usr/bin/env python3
# pdf-308 book-210
# https://play2048.co/

import time

from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys

options = webdriver.ChromeOptions()
options.add_experimental_option('detach', True)

browser = webdriver.Chrome(options=options)
browser.get('https://play2048.co/') 

htmlElem = browser.find_element(By.TAG_NAME, 'html')

while (True):
    htmlElem.send_keys(Keys.UP)
    time.sleep(0.5)
    htmlElem.send_keys(Keys.RIGHT)
    time.sleep(0.5)
    htmlElem.send_keys(Keys.DOWN)
    time.sleep(0.5)
    htmlElem.send_keys(Keys.LEFT)
    time.sleep(0.5)
