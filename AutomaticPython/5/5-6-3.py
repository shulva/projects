#!/usr/bin/env python
"""
List to Dictionary Function for Fantasy Game Inventory
Imagine that a vanquished dragon’s loot is represented as a list of strings 
like this:
dragonLoot = ['gold coin', 'dagger', 'gold coin', 'gold coin', 'ruby']
Write a function named addToInventory(inventory, addedItems), where the 
inventory parameter is a dictionary representing the player’s inventory (like 
in the previous project) and the addedItems parameter is a list like dragonLoot. 
The addToInventory() function should return a dictionary that represents the 
updated inventory. Note that the addedItems list can contain multiples of the 
same item. 
"""
def displayInventory(inventory):
   print("Inventory:")
   item_total = 0
   for k, v in inventory.items():
       # FILL THIS PART IN
       item_total+=v
       print(str(v)+" "+k)
   print("Total number of items: " + str(item_total))

def addToInventory(inventory, addedItems):
    for i in range(len(addedItems)):
        inventory.setdefault(addedItems[i],0)
        if addedItems[i] in inventory.keys():
            inventory[addedItems[i]]+=1
    return inventory

inv = {'gold coin': 42, 'rope': 1}
dragonLoot = ['gold coin', 'dagger', 'gold coin', 'gold coin', 'ruby']
inv = addToInventory(inv, dragonLoot)
displayInventory(inv)
