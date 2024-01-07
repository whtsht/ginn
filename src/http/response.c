#include "response.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../util/logger.h"
#include "header.h"

static char* status_to_string(HTTPStatus status) {
    switch (status) {
        case HS_OK:
            return " 200 OK\r\n";
        case HS_NotFound:
            return " 404 Not Found\r\n";
        case HS_InternalServerError:
            return " 500 Internal Server Error\r\n";
        case HS_NotImplemented:
            return " 501 Not Implemented\r\n";
    }

    logging(LOG_DEBUG, "status_to_string: unreachable");
    exit(EXIT_FAILURE);
}

size_t get_file_size(FILE* fp) {
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    rewind(fp);
    return sz > 0 ? sz - 1 : sz;
}

HTTPResponseResult send_http_response(HTTPResponse* response, int socket) {
    char* proto = response->protocol;
    if (!proto) proto = RESPONSE_PROTOCOL;
    if (write(socket, proto, strlen(proto)) == -1) {
        return HRR_Failure;
    }

    char* status = status_to_string(response->status);
    if (write(socket, status, strlen(status)) == -1) {
        return HRR_Failure;
    }

    long file_size = get_file_size(response->body.file);
    char content_length[200];
    /* output was truncated */
    if (200 <= snprintf(content_length, 200, "Content-Length: %ld\r\n\r\n",
                        file_size)) {
        return HRR_Failure;
    }
    if (write(socket, content_length, strlen(content_length)) == -1) {
        return HRR_Failure;
    }

    char buf[1000];
    size_t cnt = 0;
    do {
        cnt = fread(buf, sizeof(char), 1000, response->body.file);
        if (write(socket, buf, cnt) == -1) {
            return HRR_Failure;
        }
    } while (cnt == 1000);

    return HRR_Success;
}

void response_free(HTTPResponse* response) {
    headers_free(response->headers, response->header_length);
    fclose(response->body.file);
    free(response);
}
