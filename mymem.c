#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#include <time.h>

/* The main structure for implementing memory allocation.
 * You may change this to fit your implementation.
 */

typedef struct memoryList
{
    // doubly-linked list
    struct memoryList *last;
    struct memoryList *next;

    int size;   // How many bytes in this block?
    char alloc; // 1 if this block is allocated,
                // 0 if this block is free.
    void *ptr;  // location of block in memory pool.
} memoryList;

strategies myStrategy = NotSet; // Current strategy

size_t mySize;
void *myMemory = NULL;

static memoryList *head;
static memoryList *next;

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

    if (myMemory != NULL)
        free(myMemory); /* in case this is not the first time initmem2 is called */

    // Checking if the list has been used before
    if (head != NULL)
    {
        // Using trav as the pointer to the nodes
        memoryList *trav;

        // Searching throug the list
        for (trav = head; trav->next != NULL; trav = trav->next)
        {
            // Releasing the prev node from memory
            free(trav->last);
        }
        // Releasing the last node in the list
        free(trav);
    }

    // Seting up new memory
    myMemory = malloc(sz);

    // Setting up the first node in the list
    head = (memoryList *)malloc(sizeof(memoryList)); // Setting up space in the heap

    // Setting pointer to other node to NULL
    head->last = NULL; // Pointer to prev node
    head->next = NULL; // Pointer to next node

    // Since it is the only node it has the whole size of the menory
    head->size = sz;      // size of block
    head->alloc = 0;      // not allocated
    head->ptr = myMemory; // First node in the list.

    next = head; // next will start as head
}

