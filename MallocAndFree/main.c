/*
*
*Malloc() and Free() error detections
*By: Helen See and HyunMo Yang
*
*/

#include "mymalloc.h"

void case1(){
	// test case 1: general malloc and free
	void * one_first = malloc(5);
	char * one_second = (char *) malloc(10);
	free(one_first);
	free(one_second);
}

void case2(){
	// test case 2: combination of malloc and free
	void * two_first = malloc(50);
	free(two_first);
	two_first = malloc(100);
	free(two_first);
	char * two_second = (char *) malloc(20);
	free(two_second);
}

void case3(){
	// test case 3: redundant free()ing of the same pointer
	char * three_first = (char *) malloc(30);
	free(three_first);
	free(three_first);

}

void case4(){
	// test case 4: free()ing pointers to dynamic memory that were not returned from malloc()
	void * four_first = malloc(4);
	free(four_first + 2);
}

void case5(){
	// test case 5: fragmentation
	void * five_first = malloc(10);
	void * five_second = malloc(40);
	void * five_third = malloc(1000);
	void * five_fourth = malloc(100);
	free(five_first);
	five_first = malloc(30);
	free(five_first);
	free(five_second);
	free(five_fourth);
	five_first = malloc(3000);
	free(five_first);
	free(five_third);

}

void case6(){
	// test case 6: saturation
	void * six_first = malloc(1000);
	void * six_second = malloc(300);
	void * six_third = malloc(4000);
	free(six_first);
	free(six_second);
	free(six_third);

}

void case7(){
	// test case 7: valid double free
	void * seven_first = malloc(4000);
	free(seven_first);
	seven_first = malloc(4000);
	free(seven_first);
}

void case8(){
	// test case 8: free()ing pointers that were never allocated
	int x = 5;
	free(&x);

}

void case9(){
	//test case 9: checking for memory leaks
	void * nine_first = malloc(100);
	void * nine_second = malloc(300);
	void * nine_third = malloc(200);
	void * nine_fourth = malloc(20);
	free(nine_first);
	free(nine_third);

}

void case10(){
	// test case 10: calloc
	void * ten_first = calloc(4, 100*sizeof(int));
	void * ten_second = calloc(0, 50*sizeof(char));
	void * ten_third = malloc(1000);
	free(ten_first);
	free(ten_second);
	free(ten_third);
	ten_first = malloc(10);
	ten_second = malloc(20);
	free(ten_first);
	free(ten_second);
	
}

int main(int argc, char*argv[]){
    
    if(argc < 2){
        printf("Missing arguments: ./mymalloc <case number>");
    }
    int casePick = atoi(argv[1]);
    printf(BOLDBLUE "Case %d\n" RESET, casePick);
    
    switch (casePick){
        case 1: case1(); break; //general malloc and free
        case 2: case2(); break; //combination of malloc and free
        case 3: case3(); break; //Check for double free
        case 4: case4(); break; //free()ing pointers to dynamic memory that were not returned from malloc()
        case 5: case5(); break; //fragmentation
        case 6: case6(); break; //saturation
        case 7: case7(); break; //valid double free
        case 8: case8(); break; //free()ing pointers that were never allocated
        case 9: case9(); break; //Checking for leaks
        case 10: case10(); break; // calloc
        default: printf("Case number is not available.\n"); break;
    };
    
	return 1;
}
