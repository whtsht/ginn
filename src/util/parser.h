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
    PR_Success,
    PR_Failure,
    PR_EndOfContent,
} ParserResult;

struct Parser {
    ParserResult (*lexer)(struct Parser* parser);
    ParserResult (*next)(ParserData* data);
    ParserResult (*current)(char* c, ParserData* data);
    ParserData data;
    ParserType type;
};

typedef struct Parser Parser;

Parser* parser_from_string(ParserResult (*lexer)(struct Parser* parser),
                           char* string);
Parser* parser_from_file(ParserResult (*lexer)(struct Parser* parser),
                         FILE* file);
Parser* parser_from_socket(ParserResult (*lexer)(struct Parser* parser),
                           int socket);
ParserResult parser_next(Parser* parser);
ParserResult parser_current(Parser* parser, char* c);
ParserResult parser_char(Parser* parser, char c);
ParserResult parser_string(Parser* parser, char* s);
ParserResult parser_word(Parser* parser, int (*separator)(char), char* word,
                         size_t max_word_length);
void parser_free(Parser* parser);

#endif
