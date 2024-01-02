#include <CUnit/Basic.h>
#include <fcntl.h>
#include <unistd.h>

void test_http_request();
void test_config();

int main(void) {
    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }

    int fd = open("/dev/null", O_RDWR, 0);
    if (fd != -1) {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDERR_FILENO);
        close(fd);
    }

    test_http_request();
    test_config();

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}
