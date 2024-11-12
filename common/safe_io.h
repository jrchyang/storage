#ifndef SAFE_IO_H
#define SAFE_IO_H

#include <sys/types.h>

/*
 * Safe functions wrapping the raw read() and write() libc functions.
 * These retry on EINTR, and on error return -errno instead of returning
 * -1 and setting errno).
 *
 * On Windows, only recv/send work with sockets.
 */
ssize_t safe_read(int fd, void *buf, size_t count)
  __attribute__((warn_unused_result));
ssize_t safe_write(int fd, const void *buf, size_t count)
  __attribute__((warn_unused_result));
ssize_t safe_recv(int fd, void *buf, size_t count)
  __attribute__((warn_unused_result));
ssize_t safe_send(int fd, const void *buf, size_t count)
  __attribute__((warn_unused_result));
ssize_t safe_pread(int fd, void *buf, size_t count, off_t offset)
  __attribute__((warn_unused_result));
ssize_t safe_pwrite(int fd, const void *buf, size_t count, off_t offset)
  __attribute__((warn_unused_result));

/*
 * Same as the above functions, but return -EDOM unless exactly the requested
 * number of bytes can be read.
 */
ssize_t safe_read_exact(int fd, void *buf, size_t count)
  __attribute__((warn_unused_result));
ssize_t safe_recv_exact(int fd, void *buf, size_t count)
  __attribute__((warn_unused_result));
ssize_t safe_pread_exact(int fd, void *buf, size_t count, off_t offset)
  __attribute__((warn_unused_result));
/*
 * Safe functions to read and write an entire file.
 */
int safe_write_file(const char *base, const char *file,
                    const char *val, size_t vallen,
                    unsigned mode);
int safe_read_file(const char *base, const char *file,
                   char *val, size_t vallen);

#endif // SAFE_IO_H
