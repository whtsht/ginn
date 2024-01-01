#include "config.h"

#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "default.h"
#include "parser.h"

Config CONFIG;

static ParserStatus config_lexer(Parser* parser) {
    char n;
    while (parser_current(parser, &n) == PS_Success) {
        /* skip line comment */
        if (n == '#') {
            while (parser_current(parser, &n) == PS_Success && n != '\n') {
                parser_next(parser);
            }
            continue;
        }

        if (n == ' ' || n == '\n') {
            parser_next(parser);
            continue;
        }

        break;
    }
    return parser_current(parser, &n);
}

static int separator(char c) { return c == ';' || c == ' ' || c == '\n'; }
static int space(char c) { return c == ' '; }

static char* parser_logfile(Parser* parser) {
    char* filename = malloc(sizeof(char) * NAME_MAX);
    if (parser_word(parser, separator, filename, NAME_MAX) != PS_Success) {
        fprintf(stderr, "logfile: expected filename\n");
        free(filename);
        exit(EXIT_FAILURE);
    }

    if (parser_char(parser, ';') != PS_Success) {
        fprintf(stderr, "logfile: expected `;`\n");
        free(filename);
        exit(EXIT_FAILURE);
    }
    return filename;
}

static char* parser_pidfile(Parser* parser) {
    char* filename = malloc(sizeof(char) * NAME_MAX);
    if (parser_word(parser, separator, filename, NAME_MAX) != PS_Success) {
        fprintf(stderr, "pidfile: expected filename\n");
        free(filename);
        exit(EXIT_FAILURE);
    }

    if (parser_char(parser, ';') != PS_Success) {
        fprintf(stderr, "pidfile: expected `;`\n");
        free(filename);
        exit(EXIT_FAILURE);
    }

    return filename;
}

static char* parser_port(Parser* parser) {
    char* port = malloc(sizeof(char) * 100);
    if (parser_word(parser, separator, port, 100) != PS_Success) {
        fprintf(stderr, "port: expected port number\n");
        free(port);
        exit(EXIT_FAILURE);
    }

    if (parser_char(parser, ';') != PS_Success) {
        fprintf(stderr, "port: expected `;`\n");
        free(port);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; port[i] != '\0'; i++) {
        if (port[i] < '0' || '9' < port[i]) {
            fprintf(stderr, "port: expected port number, found %s\n", port);
            free(port);
            exit(EXIT_FAILURE);
        }
    }

    return port;
}

void load_config(const char* conf_file) {
    Config config = default_config();

    FILE* fp = fopen(conf_file, "r");
    if (!fp) {
        perror("Can't open config file");
        exit(EXIT_FAILURE);
    }

    Parser* parser = parser_from_file(config_lexer, fp);

    char word[100];
    while (parser_word(parser, space, word, 100) == PS_Success) {
        if (strcmp(word, "logfile") == 0) {
            config.logfile = parser_logfile(parser);
            continue;
        }

        if (strcmp(word, "pidfile") == 0) {
            config.pidfile = parser_pidfile(parser);
            continue;
        }

        if (strcmp(word, "port") == 0) {
            config.port = parser_port(parser);
            continue;
        }

        fprintf(stderr, "unexpected string: %s\n", word);
        exit(EXIT_FAILURE);
    }

    parser_delete(parser);

    CONFIG = config;
}

void print_config() {
    printf("\tlogfile\t= %s\n", CONFIG.logfile);
    printf("\tpidfile\t= %s\n", CONFIG.pidfile);
    printf("\tport\t= %s\n", CONFIG.port);
}
