#ifndef PROCESS_H
#define PROCESS_H

#include <stdbool.h>

typedef struct Process{
    pid_t pid; 
    int CPUBurst;  //0-30
    int IOBurst;
    int IOTerm;
    int IOCheck;
    int arrvTime;   //
    int priority;   //0-9
    int remaining_time;
    int waitQueInout[2];//////////////////
    int preemptiveInout[2];
    int start_time;
    int end_time;
    int waiting_time;
    int turnaround_time;
}Process;


#endif