#include "request.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ParserStatus http_lexer(Parser* parser) {
    char n;
    while (parser_current(parser, &n) == PS_Success && n == ' ') {
        parser_next(parser);
    }
    return parser_current(parser, &n);
}

static int separator(char c) { return c == ' ' || c == '\r' || c == '\n'; }

HTTPRequest* parse_http_request(int socket) {
    Parser* parser = parser_from_socket(http_lexer, socket);

    /* Method */
    char method_str[METHOD_MAX_LENGTH];
    if (parser_word(parser, separator, method_str, METHOD_MAX_LENGTH) !=
        PS_Success) {
        return NULL;
    }
    HTTPRequestMethod method;
    if (strcmp(method_str, "GET") == 0) {
        method = GETMethod;
    } else if (strcmp(method_str, "HEAD") == 0) {
        method = HEADMethod;
    } else {
        return NULL;
    }

    /* Url */
    char url[URL_MAX_LENGTH] = "";
    if (parser_word(parser, separator, url, URL_MAX_LENGTH) != PS_Success) {
        return NULL;
    }

    /* Version */
    char version[VERSION_MAX_LENGTH] = "";
    if (parser_word(parser, separator, version, VERSION_MAX_LENGTH) !=
        PS_Success) {
        return NULL;
    }

    /* Newline */
    if (parser_string(parser, "\r\n") != PS_Success) {
        return NULL;
    }

    HTTPRequest* request = malloc(sizeof(HTTPRequest));
    request->method = method;
    request->url = calloc(sizeof(char), strlen(url) + 1);
    strcpy(request->url, url);
    request->version = calloc(sizeof(char), strlen(version) + 1);
    strcpy(request->version, version);

    return request;
}
