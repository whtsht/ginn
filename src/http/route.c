#include "route.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../core/config.h"
#include "../core/default.h"
#include "../core/streq.h"
#include "response.h"

static HTTPResponse* ok_response(FILE* fp) {
    HTTPResponse* response = malloc(sizeof(HTTPResponse));
    response->status = HS_OK;
    response->protocol = NULL;
    response->headers = NULL;
    response->header_length = get_file_size(fp);
    response->body = (HTTPResponseBody){.file = fp};
    return response;
}

static HTTPResponse* not_found_response() {
    char* filepath = calloc(sizeof(char), FILE_PATH_LENGTH);
    strcat(filepath, CONFIG.root);
    strcat(filepath, default_error_pages(404));
    FILE* fp = fopen(filepath, "r");
    if (fp) {
        HTTPResponse* response = malloc(sizeof(HTTPResponse));
        response->status = HS_NotFound;
        response->protocol = NULL;
        response->headers = NULL;
        response->header_length = get_file_size(fp);
        response->body = (HTTPResponseBody){.file = fp};

        return response;
    }
    return NULL;
}

HTTPResponse* route(HTTPRequest* request) {
    char* filepath = calloc(sizeof(char), FILE_PATH_LENGTH);
    if (streq(request->url, "/")) {
        strcat(filepath, CONFIG.root);
        strcat(filepath, "/");
        strcat(filepath, CONFIG.index);
        FILE* fp = fopen(filepath, "r");
        if (fp) {
            free(filepath);
            return ok_response(fp);
        }
    }

    strcat(filepath, CONFIG.root);
    strcat(filepath, request->url);
    FILE* fp = fopen(filepath, "r");
    if (fp) {
        free(filepath);
        return ok_response(fp);
    }

    free(filepath);

    return not_found_response();
}
