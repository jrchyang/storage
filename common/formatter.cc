#include <algorithm>
#include <set>
#include <limits>
#include <iomanip>
#include <fmt/format.h>
#include <boost/optional.hpp>

#include "global_definition.h"
#include "macros.h"
#include "assertion.h"
#include "formatter.h"

#define DBL_QUOTE_JESCAPE "\\\""
#define BACKSLASH_JESCAPE "\\\\"
#define TAB_JESCAPE "\\t"
#define NEWLINE_JESCAPE "\\n"

size_t escape_json_attr_len(const char *buf, size_t src_len)
{
  const char *b;
  size_t i, ret = 0;
  for (i = 0, b = buf; i < src_len; ++i, ++b) {
    unsigned char c = *b;
    switch (c) {
    case '"':
      ret += SSTRL(DBL_QUOTE_JESCAPE);
      break;
    case '\\':
      ret += SSTRL(BACKSLASH_JESCAPE);
      break;
    case '\t':
      ret += SSTRL(TAB_JESCAPE);
      break;
    case '\n':
      ret += SSTRL(NEWLINE_JESCAPE);
      break;
    default:
      // Escape control characters.
      if ((c < 0x20) || (c == 0x7f)) {
        ret += 6;
      } else {
        ret++;
      }
    }
  }
  // leave room for null terminator
  ret++;
  return ret;
}

void escape_json_attr(const char *buf, size_t src_len, char *out)
{
  char *o = out;
  const char *b;
  size_t i;
  for (i = 0, b = buf; i < src_len; ++i, ++b) {
    unsigned char c = *b;
    switch (c) {
    case '"':
      // cppcheck-suppress invalidFunctionArg
      memcpy(o, DBL_QUOTE_JESCAPE, SSTRL(DBL_QUOTE_JESCAPE));
      o += SSTRL(DBL_QUOTE_JESCAPE);
      break;
    case '\\':
      // cppcheck-suppress invalidFunctionArg
      memcpy(o, BACKSLASH_JESCAPE, SSTRL(BACKSLASH_JESCAPE));
      o += SSTRL(BACKSLASH_JESCAPE);
      break;
    case '\t':
      // cppcheck-suppress invalidFunctionArg
      memcpy(o, TAB_JESCAPE, SSTRL(TAB_JESCAPE));
      o += SSTRL(TAB_JESCAPE);
      break;
    case '\n':
      // cppcheck-suppress invalidFunctionArg
      memcpy(o, NEWLINE_JESCAPE, SSTRL(NEWLINE_JESCAPE));
      o += SSTRL(NEWLINE_JESCAPE);
      break;
    default:
      // Escape control characters.
      if ((c < 0x20) || (c == 0x7f)) {
        snprintf(o, 7, "\\u%04x", c);
        o += 6;
      } else {
        *o++ = c;
      }
      break;
    }
  }
  // null terminator
  *o = '\0';
}

// applies hex formatting on construction, restores on destruction
struct hex_formatter {
  std::ostream& out;
  const char old_fill;
  const std::ostream::fmtflags old_flags;

  explicit hex_formatter(std::ostream& out)
    : out(out),
      old_fill(out.fill('0')),
      old_flags(out.setf(out.hex, out.basefield)) { }

  ~hex_formatter() {
    out.fill(old_fill);
    out.flags(old_flags);
  }
};

std::ostream& operator<<(std::ostream& out, const json_stream_escaper& e)
{
  boost::optional<hex_formatter> fmt;

  for (unsigned char c : e.str) {
    switch (c) {
    case '"':
      out << DBL_QUOTE_JESCAPE;
      break;
    case '\\':
      out << BACKSLASH_JESCAPE;
      break;
    case '\t':
      out << TAB_JESCAPE;
      break;
    case '\n':
      out << NEWLINE_JESCAPE;
      break;
    default:
      // Escape control characters.
      if ((c < 0x20) || (c == 0x7f)) {
        if (!fmt) {
          fmt.emplace(out); // enable hex formatting
        }
        out << "\\u" << std::setw(4) << static_cast<unsigned int>(c);
      } else {
        out << c;
      }
      break;
    }
  }
  return out;
}

