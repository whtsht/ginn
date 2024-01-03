#include "default.h"

#include <stddef.h>

Config default_config() {
    return (Config){
        .logfile = "/var/log/ginn.log",
        .pidfile = "/var/run/ginn.pid",
        .port = "4700",
        .root = "/usr/share/ginn/html",
        .index = "index.html",
        .error_pages = NULL,
        .error_pages_len = 0,
    };
}

Args default_args() {
    return (Args){.command = StartCommand, .conf_file = "/etc/ginn.conf"};
}

char* default_error_pages(int error_code) {
    switch (error_code) {
        case 404:
            return "/404.html";
        default:
            return NULL;
    }
}
