/*
*
*Malloc() and Free() error detections
*By: Helen See and HyunMo Yang
*
*/

#include "mymalloc.h"

void case1leak(){//multiple malloc and free with leaks
    
    void * one_first = malloc(100);
	void * one_second = malloc(300);
	void * one_third = malloc(200);
	void * one_fourth = calloc(1, 20);
	free(one_first);
	free(one_third);
}

void case2leak(){ //one malloc
    void * two_first = malloc(500);
    
}

void case3leak(){//no leaks
    char * three_first = (char*) calloc(3, 50);
    free(three_first);
    
}

void case4leak(){//no leaks, because free()ing non-exist pointer
    int i;
    free(&i);
}
void case5leak(){// in manyyyy blocks
    void * fifth_one = malloc(20);
    void * fifth_two = malloc(30);
    void * fifth_three = malloc(300);
    void * fifth_four = malloc(40);
    free(fifth_three);
    void * fifth_five = malloc(50);
    
}

void caseEnd(){
    printf(RED "Happy " BLUE "Holiday!\n");
    
}
int main(int argc, char*argv[]){
    
    if(argc < 2){
        printf("missing arguements");
    }
    forLeaks = 1;
    int casePick = atoi(argv[1]);
    printf(BOLDBLUE "Case %d\n" RESET, casePick);
    
    switch (casePick){
        case 1: case1leak(); break; //general malloc and free with leaks
        case 2: case2leak(); break; //No leaks
        case 3: case3leak(); break; //Check for double free
        case 4: case4leak(); break; //free()ing pointers that dont exist
        case 5: case5leak(); break; //manyyyy blocks leaked

        case 9: caseEnd(); break; //:D
        default: printf("Case number is not available.\n"); break;
    };
    
	return 1;
}
