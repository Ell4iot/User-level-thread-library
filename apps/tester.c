#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <uthread.h>
uthread_t t1;
uthread_t t2;
uthread_t t3;


int thread3(void)
{
    printf("first time 3\n");
    while(1);

    return 1;
}

int thread2(void)
{
    printf("first time 2\n");
    //int i = 0;
    while(1);

    return 1;
}

int thread1(void)
{
    //int i = 0;
    printf("first time 1\n");
    return 1;
}

int main(void) {


    uthread_start(0);


    t1 = uthread_create(thread1);
    t2 = uthread_create(thread2);
    t3 = uthread_create(thread3);
    printf("begin join 1\n");
    uthread_join(t1, NULL);
    printf("end of join1\n");
    uthread_join(t2, NULL);
    uthread_join(t3, NULL);

    uthread_stop();

    return 0;
}