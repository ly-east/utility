#include "Utility/Json.h"
#include "spdlog/spdlog.h"
#include <cassert>
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

bool getOptionalField(const nlohmann::json &json_obj,
                      const std::string &field_l, const std::string &field_r,
                      nlohmann::json &result) {
  return getOptionalField(json_obj, field_l, json_obj, field_r, result);
}

bool getOptionalField(const nlohmann::json &json_l, const std::string &field_l,
                      const nlohmann::json &json_r, const std::string &field_r,
                      nlohmann::json &result) {
  assert(!field_l.empty() || !field_r.empty());

  nlohmann::json l, r;

  auto optionalFieldJson = [](const nlohmann::json &j,
                              const std::string &f) -> nlohmann::json {
    if (!f.empty() && !j.is_null() && j.contains(f))
      return j[f];
    return nlohmann::json();
  };

  l = optionalFieldJson(json_l, field_l);
  r = optionalFieldJson(json_r, field_r);

  if (l.is_null() && r.is_null()) {
    spdlog::error("getOptionalField: neither {} nor {} are existed", field_l,
                  field_r);
    return false;
  }

  result = r.is_null() ? std::move(l) : std::move(r);
  return true;
}
} // namespace utility
