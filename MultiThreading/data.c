/*
*
*Multi-threading
*By: Helen See and HyunMo Yang
*
*/

#include "data.h"
#define QSIZE 5

//global variable
struct queue * orderQ;
dbptr dbStore; //for main database
sdbptr dbshare; //shared data with mutex


catPtr catStore; //linked list holding categories
static int catCount; //count of categories


void init_DB_Shared(){
	dbshare = (sdbptr)calloc(1, sizeof(struct SharedDB));
	dbshare->isopen = 1;
	pthread_mutex_init(&dbshare->mutex, 0);
	dbshare->data = dbStore;
	return;
}

void buildDatabase(FILE *dbFile){
//	int count = 0;
	char* buffer = (char*)malloc((sizeof(char)*200));
	char* token;
//	printf("building db\n");
	dbStore = (dbptr)calloc(1, sizeof(struct Database));
	dbptr curr = dbStore;
	while(fgets(buffer, 200, dbFile) != NULL){
		if(strlen(buffer) < 2){
			break;
		
		}
		token = strtok(buffer, "|\"");
		if(isalnum(token[0]) == 0){
			break;
		}	
		curr->name = (char*)malloc((sizeof(char)*(strlen(token)+1)));
		memcpy(curr->name, token, (strlen(token)+1));
		token = strtok(NULL, "|\"");
			
		curr->userID = atoi(token);
		token = strtok(NULL, "|");
					
		curr->balance = atoi(token);
		token = strtok(NULL, "|\"");
			
		curr->street = (char*)malloc((sizeof(char)*(strlen(token)+1)));
		memcpy(curr->street, token, (strlen(token)+1));
		token = strtok(NULL, "|\"");

		curr->state = (char*)malloc((sizeof(char)*(strlen(token)+1)));
		memcpy(curr->state, token, (strlen(token)+1));
		token = strtok(NULL, "|\"");

		curr->zip = (char*)malloc((sizeof(char)*(strlen(token)+1)));
		memcpy(curr->zip, token, (strlen(token)+1));
		token = strtok(NULL, "|\"");
		token = NULL;

		curr->next = (dbptr)calloc(1, sizeof(struct Database));
		curr = curr->next;
	}
	if(buffer == NULL && dbStore == NULL){
		printf("EMPTY\n");

	}
	free(buffer);
	return ;
}


int readCat(FILE *categories){
	//reads filestream
	//stores value into a category structure
	int count = 0;
	char* buffer = (char*)malloc((sizeof(char)*200));
	catStore = (catPtr)calloc(1, sizeof(struct Category));
	catPtr curr = catStore;
	while(fgets(buffer, 200, categories) != NULL){
		if(isalnum(buffer[0]) == 0){
			break;
		}
		curr->catStr = (char*)malloc(((strlen(buffer)+1)*sizeof(char)));
		memcpy(curr->catStr, buffer, (strlen(buffer)+1));
		curr->catStr[strlen(buffer)-1] = '\0';	
		curr->next = (catPtr)calloc(1, sizeof(struct Category));
		curr = curr->next;
		free(buffer);
		buffer = (char*)malloc((sizeof(char)*200));
		count++;
	} //gets all categories
	free(buffer);
	catCount = count;	
	return count;
}

struct queue * getOrder(char * input){
	struct queue *		book = (struct queue *) calloc(1, sizeof(struct queue));
	char *			token = strtok(input, "|");
	book->title = (char *) malloc(sizeof(char) * (1 + strlen(token)));
	strncpy(book->title, token, strlen(token) + 1);
	token = strtok(NULL, "|");
	book->price = strtod(token, NULL);
	token = strtok(NULL, "|");
	book->id = atoi(token);
	token = strtok(NULL, "|");
	book->category = (char *) malloc(sizeof(char) * (1 + strlen(token)));
	strncpy(book->category, token, strlen(token) +1);
	book->category[strlen(book->category)-1] = '\0';
	return book;
}

void queue_init(struct SharedQueue * qptr){ //Initializing shared queue
	qptr->isopen = 1;
//	qptr->fp = fp;
	qptr->size = QSIZE;
	qptr->finished = 0;
	qptr->count = 0;
	qptr->queue = NULL;
	pthread_mutex_init(&qptr->mutex, 0);
	pthread_cond_init(&qptr->dataAvailable, 0);
	pthread_cond_init(&qptr->spaceAvailable, 0);
}

