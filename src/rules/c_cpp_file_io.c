#include <stdbool.h>
#include "seccomp_rules.h"


int c_cpp_file_io_seccomp_rules(struct config *_config) {
    return _c_cpp_seccomp_rules(_config, true);
}
