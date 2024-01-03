#include "response.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../core/logger.h"

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

static ssize_t send_(int fd, const void* buf, size_t n, int flag) {
#ifdef TEST
    return write(fd, buf, n);
#else
    return send(fd, buf, n, flag);
#endif
}

HTTPResponseResult send_http_response(HTTPResponse* response, int socket) {
    char* proto = response->protocol;
    if (!proto) proto = RESPONSE_PROTOCOL;
    if (send_(socket, proto, strlen(proto), 0) == -1) {
        return HRR_Failure;
    }

    char* status = status_to_string(response->status);
    if (send_(socket, status, strlen(status), 0) == -1) {
        return HRR_Failure;
    }

    long file_size = get_file_size(response->body.file);
    char content_length[200];
    /* output was truncated */
    if (200 <= snprintf(content_length, 200, "Content-Length: %ld\r\n\r\n",
                        file_size)) {
        return HRR_Failure;
    }
    if (send_(socket, content_length, strlen(content_length), 0) == -1) {
        return HRR_Failure;
    }

    char buf[1000];
    size_t cnt = 0;
    do {
        cnt = fread(buf, sizeof(char), 1000, response->body.file);
        if (send_(socket, buf, cnt, 0) == -1) {
            return HRR_Failure;
        }
    } while (cnt == 1000);

    return HRR_Success;
}
