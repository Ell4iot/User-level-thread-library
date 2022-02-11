#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

struct sigaction new_action;
struct sigaction old_action;
struct itimerval new_timer;
struct itimerval old_timer;
sigset_t block;

void alarm_handler(int signum)
{
    (void)signum;

    uthread_yield();
}


void preempt_start(void)
{
    /* TODO */
    memset (&new_action, 0, sizeof (new_action));
    new_action.sa_handler = &alarm_handler;
    sigaction(SIGVTALRM, &new_action, &old_action);

    sigemptyset(&block);
    sigaddset(&block, SIGVTALRM);


    new_timer.it_value.tv_sec = 0;
    new_timer.it_value.tv_usec = 10000;  // 100hz

    new_timer.it_interval.tv_sec = 0;
    new_timer.it_interval.tv_usec = 10000;
    handler_time = 0;
    setitimer(ITIMER_VIRTUAL, &new_timer, &old_timer);

    preempt_enable();
}

void preempt_stop(void)
{

    preempt_disable();

    // disarm the timer
    new_timer.it_value.tv_sec = 0;
    new_timer.it_value.tv_usec = 0;
    setitimer(ITIMER_VIRTUAL, &new_timer, NULL);
    // restore previous timer
    setitimer(ITIMER_VIRTUAL, &old_timer, NULL);

    // uninstall the handler
    new_action.sa_handler = SIG_DFL;
    sigaction(SIGVTALRM, &new_action, NULL);
    // restore previous action
    sigaction(SIGVTALRM, &old_action, NULL);


}

void preempt_enable(void)
{
    sigprocmask(SIG_UNBLOCK, &block, NULL);
}

void preempt_disable(void)
{
    sigprocmask(SIG_BLOCK, &block, NULL);
}


