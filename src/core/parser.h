#ifndef PARSER_H
#define PARSER_H

#define MAX_WORD_LENGTH 1000

typedef struct {
    char* str;
    int size;
    int cur;
} Parser;

extern char P_WORD[MAX_WORD_LENGTH];

Parser* parser_new(char* str);
void parser_free(Parser* parser);

char parser_char(Parser* parser, char c);
int parser_word(Parser* parser);

#endif
