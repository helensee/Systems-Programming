/*
*
*Multi-threading
*By: Helen See and HyunMo Yang
*
*/

/*
#include <malloc.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdlib.h>
#include <fcntl.h>
*/
#include "data.h"
//--- STRUCT -----
//



//--- GLOBAL VARIABLES-----
//

//struct queue * orderQ;
//dbptr dbStore; //for main database
//sdbptr dbshare; //shared data with mutex


//catPtr catStore; //linked list holding categories
//static int catCount; //count of categories

//place producer queue and prod shared data here



//------------------------------------------------------

int main(int argc, char **argv){

	if(argc != 4){
		printf("You must have 3 arguments.\n");
		printf("./order <database.txt> <orders.txt> <categories.txt>\n");
		return -1;
	}

	FILE *databaseFile;
	databaseFile = fopen(argv[1], "r");
	if(databaseFile == NULL){
		printf("Failed at reading database file, please try again.\n");
	}
	FILE *categoryFile;
	categoryFile = fopen(argv[3], "r");
	if(categoryFile == NULL){
		printf("Failed at reading category file, please try again.\n");
		return 0;
	}
	catCount = readCat(categoryFile);
	if(catCount <=0){
		printf("The file containing categories is empty, please try again\n");
		return 0;
	}
	
	fseek(databaseFile, 0, SEEK_END);
	int check = ftell(databaseFile);
//	printf("check = %d\n", check);
	if(check == 0){
		printf("The database file is empty, please try again.\n");
		return 0;
	}
	
	fseek(databaseFile, 0, SEEK_SET);
	buildDatabase(databaseFile);

	char *			orders = argv[2];
	struct SharedData * 	sharedData;
	struct SharedQueue *	SharedQ = (struct SharedQueue *) calloc(1, sizeof(struct SharedQueue));
	FILE * 			orderfile = fopen(orders, "r");
	if(orderfile == NULL){
		printf("failed at reading order file\n");
	}
	int i = 0;

//	pthread_t catThread[catCount];
	
	//set up categories
//	catCount = readCat(categoryFile);
//	printf("catcount is %d\n", catCount);
	catPtr terp = catStore;
	while(terp->next != NULL){
//		printf("cat is: %s\n",terp->catStr);
		terp = terp->next;
	}
	pthread_t catThread[catCount];
	
//	buildDatabase(databaseFile);
	init_DB_Shared();

	fclose(databaseFile);
	fclose(categoryFile);
	
//-------------------------------------------------------------------------------------------------
	sharedData = init_Shared_Data(SharedQ, orderfile);
	
	queue_init(SharedQ);	
//	printf("q init\n");	
	pthread_t prod;

	pthread_create(&prod, 0, producer, (void *)sharedData);
			
//----------categories threads---------------->
	int * cats[catCount];
	sleep(1);
	catPtr curr = catStore;
	for(i=0; i < catCount; i++){
		if(pthread_create(&catThread[i], NULL, consumer, sharedData) != 0){
			return 1;
		}
		curr->catID = catThread[i];
	//	printf("catID: %d and catThread[ID]: %d\n", (int)curr->catID, (int)catThread[i]);
		curr = curr->next;
	}

//-------------Print results-------------------------->
	for(i = 0; i < catCount; i++){
		pthread_join(catThread[i], (void**)&(cats[i]));
	}
	sleep(5);	
// THIS IS THE PROBLEM. program will PRINT before all the thread finishes. got to FIX!!
	printf("\n\n\nAll the threads have finished.\nFile finalreport.txt created.\n\n\n\n");
	printEverything();
// free
	catFree();
	sharedFree(sharedData);

	pthread_exit(0);
	return 0;
}