// Assigning the requested memory size
void *mymalloc(size_t requested)
{
    // travpointer the mymalloc functions
    memoryList *trav;

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

        // set the trav node to the next node
        trav = next;

        // Finding a node in the list which is free and can hold the requested size
        while (trav->alloc != 0 && trav->size <= requested)
        {
            // Checking if it the end of the list, to jump to the head
            if (trav->next == NULL)
                trav = head;
            else // Setting trav as the next node
                trav = trav->next;
        }

        // Now trav points to a node which is free and has a size large enough for the requested size
        // If the node is larger than requested we make a new free node with the remaining size
        if (trav->size > requested)
        {
            // makeing new node
            // Allocating space in the heap for the new node
            memoryList *temp = (memoryList *)malloc(sizeof(memoryList));

            // checking if the new node will be the last in the list
            if (trav->next == NULL)
            {
                // Setting the new node as the last
                temp->next = NULL;
            }

            // setting the properties for the new node
            temp->last = trav;                   // setting the prev node
            temp->size = trav->size - requested; // the remaining size
            temp->alloc = 0;                     // setting the new node as free
            temp->ptr = trav->ptr + requested;   // Moving the pointer to the memory location

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

// freeing the block of memory
void myfree(void *block)
{
    // checking if a address where sent
    if (block == NULL)
    {
        return;
    }

    // setting trav to head
    struct memoryList *trav = head;

    // Seaching for the block with the memory location to free
    do
    {
        if (trav->ptr == block)
        {
            trav->alloc = 0;
            // Once the node has been found we break out of the search
            break;
        }
        // setting trav to the next node
        trav = trav->next;

    } while (trav != NULL);

    // cheing if a node where found
    if (trav == NULL)
    {
        return;
    }

    // Checking if the prev node is free
    if (trav->last != NULL)
    {
        if (trav->last->alloc == 0)
        {
            // merging node
            // holds the node to delete from memory
            memoryList *delNode = trav;
            trav = trav->last; // setting trav which is going to be updated

            // Setting up properties from the delete node
            trav->size += delNode->size; // Setting up the new size

            // setting up pointer
            if (delNode->next == NULL)
                trav->next = NULL;
            else
            {
                trav->next = delNode->next;
                trav->next->last = trav;
            }

            // updating next if needed
            if (next == delNode)
            {
                if (delNode->next == NULL)
                {
                    next = head;
                }
                else
                {
                    next = trav;
                }
            }

            free(delNode); // removing the node from memory
        }
    }

    // Checking if the prev node is free
    if (trav->next != NULL)
    {
        if (trav->next->alloc == 0)
        {
            // merging block
            memoryList *delNode = trav->next; // the node to delete

            // Updating trav node
            trav->size += delNode->size; // Setting up new size

            // updating pointers
            if (delNode->next == NULL)
            {
                trav->next = NULL;
            }
            else
            {
                trav->next = delNode->next;
                trav->next->last = trav;
            }

            // updating next if needed
            if (next == delNode)
            {
                if (delNode->next == NULL)
                {
                    next = head;
                }
                else
                {
                    next = trav;
                }
            }

            // Removing node and updating position in list
            free(delNode);
        }
    }
    return;
}

/* Get the number of contiguous areas of free space in memory. */
int mem_holes()
{

    int cnt = 0; // Holds the count

    // Searching the list from head to tail
    memoryList *trav;
    for (trav = head; trav != NULL; trav = trav->next)
    {
        // Checking if node is free
        if (trav->alloc == 0)
        {
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
    memoryList *trav;
    for (trav = head; trav != NULL; trav = trav->next)
    {
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
    memoryList *trav;
    for (trav = head; trav != NULL; trav = trav->next)
    {
        // Checking if the node is free
        if (trav->alloc == 0)
            cnt += trav->size;
    }

    return cnt;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free()
{
    int size = 0;        // Holds the counted size
    int largestSize = 0; // Hold the largest counted size

    // Searching the list from head to tail
    memoryList *trav;
    for (trav = head; trav != NULL; trav = trav->next)
    {
        if (trav->alloc == 0)
        {
            size += trav->size; // Adding the size

            // Checking if counted size larger
            if (size > largestSize)
            {
                largestSize = size;
            }
        }
        else
        {
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
    memoryList *trav;
    for (trav = head; trav != NULL; trav = trav->next)
    {
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
    memoryList *trav;
    for (trav = head; trav != NULL; trav = trav->next)
    {
        // checking if the node the given position
        if (trav->ptr == ptr)
        {
            // Returning the alloc status instead of making a if statement with the same result
            return trav->alloc;
        }
    }
    return 0;
}

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
strategies strategyFromString(char *strategy)
{
    if (!strcmp(strategy, "best"))
    {
        return Best;
    }
    else if (!strcmp(strategy, "worst"))
    {
        return Worst;
    }
    else if (!strcmp(strategy, "first"))
    {
        return First;
    }
    else if (!strcmp(strategy, "next"))
    {
        return Next;
    }
    else
    {
        return 0;
    }
}

/* Use this function to print out the current contents of memory. */
void print_memory()
{
    memoryList *trav;
    for (trav = head; trav != NULL; trav = trav->next)
    {
        // printing if memory block is in use or not
        printf("%s->", (trav->alloc == 0) ? "Free" : "Alloced");
    }
    printf("\n");
    return;
}

void print_memory_status()
{
    printf("%d out of %d bytes allocated.\n", mem_allocated(), mem_total());
    printf("%d bytes are free in %d holes; maximum allocatable block is %d bytes.\n", mem_free(), mem_holes(), mem_largest_free());
    printf("Average hole size is %f.\n\n", ((float)mem_free()) / mem_holes());
}

/* Use this function to see what happens when your malloc and free
 * implementations are called.  Run "mem -try <args>" to call this function.
 * We have given you a simple example to start.
 */
void try_mymem(int argc, char **argv)
{
    strategies strat;
    void *a, *b, *c, *d, *e;
    if (argc > 1)
        strat = strategyFromString(argv[1]);
    else
        strat = First;

    /*    // test 2
    initmem(strat, 100);

    a = mymalloc(10);
    b = mymalloc(1);
    myfree(a);
    c = mymalloc(1);

    print_memory();
    print_memory_status(); */
}
