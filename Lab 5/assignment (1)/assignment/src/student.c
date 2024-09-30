/*
 * student.c
 * Multithreaded OS Simulation for CS 2200 and ECE 3058
 *
 * This file contains the CPU scheduler for the simulation.
 */

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "os-sim.h"

/** Newly added functions **/
static void queue(pcb_t* new_process);
static pcb_t* dequeue();


/** Function prototypes **/
extern void idle(unsigned int cpu_id);
extern void preempt(unsigned int cpu_id);
extern void yield(unsigned int cpu_id);
extern void terminate(unsigned int cpu_id);
extern void wake_up(pcb_t *process);



/*
 * current[] is an array of pointers to the currently running processes.
 * There is one array element corresponding to each CPU in the simulation.
 *
 * current[] should be updated by schedule() each time a process is scheduled
 * on a CPU.  Since the current[] array is accessed by multiple threads, you
 * will need to use a mutex to protect it.  current_mutex has been provided
 * for your use.
 */
static pcb_t **current;
static pthread_mutex_t current_mutex;

//static int cpu_count; //total number of cpus in use
static pthread_mutex_t free_mutex; // mutex that is free and ready to use
static char alg_type; //type of algorithm being used
static unsigned int cpu_count;              // the number of cpus in use

static pcb_t* head;  //head of the linked list
static pthread_cond_t not_idle; // not idle condition
static int time_slice; 

/*
 * schedule() is your CPU scheduler.  It should perform the following tasks:
 *
 *   1. Select and remove a runnable process from your ready queue which 
 *	you will have to implement with a linked list or something of the sort.
 *
 *   2. Set the process state to RUNNING
 *
 *   3. Call context_switch(), to tell the simulator which process to execute
 *      next on the CPU.  If no process is runnable, call context_switch()
 *      with a pointer to NULL to select the idle process.
 *	The current array (see above) is how you access the currently running process indexed by the cpu id. 
 *	See above for full description.
 *	context_switch() is prototyped in os-sim.h. Look there for more information 
 *	about it and its parameters.
 */
static void schedule(unsigned int cpu_id)
{
    /* FIX ME */

    pcb_t* curr_process = dequeue(); //get a process to execute

    if (curr_process) {
        curr_process->state = PROCESS_RUNNING;
    }

    pthread_mutex_lock(&current_mutex); 
    current[cpu_id] = curr_process;
    pthread_mutex_unlock(&current_mutex);
    context_switch(cpu_id, curr_process, time_slice); 

}


/*
 * idle() is your idle process.  It is called by the simulator when the idle
 * process is scheduled.
 *
 * This function should block until a process is added to your ready queue.
 * It should then call schedule() to select the process to run on the CPU.
 */
extern void idle(unsigned int cpu_id)
{
    /* FIX ME */
    pthread_mutex_lock(&free_mutex);

    while (!head) {
        pthread_cond_wait(&not_idle, &free_mutex);
    }

    pthread_mutex_unlock(&free_mutex);
    schedule(cpu_id);  

    /*
     * REMOVE THE LINE BELOW AFTER IMPLEMENTING IDLE()
     *
     * idle() must block when the ready queue is empty, or else the CPU threads
     * will spin in a loop.  Until a ready queue is implemented, we'll put the
     * thread to sleep to keep it from consuming 100% of the CPU time.  Once
     * you implement a proper idle() function using a condition variable,
     * remove the call to mt_safe_usleep() below.
     */
    // mt_safe_usleep(1000000);
}


/*
 * preempt() is the handler called by the simulator when a process is
 * preempted due to its timeslice expiring.
 *
 * This function should place the currently running process back in the
 * ready queue, and call schedule() to select a new runnable process.
 */
extern void preempt(unsigned int cpu_id)
{
    /* FIX ME */


    pthread_mutex_lock(&current_mutex); // locks the current mutex

    pcb_t* curr_process = current[cpu_id]; // gets the current process running on our cpu
    curr_process->state = PROCESS_READY;   // sets that current process as ready

    pthread_mutex_unlock(&current_mutex); // unlocks the current mutex so it can be used again

    queue(curr_process); // adds the current process to the linked list

    schedule(cpu_id);

}


/*
 * yield() is the handler called by the simulator when a process yields the
 * CPU to perform an I/O request.
 *
 * It should mark the process as WAITING, then call schedule() to select
 * a new process for the CPU.
 */
