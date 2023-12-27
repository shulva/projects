#!/usr/bin/env python
'''
Write a function that takes a list value as an argument and returns 
a string with all the items separated by a comma and a space, with and 
inserted before the last item. For example, passing the previous spam list to 
the function would return 'apples, bananas, tofu, and cats'. But your func
tion should be able to work with any list value passed to it. Be sure to test 
the case where an empty list [] is passed to your function.
list=['apples', 'bananas', 'tofu', 'cats']
'''

def list_to_string(list):
    string=''
    for i in range(len(list)-1):
        string=string+list[i]+', ' 

    string+='and '
    string+=list[len(list)-1]
    return string

print(list_to_string(list))


