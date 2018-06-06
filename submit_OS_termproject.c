#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "process_function.h"
#include "queue_function.h"
#include "heap_function.h"

#define ProcessNum 3
#define TimeBlock 4
//global variable


int timeCnt=0;
int IsRunning = 0;
int finished=0;
int preempted = 0;
int RRNum=0;
int IOcheck=0;

Process **running;
Queue* readyQueue;
Queue* waitQueue;
Process* tmp[ProcessNum];
Process** processW2R;
HEAP* readyHeap_remaining;
HEAP* readyHeap_priority;
Queue* tmpQueue;
Process** processW2R;
Process** arrived;

//Algorithm
void FCFS();
void RR();
void SJF();
void P_SJF();
void Priority();
void P_Priority();

//funcion declaration
void initVariable();
Process* FindProcess(Queue* q, int pid);
void insertionSort_forArriv(Process* tmp[], int count);
void insertionSort_forArriv_remaining(Process* tmp[], int count);
void insertionSort_forArriv_priority(Process* tmp[], int count);
int comparePriority(Process* p1, Process* p2);
int compareRemaining(Process* p1, Process* p2);
void TimeCnt_readyQueue (Queue* readyQueue, Queue* waitQueue);
void TimeCnt_readyHeap (HEAP* readyHeap, Queue* waitQueue);

void main()
{
    int i = 0;

    
    running = (Process**)malloc(sizeof(Process**)); 
    processW2R = (Process**)malloc(sizeof(Process**));
    arrived = (Process**)malloc(sizeof(Process**));
    
    readyQueue = createQueue();
    waitQueue = createQueue();
    tmpQueue = createQueue();
    readyHeap_remaining = heapCreate(10, compareRemaining);
    readyHeap_priority = heapCreate(10, comparePriority);
  

    //making process randomly
    for (i=0; i<ProcessNum; i++)
	    tmp[i] = ProcessInit(i);

    printf("\nFCFS\n");
    FCFS();
    //printf("FCFS Finished!\n\n");
    initVariable();

    printf("\nRR\n");
    RR();
    //printf("RR Finished!\n\n");
    initVariable();

    printf("\nSJF\n");
    SJF();
    //printf("SJF Finished!\n\n");
    initVariable();

    printf("\nPSJF\n");
    P_SJF();
    //printf("PSJF Finished!\n\n");
    initVariable();

    printf("\nPriority\n");
    Priority();
    //printf("Priority Finished!\n\n");
    initVariable();

    printf("\nPPriority\n");
    P_Priority();
    //printf("PPriority Finished!\n\n");
}

