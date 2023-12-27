#!/usr/bin/env python3
# pdf-308 book-210
# use selenium and webdriver-manager
import time

from selenium import webdriver
from selenium.webdriver.common.by import By

options = webdriver.ChromeOptions()
options.add_experimental_option('detach', True)

browser = webdriver.Chrome(options=options)
browser.get('https://mail.qq.com')

time.sleep(1)

browser.switch_to.frame(browser.find_element(By.CLASS_NAME, "QQMailSdkTool_login_loginBox_qq_iframe"))
browser.switch_to.frame("ptlogin_iframe")
browser.find_element(By.ID, "switcher_plogin").click()

browser.find_element(By.ID, "u").send_keys("1329068252@qq.com")
browser.find_element(By.ID, "p").send_keys("saierhao")
browser.find_element(By.ID, "login_button").click()
#要过图像验证码，还有手机短信验证码，没法写了
#同理，12-11-2也是一样的
