#include <gtest/gtest.h>

#include "pytorch/tokenizers/pcre2_regex.h"
#include "pytorch/tokenizers/regex.h"

TEST(Pcre2RegexTest, BasicMatching) {
  Pcre2Regex regex("(\\w+)");
  std::vector<Match> matches = regex.findAll("Hello world");
  
  ASSERT_EQ(matches.size(), 2);
  EXPECT_EQ(matches[0].text, "Hello");
  EXPECT_EQ(matches[0].position, 0);
  EXPECT_EQ(matches[1].text, "world");
  EXPECT_EQ(matches[1].position, 6);
}

TEST(Pcre2RegexTest, ComplexPatterns) {
  // Test with a more complex pattern that includes lookaheads
  Pcre2Regex regex("(?<=@)(\\w+)");
  std::vector<Match> matches = regex.findAll("user@example.com");
  
  ASSERT_EQ(matches.size(), 1);
  EXPECT_EQ(matches[0].text, "example");
  EXPECT_EQ(matches[0].position, 5);
}

TEST(Pcre2RegexTest, UnicodeSupport) {
  // Test with Unicode characters
  Pcre2Regex regex("(\\p{L}+)");
  std::vector<Match> matches = regex.findAll("Hello 世界");
  
  ASSERT_EQ(matches.size(), 2);
  EXPECT_EQ(matches[0].text, "Hello");
  EXPECT_EQ(matches[0].position, 0);
  EXPECT_EQ(matches[1].text, "世界");
  EXPECT_EQ(matches[1].position, 6);
}

TEST(Pcre2RegexTest, InvalidPattern) {
  // Test with an invalid pattern
  Pcre2Regex regex("(");  // Unmatched parenthesis
  EXPECT_FALSE(regex.ok());
  
  std::vector<Match> matches = regex.findAll("test");
  EXPECT_TRUE(matches.empty());
}

TEST(Pcre2RegexTest, EmptyMatches) {
  // Test with a pattern that can match empty strings
  Pcre2Regex regex("(a*)");
  std::vector<Match> matches = regex.findAll("b");
  
  // Should find one empty match at the beginning
  ASSERT_EQ(matches.size(), 1);
  EXPECT_EQ(matches[0].text, "");
  EXPECT_EQ(matches[0].position, 0);
}

TEST(Pcre2RegexTest, FactoryFunction) {
  // Test the factory function with a pattern that RE2 doesn't support
  auto regex = createRegex("(?<=@)(\\w+)");
  ASSERT_NE(regex, nullptr);
  
  std::vector<Match> matches = regex->findAll("user@example.com");
  ASSERT_EQ(matches.size(), 1);
  EXPECT_EQ(matches[0].text, "example");
  EXPECT_EQ(matches[0].position, 5);
} 