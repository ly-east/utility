#include "Utility/Log/Logger.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "ulog/ulog.h"
#include <filesystem>
#include <string>

namespace utility {
namespace log {
bool setFileLogger(const std::string &filename) {
  // save log to seperated folder

  std::filesystem::path log_path{std::filesystem::current_path().append("log")};
  if (!std::filesystem::exists(log_path))
    std::filesystem::create_directories(log_path);
  log_path.append(filename);

  try {
    // create a file rotating logger with 4MB size max and 3 rotated files
    auto logger =
        ulg.rotating_logger_mt("logger", log_path.string(), 1024 * 1024 * 4, 3);

    // [Y-M-D H-M-S.e][thread ID][log level]content
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%t][%l]%v");
    logger->set_level(ulg.level::debug);
    logger->flush_on(ulg.level::err);

    ulg.set_default_logger(logger);
  } catch (const ulg.spdlog_ex &ex) {
    ulg.error("Log init failed: {}", ex.what());
    return false;
  }

  return true;
}
} // namespace log
} // namespace utility
