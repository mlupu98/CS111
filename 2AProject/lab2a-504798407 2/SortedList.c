#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include "SortedList.h"

/**
 * SortedList_insert ... insert an element into a sorted list
 *
 *	The specified element will be inserted in to
 *	the specified list, which will be kept sorted
 *	in ascending order based on associated keys
 *
 * @param SortedList_t *list ... header for the list
 * @param SortedListElement_t *element ... element to be added to the list
 */


void SortedList_insert(SortedList_t *list, SortedListElement_t *element)
{
    SortedListElement_t* p = list->next;

    while(p->key != NULL && (strcmp(p->key, element->key) < 0))
    {
        p = p->next;
    }

    if(opt_yield & INSERT_YIELD)
        sched_yield();

    element->prev = p->prev;
    element->next = p;
    p->prev->next = element;
    p->prev = element;

    return;
}
///**
// * SortedList_delete ... remove an element from a sorted list
// *
// *	The specified element will be removed from whatever
// *	list it is currently in.
// *
// *	Before doing the deletion, we check to make sure that
// *	next->prev and prev->next both point to this node
// *
// * @param SortedListElement_t *element ... element to be removed
// *
// * @return 0: element deleted successfully, 1: corrtuped prev/next pointers
// *
// */
//
int SortedList_delete( SortedListElement_t *element)
{
    int retVal;

    if( element == NULL || element->prev->next != element || element->next->prev != element)
    {
        fprintf(stderr, "element is invalid");
        retVal = 1;
    }
    else
    {
        if(opt_yield & DELETE_YIELD)
        {
            sched_yield();
        }
        element->next->prev = element->prev;
        element->prev->next = element->next;
        retVal = 0;
    }

    return retVal;

}

///**
// * SortedList_lookup ... search sorted list for a key
// *
// *	The specified list will be searched for an
// *	element with the specified key.
// *
// * @param SortedList_t *list ... header for the list
// * @param const char * key ... the desired key
// *
// * @return pointer to matching element, or NULL if none is found
// */

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key){

    SortedListElement_t *retVal = NULL;
    SortedListElement_t *p = list->next;

    if( key == NULL || list == NULL )
    {
        //do nothing already NULL
    }
    else
    {
        while( p != list )
        {
            if(strcmp(p->key, key) == 0)
            {
                retVal = p;
                break;
            }
            if( opt_yield & LOOKUP_YIELD)
            {
                sched_yield();
            }
            p = p->next;
        }
    }
    return retVal;
}
///**
// * SortedList_length ... count elements in a sorted list
// *	While enumeratign list, it checks all prev/next pointers
// *
// * @param SortedList_t *list ... header for the list
// *
// * @return int number of elements in list (excluding head)
// *	   -1 if the list is corrupted
// */
int SortedList_length(SortedList_t *list)
{
    SortedList_t* p;
    int count = 0;

    for( p = list; p->next != list; p = p->next )
    {
        if(p->next->prev != p || p->prev->next != p)
        {
            count = -1;
            break;
        }
        else
        {
            count++;
            if( opt_yield & LOOKUP_YIELD )
            {
                sched_yield();
            }
        }
    }
    return count;
}



