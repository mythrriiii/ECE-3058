/**
 * @author ECE 3058 TAs
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cachesim.h"
#include "lrustack.h"

// Statistics you will need to keep track. DO NOT CHANGE THESE.
counter_t accesses = 0;   // Total number of cache accesses
counter_t hits = 0;       // Total number of cache hits
counter_t misses = 0;     // Total number of cache misses
counter_t writebacks = 0; // Total number of writebacks

/**
 * Function to perform a very basic log2. It is not a full log function,
 * but it is all that is needed for this assignment. The <math.h> log
 * function causes issues for some people, so we are providing this.
 *
 * @param x is the number you want the log of.
 * @returns Techinically, floor(log_2(x)). But for this lab, x should always be a power of 2.
 */
int simple_log_2(int x)
{
    int val = 0;
    while (x > 1)
    {
        x /= 2;
        val++;
    }
    return val;
}

//  Here are some global variables you may find useful to get you started.
//      Feel free to add/change anyting here.
cache_set_t *cache;  // Data structure for the cache
int block_size;      // Block size
int cache_size;      // Cache size
int ways;            // Ways
int num_sets;        // Number of sets
int num_offset_bits; // Number of offset bits
int num_index_bits;  // Number of index bits.

/**
 * Function to intialize your cache simulator with the given cache parameters.
 * Note that we will only input valid parameters and all the inputs will always
 * be a power of 2.
 *
 * @param _block_size is the block size in bytes
 * @param _cache_size is the cache size in bytes
 * @param _ways is the associativity
 */
void cachesim_init(int _block_size, int _cache_size, int _ways)
{
    // Set cache parameters to global variables
    block_size = _block_size;
    cache_size = _cache_size;
    ways = _ways;

    ////////////////////////////////////////////////////////////////////
    //  TODO: Write the rest of the code needed to initialize your cache
    //  simulator. Some of the things you may want to do are:
    //      - Calculate any values you need such as number of index bits.
    //      - Allocate any data structures you need.
    ////////////////////////////////////////////////////////////////////
    // we must first find the number of sets in the cache (which is the number of rows in the table)
    num_sets = cache_size / (block_size * ways);

    // now we allocate the memory for the cache (which is comprised of multiple sets) and create a pointer to it
    cache = (cache_set_t *)malloc(num_sets * sizeof(cache_set_t));

    // we will now initialize the values for each cache set within our cache:
    int i; // creates a loop counter
    for (i = 0; i < num_sets; i++)
    {                                                                            // loops through each set within our cache
        cache[i].size = ways;                                                    // sets the number of cache blocks within this set
        cache[i].stack = init_lru_stack(ways);                                   // creates the lru stack for this set
        cache[i].blocks = (cache_block_t *)malloc(ways * sizeof(cache_block_t)); // allocates memory for all cache blocks within this set
        int j;
        for (j = 0; j < ways; j++)
        { // loop through each block within the set
            // initially each block will be invalid (since they're empty) and not dirty
            cache[i].blocks[j].valid = 0;
            cache[i].blocks[j].dirty = 0;
            cache[i].blocks[j].tag = 0;
        }
    }
    ////////////////////////////////////////////////////////////////////
    //  End of your code
    ////////////////////////////////////////////////////////////////////
}

/**
 * Function to perform a SINGLE memory access to your cache. In this function,
 * you will need to update the required statistics (accesses, hits, misses, writebacks)
 * and update your cache data structure with any changes necessary.
 *
 * @param physical_addr is the address to use for the memory access.
 * @param access_type is the type of access - 0 (data read), 1 (data write) or
 *      2 (instruction read). We have provided macros (MEMREAD, MEMWRITE, IFETCH)
 *      to reflect these values in cachesim.h so you can make your code more readable.
 */
