#include "UtilityString.h"
#include "Utility/String/Encoding.h"
#include "Utility/String/Format.h"
#include "Utility/String/Match.h"
#include "Utility/String/Replace.h"
#include "Utility/String/Trim.h"
#include "ulog/ulog.h"
#include <string>

void UtilityString::SetUpTestSuite() {
  ulog::Logger::setFileLogger("tester.log");
}

// ============================================================================
// Match tests
// ============================================================================

TEST_F(UtilityString, IsMatched_True) {
  EXPECT_TRUE(utility::string::isMatched("progress=  86%", R"((\d+)%)"));
  EXPECT_TRUE(utility::string::isMatched("abc123def", R"(\d+)"));
}

TEST_F(UtilityString, IsMatched_False) {
  EXPECT_FALSE(utility::string::isMatched("hello world", R"((\d+)%)"));
  EXPECT_FALSE(utility::string::isMatched("", R"(\d+)"));
}

TEST_F(UtilityString, GetMatchedString_Found) {
  auto result =
      utility::string::getMatchedString("progress=  86%", R"((\d+)%)");
  EXPECT_EQ(result, "86%");

  result = utility::string::getMatchedString("abc123def", R"(\d+)");
  EXPECT_EQ(result, "123");
}

TEST_F(UtilityString, GetMatchedString_NotFound) {
  auto result = utility::string::getMatchedString("abcdef", R"(\d+)");
  EXPECT_TRUE(result.empty());
}

// ============================================================================
// Trim tests
// ============================================================================

TEST_F(UtilityString, Trim_Basic) {
  EXPECT_EQ(utility::string::trim("  hello  "), "hello");
  EXPECT_EQ(utility::string::trim("\t\nhello\r\n"), "hello");
}

TEST_F(UtilityString, Trim_NoSpace) {
  EXPECT_EQ(utility::string::trim("hello"), "hello");
}

TEST_F(UtilityString, Trim_AllSpaces) {
  EXPECT_TRUE(utility::string::trim("   ").empty());
  EXPECT_TRUE(utility::string::trim("").empty());
}

TEST_F(UtilityString, LeftTrim_Basic) {
  EXPECT_EQ(utility::string::leftTrim("  hello"), "hello");
  EXPECT_EQ(utility::string::leftTrim("hello"), "hello");
  EXPECT_TRUE(utility::string::leftTrim("   ").empty());
}

TEST_F(UtilityString, RightTrim_Basic) {
  EXPECT_EQ(utility::string::rightTrim("hello  "), "hello");
  EXPECT_EQ(utility::string::rightTrim("hello"), "hello");
  EXPECT_TRUE(utility::string::rightTrim("   ").empty());
}

// ============================================================================
// Replace tests
// ============================================================================

TEST_F(UtilityString, ReplaceSpecialChar_String) {
  EXPECT_EQ(utility::string::replaceSpecialChar(std::string("a:b?c<d>e")),
            "abcde");
  EXPECT_EQ(utility::string::replaceSpecialChar(std::string("file/name.txt")),
            "filenametxt");
  EXPECT_EQ(utility::string::replaceSpecialChar(std::string("a\"b|c")), "abc");
}

TEST_F(UtilityString, ReplaceSpecialChar_Wstring) {
  EXPECT_EQ(utility::string::replaceSpecialChar(std::wstring(L"f/g:h")), L"fgh");
  EXPECT_EQ(utility::string::replaceSpecialChar(std::wstring(L"a*b?c")), L"abc");
}

TEST_F(UtilityString, ReplaceSpecialChar_U16string) {
  EXPECT_EQ(utility::string::replaceSpecialChar(std::u16string(u"a\\b:c")),
            u"abc");
}

TEST_F(UtilityString, ReplaceSpecialChar_NoSpecial) {
  EXPECT_EQ(utility::string::replaceSpecialChar(std::string("normal_text")),
            "normal_text");
  EXPECT_EQ(utility::string::replaceSpecialChar(std::wstring(L"normal")),
            L"normal");
}

// ============================================================================
// Format tests
// ============================================================================

TEST_F(UtilityString, BinToHex_Basic) {
  const unsigned char data[] = {0x61, 0x62, 0x63}; // "abc"
  EXPECT_EQ(utility::string::binToHex(data, 3), "616263");
}

TEST_F(UtilityString, BinToHex_Empty) {
  const unsigned char data[] = {0x00, 0xff, 0xab};
  EXPECT_EQ(utility::string::binToHex(data, 3), "00ffab");
}

TEST_F(UtilityString, BinToHex_SingleByte) {
  const unsigned char data[] = {0x00};
  EXPECT_EQ(utility::string::binToHex(data, 1), "00");

  const unsigned char data2[] = {0xff};
  EXPECT_EQ(utility::string::binToHex(data2, 1), "ff");
}

TEST_F(UtilityString, BinToHex_NullData) {
  EXPECT_TRUE(utility::string::binToHex(nullptr, 5).empty());
}

TEST_F(UtilityString, BinToHex_ZeroLen) {
  const unsigned char data[] = {0x61, 0x62};
  EXPECT_TRUE(utility::string::binToHex(data, 0).empty());
}

// ============================================================================
// Encoding tests
// ============================================================================

TEST_F(UtilityString, Utf16_RoundTrip) {
  const std::string original = "Hello, World! 123";
  auto utf16 = utility::string::utf8ToUtf16(original);
  auto result = utility::string::utf16ToUtf8(utf16);
  EXPECT_EQ(original, result);
}

TEST_F(UtilityString, Utf8ToUtf16_NonAscii) {
  // CJK character: U+4E2D (中) = UTF-8 E4 B8 AD
  const std::string utf8 = "\xe4\xb8\xad\xe6\x96\x87"; // "中文"
  auto utf16 = utility::string::utf8ToUtf16(utf8);
  auto result = utility::string::utf16ToUtf8(utf16);
  EXPECT_EQ(utf8, result);
}

TEST_F(UtilityString, Utf8ToUtf16_Empty) {
  auto result = utility::string::utf8ToUtf16("");
  EXPECT_TRUE(result.empty());
}

TEST_F(UtilityString, Utf16ToUtf8_Empty) {
  auto result = utility::string::utf16ToUtf8(u"");
  EXPECT_TRUE(result.empty());
}

TEST_F(UtilityString, Unicode_RoundTrip) {
  const std::string original = "Hello, World!";
  auto wide = utility::string::utf8ToUnicode(original);
  auto result = utility::string::unicodeToUTF8(wide);
  EXPECT_EQ(original, result);
}

TEST_F(UtilityString, AnsiRoundTrip) {
  // Round-trip through ANSI may lose data for non-ANSI characters,
  // but basic ASCII should survive.
  const std::string original = "HelloWorld123";
  auto ansi = utility::string::utf8ToANSI(original);
  auto result = utility::string::ansiToUtf8(ansi);
  EXPECT_EQ(original, result);
}
