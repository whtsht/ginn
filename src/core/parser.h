#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

typedef struct {
    char* data;
    int size;
    int cur;
} String;

typedef union {
    String string;
    FILE* file;
    int socket;
} ParserData;

typedef enum {
    PS_Success,
    PS_Waiting,
    PS_Failure,
    PS_EndOfContent,
} ParserStatus;

struct Parser {
    ParserStatus (*lexer)(struct Parser* parser);
    ParserStatus (*next)(ParserData* data);
    ParserStatus (*current)(char* c, ParserData* data);
    ParserData data;
};

typedef struct Parser Parser;

Parser* parser_from_string(ParserStatus (*lexer)(struct Parser* parser),
                           char* string);
ParserStatus parser_next(Parser* parser);
ParserStatus parser_current(Parser* parser, char* c);
ParserStatus parser_char(Parser* parser, char c);
ParserStatus parser_word(Parser* parser, int (*separator)(char), char* word,
                         size_t max_word_length);
void parser_delete(Parser* parser);

#endif