void cachesim_access(addr_t physical_addr, int access_type)
{
    ////////////////////////////////////////////////////////////////////
    //  TODO: Write the code needed to perform a cache access on your
    //  cache simulator. Some things to remember:
    //      - When it is a cache hit, you SHOULD NOT bring another cache
    //        block in.
    //      - When it is a cache miss, you should bring a new cache block
    //        in. If the set is full, evict the LRU block.
    //      - Remember to update all the necessary statistics as necessary
    //      - Remember to correctly update your valid and dirty bits.
    ////////////////////////////////////////////////////////////////////
    accesses++; // increment the number of accesses

    // calculate the tag and index given our inputs:
    int index = (physical_addr / block_size) % num_sets;
    int tag = physical_addr / (block_size * num_sets);

    int i; // instantiates loop counter

    for (i = 0; i < ways; i++)
    {
        if (cache[index].blocks[i].valid && cache[index].blocks[i].tag == tag)
        {
            // if the cache is hit, we update hits
            hits++;

            if (access_type == MEMWRITE)
            {
                cache[index].blocks[i].dirty = 1; // update the dirty bit if needed
            }

            // update the LRU stack to make this block the new MRU
            lru_stack_set_mru(cache[index].stack, i);

            return;
        }
    }

    // if we reach this point, then there did not hit and a miss must have occured
    misses++;

    int j; // instantiates a new loop counter
    for (j = 0; j < ways; j++)
    {
        // in this loop we want to check if there is a non-valid block

        if (!cache[index].blocks[j].valid)
        {
            cache[index].blocks[j].valid = 1; // because we missed, this data becomes valid now
            cache[index].blocks[j].tag = tag; // we update the tag
            if (access_type == MEMWRITE)
            {
                cache[index].blocks[j].dirty = 1; // this block becomes to dirty if we're writing to it
            }

            lru_stack_set_mru(cache[index].stack, i); // now we update the mru

            return;
        }
    }

    // if we reach this point, we have missed, but every block is valid
    // we need to remove and replace the lru
    int lru_index = lru_stack_get_lru(cache[index].stack); // this is the index of the lru

    cache[index].blocks[lru_index].tag = tag; // we now update the tag

    if (cache[index].blocks[lru_index].valid && cache[index].blocks[lru_index].dirty)
    {
        writebacks++; // if this block is valid and dirty, then a writeback must have occured
    }

    cache[index].blocks[lru_index].valid = 1; // set block to valid since it will now have data

    if (access_type == MEMWRITE)
    {
        cache[index].blocks[lru_index].dirty = 1; // we set the dirty bit based on read or write
    }
    else
    {
        cache[index].blocks[lru_index].dirty = 0;
    }

    lru_stack_set_mru(cache[index].stack, lru_index); // finally, we update the mru
    ////////////////////////////////////////////////////////////////////
    //  End of your code
    ////////////////////////////////////////////////////////////////////
}

/**
 * Function to free up any dynamically allocated memory you allocated
 */
void cachesim_cleanup()
{
    ////////////////////////////////////////////////////////////////////
    //  TODO: Write the code to do any heap allocation cleanup
    ////////////////////////////////////////////////////////////////////
    int i; // instantiate our loop counter
    for (i = 0; i < num_sets; i++)
    {
        // free memory within each cache block and lru stack
        free(cache[i].blocks);
        lru_stack_cleanup(cache[i].stack);
    }

    free(cache); // free memory within the entire cache

    // reset global variables to initial values:
    // accesses = 0;
    // hits = 0;
    // misses = 0;
    // writebacks = 0;
    ////////////////////////////////////////////////////////////////////
    //  End of your code
    ////////////////////////////////////////////////////////////////////
}

/**
 * Function to print cache statistics
 * DO NOT update what this prints.
 */
void cachesim_print_stats()
{
    printf("%llu, %llu, %llu, %llu\n", accesses, hits, misses, writebacks);
}

/**
 * Function to open the trace file
 * You do not need to update this function.
 */
FILE *open_trace(const char *filename)
{
    return fopen(filename, "r");
}

/**
 * Read in next line of the trace
 *
 * @param trace is the file handler for the trace
 * @return 0 when error or EOF and 1 otherwise.
 */
int next_line(FILE *trace)
{
    if (feof(trace) || ferror(trace))
        return 0;
    else
    {
        int t;
        unsigned long long address, instr;
        fscanf(trace, "%d %llx %llx\n", &t, &address, &instr);
        cachesim_access(address, t);
    }
    return 1;
}

/**
 * Main function. See error message for usage.
 *
 * @param argc number of arguments
 * @param argv Argument values
 * @returns 0 on success.
 */
int main(int argc, char **argv)
{
    FILE *input;

    if (argc != 5)
    {
        fprintf(stderr, "Usage:\n  %s <trace> <block size(bytes)>"
                        " <cache size(bytes)> <ways>\n",
                argv[0]);
        return 1;
    }

    input = open_trace(argv[1]);
    cachesim_init(atol(argv[2]), atol(argv[3]), atol(argv[4]));
    while (next_line(input))
        ;
    cachesim_print_stats();
    cachesim_cleanup();
    fclose(input);
    return 0;
}
