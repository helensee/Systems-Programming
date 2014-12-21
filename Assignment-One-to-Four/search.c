/*
*Assignment 4
*Search
*By: Helen See and HyunMo Yang
*
*/

#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

struct WordSearch{		// contains all the words that user wants to search
	char* word;
	struct WordSearch *next;
};
typedef struct WordSearch* wsptr;

struct FileStore{		// we use this struct for both OR and AND
	char* filename;
	int count;
	struct FileStore *next;
};
typedef struct FileStore* fsor;


fsor or;

int isSameToken(char* token, wsptr head){
	wsptr checkToken = head;
//	printf("curr->word, %s\n", checkToken->word);	
	while(checkToken->next != NULL){
		
		if(checkToken->word == NULL){
			return 0;
		}
		if(checkToken->word[0] == '0'){
			return 0;
		}	

	//	printf("token: %s %d\n", token, strlen(token));	
	//	printf("word: %s %d\n", checkToken->word, strlen(checkToken->word));

		if(strcmp(token, checkToken->word) == 0){
			checkToken = NULL;
			return 1; //the token and user word are the same, return 1 
		}
		checkToken = checkToken->next;
	}
	
	if(checkToken->next == NULL){
		checkToken = NULL;
		return 0;

	}
	checkToken = NULL;	
	return 0;

}

int existingFile(char* token){
	fsor curr = or;	
	if(or == NULL){
		return 1;
	}


	while(curr->next != NULL){
		if(curr->filename == NULL){
			return 1;
		}
//		printf("the token %s\n", token);
//		printf("the filename %s\n", curr->filename);

		if(strcmp(token,curr->filename) == 0){
			curr->count++;
			curr = NULL;
			//the filename and token match so the file already exists
			return 1;
		}
		if(curr->next == NULL){
			return 0;
		}
		curr = curr->next;
	}
	if(curr->next == NULL){
		return 0;
	}
//	printf("the file exists\n");
	return 0;

}

//fsor
int SearchOR(wsptr head, FILE* file){
	char* linebuffer = (char*)malloc((sizeof(char)*300));
	char* token;
	fsor curr = or;
	int isToken = -1;
	int toStore = 0;
	int isExist = -1;
	int found = 0;
	//read line
	while(fgets(linebuffer, 300, file) != NULL){
		token = strtok(linebuffer, " \n");
		while(token != NULL){
		//	printf("token: %s\n", token);
			if(toStore == 1 && strcmp(token, "</list>") != 0){
				int temp = atoi(token);
			//	printf("token: %s and temp: %d %d\n", token, temp, isToken);
				//check if token is the occurence value
				if(temp == 0 && isToken == 1){	
				//	printf("\ntoken being stored: %s\n\n", token);
					//if not integer, copy values into curr->filename
					isExist = existingFile(token);
				//	printf("is exist = %d\n", isExist);
					if(isExist == 0){
						//if the file doesnt exist yet
						found = 1;
//						printf("token stored: %s\n", token);
						curr->filename = (char*)malloc(((strlen(token)+1)*sizeof(char)));
						curr->count = 1;
						strncpy(curr->filename, token, strlen(token)+1);
						curr->next = (fsor)calloc(1, sizeof(struct FileStore));
//						printf("the file stored is: %s\n", curr->filename);
						curr = curr->next;
						isExist = -1;
					}else{
					//	printf("file already exists\n");
					}
				}else if(isToken == 0){
				//	printf("not the token we are looking for\n");
				}else{
				//	printf("not stored\n");
				}
			}else if(toStore == 1 && strcmp(token, "</list>") == 0){
				//once all the file names have been stored and </list> is reached
				//we reset the indicator toStore to 0
			//	printf("you have reached first </list>\n");
				toStore = 0;
				isToken = -1;
				//there are two of these and they do the same thing
				//there is no damage if i remove one or the other
			}

			if(isToken == 0){
			//	printf("token being compared: %s\n", token);
				isToken = isSameToken(token, head);
			//	printf("isToken: %d\n", isToken);
			}

			if(strcmp(token, "<list>") == 0){
				//This is simply to check if it's the start of a new token
				//Indicators changed for different if statements
				toStore = 1;
				isToken = 0;
				
			}else if(strcmp(token, "</list>") == 0){
				toStore = 0;
				isToken = -1;
			}		
			
			token = strtok(NULL, " \n");
		}
	}	
	free(linebuffer);
	free(token);
	if(or->filename == NULL || found == 0){
		free(or);
		printf("There are no files matching word(s).\n");
		return 0;
	}
	free(curr);	
	return 1;
}

void printSO(){
	fsor curr = or;
//	printf("\nsearch OR result: \n");

	while(or != NULL && or->filename != NULL){
		printf("%s ", curr->filename);
		if(or->next == NULL){
			break;
		}
		or = or->next;
		free(curr->filename);
		free(curr);
		if(or != NULL){
			curr = or;
		}
	}
	printf("\n");
	
}