void initVariable()
{
    int i=0;

    timeCnt=0;
    IsRunning = 0;
    finished=0;
    preempted = 0;
    RRNum=0;
    IOcheck=0;

    for (i=0; i<ProcessNum; i++)
	{
        tmp[i]->remaining_time = tmp[i]->CPUBurst;
        tmp[i]->waitQueInout[0]=0;
        tmp[i]->waitQueInout[1]=0;
        tmp[i]->preemptiveInout[0] = 0;
        tmp[i]->preemptiveInout[1] = 0;
        tmp[i]->start_time= -1;
        tmp[i]->end_time=0;
        tmp[i]->waiting_time=0;
        tmp[i]->turnaround_time=0;
    }

}
void FCFS()
{
    int i,j=0;

    //put process into readyQueue in arrival time order
    insertionSort_forArriv(tmp, ProcessNum);
    
    for (i=0;i<ProcessNum;i++)
        enqueue(readyQueue, tmp[i]);
	
	printf("==================Gantt chart==========================\n");
  
    while (readyQueue->count!=0 || waitQueue->count!=0)
    {
        //when there processes are only in waiting queue, waiting for IO to end.
	    //ready queue is empty and no running, but in waiting. | waiting empty but ready queue is not ready   ->  timeVacantCheck          
        if (IsRunning==0 && (readyQueue->count==0 && waitQueue->count!=0))
        {         
                while(readyQueue->count == 0)
                    {   //check whether wait queue need dequeue, and plus timeCnt
                        printf("- ");
                        TimeCnt_readyQueue(readyQueue, waitQueue);	
                    }
        }
	
	
	
        //if there is no running process, dequeue. 
        if (IsRunning==0)
        {

            Process* readyFront = readyQueue->front->processPtr;

    
            while(timeCnt < readyFront -> arrvTime)
            {
                printf("- ");
                TimeCnt_readyQueue (readyQueue, waitQueue);
            }

            dequeue(readyQueue,running); 
            IsRunning=1;       
		
		    //setting start time for running process
            if (((*running)->start_time) < 0) 
			    (*running)->start_time = timeCnt;

            
            //process without IO interruption
        	if((*running)->IOTerm==0 || (*running)->IOBurst==0)
            {       
                for (i=(*running)->remaining_time; i>0 ; i--)
                {
                    printf("%d ", (*running)->pid);
                    TimeCnt_readyQueue(readyQueue, waitQueue);
                    (*running)->remaining_time--; 
                }
        
                (*running)->end_time=timeCnt;
        
                IsRunning=0;

                (*running)->turnaround_time = (*running)->end_time - (*running)->arrvTime;
                (*running)->waiting_time += (*running)->start_time - (*running)->arrvTime;	
            }

       		//process with IO interruption
        	else
       		{
                while (true)
                {
                    printf("%d ", (*running)->pid);
                    TimeCnt_readyQueue(readyQueue, waitQueue); 
                    (*running)->remaining_time--;
                   

                    if ((*running)->remaining_time == 0)
                    {
                        (*running)->end_time = timeCnt;
                        (*running)->turnaround_time = (*running)->end_time - (*running)->arrvTime;
                        (*running)->waiting_time += (*running)->start_time - (*running)->arrvTime;

                        IsRunning=0;
                        finished=1;
                        break;
                    }


                    if (((*running)->CPUBurst - (*running)->remaining_time) % (*running)-> IOTerm == 0)
                    {
                        IsRunning=0; 
                        break;
                    }
                }
                
                if (finished == 0)
                {
                    //enqueue waitQueue
                    (*running)->waitQueInout[0] = timeCnt;
                    enqueue(waitQueue, *running);        		
                }
        	}
   	            
        }
	}
	
	printf("\n========================================================\n");
	for (i=0;i<ProcessNum;i++)
		//turnaroundtime, waiting time print
		printf("PID : %d, turnaroundtime : %d, waitingtime : %d\n", tmp[i]->pid , tmp[i]->turnaround_time, tmp[i]->waiting_time);
}

void RR()
{ 

    int i;
	
    //put process into readyQueue in arrival time order
    insertionSort_forArriv(tmp, ProcessNum);
    
    for (i=0;i<ProcessNum;i++)
        enqueue(readyQueue, tmp[i]);
	
	printf("==================Gantt chart==========================\n");
  
    while (readyQueue->count!=0 || waitQueue->count != 0)
    {	
        //when there processes are only in waiting queue, waiting for IO to end.
	    if (IsRunning==0 && (readyQueue->count==0 && waitQueue->count != 0))
   	    {         
		    while(readyQueue->count == 0)
            {
                //check whether wait queue need dequeue, and plus timeCnt
                printf("- ");
                TimeCnt_readyQueue(readyQueue, waitQueue);	
            }
			
   	    }
	
	
        //if there is no running process, dequeue. 
        if (IsRunning==0)
        {
		    Process* readyFront = readyQueue->front->processPtr;

		    while(timeCnt < readyFront -> arrvTime)
            {
                printf("- ");
			    TimeCnt_readyQueue (readyQueue, waitQueue);
            }

            dequeue(readyQueue,running);
            IsRunning=1;

		    if((*running)->waitQueInout[0] != 0)
		    {
			    (*running)->waitQueInout[1]=timeCnt;    
		    	(*running)->waiting_time += (*running)->waitQueInout[1]-(*running)->waitQueInout[0];
                (*running)->waitQueInout[0]=0;    
                (*running)->waitQueInout[1]=0;     
		    }
		
		    //setting start time for running process
            if (((*running)->start_time) < 0) 
				(*running)->start_time = timeCnt;

        	while (true)
		    {
                printf("%d ", (*running)->pid);
                TimeCnt_readyQueue(readyQueue, waitQueue);
			
               	(*running)->remaining_time--;
			    (*running)->IOCheck++;
                RRNum++;

			    
		    	if ((*running)->remaining_time == 0)
			    {
                    (*running)->end_time = timeCnt;
  				    IsRunning=0;

                    (*running)->turnaround_time = (*running)->end_time - (*running)->arrvTime;
		            (*running)->waiting_time += (*running)->start_time - (*running)->arrvTime;
	
				    finished=1;
				    RRNum=0;

				    break;
			    }
			
			    if ((*running)->IOBurst!=0 && (*running)->IOTerm!=0)
                {
                    if ((*running)->IOCheck == (*running)-> IOTerm)
					{
						(*running)->IOCheck=0;
						RRNum=0;

						break;
					}
			    }

			    
			    if (RRNum==TimeBlock)
			    	goto NEWROUND;

		    }
			
		
		    //go waitqueue because of IO
		    if (finished == 0)
		    {
            	//enqueue waitQueue
            	(*running)->waitQueInout[0] = timeCnt;
            	enqueue(waitQueue, *running);
            	IsRunning=0;
		        continue;   		
		    }

            if (finished ==1)
            {
                finished=0;
                continue;
            }
            
            //go readyqueue because of RR		
            NEWROUND:	
                RRNum=0;
                (*running)->waitQueInout[0] = timeCnt;
                enqueue(readyQueue, *running);
                IsRunning=0; 

      	} 

	}
	
	printf("\n========================================================\n");

	for (i=0;i<ProcessNum;i++)
		//turnaroundtime, waiting time print
		printf("PID : %d, turnaroundtime : %d, waitingtime : %d\n", tmp[i]->pid , tmp[i]->turnaround_time, tmp[i]->waiting_time);
}


