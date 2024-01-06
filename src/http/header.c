#include "header.h"

#include <stdlib.h>
#include <string.h>

static int colon(char c) { return c == ':'; }
static int carriage_return(char c) { return c == '\r'; }

ParserResult parser_headers(Parser* parser, HTTPHeader** headers,
                            size_t* header_length) {
    int client_headers = 0;
    HTTPHeader* tmp_headers = malloc(sizeof(HTTPHeader) * MAX_CLIENT_HEADERS);

    while (parser_char(parser, '\r') == PR_Failure) {
        /* Header field */
        char field[MAX_HEADER_FIELD];
        if (parser_word(parser, colon, field, MAX_HEADER_FIELD) != PR_Success) {
            headers_free(tmp_headers, client_headers);
            return PR_Failure;
        }

        /* Colon */
        if (parser_char(parser, ':') != PR_Success) {
            headers_free(tmp_headers, client_headers);
            return PR_Failure;
        }

        /* Header value */
        char value[MAX_HEADER_VALUE];
        if (parser_word(parser, carriage_return, value, MAX_HEADER_VALUE) !=
            PR_Success) {
            headers_free(tmp_headers, client_headers);
            return PR_Failure;
        }

        /* Newline */
        if (parser_string(parser, "\r\n") != PR_Success) {
            headers_free(tmp_headers, client_headers);
            return PR_Failure;
        }

        HTTPHeader header;
        header.field = strdup(field);
        header.value = strdup(value);

        tmp_headers[client_headers] = header;
        client_headers += 1;
        if (client_headers > MAX_CLIENT_HEADERS) {
            headers_free(tmp_headers, client_headers);
            return PR_Failure;
        }
    }

    /* Newline */
    char a;
    if (parser_current(parser, &a) != PR_Success || a != '\n') {
        headers_free(tmp_headers, client_headers);
        return PR_Failure;
    }

    *header_length = client_headers;
    *headers = malloc(sizeof(HTTPHeader) * client_headers);
    memcpy(*headers, tmp_headers, sizeof(HTTPHeader) * client_headers);
    free(tmp_headers);

    return PR_Success;
}

static void header_free(HTTPHeader* header) {
    free(header->field);
    free(header->value);
}

void headers_free(HTTPHeader* headers, size_t length) {
    for (size_t i = 0; i < length; i++) {
        header_free(&headers[i]);
    }
    free(headers);
}
