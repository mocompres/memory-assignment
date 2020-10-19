#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#include <time.h>


/* The main structure for implementing memory allocation.
 * You may change this to fit your implementation.
 */

struct memoryList
{
  // doubly-linked list
  struct memoryList *last;
  struct memoryList *next;

  int size;            // How many bytes in this block?
  char alloc;          // 1 if this block is allocated,
                       // 0 if this block is free.
  void *ptr;           // location of block in memory pool.
};

strategies myStrategy = NotSet;    // Current strategy


size_t mySize;
void *myMemory = NULL;

static struct memoryList *head;
static struct memoryList *next;


/* initmem must be called prior to mymalloc and myfree.

   initmem may be called more than once in a given exeuction;
   when this occurs, all memory you previously malloc'ed  *must* be freed,
   including any existing bookkeeping data.

   strategy must be one of the following:
		- "best" (best-fit)
		- "worst" (worst-fit)
		- "first" (first-fit)
		- "next" (next-fit)
   sz specifies the number of bytes that will be available, in total, for all mymalloc requests.
*/

void initmem(strategies strategy, size_t sz)
{
	myStrategy = strategy;

	/* all implementations will need an actual block of memory to use */
	mySize = sz;
	printf("Freeing memory\n");
	if (myMemory != NULL) free(myMemory); /* in case this is not the first time initmem2 is called */
	
	/* TODO: release any other memory you were using fo,,r bookkeeping when doing a re-initialization! */
	// Checking the list has been used before
	if (head != NULL)
	{
		// Using trav as the pointer to the nodes 
		struct memoryList *trav;
		for (trav = head; trav->next != NULL; trav=trav->next) {
			// Releasing the prev node from memory
			free(trav->last);
		}
		// Releasing the last node in the list
		free(trav);
	}
	
		
	printf("Setup memory\n");
	myMemory = malloc(sz);
	
	/* TODO: Initialize memory management structure. */
	// Setting up the first node in the list
	head = (struct memoryList*) malloc(sizeof(struct memoryList)); // Setting up space in the heap
	// Setting pointer to other node to NULL
	head->last = NULL; // Pointer to prev node
	head->next = NULL; // Pointer to next node

	// Since it is the only node it has the whole size of the menory
	head->size = sz; // size of block
	head->alloc = 0; // not allocated
	head->ptr = myMemory; // First node in the list.
	
	// Checking if the user choose the correct strategy
	if ( strategy  != next )
		printf("Can only do next-fit strategy... Setting up next\n");

	next = head; // next will start as head 
}

/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1 
 */

void *mymalloc(size_t requested)
{
	struct memoryList *trav;

	assert((int)myStrategy > 0);
	
	switch (myStrategy)
	  {
	  case NotSet: 
	            return NULL;
	  case First:
	            return NULL;
	  case Best:
	            return NULL;
	  case Worst:
	            return NULL;
	  case Next:
				printf("Allocating memory\n");
	  			// find the node
				trav = next; 

				// Finding a node in the list which is free and can hold the requested size
	  			while (trav->alloc != 0 && trav->size < requested)
				{
					// Checking if it the end of the list, to stip to the head
					if (trav->next == NULL)
						trav = head;
					else // Setting trav as the next node
						trav = trav->next;
				}  			

				// Now trav points to a node which is free and has a size large enough for the requested size
				// If the node is larger than requested we make a new free node with the remaining size
				if (trav->size > requested) {
					// makeing new node
					// Allocating space in the heap for the new node
					struct memoryList *temp = (struct memoryList*) malloc(sizeof(struct memoryList));

					// Setting up the node to be placed after trav
					temp->last = trav; 
					temp->next = trav->next;
					temp->size = trav->size - requested; // the remaining size
					temp->alloc = 0;
					temp->ptr = trav->ptr + requested; // Moving the pointer to the memory location
					// setup next node with pre trav->next->last = temp
					
					// setting up and updating the allocated node
					trav->next = temp;
					trav->size = requested;
				}

				// Marking trav as allocated
				trav->alloc = 1;
				
				// Updating next ptr to next mymalloc
				if (trav->next != NULL)
					next = trav->next;
				else 
					next = head;
	  }

	// Returning the assigned memory location for the requested size
	return trav->ptr;
}


/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void* block)
{
	printf("Deallocating memory\n");

	
	struct memoryList *trav;

	// Seaching for the block with the memory location to free
	for (trav = head; trav != NULL; trav=trav->next) { 
		if (trav->ptr == block)
		{
			// Once the node has been found we break out of the search
			break;
		}
	}
	// trav is now the node to be freed
	trav->alloc = 0;

	// test if prev node is free to merge
	if (trav != head && trav->last->alloc == 0) {
		// merging node

		// Setting up pointers from the prev node to be the one before
		trav->size += trav->last->size; // Setting up the new size
		trav->ptr = trav->last->ptr; // setting up the pointer to the prev
		trav->last->last->next = trav; // setting up the pointer

		// Removing the prev pointer and updating possition in the list
		struct memoryList* tempHold = trav->last->last; // Hold the pointer to the new prev pointer
		free(trav->last); // removing the node from memory
		trav->last = tempHold; // setting up the new prev
	}

	// test if next block is free to merge
	if (trav->next != NULL && trav->next->alloc == 0)
	{
		// merging block

		// Updateting trav node
		trav->size += trav->next->size; // Setting up new size
		trav->next->next->last = trav; // Setting new pointer to the next node

		// Removing node and updating position in list
		struct memoryList* tempHold = trav->next->next; // Temp hold of pointer to next node
		free(trav->next); // Removing node from memory
		trav->next = tempHold; // Setting up the new next 
	}
	
	return;
}

