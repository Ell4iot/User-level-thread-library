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
    int state;
    void* stack;
    int retval;
    struct TCB* child;  // "child" to collect
    struct TCB* parent; // "parent" which is waiting you
};

typedef struct TCB* TCB_t;

queue_t ready_queue;
queue_t zombie_queue;
uthread_t count;
TCB_t running_thread;
TCB_t main_thread;
int preempt_indicator;

int uthread_start(int preempt)
{
    ready_queue = queue_create();
    zombie_queue = queue_create();

    // create the main thread pointer
    TCB_t thread = (TCB_t) malloc(sizeof(struct TCB));
    if (thread == NULL) {
        // failure
        return -1;
    }
    count = 0;
    thread->TID = 0;

    running_thread = thread;
    main_thread = thread;
    if (preempt) {
        // set the global preempt var to true
        // we need this variable in uthread_stop
        preempt_indicator = 1;
        preempt_start();
    }
	return 0;
}

int uthread_stop(void)
{
    preempt_disable();

	if (running_thread->TID != 0) {
        // only the main thread can call this function
        preempt_enable();
        return -1;

    }

    if (queue_destroy(ready_queue) == -1) {
        // ready_queue is not empty
        preempt_enable();
        return -1;
    }

    if (queue_destroy(zombie_queue) == -1) {
        // zombie_queue is not empty
        preempt_enable();
        return -1;
    }

    if (preempt_indicator) {
        // only called "if reemption was enabled."
        preempt_stop();
    }
    free(main_thread);
    return 0;
}

int uthread_create(uthread_func_t func)
{
    if (func == NULL) {
        return -1;
    }
    // create new stack object
    TCB_t thread = (TCB_t) malloc(sizeof(struct TCB));
    void* stack;
    uthread_ctx_t uctx;

    preempt_disable();
    // creatre TID
    if (count == USHRT_MAX) {
        // failure
        return -1;
    }
    count++;
    preempt_enable();
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

    preempt_disable();
    queue_enqueue(ready_queue, thread);
    preempt_enable();

    return thread->TID;
}

void uthread_yield(void)
{
    preempt_disable();
    if (queue_length(ready_queue) != 0) {
        if ((running_thread->state) == NORMAL) {
            queue_enqueue(ready_queue, running_thread);
        }
        TCB_t previous_running = running_thread;
        queue_dequeue(ready_queue,(void**)&running_thread);
        uthread_ctx_switch(&(previous_running->context),
                           &(running_thread->context));
    }
}

uthread_t uthread_self(void)
{
	return running_thread->TID;
}

void uthread_exit(int retval)
{
    preempt_disable();
    running_thread->retval = retval;
    running_thread->state = ZOMBIE;
    // unblock the parent
    if (running_thread->parent != NULL) {
        running_thread->parent->state = NORMAL;
        queue_enqueue(ready_queue, running_thread->parent);
    } else {
        // need someone to collect them
        queue_enqueue(zombie_queue, running_thread);
    }
    preempt_enable();
    uthread_yield();
}

static int find_item(queue_t q, void *data, void *arg)
{
    TCB_t temp = (TCB_t)data;
    uthread_t* match = (uthread_t*)arg;
    (void)q; //unused

    if (temp->TID == *match) {
        return 1;
    }
    return 0;
}

int uthread_join(uthread_t tid, int *retval)
{
   preempt_disable();
    preempt_disable();
    if (tid == 0) {
        // main can not be joined
        return -1;
    } else if (tid == uthread_self()) {
        // you can not join yourself
        return -1;
    } else if (running_thread->parent != NULL) {
        if (running_thread->parent->TID == tid) {
            // you can not join the thread that already joins you
            return -1;
        }
    }
    // find in the queue whether is joined or not
    TCB_t thread_to_join = NULL;

    queue_iterate(zombie_queue, find_item, (void*)&tid, (void**)&thread_to_join);
    if (thread_to_join == NULL) {
        queue_iterate(ready_queue, find_item, (void*)&tid, (void**)&thread_to_join);
        if (thread_to_join == NULL) {
            return -1;
        } else if (thread_to_join->parent != NULL) {
            return -1;
        } else {
            // beginning yielding
            thread_to_join->parent = running_thread;
            running_thread->child = thread_to_join;
            running_thread->state = BLOCKED;
            uthread_yield();
        }
    }
    thread_to_join->parent = running_thread;
    running_thread->child = thread_to_join;
    preempt_enable();
    // free the thread
    if (retval != NULL){
        *retval = thread_to_join->retval;
    }
    uthread_ctx_destroy_stack(thread_to_join->stack);
    free(thread_to_join);
    return 0;
}


