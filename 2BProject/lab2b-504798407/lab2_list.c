/*
//
// Created by Matei Lupu on 11/1/18.
//
*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#include "SortedList.h"

#define THREADS    't'
#define ITERATIONS 'i'
#define YIELD      'y'
#define SYNC       'n'
#define MUTEX      'm'
#define SPINLOCK   's'
#define LISTS      'q'

char lock = 'x';
char yieldFlag = 'x';

pthread_mutex_t* mutexes;
int* spinLock = 0;
int opt_yield = 0;

int insert = 0;
int delete = 0;
int lookup = 0;
long long lockDuration = 0;
int length = 0;

int numberOfThreads    = 1;
int numberOfIterations = 1;
int numberOfElements   = 0;
int numberOfLists      = 1;
int* lists;
SortedList_t* sortedList;

SortedList_t* elementArray;

SortedList_t* initializeRandKey( int numberOfElements )
{
    sortedList = malloc(numberOfLists*sizeof(SortedList_t));
    int j = 0;
    for(; j < numberOfLists; j++)
    {
        sortedList[j].key = NULL;
        sortedList[j].next = &sortedList[j];
        sortedList[j].prev = &sortedList[j];
    }

    char* characters = "abcdefghijklmnopqrstuvwxyz";

    int i = 0;
    for(; i < numberOfElements; i++)
    {
        int pickChar = rand() % 26;

        elementArray[i].key = &characters[pickChar];
    }

    return sortedList;
}

void hashElements()
{
    lists = malloc(numberOfElements*sizeof(int));
    int i = 0;
    for(; i < numberOfElements; i++)
    {
        lists[i] = elementArray[i].key[0] % numberOfLists;
    }
}

void handler()
{
    fprintf(stderr, "segmentation fault caught");
    exit(2);
}

void timerStartError()
{
    fprintf(stderr, "Error could not start timer");
    exit(1);
}

void timerEndError()
{
    fprintf(stderr, "Error could not start timer");
    exit(1);
}

void* iterateLocks( void* threadNum)
{

    struct timespec startTime;
    struct timespec endTime;

    int thread = *(int*)threadNum;

    int i = thread;
    //insert
    for(; i < numberOfElements; i += numberOfThreads)
    {
        if(lock == 'x')
        {
            SortedList_insert(&sortedList[lists[i]], &elementArray[i]);
        }
        else if ( lock == MUTEX )
        {
            if( clock_gettime(CLOCK_MONOTONIC, &startTime) < 0 )
            {
                timerStartError();
            }

            pthread_mutex_lock(&mutexes[lists[i]]);

            if( clock_gettime(CLOCK_MONOTONIC, &endTime) < 0 )
            {
                timerEndError();
            }

            lockDuration += (endTime.tv_sec - startTime.tv_sec) * 1000000000 + endTime.tv_nsec - startTime.tv_nsec;

            SortedList_insert(&sortedList[lists[i]], &elementArray[i]);

            pthread_mutex_unlock(&mutexes[lists[i]]);
        }
        else if ( lock == SPINLOCK )
        {
            if( clock_gettime(CLOCK_MONOTONIC, &startTime) < 0 )
            {
                timerStartError();
            }

            while(__sync_lock_test_and_set(&spinLock[lists[i]], 1))
            {
                // just wait for resource to be freed up
            }

            if( clock_gettime(CLOCK_MONOTONIC, &endTime) < 0 )
            {
                timerEndError();
            }

            lockDuration += (endTime.tv_sec - startTime.tv_sec) * 1000000000 + endTime.tv_nsec - startTime.tv_nsec;

            SortedList_insert(&sortedList[lists[i]], &elementArray[i]);

            __sync_lock_release(&spinLock[lists[i]]);
        }
    }

    //length
    if(lock == 'x')
    {
        int j = 0;
        for(; j < numberOfLists; j++)
        {
            int k = SortedList_length(&sortedList[j]);
            if( k < 0 )
            {
                fprintf(stderr, "invalid list, could not retrieve size 1 \n");
                exit(2);
            }
            length += k;
        }

    }
    else if ( lock == MUTEX )
    {
        int j = 0;
        for(; j < numberOfLists; j++) {

            if( clock_gettime(CLOCK_MONOTONIC, &startTime) < 0 )
            {
                timerStartError();
            }

            pthread_mutex_lock(&mutexes[j]);

            if( clock_gettime(CLOCK_MONOTONIC, &endTime) < 0 )
            {
                timerEndError();
            }

            lockDuration += (endTime.tv_sec - startTime.tv_sec) * 1000000000 + endTime.tv_nsec - startTime.tv_nsec;

            int k = SortedList_length(&sortedList[j]);
            if( k < 0 )
            {
                fprintf(stderr, "invalid list, could not retrieve size 2 \n");
                exit(2);
            }
            length += k;
            pthread_mutex_unlock(&mutexes[j]);
        }
    }
    else if ( lock == SPINLOCK )
    {
        int j = 0;
        for(; j < numberOfLists; j++) {

            if (clock_gettime(CLOCK_MONOTONIC, &startTime) < 0) {
                timerStartError();
            }

            while (__sync_lock_test_and_set(&spinLock[j], 1)) {
                // just wait for resource to be freed up
            }

            if (clock_gettime(CLOCK_MONOTONIC, &endTime) < 0) {
                timerEndError();
            }

            lockDuration += (endTime.tv_sec - startTime.tv_sec) * 1000000000 + endTime.tv_nsec - startTime.tv_nsec;

            int k = SortedList_length(&sortedList[j]);
            if (k < 0) {
                fprintf(stderr, "invalid list, could not retrieve size 3 \n");
                exit(2);
            }
            length += k;

            __sync_lock_release(&spinLock[j]);
        }
    }

    //delete
    i = thread;
    SortedListElement_t* removeElement;

    for(; i < numberOfElements; i += numberOfThreads)
    {
        if(lock == 'x')
        {
            removeElement = SortedList_lookup(&sortedList[lists[i]], elementArray[i].key);
            if(removeElement == NULL)
            {
                fprintf(stderr, "could not find element \n");
                exit(2);
            }

            if(SortedList_delete(removeElement) == 1)
            {
                fprintf(stderr, "could not remove element \n");
                exit(2);
            }
        }
        else if ( lock == MUTEX )
        {
            if( clock_gettime(CLOCK_MONOTONIC, &startTime) < 0 )
            {
                timerStartError();
            }

            pthread_mutex_lock(&mutexes[lists[i]]);

            if( clock_gettime(CLOCK_MONOTONIC, &endTime) < 0 )
            {
                timerEndError();
            }

            lockDuration += (endTime.tv_sec - startTime.tv_sec) * 1000000000 + endTime.tv_nsec - startTime.tv_nsec;

            removeElement = SortedList_lookup(&sortedList[lists[i]], elementArray[i].key);

            if(removeElement == NULL)
            {
                fprintf(stderr, "could not find element \n");
                exit(2);
            }

            if(SortedList_delete(removeElement) == 1)
            {
                fprintf(stderr, "could not remove element \n");
                exit(2);
            }
            pthread_mutex_unlock(&mutexes[lists[i]]);
        }
        else if ( lock == SPINLOCK )
        {
            if( clock_gettime(CLOCK_MONOTONIC, &startTime) < 0 )
            {
                timerStartError();
            }

            while(__sync_lock_test_and_set(&spinLock[lists[i]], 1))
            {
                // just wait for resource to be freed up
            }

            if( clock_gettime(CLOCK_MONOTONIC, &endTime) < 0 )
            {
                timerEndError();
            }

            lockDuration += (endTime.tv_sec - startTime.tv_sec) * 1000000000 + endTime.tv_nsec - startTime.tv_nsec;

            removeElement = SortedList_lookup(&sortedList[lists[i]], elementArray[i].key);

            if(removeElement == NULL)
            {
                fprintf(stderr, "could not find element \n");
                exit(2);
            }

            if(SortedList_delete(removeElement) == 1)
            {
                fprintf(stderr, "could not remove element \n");
                exit(2);
            }
            __sync_lock_release(&spinLock[lists[i]]);
        }
    }
    return NULL;
}

int main( int argc, char* argv[] )
{
    static struct option long_options[] = {
            {"threads"   , optional_argument, NULL, THREADS   },
            {"iterations", optional_argument, NULL, ITERATIONS},
            {"yield"     , required_argument, NULL, YIELD     },
            {"sync"      , required_argument, NULL, SYNC      },
            {"lists"     , required_argument, NULL, LISTS     }

    };

    int getoptReturnChar = -1;
    char optionType[50] = "list";

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
            yieldFlag = 'p';
            strcat(optionType, "-");
            size_t i = 0;
            for( ; i < strlen(optarg); i++ )
            {
                switch(optarg[i])
                {
                    case 'i':
                        insert = 1;
                        opt_yield |= INSERT_YIELD;
                        strcat(optionType, "i");
                        break;
                    case 'd':
                        delete = 1;
                        opt_yield |= DELETE_YIELD;
                        strcat(optionType, "d");
                        break;
                    case 'l':
                        lookup = 1;
                        opt_yield |= LOOKUP_YIELD;
                        strcat(optionType, "l");
                        break;
                    default:
                        fprintf(stderr, "Not one of the specified options \n");
                        exit(1);
                }
            }
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
                default:
                    fprintf(stderr, "Not one of the specified options \n");
                    exit(1);
            }

        }
        else if ( getoptReturnChar == LISTS )
        {
            if( atoi(optarg) >= 1)
            {
                numberOfLists = atoi(optarg);
            }
            else
            {
                fprintf(stderr, "Invalid number of lists");
                exit(2);
            }
        }
        else
        {
            fprintf(stderr, "Not one of the specified options \n");
            exit(1);
        }
    }


    if(yieldFlag == 'x')
    {
        strcat(optionType, "-none");
    }

    switch(lock)
    {
        case 'm':
            strcat(optionType, "-m");
            break;
        case 's':
            strcat(optionType, "-s");
            break;
        case 'x':
            strcat(optionType, "-none");
            break;
    }


    signal(SIGSEGV, handler); //implement handler

//    if(lock == MUTEX)
//    {
//        pthread_mutex_init(&mutex, NULL);
//    }


    if( lock == MUTEX )
    {
        mutexes = malloc(numberOfLists * sizeof(pthread_mutex_t));
        int i = 0;
        for (; i < numberOfLists; i++)
        {
            pthread_mutex_init(&mutexes[i], NULL);
        }
    }
    else if ( lock == SPINLOCK )
    {
        spinLock = malloc(numberOfLists* sizeof(int));
        int i = 0;
        for (; i < numberOfLists; i++)
        {
            spinLock[i] = 0;
        }
    }

    numberOfElements = numberOfIterations * numberOfThreads;
    elementArray = malloc(numberOfElements*sizeof(SortedListElement_t));
    initializeRandKey(numberOfElements);

    hashElements();

    pthread_t* pid = malloc(numberOfThreads*sizeof(pthread_t));
    int* pthreadNum = malloc(numberOfThreads * sizeof(int));

    struct timespec startTime;
    if( clock_gettime(CLOCK_MONOTONIC, &startTime) < 0 )
    {
        timerStartError();
    }


    int i = 0;
    for(; i < numberOfThreads; i++)
    {
        pthreadNum[i] = i;
        if( pthread_create(&pid[i], NULL, iterateLocks, &pthreadNum[i]) != 0)
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
    if( clock_gettime(CLOCK_MONOTONIC, &endTime) < 0 )
    {
        timerEndError();
    }

    free(elementArray);
    free(pthreadNum);
    free(pid);
    free(sortedList);
    free(mutexes);
    free(lists);
    free(spinLock);

    long long duration = (endTime.tv_sec - startTime.tv_sec) * 1000000000 + endTime.tv_nsec - startTime.tv_nsec;
    int operations = numberOfIterations * numberOfThreads * 3;
    int avgTimePerOperation = duration / operations;
    long avgLockTime = lockDuration / operations;

    //int numberOfLists = 1;

    printf("%s,%d,%d,%d,%d,%lld,%d,%ld\n",optionType,numberOfThreads,numberOfIterations,numberOfLists,operations,duration,avgTimePerOperation, avgLockTime);

    exit(0);
}


