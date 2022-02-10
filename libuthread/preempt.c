#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h> //////////////////////////////// warning
#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

struct sigaction new;
struct sigaction old;
struct itimerval new_timer;
struct itimerval old_timer;
int handler_time;

void alarm_handler(int signum)
{
    (void)signum;
    handler_time++;

    printf("28888  %d\n", handler_time);
    //uthread_yield();
}


void preempt_start(void)
{
	/* TODO */
    memset (&new, 0, sizeof (new));
    new.sa_handler = &alarm_handler;
    int resultt = sigaction(SIGVTALRM, &new, NULL);
    printf("line 39 sigaction result%d\n", resultt);


    new_timer.it_value.tv_sec = 0;
    new_timer.it_value.tv_usec = 10000;  // 100hz

    new_timer.it_interval.tv_sec = 0;
    new_timer.it_interval.tv_usec = 10000;
    handler_time = 0;
    int result = setitimer(ITIMER_VIRTUAL, &new_timer, NULL);
    printf("line 49 setitimer result%d\n", result);
    //preempt_enable();
}

void preempt_stop(void)
{
	/* TODO */
    preempt_disable();
    new_timer.it_value.tv_sec = 0;
    new_timer.it_value.tv_usec = 0;
    new.sa_handler = SIG_DFL;
    sigaction(SIGVTALRM, &new, &old);
    setitimer(ITIMER_VIRTUAL, &new_timer, &old_timer);
}

void preempt_enable(void)
{
	/* TODO */
    sigset_t block;
    sigemptyset(&block);
    sigaddset(&block, SIGVTALRM);
    sigprocmask(SIG_UNBLOCK, &block, NULL);
}

void preempt_disable(void)
{
	/* TODO */
    sigset_t block;
    sigemptyset(&block);
    sigaddset(&block, SIGVTALRM);
    sigprocmask(SIG_BLOCK, &block, NULL);
}

