#include "default.h"

Config default_config() {
    return (Config){.logfile = "/var/log/ginn.log",
                    .pidfile = "/var/run/ginn.pid",
                    .port = "4700"};
}

Args default_args() {
    return (Args){.command = StartCommand, .conf_file = "/etc/ginn.conf"};
}
