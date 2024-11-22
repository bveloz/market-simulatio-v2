#include "mytime.h"
#include <stdio.h>
#include <stdlib.h>

// Will be used to keep track of the passage of time within the simulation
struct Time
{
    int* hours;
    int* minutes;
    int* seconds;
};

// Initialize empty time
Time* initialize_default_time()
{
    Time* newTime = (Time*)malloc(sizeof(Time));
    newTime->hours = (int*)malloc(sizeof(int));
    newTime->minutes = (int*)malloc(sizeof(int));
    newTime->seconds = (int*)malloc(sizeof(int));
    *newTime->hours = 0;
    *newTime->minutes = 0;
    *newTime->seconds = 0;
    return newTime;
}

Time* initialize_time(int h, int m, int s)
{
    Time* newTime = initialize_default_time();
    *newTime->hours = h;
    *newTime->minutes = m;
    *newTime->seconds = s;
    return newTime;
}

void increment_time(Time* time, int amount, enum time_type type)
{
    int* value;
    int maxValue;
    //identify type and copy the pointer value of corresponding type
    //identify max of time type
    if (type == HOUR)
    {
        value = time->hours;
        maxValue = MAX_HOURS;
    }
    if (type == MINUTE)
    {
        value = time->minutes;
        maxValue = MAX_MINUTES;
    }
    if (type == SECOND)
    {
        value = time->seconds;
        maxValue = MAX_SECONDS;
    }
    //if the input is larger than maxvalue, subtract 
    if (amount >= maxValue)
    {
        printf("Time increase is too large\n");
        return;
    }
    //set the pointer to the new value
    *value += amount;
    if (*value >= maxValue)
    {
        *value -= maxValue;
    }
}

void print_time(Time* t)
{
    printf("%02d:%02d:%02d\n", *t->hours, *t->minutes, *t->seconds);
}

void free_time(Time* victim)
{
    //avoid double frees
    if (victim != NULL)
    {
        if (victim->hours != NULL)
        {
            free(victim->hours);
            victim->hours = NULL;
        }
        if (victim->minutes != NULL)
        {
            free(victim->minutes);
            victim->minutes = NULL;
        }
        if (victim->seconds != NULL)
        {
            free(victim->seconds);
            victim->seconds = NULL;
        }
    }    
}