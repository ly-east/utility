#include "Utility/Crypto/Hash.h"
#include "gtest/gtest.h"
#include <string>

namespace {

class UtilityCrypto : public testing::Test {
public:
  static void SetUpTestSuite() {}
};

} // namespace

TEST_F(UtilityCrypto, MD5_EmptyString) {
  EXPECT_TRUE(utility::crypto::getMD5("").empty());
}

TEST_F(UtilityCrypto, MD5_Hello) {
  EXPECT_EQ(utility::crypto::getMD5("hello"),
            "5d41402abc4b2a76b9719d911017c592");
}

TEST_F(UtilityCrypto, MD5_QuickBrownFox) {
  EXPECT_EQ(utility::crypto::getMD5("The quick brown fox jumps over the lazy dog"),
            "9e107d9d372bb6826bd81d3542a419d6");
}

TEST_F(UtilityCrypto, MD5_EmptyAfterGuard) {
  // Empty string returns empty result (guard in getMD5)
  auto result = utility::crypto::getMD5("");
  EXPECT_TRUE(result.empty());
}
