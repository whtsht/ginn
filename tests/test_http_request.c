#include <CUnit/Basic.h>
#include <CUnit/CUError.h>
#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>

#include "../src/core/parser.h"
#include "../src/http/request.h"

static void get_request1() {
    Parser* parser = parser_from_string(http_lexer, "GET / HTTP/1.1\r\n");
    HTTPRequest* req = parse_http_request(parser);

    CU_ASSERT_PTR_NOT_NULL(req);
    CU_ASSERT_EQUAL(req->method, GETMethod);
    CU_ASSERT_STRING_EQUAL(req->url, "/");
    CU_ASSERT_STRING_EQUAL(req->version, "HTTP/1.1");
    return;
}

void test_http_request() {
    CU_pSuite suite = CU_add_suite("HTTP Request", NULL, NULL);

    CU_add_test(suite, "Get request 1", get_request1);
}
