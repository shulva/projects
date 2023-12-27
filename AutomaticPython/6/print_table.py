#! /usr/bin/env python
# 6-11-1 pdf-196/154

tableData = [['apples', 'oranges', 'cherries', 'banana'], 
             ['Alice', 'Bob', 'Carol', 'David'], 
             ['dogs', 'cats', 'moose', 'goose']]

def print_table(str_list):
    col_width=[0]*len(str_list) # the num of list in tableData

    for i in range(len(str_list)):
        col_width[i]=len(max(str_list[i],key=len))

    for j in range(len(str_list[0])):
        for i in range(len(str_list)):
            print(str_list[i][j].rjust(col_width[i]),end=" ")
        print()

print_table(tableData)
