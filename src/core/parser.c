#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static ParserStatus next_string(ParserData* data) {
    if (data->string.cur + 1 < data->string.size) {
        data->string.cur += 1;
        return PS_Success;
    } else {
        return PS_EndOfContent;
    }
}

static ParserStatus current_string(char* c, ParserData* data) {
    if (data->string.cur < data->string.size) {
        *c = data->string.data[data->string.cur];
        return PS_Success;
    } else {
        return PS_EndOfContent;
    }
}

Parser* parser_from_string(ParserStatus (*lexer)(struct Parser* parser),
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

static ParserStatus next_file(ParserData* data) {
    if ((data->file.cur = fgetc(data->file.data)) != EOF) {
        return PS_Success;
    } else {
        if (ferror(data->file.data)) {
            return PS_Failure;
        } else {
            return PS_EndOfContent;
        }
    }
}

static ParserStatus current_file(char* c, ParserData* data) {
    if (data->file.cur != -1 ||
        (data->file.cur = fgetc(data->file.data)) != EOF) {
        *c = data->file.cur;
        return PS_Success;
    } else {
        if (ferror(data->file.data)) {
            return PS_Failure;
        } else {
            return PS_EndOfContent;
        }
    }
}

Parser* parser_from_file(ParserStatus (*lexer)(struct Parser* parser),
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

ParserStatus parser_next(Parser* parser) { return parser->next(&parser->data); }

ParserStatus parser_current(Parser* parser, char* c) {
    return parser->current(c, &parser->data);
}

ParserStatus parser_char(Parser* parser, char c) {
    if (parser->lexer(parser) == PS_EndOfContent) return PS_EndOfContent;

    char n;
    if (parser_current(parser, &n) == PS_Success) {
        if (n == c) {
            parser_next(parser);
            return PS_Success;
        } else {
            return PS_Failure;
        }
    }
    return PS_EndOfContent;
}

ParserStatus parser_word(Parser* parser, int (*separator)(char), char* word,
                         size_t max_word_length) {
    if (parser->lexer(parser) == PS_EndOfContent) return PS_EndOfContent;

    char n;
    size_t cnt = 0;

    if (parser_current(parser, &n) != PS_Success || separator(n)) {
        return PS_Failure;
    }

    while (parser_current(parser, &n) == PS_Success && !separator(n)) {
        if (cnt + 1 >= max_word_length) {
            return PS_Failure;
        }
        word[cnt++] = n;
        parser_next(parser);
    }
    word[cnt] = '\0';

    return PS_Success;
}

void parser_delete(Parser* parser) {
    if (parser->type == ParserTypeFile) {
        fclose(parser->data.file.data);
    }

    free(parser);
}
