#ifndef QUEUE_H
#define QUEUE_H

typedef struct node
{
    Process* processPtr;
    struct node* next;
}Queue_node;

typedef struct
{
    Queue_node* front;
    Queue_node* rear;
    int count;
}Queue;


#endif