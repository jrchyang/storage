#include <iostream>
#include <string.h>
#include <pthread.h>

#include "code_environment.h"

code_environment_t g_code_env = CODE_ENVIRONMENT_UTILITY;

const char *code_environment_to_str(enum code_environment_t e)
{
  switch (e) {
    case CODE_ENVIRONMENT_UTILITY:
      return "CODE_ENVIRONMENT_UTILITY";
    case CODE_ENVIRONMENT_DAEMON:
      return "CODE_ENVIRONMENT_DAEMON";
    case CODE_ENVIRONMENT_LIBRARY:
      return "CODE_ENVIRONMENT_LIBRARY";
    default:
      return NULL;
  }
}

std::ostream &operator<<(std::ostream &oss, const enum code_environment_t e)
{
  oss << code_environment_to_str(e);
  return oss;
}

int get_process_name(char *buf, int len)
{
  if (len <= 16) {
    // The man page discourages using pthread_getname_np() with a buffer shorter
    // than 16 bytes. With a 16-byte buffer, it might not be null-terminated.
    return -ENAMETOOLONG;
  }
  // FIPS zeroization audit 20191115: this memset is not security related.
  memset(buf, 0, len);
  return pthread_getname_np(pthread_self(), buf, len);
}

std::string get_process_name_cpp()
{
  char buf[32];
  if (get_process_name(buf, sizeof(buf))) {
    return "(unknown)";
  }
  return std::string(buf);
}