void SJF()
{

    int i,j=0;
    

    insertionSort_forArriv_remaining(tmp, ProcessNum);


    for (i=0; i<ProcessNum; i++)
         enqueue(tmpQueue, tmp[i]);
    
	printf("==================Gantt chart==========================\n");
  
    while ((readyHeap_remaining->size != 0 || waitQueue->count != 0)||tmpQueue->count !=0)
    {   
        //when there processes are only in waiting queue, waiting for IO to end.
	    //ready queue is empty and no running, but in waiting. | waiting empty but ready queue is not ready   ->  timeVacantCheck          

        if (IsRunning==0 && (readyHeap_remaining->size ==0 && waitQueue->count != 0))
   	    {     
                while(readyHeap_remaining->size == 0)
				//check whether wait queue need dequeue, and plus timeCnt
                {
                    printf("- ");
                    TimeCnt_readyHeap(readyHeap_remaining, waitQueue);	
                }
			        
   	    }
	
        //if there is no running process, dequeue. 
        if (IsRunning==0)
        {
            if(readyHeap_remaining->size == 0)
            { 
                //If first process arrived at 0
                if (tmpQueue->front->processPtr->arrvTime == 0)
                    {
                        while(tmpQueue->front->processPtr->arrvTime == 0)
                        {
                            dequeue(tmpQueue, arrived);
                            heapInsert(readyHeap_remaining, *arrived);

                            if (tmpQueue->count == 0)
                                break;
                        }
                    }
                else
                {
                    //If first process did not arrived at 0
                    while(tmpQueue->front->processPtr->arrvTime > timeCnt)   
                        {
                            printf("- ");
                            TimeCnt_readyHeap (readyHeap_remaining, waitQueue);

                            if (readyHeap_remaining->size != 0)
                                break;
                        }
                }
            }
            IsRunning=1;
            heapDelete(readyHeap_remaining, running);    


            //setting start time for running process
            if (((*running)->start_time) < 0) 
                (*running)->start_time = timeCnt;


            //process without IO interruption
            if((*running)->IOTerm==0 || (*running)->IOBurst==0)
            {
                for (i=(*running)->remaining_time; i>0 ; i--)
                {
                    printf("%d ", (*running)->pid);
                    TimeCnt_readyHeap(readyHeap_remaining, waitQueue);
                    (*running)->remaining_time--;
                }
        
                
                (*running)->end_time=timeCnt;
        
                IsRunning=0;

                (*running)->turnaround_time = (*running)->end_time - (*running)->arrvTime;
                (*running)->waiting_time += (*running)->start_time - (*running)->arrvTime;
            }
        
            
            //process with IO interruption
            else
            {
                while (true)
                {
                    printf("%d ", (*running)->pid);
                    TimeCnt_readyHeap(readyHeap_remaining, waitQueue); 
                    (*running)->remaining_time--;
                   
                    
                    if ((*running)->remaining_time == 0)
                    {
                        (*running)->end_time = timeCnt;
                        IsRunning=0;

                        (*running)->turnaround_time = (*running)->end_time - (*running)->arrvTime;
                        (*running)->waiting_time += (*running)->start_time - (*running)->arrvTime;

                        finished=1;
                        break;
                    }

                   
                    if (((*running)->CPUBurst - (*running)->remaining_time) % (*running)-> IOTerm == 0)
                        break;
                }
            
            
                if (finished == 0)
                {
                    //enqueue waitQueue
                    (*running)->waitQueInout[0] = timeCnt;
                    enqueue(waitQueue, *running);
                    IsRunning=0;   		
                }
                
            }
        }
    }
	printf("\n========================================================\n");
	for (i=0;i<ProcessNum;i++)
		//turnaroundtime, waiting time print
		printf("PID : %d, turnaroundtime : %d, waitingtime : %d\n", tmp[i]->pid , tmp[i]->turnaround_time, tmp[i]->waiting_time);
        	
}


