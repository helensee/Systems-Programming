/*
*
*Multi-threading
*By: Helen See and HyunMo Yang
*
*/

#ifndef DATA_H
#define DATA_H

#include <malloc.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>


struct Category{
	char *catStr;
	pthread_t catID;
	struct Category *next;

};
typedef struct Category *catPtr;

struct ProcessedOrder{
	char* title;
	double cost;
	double balance_after;
	char* category;
	struct ProcessedOrder* next;
};
typedef struct ProcessedOrder* aptr;
typedef struct ProcessedOrder* rptr;


struct Database{
	int userID;
	char* name;
	double balance;
	char* street;
	char* state;
	char* zip;
	struct Database* next;
	struct ProcessedOrder * acceptNext;
	struct ProcessedOrder * rejectNext;
};

typedef struct Database* dbptr;

struct SharedDB{ //shared data that contains mutex for database
	int 		isopen;
	pthread_mutex_t mutex;
//	pthread_cond_t 	dataAvailable;
	struct Database * data;
};
typedef struct SharedDB* sdbptr;

struct SharedData{
	struct SharedQueue *	queue;
	struct SharedDB *	database;
//	struct Category *	
	FILE *			fp;
};

struct SharedQueue{
	pthread_mutex_t		mutex;
	pthread_cond_t		dataAvailable;
	pthread_cond_t		spaceAvailable;
//	FILE *			fp;
	int 			isopen;
	int			finished;
	int			size;
	int			count;
	struct queue *		queue;	
};

struct queue{
	char *			title;
	int			id;
	char *			category;
	double			price;
	struct queue *		next;
};



catPtr catStore; //linked list holding categories
static int catCount; //count of categories


void init_DB_Shared();

void buildDatabase(FILE *dbFILE);

int readCat(FILE *categories);

struct queue * getOrder(char * input);

void queue_init(struct SharedQueue * qptr);

struct SharedData * init_Shared_Data(struct SharedQueue * sq, FILE * fp);

void catFree();

void dbFree();

struct queue * dequeue();

void enqueue(struct queue * insert);

void *producer(void* arg);

void *consumer(void* arg);

void printEverything();

void sharedFree(struct SharedData* sd);

#endif
