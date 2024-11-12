#ifndef COMPAT_H
#define COMPAT_H

#include <unistd.h>

#define aligned_free(ptr) free(ptr)

#define VOID_TEMP_FAILURE_RETRY(expression)        \
  static_cast<void>(TEMP_FAILURE_RETRY(expression))

#if defined(HAVE_PTHREAD_SETNAME_NP)
  #define cpputils_pthread_setname pthread_setname_np
#endif

#endif // COMPAT_H
