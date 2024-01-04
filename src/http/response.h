#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <stdio.h>

#include "header.h"

#define RESPONSE_PROTOCOL "HTTP/1.1"

typedef enum {
    HS_OK = 200,
    HS_NotFound = 404,
    HS_InternalServerError = 500,
    HS_NotImplemented = 501
} HTTPStatus;

typedef struct {
    FILE* file;
} HTTPResponseBody;

typedef enum {
    HRR_Success,
    HRR_Failure,
} HTTPResponseResult;

typedef struct {
    HTTPStatus status;
    char* protocol;
    HTTPHeader* headers;
    int header_length;
    HTTPResponseBody body;
} HTTPResponse;

HTTPResponseResult send_http_response(HTTPResponse* response, int socket);
size_t get_file_size(FILE* fp);
void response_free(HTTPResponse* response);

#endif