void P_SJF()
{

    int i,j=0;

    insertionSort_forArriv_remaining(tmp, ProcessNum);


    for (i=0; i<ProcessNum; i++)
            enqueue(tmpQueue, tmp[i]);

    
	printf("==================Gantt chart==========================\n");
  
    while ((readyHeap_remaining->size != 0 || waitQueue->count != 0)||tmpQueue->count !=0)
    {
        //when there processes are only in waiting queue, waiting for IO to end.
	    //ready queue is empty and no running, but in waiting. | waiting empty but ready queue is not ready   ->  timeVacantCheck          
	    if (IsRunning==0 && (readyHeap_remaining->size ==0 && waitQueue->count != 0))
   	    {     
               while(readyHeap_remaining->size == 0)
				{//check whether wait queue need dequeue, and plus timeCnt
			        printf("- ");
                    TimeCnt_readyHeap(readyHeap_remaining, waitQueue);	
                }
   	    }
	
        //if there is no running process, dequeue. 
        if (IsRunning==0)
        {
            if(readyHeap_remaining->size == 0)
            { 
                if (tmpQueue->front->processPtr->arrvTime == 0)
                    {
                        while(tmpQueue->front->processPtr->arrvTime == 0)
                        {
                            dequeue(tmpQueue, arrived);
                            heapInsert(readyHeap_remaining, *arrived);

                            if (tmpQueue->count == 0)
                                break;
                        }
                    }
                else
                {
                    while(tmpQueue->front->processPtr->arrvTime > timeCnt)
                        {
                            printf("- ");
                            TimeCnt_readyHeap (readyHeap_remaining, waitQueue);

                            if (readyHeap_remaining->size != 0)
                                break;
                        }
                }
            }
    
            IsRunning=1;
            heapDelete(readyHeap_remaining, running);
            
            if ((*running)->preemptiveInout[0]!=0)
                {
                    (*running)->preemptiveInout[1] = timeCnt;
                    (*running)->waiting_time += (*running)->preemptiveInout[1]-(*running)->preemptiveInout[0];
                    (*running)->preemptiveInout[0] = 0;
                    (*running)->preemptiveInout[1] = 0;
                }    
           
            //setting start time for running process
            if (((*running)->start_time) < 0) 
                (*running)->start_time = timeCnt;

            //process without IO interruption
            if((*running)->IOTerm==0 || (*running)->IOBurst==0)
            {
                for (i=(*running)->remaining_time; i>0 ; i--)
                {
                    printf("%d ", (*running)->pid);
                    TimeCnt_readyHeap(readyHeap_remaining, waitQueue);
                    (*running)->remaining_time--;
                   

                    if(readyHeap_remaining->size !=0 && (((Process*)(readyHeap_remaining->heapAry[0]))->remaining_time < (*running)->remaining_time))   
                        {
                            preempted = 1;
                            finished=0;
                            break;
                        }
                }
                
                if((*running)->remaining_time == 0)
                    finished = 1;
                
                if (finished==1)
                {
                    (*running)->end_time=timeCnt;
            
                    IsRunning=0;

                    (*running)->turnaround_time = (*running)->end_time - (*running)->arrvTime;
                    (*running)->waiting_time += (*running)->start_time - (*running)->arrvTime;

                    finished=0;
                }

                if (preempted == 1)
                {   
                    
                    heapInsert(readyHeap_remaining, (*running));
                    (*running)->preemptiveInout[0] = timeCnt;
                    IsRunning=0;
                    preempted = 0;
                    finished=0;  		
                }
            }
        

            //process with IO interruption
            else
            {
                while (true)
                {
                    printf("%d ", (*running)->pid);
                    TimeCnt_readyHeap(readyHeap_remaining, waitQueue); 
                    (*running)->remaining_time--;

                     if ((*running)->remaining_time<0)
                        {
                            printf("ERROR!!\n");
                            return;
                        }

                  
                    if ((*running)->remaining_time == 0)
                    {
                        finished = 1;
                        (*running)->end_time = timeCnt;
                        IsRunning=0;
                        preempted=0;

                        (*running)->turnaround_time = (*running)->end_time - (*running)->arrvTime;
                        (*running)->waiting_time += (*running)->start_time - (*running)->arrvTime;

                        break;
                    }

                   
                    if (((*running)->CPUBurst - (*running)->remaining_time) % (*running)-> IOTerm == 0)
                        {
                             finished=0;
                             break;
                        }

                  
                    if(readyHeap_remaining->size !=0 && (((Process*)(readyHeap_remaining->heapAry[0]))->remaining_time < (*running)->remaining_time))  
                        {
                            preempted = 1;
                            break;
                        }

                }
            
              
                if (finished == 0 && preempted == 0)
                {
                    //enqueue waitQueue
                    (*running)->waitQueInout[0] = timeCnt;
                    enqueue(waitQueue, *running);
                    IsRunning=0;   		
                }
                
                if (preempted == 1)
                {
                    heapInsert(readyHeap_remaining, (*running));
                    (*running)->preemptiveInout[0] = timeCnt;
                    IsRunning=0;
                    finished = 0; 
                    preempted = 0;   
                }
            }
        }
    }
	printf("\n========================================================\n");
	for (i=0;i<ProcessNum;i++)
		//turnaroundtime, waiting time print
		printf("PID : %d, turnaroundtime : %d, waitingtime : %d\n", tmp[i]->pid , tmp[i]->turnaround_time, tmp[i]->waiting_time);
       
        	
}

