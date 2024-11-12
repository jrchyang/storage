#include "demangle.h"
#include "mempool.h"

// default to debug_mode off
bool mempool::debug_mode = false;
void mempool::set_debug_mode(bool d)
{
  debug_mode = d;
}

// --------------------------------------------------------------

mempool::pool_t& mempool::get_pool(mempool::pool_index_t ix)
{
  // We rely on this array being initialized before any invocation of
  // this function, even if it is called by ctors in other compilation
  // units that are being initialized before this compilation unit.
  static mempool::pool_t table[num_pools];
  return table[ix];
}

const char *mempool::get_pool_name(mempool::pool_index_t ix) {
#define P(x) #x,
  static const char *names[num_pools] = {
    DEFINE_MEMORY_POOLS_HELPER(P)
  };
#undef P
  return names[ix];
}

void mempool::dump(Formatter *f)
{
  stats_t total;

  /**
   * we need (dummy?) topmost section for JSON Formatter to print
   * pool names. It omits them otherwise.
   */
  f->open_object_section("mempool");
  f->open_object_section("by_pool");
  for (size_t i = 0; i < num_pools; ++i) {
    const pool_t &pool = mempool::get_pool((pool_index_t)i);
    f->open_object_section(get_pool_name((pool_index_t)i));
    pool.dump(f, &total);
    f->close_section();
  }
  f->close_section();
  f->dump_object("total", total);
  f->close_section();
}

// --------------------------------------------------------------
// pool_t

size_t mempool::pool_t::allocated_bytes() const
{
  ssize_t result = 0;
  for (size_t i = 0; i < num_shards; ++i) {
    result += shard[i].bytes;
  }
  if (result < 0) {
    // we raced with some unbalanced allocations/deallocations
    result = 0;
  }
  return (size_t) result;
}

size_t mempool::pool_t::allocated_items() const
{
  ssize_t result = 0;
  for (size_t i = 0; i < num_shards; ++i) {
    result += shard[i].items;
  }
  if (result < 0) {
    // we raced with some unbalanced allocations/deallocations
    result = 0;
  }
  return (size_t) result;
}

void mempool::pool_t::adjust_count(ssize_t items, ssize_t bytes)
{
  shard_t *shard = pick_a_shard();
  shard->items += items;
  shard->bytes += bytes;
}

void mempool::pool_t::get_stats(stats_t *total,
                                std::map<std::string, stats_t> *by_type) const
{
  for (size_t i = 0; i < num_shards; ++i) {
    total->items += shard[i].items;
    total->bytes += shard[i].bytes;
  }
  if (debug_mode) {
    std::lock_guard shard_lock(lock);
    for (auto &p : type_map) {
      std::string n = cpputils_demangle(p.second.type_name);
      stats_t &s = (*by_type)[n];
      s.bytes = p.second.items * p.second.item_size;
      s.items = p.second.items;
    }
  }
}

void mempool::pool_t::dump(Formatter *f, stats_t *ptotal) const
{
  stats_t total;
  std::map<std::string, stats_t> by_type;

  get_stats(&total, &by_type);
  if (ptotal) {
    *ptotal += total;
  }
  total.dump(f);
  if (!by_type.empty()) {
    f->open_object_section("by_type");
    for (auto &i : by_type) {
      f->open_object_section(i.first.c_str());
      i.second.dump(f);
      f->close_section();
    }
    f->close_section();
  }
}
