#!/usr/bin/env python
"""
Fantasy Game Inventory
You are creating a fantasy video game. The data structure to model the 
player’s inventory will be a dictionary where the keys are string values 
describing the item in the inventory and the value is an integer value 
detailing how many of that item the player has. For example, the diction
ary value {'rope': 1, 'torch': 6, 'gold coin': 42, 'dagger': 1, 'arrow': 12} 
means the player has 1 rope, 6 torches, 42 gold coins, and so on.
Write a function named displayInventory()
Hint: You can use a for loop to loop through all the keys in a dictionary.
"""
stuff = {'rope': 1, 'torch': 6, 'gold coin': 42, 'dagger': 1, 'arrow': 12}

def displayInventory(inventory):
   print("Inventory:")
   item_total = 0
   for k, v in inventory.items():
       # FILL THIS PART IN
       item_total+=v
       print(str(v)+" "+k)
   print("Total number of items: " + str(item_total))

displayInventory(stuff)
