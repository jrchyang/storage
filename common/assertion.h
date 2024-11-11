#ifndef ASSERTION_H
#define ASSERTION_H

#include <string>

/*
 * Select a function-name variable based on compiler tests, and any compiler
 * specific overrides.
 */
#if defined(HAVE_PRETTY_FUNC)
# define __ASSERTION_FUNCTION __PRETTY_FUNCTION__
#elif defined(HAVE_FUNC)
# define __ASSERTION_FUNCTION __func__
#else
# define __ASSERTION_FUNCTION ((__const char *) 0)
#endif

struct assert_data {
  const char *assertion;
  const char *file;
  const int   line;
  const char *function;
};

extern void __assert_fail(const char *assertion, const char *file, int line,
                          const char *function) __attribute__ ((__noreturn__));
extern void __assert_fail(const assert_data &ctx) __attribute__ ((__noreturn__));
extern void __assertf_fail(const char *assertion, const char *file, int line,
                           const char *function, const char* msg, ...)
                           __attribute__ ((__noreturn__));

[[noreturn]] void __abort(const char *file, int line, const char *func, const std::string& msg);
[[noreturn]] void __abortf(const char *file, int line, const char *func, const char* msg, ...);

#define common_abort(msg, ...)                                                \
  __abort(__FILE__, __LINE__, __ASSERTION_FUNCTION, "abort() called")
#define common_abort_msg(msg)                                                 \
  __abort(__FILE__, __LINE__, __ASSERTION_FUNCTION, msg)
#define common_abort_msgf(...)                                                \
  __abortf(_FILE__, __LINE__, __ASSERTION_FUNCTION, __VA_ARGS__)

#define common_assert(expr)                                                   \
  do {                                                                        \
    static const assert_data assert_data_ctx =                                \
      {__STRING(expr), __FILE__, __LINE__, __ASSERTION_FUNCTION};                \
    ((expr)                                                                   \
      ? static_cast<void> (0)                                                 \
      : __assert_fail(assert_data_ctx));                                      \
  } while(false)

#define common_assert_always(expr)                                            \
  do {                                                                        \
    static const assert_data assert_data_ctx =                                \
      {__STRING(expr), __FILE__, __LINE__, __ASSERTION_FUNCTION};                \
    ((expr)                                                                   \
      ? static_cast<void> (0)                                                 \
      : __assert_fail(assert_data_ctx));                                      \
  } while(false)

#define common_assertf(expr, ...)                                             \
  ((expr)                                                                     \
    ? static_cast<void> (0)                                                   \
    : __assertf_fail(__STRING(expr),                                          \
      __FILE__, __LINE__, __ASSERTION_FUNCTION, __VA_ARGS__))

#define common_assertf_always(expr, ...)                                      \
  ((expr)                                                                     \
    ? static_cast<void> (0)                                                   \
    : __assertf_fail(__STRING(expr),                                          \
      __FILE__, __LINE__, __ASSERTION_FUNCTION, __VA_ARGS__))

#endif // ASSERTION_H
