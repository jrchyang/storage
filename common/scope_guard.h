#ifndef SCOPE_GUARD_H
#define SCOPE_GUARD_H

#include <utility>

template <typename F>
struct scope_guard {
  F f;
  scope_guard() = delete;
  scope_guard(const scope_guard &) = delete;
  scope_guard(scope_guard &&) = default;
  scope_guard & operator=(const scope_guard &) = delete;
  scope_guard & operator=(scope_guard &&) = default;
  scope_guard(const F& f) : f(f) {}
  scope_guard(F &&f) : f(std::move(f)) {}
  template<typename... Args>
  scope_guard(std::in_place_t, Args&& ...args) : f(std::forward<Args>(args)...) {}
  ~scope_guard() {
    std::move(f)(); // Support at-most-once functions
  }
};

template <typename F>
scope_guard<F> make_scope_guard(F &&f) {
  return scope_guard<F>(std::forward<F>(f));
}

template<typename F, typename... Args>
scope_guard<F> make_scope_guard(std::in_place_type_t<F>, Args&& ...args) {
  return { std::in_place, std::forward<Args>(args)... };
}

#endif // SCOPE_GUARD_H
