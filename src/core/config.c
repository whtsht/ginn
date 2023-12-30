#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "default.h"
#include "parser.h"

Config CONFIG;

static char* load_content(const char* conf_file) {
    FILE* fp = fopen(conf_file, "r");
    if (!fp) {
        perror("Can't open config file");
        exit(EXIT_FAILURE);
    }
    fseek(fp, 0L, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    char* content = (char*)malloc(sizeof(char) * (sz + 1));

    fread(content, 1, sz, fp);
    content[sz] = '\0';
    fclose(fp);
    return content;
}

static char* parser_logfile(Parser* parser) {
    if (!parser_word(parser)) {
        fprintf(stderr, "logfile: expected filename\n");
        exit(EXIT_FAILURE);
    }

    if (!parser_char(parser, ';')) {
        fprintf(stderr, "logfile: expected `;`\n");
        exit(EXIT_FAILURE);
    }

    char* filename = malloc(sizeof(char) * MAX_WORD_LENGTH);
    strcpy(filename, P_WORD);

    return filename;
}

static char* parser_pidfile(Parser* parser) {
    if (!parser_word(parser)) {
        fprintf(stderr, "pidfile: expected filename\n");
        exit(EXIT_FAILURE);
    }

    if (!parser_char(parser, ';')) {
        fprintf(stderr, "pidfile: expected `;`\n");
        exit(EXIT_FAILURE);
    }

    char* filename = malloc(sizeof(char) * MAX_WORD_LENGTH);
    strcpy(filename, P_WORD);

    return filename;
}

static char* parser_port(Parser* parser) {
    if (!parser_word(parser)) {
        fprintf(stderr, "port: expected port number\n");
        exit(EXIT_FAILURE);
    }

    if (!parser_char(parser, ';')) {
        fprintf(stderr, "port: expected `;`\n");
        exit(EXIT_FAILURE);
    }

    char* port = malloc(sizeof(char) * MAX_WORD_LENGTH);
    strcpy(port, P_WORD);

    for (int i = 0; port[i] != '\0'; i++) {
        if (port[i] < '0' || '9' < port[i]) {
            fprintf(stderr, "port: expected port number, found %s\n", port);
            exit(EXIT_FAILURE);
        }
    }

    return port;
}

void load_config(const char* conf_file) {
    Config config = default_config();

    char* content = load_content(conf_file);
    Parser* parser = parser_new(content);

    while (parser_word(parser)) {
        if (strcmp(P_WORD, "logfile") == 0) {
            config.logfile = parser_logfile(parser);
            continue;
        }

        if (strcmp(P_WORD, "pidfile") == 0) {
            config.pidfile = parser_pidfile(parser);
            continue;
        }

        if (strcmp(P_WORD, "port") == 0) {
            config.port = parser_port(parser);
            continue;
        }

        fprintf(stderr, "unexpected string: %s\n", P_WORD);
        exit(EXIT_FAILURE);
    }

    CONFIG = config;
}