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
    response->header_length = 0;
    response->body = (HTTPResponseBody){.file = fp};
    return response;
}

static HTTPResponse* not_found_response() {
    char filepath[FILE_PATH_LENGTH] = "";
    strcat(filepath, CONFIG.root);
    char* error_page = get_error_page(404);
    if (error_page) {
        strcat(filepath, error_page);
    } else {
        strcat(filepath, server_error_page());
    }
    FILE* fp = fopen(filepath, "r");
    if (fp) {
        HTTPResponse* response = malloc(sizeof(HTTPResponse));
        response->status = HS_NotFound;
        response->protocol = NULL;
        response->headers = NULL;
        response->header_length = 0;
        response->body = (HTTPResponseBody){.file = fp};

        return response;
    }
    return NULL;
}

static void root_path(char* filepath) {
    strcpy(filepath, "");
    strcat(filepath, CONFIG.root);
    strcat(filepath, "/");
    strcat(filepath, CONFIG.index);
}

static void path(char* filepath, char* path) {
    strcpy(filepath, "");
    strcat(filepath, CONFIG.root);
    strcat(filepath, path);
}

HTTPResponse* route(HTTPRequest* request) {
    char filepath[FILE_PATH_LENGTH] = "";
    FILE* fp;

    if (streq(request->url, "/")) {
        root_path(filepath);
        if ((fp = fopen(filepath, "r"))) {
            return ok_response(fp);
        }
    }

    path(filepath, request->url);
    if ((fp = fopen(filepath, "r"))) {
        return ok_response(fp);
    }

    return not_found_response();
}