struct SharedData * init_Shared_Data(struct SharedQueue * sq, FILE * fp){ //Initializing shared data to contain shared queue and shared database
	struct SharedData * sd = (struct SharedData *)calloc(1, sizeof(struct SharedData));

	sd->fp = fp;
	sd->queue = sq;
	sd->database = dbshare;
	
	return sd;
}

void catFree(){
	catPtr curr = catStore;

	while(curr->next != NULL){
		if(curr->next == NULL){
			break;
		}
		catStore = catStore->next;
		
		free(curr->catStr);	
		free(curr);
		if(catStore == NULL){
			break;
		}
		curr = catStore;
		if(curr == NULL){
			break;
		}	
	//	printf("freeed %s\n", catStore->catStr);
		
	}	
	return;
}

struct queue * dequeue(){
	struct queue* returning = orderQ;
	if(orderQ == NULL){
		return NULL;
	}
	if(orderQ->next == NULL){
		orderQ = NULL;
		return returning;
	}
	orderQ = orderQ->next;
	returning->next = NULL;

	return returning;
}

void enqueue(struct queue * insert){
	if(orderQ == NULL){
		orderQ = insert;	
		return;	
	}

	struct queue *temp = orderQ;
	insert->next = temp;
	orderQ = insert;
	temp = NULL;
//	insert = NULL;
	
	return;	

}


void * producer(void * arg){
	struct SharedData *	Shared = (struct SharedData *) arg;
	struct SharedQueue *	SharedQ = Shared->queue;
	char *			linebuffer = (char *) malloc(sizeof(char) * 500);
	struct queue *		order;
	
	pthread_detach(pthread_self());
//	printf("let's start producing\n");
	if(SharedQ->finished == 1){
	//	printf("already finished?!\n");
	}
	
	while(fgets(linebuffer, 500, Shared->fp) != NULL){
//		printf("linebuffer is %s\n", linebuffer);
		if(strlen(linebuffer) < 2){
			// last line
//			SharedQ->finished = 1;			
//			printf("finished all the book orders\n");
			break;
		}
		order = getOrder(linebuffer);
		free(linebuffer);
		linebuffer = (char *) malloc(sizeof(char) * 500);
		while(pthread_mutex_trylock(&SharedQ->mutex) != 0){
			printf("Producer waits because shared queue is locked\n");
			sleep(1);
		}
		printf("Producer locked the shared queue\n");
		// locked
		// check for space
		while(SharedQ->count == SharedQ->size){
			// queue is full
			pthread_cond_signal(&SharedQ->dataAvailable);	// shout at consumer
			printf("Producer waits because shared queue is full\n");
			pthread_cond_wait(&SharedQ->spaceAvailable, &SharedQ->mutex);
		}
		// space available
		// printf("store order in the queue\n");
		if(orderQ == NULL){
			// queue is empty
			orderQ = order;
			SharedQ->queue = orderQ;
			SharedQ->count = 1;
//			printf("one order available\n");
		}else{
			struct queue * iter = orderQ;
			while(iter->next != NULL){
				iter = iter->next;
			}
			iter->next = order;
			SharedQ->count++;
//			printf("%d order available\n", SharedQ->count);
		}
		// printf("order is stored in the queue\n");
		pthread_cond_signal(&SharedQ->dataAvailable);
//		printf("producer is unlocking the shared queue\n");
		pthread_mutex_unlock(&SharedQ->mutex);
		sleep(1);
	}
	while(pthread_mutex_trylock(&SharedQ->mutex) != 0){
		printf("Producer finished, but waits because shared queue is locked\n");
		sleep(1);
	}
	SharedQ->finished = 1;
	free(linebuffer);
	pthread_cond_signal(&SharedQ->dataAvailable);
	pthread_mutex_unlock(&SharedQ->mutex);
	printf("Producer finished all the book orders\n");
	fclose(Shared->fp);
	return 0;
}


