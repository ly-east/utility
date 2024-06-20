#include "Utility/Json/AriaRpc.h"
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

const char *getKeyString(Key key) {
  static const std::unordered_map<Key, const char *> rpc_key{
      {Key::JSONRPC, "jsonrpc"},
      {Key::ID, "id"},
      {Key::METHOD, "method"},
      {Key::PARAMS, "params"}};

  return rpc_key.at(key);
}

const char *getValueString(Value value) {
  static const std::unordered_map<Value, const char *> rpc_value{
      {Value::JSONRPC, "2.0"}, {Value::ID, "qwer"}};

  return rpc_value.at(value);
}
} // namespace

namespace utility {
namespace aria {

bool ariaRpc(Method method, const std::string &param, const RpcArgTy &option) {
  using nlohmann::json;

  json j = {{getKeyString(Key::JSONRPC), getValueString(Value::JSONRPC)},
            {getKeyString(Key::ID), getValueString(Value::ID)},
            {getKeyString(Key::METHOD), getMethodString(method)},
            {getKeyString(Key::PARAMS), {{param}}}};

  if (option.size()) {
    json opt;

    for (const auto &pair : option)
      opt[pair.first] = pair.second;

    j[getKeyString(Key::PARAMS)].emplace_back(std::move(opt));
  }

  spdlog::debug("\n{}", j.dump());

  return true;
}
} // namespace aria
} // namespace utility
