#include "default.h"

#include <stddef.h>

Config default_config() {
    return (Config){
        .logfile = "/var/log/ginn.log",
        .pidfile = "/var/run/ginn.pid",
        .port = "80",
        .root = "/usr/share/ginn/html",
        .index = "index.html",
        .error_pages = NULL,
        .error_pages_len = 0,
        .worker_connections = 1000,
    };
}

Args default_args() {
    return (Args){.command = StartCommand, .conf_file = "/etc/ginn.conf"};
}

char* server_error_page() { return "/50x.html"; }
