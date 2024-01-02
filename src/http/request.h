#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include "../core/parser.h"
#include "header.h"

#define METHOD_MAX_LENGTH 10
#define URL_MAX_LENGTH 2048
#define VERSION_MAX_LENGTH 20

typedef enum {
    GETMethod,
    HEADMethod,
} HTTPRequestMethod;

typedef struct {
    HTTPRequestMethod method;
    char* url;
    char* version;
    HTTPHeader* headers;
    int header_length;
} HTTPRequest;

ParserStatus http_lexer(Parser* parser);
HTTPRequest* parse_http_request(int socket);

#endif
