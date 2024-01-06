#ifndef HTTP_HEADER_H
#define HTTP_HEADER_H

#define MAX_HEADER_FIELD 256
#define MAX_HEADER_VALUE 512
#define MAX_CLIENT_HEADERS 20

#include "../util/parser.h"

typedef struct {
    char* field;
    char* value;
} HTTPHeader;

ParserStatus parser_headers(Parser* parser, HTTPHeader** headers,
                            size_t* header_length);

void headers_free(HTTPHeader* headers, size_t length);

#endif
