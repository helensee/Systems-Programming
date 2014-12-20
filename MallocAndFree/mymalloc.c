/*
*
*Malloc() and Free() error detections
*By: Helen See and HyunMo Yang
*
*/

#include "mymalloc.h"

#define BLOCKSIZE 5000
#define LARGEBLOCK 500

static char heap[BLOCKSIZE];
static int init; 
int forLeaks = 0;


void * mymalloc(unsigned int size, const char * file, const int line){
	static struct mem * root = 0;	// root of our array
	struct mem * ptr = 0;	// iterator
	struct mem * succ = 0;	// next iterator
//	static int init = 0;	// initialing
	void * result = NULL;	// what we return
	if(forLeaks == 1){
		atexit(leakCheck);
	}
	if(size == 0){
//		printf("Malloc Error: Nothing to malloc. FILE: %s LINE: %d\n", file, line);
		return NULL;
	}
	if(size < 0){
		printf("Malloc Error: Invalid value. FILE: %s LINE: %d\n", file, line);
		return NULL;
	}
	root = (struct mem *) heap;
	// initialize
	if(init == 0){
		root->prev = 0;
		root->next = 0;
		root->size = BLOCKSIZE - sizeof(struct mem);
		root->isFree = 1;	// is free
		init = 1;
	}

	ptr = root;
	// iterate
	do{
//		printf("ptr->size %d and size plus memsize %d\n",(int)ptr->size, (int)size+ (int)sizeof(struct mem));
//		printf("do while loop\n");
		if(ptr->size < size){
			// need more space
			ptr = ptr->next;
		}else if(ptr->isFree == 0){
			// not free
			ptr = ptr->next;
		}else if(ptr->size < (size + sizeof(struct mem))){
			// they ask for exact memory left
			ptr->isFree = 0;
			result = (void *)((char *)ptr + sizeof(struct mem));
	//		printf("malloc returns %d (everything) to FILE: %s LINE: %d\n", ptr->size, file, line);
			return result;
		}else{
			succ = (struct mem *)((char *)ptr + sizeof(struct mem) + size);
			succ->prev = ptr;
			succ->next = ptr->next;

			if(ptr->next != 0){
				ptr->next->prev = succ;
			}

			ptr->next = succ;
			succ->size = (ptr->size - sizeof(struct mem) - size);
			succ->isFree = 1;
			ptr->size = size;
			ptr->isFree = 0;
			result = (void*)((char *)ptr + sizeof(struct mem));
	//		printf("malloc returns %d to FILE: %s LINE: %d\n", ptr->size, file, line);
			return result;
		}
	} while(ptr != NULL || ptr != 0);

	// saturation
	ptr = root;
	int leftover = BLOCKSIZE;
	do{
		if(ptr->isFree == 0){
			leftover -= ptr->size;
			ptr = ptr->next;
		}else{
			ptr = ptr->next;
		}
	} while(ptr != NULL || ptr != 0);
	
//	printf("leftover is %d, and we try to allocate %d\n", leftover, size);
	if(leftover < size){
		// saturation
		printf("Error: Saturation at FILE: %s LINE: %d\n", file, line);
	}else{
		// fragmentation
		printf("Error: Fragmentation at FILE: %s LINE: %d\n", file, line);
	}
		
	printf("Error: Malloc failed at FILE: %s LINE: %d\n", file, line);

	return 0;
}

