#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <atomic>

class spinlock;

inline void spin_lock(std::atomic_flag& lock);
inline void spin_unlock(std::atomic_flag& lock);
inline void spin_lock(spinlock& lock);
inline void spin_unlock(spinlock& lock);

class spinlock final
{
  std::atomic_flag af = ATOMIC_FLAG_INIT;

  public:
  void lock() {
    spin_lock(af);
  }

  void unlock() noexcept {
    spin_unlock(af);
  }
};

// Free functions:
inline void spin_lock(std::atomic_flag& lock) {
  while(lock.test_and_set(std::memory_order_acquire))
    ;
}
inline void spin_unlock(std::atomic_flag& lock) {
  lock.clear(std::memory_order_release);
}
inline void spin_lock(std::atomic_flag *lock) {
  spin_lock(*lock);
}
inline void spin_unlock(std::atomic_flag *lock) {
  spin_unlock(*lock);
}

inline void spin_lock(spinlock& lock) {
  lock.lock();
}
inline void spin_unlock(spinlock& lock) {
  lock.unlock();
}
inline void spin_lock(spinlock *lock) {
  spin_lock(*lock);
}
inline void spin_unlock(spinlock *lock) {
  spin_unlock(*lock);
}

#endif // SPINLOCK_H
