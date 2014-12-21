/*
*Assignment 4
*Search
*By: Helen See and HyunMo Yang
*
*/

#include "tokenizer.h"
#include "sorted-list.h"
#include "sorted-token.h"
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>  
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
// #include <errno.h>

#define BUFFER_SIZE 500

// global variable
SortedTokenPtr tokenlist;


struct stack		// for storing directory names as we go in deeper
{
	char * name;
	struct stack * prev;
	int depth;	// for checking only
};
typedef struct stack * stackptr;

struct input
{
	int fd;
	int length;
	int used;
	int total;
//	char* buffer;
	char buffer[BUFFER_SIZE];
};
/*
void StackDestroy(stackptr ptr){
	stackptr temp = ptr;
	while(temp->prev != NULL){
		ptr = ptr->prev;
		free(temp->name);
		free(temp);
		temp = ptr;
	}
}
*/
void resetinput(struct input * iptr, int fd)
{
	iptr->fd = fd;
	iptr->used = 0;
	iptr->length = 0;
	iptr->total = 1;
//	iptr->buffer = (char*) malloc(1024*sizeof(char));
	memset(iptr->buffer, 0, sizeof(iptr->buffer));
}


int read_input(struct input * iptr)
{
	char c;
/*
	if((int)strlen(iptr->buffer)+500 < iptr->total){
		bigger(iptr);
	}
*/
	if(iptr->used < iptr->length)
	{
		c = iptr->buffer[iptr->used];
		iptr->used += 1;
		iptr->total += 1;
//		printf("length is %d  and used is %d\n", iptr->length, iptr->used);
		return c;		// increase
	} else if((iptr->length = read(iptr->fd, iptr->buffer, sizeof(iptr->buffer))) < 1)
	{
		int i;
//		printf("here\n");
		for(i = 0; i < iptr->total; i++)		// to-lower-case
		{
			if(isupper(iptr->buffer[i]) != 0)
			{
				// to lower case
				iptr->buffer[i] = tolower(iptr->buffer[i]);
			}
//			printf("Buffer %d is %c\n", i, iptr->buffer[i]);
		}
		iptr->buffer[iptr->total-1] = '\0';		// deletes EOF (\n)

		return -1;		// when EOF or error
	} else
	{
//		printf("length is %d  and used is %d\n", iptr->length, iptr->used);
		c = iptr->buffer[0];
		iptr->used = 1;
//		printf("first\n");
		return c;		// first character
	}
}

void processFile(char *pf, char *path){ //pf = processing file
	FILE *file;
	file = fopen(pf, "r");
	int fd = open(pf, O_RDONLY);
//	printf("process file\n");	
	if(fd == -1){
		// error while opening the file
		printf("error while opening the file\n");
	}

	struct input *index = (struct input*) malloc(sizeof(struct input));
	resetinput(index, fd);
	while(1){
		if(read_input(index) == -1){
			break;
		}
	} //while file is not EOF
	
	char * delimiters = " !@~`#\"$%^&*()_+-=\\|][{}';:/.,<>?";		// all the delimiters

	TokenizerT * tokenizer = TKCreate(delimiters, index->buffer);		// tokenize
	
	char * token;		// a token
	// filename is MALLOC'ed
	char * filename;
	if(strcmp(pf, path) == 0){
		filename = (char *) malloc((sizeof(char)*strlen(pf))+1);
		strcpy(filename, pf);
	}else{
		filename = (char *) malloc((sizeof(char)*strlen(path)) + (sizeof(char)*strlen(pf)) + (sizeof(char))+1);
		strcpy(filename, path);
		strcat(filename, "/");
		strcat(filename, pf);
	}
	if(tokenlist == NULL){
//		printf("STCreate\n");
		if((token = TKGetToken(tokenizer)) == NULL){
			// no token
//			printf("no token\n");
			return;
		}
		tokenlist = STCreate(token, filename);	// first file
//		printf("create %s\n", token);
	}else{
//		printf("STInsert\n");
		if((token = TKGetToken(tokenizer)) == NULL){
			// no token
//			printf("no token\n");
			return;
		}
		tokenlist = STInsert(tokenlist, token, filename);
//		printf("insert %s\n", token);
	}
	while((token = TKGetNextToken(tokenizer)) != NULL)	// get next tokens
	{
//		printf("adding: %s\n", token);
		tokenlist = STInsert(tokenlist, token, filename);
//		printf("%s\n", token);
	}
//	printf("index free\n");
	TKDestroy(tokenizer);
//	free(index->buffer);
	free(index);
//	printf("close file\n");
	fclose(file);
	return;
}



