#ifndef GLOBAL_DEFINITION_H
#define GLOBAL_DEFINITION_H

#define LARGE_SIZE 1024

typedef unsigned uint128_t __attribute__ ((mode (TI)));

/**
 * 参数 key
 */
#define KEY_LOG_DIR   "log_dir"
#define KEY_LOG_PATH  "log_path"

/**
 * 默认配置
 */
#define DEFAULT_LOGGER_TYPE       "baseLogger"
#define DEFAULT_LOG_DIR           "/var/log"
#define DEFAULT_LOG_DIR_SUFFIX    "out"

#endif // GLOBAL_DEFINITION_H
