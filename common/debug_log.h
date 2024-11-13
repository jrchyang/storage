#ifndef DEBUG_LOG_H
#define DEBUG_LOG_H

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include "global_definition.h"
#include "global_context.h"

/**
 * 默认的日志输出前缀，各个模块可通过如下方式定义自己的前缀
 *   #undef log_prefix
 *   #define log_prefix "xxx"
 */
#define log_prefix "default"

#define ldebug_logger(logger, fmt, ...)                                       \
  do {                                                                        \
    logger->debug("{}: " fmt, log_prefix, ##__VA_ARGS__);                     \
  } while (0)

#define ldebug(fmt, ...)                                                      \
  do {                                                                        \
    spdlog::debug("{}: " fmt, log_prefix, ##__VA_ARGS__);                     \
  } while (0)

#define linfo_logger(logger, fmt, ...)                                        \
  do {                                                                        \
    logger->info("{}: " fmt, log_prefix, ##__VA_ARGS__);                      \
  } while (0)

#define linfo(fmt, ...)                                                       \
  do {                                                                        \
    spdlog::info("{}: " fmt, log_prefix, ##__VA_ARGS__);                      \
  } while (0)

#define lwarn_logger(logger, fmt, ...)                                        \
  do {                                                                        \
    logger->warn("{}: " fmt, log_prefix, ##__VA_ARGS__);                      \
  } while (0)

#define lwarn(fmt, ...)                                                       \
  do {                                                                        \
    spdlog::warn("{}: " fmt, log_prefix, ##__VA_ARGS__);                      \
  } while (0)

#define lerr_logger(logger, fmt, ...)                                         \
  do {                                                                        \
    logger->error("{}: " fmt, log_prefix, ##__VA_ARGS__);                     \
  } while (0)

#define lerr(fmt, ...)                                                        \
  do {                                                                        \
    spdlog::error("{}: " fmt, log_prefix, ##__VA_ARGS__);                     \
  } while (0)

#define lcritical_logger(logger, fmt, ...)                                    \
  do {                                                                        \
    logger->critical("{}: " fmt, log_prefix, ##__VA_ARGS__);                  \
  } while (0)

#define lcritical(fmt, ...)                                                   \
  do {                                                                        \
    spdlog::critical("{}: " fmt, log_prefix, ##__VA_ARGS__);                  \
  } while (0)

#endif // DEBUG_LOG_H
