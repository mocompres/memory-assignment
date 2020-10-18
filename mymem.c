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
	if (head != NULL)
	{
		struct memoryList *trav;
		for (trav = head; trav->next != NULL; trav=trav->next) {
			free(trav->last);
		}
		free(trav);
	}
	
		
	printf("Setup memory\n");
	myMemory = malloc(sz);
	
	/* TODO: Initialize memory management structure. */
	head = (struct memoryList*) malloc(sizeof(struct memoryList));
	head->last = NULL; // ptr to pre
	head->next = NULL; // ptr to next
	head->size = sz; // size of block
	head->alloc = 0; // not allocated
	head->ptr = myMemory;
	
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

	  			while (trav->alloc == 1 && trav->size < requested)
				{
					if (trav->next == NULL)
						trav = head;
					else
						trav = trav->next;
				}  			
					
				//struct memoryList *rtnNode = next; 

				if (trav->size > requested) {
				// make new node
					struct memoryList *temp = (struct memoryList*) malloc(sizeof(struct memoryList));
					temp->last = trav;
					temp->next = trav->next;
					temp->size = trav->size - requested;
					temp->alloc = 0;
					temp->ptr = trav->ptr + requested;
					// setup next node with pre trav->next->last = temp
					
					// setting up the allocated node
					trav->next = temp;
					trav->size = requested;
				}
				
				// setting next
				if (trav->next != NULL)
					next = trav->next;
				else 
					next = head;
					
				trav->alloc = 1;
				
				//return rtnNode->ptr;
	  }
	return trav->ptr;
}


/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void* block)
{
	printf("Deallocating memory\n");

	struct memoryList *trav;
	for (trav = head; trav != NULL; trav=trav->next) { 
		if (trav->ptr == block)
		{
			//trav->alloc = 0;
			//return;
			break;
		}
	}

	trav->alloc = 0;

	// test if prev block is free
	if (trav != head && trav->last->alloc == 0) {
		// merging block
		trav->size += trav->last->size;
		trav->ptr = trav->last->ptr;
		trav->last->last->next = trav;

		struct memoryList* tempHold = trav->last->last;
		free(trav->last);
		trav->last = tempHold;
	}

	// test if next block is free
	if (trav->next != NULL && trav->next->alloc == 0)
	{
		// merging block
		trav->size += trav->next->size;
		trav->next->next->last = trav;

		struct memoryList* tempHold = trav->next->next;
		free(trav->next);
		trav->next = tempHold;
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
	
	int cnt = 0;

	struct memoryList *trav;
	for (trav = head; trav != NULL; trav=trav->next) {
		if (trav->alloc == 0) {
			cnt++;
		}
	}

	return cnt;
	
}

/* Get the number of bytes allocated */
int mem_allocated()
{
	int cnt = 0;

	struct memoryList *trav;
	for (trav = head; trav != NULL; trav=trav->next) {
		if (trav->alloc == 1) 
			cnt += trav->size;
	}
	
	return cnt;
}

/* Number of non-allocated bytes */
int mem_free()
{
	int cnt = 0;

	struct memoryList *trav;
	for (trav = head; trav != NULL; trav=trav->next) {
		if (trav->alloc == 0) 
			cnt++;
	}
	
	return cnt;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free()
{
	int size = 0;
	int largestSize = 0;

	struct memoryList *trav;
	for (trav = head; trav != NULL; trav = trav->next) {
		if (trav->alloc == 0) {
			size += trav->size;
		
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
	int cnt = 0;

	struct memoryList *trav;
	for (trav = head; trav != NULL; trav = trav->next) {
		
		if (trav->alloc == 0 && trav->size < size)
		{
			cnt++;
		}
	}

	return cnt;
}       

char mem_is_alloc(void *ptr)
{
	struct memoryList *trav;
	for (trav = head; trav != NULL; trav=trav->next) { 
		if (trav->ptr == ptr)
		{
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