/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when refered to "memory" here, it is meant that the 
 * memory pool this module manages via initmem/mymalloc/myfree. 
 */

/* Get the number of contiguous areas of free space in memory. */
int mem_holes()
{
	
	int cnt = 0; // Holds the count

	// Searching the list from head to tail
	struct memoryList *trav;
	for (trav = head; trav != NULL; trav=trav->next) {
		// Checking if node is free
		if (trav->alloc == 0) {
			cnt++;
		}
	}

	// Returning the count
	return cnt;
	
}

/* Get the number of bytes allocated */
int mem_allocated()
{
	int cnt = 0; // Holds the total count

	// Searching the list from head to tail
	struct memoryList *trav;
	for (trav = head; trav != NULL; trav=trav->next) {
		// Checking if node is allocated
		if (trav->alloc == 1) 
			cnt += trav->size; // Adding size
	}
	
	return cnt;
}

/* Number of non-allocated bytes */
int mem_free()
{
	int cnt = 0; // Holds the count

	// Searching the list from head to tail
	struct memoryList *trav;
	for (trav = head; trav != NULL; trav=trav->next) {
		// Checking if the node is free
		if (trav->alloc == 0) 
			cnt += trav->size;
	}
	
	return cnt;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free()
{
	int size = 0; // Holds the counted size
	int largestSize = 0; // Hold the largest counted size

	// Searching the list from head to tail
	struct memoryList *trav;
	for (trav = head; trav != NULL; trav = trav->next) {
		if (trav->alloc == 0) {
			size += trav->size; // Adding the size

			// Checking if counted size larger
			if (size > largestSize)
			{
				largestSize = size;
			}
		}
		else {
			size = 0;
		}
			
	}
	
	return largestSize;
}

/* Number of free blocks smaller than "size" bytes. */
int mem_small_free(int size)
{
	int cnt = 0; // Holds the count

	// Seaching the list from head to tail
	struct memoryList *trav;
	for (trav = head; trav != NULL; trav = trav->next) {
		// Checking if the node is free and small than the specified size
		if (trav->alloc == 0 && trav->size < size)
		{
			cnt++;
		}
	}

	return cnt;
}       

char mem_is_alloc(void *ptr)
{
	// Seaching the list from head to tail
	struct memoryList *trav;
	for (trav = head; trav != NULL; trav=trav->next) { 
		// checking if the node the given position
		if (trav->ptr == ptr)
		{
			// Returning the alloc status instead of making a if statement with the same result
			return trav->alloc;
		}
	}
        return 0;
}

/* 
 * Feel free to use these functions, but do not modify them.  
 * The test code uses them, but you may find them useful.
 */


//Returns a pointer to the memory pool.
void *mem_pool()
{
	return myMemory;
}

// Returns the total number of bytes in the memory pool. */
int mem_total()
{
	return mySize;
}


// Get string name for a strategy. 
char *strategy_name(strategies strategy)
{
	switch (strategy)
	{
		case Best:
			return "best";
		case Worst:
			return "worst";
		case First:
			return "first";
		case Next:
			return "next";
		default:
			return "unknown";
	}
}

// Get strategy from name.
strategies strategyFromString(char * strategy)
{
	if (!strcmp(strategy,"best"))
	{
		return Best;
	}
	else if (!strcmp(strategy,"worst"))
	{
		return Worst;
	}
	else if (!strcmp(strategy,"first"))
	{
		return First;
	}
	else if (!strcmp(strategy,"next"))
	{
		return Next;
	}
	else
	{
		return 0;
	}
}


/* 
 * These functions are for you to modify however you see fit.  These will not
 * be used in tests, but you may find them useful for debugging.
 */

/* Use this function to print out the current contents of memory. */
void print_memory()
{
	struct memoryList *trav;
	for (trav = head; trav != NULL; trav=trav->next) { 
		printf("%s->", (trav->alloc == 0) ? "Free" : "Alloced");
	}
	printf("\n");
	return;
}

/* Use this function to track memory allocation performance.  
 * This function does not depend on your implementation, 
 * but on the functions you wrote above.
 */ 
void print_memory_status()
{
	printf("%d out of %d bytes allocated.\n",mem_allocated(),mem_total());
	printf("%d bytes are free in %d holes; maximum allocatable block is %d bytes.\n",mem_free(),mem_holes(),mem_largest_free());
	printf("Average hole size is %f.\n\n",((float)mem_free())/mem_holes());
}

/* Use this function to see what happens when your malloc and free
 * implementations are called.  Run "mem -try <args>" to call this function.
 * We have given you a simple example to start.
 */
void try_mymem(int argc, char **argv) {
        strategies strat;
	void *a, *b, *c, *d, *e;
	if(argc > 1)
	  strat = strategyFromString(argv[1]);
	else
	  strat = First;
	
	
	/* A simple example.  
	   Each algorithm should produce a different layout. */
	/* test 1
	initmem(strat,500);
	
	a = mymalloc(100);
	b = mymalloc(100);
	c = mymalloc(100);
	myfree(b);
	d = mymalloc(50);
	myfree(a);
	e = mymalloc(25);
	
	print_memory();
	print_memory_status();
	*/

	// test 2
		initmem(strat,100);

		a = mymalloc(10);
		b = mymalloc(1);
		myfree(a);
		c = mymalloc(1);


	print_memory();
	print_memory_status();
}
