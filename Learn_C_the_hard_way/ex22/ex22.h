//
// Created by shulva on 24-9-11.
//

#ifndef _ex22_h
#define _ex22_h

// makes THE_SIZE in ex22.c available to other .c files
int THE_SIZE;
double *ptr_to_ratio;

// gets and sets an internal static variable in ex22.c
int get_age();
void set_age(int age);

// updates a static variable that's inside update_ratio
double update_ratio(double ratio);

void print_size();

#endif