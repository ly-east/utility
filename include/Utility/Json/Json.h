#ifndef UTILITY_JSON_JSON_H
#define UTILITY_JSON_JSON_H

#include "Utility/DllExport.h"
#include "nlohmann/json.hpp"
#include <filesystem>
#include <memory>
#include <string>

namespace utility {
namespace json {

using JsonPtrTy = std::unique_ptr<nlohmann::json>;

// Load json from file
// Returns nullptr if any error occurred
DllExport JsonPtrTy loadJsonFile(const std::filesystem::path &path);

// Parse json string into json object
// Returns nullptr if any error occurred
DllExport JsonPtrTy parseJsonString(const std::string &json_str);

// Select the existing field of given json object. This function is useful when
// two fields MAY appear, and one of them must exist.
// @return false if all of them don't exist.
// @warning param `field_l` and `field_r` cannot be empty strings at the same
// time
DllExport bool getOptionalField(const nlohmann::json &json_obj,
                                const std::string &field_l,
                                const std::string &field_r,
                                nlohmann::json &result);

// Select the existing field of given json objects. This function is useful when
// two fields MAY appear, and one of them must exist.
// @return false if all of them don't exist.
// @warning fields cannot be empty strings at the same time, so do json
// objects(cannot be null at the same time).
DllExport bool getOptionalField(const nlohmann::json &json_l,
                                const std::string &field_l,
                                const nlohmann::json &json_r,
                                const std::string &field_r,
                                nlohmann::json &result);
} // namespace json
} // namespace utility

#endif // UTILITY_JSON_JSON_H
