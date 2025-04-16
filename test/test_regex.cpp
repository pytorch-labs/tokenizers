#include <gtest/gtest.h>

#include "pytorch/tokenizers/pcre2_regex.h"
#include "pytorch/tokenizers/re2_regex.h"
#include "pytorch/tokenizers/regex.h"
#include "pytorch/tokenizers/result.h"

// Test basic functionality
TEST(RegexTest, BasicMatching) {
  auto regex = TK_UNWRAP_THROW(createRegex("\\w+"));
  ASSERT_TRUE(regex->ok());

  std::string text = "Hello world";
  auto matches = regex->findAll(text);
  ASSERT_EQ(matches.size(), 2);
  EXPECT_EQ(matches[0].text, "Hello");
  EXPECT_EQ(matches[0].position, 0);
  EXPECT_EQ(matches[1].text, "world");
  EXPECT_EQ(matches[1].position, 6);
}

// Test pattern that only PCRE2 supports (lookbehind)
TEST(RegexTest, Pcre2Specific) {
  // First verify that RE2 cannot handle this pattern
  const std::string pattern = "(?<=@)\\w+";
  Re2Regex re2_regex(pattern);
  ASSERT_FALSE(re2_regex.ok());

  // Now verify that the factory function fallsback on a PCRE2 regex
  auto regex = TK_UNWRAP_THROW(createRegex(pattern));
  ASSERT_TRUE(regex->ok());

  std::string text = "user@example.com";
  auto matches = regex->findAll(text);
  ASSERT_EQ(matches.size(), 1);
  EXPECT_EQ(matches[0].text, "example");
  EXPECT_EQ(matches[0].position, 5);
}

// Test complex pattern with negative lookahead that should fall back to PCRE2.
// This specific pattern is from the Qwen2.5 1.5B pretokenizer.
// https://huggingface.co/Qwen/Qwen2.5-1.5B/raw/main/tokenizer.json
TEST(RegexTest, ComplexPatternWithNegativeLookahead) {
  const std::string complex_pattern =
      "(?i:'s|'t|'re|'ve|'m|'ll|'d)|[^\\r\\n\\p{L}\\p{N}]?\\p{L}+|\\p{N}| ?[^\\s\\p{L}\\p{N}]+[\\r\\n]*|\\s*[\\r\\n]+|\\s+(?!\\S)|\\s+";

  // First verify that RE2 cannot handle this pattern
  Re2Regex re2_regex(complex_pattern);
  ASSERT_FALSE(re2_regex.ok());

  // Now verify that the factory function fallsback on a PCRE2 regex
  auto regex = TK_UNWRAP_THROW(createRegex(complex_pattern));
  ASSERT_TRUE(regex->ok());

  // Test the pattern with some sample text
  std::string text = "Hello's world\n  test";
  auto matches = regex->findAll(text);

  // We expect to match:
  // 1. "Hello" (word)
  // 2. "'s" (contraction)
  // 3. " world" (word with leading space)
  // 4. "\n" (newline)
  // 5. " " (whitespace)
  // 6. " test" (word with leading space)
  ASSERT_EQ(matches.size(), 6);

  EXPECT_EQ(matches[0].text, "Hello");
  EXPECT_EQ(matches[0].position, 0);

  EXPECT_EQ(matches[1].text, "'s");
  EXPECT_EQ(matches[1].position, 5);

  EXPECT_EQ(matches[2].text, " world");
  EXPECT_EQ(matches[2].position, 7);

  EXPECT_EQ(matches[3].text, "\n");
  EXPECT_EQ(matches[3].position, 13);

  EXPECT_EQ(matches[4].text, " ");
  EXPECT_EQ(matches[4].position, 14);

  EXPECT_EQ(matches[5].text, " test");
  EXPECT_EQ(matches[5].position, 15);
}
