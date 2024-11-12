#include "uuid.h"

void uuid_d::dump(Formatter *f) const
{
  f->dump_stream("uuid") << to_string();
}

void uuid_d::generate_test_instances(std::list<uuid_d*>& o)
{
  // these are sourced from examples at
  // https://www.boost.org/doc/libs/1_62_0/libs/uuid/uuid.html#Synopsis_generators
  boost::uuids::string_generator gen;
  o.push_back(new uuid_d());
  o.back()->uuid = gen("{01234567-89ab-cdef-0123-456789abcdef}");
  o.push_back(new uuid_d());
  o.back()->uuid = gen(L"01234567-89ab-cdef-0123-456789abcdef");
  o.push_back(new uuid_d());
  o.back()->uuid = gen(std::string("0123456789abcdef0123456789abcdef"));
  o.push_back(new uuid_d());
  o.back()->uuid = gen(std::wstring(L"01234567-89ab-cdef-0123-456789abcdef"));
}