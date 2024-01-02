#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

typedef struct {
    char* data;
    int size;
    int cur;
} String;

typedef struct {
    FILE* data;
    // None => -1
    // Some(c) => c
    char cur;
} File;

typedef struct {
    int desc;
    // None => -1
    // Some(c) => c
    char cur;
} Socket;

typedef enum {
    ParserTypeString,
    ParserTypeFile,
    ParserTypeSocket,
} ParserType;

typedef union {
    String string;
    File file;
    Socket socket;
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
    ParserType type;
};

typedef struct Parser Parser;

Parser* parser_from_string(ParserStatus (*lexer)(struct Parser* parser),
                           char* string);
Parser* parser_from_file(ParserStatus (*lexer)(struct Parser* parser),
                         FILE* file);
Parser* parser_from_socket(ParserStatus (*lexer)(struct Parser* parser),
                           int socket);
ParserStatus parser_next(Parser* parser);
ParserStatus parser_current(Parser* parser, char* c);
ParserStatus parser_char(Parser* parser, char c);
ParserStatus parser_string(Parser* parser, char* s);
ParserStatus parser_word(Parser* parser, int (*separator)(char), char* word,
                         size_t max_word_length);
void parser_delete(Parser* parser);

#endif
