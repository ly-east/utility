#include "Utility/AriaRpc.h"
#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"
#include <unordered_map>

namespace {
using namespace utility::aria;

const char *getMethodString(Method method) {
  static const std::unordered_map<Method, const char *> rpc_method{
      {Method::ADD_URI, "aria2.addUri"},
      {Method::PAUSE, "aria2.pause"},
      {Method::UNPAUSE, "aria2.unpause"},
      {Method::REMOVE, "aria2.remove"},
      {Method::TELL_STATUS, "aria2.tellStatus"}};

  return rpc_method.at(method);
}
} // namespace

namespace utility {
namespace aria {

bool ariaRpc(Method method) {

  using nlohmann::json;

  json j = {
      {"jsonrpc", "2.0"},
      {"id", "qwer"},
      {"method", getMethodString(method)},
      {"params",
       {{"https://baidu.com"}, {{"dir", "D:\\download"}, {"out", "a.exe"}}}}};

  spdlog::debug("\n{}", j.dump());

  return true;
}
} // namespace aria
} // namespace utility
