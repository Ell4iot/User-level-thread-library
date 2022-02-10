#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>
uthread_t t1;
uthread_t t2;
uthread_t t3;

int thread3(void)
{
    int i = 0;
    while (1) {
        if (i == 1000) {
            break;
        }
        i++;
    }
    printf("i am 3\n");
    return 1;

}

int thread2(void)
{
    int i = 0;
    while (1) {
        if (i == 1000) {
            break;
        }
        i++;
    }
    printf("i am 1\n");
    return 1;
}

int thread1(void)
{
    int i = 0;
    while (1) {
        if (i == 1000) {
            break;
        }
        i++;
    }
    uthread_yield();
    printf("i am 1\n");
    return 1;
}

int main(void) {


    uthread_start(0);
    t1 = uthread_create(thread1);
    printf("create thread 1\n");
    t2 = uthread_create(thread2);

    printf("create thread 2\n");
    t3 = uthread_create(thread3);
    printf("create thread 3\n");

    uthread_join(t1, NULL);
    printf("finish join t1\n");
    uthread_join(t2, NULL);
    printf("finish join t2\n");
    uthread_join(t3, NULL);
    printf("finish join t3\n");
    uthread_stop();


    return 0;
}