void processDir(char *path, stackptr prev){
	DIR *dirp; //directory path
	struct dirent *entInfo;
	
	dirp = opendir(path);
//	printf("processing dir\n");
	if(dirp != NULL){
		
//		printf("directory not null\n");
		chdir(path);
		while((entInfo = readdir(dirp)) != NULL){	
			if(!strstr(entInfo->d_name, ".")){
//				printf("directory name is %s\n", entInfo->d_name);
				stackptr top = (stackptr) malloc(sizeof(struct stack));
				top->prev = prev;
				char * temp = (char*)malloc((sizeof(char)*strlen(prev->name)) + (sizeof(char)) + (sizeof(char)*strlen(entInfo->d_name))+1);
				strcpy(temp, prev->name);
				strcat(temp, "/");
				strcat(temp, entInfo->d_name);
				top->name = temp;
				top->depth = top->prev->depth + 1;
//				printf("depth %d, path %s\n", top->depth, top->name);
				processDir(entInfo->d_name, top);
				free(temp);
				free(top);
		//	}else if(strcmp(entInfo->d_name, ".") == 0 || strcmp(entInfo->d_name, "..") == 0){
		//		continue;
			}else{
//				printf("here is the file %s\n", entInfo->d_name);
//				printf("the file is at %s\n", prev->name);
				processFile(entInfo->d_name, prev->name);
			}	
		}
	}
	chdir("..");
//	printf("closing dir\n");
	closedir(dirp);
	free(entInfo);
	return;
}


int writeFile(char* filename){
	char option = 'y';
	FILE * file;

	SortedTokenIteratorPtr stip = STCreateIterator(tokenlist);
	SortedListIteratorPtr slip;
	SortedListPtr temp;
	char * tk;
	int record = 0;		// not exceeding 5
	if((file = fopen(filename, "r"))){
		printf("The file %s exists, do you want to overwrite it?\n (y or n): ", filename);
		scanf("%c", &option);
	}

	if(option == 'y' || option == 'Y'){
		file = fopen(filename, "w+");
		// write
		fprintf(file, "%s %s\n", "<list>", stip->itr->token);
		slip = SLCreateIterator(stip->itr->filenames);
//		printf("create iterator for slip\n");

		while((temp = SLGetItem(slip)) != NULL){
//			printf("writing %s\n", temp->filename);
			if(record == 5){
				record = 0;
				fprintf(file, "\n");
			}
			record += 1;
			fprintf(file, "%s %d ", temp->filename, temp->count);
			if(SLNextItem(slip) == NULL){
				break;
			}
		}
		record = 0;
		fprintf(file, "\n</list>\n");
		// end of first token.

		SLDestroyIterator(slip);
		while(stip->itr->next != NULL){
			tk = STNextToken(stip);
			fprintf(file, "%s %s\n", "<list>", tk);
			slip = SLCreateIterator(stip->itr->filenames);
			while((temp = SLGetItem(slip)) != NULL){
//				printf("writing %s\n", temp->filename);
				if(record == 5){
					record = 0;
					fprintf(file, "\n");
				}
				record += 1;
				fprintf(file, "%s %d ", temp->filename, temp->count);
				if(SLNextItem(slip) == NULL){
					break;
				}
			}
			record = 0;
			fprintf(file, "\n</list>\n");
			// end of next token
			SLDestroyIterator(slip);
		}
		temp = NULL;
	} else {
		printf("The file has not been overwritten. Exit.\n");
		STDestroyIterator(stip);
		return 0;
	}
	fclose(file);
	// free stuff
	STDestroyIterator(stip);

	return 1;

}

int 
main(int argc, char ** argv)
{

	if(argc != 3)
	{
		printf("you must have 2 arguments. \n");
		printf("./index <writefile.txt> <readfile.txt>\n");
	}

//	printf("start\n");	
	struct stat statbuffer;
	stat(argv[2],&statbuffer);
	stackptr head;
	if(S_ISDIR(statbuffer.st_mode))
	{
		if(access(argv[2], F_OK) == 0){
//			printf("You have access to the directory\n");
			head = (stackptr) malloc(sizeof(struct stack));
			head->depth = 1;
			char *temp = (char*)malloc(sizeof(argv[2]));
			strcpy(temp, argv[2]);
			head->name = temp;
			head->prev = NULL;	// it is at the bottom
			processDir(argv[2], head);
			free(temp);
			free(head);
		}else{
			// no access
			printf("You do not have permission to access this folder\n");
			return 0;
		}
	} else if(S_ISREG(statbuffer.st_mode))
	{
		// when index is an existing file
//		printf("file name is : %s\n", argv[2]);
		if(access(argv[2], R_OK) == -1)
		{
			// no access
			printf("You do not have permission to access this file\n");
			return 0;
		}else{
			// do have access
			processFile(argv[2], argv[2]);
		}
	} else
	{
		// fail.
		printf("An error occured. Please check if the directory/path exists.\n");
		return 0;
	}
	
	// check if empty
	if(tokenlist == NULL){
		printf("list is empty // nothing is stored, so nothing to write.\n");
		return 1;
	}


	// sort list
	
	SortedTokenIteratorPtr iter = STCreateIterator(tokenlist);
	// sort List by count.
//	printf("sort for %s\n", (char*)STGetToken(iter));
	iter->itr->filenames = SLSort(iter->itr->filenames);
	while((iter->itr = iter->itr->next) != NULL){
//		printf("sort for %s \n", (char*)STGetToken(iter));
		iter->itr->filenames = SLSort(iter->itr->filenames);
	}
	free(iter);

	
	// write stuff
	int itworks = 0;
	
	if((itworks = writeFile(argv[1])) == 1){
//		printf("writing success\n");
	}else{
//		printf("writing failed\n");
	}
	
	// FREE... the worst enemy
	STDestroy(tokenlist);
//	StackDestroy(head);

	if(itworks == 1){	
		printf("Your file has been written as %s.\n", argv[1]);
	}
	return 1;
}


