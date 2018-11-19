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

char lock = 'x';
char yieldFlag = 'x';

pthread_mutex_t mutex;
int spinLock = 0;
int opt_yield = 0;

int insert = 0;
int delete = 0;
int lookup = 0;

int numberOfThreads    = 1;
int numberOfIterations = 1;
int numberOfElements   = 0;
SortedList_t* sortedList;

SortedList_t* elementArray;

SortedList_t* initializeRandKey( int numberOfElements )
{
    sortedList = malloc(sizeof(SortedList_t));
    sortedList->next = sortedList;
    sortedList->prev = sortedList;
    sortedList->key = NULL;

    char* characters = "abcdefghijklmnopqrstuvwxyz";

    int i = 0;
    for(; i < numberOfElements; i++)
    {
        //SortedList_t* newElement;
        int pickChar = rand() % 26;

        elementArray[i].key = &characters[pickChar];

    }

    return sortedList;
}

void handler()
{
    fprintf(stderr, "segmentation fault caught");
    exit(2);
}

void* iterateLocks( void* threadNum)
{
    int thread = *(int*)threadNum;

    int i = thread;
    //insert
    for(; i < numberOfElements; i += numberOfThreads)
    {
        if(lock == 'x')
        {
            SortedList_insert(sortedList, &elementArray[i]);
        }
        else if ( lock == MUTEX )
        {
            pthread_mutex_lock(&mutex);

            SortedList_insert(sortedList, &elementArray[i]);

            pthread_mutex_unlock(&mutex);
        }
        else if ( lock == SPINLOCK )
        {
            while(__sync_lock_test_and_set(&spinLock, 1))
            {
                // just wait for resource to be freed up
            }

            SortedList_insert(sortedList, &elementArray[i]);

            __sync_lock_release(&spinLock);
        }
    }

    //length
    if(lock == 'x')
    {
        int k = SortedList_length(sortedList);
        if( k < 0 )
        {
            fprintf(stderr, "invalid list, could not retrieve size \n");
            exit(2);
        }
    }
    else if ( lock == MUTEX )
    {
        pthread_mutex_lock(&mutex);

        if( SortedList_length(sortedList) < 0 )
        {
            fprintf(stderr, "invalid list, could not retrieve size \n");
            exit(2);
        }

        pthread_mutex_unlock(&mutex);
    }
    else if ( lock == SPINLOCK )
    {
        while(__sync_lock_test_and_set(&spinLock, 1))
        {
            // just wait for resource to be freed up
        }

        if( SortedList_length(sortedList) < 0 )
        {
            fprintf(stderr, "invalid list, could not retrieve size \n");
            exit(2);
        }

        __sync_lock_release(&spinLock);
    }

    //delete
    int j = thread;
    SortedListElement_t* removeElement;

    for(; j < numberOfElements; j += numberOfThreads)
    {
        if(lock == 'x')
        {
            removeElement = SortedList_lookup(sortedList, elementArray[j].key);
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
            pthread_mutex_lock(&mutex);

            removeElement = SortedList_lookup(sortedList, elementArray[j].key);
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
            pthread_mutex_unlock(&mutex);
        }
        else if ( lock == SPINLOCK )
        {
            while(__sync_lock_test_and_set(&spinLock, 1))
            {
                // just wait for resource to be freed up
            }

            removeElement = SortedList_lookup(sortedList, elementArray[j].key);
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
            __sync_lock_release(&spinLock);
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
            {"sync"      , required_argument, NULL, SYNC      }


    };

    int getoptReturnChar = -1;
    char optionType[30] = "list";

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

    if(lock == MUTEX)
    {
        pthread_mutex_init(&mutex, NULL);
    }

    numberOfElements = numberOfIterations * numberOfThreads;
    elementArray = malloc(numberOfElements*sizeof(SortedListElement_t));
    initializeRandKey(numberOfElements);

    pthread_t* pid = malloc(numberOfThreads*sizeof(pthread_t));
    int* pthreadNum = malloc(numberOfThreads * sizeof(int));

    struct timespec startTime;
    clock_gettime(CLOCK_MONOTONIC, &startTime);

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
    clock_gettime(CLOCK_MONOTONIC, &endTime);

    free(elementArray);
    free(pthreadNum);

    long long duration = (endTime.tv_sec - startTime.tv_sec) * 1000000000 + endTime.tv_nsec - startTime.tv_nsec;
    int operations = numberOfIterations * numberOfThreads * 3;
    int avgTimePerOperation = duration / operations;

    int numberOfLists = 1;

    printf("%s,%d,%d,%d,%d,%lld,%d\n",optionType,numberOfThreads,numberOfIterations,numberOfLists,operations,duration,avgTimePerOperation);

    exit(0);
}


