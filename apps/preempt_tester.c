#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>

int thread3(void)
{
    int i = 0;
    while (1) {
        if (i == 100000) {
            break;
        }
        i++
    }
    return 1;
}

int thread2(void)
{
    int i = 0;
    while (1) {
        if (i == 100000) {
            break;
        }
        i++
    }
    return 1;
}

int thread1(void)
{
    int i = 0;
    while (1) {
        if (i == 100000) {
            break;
        }
        i++
    }
    return 1;
}

int main(void) {
    uthread_t t1;
    uthread_t t2;
    uthread_t t3;

    uthread_start(0);
    t1 = uthread_create(thread1);
    t2 = uthread_create(thread2);
    t3 = uthread_create(thread3);
    uthread_join(t1, NULL);
    uthread_join(t2, NULL);
    uthread_join(t3, NULL);
    uthread_stop();


    return 0;
}