extern void yield(unsigned int cpu_id)
{
    /* FIX ME */

    pthread_mutex_lock(&current_mutex); // locks the current mutex

    pcb_t *curr_process = current[cpu_id]; // gets the current process running on our cpu
    curr_process->state = PROCESS_WAITING; // sets that current process as waiting

    pthread_mutex_unlock(&current_mutex); // unlocks the current mutex so it can be used again

    schedule(cpu_id);
}


/*
 * terminate() is the handler called by the simulator when a process completes.
 * It should mark the process as terminated, then call schedule() to select
 * a new process for the CPU.
 */
extern void terminate(unsigned int cpu_id)
{
    /* FIX ME */

    pthread_mutex_lock(&current_mutex); // locks the current mutex

    pcb_t *curr_process = current[cpu_id];    // gets the current process running on our cpu
    curr_process->state = PROCESS_TERMINATED; // sets that current process as terminated

    pthread_mutex_unlock(&current_mutex); // unlocks the current mutex so it can be used again

    schedule(cpu_id);
}


/*
 * wake_up() is the handler called by the simulator when a process's I/O
 * request completes.  It should perform the following tasks:
 *
 *   1. Mark the process as READY, and insert it into the ready queue.
 *
 *   2. If the scheduling algorithm is LRTF, wake_up() may need
 *      to preempt the CPU with lower remaining time left to allow it to
 *      execute the process which just woke up with higher reimaing time.
 * 	However, if any CPU is currently running idle,
* 	or all of the CPUs are running processes
 *      with a higher remaining time left than the one which just woke up, wake_up()
 *      should not preempt any CPUs.
 *	To preempt a process, use force_preempt(). Look in os-sim.h for 
 * 	its prototype and the parameters it takes in.
 */
extern void wake_up(pcb_t *process)
{
    /* FIX ME */
    process->state = PROCESS_READY;
    queue(process);

    if (alg_type == 'r') {
        unsigned int count = 0;
        while (count < cpu_count) {
            if (current[count]) {
                force_preempt(count);
            }
            count ++;
        }
    }
}


/*
 * main() simply parses command line arguments, then calls start_simulator().
 * You will need to modify it to support the -l and -r command-line parameters.
 */
int main(int argc, char *argv[])
{

    /* Parse command-line arguments */
    if (argc < 2)
    {
        fprintf(stderr, "Multithreaded OS Simulator\n"
            "Usage: ./os-sim <# CPUs> [ -l | -r <time slice> ]\n"
            "    Default : FIFO Scheduler\n"
	        "         -l : Longest Remaining Time First Scheduler\n"
            "         -r : Round-Robin Scheduler\n\n");
        return -1;
    }

    cpu_count = strtoul(argv[1], NULL, 0);
    time_slice = -1;
    alg_type = 'f';

    // Check for additional argument
    if (argc > 2) {
        for (int i = 2; i < argc; ++i) {
            if (i < argc - 1 && !strcmp("-r", argv[i])) {
                alg_type = 'r';
                time_slice = atoi(argv[i + 1]);
            }
        }
    }

    

    /* FIX ME - Add support for -l and -r parameters*/


    /* Allocate the current[] array and its mutex */
    current = malloc(sizeof(pcb_t*) * cpu_count);

    for (int i = 0; i < cpu_count; ++i) {
        current[i] = NULL;
    }

    assert(current != NULL);
    pthread_mutex_init(&current_mutex, NULL);

    /* Start the simulator in the library */
    start_simulator(cpu_count);

    pthread_mutex_destroy(&current_mutex);

    return 0;
}


/** New function to add a process to the queue (implemented using a linkedlist)*/
static void queue(pcb_t* new_process) {

    //lock the free mutex
    pthread_mutex_lock(&free_mutex); 

    
    pcb_t* curr_node = head; 

    //empty list
    if (!curr_node) {

        head = new_process;

    } else {

        //reach the last node in the list
        while (curr_node->next) {
            curr_node = curr_node->next;
        }

        curr_node->next = new_process;

    }

    new_process->next = NULL;

    pthread_cond_broadcast(&not_idle);
    pthread_mutex_unlock(&free_mutex);
    
}


/** New function to remove a process */
static pcb_t* dequeue() {

    pcb_t *popped_node;
    
    pthread_mutex_lock(&free_mutex); // lock the free mutex since we're about to remove from the linked list

    popped_node = head;

    if (popped_node) {
            head = popped_node->next;
    }

    pthread_mutex_unlock(&free_mutex); // unlock the free mutex since we're done changing the linked list
    return popped_node;

}