#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

int gLockFlag1, gLockFlag2;

void* t1_main()
{
    int count = 0;
    while(1)
    {
        gLockFlag1 = 1;
        printf("thread1 print %d\n", count);
        count++;
    }
    return NULL;
}

void* t2_main()
{
    int count = 0;
    while(1)
    {
        gLockFlag2 = 1;
        printf("thread2 print %d\n", count);
        count++;
    }
    return NULL;
}

void* HandleWatchDogMain()
{
    struct timeval interval;
    struct timeval lt, ct;
    int *p;
    long int diff;
    int err;

    interval.tv_sec = 2;
    interval.tv_usec = 0;

    sleep(5);
    while(1)
    {
        interval.tv_sec = 5;
        interval.tv_usec = 0;
        gLockFlag1 = 0;
        gLockFlag2 = 0;
        gettimeofday(&lt, 0);
        do
        {
            err = select(0, NULL, NULL, NULL, &interval);
        } while(err < 0 && errno==EINTR);
        gettimeofday(&ct, 0);
        if(gLockFlag1 == 0)
        {
            diff = (ct.tv_sec - lt.tv_sec) * 1000000 + ct.tv_usec - lt.tv_usec;
            printf("thread 1 is blocked long time, diff = %ld\n", diff);
            p = NULL;
            *p = 10000000;
        }
        if(gLockFlag2 == 0)
        {
            diff = (ct.tv_sec - lt.tv_sec) * 1000000 + ct.tv_usec - lt.tv_usec;
            printf("thread 2 is blocked long time, diff = %ld\n", diff);
            p = NULL;
            *p = 10000000;
        }

    }
}


int main()
{
    pthread_t t1, t2, twd;

    pthread_create(&t1, NULL, t1_main, NULL);
    pthread_create(&t2, NULL, t2_main, NULL);
    pthread_create(&twd, NULL, HandleWatchDogMain, NULL);

    while(1)
    {
        pause();
    }
    return 0;
}
