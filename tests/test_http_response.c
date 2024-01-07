#include <CUnit/Basic.h>
#include <CUnit/CUError.h>
#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/http/response.h"

static int cleanup() {
    remove("./out.txt");
    return 0;
}

static void cmp_file(char* expected) {
    FILE* out = fopen("out.txt", "r");
    size_t sz = get_file_size(out);
    char* buf = malloc(sz);
    fread(buf, 1, sz, out);
    CU_ASSERT_STRING_EQUAL(buf, expected);
    fclose(out);
    free(buf);
}

static void send_request() {
    FILE* fp = fopen("./tests/public/index.html", "r");
    FILE* out = fopen("out.txt", "w");
    HTTPResponse response =
        (HTTPResponse){.status = HS_OK,
                       .protocol = NULL,
                       .headers = NULL,
                       .header_length = 0,
                       .body = (HTTPResponseBody){.file = fp}};
    CU_ASSERT_EQUAL(send_http_response(&response, fileno(out)), HRR_Success);
    fclose(out);

    cmp_file("HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello World!");
    fclose(fp);
}

void test_http_response() {
    CU_pSuite suite = CU_add_suite("HTTP Response", NULL, cleanup);

    CU_add_test(suite, "Send response 1", send_request);
}
