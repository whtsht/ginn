#include <CUnit/Basic.h>
#include <CUnit/CUError.h>
#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>

#include "../src/core/config.h"

void config_success() {
    ConfigResult result = load_config("./tests/conf/ginn.correct.conf");
    CU_ASSERT_EQUAL(result, CR_Success);
    CU_ASSERT_STRING_EQUAL(CONFIG.logfile, "./ginn.log");
    CU_ASSERT_STRING_EQUAL(CONFIG.pidfile, "./ginn.pid");
    CU_ASSERT_STRING_EQUAL(CONFIG.port, "8080");
}

void config_failure() {
    ConfigResult result;

    result = load_config("./tests/conf/ginn.failure1.conf");
    CU_ASSERT_EQUAL(result, CR_Failure);

    result = load_config("./tests/conf/ginn.failure2.conf");
    CU_ASSERT_EQUAL(result, CR_Failure);

    result = load_config("./tests/conf/ginn.failure3.conf");
    CU_ASSERT_EQUAL(result, CR_Failure);
}

void test_config() {
    CU_pSuite suite = CU_add_suite("Configuration", NULL, NULL);

    CU_add_test(suite, "Config success", config_success);
    CU_add_test(suite, "Config failure", config_failure);
}
