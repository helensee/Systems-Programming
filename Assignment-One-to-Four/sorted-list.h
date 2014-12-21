/*
*Assignment 2
*SortedList
*By: Helen See and HyunMo Yang
*
*/

#ifndef SORTED_LIST_H
#define SORTED_LIST_H
/*
: * sorted-list.h
 */

#include <stdlib.h>

struct SortedList
{
	char * filename;	// filename
	int count;		// number of times a word is counted
	struct SortedList * prev;	// previous in list
	struct SortedList * next;	// next in list
};
typedef struct SortedList* SortedListPtr;

struct SortedListIterator
{
	SortedListPtr itr;	// iterator
};
typedef struct SortedListIterator* SortedListIteratorPtr;


SortedListPtr SLCreate(char * filename);

void SLDestroy(SortedListPtr list);

SortedListPtr SLInsert(SortedListPtr list, char *newObj);

SortedListPtr SLSort(SortedListPtr list);

int SLRemove(SortedListPtr list, char *newObj);


SortedListIteratorPtr SLCreateIterator(SortedListPtr list);


void SLDestroyIterator(SortedListIteratorPtr iter);


SortedListPtr SLGetItem( SortedListIteratorPtr iter );


SortedListPtr SLNextItem(SortedListIteratorPtr iter);

#endif