/*
 * FormatterAttrs(const char *attr, ...)
 *
 * Requires a list of attrs followed by NULL. The attrs should be char *
 * pairs, first one is the name, second one is the value. E.g.,
 *
 * FormatterAttrs("name1", "value1", "name2", "value2", NULL);
 */
FormatterAttrs::FormatterAttrs(const char *attr, ...)
{
  const char *s = attr;
  va_list ap;
  va_start(ap, attr);
  do {
    const char *val = va_arg(ap, char *);
    if (!val)
      break;

    attrs.push_back(make_pair(std::string(s), std::string(val)));
    s = va_arg(ap, char *);
  } while (s);
  va_end(ap);
}

void Formatter::write_bin_data(const char*, int) { }
Formatter::Formatter() { }
Formatter::~Formatter() { }

Formatter *Formatter::create(std::string_view type,
                             std::string_view default_type,
                             std::string_view fallback)
{
  std::string_view mytype(type);
  if (mytype.empty()) {
    mytype = default_type;
  }

  if (mytype == "json")
    return new JSONFormatter(false);
  else if (mytype == "json-pretty")
    return new JSONFormatter(true);
  else if (mytype == "table")
    return new TableFormatter();
  else if (mytype == "table-kv")
    return new TableFormatter(true);
  else if (fallback != "")
    return create(fallback, "", "");
  else
    return (Formatter *) NULL;
}

void Formatter::dump_format(std::string_view name, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  dump_format_va(name, NULL, true, fmt, ap);
  va_end(ap);
}

void Formatter::dump_format_ns(std::string_view name, const char *ns, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  dump_format_va(name, ns, true, fmt, ap);
  va_end(ap);
}

void Formatter::dump_format_unquoted(std::string_view name, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  dump_format_va(name, NULL, false, fmt, ap);
  va_end(ap);
}

/**
 * Json
 */
JSONFormatter::JSONFormatter(bool p) : m_pretty(p), m_is_pending_string(false)
{
  reset();
}

void JSONFormatter::flush(std::ostream& os)
{
  finish_pending_string();
  os << m_ss.str();
  if (m_line_break_enabled)
    os << "\n";
  m_ss.clear();
  m_ss.str("");
}

void JSONFormatter::reset()
{
  m_stack.clear();
  m_ss.clear();
  m_ss.str("");
  m_pending_string.clear();
  m_pending_string.str("");
}

void JSONFormatter::print_comma(json_formatter_stack_entry_d& entry)
{
  if (entry.size) {
    if (m_pretty) {
      m_ss << ",\n";
      for (unsigned i = 1; i < m_stack.size(); i++)
        m_ss << "    ";
    } else {
      m_ss << ",";
    }
  } else if (m_pretty) {
    m_ss << "\n";
    for (unsigned i = 1; i < m_stack.size(); i++)
    m_ss << "    ";
  }
  if (m_pretty && entry.is_array)
    m_ss << "    ";
}

void JSONFormatter::print_quoted_string(std::string_view s)
{
  m_ss << '\"' << json_stream_escaper(s) << '\"';
}

void JSONFormatter::print_name(std::string_view name)
{
  finish_pending_string();
  if (m_stack.empty())
    return;

  struct json_formatter_stack_entry_d& entry = m_stack.back();
  print_comma(entry);
  if (!entry.is_array) {
    if (m_pretty) {
      m_ss << "    ";
    }
    m_ss << "\"" << name << "\"";
    if (m_pretty)
      m_ss << ": ";
    else
      m_ss << ':';
  }
  ++entry.size;
}

void JSONFormatter::open_section(std::string_view name, const char *ns, bool is_array)
{
  if (handle_open_section(name, ns, is_array)) {
    return;
  }
  if (ns) {
    std::ostringstream oss;
    oss << name << " " << ns;
    print_name(oss.str().c_str());
  } else {
    print_name(name);
  }
  if (is_array)
    m_ss << '[';
  else
    m_ss << '{';

  json_formatter_stack_entry_d n;
  n.is_array = is_array;
  m_stack.push_back(n);
}

