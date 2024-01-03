#include "config.h"

#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "default.h"
#include "parser.h"
#include "streq.h"

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
    if (parser_word(parser, separator, filename, PATH_MAX) != PS_Success ||
        strlen(filename) == 0) {
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
    if (parser_word(parser, separator, filename, PATH_MAX) != PS_Success ||
        strlen(filename) == 0) {
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

static ConfigResult parser_error_page(Parser* parser, ErrorPage* error_pages,
                                      int* error_pages_len) {
    char word[PATH_MAX];
    int count = 0;
    char* end;
    while (parser_word(parser, separator, word, PATH_MAX) == PS_Success) {
        int error_code = 0;
        if (!(error_code = strtol(word, &end, 10))) {
            break;
        }
        error_pages[count].code = error_code;
        count += 1;
    }

    if (count == 0) {
        fprintf(stderr, "error_page: expected status codes\n");
        return CR_Failure;
    }

    *error_pages_len = count;
    for (int i = 0; i < count; i++) {
        error_pages->path = strdup(word);
    }

    if (parser_char(parser, ';') != PS_Success) {
        fprintf(stderr, "error_page: expected `;`\n");
        return CR_Failure;
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
        if (streq(directive, "logfile")) {
            char* filename = malloc(sizeof(char) * PATH_MAX);
            if (parser_logfile(parser, filename)) {
                free(filename);
                return CR_Failure;
            }
            config.logfile = filename;
            continue;
        }

        if (streq(directive, "pidfile")) {
            char* filename = malloc(sizeof(char) * PATH_MAX);
            if (parser_pidfile(parser, filename)) {
                free(filename);
                return CR_Failure;
            }
            config.pidfile = filename;
            continue;
        }

        if (streq(directive, "port")) {
            char* port = malloc(sizeof(char) * PORT_MAX);
            if (parser_port(parser, port)) {
                free(port);
                return CR_Failure;
            }
            config.port = port;
            continue;
        }

        if (streq(directive, "error_page")) {
            ErrorPage* error_pages = malloc(sizeof(ErrorPage) * 100);
            int error_pages_len = 0;
            if (parser_error_page(parser, error_pages, &error_pages_len)) {
                free(error_pages);
                return CR_Failure;
            }
            config.error_pages = error_pages;
            config.error_pages_len = error_pages_len;
            continue;
        }

        fprintf(stderr, "invalid directive: %s\n", directive);
        return CR_Failure;
    }

    parser_delete(parser);

    CONFIG = config;

    return CR_Success;
}

char* get_error_page(int error_code) {
    for (int i = 0; i < CONFIG.error_pages_len; i++) {
        if (CONFIG.error_pages[i].code == error_code) {
            return CONFIG.error_pages[i].path;
        }
    }
    return NULL;
}

void print_config() {
    printf("\tlogfile\t= %s\n", CONFIG.logfile);
    printf("\tpidfile\t= %s\n", CONFIG.pidfile);
    printf("\tport\t= %s\n", CONFIG.port);
    printf("\troot\t= %s\n", CONFIG.root);
    printf("\tindex\t= %s\n", CONFIG.index);
    printf("\t404_error_page\t= %s\n", get_error_page(404));
}
