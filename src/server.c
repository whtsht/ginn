#include "server.h"

#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "logger.h"

void send_recv_loop(int acc);

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
            logging(LOG_DEBUG, "accept: %s:%s\n", hbuf, sbuf);

            send_recv_loop(acc);

            (void)close(acc);
            acc = 0;
        }
    }
}

void send_recv_loop(int acc) {
    char buf[512], *ptr;
    ssize_t len;
    for (;;) {
        if ((len = recv(acc, buf, sizeof(buf), 0)) == -1) {
            logging(LOG_ERROR, "recv: %s\n", strerror(errno));
            break;
        }

        if (len == 0) {
            logging(LOG_INFO, "recv: EOF");
            break;
        }

        buf[len] = '\0';
        if ((ptr = strpbrk(buf, "\r\n")) != NULL) {
            *ptr = '\0';
        }
        logging(LOG_INFO, "[client] %s", buf);

        strncat(buf, ":OK\r\n", sizeof(buf) - strlen(buf) - 1);
        len = (ssize_t)strlen(buf);
        if ((len = send(acc, buf, (size_t)len, 0)) == -1) {
            logging(LOG_ERROR, "send: %s\n", strerror(errno));
            break;
        }
    }
}
