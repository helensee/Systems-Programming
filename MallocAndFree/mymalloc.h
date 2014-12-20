/*
*
*Malloc() and Free() error detections
*By: Helen See and HyunMo Yang
*
*/

#ifndef MYMALLOC_H
#define MYMALLOC_H

#define RESET   "\033[0m"
#define RED     "\033[31m"      /* Red */
#define BLUE    "\033[34m"      /* Blue */

#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */

#include <stdlib.h>
#define malloc(x) (mymalloc(x , __FILE__, __LINE__))
#define calloc(x, y) (mycalloc(x , y , __FILE__, __LINE__))
#define free(x) (myfree(x , __FILE__, __LINE__))
#define atexit(x) (atexit(x))
#define sbrk(x) (sbrk(x))

#include <stdio.h>
#include <string.h>
#include <ctype.h>

extern int forLeaks;

struct mem{
	struct mem * prev;
	struct mem * next;
	int size;
	int isFree;
};

void * mymalloc(unsigned int size, const char * file, const int line);

void * mycalloc(unsigned int nitems, unsigned int size, const char * file, const int line);

void myfree(void * p, const char * file, const int line);

void leakCheck();


#endif