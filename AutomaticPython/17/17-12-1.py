#! python3
# stopwatch.py - A simple stopwatch program.
# pdf-320
import time
import pyperclip

print('Press ENTER to begin. Afterwards, press ENTER to "click" the stopwatch. Press Ctrl-C to quit.')
input()
print('Started.')
startTime = time.time()
lastTime = startTime
lapNum = 1
# press Enter to begin
# get the first lap's start time
# TODO: Start tracking the lap times.
try:
    while True:
        input()
        lapTime = round(time.time() - lastTime, 2)
        totalTime = round(time.time() - startTime, 2)
        print('Lap #%s: %s (%s)' % (str(lapnum).ljust(2), str(totaltime).rjust(5), str(laptime).rjust(5)), end='')
        lapNum += 1
        lastTime = time.time()  # reset the last lap time
        pyperclip.copy((str(lapNum).ljust(2), str(totalTime).rjust(5), str(lapTime).rjust(5)))
except KeyboardInterrupt:
    # Handle the Ctrl-C exception to keep its error message from displaying.
    print('\nDone.')
