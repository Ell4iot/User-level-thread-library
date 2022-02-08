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

enum {
    NORMAL,
    ZOMBIE,
    BLOCKED
};

struct TCB {
    uthread_t TID;
    uthread_ctx_t context;
    int state; // 0 for running, 1 for ready, 2 for blocked, 3 for zombie
    void* stack;
    int retval;
    struct TCB* child;  // "child" to collect
    struct TCB* parent; // "parent" which is waiting you
};

typedef struct TCB* TCB_t;

queue_t ready_queue;
// queue_t block_queue = queue_create();
//queue_t zombie_queue;
uthread_t count;
TCB_t running_thread;


int uthread_start(int preempt)
{
	/* TODO */
    (void)preempt;
    ready_queue = queue_create();
    //zombie_queue = queue_create();

    // create the main thread pointer
    TCB_t thread = (TCB_t) malloc(sizeof(struct TCB));
    if (thread == NULL) {
        // failure
        return -1;
    }

    uthread_ctx_t uctx;
    count = 0;
    thread->TID = 0;
    thread->context = uctx;
    running_thread = thread;
	return 0;
}

int uthread_stop(void)
{
	/* TODO */
    if (queue_length(ready_queue) != 0) {
        return -1;
    } else {
        free(ready_queue);
        return 0;
    }
}

int uthread_create(uthread_func_t func)
{
    // create new stack object
    TCB_t thread = (TCB_t) malloc(sizeof(struct TCB));
    void* stack;
    uthread_ctx_t uctx;

    // creatre TID
    if (count == USHRT_MAX) {
        // failure
        return -1;
    }
    count++;
    thread->TID = count;

    // allocate stack segment
    stack = uthread_ctx_alloc_stack();
    if (stack == NULL) {
        // failure
        return -1;
    }
    thread->stack = stack;

    // initialize context
    if (uthread_ctx_init(&uctx, stack, func) == -1) {
        // failure
        return -1;
    }
    thread->context = uctx;
    thread->state = NORMAL;
    thread->parent = NULL;
    thread->child = NULL;
    queue_enqueue(ready_queue, thread);
    return thread->TID;
}

void uthread_yield(void)
{
    if ((running_thread->state) == NORMAL) {
        queue_enqueue(ready_queue, running_thread);
        //printf("line 108, current queue length: %d\n", queue_length
        //(ready_queue));
    }

    TCB_t previous_running = running_thread;

    queue_dequeue(ready_queue,(void**)&running_thread);
    //printf("line 117, %hu\n", previous_running->TID);
    //printf("line 118, %hu\n", running_thread->TID);
    uthread_ctx_switch(&(previous_running->context),
                       &(running_thread->context));
}

uthread_t uthread_self(void)
{
	return running_thread->TID;
}

void uthread_exit(int retval)
{
	/* TODO */
    running_thread->retval = retval;
    running_thread->state = ZOMBIE;
    // unblock the parent
    if (running_thread->parent != NULL) {
        running_thread->parent->state = NORMAL;
    }
    queue_enqueue(ready_queue, running_thread->parent);
    uthread_yield();
}

static int find_item(queue_t q, void *data, void *arg)
{
    TCB_t temp = (TCB_t)data;
    uthread_t* match = (uthread_t*)arg;
    (void)q; //unused

    if (temp->TID == *match) {
        //printf("catch the pid\n");
        return 1;
    }

    return 0;
}

int uthread_join(uthread_t tid, int *retval)
{
    if (tid == 0) {
        // main can not be joined
        return -1;
    } else if (tid == uthread_self()) {
        // you can not join yourself
        return -1;
    }
    // find in the queue whether is joined or not
    TCB_t thread_to_join = NULL;
    queue_iterate(ready_queue, find_item, (void*)&tid, (void**)&thread_to_join);
    printf(" 170\n");
    if (thread_to_join == NULL) {
        // tid not found
        //printf("tid %hu not found\n", tid);
        return -1;
    } else if (thread_to_join->parent != NULL) {
        // the thread is already being joined
        //printf("tid %hu is already being joined\n", tid);
        return -1;
    }
    printf(" 180\n");
    thread_to_join->parent = running_thread;
    running_thread->child = thread_to_join;
    printf(" 183\n");
    if (thread_to_join->state != ZOMBIE) {
        running_thread->state = BLOCKED;
        uthread_yield();
    }
    printf(" 188\n");
    // resume from here, collect and free it
    *retval = thread_to_join->retval;
    uthread_ctx_destroy_stack(thread_to_join->stack);
    printf(" 192\n");
    free(thread_to_join);

    return 0;
}

