#include "UtilityJson.h"
#include "Utility/Json/AriaRpc.h"
#include "Utility/Json/AriaStatus.h"
#include "Utility/Json/Json.h"
#include "ulog/ulog.h"
#include <string>

void UtilityJson::SetUpTestSuite() { ulog::Logger::setFileLogger("tester.log"); }

// ============================================================================
// parseJsonString tests
// ============================================================================

TEST_F(UtilityJson, ParseValidJson_Object) {
  auto json = utility::json::parseJsonString(R"({"key": "value"})");
  ASSERT_NE(json, nullptr);
  EXPECT_EQ((*json)["key"].get<std::string>(), "value");
}

TEST_F(UtilityJson, ParseValidJson_Array) {
  auto json = utility::json::parseJsonString(R"([1, 2, 3])");
  ASSERT_NE(json, nullptr);
  EXPECT_EQ(json->size(), 3);
}

TEST_F(UtilityJson, ParseValidJson_Number) {
  auto json = utility::json::parseJsonString(R"({"count": 42})");
  ASSERT_NE(json, nullptr);
  EXPECT_EQ((*json)["count"].get<int>(), 42);
}

TEST_F(UtilityJson, ParseValidJson_Boolean) {
  auto json = utility::json::parseJsonString(R"({"flag": true})");
  ASSERT_NE(json, nullptr);
  EXPECT_EQ((*json)["flag"].get<bool>(), true);
}

TEST_F(UtilityJson, ParseValidJson_Null) {
  auto json = utility::json::parseJsonString(R"({"val": null})");
  ASSERT_NE(json, nullptr);
  EXPECT_TRUE((*json)["val"].is_null());
}

TEST_F(UtilityJson, ParseInvalidJson) {
  EXPECT_EQ(utility::json::parseJsonString("not json"), nullptr);
  EXPECT_EQ(utility::json::parseJsonString("{invalid}"), nullptr);
}

TEST_F(UtilityJson, ParseEmptyString) {
  EXPECT_EQ(utility::json::parseJsonString(""), nullptr);
}

// ============================================================================
// getOptionalField tests
// ============================================================================

TEST_F(UtilityJson, GetOptionalField_BothExist) {
  nlohmann::json obj;
  obj["a"] = 1;
  obj["b"] = 2;
  nlohmann::json result;
  EXPECT_TRUE(utility::json::getOptionalField(obj, "a", "b", result));
  // field_r takes precedence when both exist
  EXPECT_EQ(result.get<int>(), 2);
}

TEST_F(UtilityJson, GetOptionalField_OnlyFirstExists) {
  nlohmann::json obj;
  obj["a"] = 42;
  nlohmann::json result;
  EXPECT_TRUE(utility::json::getOptionalField(obj, "a", "not_exist", result));
  EXPECT_EQ(result.get<int>(), 42);
}

TEST_F(UtilityJson, GetOptionalField_OnlySecondExists) {
  nlohmann::json obj;
  obj["b"] = 99;
  nlohmann::json result;
  EXPECT_TRUE(utility::json::getOptionalField(obj, "not_exist", "b", result));
  EXPECT_EQ(result.get<int>(), 99);
}

TEST_F(UtilityJson, GetOptionalField_NoneExist) {
  nlohmann::json obj;
  nlohmann::json result;
  EXPECT_FALSE(utility::json::getOptionalField(obj, "x", "y", result));
}

TEST_F(UtilityJson, GetOptionalField_TwoObjects) {
  nlohmann::json json_l;
  json_l["left"] = "from_left";
  nlohmann::json json_r;
  json_r["right"] = "from_right";
  nlohmann::json result;

  EXPECT_TRUE(
      utility::json::getOptionalField(json_l, "left", json_r, "right", result));
  // field_r takes precedence when both exist
  EXPECT_EQ(result.get<std::string>(), "from_right");

  EXPECT_TRUE(utility::json::getOptionalField(json_l, "missing", json_r,
                                              "right", result));
  EXPECT_EQ(result.get<std::string>(), "from_right");
}

