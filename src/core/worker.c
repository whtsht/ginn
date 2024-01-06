#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../http/request.h"
#include "../http/response.h"
#include "../http/route.h"
#include "../util/logger.h"
#include "config.h"

int send_recv(int acc);

int server_socket(const char *portnm) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int errcode;
    struct addrinfo *res0;
    if ((errcode = getaddrinfo(NULL, portnm, &hints, &res0)) != 0) {
        logging(LOG_ERROR, "getaddrinfo: %s", gai_strerror(errcode));
        exit(EXIT_FAILURE);
    }

    char nbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
    if ((errcode = getnameinfo(res0->ai_addr, res0->ai_addrlen, nbuf,
                               sizeof(nbuf), sbuf, sizeof(sbuf),
                               NI_NUMERICHOST | NI_NUMERICSERV)) != 0) {
        logging(LOG_ERROR, "getnameinfo: %s", gai_strerror(errcode));
        freeaddrinfo(res0);
        exit(EXIT_FAILURE);
    }

    logging(LOG_DEBUG, "port = %s", sbuf);

    int soc;
    if ((soc = socket(res0->ai_family, res0->ai_socktype, res0->ai_protocol)) ==
        -1) {
        logging(LOG_ERROR, "socket: %s", strerror(errno));
        freeaddrinfo(res0);
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    int opt_len = sizeof(opt);
    if (setsockopt(soc, SOL_SOCKET, SO_REUSEPORT, &opt, opt_len) == -1) {
        logging(LOG_ERROR, "setsockopt: %s", strerror(errno));
        close(soc);
        freeaddrinfo(res0);
        exit(EXIT_FAILURE);
    }

    if (bind(soc, res0->ai_addr, res0->ai_addrlen) == -1) {
        logging(LOG_ERROR, "bind: %s", strerror(errno));
        close(soc);
        freeaddrinfo(res0);
        exit(EXIT_FAILURE);
    }

    if (listen(soc, SOMAXCONN) == -1) {
        logging(LOG_ERROR, "listen: %s", strerror(errno));
        close(soc);
        freeaddrinfo(res0);
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(res0);

    return soc;
}

static int accept_connection(int soc, int acc, struct sockaddr_storage from,
                             int epollfd, struct epoll_event ev, int count) {
    socklen_t len = sizeof(from);
    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
    if ((acc = accept(soc, (struct sockaddr *)&from, &len)) == -1) {
        if (errno != EINTR) {
            logging(LOG_ERROR, "accept: %s", strerror(errno));
        }
    } else {
        getnameinfo((struct sockaddr *)&from, len, hbuf, sizeof(hbuf), sbuf,
                    sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV);
        logging(LOG_INFO, "accept: %s:%s", hbuf, sbuf);

        if (count >= CONFIG.worker_connections) {
            logging(LOG_WARNING, "connection is full : cannot accept");
            close(acc);
        } else {
            ev.data.fd = acc;
            ev.events = EPOLLIN;
            if (epoll_ctl(epollfd, EPOLL_CTL_ADD, acc, &ev) == -1) {
                logging(LOG_ERROR, "epoll_ctl: %s", strerror(errno));
                close(acc);
                close(epollfd);
            } else {
                return 1;
            }
        }
    }

    return 0;
}

static void serve(int acc, int epollfd, struct epoll_event *ev) {
    if (send_recv(acc) == -1) {
        if (epoll_ctl(epollfd, EPOLL_CTL_DEL, acc, ev) == -1) {
            logging(LOG_ERROR, "epoll_ctl: %s", strerror(errno));
            close(acc);
            close(epollfd);
            exit(EXIT_FAILURE);
        }
    }
}

void accept_loop(int soc) {
    struct sockaddr_storage from;
    struct epoll_event events[CONFIG.worker_connections + 1];
    int acc = 0;

    int epollfd;
    if ((epollfd = epoll_create(CONFIG.worker_connections + 1)) == -1) {
        logging(LOG_ERROR, "epoll_create: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct epoll_event ev;
    ev.data.fd = soc;
    ev.events = EPOLLIN;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, soc, &ev) == -1) {
        logging(LOG_ERROR, "epoll_ctl: %s", strerror(errno));
        close(epollfd);
        exit(EXIT_FAILURE);
    }

    int count = 0;
    for (;;) {
        int nfds;
        switch ((nfds = epoll_wait(epollfd, events,
                                   CONFIG.worker_connections + 1, 10 * 1000))) {
            case -1: {
                logging(LOG_ERROR, "epoll_ctl: %s", strerror(errno));
                exit(EXIT_FAILURE);
            }
            case 0: {
                break;
            }
            default: {
                for (int i = 0; i < nfds; i++) {
                    if (events[i].data.fd == soc) {
                        count += accept_connection(soc, acc, from, epollfd, ev,
                                                   count);
                    } else {
                        serve(events[i].data.fd, epollfd, &ev);
                        close(events[i].data.fd);
                        count--;
                    }
                }
            }
        }
    }
}

int send_recv(int acc) {
    logging(LOG_INFO, "send_recv");
    Parser *parser = parser_from_socket(http_lexer, acc);
    HTTPRequest *request = parse_http_request(parser);
    if (!request) {
        logging(LOG_INFO, "invalid request");
        parser_free(parser);
        return -1;
    } else {
        logging(LOG_INFO, "[%s] %s %s", method_to_string(request->method),
                request->url, request->version);
        for (size_t i = 0; i < request->header_length; i++) {
            logging(LOG_INFO, "%s : %s", request->headers[i].field,
                    request->headers[i].value);
        }
    }

    HTTPResponse *response = route(request);
    if (!response) {
        logging(LOG_DEBUG, "failed to route");
        parser_free(parser);
        request_free(request);
        return -1;
    } else if (send_http_response(response, acc)) {
        logging(LOG_DEBUG, "failed to send http response");
        request_free(request);
        response_free(response);
        return -1;
    }

    request_free(request);
    response_free(response);
    parser_free(parser);
    return 0;
}
