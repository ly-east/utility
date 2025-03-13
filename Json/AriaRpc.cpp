#include "Utility/Json/AriaRpc.h"
#include "Type/AriaStatus.h"
#include "Utility/Json/Json.h"
#include "nlohmann/json.hpp"
#include "ulog/ulog.h"
#include <cstdint>
#include <exception>
#include <functional>
#include <unordered_map>

namespace {
using namespace utility::aria;
using RpcRespRecvTy = std::function<bool(const nlohmann::json &)>;

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

bool parseRpcResponse(const std::string &response, RpcRespRecvTy func) {
  auto resp = utility::json::parseJsonString(response);
  if (!resp)
    return false;

  nlohmann::json result;
  const std::string error{"error"};
  if (!utility::json::getOptionalField(*resp, "result", error, result)) {
    ulg.error("parseRpcResponse: getOptionalField failed");
    return false;
  }

  if (resp->contains(error)) {
    int code = 0;
    std::string message;

    try {
      code = result["code"].template get<decltype(code)>();
      message = result["message"].template get<decltype(message)>();
    } catch (const std::exception &e) {
      ulg.error("parseRpcResponse: {}", e.what());
    }

    ulg.error("Aria2 returns error({}): {}", code, message);
    return false;
  }

  return func(result);
}
} // namespace

namespace utility {
namespace aria {
std::string getRpcRequest(Method method, const std::string &param,
                          const RpcArgTy &option) {
  using nlohmann::json;

  json j = {{getKeyString(Key::JSONRPC), getValueString(Value::JSONRPC)},
            {getKeyString(Key::ID), getValueString(Value::ID)},
            {getKeyString(Key::METHOD), getMethodString(method)}};

  if (Method::ADD_URI == method)
    j[getKeyString(Key::PARAMS)] = {{param}};
  else
    j[getKeyString(Key::PARAMS)] = {param};

  if (option.size()) {
    json opt;

    for (const auto &pair : option)
      opt[pair.first] = pair.second;

    j[getKeyString(Key::PARAMS)].emplace_back(std::move(opt));
  }

  return j.dump();
}

bool getRpcResponseGid(const std::string &response, std::string &gid) {
  auto extractor = [&gid](const nlohmann::json &result) -> bool {
    try {
      gid = result.template get<std::string>();
    } catch (const std::exception &e) {
      ulg.error("getRpcResponseGid failed: {}", e.what());
      return false;
    }

    return true;
  };

  return parseRpcResponse(response, extractor);
}

bool getRpcResponseStatus(const std::string &response,
                          bbdown::StatusResult &status) {
  auto extractor = [&status](const nlohmann::json &result) -> bool {
    try {
      uint64_t current =
          std::stoull(result["completedLength"].get<std::string>());
      uint64_t total = std::stoull(result["totalLength"].get<std::string>());
      status.progress = (double)current / (double)total;

      status.speed =
          (double)std::stoull(result["downloadSpeed"].get<std::string>()) /
          1024;
      status.path = result["files"].front()["path"].get<std::string>();

      std::string task_status{result["status"].get<std::string>()};
      if ("waiting" == task_status)
        status.status = bbdown::TaskStatus::WAITING;
      else if ("active" == task_status)
        status.status = bbdown::TaskStatus::ACTIVE;
      else if ("complete" == task_status)
        status.status = bbdown::TaskStatus::COMPLETE;
      else if ("paused" == task_status)
        status.status = bbdown::TaskStatus::PAUSED;
      else if ("error" == task_status)
        status.status = bbdown::TaskStatus::FAILED;
      else {
        ulg.warn("unknown task status {}", task_status);
        status.status = bbdown::TaskStatus::UNKNOWN;
      }

    } catch (const std::exception &e) {
      ulg.error("getRpcResponseStatus failed: {}", e.what());
      return false;
    }

    return true;
  };

  return parseRpcResponse(response, extractor);
}
} // namespace aria
} // namespace utility