void * mycalloc(unsigned int nitems, unsigned int size, const char * file, const int line){
	//similar to mymalloc except need to initialize each space with 0
	static struct mem * root = 0;	// root of our array
	struct mem * ptr = 0;	// iterator
	struct mem * succ = 0;	// next iterator
//	static int init = 0;	// initialing
	void * result = NULL;	// what we return
	int i;
	char * temp;

	unsigned int totalsize = nitems * size;

	if(forLeaks == 1){
		atexit(leakCheck);
	}

	if(size == 0){
//		printf("Calloc Error: Nothing to calloc. FILE: %s LINE: %d\n", file, line);
		return NULL;
	}
	if(size < 0){
		printf("Calloc Error: Invalid value. FILE: %s LINE: %d\n", file, line);
		return NULL;
	}
	if(nitems == 0){
		return NULL;
	}
	root = (struct mem *) heap;
	// initialize
	if(init == 0){
		root->prev = 0;
		root->next = 0;
		root->size = BLOCKSIZE - sizeof(struct mem);
		root->isFree = 1;	// is free
		init = 1;
	}

	ptr = root;
	// iterate
	
	do{
//		printf("ptr->size %d and size plus memsize %d\n",(int)ptr->size, (int)totalsize+ (int)sizeof(struct mem));
//		printf("do while loop\n");
		if(ptr->size < totalsize){
			// need more space
			ptr = ptr->next;
		}else if(ptr->isFree == 0){
			// not free
			ptr = ptr->next;
		}else if(ptr->size < (totalsize + sizeof(struct mem))){
			// they ask for exact memory left
			ptr->isFree = 0;
			result = (void *)((char *)ptr + sizeof(struct mem));
	//		printf("malloc returns %d (everything) to FILE: %s LINE: %d\n", ptr->size, file, line);
			for(i = 0; i < totalsize; i++){		// fill up with zero
				temp = (char*)result + i;
				temp = 0;
				
			}
			return result;
		}else{
			succ = (struct mem *)((char *)ptr + sizeof(struct mem) + totalsize);
			succ->prev = ptr;
			succ->next = ptr->next;

			if(ptr->next != 0){
				ptr->next->prev = succ;
			}

			ptr->next = succ;
			succ->size = (ptr->size - sizeof(struct mem) - totalsize);
			succ->isFree = 1;
			ptr->size = totalsize;
			ptr->isFree = 0;
			result = (void*)((char *)ptr + sizeof(struct mem));
	//		printf("malloc returns %d to FILE: %s LINE: %d\n", ptr->size, file, line);
			for(i = 0; i < totalsize; i++){		// fill up with zero
				temp = (char*)result + i;
				temp = 0;
			}
			return result;
		}
	} while(ptr != NULL || ptr != 0);

	// saturation
	ptr = root;
	int leftover = BLOCKSIZE;
	do{
		if(ptr->isFree == 0){
			leftover -= ptr->size;
			ptr = ptr->next;
		}else{
			ptr = ptr->next;
		}
	} while(ptr != NULL || ptr != 0);
	
//	printf("leftover is %d, and we try to allocate %d many of %d\n", leftover, nitems, size);
	if(leftover < totalsize){
		// saturation
		sbrk(500);
		printf("Error: Saturation at FILE: %s LINE: %d\n", file, line);
	}else{
		// fragmentation
		printf("Error: Fragmentation at FILE: %s LINE: %d\n", file, line);
	}
		
	printf("Error: Calloc failed at FILE: %s LINE: %d\n", file, line);
	temp = 0;
	return (void *)temp;
}

void myfree(void * p, const char * file, const int line){
	struct mem *ptr = 0;
	struct mem *pred = 0;
	struct mem *succ = 0;

	if(p == NULL){
		// NULL pointer
		printf("Error: Cannot free NULL from FILE: %s LINE: %d\n", file, line);
		return;
	}
	ptr = (struct mem*)((char*)p - sizeof(struct mem));

//	printf("will free %d from FILE: %s LINE: %d\n", ptr->size, file, line);
	

	if((char*)ptr < (char*)heap || (char*)ptr > ((char*)heap + BLOCKSIZE)){
		// not malloc()ed pointer
		printf("Error: Cannot free pointer that has never been allocated from FILE: %s LINE: %d\n", file, line);
		return;
	}
	if(ptr->size == 0){
		// not the pointer malloc() returned
		printf("Error: Cannot free pointers, to dynamic memory, that were not returned from malloc() from FILE: %s LINE: %d\n", file, line);
		return;
	}
	
	if((pred = ptr->prev) != 0 && pred->isFree){
		//reclaiming memory chunks
		pred->size += (sizeof(struct mem) + ptr->size);
		pred->next = ptr->next;
		if(ptr->next != 0){
			ptr->next->prev = pred;
		}
	}else{
		if(ptr->isFree == 1){
			// double free() detected
			printf(RED "Error: Double free detected at FILE: %s LINE: %d\n" RESET, file, line);
			return;
		}else{
			ptr->isFree = 1;
			pred = ptr;
		}
	}

	if((succ = ptr->next) != 0 && succ->isFree){
		pred->size += (sizeof(struct mem) + succ->size);
		pred->next = succ->next;
		if(succ->next != 0){
			succ->next->prev = pred;
		}
	}

//	printf("myfree freed from FILE: %s LINE: %d\n", file, line);
	return;
}

void leakCheck(){
	static int checked = 0;
	static struct mem * root = 0;	// root of our array
	struct mem * ptr = 0;	// iterator
	if(checked == 0){
		checked = 1;
	}else{
		return;
	}
	root = (struct mem *) heap;

	ptr = root;
	int leftover = 0;
	int blocks = 0;
	do{
		if(ptr->isFree == 0){ //not free
			leftover += ptr->size;
			blocks++;
			ptr = ptr->next;
		}else{
			ptr = ptr->next;
		}
	} while(ptr != NULL || ptr != 0);
	
	if(leftover > 0){
		printf(RED "Leaked %d bytes in %d blocks\n" RESET, leftover, blocks); 
	}else{
		printf("No memory leaks\n");
	}
}
