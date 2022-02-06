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
uthread_t count;
struct TCB {
    uthread_t TID;
    uthread_ctx_t context;
    // 0 for running, 1 for ready, 2 for blocked, 3 for zombie
    int state;
    void* stack;
    uthread_func_t* func;
};

typedef struct TCB* TCB_t;

int uthread_start(int preempt)
{
	/* TODO */
    void(preempt);

    // create the main thread
    TCB_t thread = (TCB_t) malloc(sizeof(struct TCB));
    count = 0;
    thread->TID = 0;
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

    count++;
    thread->TID = count;

    // allocate stack segment
    stack = uthread_ctx_alloc_stack();
    uthread_ctx_init(uctx, stack, func);

    queue_enqueue(ready_queue, thread);
	return -1;
}

void uthread_yield(void)
{
	/* TODO */
}

uthread_t uthread_self(void)
{
	/* TODO */
	return -1;
}

void uthread_exit(int retval)
{
	/* TODO */
}

int uthread_join(uthread_t tid, int *retval)
{
	/* TODO */
	return -1;
}

