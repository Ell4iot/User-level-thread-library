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

queue_t ready_queue = queue_create();
queue_t running_queue = queue_create();

queue_t block_queue = queue_create();

uthread_t count;


struct TCB {
    uthread_t TID;
    uthread_ctx_t context;
    // 0 for running, 1 for ready, 2 for blocked, 3 for zombie
    int state;
    void* stack;
};

typedef struct TCB* TCB_t;

TCB_t running_TCB_t;

int uthread_start(int preempt)
{
	/* TODO */
    void(preempt);

    // create the main thread
    TCB_t thread = (TCB_t) malloc(sizeof(struct TCB));
    uthread_ctx_t uctx;
    count = 0;


    thread->TID = 0;
    thread->context = uctx;

    running_TCB_t = thread;

    queue_enqueue(ready_queue, thread);

	return -1;
}

int uthread_stop(void)
{
	/* TODO */
	return -1;
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
    uthread_ctx_init(uctx, stack, func);
    thread->context = uctx;

    queue_enqueue(ready_queue, thread);
	//return -1;
    return count;
}

void uthread_yield(void)
{
    queue_enqueue(ready_queue, running_TCB_t);
    TCB_t previous_running_TCB_t = running_TCB_t;
    queue_dequeue(ready_queue,(void**)&running_TCB_t);
    uthread_ctx_switch(previous_running_TCB_t,running_TCB_t);
	/* TODO */





}

uthread_t uthread_self(void)
{
	return -1;
}

void uthread_exit(int retval)
{
	/* TODO */
}

int uthread_join(uthread_t tid, int *retval)
{
	/* TODO */
	if (tid == 0) {
        return -1;
    }

    TCB_t current;
    current = running_TCB_t;
    queue_enqueue(block_queue, current);

    while (true) {
        if (queue_length(ready_queue) > 0) {
            queue_dequeue(ready_queue, (void**)&running_TCB_t);
            uthread_ctx_switch( &(current->context),&(running_TCB_t->context));
        } else {
            break;
        }
    }
}

