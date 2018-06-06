#ifndef HEAP_FUNCTION_H
#define HEAP_FUNCTION_H

#include <math.h>
#include "heap.h"

//	Prototype Definitions 
HEAP* heapCreate (int maxSize, int (*compare) (void* arg1, void* arg2));
bool  heapInsert  (HEAP* heap, void*  dataPtr);
bool  heapDelete  (HEAP* heap, void** dataOutPtr);
int   heapCount   (HEAP* heap);
bool  heapFull    (HEAP* heap);
bool  heapEmpty   (HEAP* heap);
void  heapDestroy (HEAP* heap);

void  _reheapUp  (HEAP* heap, int childLoc);
void _reheapDown (HEAP* heap, int root);


/*
void heapSort_priority(Process* heapAry[], int last);
void reheapDown_priority(Process* heapAry[], int root, int last);


void heapSort_remaining(Process* heapAry[], int last);
void reheapDown_remaining(Process* heapAry[], int root, int last);

*/



/*	==================== heapCreate ====================
	Allocates memory for heap and returns address of
	heap head structure.
	   Pre  Nothing
	   Post heap created and address returned
	             if memory overflow, NULL returned
*/


HEAP* heapCreate (int maxSize, int  (*compare) (void* argu1, void* argu2))
{
//	Local Definitions
	HEAP* heap;

//	Statements 
	heap = (HEAP*)malloc(sizeof (HEAP));
	if (!heap)
	   return NULL;

	heap->last    = -1;
	heap->compare = compare;
	heap->size = 0;

	// Force heap size to power of 2 -1
	heap->maxSize = (int) pow (2, ceil(log2(maxSize))) - 1;
	heap->heapAry = (void*) 
	                 calloc(heap->maxSize, sizeof(void*));
	return heap;
}	// createHeap 


/*	================== heapInsert =================== 
	Inserts data into heap.
	   Pre    Heap is a valid heap structure
	          last is pointer to index for last element
	          data is data to be inserted
	   Post   data have been inserted into heap
	   Return true if successful; false if array full
*/
bool heapInsert (HEAP* heap, void* dataPtr)
{
//	Statements 
	if (heap->size == 0)                 // Heap empty
	   {
	    heap->size                = 1;
	    heap->last                = 0;
	    heap->heapAry[heap->last] = dataPtr;
	    return true;
	   } // if
	if (heap->last == heap->maxSize - 1)
	  return false;
	++(heap->last); 
	++(heap->size);
	heap->heapAry[heap->last] = dataPtr;
	_reheapUp (heap, heap->last);
	return true;
}	// heapInsert 

/*	==================== reheapUp ====================
	Reestablishes heap by moving data in child up to
	correct location heap array.
	   Pre  heap is array containing an invalid heap
	        newNode is index to new data in heap
	   Post newNode inserted into heap
*/
void  _reheapUp  (HEAP* heap, int childLoc)
{
//	Local Definitions 
	int    parent;
	void** heapAry;
	void*  hold;

//	Statements 
	// if not at root of heap -- index 0 
	if (childLoc)
	   {
	    heapAry = heap->heapAry;
	    parent = (childLoc - 1)/ 2;
	    if (heap->compare(heapAry[childLoc], heapAry[parent]) < 0)
	        // child is smaller than parent -- swap 
	        {
	         hold             = heapAry[parent]; 
	         heapAry[parent]  = heapAry[childLoc];
	         heapAry[childLoc] = hold;
	        _reheapUp (heap, parent);
	       } // if heap[] 
	   } // if newNode 
	return;
}	// reheapUp 


/*	 ================== heapDelete ===================
	Deletes root of heap and passes data back to caller.
	   Pre    heap is a valid heap structure 
	          last is reference to last node in heap 
	          dataOut is reference to output area
	   Post   last deleeted and heap rebuilt
	          deleted data passed back to user
	   Return true if successful; false if array empty
*/
bool heapDelete (HEAP* heap, void** dataOutPtr)
{
//	Statements 
	if (heap->size == 0)
	    // heap empty 
	    return false;
	*dataOutPtr = heap->heapAry[0];
	heap->heapAry[0]  = heap->heapAry[heap->last];
	(heap->last)--;
	(heap->size)--;
	_reheapDown (heap, 0);
	return true;
}	// heapDelete 


