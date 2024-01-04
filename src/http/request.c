#include "request.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"

ParserStatus http_lexer(Parser* parser) {
    char n;
    while (parser_current(parser, &n) == PS_Success && n == ' ') {
        parser_next(parser);
    }
    return parser_current(parser, &n);
}

static int separator(char c) { return c == ' ' || c == '\r' || c == '\n'; }

HTTPRequest* parse_http_request(Parser* parser) {
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

    HTTPHeader* headers = NULL;
    size_t header_length = 0;
    if (parser_headers(parser, &headers, &header_length) != PS_Success) {
        return NULL;
    }

    HTTPRequest* request = malloc(sizeof(HTTPRequest));
    request->method = method;
    request->url = strdup(url);
    request->version = strdup(version);
    request->headers = headers;
    request->header_length = header_length;

    return request;
}

void request_free(HTTPRequest* request) {
    free(request->url);
    free(request->version);
    headers_free(request->headers, request->header_length);
    free(request);
}

char* method_to_string(HTTPRequestMethod method) {
    switch (method) {
        case GETMethod:
            return "GET";
        case HEADMethod:
            return "HEAD";
    }

    /* unreachable */
    return NULL;
}
