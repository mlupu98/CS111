//
// Created by Matei Lupu on 10/31/18.
//

#include <unistd.h>

#include <string.h>

#include <errno.h>

#include <getopt.h>

#include <stdbool.h>

#include <stddef.h>

#include <stdio.h>

#include <stdlib.h>

#include <pthread.h>

#define THREADS    't'
#define ITERATIONS 'i'
#define YIELD      'y'
#define SYNC       'n'
#define MUTEX      'm'
#define SPINLOCK   's'
#define COMPSWAP   'c'


long long totalSum = 0;
int numberOfIterations = 1;

char lock = 'x';
int opt_yield = 0;
pthread_mutex_t mutex;
int spinLock = 0;

void add(long long *pointer, long long value) {
    long long sum = *pointer + value;
    if (opt_yield)
        sched_yield();
    *pointer = sum;
}

void* iterate( )
{
    long long addOrSubtractOne = 1;

    int i = 0;
    for(; i < 2; i++)
    {
        int j = 0;
        for(; j < numberOfIterations; j++)
        {
            add( &totalSum, addOrSubtractOne );
        }
        addOrSubtractOne -= 2;
    }

    return NULL;
}


void* iterateLocks(){

    long long addOrSubtractOne = 1;
    int i = 0;
    for(; i < 2; i++)
    {
        int j = 0;
        for (; j < numberOfIterations; j++)
        {
            //printf("%c",lock);
            switch(lock)
            {
                case 'x': //NONE
                    add(&totalSum, addOrSubtractOne);
                    break;
                case 'm':
                    pthread_mutex_lock(&mutex);
                    add(&totalSum, addOrSubtractOne);
                    pthread_mutex_unlock(&mutex);
                    break;
                case 's':
                    while (__sync_lock_test_and_set(&spinLock, 1));
                    add(&totalSum, addOrSubtractOne);
                    __sync_lock_release(&spinLock);
                    break;
                case 'c':
                    ; //WHY I HAVE NO IDEA
                    long long old;
                    long long new;
                    do {
                        old = totalSum;
                        new = old + addOrSubtractOne;
                        if (opt_yield)
                        {
                            sched_yield();
                        }
                    } while (__sync_val_compare_and_swap(&totalSum, old, new) != old);
                    break;
            }

        }

        addOrSubtractOne -= 2;
    }
    return NULL;
}

int main( int argc, char * argv[])
{

    char optionType[50] = "add";
    int getoptReturnChar   = -1; // returns -1 if option not found
    int numberOfThreads    = 1; // default values

    struct timespec startTime;

    static struct option long_options[] = {
            {"threads"   , optional_argument, NULL, THREADS   },
            {"iterations", optional_argument, NULL, ITERATIONS},
            {"yield"     , no_argument      , NULL, YIELD     },
            {"sync"      , required_argument, NULL, SYNC      }

    };

    while( (getoptReturnChar = getopt_long(argc, argv, "", long_options, NULL)) != -1)
    {
        if( getoptReturnChar == THREADS)
        {
            numberOfThreads = atoi(optarg);
        }
        else if ( getoptReturnChar == ITERATIONS )
        {
            numberOfIterations = atoi(optarg);
        }
        else if ( getoptReturnChar == YIELD )
        {
            opt_yield = 1;
            strcat(optionType, "-yield");
        }
        else if ( getoptReturnChar == SYNC )
        {
            switch(optarg[0]){
                case 'm':
                    lock = MUTEX;
                    break;
                case 's':
                    lock = SPINLOCK;
                    break;
                case 'c':
                    lock = COMPSWAP;
                    break;
                default:
                    fprintf(stderr, "Not one of the specified options \n");
                    exit(1);
            }

        }
        else
        {
            fprintf(stderr, "Not one of the specified options \n");
            exit(1);
        }
    }



    switch(lock){
        case 'm':
            strcat(optionType, "-m");
            break;
        case 's':
            strcat(optionType, "-s");
            break;
        case 'c':
            strcat(optionType, "-c");
            break;
        case 'x':
            strcat(optionType, "-none");
            break;
    }

    if (lock == MUTEX)
    {
        pthread_mutex_init(&mutex, NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &startTime);

    pthread_t* pid = malloc(numberOfThreads * sizeof(pthread_t));

    int i = 0;
    for(; i < numberOfThreads; i++)
    {
        if( pthread_create(&pid[i], NULL, iterateLocks, NULL) != 0)
        {
            fprintf(stderr, "Could not create threads \n");
            exit(1);
        }

    }

    int j = 0;
    for(; j < numberOfThreads; j++)
    {
        if( pthread_join(pid[j], NULL) != 0 )
        {
            fprintf(stderr, "Could not join threads \n");
            exit(1);
        }
    }



    struct timespec endTime;
    clock_gettime(CLOCK_MONOTONIC, &endTime);

    free(pid);

    long long duration = (endTime.tv_sec - startTime.tv_sec) * 1000000000 + endTime.tv_nsec - startTime.tv_nsec;
    long long operations = numberOfIterations * numberOfThreads * 2;
    long long avgTimePerOperation = duration / operations;

    printf("%s,%d,%d,%lld,%lld,%lld,%lld\n",optionType,numberOfThreads,numberOfIterations,operations,duration,avgTimePerOperation,totalSum);

    exit(0);
}