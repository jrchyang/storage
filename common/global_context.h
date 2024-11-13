#ifndef GLOBAL_CONTEXT_H
#define GLOBAL_CONTEXT_H

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include <map>
#include <boost/noncopyable.hpp>

class GlobalContext;

extern GlobalContext *g_context;

class GlobalContext : private boost::noncopyable {
public:
  explicit GlobalContext(const std::map<std::string, std::string>& cfg);
  ~GlobalContext();
private:
  void init_logger(const std::string& log_path);
public:
  std::shared_ptr<spdlog::logger> logger = nullptr;
};

void InitGlobalContext(const std::map<std::string, std::string>& args);

#endif // GLOBAL_CONTEXT_H
