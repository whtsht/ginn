#include "server.h"

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../core/logger.h"
#include "../http/request.h"
#include "../http/response.h"
#include "../http/route.h"

void send_recv(int acc, char hbuf[NI_MAXHOST], char sbuf[NI_MAXSERV]);

int server_socket(const char *portnm) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int errcode;
    struct addrinfo *res0;
    if ((errcode = getaddrinfo(NULL, portnm, &hints, &res0)) != 0) {
        logging(LOG_ERROR, "getaddrinfo: %s\n", gai_strerror(errcode));
        exit(EXIT_FAILURE);
    }

    char nbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
    if ((errcode = getnameinfo(res0->ai_addr, res0->ai_addrlen, nbuf,
                               sizeof(nbuf), sbuf, sizeof(sbuf),
                               NI_NUMERICHOST | NI_NUMERICSERV)) != 0) {
        logging(LOG_ERROR, "getnameinfo: %s\n", gai_strerror(errcode));
        freeaddrinfo(res0);
        exit(EXIT_FAILURE);
    }

    logging(LOG_DEBUG, "port = %s\n", sbuf);

    int soc;
    if ((soc = socket(res0->ai_family, res0->ai_socktype, res0->ai_protocol)) ==
        -1) {
        logging(LOG_ERROR, "socket: %s\n", strerror(errno));
        freeaddrinfo(res0);
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    int opt_len = sizeof(opt);
    if (setsockopt(soc, SOL_SOCKET, SO_REUSEADDR, &opt, opt_len) == -1) {
        logging(LOG_ERROR, "setsockopt: %s\n", strerror(errno));
        close(soc);
        freeaddrinfo(res0);
        exit(EXIT_FAILURE);
    }

    if (bind(soc, res0->ai_addr, res0->ai_addrlen) == -1) {
        logging(LOG_ERROR, "bind: %s\n", strerror(errno));
        close(soc);
        freeaddrinfo(res0);
        exit(EXIT_FAILURE);
    }

    if (listen(soc, SOMAXCONN) == -1) {
        logging(LOG_ERROR, "listen: %s\n", strerror(errno));
        close(soc);
        freeaddrinfo(res0);
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(res0);

    return soc;
}

void accept_loop(int soc) {
    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
    struct sockaddr_storage from;
    int acc;
    socklen_t len;
    for (;;) {
        len = (socklen_t)sizeof(from);
        if ((acc = accept(soc, (struct sockaddr *)&from, &len)) == -1) {
            if (errno != EINTR) {
                logging(LOG_ERROR, "accept: %s\n", strerror(errno));
            }
        } else {
            (void)getnameinfo((struct sockaddr *)&from, len, hbuf, sizeof(hbuf),
                              sbuf, sizeof(sbuf),
                              NI_NUMERICHOST | NI_NUMERICSERV);
            logging(LOG_INFO, "accept: %s:%s", hbuf, sbuf);

            send_recv(acc, hbuf, sbuf);

            (void)close(acc);
            acc = 0;
        }
    }
}

void send_recv(int acc, char hbuf[NI_MAXHOST], char sbuf[NI_MAXSERV]) {
    Parser *parser = parser_from_socket(http_lexer, acc);
    HTTPRequest *request = parse_http_request(parser);
    if (!request) {
        logging(LOG_INFO, "invalid request from %s:%s", hbuf, sbuf);
        return;
    } else {
        logging(LOG_DEBUG, "method: %d", request->method);
        logging(LOG_DEBUG, "url: %s", request->url);
        logging(LOG_DEBUG, "version: %s", request->version);
        for (size_t i = 0; i < request->header_length; i++) {
            logging(LOG_DEBUG, "header %ld: %s: %s", i,
                    request->headers[i].field, request->headers[i].value);
        }
    }

    HTTPResponse *response = route(request);
    if (!response) {
        logging(LOG_DEBUG, "failed to route");
    } else if (send_http_response(response, acc)) {
        logging(LOG_DEBUG, "failed to send http response");
    } else {
        logging(LOG_DEBUG, "send http response");
    }
}
