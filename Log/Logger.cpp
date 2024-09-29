#include "Utility/Log/Logger.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

namespace utility {
namespace log {
bool setFileLogger(const std::string &filename) {
  static bool has_settled = false;

  if (has_settled)
    return true;

  try {
    auto logger = spdlog::basic_logger_mt("basic_logger", filename);

    // [Y-M-D H-M-S.e][thread ID][log level]content
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%t][%l]%v");
    logger->set_level(spdlog::level::debug);
    logger->flush_on(spdlog::level::err);

    spdlog::set_default_logger(logger);
    has_settled = true;
  } catch (const spdlog::spdlog_ex &ex) {
    spdlog::error("Log init failed: {}", ex.what());
    return false;
  }

  return true;
}
} // namespace log
} // namespace utility
