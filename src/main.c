#include "logger.h"

int main(int argc, char *argv[]) {
    init_logger("./test.log", LOG_INFO);
    logging(LOG_INFO, "LOG_INFO LOG_INFO must show");
    logging(LOG_WARNING, "LOG_INFO LOG_WARNING must show");
    logging(LOG_ERROR, "LOG_INFO LOG_ERROR must show");
    cleanup_logger();

    init_logger("./test.log", LOG_WARNING);
    logging(LOG_INFO, "LOG_WARNING LOG_INFO must not show");
    logging(LOG_WARNING, "LOG_WARNING LOG_WARNING must show");
    logging(LOG_ERROR, "LOG_WARNING LOG_ERROR must show");
    cleanup_logger();

    init_logger("./test.log", LOG_ERROR);
    logging(LOG_INFO, "LOG_ERROR LOG_INFO must not show");
    logging(LOG_WARNING, "LOG_ERROR LOG_WARNING must not show");
    logging(LOG_ERROR, "LOG_ERROR LOG_ERROR must show");
    cleanup_logger();
}