void printAND(int total){
	fsor curr = or;
	int prints = -1;
//	printf("search AND result: \n");
	while(or != NULL && or->filename != NULL){
		if(curr->count == total){
			printf("%s ", curr->filename);
			prints = 1;
		}
		if(or->next == NULL){
			break;
		}
		or = or->next;
		free(curr->filename);
		free(curr);
		if(or != NULL){
			curr = or;
		}
	}
	if(prints == -1){
		printf("There are no files matching word(s).");
	}
	printf("\n");
}


void usage(){
	printf("---------------- Usage ---------------\n");
	printf("sa <term> <term> ...   : search files containing ALL the terms\n");
	printf("so <term> <term> ...   : search files containing at least one of the terms\n");
	printf("q   : quit\n");
	printf("--------------------------------------\n");	
}

int duplicateCheck(wsptr head, char * token){
	wsptr curr = head;
	while(curr != NULL && curr->word != NULL){
		if(strcmp(curr->word, token) == 0){
			return 1;
		}
		curr = curr->next;
	}
	return 0;
}


int setWordSearch(FILE* pf){
	or = (fsor)calloc(1, sizeof(struct FileStore));
	int input = 0;
	char* buffer = (char*)malloc((sizeof(char)*1024));
	printf("Enter string: \n");
	fgets(buffer, 300, stdin);
//	printf("buffer: %s", buffer);
	
	if(isalpha(buffer[0]) == 0 ){
		free(buffer);
		free(or);
		printf("Wrong input\n");
		return 1;
	}
	wsptr head = (wsptr)calloc(1, sizeof(struct WordSearch));
	wsptr curr = head;

	fsor ORfileHead;

	char *token;
	token = strtok(buffer, " \n");	
	char* command = (char*)malloc(sizeof(token));
	
	//Check if first word is sa, so or q
	if(strcmp(token, "sa") == 0){
		memcpy(command, token, strlen(token)+1);
	}else if(strcmp(token, "so")== 0){
		memcpy(command, token, strlen(token)+1);
	}else if(strcmp(token, "q") == 0){
		printf("Exiting\n");
		free(or);
		free(head);
		free(command);
		free(buffer);
		return 0;
	}else{
		free(head);
		free(buffer);
		free(or);
		printf("Incorrect inputs: %s\n", command);
		free(command);
		usage();
		return 1;
	}
	

	token = strtok(NULL, " \n");
	if(token == NULL){
		free(head);
		free(command);
		free(buffer);
		free(or);
		printf("Missing words for search\n");
		usage();
		return 1;
	}

	while(token != NULL){
	//	printf("token %s \n", token);
		if(duplicateCheck(head, token) == 1){
			// duplicate token
		}else{
			curr->word = (char*)malloc(((strlen(token)+1)*sizeof(char)));
//			printf("token len %d\n", strlen(token));
			memcpy(curr->word, token, strlen(token)+1);
//			printf("curr->word: %s\n", curr->word);
			curr->next = (wsptr)calloc(1, sizeof(struct WordSearch));
			curr = curr->next;
		}
		token = strtok(NULL, " \n");
	}

	if(strcmp(command, "sa") == 0){
		int total;
		wsptr temp = head;
		for(total = 0; temp->next != NULL; total++){
			temp = temp->next;
		}
		int found = 0;
		found = SearchOR(head, pf);
		if(found == 1){
			printAND(total);
		}
	}else if(strcmp(command, "so") == 0){		
		int found = 0;
		found = SearchOR(head, pf);
		if(found == 1){
			printSO();
		}
	}

	/****free*****/
	curr = head;
//	printf("curr->next: %s\n", curr->next->word);
	while(curr->next != NULL || curr->word != NULL){
//		printf("curr->word to be freed: %s\n", curr->word);
		head = head->next;
		free(curr->word);
		free(curr);
		curr = head;
	}
	if(head != NULL){
		free(head->word);
		free(head);
	}
//	printf("command to be freed: %s\n", command);	
	free(command);
//	printf("bufffer to be freed: %s\n", buffer);
	free(buffer);
	return 1;
}

void usuage(){
	printf("---------------- Usage ---------------\n");
	printf("sa <term> <term> ...   : search files containing ALL the terms.\n");
	printf("so <term> <term> ...   : search files containing at least one of the terms.\n");
	printf("q   : quit\n");
	printf("--------------------------------------\n");	
}

int main(int argc, char** argv){
	if(argc != 2){
		printf("Incorrect amount of inputs.\n");
		return 0;
	}
	FILE* file;
	file = fopen(argv[1], "r");
	int fd = open(argv[1], O_RDONLY);
	if(fd == -1){
		printf("Error while opening the file.\n");
		return -1;
	}

	struct stat statbuffer;
	stat(argv[1], &statbuffer);
	if(S_ISDIR(statbuffer.st_mode)){
		printf("User input is a directory. Please input a file.\n");
		fclose(file);
		return -1;
	}
	
	// menu print
	printf("------------ SEARCH MENU -------------\n");
	printf("sa <term> <term> ...   : search files containing ALL the terms.\n");
	printf("so <term> <term> ...   : search files containing at least one of the terms.\n");
	printf("q   : quit\n");
	printf("--------------------------------------\n");


	while(setWordSearch(file) == 1){
		rewind(file);
	}

	fclose(file);
	printf("The search tool is gracefully shutting itself down.\n");
	return 0;	
}