void *consumer(void* arg){
	char *cat = NULL;
	struct SharedData * sharedData = (struct SharedData*)arg;
	sdbptr sdb = sharedData->database;
	struct SharedQueue * sq = sharedData->queue;

	struct queue *po = NULL; //process order (only one node);
	pthread_t tid = pthread_self();	
	//find category for this thread
	catPtr curr = catStore;
	while(curr != NULL){
		if((int)curr->catID ==(int)tid){
//			printf("compare: curr->catid %d with tid: %d\n", (int)curr->catID, (int)tid);
			cat = (char *)malloc(strlen(curr->catStr)*(sizeof(char)+1));
			memcpy(cat, curr->catStr, strlen(curr->catStr)+1);
			break;
		} 
		curr = curr->next;
	}

	printf("Consumer %s thread is made\n", cat);
	curr = NULL;
//	printf("I am category: %s %d\n", cat, (int)tid);
	if(cat == NULL){
	//	printf("cat is null, BEGONE!\n");
		return 0;
	}
	pthread_detach(pthread_self());

	while(sq->finished == 0 || orderQ != NULL){ //going to have different checks
	//	pthread_mutex_lock(&con->mutex);
		while(po == NULL){
			if(pthread_mutex_trylock(&sq->mutex) == 0){
				//if the mutex is currently unlock, do this code
			//	printf("locked shared queue mutex from %s\n", cat);	
				if(orderQ == NULL && sq->finished == 0){ //the shared queue is empty
					pthread_cond_signal(&sq->spaceAvailable);
				//	printf("Consumer %s waits because queue is empty\n", cat);
					pthread_cond_wait(&sq->dataAvailable, &sq->mutex);
				}
				po = dequeue();
				if(po == NULL && sq->finished == 1){
					pthread_mutex_unlock(&sq->mutex);
					break;
				}
				sq->count--;
				if(strcmp(po->category, cat) != 0){
					//Compare categories to decide if that's the node this thread needs
					enqueue(po);
					sq->count++;
					po = NULL;
				}
				pthread_mutex_unlock(&sq->mutex);
				sleep(1);
			}
			else{
				printf("Consumer %s waits because shared queue is locked\n", cat);
				sleep(1);
			}
		}
	
		double newBalance = 0;

		while(po != NULL){
			if(pthread_mutex_trylock(&sdb->mutex) == 0){ 
				//Thread has locked shared database
				printf("Consumer %s locked the database\n", cat);	
				dbptr dbiter = sdb->data;
				while(dbiter->next != NULL){
					if(dbiter->userID == po->id){
						if((newBalance = (dbiter->balance) - (po->price)) >= 0){
							// enough balance, SUCCESSFUL ORDERS
							dbiter->balance = newBalance;
							if(dbiter->acceptNext == NULL){
								dbiter->acceptNext = (aptr) calloc(1, sizeof(struct ProcessedOrder));
								dbiter->acceptNext->cost = po->price;
								dbiter->acceptNext->balance_after = newBalance;
								dbiter->acceptNext->title = (char *) malloc(sizeof(char) * (strlen(po->title)+1));
								strncpy(dbiter->acceptNext->title, po->title, strlen(po->title) + 1);
								dbiter->acceptNext->category = (char *) malloc(sizeof(char) * (strlen(po->category) + 1));
								strncpy(dbiter->acceptNext->category, po->category, strlen(po->category) + 1);
							}else{
								aptr curr = dbiter->acceptNext;
								while(curr->next != NULL){
									curr = curr->next;
								}
								curr->next = (aptr) calloc(1, sizeof(struct ProcessedOrder));
								curr = curr->next;
								curr->cost = po->price;
								curr->balance_after = newBalance;
								curr->title = (char *) malloc(sizeof(char) * (strlen(po->title) +1));
								strncpy(curr->title, po->title, strlen(po->title) + 1);
								curr->category = (char *) malloc(sizeof(char) * (strlen(po->category) +1));
								strncpy(curr->category, po->category, strlen(po->category + 1));
							}
						}else{
							// not enough balance, UNSUCESSFUL ORDERS
							if(dbiter->rejectNext == NULL){
								dbiter->rejectNext = (aptr) calloc(1, sizeof(struct ProcessedOrder));
								dbiter->rejectNext->cost = po->price;
								dbiter->rejectNext->balance_after = dbiter->balance;
								dbiter->rejectNext->title = (char *) malloc(sizeof(char) * (strlen(po->title)+1));
								strncpy(dbiter->rejectNext->title, po->title, strlen(po->title) + 1);
								dbiter->rejectNext->category = (char *) malloc(sizeof(char) * (strlen(po->category) + 1));
								strncpy(dbiter->rejectNext->category, po->category, strlen(po->category) + 1);
							}else{
								rptr curr = dbiter->rejectNext;
								while(curr->next != NULL){
									curr = curr->next;
								}
								curr->next = (aptr) calloc(1, sizeof(struct ProcessedOrder));
								curr = curr->next;
								curr->cost = po->price;
								curr->balance_after = dbiter->balance;
								curr->title = (char *) malloc(sizeof(char) * (strlen(po->title) +1));
								strncpy(curr->title, po->title, strlen(po->title) + 1);
								curr->category = (char *) malloc(sizeof(char) * (strlen(po->category) +1));
								strncpy(curr->category, po->category, strlen(po->category + 1));
							}
						}	
					}
					dbiter = dbiter->next;
				}
				free(po->title);
				free(po->category);
				free(po);
				po = NULL;	
				
				pthread_mutex_unlock(&sdb->mutex);
				sleep(1);
			}
			else{
				sleep(1);
				printf("Consumer %s waits because shared database is locked\n", cat);
			}
		}

	}

	//if shared data isnt open/available anymore
	//destroy thread from within
	printf("Thread %s is done\n", cat);
	free(cat);	
	
	return NULL;
}


