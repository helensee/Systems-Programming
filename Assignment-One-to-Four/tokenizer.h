/*
*For Assignment 4
*Search
*Provided by Professor
*
*/

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct TokenizerT_{
	char * delimiters;
	char * copied_string;
	char * current_position;
};
typedef struct TokenizerT_ TokenizerT;

char is_escape_character(char character);

int char_to_hex(char character);
 
int char_to_oct(char character);
 
int is_oct_digit(char oct_digit);
 
char* unescape_string(char* string);

TokenizerT *TKCreate(char *separators, char *ts);

void TKDestroy(TokenizerT *tk);

char is_delimiter(char character, char* delimiters);

char *TKGetNextToken(TokenizerT *tk);

char *TKGetToken(TokenizerT *tk);

#endif
