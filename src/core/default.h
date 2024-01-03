#ifndef DEFAULT_H
#define DEFAULT_H

#include "args.h"
#include "config.h"

Config default_config();
Args default_args();

char* default_error_pages(int error_code);

#endif
