#!/usr/bin/env python
# pdf-308 book-210
# debug this program

import random
guess = ''
coins=['heads','tails']
while guess not in ('heads', 'tails'):
   print('Guess the coin toss! Enter heads or tails:')
   guess = input()

toss = random.randint(0, 1) # 0 is tails, 1 is heads
if coins[toss]== guess:    # toss = guess 改为 coins[toss] == guess
   print('You got it!')
else:
   print('Nope! Guess again!')
#  guesss = input()
   guess=input()
   if toss == guess:
       print('You got it!')
   else:
       print('Nope. You are really bad at this game.')
