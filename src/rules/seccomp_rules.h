#ifndef JUDGER_SECCOMP_RULES_H
#define JUDGER_SECCOMP_RULES_H
#include "../runner.h"

int c_cpp_seccomp_rules(struct config *_config);
int general_seccomp_rules(struct config *_config);

#endif //JUDGER_SECCOMP_RULES_H
