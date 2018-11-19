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
////void SortedList_insert(SortedList_t *list, SortedListElement_t *element)
////{
////    SortedListElement_t* next = list->next;
////    while(next->key != NULL && strcmp(next->key, element->key) < 0)
////        next = next->next;
////
////    next->prev->next = element; //link up prev's next pointer
////    element->prev = next->prev; //connect element's prev to prev element in list
////    if(opt_yield & INSERT_YIELD)
////        sched_yield();
////    //link up element's next pointer
////    element->next = next;
////    next->prev = element; //reassign next's pre vpointer
//
////    if( list == NULL || element == NULL )
////    {
////        fprintf(stderr, "list or element are invalid");
////        return;
////    }
////
////
////    SortedList_t * p = list->next;
////
////    while( p->key != NULL && (strcmp(p->key, element->key) < 0))
////    {
////        p = p->next;
////    }
////
////    element->next = p;
////    element->prev = p->prev;
////    p->prev->next = element;
////    p->prev = element;
////
////    return;
//
////}

//void SortedList_insert(SortedList_t *list, SortedListElement_t *element) {
//    SortedListElement_t* next = list->next;
//    while(next->key != NULL && strcmp(next->key, element->key) < 0)
//        next = next->next;
//
//    next->prev->next = element; //link up prev's next pointer
//    element->prev = next->prev; //connect element's prev to prev element in list
//   /* if(opt_yield & INSERT_YIELD)
//        sched_yield();*/
//    //link up element's next pointer
//    element->next = next;
//    next->prev = element; //reassign next's pre vpointer
//}


void SortedList_insert(SortedList_t *list, SortedListElement_t *element) {
    SortedListElement_t* next = list->next;
    while(next->key != NULL && strcmp(next->key, element->key) < 0)
        next = next->next;

    next->prev->next = element; //link up prev's next pointer
    element->prev = next->prev; //connect element's prev to prev element in list
    if(opt_yield & INSERT_YIELD)
        sched_yield();
    //link up element's next pointer
    element->next = next;
    next->prev = element; //reassign next's pre vpointer
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
//int SortedList_delete( SortedListElement_t *element)
//{
//    SortedList_t *p;
//
//    if( element == NULL || element->prev->next != element || element->next->prev != element)
//    {
//        fprintf(stderr, "element is invalid");
//        return 1;
//    }
//    else
//    {
//        if(opt_yield & DELETE_YIELD)
//        {
//            sched_yield();
//        }
//        element->next->prev = element->prev;
//        element->prev->next = element->next;
//        return 0;
//    }
//
//}
int SortedList_delete( SortedListElement_t *element){
    if(element == NULL || element->next->prev != element->prev->next) return 1;
    if(opt_yield & DELETE_YIELD)
        sched_yield();
    element->prev->next = element->next;
    element->next->prev = element->prev;
    return 0;
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
//SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key)
//{
//    SortedList_t* retPointer;
//
//    retPointer = NULL;
//    SortedList_t* p;
//
//    if( list == NULL || key == NULL)
//    {
//        fprintf(stderr, "list or element are invalid");
//        retPointer = NULL;
//    }
//    else
//    {
//        for( p = list; p->next != list; p = p->next )
//        {
//            if( strcmp(p->key, key) == 0 ) // they are the same
//            {
//                retPointer = p;
//                break;
//            }
//
//            if(opt_yield & LOOKUP_YIELD)
//            {
//                sched_yield();
//            }
//        }
//        if( strcmp(p->key, key) == 0 ) // they are the same
//        {
//            retPointer = p;
//        }
//
//        if(opt_yield & LOOKUP_YIELD)
//        {
//            sched_yield();
//        }
//    }
//
//
//    return retPointer;
//}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key){
    if( key == NULL || list == NULL )
    {
        return NULL;
    }

    SortedListElement_t *curr = list->next;

    while( curr != list )
    {
        if(strcmp(curr->key, key) == 0)
        {
            return curr;
        }
        if( opt_yield & LOOKUP_YIELD)
        {
            sched_yield();
        }
        curr = curr->next;
    }
    return NULL;
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
//int SortedList_length(SortedList_t *list)
//{
//    SortedList_t* p;
//    int count = 0;
//
//    for( p = list; p->next != list; p = p->next )
//    {
//        if(p->next->prev != p || p->prev->next != p)
//        {
//            count = -1;
//            break;
//        }
//        else
//        {
//            count++;
//            if( opt_yield & LOOKUP_YIELD )
//            {
//                sched_yield();
//            }
//        }
//    }
//
//    return count;
//}
int SortedList_length(SortedList_t *list){
    if(list == NULL) return -1; //indicates corruption
    int count = 0;
    SortedListElement_t *next = list->next;
    while(next != list){
        //verify ptrs
//        if(next->prev->next != next || next->next->prev != next)
//            return -1;
        ++count;
        if(opt_yield & LOOKUP_YIELD)
            sched_yield();
        next=next->next;
    }
    return count;
}

//int main(){
//	SortedList_t* list;
//	SortedListElement_t element[5];
//	int i = 0;
//	char val;
//	for (; i < 5; i++){
//	    val = 'z' - i;
//		element[i].key = &val;
//		//SortedList_insert(list, &element[i]);
//	}
//
//	char c = 'a';
//	SortedListElement_t elem;
//	elem.key = &c;
//	SortedList_insert(list, &elem);
//	SortedList_delete(&elem);
//	SortedList_lookup(list, &val);
//	return 0;
//}


