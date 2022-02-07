#include <assert.h>
#include <limits.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"
/* TODO */

struct TCB {
    uthread_t TID;
    uthread_ctx_t context;
    // 0 for running, 1 for ready, 2 for blocked, 3 for zombie
    int state;
    void* stack;
    int retval;
};

typedef struct TCB* TCB_t;

queue_t ready_queue;
// queue_t block_queue = queue_create();
queue_t zombie_queue;
uthread_t count;
TCB_t running_TCB_t;
TCB_t the_main;

int uthread_start(int preempt)
{
	/* TODO */
    (void)preempt;
    ready_queue = queue_create();
    zombie_queue = queue_create();

    // create the main thread
    TCB_t thread = (TCB_t) malloc(sizeof(struct TCB));
    uthread_ctx_t uctx;
    count = 0;
    thread->TID = 0;
    thread->context = uctx;

    // current running thread is main thread
    running_TCB_t = thread;
    the_main = thread;
    //queue_enqueue(ready_queue, thread);

	return 0;
}

int uthread_stop(void)
{
	/* TODO */
    if (queue_length(ready_queue) != 0) {
        return -1;
    } else {
        while(queue_length(zombie_queue)) {
            TCB_t temp;
            queue_dequeue(zombie_queue, (void**)&temp);
            // deallocate the stack of thread
            uthread_ctx_destroy_stack(temp->stack);
            // free the entire thread
            free(temp);
        }
        // free the queue
        free(ready_queue);
        //free(block_queue);
        free(zombie_queue);
        return 0;
    }
}

int uthread_create(uthread_func_t func)
{
	/* TODO */
    // create new stack object
    TCB_t thread = (TCB_t) malloc(sizeof(struct TCB));
    void* stack;
    uthread_ctx_t uctx;

    // figuring out TID
    count++;
    thread->TID = count;

    // allocate stack segment
    stack = uthread_ctx_alloc_stack();
    thread->stack = stack;

    // initialize context
    uthread_ctx_init(&uctx, stack, func);
    thread->context = uctx;
    thread->state = 1;
    queue_enqueue(ready_queue, thread);
	//return -1;
    return count;
}

void uthread_yield(void)
{
    printf("\nbeginning of yield\n");
    printf("tid: %d state: %d\n", running_TCB_t->TID ,running_TCB_t->state);
    if ((running_TCB_t->state) != 0) {
        queue_enqueue(ready_queue, running_TCB_t);
        printf("line 108, current queue length: %d\n", queue_length(ready_queue));
    }

    TCB_t previous_running = running_TCB_t;

    printf("queue length now is: %d\n", queue_length(ready_queue));
    if (!queue_length(ready_queue)) {
        queue_enqueue(ready_queue, the_main);
        printf("line 114\n");
    }
    queue_dequeue(ready_queue,(void**)&running_TCB_t);
    printf("line 117, %hu\n", previous_running->TID);
    printf("line 118, %hu\n", running_TCB_t->TID);
    uthread_ctx_switch(&(previous_running->context),
                       &(running_TCB_t->context));
	/* TODO */
}

uthread_t uthread_self(void)
{
	return running_TCB_t->TID;
}

void uthread_exit(int retval)
{
	/* TODO */
    running_TCB_t->retval = retval;
    running_TCB_t->state = 0;
    queue_enqueue(zombie_queue, running_TCB_t);
    uthread_yield();
}

int uthread_join(uthread_t tid, int *retval)
{
	/* TODO */
	if (tid == 0) {
        return -1;
    }
    (void)retval;

    TCB_t current;
    current = running_TCB_t;

    //queue_enqueue(block_queue, current);

    while (1) {
        if (queue_length(ready_queue) > 0) {
            queue_dequeue(ready_queue, (void**)&running_TCB_t);
            uthread_ctx_switch( &(current->context),&(running_TCB_t->context));
        } else {
            break;
        }
    }
    printf("end of join\n");
    return 0;
}

