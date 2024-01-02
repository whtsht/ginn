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

static ConfigResult parser_logfile(Parser* parser, char* filename) {
    if (parser_word(parser, separator, filename, NAME_MAX) != PS_Success) {
        fprintf(stderr, "logfile: expected filename\n");
        return CR_Failure;
    }

    if (parser_char(parser, ';') != PS_Success) {
        fprintf(stderr, "logfile: expected `;`\n");
        return CR_Failure;
    }

    return CR_Success;
}

static ConfigResult parser_pidfile(Parser* parser, char* filename) {
    if (parser_word(parser, separator, filename, NAME_MAX) != PS_Success) {
        fprintf(stderr, "pidfile: expected filename\n");
        return CR_Failure;
    }

    if (parser_char(parser, ';') != PS_Success) {
        fprintf(stderr, "pidfile: expected `;`\n");
        return CR_Failure;
    }

    return CR_Success;
}

static ConfigResult parser_port(Parser* parser, char* port) {
    if (parser_word(parser, separator, port, PORT_MAX) != PS_Success) {
        fprintf(stderr, "port: expected port number\n");
        return CR_Failure;
    }

    if (parser_char(parser, ';') != PS_Success) {
        fprintf(stderr, "port: expected `;`\n");
        return CR_Failure;
    }

    for (int i = 0; port[i] != '\0'; i++) {
        if (port[i] < '0' || '9' < port[i]) {
            fprintf(stderr, "port: expected port number, found %s\n", port);
            return CR_Failure;
        }
    }

    return CR_Success;
}

ConfigResult load_config(const char* conf_file) {
    Config config = default_config();

    FILE* fp = fopen(conf_file, "r");
    if (!fp) {
        perror("Can't open config file");
        return CR_Failure;
    }

    Parser* parser = parser_from_file(config_lexer, fp);

    char directive[DIRECTIVE_MAX];
    while (parser_word(parser, space, directive, DIRECTIVE_MAX) == PS_Success) {
        if (strcmp(directive, "logfile") == 0) {
            char* filename = malloc(sizeof(char) * NAME_MAX);
            if (parser_logfile(parser, filename)) {
                free(filename);
                return CR_Failure;
            }
            config.logfile = filename;
            continue;
        }

        if (strcmp(directive, "pidfile") == 0) {
            char* filename = malloc(sizeof(char) * NAME_MAX);
            if (parser_pidfile(parser, filename)) {
                free(filename);
                return CR_Failure;
            }
            config.pidfile = filename;
            continue;
        }

        if (strcmp(directive, "port") == 0) {
            char* port = malloc(sizeof(char) * PORT_MAX);
            if (parser_port(parser, port)) {
                free(port);
                return CR_Failure;
            }
            config.port = port;
            continue;
        }

        fprintf(stderr, "invalid directive: %s\n", directive);
        return CR_Failure;
    }

    parser_delete(parser);

    CONFIG = config;

    return CR_Success;
}

void print_config() {
    printf("\tlogfile\t= %s\n", CONFIG.logfile);
    printf("\tpidfile\t= %s\n", CONFIG.pidfile);
    printf("\tport\t= %s\n", CONFIG.port);
}
