#include "streq.h"

#include <string.h>

int streq(const char* a, const char* b) { return strcmp(a, b) == 0; }