void Priority()
{
    int i,j=0;


    insertionSort_forArriv_priority(tmp, ProcessNum);

    for (i=0; i<ProcessNum; i++)
        enqueue(tmpQueue, tmp[i]);

    
	printf("==================Gantt chart==========================\n");
  
    while ((readyHeap_priority->size != 0 || waitQueue->count != 0)||tmpQueue->count !=0)
    {
        //when there processes are only in waiting queue, waiting for IO to end.
	    //ready queue is empty and no running, but in waiting. | waiting empty but ready queue is not ready   ->  timeVacantCheck          
	    if (IsRunning==0 && (readyHeap_priority->size ==0 && waitQueue->count != 0))
   	    {     
               while(readyHeap_priority->size == 0){
                    printf("- ");
                    //check whether wait queue need dequeue, and plus timeCnt
			        TimeCnt_readyHeap(readyHeap_priority, waitQueue);	
               }
				
   	    }
	
        //if there is no running process, dequeue. 
        if (IsRunning==0)
        {
            if(readyHeap_priority->size == 0)
            { 
                if (tmpQueue->front->processPtr->arrvTime == 0)
                    {
                        while(tmpQueue->front->processPtr->arrvTime == 0)
                        {
                            dequeue(tmpQueue, arrived);
                            heapInsert(readyHeap_priority, *arrived);

                            if (tmpQueue->count == 0)
                                break;
                        }
                    }
                else
                {
                   
                    while(tmpQueue->front->processPtr->arrvTime > timeCnt)
                        {
                            printf("- ");

                            TimeCnt_readyHeap (readyHeap_priority, waitQueue);

                            if (readyHeap_priority->size != 0)
                                break;
                        }
                }
            }
           
            IsRunning=1;
            heapDelete(readyHeap_priority, running);    
           
            //setting start time for running process
            if (((*running)->start_time) < 0) 
                (*running)->start_time = timeCnt;

            //process without IO interruption
            if((*running)->IOTerm==0 || (*running)->IOBurst==0)
            {
                for (i=(*running)->remaining_time; i>0 ; i--)
                {
                    printf("%d ", (*running)->pid);
                    TimeCnt_readyHeap(readyHeap_priority, waitQueue);
                    (*running)->remaining_time--;
                   
                }
        
                
                (*running)->end_time=timeCnt;
        
                IsRunning=0;

                (*running)->turnaround_time = (*running)->end_time - (*running)->arrvTime;
                (*running)->waiting_time += (*running)->start_time - (*running)->arrvTime;

            }
        

            //process with IO interruption
            else
            {
                while (true)
                {
                    printf("%d ", (*running)->pid);
                    TimeCnt_readyHeap(readyHeap_priority, waitQueue); 
                    (*running)->remaining_time--;
                    
                    if ((*running)->remaining_time == 0)
                    {
                        (*running)->end_time = timeCnt;
                        IsRunning=0;

                        (*running)->turnaround_time = (*running)->end_time - (*running)->arrvTime;
                        (*running)->waiting_time += (*running)->start_time - (*running)->arrvTime;

                        finished=1;
                        break;
                    }

                   
                    if (((*running)->CPUBurst - (*running)->remaining_time) % (*running)-> IOTerm == 0)
                             break;
                }
            
            
                if (finished == 0)
                {
                    //enqueue waitQueue
                    (*running)->waitQueInout[0] = timeCnt;
                    enqueue(waitQueue, *running);
                    IsRunning=0;   		
                }
                
            }
        }
    }
	printf("\n========================================================\n");
	for (i=0;i<ProcessNum;i++)
		//turnaroundtime, waiting time print
		printf("PID : %d, turnaroundtime : %d, waitingtime : %d\n", tmp[i]->pid , tmp[i]->turnaround_time, tmp[i]->waiting_time);
        	

}


