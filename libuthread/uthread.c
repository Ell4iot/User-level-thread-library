
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
queue_t zombie_queue;
uthread_t count;
TCB_t running_thread;

int uthread_start(int preempt)
{
	/* TODO */
    (void)preempt;
    ready_queue = queue_create();
    zombie_queue = queue_create();

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
    //preempt_start();
	return 0;
}

int uthread_stop(void)
{
    //preempt_disable();
    //preempt_stop();

	/* TODO */
    if (queue_destroy(ready_queue) == -1) {
        return -1;
    } else if (queue_destroy(zombie_queue) == -1) {
        return -1;
    }
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

    //preempt_disable();
    // creatre TID
    if (count == USHRT_MAX) {
        // failure
        return -1;
    }
    count++;
    //preempt_enable();

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
    //preempt_disable();
    //printf("tid is %hu\n", uthread_self());
    if ((running_thread->state) == NORMAL) {
        //printf("line 127   i am not a zombie???\n");
        queue_enqueue(ready_queue, running_thread);
        //printf("line 108, current queue length: %d\n", queue_length
        //(ready_queue));
    }

    TCB_t previous_running = running_thread;

    queue_dequeue(ready_queue,(void**)&running_thread);

    //printf("in yield, after dequeue, the queue length is %d\n", queue_length

    //printf("line 117, %hu\n", previous_running->TID);
    //printf("line 118, %hu\n", running_thread->TID);

    uthread_ctx_switch(&(previous_running->context),
                       &(running_thread->context));
    //preempt_enable();
}

uthread_t uthread_self(void)
{
	return running_thread->TID;
}

void uthread_exit(int retval)
{
	/* TODO */
    //preempt_disable();
    running_thread->retval = retval;
    running_thread->state = ZOMBIE;
    //printf("running tid: %d is exiting\n", running_thread->TID);
    // unblock the parent
    if (running_thread->parent != NULL) {
        running_thread->parent->state = NORMAL;
        queue_enqueue(ready_queue, running_thread->parent);
    } else {
        // need someone to collect them
        queue_enqueue(zombie_queue, running_thread);
    }
    //preempt_enable();
    //printf("calling yield inside exit\n");
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

    //preempt_disable();
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

    //printf("line 19000000000000\n");
    //printf("    join queue length is: %d\n", queue_length(ready_queue));

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
            //preempt_disable();
            running_thread->child = thread_to_join;
            running_thread->state = BLOCKED;
            //printf("calling yield inside join\n");
            uthread_yield();
        }
    }
    thread_to_join->parent = running_thread;
    running_thread->child = thread_to_join;
    // free the thread
    if (retval != NULL){
        *retval = thread_to_join->retval;
    }
    uthread_ctx_destroy_stack(thread_to_join->stack);
    free(thread_to_join);
    return 0;

}


