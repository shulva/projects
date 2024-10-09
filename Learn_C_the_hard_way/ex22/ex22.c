//
// Created by shulva on 24-9-11.
//
#include <stdio.h>
#include "ex22.h"
#include "dbg.h"

int THE_SIZE = 1000;

double *ptr_to_ratio;

static int THE_AGE = 37;

int get_age()
{
    return THE_AGE;
}

void set_age(int age)
{
    THE_AGE = age;
}


double update_ratio(double new_ratio)
{
    static double ratio = 1.0;
    ptr_to_ratio = &ratio;

    double old_ratio = ratio;
    ratio = new_ratio;

    return old_ratio;
}

void print_size()
{
    log_info("I think size is: %d", THE_SIZE);
}