void P_Priority()
{

    int i,j=0;

    
    insertionSort_forArriv_priority(tmp, ProcessNum);


    for (i=0; i<ProcessNum; i++)
        enqueue(tmpQueue, tmp[i]);

    
	printf("==================Gantt chart==========================\n");
  
    while ((readyHeap_priority->size != 0 || waitQueue->count != 0)||tmpQueue->count !=0)
    {
        //when there processes are only in waiting queue, waiting for IO to end.
	    //ready queue is empty and no running, but in waiting. | waiting empty but ready queue is not ready   ->  timeVacantCheck          
	    if (IsRunning==0 && (readyHeap_priority->size ==0 && waitQueue->count != 0))
   	    {     
               while(readyHeap_priority->size == 0)
				{
                    //check whether wait queue need dequeue, and plus timeCnt
                    printf("- ");
			        TimeCnt_readyHeap(readyHeap_priority, waitQueue);	
                }
   	    }
	
        //if there is no running process, dequeue. 
        if (IsRunning==0)
        {
            if(readyHeap_priority->size == 0)
            { 
                if (tmpQueue->front->processPtr->arrvTime == 0)
                    {
                        while(tmpQueue->front->processPtr->arrvTime == 0)
                        {
                            dequeue(tmpQueue, arrived);
                            heapInsert(readyHeap_priority, *arrived);

                            if (tmpQueue->count == 0)
                                break;
                        }
                    }
                else
                {
                   
                    while(tmpQueue->front->processPtr->arrvTime > timeCnt)
                        {
                            printf("- ");
                            TimeCnt_readyHeap(readyHeap_priority, waitQueue);

                            if (readyHeap_priority->size != 0)
                                break;
                        }
                }
            }
    
            IsRunning=1;
            heapDelete(readyHeap_priority, running);
            
            if ((*running)->preemptiveInout[0]!=0)
                {
                    (*running)->preemptiveInout[1] = timeCnt;
                    (*running)->waiting_time += (*running)->preemptiveInout[1]-(*running)->preemptiveInout[0];
                    (*running)->preemptiveInout[0] = 0;
                    (*running)->preemptiveInout[1] = 0;
                }    
           
            //setting start time for running process
            if (((*running)->start_time) < 0) 
                (*running)->start_time = timeCnt;

            //process without IO interruption
            if((*running)->IOTerm==0 || (*running)->IOBurst==0)
            {
                for (i=(*running)->remaining_time; i>0 ; i--)
                {
                    printf("%d ", (*running)->pid);
                    TimeCnt_readyHeap(readyHeap_priority, waitQueue);
                    (*running)->remaining_time--;
                   

                    if(readyHeap_priority->size !=0 && (((Process*)(readyHeap_priority->heapAry[0]))->priority < (*running)->priority))   
                        {
                            preempted = 1;
                            finished=0;
                            break;
                        }
                }
                
                if((*running)->remaining_time == 0)
                    finished = 1;
                
                if (finished==1)
                {
                    (*running)->end_time=timeCnt;
            
                    IsRunning=0;

                    (*running)->turnaround_time = (*running)->end_time - (*running)->arrvTime;
                    (*running)->waiting_time += (*running)->start_time - (*running)->arrvTime;

                    finished=0;
                }

                if (preempted == 1)
                {   
                    
                    heapInsert(readyHeap_priority, (*running));
                    (*running)->preemptiveInout[0] = timeCnt;
                    IsRunning=0;
                    preempted = 0;
                    finished=0;  		

                }
            }
        

            //process with IO interruption
            else
            {
                while (true)
                {
                    printf("%d ", (*running)->pid);
                    TimeCnt_readyHeap(readyHeap_priority, waitQueue); 
                    (*running)->remaining_time--;

                     if ((*running)->remaining_time<0)
                        {
                            printf("ERROR!!\n");
                            return;
                        }


                   
                    if ((*running)->remaining_time == 0)
                    {
                        finished = 1;
                        (*running)->end_time = timeCnt;
                        IsRunning=0;
                        preempted=0;

                        (*running)->turnaround_time = (*running)->end_time - (*running)->arrvTime;
                        (*running)->waiting_time += (*running)->start_time - (*running)->arrvTime;


                        break;
                    }

                   
                    if (((*running)->CPUBurst - (*running)->remaining_time) % (*running)-> IOTerm == 0)
                        {
                             finished=0;
                             break;
                        }

                  
                    if(readyHeap_priority->size !=0 && (((Process*)(readyHeap_priority->heapAry[0]))->priority < (*running)->priority))  
                        {
                            preempted = 1;
                            break;
                        }

                }
            
              
                if (finished == 0 && preempted == 0)
                {
                    //enqueue waitQueue
                    (*running)->waitQueInout[0] = timeCnt;
                    enqueue(waitQueue, *running);
                    IsRunning=0;   		
                }
                
                if (preempted == 1)
                {
                    heapInsert(readyHeap_priority, (*running));
                    (*running)->preemptiveInout[0] = timeCnt;
                    IsRunning=0;
                    finished = 0; 
                    preempted = 0;   
                }
            }
        }
    }
	printf("\n========================================================\n");
	for (i=0;i<ProcessNum;i++)
		//turnaroundtime, waiting time print
		printf("PID : %d, turnaroundtime : %d, waitingtime : %d\n", tmp[i]->pid , tmp[i]->turnaround_time, tmp[i]->waiting_time);
       
        	

}