void JSONFormatter::open_array_section(std::string_view name)
{
  open_section(name, nullptr, true);
}

void JSONFormatter::open_array_section_in_ns(std::string_view name, const char *ns)
{
  open_section(name, ns, true);
}

void JSONFormatter::open_object_section(std::string_view name)
{
  open_section(name, nullptr, false);
}

void JSONFormatter::open_object_section_in_ns(std::string_view name, const char *ns)
{
  open_section(name, ns, false);
}

void JSONFormatter::close_section()
{
  if (handle_close_section()) {
    return;
  }

  common_assert(!m_stack.empty());
  finish_pending_string();

  struct json_formatter_stack_entry_d& entry = m_stack.back();
  if (m_pretty && entry.size) {
    m_ss << "\n";
    for (unsigned i = 1; i < m_stack.size(); i++)
      m_ss << "    ";
  }
  m_ss << (entry.is_array ? ']' : '}');
  m_stack.pop_back();
  if (m_pretty && m_stack.empty())
    m_ss << "\n";
}

void JSONFormatter::finish_pending_string()
{
  if (m_is_pending_string) {
    m_is_pending_string = false;
    add_value(m_pending_name.c_str(), m_pending_string.str(), true);
    m_pending_string.str("");
  }
}

template <class T>
void JSONFormatter::add_value(std::string_view name, T val)
{
  std::stringstream ss;
  ss.precision(std::numeric_limits<T>::max_digits10);
  ss << val;
  add_value(name, ss.str(), false);
}

void JSONFormatter::add_value(std::string_view name, std::string_view val, bool quoted)
{
  if (handle_value(name, val, quoted)) {
    return;
  }
  print_name(name);
  if (!quoted) {
    m_ss << val;
  } else {
    print_quoted_string(val);
  }
}

void JSONFormatter::dump_unsigned(std::string_view name, uint64_t u)
{
  add_value(name, u);
}

void JSONFormatter::dump_int(std::string_view name, int64_t s)
{
  add_value(name, s);
}

void JSONFormatter::dump_float(std::string_view name, double d)
{
  add_value(name, d);
}

void JSONFormatter::dump_string(std::string_view name, std::string_view s)
{
  add_value(name, s, true);
}

std::ostream& JSONFormatter::dump_stream(std::string_view name)
{
  finish_pending_string();
  m_pending_name = name;
  m_is_pending_string = true;
  return m_pending_string;
}

void JSONFormatter::dump_format_va(std::string_view name, const char *ns,
                                   bool quoted, const char *fmt, va_list ap)
{
  char buf[LARGE_SIZE];
  vsnprintf(buf, LARGE_SIZE, fmt, ap);
  add_value(name, buf, quoted);
}

int JSONFormatter::get_len() const
{
  return m_ss.str().size();
}

void JSONFormatter::write_raw_data(const char *data)
{
  m_ss << data;
}

TableFormatter::TableFormatter(bool keyval) : m_keyval(keyval)
{
  reset();
}

