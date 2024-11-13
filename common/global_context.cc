#include <filesystem>

#include "stringify.h"
#include "global_definition.h"
#include "code_environment.h"
#include "global_context.h"

using namespace std;

GlobalContext *g_context = nullptr;

void InitGlobalContext(const std::map<std::string, std::string> &args)
{
  /**
   * 初始化日志
   */
  string log_file_dir;
  string log_file_path;

  auto it = args.find(KEY_LOG_PATH);
  if (it == args.end()) {
    auto it = args.find(KEY_LOG_DIR);
    if (it == args.end()) {
      log_file_dir = std::filesystem::current_path();
      log_file_dir += "/" + stringify(DEFAULT_LOG_DIR_SUFFIX);
    } else {
      log_file_dir = it->second;
    }
    log_file_path = log_file_dir + "/" + get_process_name_cpp() + ".log";
  } else {
    log_file_path = it->second;
  }

  map<string, string> cfg = {
    { KEY_LOG_PATH, log_file_path }
  };

  g_context = new GlobalContext(cfg);
}

void GlobalContext::init_logger(const string& log)
{
  logger = spdlog::rotating_logger_mt(DEFAULT_LOGGER_TYPE, log.c_str(), 100 << 20, 10);
  spdlog::set_default_logger(logger);

  logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%l](%@): %v");
  logger->set_level(spdlog::level::info);

  logger->info("log {} initialized", log);
}

GlobalContext::GlobalContext(const std::map<std::string, std::string>& cfg)
{
  string log = cfg.find(KEY_LOG_PATH)->second;
  init_logger(log);
}
