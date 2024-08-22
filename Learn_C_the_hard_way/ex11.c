#include <stdio.h>

int main(int argc, char *argv[])
{
    // go through each string in argv

    int i = 0;
    while(i < argc) {
        printf("arg %d: %s\n", i, argv[i]);
        i++;
    }

    // let's make our own array of strings
    char *states[] = {
        "California", "Oregon",
        "Washington", "Texas"
    };

    int num_states = 4;
    i = 0;  // watch for this
    while(i < num_states) {
        printf("state %d: %s\n", i, states[i]);
        i++;
    }
    //-----------------------------------附加题
    i = argc-1;
    while(i >= 0) {
        printf("arg %d: %s\n", i, argv[i]);
        i--;
    }

    i = num_states-1;
    while(i >= 0) {
        printf("state %d: %s\n", i, states[i]);
        i--;
    }

    if (num_states < argc)
        i = argc;
    else
        i = num_states;

    i = i-1;
    
    for (int j=0;j<i;j++)
    {
        states[j] = argv[j];
    }

    i = 0;  
    while(i < num_states) {
        printf("new state %d: %s\n", i, states[i]);
        i++;
    }
    
    printf("%p,%p",states[0],argv[0]);

    return 0;
}