#ifndef CODE_ENVIRONMENT_H
#define CODE_ENVIRONMENT_H

#include <iosfwd>
#include <string>

enum code_environment_t {
  CODE_ENVIRONMENT_UTILITY = 0,
  CODE_ENVIRONMENT_DAEMON = 1,
  CODE_ENVIRONMENT_LIBRARY = 2,
  CODE_ENVIRONMENT_UTILITY_NODOUT = 3,
};



extern code_environment_t g_code_env;
const char *code_environment_to_str(enum code_environment_t e);
std::ostream &operator<<(std::ostream &oss, const enum code_environment_t e);
int get_process_name(char *buf, int len);
std::string get_process_name_cpp();

#endif // CODE_ENVIRONMENT_H
