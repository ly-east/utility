#include "Utility/Log/Logger.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/spdlog.h"
#include <filesystem>
#include <string>

namespace utility {
namespace log {
bool setFileLogger(const std::string &filename) {
  std::filesystem::path log_path{std::filesystem::current_path().append("log")};
  if (!std::filesystem::exists(log_path))
    std::filesystem::create_directories(log_path);
  log_path.append(filename);

  try {
    // create a file rotating logger with 4MB size max and 3 rotated files
    auto logger = spdlog::rotating_logger_mt("logger", log_path.string(),
                                             1024 * 1024 * 4, 3);

    // [Y-M-D H-M-S.e][thread ID][log level]content
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%t][%l]%v");
    logger->set_level(spdlog::level::debug);
    logger->flush_on(spdlog::level::err);

    spdlog::set_default_logger(logger);
  } catch (const spdlog::spdlog_ex &ex) {
    spdlog::error("Log init failed: {}", ex.what());
    return false;
  }

  return true;
}
} // namespace log
} // namespace utility
