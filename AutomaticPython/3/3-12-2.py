#!/usr/bin/env python
import sys
"""
Add try and except statements to the previous project (3-12-1)to detect whether the 
user types in a noninteger string. Normally, the int() function will raise a 
ValueError error if it is passed a noninteger string, as in int('puppy'). In the 
except clause, print a message to the user saying they must enter an integer.
"""

def collatz(number):
    count=0
    if number%2==0:
        count=int(number/2)
    else:
        count=3*number+1
    return count    

try:
    number=int(input())
except ValueError:
    print("please input integer")
    sys.exit()

while True:
    number=collatz(number) 
    print(number)
    if number==1:
        break

