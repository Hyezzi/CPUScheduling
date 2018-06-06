#ifndef HEAP_H
#define HEAP_H

#include <stdbool.h>

typedef struct
{
   void** heapAry;
   int    last;
   int    size;
   int    (*compare) (void* argu1, void* argu2);
   int    maxSize;
} HEAP;


#endif