#ifndef ERROR_CODE_H
#define ERROR_CODE_H

#include <netdb.h>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnon-virtual-dtor"

// This is for error categories we define, so we can specify the
// equivalent integral value at the point of definition.
class converting_category : public boost::system::error_category {
public:
  virtual int from_code(int code) const noexcept = 0;
};

const boost::system::error_category& category() noexcept;

enum class errc {
  not_in_map = 1,   // The requested item was not found in the map
  does_not_exist,   // Item does not exist
  failure,          // An internal fault or inconsistency
  exists,           // Already exists
  limit_exceeded,   // Attempting to use too much of something
  auth,             // May not be an auth failure. It could be that the
                    // preconditions to attempt auth failed.
  conflict,         // Conflict or precondition failure
};

namespace boost::system {
template<>
struct is_error_condition_enum<::errc> {
  static const bool value = true;
};
template<>
struct is_error_code_enum<::errc> {
  static const bool value = false;
};
}

//  explicit conversion:
inline boost::system::error_code make_error_code(errc e) noexcept {
  return { static_cast<int>(e), category() };
}

// implicit conversion:
inline boost::system::error_condition make_error_condition(errc e) noexcept {
  return { static_cast<int>(e), category() };
}

[[nodiscard]] boost::system::error_code to_error_code(int ret) noexcept;
[[nodiscard]] int from_error_code(boost::system::error_code e) noexcept;

#pragma GCC diagnostic pop
#pragma clang diagnostic pop


namespace buffer {
const boost::system::error_category& buffer_category() noexcept;
enum class errc {
  bad_alloc = 1,
  end_of_buffer,
  malformed_input
};
}

namespace boost::system {
template<>
struct is_error_code_enum<::buffer::errc> {
  static const bool value = true;
};

template<>
struct is_error_condition_enum<::buffer::errc> {
  static const bool value = false;
};
}

namespace buffer {

// implicit conversion:
inline boost::system::error_code make_error_code(errc e) noexcept {
  return { static_cast<int>(e), buffer_category() };
}

// explicit conversion:
inline boost::system::error_condition
make_error_condition(errc e) noexcept {
  return { static_cast<int>(e), buffer_category() };
}

struct error : boost::system::system_error {
  using system_error::system_error;
};

struct bad_alloc : public error {
  bad_alloc() : error(errc::bad_alloc) {}
  bad_alloc(const char* what_arg) : error(errc::bad_alloc, what_arg) {}
  bad_alloc(const std::string& what_arg) : error(errc::bad_alloc, what_arg) {}
};
struct end_of_buffer : public error {
  end_of_buffer() : error(errc::end_of_buffer) {}
  end_of_buffer(const char* what_arg) : error(errc::end_of_buffer, what_arg) {}
  end_of_buffer(const std::string& what_arg)
    : error(errc::end_of_buffer, what_arg) {}
};

struct malformed_input : public error {
  malformed_input() : error(errc::malformed_input) {}
  malformed_input(const char* what_arg)
    : error(errc::malformed_input, what_arg) {}
  malformed_input(const std::string& what_arg)
    : error(errc::malformed_input, what_arg) {}
};
struct error_code : public error {
  error_code(int r) : error(-r, boost::system::system_category()) {}
  error_code(int r, const char* what_arg)
    : error(-r, boost::system::system_category(), what_arg) {}
  error_code(int r, const std::string& what_arg)
    : error(-r, boost::system::system_category(), what_arg) {}
};

}

/* Return a given error code as a string */
std::string common_strerror(int err);

#endif // ERROR_CODE_H
