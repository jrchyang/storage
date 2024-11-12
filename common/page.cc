#include <unistd.h>
#include "page.h"

// page size crap, see page.h
int _get_bits_of(int v)
{
  int n = 0;
  while (v) {
    n++;
    v = v >> 1;
  }
  return n;
}

unsigned _g_page_size = sysconf(_SC_PAGESIZE);
unsigned _g_page_shift = _get_bits_of(_g_page_size - 1);
unsigned long _g_page_mask = ~(unsigned long)(_g_page_size - 1);
