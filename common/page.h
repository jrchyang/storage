#ifndef PAGE_H
#define PAGE_H

extern unsigned _g_page_size;
extern unsigned _g_page_shift;
extern unsigned long _g_page_mask;

#define GLOBAL_PAGE_SIZE    _g_page_size
#define GLOBAL_PAGE_MASK    _g_page_mask
#define GLOBAL_PAGE_SHIFT   _g_page_shift

#endif // PAGE_H
