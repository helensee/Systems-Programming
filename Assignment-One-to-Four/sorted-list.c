/*
*Assignment 2
*SortedList
*By: Helen See and HyunMo Yang
*
*/

#ifndef SORTED_LIST_C
#define SORTED_LIST_C
/*
 * define Sorted-List.c
 */

/*
 * this is list of files and counts of a word
 */
 
#include "sorted-list.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


SortedListPtr SLCreate(char * name)	// Sorted List, create
{
	SortedListPtr slp;
	slp = (SortedListPtr) malloc(sizeof(struct SortedList));
	slp->filename = name;
	slp->count = 1;
	slp->next = NULL;
	slp->prev = NULL;
	return slp;
};


void SLDestroy(SortedListPtr list)	// Sorted List, destroy
{
	struct SortedList * temp;
//	char * filetemp;
	while(list != NULL){
//		printf("SL free\n");
		temp = list;
		list = list->next;
		if(temp != NULL){
			if(temp->filename != NULL){
//				printf("temp->filename is not NULL\n");
				
			//	filetemp = temp->filename;
			//	free(filetemp);
			//	filetemp = NULL;
			}
			if(temp != NULL){
//				printf("SLtemp is not NULL\n");
				free(temp);
				temp = NULL;
			}
		}
	}
	return;
};


SortedListPtr SLInsert(SortedListPtr list, char *newFile)	// Sorted List, add item
{
	// check if the token is from the same file
	if(strcmp(list->filename, newFile) == 0){		// it is --> increase the count
		list->count += 1;
		// sort

		if(list->prev == NULL){		// the list is the head
//			printf("returning1: %s %d\n", list->filename, list->count);
			return list;
		}else{
//			printf("returning2: %s %d\n", list->filename, list->count);
			return list->prev;
		}
	}else{		// it is not
		if(list->next == NULL){		// last of the list
			// token is from new file
			SortedListPtr new = SLCreate(newFile);
			// connect
			new->prev = list;
			list->next = new;
			if(list->prev == NULL){		// the list is the head
//				printf("returning3: %s %d\n", list->filename, list->count);
				return list;
			}else{
//				printf("returning4: %s %d\n", list->filename, list->count);
				return list->prev;
			}
		}else{
			// keep searching
			SortedListPtr temp = SLInsert(list->next, newFile);		// recurrsive
			while(temp->prev != NULL){
				temp = temp->prev;
			}
			return temp;
		}
	}

};

SortedListPtr SLSort(SortedListPtr list)		// using Bubble sort O(n^2)
{
	// sort by COUNT.
	SortedListPtr head = list;
	SortedListPtr temp;
	int a;
	int b;
	int check = 1;	// >0 is not done, 0 is done
	SortedListIteratorPtr iter = SLCreateIterator(list);
	// get the size of Sorted List
	while(SLNextItem(iter) != NULL){
		check++;
	}
	free(iter);
	int i;
	while(check != 0){
		iter = SLCreateIterator(head);
		for(i = 1; i <= check; i++){
			if(SLNextItem(iter) != NULL){
				a = SLGetItem(iter)->count;	// second
				b = SLGetItem(iter)->prev->count;	// first
//				printf("comparing a: %s and b: %s\n", SLGetItem(iter)->filename, SLGetItem(iter)->prev->filename);
//				printf("a: %d  and b: %d\n", a, b);
				if(a > b){
					// change
					temp = SLGetItem(iter)->prev;
					if(SLGetItem(iter)->next == NULL){
						temp->next = NULL;
					}else{
						temp->next = SLGetItem(iter)->next;
						SortedListPtr nxt = temp->next;
						nxt->prev = temp;
					}
					if(temp->prev == NULL){
						// temp was the old head, and iter->itr is the new head
						head = SLGetItem(iter);
						head->prev = NULL;
						temp->prev = head;
						head->next = temp;
					}else{
						SortedListPtr prv = temp->prev;
						SLGetItem(iter)->prev = prv;
						prv->next = SLGetItem(iter);
						temp->prev = SLGetItem(iter);
						SLGetItem(iter)->next = temp;
					}
				}
			}
		}
//		printf("count is %d\n", check);
		check--;
		free(iter);
	}
	
	return head;
};


SortedListIteratorPtr SLCreateIterator(SortedListPtr list)	// Iterator, create
{
	if(list == NULL)	// no list passed
	{
		return NULL;
	}
	SortedListIteratorPtr slip = (SortedListIteratorPtr) malloc(sizeof(struct SortedListIterator));	// make iterator
	slip->itr = list;
	return slip;
};


void SLDestroyIterator(SortedListIteratorPtr iter)	// Iterator, destroy
{
	free(iter);
	iter = NULL;
	return;
};

SortedListPtr SLGetItem( SortedListIteratorPtr iter )	// Iterator, get current item
{
	if(iter->itr == NULL)
	{
		return NULL;
	}
	return iter->itr;
};


SortedListPtr SLNextItem(SortedListIteratorPtr iter)	// Iterator, get next item
{
	if(iter->itr->next == NULL)
	{
		return NULL;
	}
	iter->itr = iter->itr->next;
	return iter->itr;
};

#endif
