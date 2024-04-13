#include "Utility/Json.h"
#include "spdlog/spdlog.h"
#include <exception>
#include <filesystem>
#include <fstream>

namespace utility {
JsonPtrTy loadJsonFile(const std::string &path) {
  std::filesystem::path p{path};

  if (!std::filesystem::exists(p)) {
    spdlog::error("loadJsonFile: invalid path {}", path);
    return nullptr;
  }

  std::ifstream json_file{path};

  try {
    JsonPtrTy json =
        std::make_unique<nlohmann::json>(nlohmann::json::parse(json_file));
    return json;
  } catch (const std::exception &e) {
    spdlog::error("loadJsonFile: {}", e.what());
    return nullptr;
  }
}

JsonPtrTy parseJsonString(const std::string &json_str) {
  if (json_str.empty() || json_str.size() < 2) {
    spdlog::error("parseJsonString: json_str is empty or too short");
    return nullptr;
  }

  try {
    JsonPtrTy json =
        std::make_unique<nlohmann::json>(nlohmann::json::parse(json_str));
    return std::move(json);
  } catch (const std::exception &e) {
    spdlog::error("parseJsonString: {}", e.what());
    return nullptr;
  }
}
} // namespace utility