void TableFormatter::flush(std::ostream& os)
{
  finish_pending_string();
  std::vector<size_t> column_size = m_column_size;
  std::vector<std::string> column_name = m_column_name;
  std::set<int> need_header_set;

  // auto-sizing columns
  for (size_t i = 0; i < m_vec.size(); i++) {
    for (size_t j = 0; j < m_vec[i].size(); j++) {
      column_size.resize(m_vec[i].size());
      column_name.resize(m_vec[i].size());
      if (i > 0) {
        if (m_vec[i - 1][j] != m_vec[i][j]) {
          // changing row labels require to show the header
          need_header_set.insert(i);
          column_name[i] = m_vec[i][j].first;
        }
      } else {
        column_name[i] = m_vec[i][j].first;
      }

      if (m_vec[i][j].second.length() > column_size[j])
        column_size[j] = m_vec[i][j].second.length();
      if (m_vec[i][j].first.length() > column_size[j])
        column_size[j] = m_vec[i][j].first.length();
    }
  }

  bool need_header = false;
  if ((column_size.size() == m_column_size.size())) {
    for (size_t i = 0; i < column_size.size(); i++) {
      if (column_size[i] != m_column_size[i]) {
        need_header = true;
        break;
      }
    }
  } else {
    need_header = true;
  }

  if (need_header) {
    // first row always needs a header if there wasn't one before
    need_header_set.insert(0);
  }

  m_column_size = column_size;
  for (size_t i = 0; i < m_vec.size(); i++) {
    if (i == 0) {
      if (need_header_set.count(i)) {
        // print the header
        if (!m_keyval) {
          os << "+";
          for (size_t j = 0; j < m_vec[i].size(); j++) {
            for (size_t v = 0; v < m_column_size[j] + 3; v++)
              os << "-";
            os << "+";
          }
          os << "\n";
          os << "|";

          for (size_t j = 0; j < m_vec[i].size(); j++) {
            os << fmt::format(" {:<{}}|",
                  m_vec[i][j].first, m_column_size[j] + 2);
          }
          os << "\n";
          os << "+";
          for (size_t j = 0; j < m_vec[i].size(); j++) {
            for (size_t v = 0; v < m_column_size[j] + 3; v++)
              os << "-";
            os << "+";
          }
          os << "\n";
        }
      }
    }
    // print body
    if (!m_keyval)
      os << "|";
    for (size_t j = 0; j < m_vec[i].size(); j++) {
      if (!m_keyval)
        os << " ";
      if (m_keyval) {
        os << "key::";
        os << m_vec[i][j].first;
        os << "=";
        os << "\"";
        os << m_vec[i][j].second;
        os << "\" ";
      } else {
        os << fmt::format("{:<{}}|", m_vec[i][j].second, m_column_size[j] + 2);
      }
    }

    os << "\n";
    if (!m_keyval) {
      if (i == (m_vec.size() - 1)) {
        // print trailer
        os << "+";
        for (size_t j = 0; j < m_vec[i].size(); j++) {
          for (size_t v = 0; v < m_column_size[j] + 3; v++)
            os << "-";
          os << "+";
        }
        os << "\n";
      }
    }
    m_vec[i].clear();
  }
  m_vec.clear();
}

void TableFormatter::reset()
{
  m_ss.clear();
  m_ss.str("");
  m_section_cnt.clear();
  m_column_size.clear();
  m_section_open = 0;
}

void TableFormatter::open_object_section(std::string_view name)
{
  open_section_in_ns(name, NULL, NULL);
}

void TableFormatter::open_object_section_with_attrs(std::string_view name, const FormatterAttrs& attrs)
{
  open_section_in_ns(name, NULL, NULL);
}

void TableFormatter::open_object_section_in_ns(std::string_view name, const char *ns)
{
  open_section_in_ns(name, NULL, NULL);
}

void TableFormatter::open_array_section(std::string_view name)
{
  open_section_in_ns(name, NULL, NULL);
}

void TableFormatter::open_array_section_with_attrs(std::string_view name, const FormatterAttrs& attrs)
{
  open_section_in_ns(name, NULL, NULL);
}

void TableFormatter::open_array_section_in_ns(std::string_view name, const char *ns)
{
  open_section_in_ns(name, NULL, NULL);
}

void TableFormatter::open_section_in_ns(std::string_view name, const char *ns, const FormatterAttrs *attrs)
{
  m_section.push_back(std::string(name));
  m_section_open++;
}

void TableFormatter::close_section()
{
  m_section_open--;
  if (m_section.size()) {
    m_section_cnt[m_section.back()] = 0;
    m_section.pop_back();
  }
}

