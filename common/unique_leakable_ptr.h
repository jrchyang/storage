#ifndef UNIQUE_LEAKABLE_PTR_H
#define UNIQUE_LEAKABLE_PTR_H

#include <memory>

template <class T>
struct nop_delete {
  void operator()(T*) {}
};

// This is not unique_ptr-like smart pointer! It just signalizes ownership
// but DOES NOT manage the resource. It WILL LEAK if not manually deleted.
// It's rather a replacement for raw pointer than any other smart one.
//
// Considered options:
//  * unique_ptr with custom deleter implemented in .cc (would provide
//    the non-zero-cost resource management),
//  * GSL's owner<T*> (pretty neat but would impose an extra depedency),
//  * unique_ptr with nop deleter,
//  * raw pointer (doesn't embed ownership enforcement - std::move).
template <class T>
struct unique_leakable_ptr : public std::unique_ptr<T, nop_delete<T>> {
  using std::unique_ptr<T, nop_delete<T>>::unique_ptr;
};

#endif // UNIQUE_LEAKABLE_PTR_H
