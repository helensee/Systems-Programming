/*
*Assignment 3
*Indexer
*By: Helen See and HyunMo Yang
*
*/

#ifndef SORTED_TOKEN_C
#define SORTED_TOKEN_C
/*
 * define Sorted-Token.c
 */

/*
 * this is list of files and counts of a word
 */
 
#include "sorted-token.h"
#include "sorted-list.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

SortedTokenPtr STCreate(char * tk, char * filename)	// Sorted Token, create
{
	SortedTokenPtr stp;
	stp = (SortedTokenPtr) malloc(sizeof(struct SortedToken));
	stp->token = tk;
	stp->next = NULL;
	stp->prev = NULL;
	stp->filenames = SLCreate(filename);
	return stp;
};


void STDestroy(SortedTokenPtr list)	// Sorted Token, destroy
{
	struct SortedToken * temp;
	while(list != NULL){
//		printf("ST free\n");
		temp = list;
		SLDestroy(list->filenames);
		list = list->next;
		if(temp->token != NULL){
			free(temp->token);
		}
		if(temp != NULL){
//			printf("temp is not NULL\n");
			free(temp);
			temp = NULL;
		}
	}
	return;
};


SortedTokenPtr STInsert(SortedTokenPtr list, char *newToken, char * filename)	// Insert Token and return the head
{
//	printf("comepareing: %s and %s \n", list->token, newToken);
	// check if new token is already stored
	if(strcmp(list->token, newToken) == 0){		// it is already stored.
		// check if it is from the same file
//		printf("call SLInsert\n");
		list->filenames = SLInsert(list->filenames, filename);		// insert into SortedList  --> call SLInsert
		if(list->prev == NULL){		// list is the head
//			printf("returning1 %s\n", list->token);
			return list;
		}else{
//			printf("returning2 %s\n", list->prev->token);
			return list->prev;
		}
	}else if(strcmp(list->token, newToken) < 0){		// newToken is bigger
		if(list->next == NULL){		// reached the end --> didn't find newToken --> create
//			printf("call STCreate\n");
			SortedTokenPtr new = STCreate(newToken, filename);
			// add after list
			new->prev = list;
			list->next = new;
			if(list->prev == NULL){		// is the head
//				printf("returning3 %s\n", list->token);
				return list;
			}else{		// not the head
//				printf("returning4 %s\n", list->prev->token);
				return list->prev;
			}
			
		}else{		// check next token
//			printf("call STInsert again\n");
			SortedTokenPtr temp = STInsert(list->next, newToken, filename);		// recurrsive
			if(temp->prev == NULL){		// is the head
//				printf("returning5 %s\n", list->token);
				return temp;
			}else{		// not the head
//				printf("returning6 %s\n", list->token);
				return temp->prev;
			}
		}
	}else{		// newToken is smaller
		// add new Token before the list
//		printf("call STCreate\n");
		SortedTokenPtr new = STCreate(newToken, filename);	
		if(list->prev == NULL){		// list is the head
			new->next = list;
			new->prev = NULL;		// new is now the head
			list->prev = new;
//			printf("returning7 %s\n", new->token);
			return new;
		}else{		// adding before list and list->prev
			new->next = list;
			new->prev = list->prev;
			SortedTokenPtr temp = list->prev;
			temp->next = new;
			list->prev = new;
//			printf("returning8 %s\n", new->prev->token);
			return new->prev;
		}
		
	}
	
	// shouldn't reach.
	return NULL;
		

};


SortedTokenIteratorPtr STCreateIterator(SortedTokenPtr list)	// Iterator, create
{
	if(list == NULL)	// no list passed
	{
		return NULL;
	}
	SortedTokenIteratorPtr stip = (SortedTokenIteratorPtr) malloc(sizeof(struct SortedTokenIterator));	// make iterator
	stip->itr = list;
	return stip;
};


void STDestroyIterator(SortedTokenIteratorPtr iter)	// Iterator, destroy
{
	free(iter);
	iter = NULL;
	return;
};


char * STGetToken( SortedTokenIteratorPtr iter )	// Iterator, get current item
{
	if(iter->itr == NULL)
	{
		return NULL;
	}
	return iter->itr->token;
};


char * STNextToken(SortedTokenIteratorPtr iter)	// Iterator, get next item
{
	if(iter->itr->next == NULL)
	{
		return NULL;
	}
	iter->itr = iter->itr->next;
	return iter->itr->token;
};

#endif