Process* FindProcess(Queue* q, int pid)
{
	Queue_node* walker = q->front;
	
	if (q->count == 0)
	{
		printf("empty queue!\n");
		return NULL;
	}

	while (true)
	{
		if (walker->processPtr->pid == pid)
			return walker->processPtr;
		else
			walker = walker->next;
	}			
}


//rr sort, FCFS
void insertionSort_forArriv(Process* tmp[], int count)
{
    Process* hold;
    int walker;
    int current;

    for (current =1; current<(ProcessNum) ; current++)
    {
        hold = tmp[current];
        for (walker = current-1; (walker>-1) && (hold->arrvTime < tmp[walker]->arrvTime); walker--)
            tmp[walker+1] = tmp[walker];

        tmp[walker+1]=hold;
    }

    return;
}

//sjf, psjf
void insertionSort_forArriv_remaining(Process* tmp[], int count)
{
    Process* hold;
    int walker;
    int current;

    for (current =1; current<ProcessNum ; current++)
    {
        hold = tmp[current];

        for (walker = current-1; (walker>-1) && (hold->arrvTime < tmp[walker]->arrvTime); walker--)
            tmp[walker+1] = tmp[walker];

        

        if (walker>-1 && hold->arrvTime == tmp[walker]->arrvTime)
        {
            if (hold->remaining_time > tmp[walker]->remaining_time)
                tmp[walker+1]=hold;
            else
                {
                    tmp[walker+1]=tmp[walker];
                    tmp[walker]=hold;
                }
        }

        else
            tmp[walker+1]=hold;
    }

    return;
}