size_t TableFormatter::m_vec_index(std::string_view name)
{
  std::string key(name);

  size_t i = m_vec.size();
  if (i)
    i--;

  // make sure there are vectors to push back key/val pairs
  if (!m_vec.size())
    m_vec.resize(1);

  if (m_vec.size()) {
    if (m_vec[i].size()) {
      if (m_vec[i][0].first == key) {
        // start a new column if a key is repeated
        m_vec.resize(m_vec.size() + 1);
        i++;
      }
    }
  }

  return i;
}

std::string TableFormatter::get_section_name(std::string_view name)
{
  std::string t_name{name};
  for (size_t i = 0; i < m_section.size(); i++) {
    t_name.insert(0, ":");
    t_name.insert(0, m_section[i]);
  }
  if (m_section_open) {
    std::stringstream lss;
    lss << t_name;
    lss << "[";
    lss << m_section_cnt[t_name]++;
    lss << "]";
    return lss.str();
  } else {
    return t_name;
  }
}

template <class T>
void TableFormatter::add_value(std::string_view name, T val) {
  finish_pending_string();
  size_t i = m_vec_index(name);
  m_ss.precision(std::numeric_limits<double>::max_digits10);
  m_ss << val;

  m_vec[i].push_back(std::make_pair(get_section_name(name), m_ss.str()));
  m_ss.clear();
  m_ss.str("");
}

void TableFormatter::dump_unsigned(std::string_view name, uint64_t u)
{
  add_value(name, u);
}

void TableFormatter::dump_int(std::string_view name, int64_t s)
{
  add_value(name, s);
}

void TableFormatter::dump_float(std::string_view name, double d)
{
  add_value(name, d);
}

void TableFormatter::dump_string(std::string_view name, std::string_view s)
{
  finish_pending_string();
  size_t i = m_vec_index(name);
  m_ss << s;

  m_vec[i].push_back(std::make_pair(get_section_name(name), m_ss.str()));
  m_ss.clear();
  m_ss.str("");
}

void TableFormatter::dump_string_with_attrs(std::string_view name, std::string_view s, const FormatterAttrs& attrs)
{
  finish_pending_string();
  size_t i = m_vec_index(name);

  std::string attrs_str;
  get_attrs_str(&attrs, attrs_str);
  m_ss << attrs_str << s;

  m_vec[i].push_back(std::make_pair(get_section_name(name), m_ss.str()));
  m_ss.clear();
  m_ss.str("");
}

void TableFormatter::dump_format_va(std::string_view name,
                                    const char *ns, bool quoted,
                                    const char *fmt, va_list ap)
{
  finish_pending_string();
  char buf[LARGE_SIZE];
  vsnprintf(buf, LARGE_SIZE, fmt, ap);

  size_t i = m_vec_index(name);
  if (ns) {
    m_ss << ns << "." << buf;
  } else {
    m_ss << buf;
  }

  m_vec[i].push_back(std::make_pair(get_section_name(name), m_ss.str()));
  m_ss.clear();
  m_ss.str("");
}

std::ostream& TableFormatter::dump_stream(std::string_view name)
{
  finish_pending_string();
  // we don't support this
  m_pending_name = name;
  return m_ss;
}

int TableFormatter::get_len() const
{
  // we don't know the size until flush is called
  return 0;
}

void TableFormatter::write_raw_data(const char *data) {
  // not supported
}

void TableFormatter::get_attrs_str(const FormatterAttrs *attrs, std::string& attrs_str)
{
  std::stringstream attrs_ss;

  for (std::list<std::pair<std::string, std::string> >::const_iterator iter = attrs->attrs.begin();
       iter != attrs->attrs.end(); ++iter) {
    std::pair<std::string, std::string> p = *iter;
    attrs_ss << " " << p.first << "=" << "\"" << p.second << "\"";
  }

  attrs_str = attrs_ss.str();
}

void TableFormatter::finish_pending_string()
{
  if (m_pending_name.length()) {
    std::string ss = m_ss.str();
    m_ss.clear();
    m_ss.str("");
    std::string pending_name = m_pending_name;
    m_pending_name = "";
    dump_string(pending_name.c_str(), ss);
  }
}

