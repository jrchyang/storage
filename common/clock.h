#ifndef CLOCK_H
#define CLOCK_H

#include <time.h>
#include "utime.h"

static inline utime_t clock_now() {
  struct timespec tp;

  clock_gettime(CLOCK_REALTIME, &tp);
  utime_t n(tp);
  return n;
}


#endif // CLOCK_H