//prpiority, ppriority

void insertionSort_forArriv_priority(Process* tmp[], int count)
{
    Process* hold;
    int walker;
    int current;

    for (current =1; current<ProcessNum ; current++)
    {
        hold = tmp[current];

        for (walker = current-1; (walker>-1) && (hold->arrvTime < tmp[walker]->arrvTime); walker--)
            tmp[walker+1] = tmp[walker];

        

        if (walker>-1 && hold->arrvTime == tmp[walker]->arrvTime)
        {
            if (hold->priority > tmp[walker]->priority)
                tmp[walker+1]=hold;
            else
                {
                    tmp[walker+1]=tmp[walker];
                    tmp[walker]=hold;
                }
        }

        else
            tmp[walker+1]=hold;
    }

    return;
}


int comparePriority(Process* p1, Process* p2)
{
    if (p1->priority> p2->priority)
        return 1;
    else
        return -1;
}


int compareRemaining(Process* p1, Process* p2)
{
    if (p1->remaining_time> p2->remaining_time)
        return 1;
    else
        return -1;
}


//RR timecount, FCFS
void TimeCnt_readyQueue (Queue* readyQueue, Queue* waitQueue)
{	
	//timeCnt
	if (waitQueue->count !=0)
	{	
		Process* frontProcess;
		frontProcess = waitQueue->front->processPtr;	
	
		//2nd process might be needed to be dequeueded
		while(timeCnt == (frontProcess->waitQueInout[0] + frontProcess->IOBurst))
		{
		    dequeue(waitQueue, processW2R);
		    (*processW2R)->waitQueInout[1] = timeCnt;
		    enqueue(readyQueue, *processW2R);
		    (*processW2R)->waiting_time += ((*processW2R)->waitQueInout[1]-(*processW2R)->waitQueInout[0]);
            (*processW2R)->waitQueInout[0] = 0;
            (*processW2R)->waitQueInout[1] = 0;
			
			if (waitQueue-> count != 0)
				frontProcess = waitQueue->front->processPtr;	
			else
				break;

			sleep(1);
		 }
	}

	timeCnt++;
}

//sjf, psjf, pppriority, priority
void TimeCnt_readyHeap (HEAP* readyHeap, Queue* waitQueue)
{	
	Process* frontProcess;
/*
    //In case arrive Time is 0
    if (tmpQueue->count != 0 && tmpQueue->front->processPtr->arrvTime == 0 && timeCnt == 0)
    {  
        while(tmpQueue->front->processPtr->arrvTime == timeCnt)
        {
            dequeue(tmpQueue, arrived);
            heapInsert(readyHeap, *arrived);

            if (tmpQueue->count ==0)
                break;
        }
    }
   
*/	
    timeCnt++;
        
    if (tmpQueue->count != 0 && tmpQueue->front->processPtr->arrvTime == timeCnt)
        {  
            while(tmpQueue->front->processPtr->arrvTime == timeCnt)
            {
                dequeue(tmpQueue, arrived);
                heapInsert(readyHeap, *arrived);
                if (tmpQueue->count ==0)
                    break;
            }
        }

    if (waitQueue->count != 0)
	{	
		frontProcess = waitQueue->front->processPtr;	
	
		//2nd process might be needed to be dequeueded
		while(timeCnt == (frontProcess->waitQueInout[0] + frontProcess->IOBurst))
		{
		    dequeue(waitQueue, processW2R);
		    (*processW2R)->waitQueInout[1] = timeCnt;
		    heapInsert(readyHeap, *processW2R);
		    (*processW2R)->waiting_time += ((*processW2R)->waitQueInout[1]-(*processW2R)->waitQueInout[0]);
	
			if (waitQueue-> count != 0)
				frontProcess = waitQueue->front->processPtr;	
			else
				break;

		}
	}

	//printf("TIME : %d\n", timeCnt);
   
}
