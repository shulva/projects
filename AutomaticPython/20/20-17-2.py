import pyautogui
import pyperclip

code = pyautogui.getWindowsWithTitle('Visual Studio Code')
code[0].activate()
print(code[0])

pyautogui.hotkey('ctrl', 'a')
pyautogui.hotkey('ctrl', 'c')
str_code = pyperclip.paste()
print(str_code)