// ============================================================================
// AriaRpc tests
// ============================================================================

TEST_F(UtilityJson, AriaRpc_AddUriNoOption) {
  auto req = utility::aria::getRpcRequest(utility::aria::Method::ADD_URI,
                                          "https://example.com/file");
  EXPECT_FALSE(req.empty());

  auto json = utility::json::parseJsonString(req);
  ASSERT_NE(json, nullptr);
  EXPECT_EQ((*json)["jsonrpc"].get<std::string>(), "2.0");
  EXPECT_EQ((*json)["method"].get<std::string>(), "aria2.addUri");
  EXPECT_EQ((*json)["params"][0][0].get<std::string>(),
            "https://example.com/file");
}

TEST_F(UtilityJson, AriaRpc_AddUriWithOption) {
  auto req = utility::aria::getRpcRequest(
      utility::aria::Method::ADD_URI, "https://example.com",
      {{"dir", "/downloads"}, {"out", "file.mp4"}});
  EXPECT_FALSE(req.empty());

  auto json = utility::json::parseJsonString(req);
  ASSERT_NE(json, nullptr);
  EXPECT_EQ((*json)["params"][1]["dir"].get<std::string>(), "/downloads");
  EXPECT_EQ((*json)["params"][1]["out"].get<std::string>(), "file.mp4");
}

TEST_F(UtilityJson, AriaRpc_Pause) {
  auto req =
      utility::aria::getRpcRequest(utility::aria::Method::PAUSE, "gid123");
  EXPECT_FALSE(req.empty());

  auto json = utility::json::parseJsonString(req);
  ASSERT_NE(json, nullptr);
  EXPECT_EQ((*json)["method"].get<std::string>(), "aria2.pause");
  EXPECT_EQ((*json)["params"][0].get<std::string>(), "gid123");
}

TEST_F(UtilityJson, AriaRpc_Unpause) {
  auto req =
      utility::aria::getRpcRequest(utility::aria::Method::UNPAUSE, "gid456");
  EXPECT_FALSE(req.empty());

  auto json = utility::json::parseJsonString(req);
  ASSERT_NE(json, nullptr);
  EXPECT_EQ((*json)["method"].get<std::string>(), "aria2.unpause");
}

TEST_F(UtilityJson, AriaRpc_Remove) {
  auto req =
      utility::aria::getRpcRequest(utility::aria::Method::REMOVE, "gid789");
  EXPECT_FALSE(req.empty());

  auto json = utility::json::parseJsonString(req);
  ASSERT_NE(json, nullptr);
  EXPECT_EQ((*json)["method"].get<std::string>(), "aria2.remove");
}

TEST_F(UtilityJson, AriaRpc_TellStatus) {
  auto req = utility::aria::getRpcRequest(utility::aria::Method::TELL_STATUS,
                                          "gid100");
  EXPECT_FALSE(req.empty());

  auto json = utility::json::parseJsonString(req);
  ASSERT_NE(json, nullptr);
  EXPECT_EQ((*json)["method"].get<std::string>(), "aria2.tellStatus");
}

TEST_F(UtilityJson, GetRpcResponseGid_Error) {
  // An error response should fail to extract GID
  const std::string error_resp =
      R"({"jsonrpc":"2.0","id":"qwer","error":{"code":1,"message":"Not found"}})";
  std::string gid;
  EXPECT_FALSE(utility::aria::getRpcResponseGid(error_resp, gid));
}

TEST_F(UtilityJson, GetRpcResponseGid_MissingResult) {
  // Missing "result" field entirely should fail
  const std::string resp = R"({"jsonrpc":"2.0","id":"qwer"})";
  std::string gid;
  EXPECT_FALSE(utility::aria::getRpcResponseGid(resp, gid));
}

TEST_F(UtilityJson, GetRpcResponseGid_InvalidJson) {
  std::string gid;
  EXPECT_FALSE(utility::aria::getRpcResponseGid("not valid json", gid));
}
