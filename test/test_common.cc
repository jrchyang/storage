#include <string>
#include <sstream>
#include <regex>
#include <cassert>
#include <stdexcept>
#include <boost/algorithm/string.hpp>

#include "gtest/gtest.h"
#include "../common/backtrace.h"
#include "../common/clock.h"
#include "../common/assertion.h"
#include "../common/formatter.h"


// a dummy function, so we can check "foo" in the backtrace.
// do not mark this function as static or put it into an anonymous namespace,
// otherwise it's function name will be removed in the backtrace.
static std::string foo()
{
  std::ostringstream oss;
  oss << ClibBackTrace(0);
  return oss.str();
}

TEST(Miscs, Clock) {
  std::ostringstream tss;
  tss << clock_now();
  std::string time = tss.str();
  std::regex pattern(R"(^\d{4}-(0[1-9]|1[0-2])-(0[1-9]|[12][0-9]|3[01]).*)");
  EXPECT_TRUE(std::regex_match(time, pattern));
}

TEST(Miscs, Assert) {
  EXPECT_DEATH({ common_abort("common test"); }, ".*");
  EXPECT_DEATH({ common_abort_msg("common test"); }, ".*");
  common_assert(1);
}

TEST(Miscs, Formatter) {
  struct stats_t {
    ssize_t items = 10;
    ssize_t bytes = 1240;

    void dump(Formatter *f) const {
      f->open_object_section("MiscsFormatter");
      f->dump_int("items", items);
      f->dump_int("bytes", bytes);
      f->close_section();
    }
  } s;
  auto f = Formatter::create("json-pretty");
  s.dump(f);
  std::stringstream oss;
  f->flush(oss);
  delete f;
}
