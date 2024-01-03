#ifndef HTTP_ROUTE_H
#define HTTP_ROUTE_H

#include "request.h"
#include "response.h"

#define FILE_PATH_LENGTH 4096

HTTPResponse* route(HTTPRequest* request);

#endif