/*	=================== reheapDown =================== 
	Reestablishes heap by moving data in root down to its
	correct location in the heap.
	   Pre  heap is array of data
	        root is root of heap or subheap
	        last is an index to last element in heap
	   Post heap has been restored
*/
void _reheapDown (HEAP* heap, int root)
{
//	Local Definitions 
	void* hold;
	void* leftData;
	void* rightData;
	int   largeLoc;
	int   last;
	
//	Statements 
	last = heap->last;
	if ((root * 2 + 1) <= last)         // left subtree
	    // There is at least one child 
	   {
	    leftData   = heap->heapAry[root * 2 + 1];
	    if ((root * 2 + 2) <= last)  // right subtree 
	       rightData = heap->heapAry[root * 2 + 2];
	    else
	       rightData = NULL;
	       
	    // Determine which child is larger 
	    if ((!rightData) 
	         || heap->compare (leftData, rightData) > 0)
	       {
	        largeLoc = root * 2 + 1;
	       } // if no right key or leftKey greater
	    else
	       {
	        largeLoc = root * 2 + 2;
	       } // else 
	    // Test if root > larger subtree 
	    if (heap->compare (heap->heapAry[root], heap->heapAry[largeLoc]) > 0)
	        {
	         // parent > children 
	         hold                = heap->heapAry[root]; 
	         heap->heapAry[root] = heap->heapAry[largeLoc];
	         heap->heapAry[largeLoc] = hold;
	         _reheapDown (heap, largeLoc);
	        } // if root < 
	} // if root 
	return;
}	// reheapDown 

/*
//===============heap Sort===========================
void heapSort_priority(Process* heapAry[], int last)
{
	int sorted;
	Process* holdData;
	
	sorted=last;

	while(sorted>0)
	{
		holdData = heapAry[0];
		heapAry[0] = heapAry[sorted];
		heapAry[sorted] = holdData;
		sorted--;
		reheapDown_priority(heapAry, 0, sorted);
	}

	return;
}


void reheapDown_priority(Process* heapAry[], int root, int last)
{
	Process* hold;
	hold = (Process*)malloc(sizeof(Process*));
	Process* leftKey;
	leftKey = (Process*)malloc(sizeof(Process*));
	Process* rightKey;
	rightKey = (Process*)malloc(sizeof(Process*));
	Process* largeChildKey;
	largeChildKey = (Process*)malloc(sizeof(Process*));
	int largeChildIndex;


	if ( (root*2+1) <= last )
	//There is at least one chld
	{
		leftKey = heapAry[root*2+1];

		if((root*2+2) <= last)
			rightKey = heapAry[root*2+2];
		else
			rightKey->priority = -1;

		//Determine which child is larger
		if(leftKey->priority > rightKey->priority)
		{
			largeChildKey = leftKey;
			largeChildIndex = root*2+1;
		}
		else
		{
			largeChildKey = rightKey;
			largeChildIndex = root*2+2;
		}

		//Test if root>larger subtree
		if(heapAry[root]->priority < heapAry[largeChildIndex]->priority)
		{
			//parent<children
			hold = heapAry[root];
			heapAry[root] = heapAry[largeChildIndex];
			heapAry[largeChildIndex]= hold;
			reheapDown_priority(heapAry, largeChildIndex, last);
		}
	}
}


void heapSort_remaining(Process* heapAry[], int last)
{
	int sorted;
	Process* holdData;
	//int walker;

	//Create Heap
	//for(walker =1; walker <= last; walker++)
	//	reheapUp(heapAry, walker);
	
	//Heap created. Now sort it.
	
	sorted=last;

	while(sorted>0)
	{
		holdData = heapAry[0];
		heapAry[0] = heapAry[sorted];
		heapAry[sorted] = holdData;
		sorted--;
		reheapDown_remaining(heapAry, 0, sorted);
	}

	return;
}

void reheapDown_remaining(Process* heapAry[], int root, int last)
{
	Process* hold;
	hold = (Process*)malloc(sizeof(Process*));
	Process* leftKey;
	Process* rightKey;
	Process* largeChildKey;
	int largeChildIndex;


	if ( (root*2+1) <= last )
	//There is at least one chld
	{
		leftKey = heapAry[root*2+1];

		if((root*2+2) <= last)
			rightKey = heapAry[root*2+2];
		else
			rightKey->remaining_time = -1;

		//Determine which child is larger
		if(leftKey->remaining_time> rightKey->remaining_time)
		{
			largeChildKey = leftKey;
			largeChildIndex = root*2+1;
		}
		else
		{
			largeChildKey = rightKey;
			largeChildIndex = root*2+2;
		}

		//Test if root>larger subtree
		if(heapAry[root]->remaining_time < heapAry[largeChildIndex]->remaining_time)
		{
			//parent<children
			hold = heapAry[root];
			heapAry[root] = heapAry[largeChildIndex];
			heapAry[largeChildIndex]= hold;
			reheapDown_remaining(heapAry, largeChildIndex, last);
		}
	}
}

//=================================================

*/
#endif