#include "UtilityMath.h"
#include "Utility/Math/CorrectValue.h"
#include "Utility/Math/Random.h"
#include "ulog/ulog.h"
#include <cmath>
#include <limits>
#include <utility>
#include <vector>

void UtilityMath::SetUpTestSuite() {
  ulog::Logger::setFileLogger("tester.log");
}

// ============================================================================
// Random tests
// ============================================================================

TEST_F(UtilityMath, Random_Bounds) {
  std::vector<std::pair<unsigned, unsigned>> cases{
      {0, 8}, {1, 34567}, {9345, 104398}};

  for (const auto &pair : cases) {
    unsigned tries = 32;
    while (tries--) {
      auto result = utility::math::getRandom(pair.second, pair.first);
      EXPECT_TRUE(pair.first <= result && result < pair.second);
    }
  }
}

TEST_F(UtilityMath, Random_AdjacentBounds) {
  // When upper_bound = lower_bound + 1, there's only one possible value
  for (int i = 0; i < 10; ++i) {
    auto result = utility::math::getRandom(5u, 4u);
    EXPECT_EQ(result, 4u);
  }
}

TEST_F(UtilityMath, Random_DefaultLowerBound) {
  auto result = utility::math::getRandom(100u);
  EXPECT_LT(result, 100u);
}

// ============================================================================
// CorrectValue tests
// ============================================================================

TEST_F(UtilityMath, IsEqual_SameValue) {
  EXPECT_TRUE(utility::math::isEqual(1.0, 1.0));
  EXPECT_TRUE(utility::math::isEqual(0.0, 0.0));
  EXPECT_TRUE(utility::math::isEqual(-3.14, -3.14));
}

TEST_F(UtilityMath, IsEqual_WithinEpsilon) {
  EXPECT_TRUE(utility::math::isEqual(1.0, 1.0 + 1e-13));
  EXPECT_TRUE(utility::math::isEqual(1.0, 1.0 - 1e-13));
}

TEST_F(UtilityMath, IsEqual_Different) {
  EXPECT_FALSE(utility::math::isEqual(1.0, 2.0));
  EXPECT_FALSE(utility::math::isEqual(0.0, 1e-10));
}

TEST_F(UtilityMath, IsEqual_CustomEpsilon) {
  EXPECT_TRUE(utility::math::isEqual(1.0, 1.5, 1.0));
  EXPECT_FALSE(utility::math::isEqual(1.0, 1.5, 0.1));
}

TEST_F(UtilityMath, IsEqual_LargeNumbers) {
  // With large numbers, a small relative difference is still "equal"
  EXPECT_TRUE(utility::math::isEqual(1e9, 1e9 + 1e-3, 1.0));
  // Use a tight epsilon to distinguish close large values
  EXPECT_TRUE(utility::math::isEqual(1e9, 1e9 + 100.0, 1.0));
  EXPECT_FALSE(utility::math::isEqual(1e9, 1e9 + 100.0, 1e-8));
}

TEST_F(UtilityMath, IsValid_Finite) {
  EXPECT_TRUE(utility::math::isValid(0.0));
  EXPECT_TRUE(utility::math::isValid(42.0));
  EXPECT_TRUE(utility::math::isValid(-1.5));
  EXPECT_TRUE(utility::math::isValid(std::numeric_limits<double>::max()));
  EXPECT_TRUE(utility::math::isValid(std::numeric_limits<double>::min()));
}

TEST_F(UtilityMath, IsValid_Inf) {
  EXPECT_FALSE(
      utility::math::isValid(std::numeric_limits<double>::infinity()));
  EXPECT_FALSE(
      utility::math::isValid(-std::numeric_limits<double>::infinity()));
}

TEST_F(UtilityMath, IsValid_NaN) {
  EXPECT_FALSE(
      utility::math::isValid(std::numeric_limits<double>::quiet_NaN()));
  EXPECT_FALSE(
      utility::math::isValid(std::numeric_limits<double>::signaling_NaN()));
}
