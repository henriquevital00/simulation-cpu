#ifndef PROCESS_H_
#define PROCESS_H_

typedef struct
{
    int duration;
    int arrival;
    int *interruptions;
    int numberIO;
} Process;

int numberProcess;

#endif