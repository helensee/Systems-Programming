/*
*Assignment 3
*Indexer
*By: Helen See and HyunMo Yang
*
*/

#ifndef SORTED_TOKEN_H
#define SORTED_TOKEN_H
/*
: * sorted-token.h
 */

#include <stdlib.h>

struct SortedToken
{
	char * token;	// token
	struct SortedList * filenames;		// list of files that contains the token
	struct SortedToken * prev;	// previous in list
	struct SortedToken * next;	// next in list
};
typedef struct SortedToken* SortedTokenPtr;

struct SortedTokenIterator
{
	SortedTokenPtr itr;	// iterator
};
typedef struct SortedTokenIterator* SortedTokenIteratorPtr;


SortedTokenPtr STCreate(char * token, char * filename);

void STDestroy(SortedTokenPtr list);

SortedTokenPtr STInsert(SortedTokenPtr list, char *newObj, char * filename);


SortedTokenIteratorPtr STCreateIterator(SortedTokenPtr list);


void STDestroyIterator(SortedTokenIteratorPtr iter);


char * STGetToken( SortedTokenIteratorPtr iter );


char * STNextToken(SortedTokenIteratorPtr iter);
#endif