void printEverything(){
	FILE *fp = fopen("finalreport.txt", "w+");
	
	
	struct Database * curr = dbStore;
	while(curr->next != NULL){
		printf("=== BEGIN CUSTOMER INFO ===\n");
		printf("### BALANCE ###\n");
		printf("Customer name: %s\n", curr->name);		
		fprintf(fp, "=== BEGIN CUSTOMER INFO ===\n");
		fprintf(fp, "### BALANCE ###\n");
		fprintf(fp, "Customer name: %s\n", curr->name);

		free(curr->name);

		printf("Customer ID number: %d\n", curr->userID);
		printf("Remaining credit balance after all purchases (a dollar amount): %.02f\n", curr->balance);
		fprintf(fp, "Customer ID number: %d\n", curr->userID);
		fprintf(fp, "Remaining credit balance after all purchases (a dollar amount): %.02f\n", curr->balance);
		aptr aiter = curr->acceptNext;

		printf("### SUCCESSFUL ORDERS ###\n");
		fprintf(fp, "### SUCCESSFUL ORDERS ###\n");

		while(aiter != NULL){
			printf("%s|%.02f|%.02f\n", aiter->title, aiter->cost, aiter->balance_after);
			fprintf(fp, "%s|%.02f|%.02f\n", aiter->title, aiter->cost, aiter->balance_after);
			free(aiter->title);
			free(aiter->category);
			if(aiter->next == NULL){
				free(aiter);
				break;
			}
			curr->acceptNext = aiter->next;
			free(aiter);
			aiter = curr->acceptNext;
		}
		rptr riter = curr->rejectNext;
		printf("### REJECTED ORDERS ###\n");
		fprintf(fp, "### REJECTED ORDERS ###\n");		
		while(riter != NULL){
			printf("%s|%.02f\n", riter->title, riter->cost);
			fprintf(fp, "%s|%.02f\n", riter->title, riter->cost);
			free(riter->title);
			free(riter->category);
			if(riter->next == NULL){
				free(riter);
				break;
			}
			curr->rejectNext = riter->next;
			free(riter);
			riter = curr->rejectNext;
		}
		printf("=== END CUSTOMER INFO ===\n\n");
		fprintf(fp, "=== END CUSTOMER INFO ===\n\n");
		free(curr->street);
		free(curr->state);
		free(curr->zip);
		dbStore = dbStore->next;
		free(curr);
		curr = dbStore;
	}

	fclose(fp);
	
}

void sharedFree(struct SharedData* sd){
//	pthread_mutex_destory(&dbshare->mutex);
	free(dbshare);
	free(sd->queue);
	free(sd);
	return;

}
