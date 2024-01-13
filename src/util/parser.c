#include "parser.h"

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static ParserResult next_string(ParserData* data) {
    if (data->string.cur + 1 < data->string.size) {
        data->string.cur += 1;
        return PR_Success;
    } else {
        return PR_EndOfContent;
    }
}

static ParserResult current_string(char* c, ParserData* data) {
    if (data->string.cur < data->string.size) {
        *c = data->string.data[data->string.cur];
        return PR_Success;
    } else {
        return PR_EndOfContent;
    }
}

Parser* parser_from_string(ParserResult (*lexer)(struct Parser* parser),
                           char* string) {
    Parser* parser = malloc(sizeof(Parser));
    parser->next = next_string;
    parser->current = current_string;
    parser->lexer = lexer;
    parser->type = ParserTypeString;
    ParserData data;
    data.string = (String){
        .data = string,
        .size = strlen(string),
        .cur = 0,
    };
    parser->data = data;
    return parser;
}

static ParserResult next_file(ParserData* data) {
    if ((data->file.cur = fgetc(data->file.data)) != EOF) {
        return PR_Success;
    } else {
        if (ferror(data->file.data)) {
            return PR_Failure;
        } else {
            return PR_EndOfContent;
        }
    }
}

static ParserResult current_file(char* c, ParserData* data) {
    if (data->file.cur != -1) {
        *c = data->file.cur;
        return PR_Success;
    }

    if ((data->file.cur = fgetc(data->file.data)) != EOF) {
        *c = data->file.cur;
        return PR_Success;
    } else {
        if (ferror(data->file.data)) {
            return PR_Failure;
        } else {
            return PR_EndOfContent;
        }
    }
}

Parser* parser_from_file(ParserResult (*lexer)(struct Parser* parser),
                         FILE* file) {
    Parser* parser = malloc(sizeof(Parser));
    parser->next = next_file;
    parser->current = current_file;
    parser->lexer = lexer;
    parser->type = ParserTypeFile;
    ParserData data;
    data.file = (File){
        .data = file,
        .cur = -1,
    };
    parser->data = data;
    return parser;
}

static ParserResult next_socket(ParserData* data) {
    char n;
    ssize_t len;
    if ((len = read(data->socket.desc, &n, 1)) != -1) {
        if (len == 0) return PR_EndOfContent;

        data->socket.cur = n;
        return PR_Success;
    } else {
        return PR_Failure;
    }
}

static ParserResult current_socket(char* c, ParserData* data) {
    if (data->socket.cur != -1) {
        *c = data->socket.cur;
        if (!data->socket.cur) {
            return PR_EndOfContent;
        }
        return PR_Success;
    }

    char n;
    ssize_t len = 0;
    if ((len = read(data->socket.desc, &n, 1)) != -1) {
        if (len == 0) {
            *c = data->socket.cur = '\0';
            return PR_EndOfContent;
        }

        *c = data->socket.cur = n;
        return PR_Success;
    } else {
        return PR_Failure;
    }
}

Parser* parser_from_socket(ParserResult (*lexer)(struct Parser* parser),
                           int socket) {
    Parser* parser = malloc(sizeof(Parser));
    parser->next = next_socket;
    parser->current = current_socket;
    parser->lexer = lexer;
    parser->type = ParserTypeSocket;
    ParserData data;
    data.socket = (Socket){
        .desc = socket,
        .cur = -1,
    };
    parser->data = data;
    return parser;
}

ParserResult parser_next(Parser* parser) { return parser->next(&parser->data); }

ParserResult parser_current(Parser* parser, char* c) {
    return parser->current(c, &parser->data);
}

ParserResult parser_char(Parser* parser, char c) {
    if (parser->lexer(parser) == PR_EndOfContent) return PR_EndOfContent;

    char n;
    if (parser_current(parser, &n) == PR_Success) {
        if (n == c) {
            parser_next(parser);
            return PR_Success;
        } else {
            return PR_Failure;
        }
    }
    return PR_EndOfContent;
}

ParserResult parser_string(Parser* parser, char* s) {
    if (parser->lexer(parser) == PR_EndOfContent) return PR_EndOfContent;

    size_t len = strlen(s);
    for (size_t i = 0; i < len; i++) {
        ParserResult ps = parser_char(parser, s[i]);
        if (ps == PR_Failure) {
            return PR_Failure;
        } else if (ps == PR_EndOfContent) {
            return PR_EndOfContent;
        }
    }

    return PR_Success;
}

ParserResult parser_word(Parser* parser, int (*separator)(char), char* word,
                         size_t max_word_length) {
    if (parser->lexer(parser) == PR_EndOfContent) return PR_EndOfContent;

    char n;
    size_t cnt = 0;

    while (parser_current(parser, &n) == PR_Success && !separator(n)) {
        if (cnt + 1 >= max_word_length) {
            return PR_Failure;
        }
        word[cnt++] = n;
        parser_next(parser);
    }
    word[cnt] = '\0';

    return PR_Success;
}

void parser_free(Parser* parser) { free(parser); }
