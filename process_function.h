#ifndef PROCESS_FUNCTION_H
#define PROCESS_FUNCTION_H

#include "process.h"
#include <stdbool.h>

Process* ProcessCreate();
Process* ProcessInit(int pid);
void DestroyProcess(Process* destroy);
void DestroyProcessPtr(Process** destroy);

Process* ProcessCreate()  //for running
{
    Process* process;
    process = (Process*)malloc(sizeof(Process));
    

    srand(time(NULL));

    process->pid = 0;
    process->CPUBurst = 0;
    process->IOBurst = 0;
    process->IOTerm = 0;
    process->IOCheck=0;
    process->arrvTime = 0;
    process->priority = 0;
    process->remaining_time = 0;
    process->start_time = 0;
    process->waiting_time=0;
    process->turnaround_time=0;
	
    return process;
}

Process* ProcessInit(int pid)
{   
    Process* process;
    process = (Process*)malloc(sizeof(Process));
    

    srand(time(NULL));

    process->pid = pid;
    process->CPUBurst = rand()%9 +1 ;
    process->IOTerm = rand()%(process->CPUBurst);
    process->IOBurst = rand()%3;
    process->arrvTime = rand()%10;
    process->priority = rand()%10;
    process->remaining_time = process->CPUBurst;
    process->start_time = -1;
    process->waitQueInout[0] = 0;
    process->waitQueInout[1] = 0;
    process->preemptiveInout[0] = 0;
    process->preemptiveInout[1] = 0;
    process->waiting_time=0;
    process->turnaround_time=0;

   
    printf("================ProcessInitializing=======================\n");
    printf("ProcessID = %d\nCPU Burst time = %d\nIO Term = %d\nIO Burst = %d\nArrive Time = %d\nPriority = %d\n", process->pid, process->CPUBurst, process->IOTerm, process->IOBurst, process->arrvTime, process->priority);
    printf("==========================================================\n");

    sleep(1);
    return process;
}


void DestroyProcess(Process* destroy)
{	
	free(destroy);
}

void DestroyProcessPtr(Process** destroy)
{
	free(*destroy);
	free(destroy);
}


#endif
