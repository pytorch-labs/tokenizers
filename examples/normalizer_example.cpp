/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */
// @lint-ignore-every LICENSELINT

#include <pytorch/tokenizers/normalizer.h>
#include <iostream>

using namespace tokenizers;

int main() {
  // Example 1: Using the config builder pattern
  std::cout << "=== Example 1: Config Builder Pattern ===" << std::endl;
  auto normalizer1 =
      NormalizerConfig("Replace").set_pattern(" ").set_content("▁").create();

  std::string input1 = "Hello World Test";
  std::string result1 = normalizer1->normalize(input1);
  std::cout << "Input:  \"" << input1 << "\"" << std::endl;
  std::cout << "Output: \"" << result1 << "\"" << std::endl;
  std::cout << std::endl;

  // Example 2: Using JSON config (like tokenizer.json)
  std::cout << "=== Example 2: JSON Config ===" << std::endl;
  nlohmann::json config = {
      {"type", "Replace"}, {"pattern", {{"String", " "}}}, {"content", "▁"}};

  NormalizerConfig norm_config;
  norm_config.parse_json(config);
  auto normalizer2 = norm_config.create();

  std::string input2 = "This is a test sentence";
  std::string result2 = normalizer2->normalize(input2);
  std::cout << "Input:  \"" << input2 << "\"" << std::endl;
  std::cout << "Output: \"" << result2 << "\"" << std::endl;
  std::cout << std::endl;

  // Example 3: Using regex pattern
  std::cout << "=== Example 3: Regex Pattern ===" << std::endl;
  nlohmann::json regex_config = {
      {"type", "Replace"}, {"pattern", {{"Regex", "\\s+"}}}, {"content", "_"}};

  NormalizerConfig regex_norm_config;
  regex_norm_config.parse_json(regex_config);
  auto normalizer3 = regex_norm_config.create();

  std::string input3 = "Hello   World\t\tTest";
  std::string result3 = normalizer3->normalize(input3);
  std::cout << "Input:  \"" << input3 << "\"" << std::endl;
  std::cout << "Output: \"" << result3 << "\"" << std::endl;
  std::cout << std::endl;

  // Example 4: Sequence of normalizers
  std::cout << "=== Example 4: Sequence Normalizer ===" << std::endl;
  nlohmann::json seq_config = {
      {"type", "Sequence"},
      {"normalizers",
       {{{"type", "Replace"}, {"pattern", {{"String", " "}}}, {"content", "▁"}},
        {{"type", "Replace"},
         {"pattern", {{"String", "a"}}},
         {"content", "X"}}}}};

  NormalizerConfig seq_norm_config;
  seq_norm_config.parse_json(seq_config);
  auto normalizer4 = seq_norm_config.create();

  std::string input4 = "banana split";
  std::string result4 = normalizer4->normalize(input4);
  std::cout << "Input:  \"" << input4 << "\"" << std::endl;
  std::cout << "Output: \"" << result4 << "\"" << std::endl;
  std::cout << std::endl;

  // Example 5: Direct instantiation
  std::cout << "=== Example 5: Direct Instantiation ===" << std::endl;
  ReplaceNormalizer direct_normalizer("o", "0");

  std::string input5 = "Hello World";
  std::string result5 = direct_normalizer.normalize(input5);
  std::cout << "Input:  \"" << input5 << "\"" << std::endl;
  std::cout << "Output: \"" << result5 << "\"" << std::endl;

  return 0;
}
