#ifndef MY_TIME_H
#define MY_TIME_H

#define MAX_HOURS 24
#define MAX_MINUTES 60
#define MAX_SECONDS 60

typedef struct Time Time;

enum time_type {HOUR, MINUTE, SECOND};

Time* initialize_default_time();
Time* initialize_time(int, int, int);
void increment_time(Time*, int, enum time_type);
void print_time(Time*);
void free_time(Time*);

#endif
