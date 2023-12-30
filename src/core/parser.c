#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char P_WORD[MAX_WORD_LENGTH];

Parser* parser_new(char* str) {
    Parser* parser = malloc(sizeof(Parser));
    parser->str = str;
    parser->size = strlen(str);
    parser->cur = 0;
    return parser;
}

void parser_free(Parser* parser) { free(parser); }

static void next(Parser* parser) {
    if (parser->cur < parser->size) {
        parser->cur += 1;
    }
}

static char parser_current(Parser* parser) {
    if (parser->cur < parser->size) {
        return parser->str[parser->cur];
    } else {
        return '\0';
    }
}

static int parser_lexeme(Parser* parser) {
    char n;
    while ((n = parser_current(parser)) &&
           (n == '#' || n == ' ' || n == '\n')) {
        /* skip line comment */
        if (n == '#') {
            while ((n = parser_current(parser)) && n != '\n') next(parser);
        }

        next(parser);
    }
    return parser_current(parser) == '\0';
}

char parser_char(Parser* parser, char c) {
    if (parser_lexeme(parser)) return 0;

    char n;
    if ((n = parser_current(parser)) && n == c) {
        next(parser);
        return n;
    }
    return '\0';
}

int parser_word(Parser* parser) {
    if (parser_lexeme(parser)) return 0;

    char n;
    memset(P_WORD, 0, MAX_WORD_LENGTH);
    int cnt = 0;

    while ((n = parser_current(parser)) && n != ' ' && n != ';') {
        if (cnt + 1 >= MAX_WORD_LENGTH) {
            fprintf(stderr, "word too long %s", P_WORD);
            exit(EXIT_FAILURE);
        }
        P_WORD[cnt++] = n;
        next(parser);
    }

    return cnt;
}
