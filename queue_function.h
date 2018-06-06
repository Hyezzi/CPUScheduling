#ifndef QUEUE_FUNCTION_H
#define QUEUE_FUNCTION_H

#include <stdbool.h>
#include "queue.h"

Queue* createQueue ();
bool enqueue (Queue* queue, Process* processPtr);
bool dequeue (Queue* queue, Process** ProcessPtr);

Queue* createQueue ()
{
    //Local definitions
    Queue* queue;

    queue = (Queue*)malloc(sizeof(Queue));

    if (queue)
    {
        queue->front = NULL;
        queue->rear = NULL;
        queue->count = 0;
    }

    return queue;

}

bool enqueue (Queue* queue, Process* processPtr)
{
    Queue_node* newPtr;

    if (!(newPtr = (Queue_node*)malloc(sizeof(Queue_node))))
        return false;

    newPtr->processPtr = processPtr;
    newPtr->next = NULL;

    
    if (queue->count==0)
        queue->front = newPtr;
    else
        queue->rear->next = newPtr;

    queue->rear = newPtr;
    
    (queue->count)++;   
	

    return true;
}


bool dequeue (Queue* queue, Process** ProcessPtr)
{
    Queue_node* deleteLoc;

    if (!queue->count)
        return false;

    *ProcessPtr = queue->front->processPtr;

    deleteLoc = queue->front;

    if (queue->count ==1)
        queue->rear = queue->front =NULL;
    else
        queue->front = queue->front->next;
    (queue->count)--;

    free(deleteLoc);

    return true;
}

void DestroyQueue(Queue* destroy)
{
	free(destroy);
}

#endif