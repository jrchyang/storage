#ifndef ARMOR_H
#define ARMOR_H

int armor(char *dst, char * const dst_end, const char * src, const char *end);
int armor_linebreak(char *dst, char * const dst_end, const char *src,
                    const char *end, int line_width);
int unarmor(char *dst, char * const dst_end, const char *src, const char * const end);

#endif // ARMOR_H
