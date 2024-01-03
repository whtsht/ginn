#ifndef HTTP_HEADER_H
#define HTTP_HEADER_H

#include "../core/parser.h"

typedef enum {
    ContentLength,
} HTTPHeaderField;

typedef struct {
    HTTPHeaderField field;
    char* value;
} HTTPHeader;

#endif
